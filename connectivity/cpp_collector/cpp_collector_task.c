/**
 ****************************************************************************************
 *
 * @file cpp_collector_task.c
 *
 * @brief CPP Collector task
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "osal.h"
#include "cli.h"
#include "ble_bufops.h"
#include "ble_client.h"
#include "ble_storage.h"
#include "ble_uuid.h"
#include "cps_client.h"
#include "gatt_client.h"
#include "sdk_queue.h"

/**
 * CLI notify mask
 */
#define CLI_NOTIF               (1 << 1)

/**
 * CPS timeout notify mask
 */
#define CPS_OPERATION_TMO_NOTIF (1 << 2)

/**
 * Max number of found devices
 */
#define MAX_FOUND_DEVICES       25

/**
 * Macro checking if address is resolvable
 */
#define ADDR_RESOLVABLE(bdaddr) (bdaddr.addr_type == PRIVATE_ADDRESS && \
                                (bdaddr.addr[5] & 0xc0) == 0x40)

/**
 * BLE clients storage keys
 */
#define GATT_CLIENT_STORAGE_ID  BLE_STORAGE_KEY_APP(0, 0)
#define CPS_CLIENT_STORAGE_ID   BLE_STORAGE_KEY_APP(0, 1)

typedef enum {
        APP_STATE_IDLE,
        APP_STATE_CONNECTING,
        APP_STATE_SCANNING,
} app_state_t;

typedef enum {
        AUTH_TYPE_ENCRYPT,
        AUTH_TYPE_PAIR,
        AUTH_TYPE_BOND,
} auth_type_t;

typedef enum {
        PENDING_ACTION_ENABLE_SVC_CHANGED_IND = (1 << 0),
        PENDING_ACTION_ENABLE_MEASUREMENT_NOTIF = (1 << 1),
        PENDING_ACTION_ENABLE_POWER_VECTOR_NOTIF = (1 << 2),
        PENDING_ACTION_ENABLE_CTRL_POINT_IND = (1 << 3),
        PENDING_ACTION_READ_FEATURES = (1 << 4),
        PENDING_ACTION_READ_SENSOR_LOCATION = (1 << 5),
} pending_action_t;

/* Application state */
__RETAINED static app_state_t app_state;

typedef struct {
        bd_address_t addr;
        bool name_found;
} found_device_t;

/* Scanning state */
__RETAINED static struct {
        bool match_any;
        found_device_t devices[MAX_FOUND_DEVICES];
        size_t num_devices;
} scan_state;

typedef struct {
        void *next;
        uint16_t start_h;
        uint16_t end_h;
} browse_req_t;

typedef struct {
        void *next;

        bd_address_t addr;
        uint16_t conn_idx;

        bool busy_auth;

        bool browsing;
        queue_t pending_browse_queue;

        bool busy_init;
        pending_action_t pending_init;

        ble_client_t *gatt_client;
        ble_client_t *cps_client;

        OS_TIMER cps_tmo_timer;
} peer_info_t;

/* Current peers information */
__RETAINED static queue_t peer_info_queue;

/* Queue of connection indexes of peer where CPS timeout was triggered */
__RETAINED static OS_QUEUE peer_cps_tmo_queue;

/* Application task */
__RETAINED static OS_TASK app_task;

static void timer_cb(OS_TIMER timer)
{
        peer_info_t *peer_info = OS_TIMER_GET_TIMER_ID(timer);

        OS_QUEUE_PUT(peer_cps_tmo_queue, &peer_info->conn_idx, OS_QUEUE_FOREVER);
        OS_TASK_NOTIFY(app_task, CPS_OPERATION_TMO_NOTIF, OS_NOTIFY_SET_BITS);
}

static bool peer_conn_idx_match(const void *data, const void *match_data)
{
        const peer_info_t *peer_info = (peer_info_t *) data;
        const uint16_t conn_idx = *(uint16_t *) match_data;

        return peer_info->conn_idx == conn_idx;
}

static inline void add_peer_info(peer_info_t *peer_info)
{
        queue_push_front(&peer_info_queue, peer_info);
}

static inline peer_info_t *remove_peer_info(uint16_t conn_idx)
{
        return queue_remove(&peer_info_queue, peer_conn_idx_match, &conn_idx);
}

static inline peer_info_t *find_peer_info(uint16_t conn_idx)
{
        return queue_find(&peer_info_queue, peer_conn_idx_match, &conn_idx);
}

#define pending_init_execute_and_check(PEER_INFO, FLAG, FUNCTION, ...)          \
        ({                                                                      \
                if (PEER_INFO->pending_init & FLAG) {                           \
                        PEER_INFO->busy_init = FUNCTION(__VA_ARGS__);           \
                        if (!PEER_INFO->busy_init) {                            \
                                /* Failed to execute action, clear bit */       \
                                PEER_INFO->pending_init &= ~FLAG;               \
                        }                                                       \
                }                                                               \
                PEER_INFO->busy_init;                                           \
        })

static void process_pending_actions(peer_info_t *peer_info)
{
        if (peer_info->busy_init) {
                return;
        }

        if (pending_init_execute_and_check(peer_info, PENDING_ACTION_ENABLE_SVC_CHANGED_IND,
                                                gatt_client_set_event_state, peer_info->gatt_client,
                                                GATT_CLIENT_EVENT_SERVICE_CHANGED_INDICATE, true)) {
                return;
        }

        if (pending_init_execute_and_check(peer_info, PENDING_ACTION_ENABLE_MEASUREMENT_NOTIF,
                                                cps_client_set_event_state, peer_info->cps_client,
                                                CPS_CLIENT_EVENT_MEASUREMENT_NOTIF, true)) {
                return;
        }

        if (pending_init_execute_and_check(peer_info, PENDING_ACTION_ENABLE_POWER_VECTOR_NOTIF,
                                                cps_client_set_event_state, peer_info->cps_client,
                                                CPS_CLIENT_EVENT_CP_VECTOR_NOTIF, true)) {
                return;
        }

        if (pending_init_execute_and_check(peer_info, PENDING_ACTION_ENABLE_CTRL_POINT_IND,
                                                cps_client_set_cycling_power_cp_state,
                                                peer_info->cps_client, true)) {
                return;
        }

        if (pending_init_execute_and_check(peer_info, PENDING_ACTION_READ_FEATURES,
                                                        cps_client_read_cycling_power_feature,
                                                        peer_info->cps_client)) {
                return;
        }

        if (pending_init_execute_and_check(peer_info, PENDING_ACTION_READ_SENSOR_LOCATION,
                                                        cps_client_read_sensor_location,
                                                        peer_info->cps_client)) {
                return;
        }

        printf("Ready.\r\n");
}

static void add_pending_action(peer_info_t *peer_info, pending_action_t action)
{
        peer_info->pending_init |= action;

        process_pending_actions(peer_info);
}

static bool start_auth(peer_info_t *peer_info, auth_type_t auth_type, bool mitm);

static void clear_pending_action(peer_info_t *peer_info, pending_action_t action, att_error_t error)
{
        /* Do nothing if we try to clear action which is not pending */
        if ((peer_info->pending_init & action) == 0) {
                return;
        }

        /* Try to authenticate if action failed due to insufficient authentication/encryption */
        if ((error == ATT_ERROR_INSUFFICIENT_AUTHENTICATION) ||
                                                (error == ATT_ERROR_INSUFFICIENT_ENCRYPTION)) {
                peer_info->busy_init = false;
                start_auth(peer_info, AUTH_TYPE_PAIR, false);
                return;
        }

        peer_info->busy_init = false;
        peer_info->pending_init &= ~action;

        process_pending_actions(peer_info);
}

static void store_client(uint16_t conn_idx, ble_client_t *client, ble_storage_key_t key)
{
        uint8_t *buffer = NULL;
        size_t length;

        if (!client) {
                return;
        }

        /* Get serialized BLE Client length */
        ble_client_serialize(client, NULL, &length);
        buffer = OS_MALLOC(length);
        /* Serialize BLE Client */
        ble_client_serialize(client, buffer, &length);
        /* Put BLE Client to the storage */
        ble_storage_put_buffer(conn_idx, key, length, buffer, OS_FREE_FUNC, true);
}

static void purge_gatt(peer_info_t *peer_info)
{
        ble_storage_remove(peer_info->conn_idx, GATT_CLIENT_STORAGE_ID);
        ble_client_remove(peer_info->gatt_client);
        ble_client_cleanup(peer_info->gatt_client);
        peer_info->gatt_client = NULL;
}

static void purge_cps(peer_info_t *peer_info)
{
        ble_storage_remove(peer_info->conn_idx, CPS_CLIENT_STORAGE_ID);
        ble_client_remove(peer_info->cps_client);
        ble_client_cleanup(peer_info->cps_client);
        peer_info->cps_client = NULL;
}

static void purge_clients_in_range(peer_info_t *peer_info, uint16_t start_h, uint16_t end_h)
{
        if (peer_info->gatt_client && ble_client_in_range(peer_info->gatt_client, start_h, end_h)) {
                purge_gatt(peer_info);
        }

        if (peer_info->cps_client && ble_client_in_range(peer_info->cps_client, start_h, end_h)) {
                purge_cps(peer_info);
        }
}

static void clicmd_default_handler(int argc, const char *argv[], void *user_data)
{
        printf("Valid commands:\r\n");
        printf("\tscan <start|stop> [any]\r\n");
        printf("\tconnect <address [public|private] | index>\r\n");
        printf("\tconnect cancel\r\n");
        printf("\tdisconnect <conn_idx>\r\n");
        printf("\tunbond [[public|private] <address> | all]\r\n");
        printf("\tcp_notif <conn_idx> [on|off]\r\n");
        printf("\tupdate_sensor_loc <conn_idx> <loc>\r\n");
        printf("\tget_sup_sensor_loc <conn_idx>\r\n");
        printf("\tset <conn_idx> <parameter> <value>\r\n");
}

static void clicmd_scan_usage(void)
{
        printf("usage: scan <start|stop> [any]\r\n");
        printf("\t\"any\" will disable filtering devices by CPS UUID, only valid for \"scan start\"\r\n");
}

static void clicmd_scan_handler(int argc, const char *argv[], void *user_data)
{
        ble_error_t status;

        if (argc < 2) {
                clicmd_scan_usage();
                return;
        }

        if (!strcasecmp("start", argv[1])) {
                if (app_state != APP_STATE_IDLE) {
                        printf("ERROR: application need to be in idle state to start scanning\r\n");
                        return;
                }

                status = ble_gap_scan_start(GAP_SCAN_ACTIVE, GAP_SCAN_OBSERVER_MODE,
                                                                BLE_SCAN_INTERVAL_FROM_MS(30),
                                                                BLE_SCAN_WINDOW_FROM_MS(30),
                                                                false, false);
                if (status != BLE_STATUS_OK) {
                        printf("ERROR: scan can not be started (0x%02X)\r\n", status);
                        return;
                }

                printf("Scanning...\r\n");

                app_state = APP_STATE_SCANNING;

                scan_state.match_any = (argc > 2) && !strcmp(argv[2], "any");
                scan_state.num_devices = 0;
        } else if (!strcasecmp("stop", argv[1])) {
                if (app_state != APP_STATE_SCANNING) {
                        printf("ERROR: application need to be in scanning state to stop "
                                                                                "scanning\r\n");
                        return;
                }

                status = ble_gap_scan_stop();
                if (status != BLE_STATUS_OK) {
                        printf("ERROR: scan can not be stopped (0x%02X)\r\n", status);
                        return;
                }

                printf("Scan stopping...\r\n");
        } else {
                clicmd_scan_usage();
        }
}

static size_t connected_devices_count(void)
{
        size_t length = BLE_GAP_MAX_CONNECTED;
        gap_device_t devices[BLE_GAP_MAX_CONNECTED];

        ble_gap_get_devices(GAP_DEVICE_FILTER_CONNECTED, NULL, &length, devices);

        return length;
}

static void clicmd_connect_usage(void)
{
        printf("usage: connect <address [public|private] | index>\r\n");
        printf("       connect cancel\r\n");
        printf("\tinstead of address, index of found device can be passed\r\n");
        printf("\tif not specified, public address is assumed\r\n");
        printf("\tuse 'connect cancel' to cancel any outgoing connection attempt\r\n");
}

static void clicmd_connect_handler(int argc, const char *argv[], void *user_data)
{
        static const gap_conn_params_t cp = {
                .interval_min = BLE_CONN_INTERVAL_FROM_MS(50),
                .interval_max = BLE_CONN_INTERVAL_FROM_MS(70),
                .slave_latency = 0,
                .sup_timeout = BLE_SUPERVISION_TMO_FROM_MS(1000),
        };

        bd_address_t addr;
        size_t dev_index;
        ble_error_t status;

        if (argc < 2) {
                clicmd_connect_usage();
                return;
        }

        if (!strcasecmp("cancel", argv[1])) {
                if (app_state != APP_STATE_CONNECTING) {
                        printf("ERROR: application need to be in connecting state to stop "
                                                                                "connecting\r\n");
                        return;
                }

                status = ble_gap_connect_cancel();
                if (status != BLE_STATUS_OK) {
                        printf("ERROR: connect can not be canceled (0x%02X)\r\n", status);
                        return;
                }

                app_state = APP_STATE_IDLE;

                return;
        }

        if (app_state != APP_STATE_IDLE) {
                printf("ERROR: application need to be in idle state to connect\r\n");
                return;
        }

        /* Check if the application achieves max number of connected devices (sensors) */
        if (connected_devices_count() >= BLE_GAP_MAX_CONNECTED) {
                printf("ERROR: max number of connected devices was achieved\r\n");
                return;
        }

        /*
         * If argument cannot be parsed to valid address, check if it can be used as index in
         * found devices cache.
         */
        if (!ble_address_from_string(argv[1], PUBLIC_ADDRESS, &addr)) {
                dev_index = atoi(argv[1]);
                if (dev_index < 1 || dev_index > scan_state.num_devices) {
                        clicmd_connect_usage();
                        return;
                }

                addr = scan_state.devices[dev_index - 1].addr;
        } else {
                if (argc > 2) {
                        /*
                         * If address type argument is present, check for "private" or leave "public"
                         * as set by default.
                         */

                        if (!strcasecmp("private", argv[2])) {
                                addr.addr_type = PRIVATE_ADDRESS;
                        }
                } else {
                        size_t i;

                        /*
                         * If address type is not present try to check for address in found devices
                         * cache, otherwise leave "public".
                         */

                        for (i = 0; i < scan_state.num_devices; i++) {
                                found_device_t *dev = &scan_state.devices[i];

                                if (!memcmp(&dev->addr.addr, &addr.addr, sizeof(addr.addr))) {
                                        addr.addr_type = dev->addr.addr_type;
                                        break;
                                }
                        }
                }
        }

        status = ble_gap_connect(&addr, &cp);
        if (status != BLE_STATUS_OK) {
                printf("ERROR: connection failed\r\n");
                printf("\tStatus: 0x%02x\r\n", status);
                return;
        }

        printf("Connecting to %s ...\r\n", ble_address_to_string(&addr));

        app_state = APP_STATE_CONNECTING;
}

static void clicmd_disconnect_usage(void)
{
        printf("usage: disconnect <conn_idx>\r\n");
}

static void clicmd_disconnect_handler(int argc, const char *argv[], void *user_data)
{
        peer_info_t *peer_info;
        uint16_t conn_idx;

        if (argc < 2) {
                clicmd_disconnect_usage();
                return;
        }

        conn_idx = atoi(argv[1]);
        peer_info = find_peer_info(conn_idx);

        if (!peer_info) {
                printf("ERROR: device has to be connected to disconnect\r\n");
                return;
        }

        ble_gap_disconnect(peer_info->conn_idx, BLE_HCI_ERROR_REMOTE_USER_TERM_CON);

        printf("Disconnecting from %s\r\n", ble_address_to_string(&peer_info->addr));
}

static void clicmd_unbond_usage(void)
{
        printf("usage: unbond [[public|private] <address> | all]\r\n");
        printf("\tpublic    set address type public\r\n");
        printf("\tprivate   set address type private\r\n");
        printf("\taddress   address of bonded device\r\n");
        printf("\tall       unbond all bonded devices\r\n");
}

static void print_unbond_info(ble_error_t status, bd_address_t *address)
{
        printf("Unbond device\r\n");
        printf("\tStatus: 0x%02x\r\n", status);
        printf("\tAddress: %s\r\n", address ? ble_address_to_string(address) : "not found");
}

static void unbond_all(void)
{
        uint8_t i, length;
        bd_address_t *bonded_devices;
        ble_error_t status;

        ble_gap_get_bonded(&length, &bonded_devices);

        if (!length) {
                print_unbond_info(BLE_ERROR_NOT_FOUND, NULL);
        }

        for (i = 0; i < length; i++) {
                status = ble_gap_unpair(&bonded_devices[i]);
                print_unbond_info(status, &bonded_devices[i]);
        }

        OS_FREE(bonded_devices);
}

static void unbond_by_address(bd_address_t *address)
{
        ble_error_t status;

        status = ble_gap_unpair(address);
        print_unbond_info(status, address);
}

static void clicmd_unbond_handler(int argc, const char *argv[], void *user_data)
{
        bd_address_t address;

        if (argc < 2) {
                clicmd_unbond_usage();
                return;
        }

        if (!strcasecmp("all", argv[1])) {
                unbond_all();
                return;
        }

        if (!strcasecmp("public", argv[1])) {
                if (argc < 3 || !ble_address_from_string(argv[2], PUBLIC_ADDRESS, &address)) {
                        clicmd_unbond_usage();
                        return;
                }
        } else if (!strcasecmp("private", argv[1])) {
                if (argc < 3 || !ble_address_from_string(argv[2], PRIVATE_ADDRESS, &address)) {
                        clicmd_unbond_usage();
                        return;
                }
        } else if (!ble_address_from_string(argv[1], PUBLIC_ADDRESS, &address)) {
                clicmd_unbond_usage();
                return;
        }

        unbond_by_address(&address);
}

static void clicmd_cp_notif_usage(void)
{
        printf("usage: cp_notif <conn_idx> <on|off>\r\n");
}

static void clicmd_cp_notif_handler(int argc, const char *argv[], void *user_data)
{
        peer_info_t *peer_info;
        uint16_t conn_idx;

        if (argc < 3) {
                clicmd_cp_notif_usage();
                return;
        }

        conn_idx = atoi(argv[1]);
        peer_info = find_peer_info(conn_idx);

        if (!peer_info) {
                printf("ERROR: connection index is not recognized\r\n");
                return;
        }

        if (!peer_info->cps_client) {
                printf("ERROR: No CPS client instantiated\r\n");
                return;
        }

        if (!strcasecmp("on", argv[2])) {
                cps_client_set_event_state(peer_info->cps_client,
                                                        CPS_CLIENT_EVENT_MEASUREMENT_NOTIF, true);
        } else if (!strcasecmp("off", argv[2])) {
                cps_client_set_event_state(peer_info->cps_client,
                                                        CPS_CLIENT_EVENT_MEASUREMENT_NOTIF, false);
        } else {
                clicmd_cp_notif_usage();
        }
}

static void clicmd_update_sensor_loc_handler(int argc, const char *argv[], void *user_data)
{
        peer_info_t *peer_info;
        uint16_t conn_idx;
        bool ret;

        if (argc < 3) {
                printf("Usage: update_sensor_loc <conn_idx> <loc>\r\n");
                return;
        }

        conn_idx = atoi(argv[1]);
        peer_info = find_peer_info(conn_idx);

        if (!peer_info) {
                printf("ERROR: connection index is not recognized\r\n");
                return;
        }

        if (!peer_info->cps_client) {
                printf("ERROR: No CPS client instantiated\r\n");
                return;
        }

        ret = cps_client_update_sensor_location(peer_info->cps_client, atoi(argv[2]));

        printf("Update sensor location %s\r\n", ret ? "success" : "fail");
}

static void clicmd_get_sup_sensor_loc_handler(int argc, const char *argv[], void *user_data)
{
        peer_info_t *peer_info;
        uint16_t conn_idx;
        bool ret;

        if (argc < 2) {
                printf("Missing <conn_idx>\r\n");
                return;
        }

        conn_idx = atoi(argv[1]);
        peer_info = find_peer_info(conn_idx);

        if (!peer_info) {
                printf("ERROR: connection index is not recognized\r\n");
                return;
        }

        if (!peer_info->cps_client) {
                printf("ERROR: No CPS client instantiated\r\n");
                return;
        }

        ret = cps_client_request_supported_sensor_locations(peer_info->cps_client);

        printf("Get supported sensor locations %s\r\n", ret ? "success" : "fail");
}

static void clicmd_set_usage(void)
{
        printf("usage: set <conn_idx> <parameter> <value>\r\n");
        printf("\tconn_idx of connected device\r\n");
        printf("available parameters\r\n");
        printf("\tcrank_len\r\n");
        printf("\tchain_len\r\n");
        printf("\tchain_weight\r\n");
        printf("\tspan_len\r\n");
}

static void clicmd_set_handler(int argc, const char *argv[], void *user_data)
{
        peer_info_t *peer_info;
        uint16_t conn_idx;
        bool ret;

        if (argc < 4) {
                clicmd_set_usage();
                return;
        }

        conn_idx = atoi(argv[1]);
        peer_info = find_peer_info(conn_idx);

        if (!peer_info) {
                printf("ERROR: connection index is not recognized\r\n");
                return;
        }

        if (!strcasecmp("crank_len", argv[2])) {
                ret = cps_client_set_crank_length(peer_info->cps_client, atoi(argv[3]));
        } else if (!strcasecmp("chain_len", argv[2])) {
                ret = cps_client_set_chain_length(peer_info->cps_client, atoi(argv[3]));
        } else if (!strcasecmp("chain_weight", argv[2])) {
                ret = cps_client_set_chain_weight(peer_info->cps_client, atoi(argv[3]));
        } else if (!strcasecmp("span_len", argv[2])) {
                ret = cps_client_set_span_length(peer_info->cps_client, atoi(argv[3]));
        } else {
                clicmd_set_usage();
                return;
        }

        printf("Operation set %s %s\r\n", argv[2], ret ? "success" : "fail");
}

static const cli_command_t clicmd[] = {
        { .name = "scan",               .handler = clicmd_scan_handler, },
        { .name = "connect",            .handler = clicmd_connect_handler, },
        { .name = "disconnect",         .handler = clicmd_disconnect_handler, },
        { .name = "unbond",             .handler = clicmd_unbond_handler, },
        { .name = "cp_notif",           .handler = clicmd_cp_notif_handler, },
        { .name = "update_sensor_loc",  .handler = clicmd_update_sensor_loc_handler, },
        { .name = "get_sup_sensor_loc", .handler = clicmd_get_sup_sensor_loc_handler, },
        { .name = "set",                .handler = clicmd_set_handler, },
        {},
};

static bool match_browse_req(const void *data, const void *match_data)
{
        const browse_req_t *b = (const browse_req_t *) data;
        const browse_req_t *a = (const browse_req_t *) match_data;

        return ((a->end_h >= b->start_h) && (a->start_h <= b->end_h)) ||
                (MAX(a->end_h, b->start_h) - MIN(a->end_h, b->start_h) <= 1) ||
                (MAX(a->start_h, b->end_h) - MIN(a->start_h, b->end_h) <= 1);
}

static void add_browse_req(peer_info_t *peer_info, uint16_t start_h, uint16_t end_h)
{
        browse_req_t *req;
        browse_req_t *a;

        req = OS_MALLOC(sizeof(browse_req_t));
        req->start_h = start_h;
        req->end_h = end_h;

        do {
                a = queue_remove(&peer_info->pending_browse_queue, match_browse_req, req);
                if (a) {
                        req->start_h = MIN(a->start_h, req->start_h);
                        req->end_h = MAX(a->end_h, req->end_h);
                        OS_FREE(a);
                }
        } while (a);

        queue_push_back(&peer_info->pending_browse_queue, req);
}

static void start_browse(peer_info_t *peer_info)
{
        browse_req_t *req = queue_pop_front(&peer_info->pending_browse_queue);

        if (peer_info->browsing || !req) {
                return;
        }

        printf("Browsing...\r\n");

        ble_gattc_browse_range(peer_info->conn_idx, req->start_h, req->end_h, NULL);
        peer_info->browsing = true;
        OS_FREE(req);
}

static bool start_auth(peer_info_t *peer_info, auth_type_t auth_type, bool mitm)
{
        gap_device_t gap_dev;

        if (peer_info->busy_auth) {
                /* We're already doing authentication, ignore */
                return false;
        }

        if (ble_gap_get_device_by_conn_idx(peer_info->conn_idx, &gap_dev) != BLE_STATUS_OK) {
                return false;
        }

        if ((auth_type == AUTH_TYPE_ENCRYPT) && !gap_dev.paired) {
                return false;
        }

        if (gap_dev.paired) {
                ble_gap_set_sec_level(peer_info->conn_idx, mitm ? GAP_SEC_LEVEL_3 : GAP_SEC_LEVEL_2);
        } else {
                ble_gap_pair(peer_info->conn_idx, auth_type == AUTH_TYPE_BOND);
        }

        peer_info->busy_auth = true;

        return true;
}

static void finish_auth(peer_info_t *peer_info, ble_error_t status)
{
        if (!peer_info->busy_auth) {
                return;
        }

        if (status == BLE_ERROR_ENC_KEY_MISSING) {
                add_browse_req(peer_info, 0x0001, 0xffff);

                printf("Encrypt key missing. Trying to pair again.\r\n");
                ble_gap_pair(peer_info->conn_idx, true);
                return;
        }

        peer_info->busy_auth = false;

        if (queue_length(&peer_info->pending_browse_queue)) {
                start_browse(peer_info);
                return;
        }
}

static void resolve_found_device(found_device_t *dev)
{
        // Check if device address is resolvable
        if (ADDR_RESOLVABLE(dev->addr)) {
                ble_gap_address_resolve(dev->addr);
        }
}

static found_device_t *get_found_device(const bd_address_t *addr, size_t *index)
{
        size_t i;

        for (i = 0; i < scan_state.num_devices; i++) {
                found_device_t *dev = &scan_state.devices[i];

                if (ble_address_cmp(&dev->addr, addr)) {
                        *index = i + 1;
                        return dev;
                }
        }

        return NULL;
}

static found_device_t *add_found_device(const bd_address_t *addr, size_t *index)
{
        static found_device_t tmp_dev;
        found_device_t *dev;

        if (scan_state.num_devices >= MAX_FOUND_DEVICES) {
                dev = &tmp_dev;
                *index = 0;
        } else {
                dev = &scan_state.devices[scan_state.num_devices++];
                *index = scan_state.num_devices;
        }

        dev->addr = *addr;
        dev->name_found = false;

        return dev;
}

static void handle_evt_gap_adv_report(const ble_evt_gap_adv_report_t *evt)
{
        found_device_t *dev;
        size_t dev_index = 0;
        const uint8_t *p;
        uint8_t ad_len, ad_type;
        bool new_device = false;
        const char *dev_name = NULL;
        size_t dev_name_len = 0;

        dev = get_found_device(&evt->address, &dev_index);
        if (dev && dev->name_found) {
                return;
        }

        /* Add device if 'any' was specified as scan argument */
        if (!dev && scan_state.match_any) {
                new_device = true;
                dev = add_found_device(&evt->address, &dev_index);

                if (dev_index > 0) {
                        resolve_found_device(dev);
                }
        }

        for (p = evt->data; p < evt->data + evt->length; p += ad_len) {
                ad_len = (*p++) - 1; /* ad_len is length of value only, without type */
                ad_type = *p++;

                /* Device not found so we look for UUID */
                if (!dev && (ad_type == GAP_DATA_TYPE_UUID16_LIST ||
                                                        ad_type == GAP_DATA_TYPE_UUID16_LIST_INC)) {
                        size_t idx;

                        for (idx = 0; idx < ad_len; idx += sizeof(uint16_t)) {
                                if (get_u16(p + idx) == UUID_SERVICE_CPS) {
                                        new_device = true;
                                        dev = add_found_device(&evt->address, &dev_index);

                                        if (dev_index > 0) {
                                                resolve_found_device(dev);
                                        }

                                        break;
                                }
                        }

                        continue;
                }

                /* Look for name and store it to use later, if proper UUID is found */
                if (ad_type == GAP_DATA_TYPE_SHORT_LOCAL_NAME ||
                                                        ad_type == GAP_DATA_TYPE_LOCAL_NAME) {
                        dev_name = (const char *) p;
                        dev_name_len = ad_len;

                        if (dev) {
                                /* Already have device, no need to look further */
                                break;
                        }
                }
        }

        /*
         * If we have both device and device name, print as new device found with name.
         * For new device and no name, just print address for now.
         */
        if (dev && dev_name) {
                dev->name_found = true;
                printf("[%02d] Device found: %s %s (%.*s)\r\n", dev_index,
                                evt->address.addr_type == PUBLIC_ADDRESS ? "public " : "private",
                                ble_address_to_string(&evt->address),
                                dev_name_len, dev_name);
        } else if (new_device) {
                printf("[%02d] Device found: %s %s\r\n", dev_index,
                                evt->address.addr_type == PUBLIC_ADDRESS ? "public " : "private",
                                ble_address_to_string(&evt->address));
        }
}

static void handle_evt_gap_scan_completed(const ble_evt_gap_scan_completed_t *evt)
{
        printf("Scan stopped\r\n");

        app_state = APP_STATE_IDLE;
}

static void gatt_service_changed_cb(ble_client_t *gatt_client, uint16_t start_h, uint16_t end_h)
{
        uint16_t conn_idx = gatt_client->conn_idx;
        peer_info_t *peer_info;

        peer_info = find_peer_info(conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Service changed, start_h: 0x%04x, end_h: 0x%04x\r\n", start_h, end_h);

        purge_clients_in_range(peer_info, start_h, end_h);

        peer_info->pending_init = 0;
        peer_info->busy_init = false;

        add_browse_req(peer_info, start_h, end_h);
        start_browse(peer_info);
}

static void gatt_set_event_state_completed_cb(ble_client_t *gatt_client, gatt_client_event_t event,
                                                                                att_error_t status)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(gatt_client->conn_idx);
        if (!peer_info) {
                return;
        }

        if (event != GATT_CLIENT_EVENT_SERVICE_CHANGED_INDICATE) {
                return;
        }

        clear_pending_action(peer_info, PENDING_ACTION_ENABLE_SVC_CHANGED_IND, status);
}

static const gatt_client_callbacks_t gatt_callbacks = {
        .service_changed = gatt_service_changed_cb,
        .set_event_state_completed = gatt_set_event_state_completed_cb,
};

static const char *event2str(cps_client_event_t event)
{
        switch (event) {
        case CPS_CLIENT_EVENT_CP_VECTOR_NOTIF:
                return "Cycling Power Vector Notifications";
        case CPS_CLIENT_EVENT_MEASUREMENT_BROADCAST:
                return "Measurement Broadcast";
        case CPS_CLIENT_EVENT_MEASUREMENT_NOTIF:
                return "Cycling Power Measurement Notifications";
        default:
                return "unknown";
        }
}

static void cps_set_event_state_completed_cb(ble_client_t *client, att_error_t status,
                                                                cps_client_event_t event)
{
        peer_info_t *peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        switch (event) {
        case CPS_CLIENT_EVENT_CP_VECTOR_NOTIF:
                clear_pending_action(peer_info, PENDING_ACTION_ENABLE_POWER_VECTOR_NOTIF, status);
                break;
        case CPS_CLIENT_EVENT_MEASUREMENT_NOTIF:
                clear_pending_action(peer_info, PENDING_ACTION_ENABLE_MEASUREMENT_NOTIF, status);
                break;
        default:
                break;
        }

        printf("Set %s completed\r\n", event2str(event));
        printf("\tStatus: 0x%02x\r\n", status);
        printf("\tConnection index: %u\r\n", client->conn_idx);
}

static void cps_set_cycling_power_cp_state_completed_cb(ble_client_t *client, att_error_t status)
{
        peer_info_t *peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Set Control Point indications completed\r\n");
        printf("\tStatus: 0x%02x\r\n", status);
        printf("\tConnection index: %u\r\n", client->conn_idx);

        clear_pending_action(peer_info, PENDING_ACTION_ENABLE_CTRL_POINT_IND, status);
}

static void cps_read_feature_completed_cb(ble_client_t *client, att_error_t status,
                                                                cps_client_feature_t features)
{
        peer_info_t *peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        if (status == ATT_ERROR_OK) {
                printf("Cycling Power Features: 0x%08x\r\n", features);
                printf("\tConnection index: %u\r\n", client->conn_idx);

                printf("\tPedal Power Balance %ssupported\r\n",
                                        features & CPS_CLIENT_FEATURE_PEDAL_POWER_BALANCE_SUPPORT ?
                                        "" : "NOT ");

                printf("\tAccumulated Torque %ssupported\r\n",
                                        features & CPS_CLIENT_FEATURE_ACCUMULATED_TORQUE_SUPPORRT ?
                                        "" : "NOT ");

                printf("\tWheel Revolution Data %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_WHEEL_REVOLUTION_DATA_SUPPORT ?
                                "" : "NOT ");

                printf("\tCrank Revolution Data %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_CRANK_REVOLUTION_DATA_SUPPORT ?
                                "" : "NOT ");

                printf("\tExtreme Magnitudes %ssupported\r\n",
                                        features & CPS_CLIENT_FEATURE_EXTREME_MAGNITUDE_SUPPORT ?
                                        "" : "NOT ");

                printf("\tExtreme Angles %ssupported\r\n",
                                        features & CPS_CLIENT_FEATURE_EXTREME_ANGLE_SUPPORT ?
                                        "" : "NOT ");

                printf("\tTop and Bottom Dead Spot Angles %ssupported\r\n",
                        features & CPS_CLIENT_FEATURE_TOP_AND_BOTTOM_DEAD_SPOT_ANGLES_SUPPORT ?
                        "" : "NOT ");

                printf("\tAccumulated Energy %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_ACCUMULATED_ENERGY_SUPPORT ?
                                "" : "NOT ");

                printf("\tOffset Compensation Indicator %ssupported\r\n",
                        features & CPS_CLIENT_FEATURE_OFFSET_COMPENSATION_INDICATOR_SUPPORT ?
                        "" : "NOT ");

                printf("\tOffset Compensation %ssupported\r\n",
                                        features & CPS_CLIENT_FEATURE_OFFSET_COMPENSATION_SUPPORT ?
                                        "" : "NOT ");

                printf("\tCycling Power Measurement Characteristic Content Masking %ssupported\r\n",
                        features & CPS_CLIENT_FEATURE_CHARACTERISTIC_CONTENT_MASKING_SUPPORT ?
                        "" : "NOT ");

                printf("\tMultiple Sensor Locations %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORT ?
                                "" : "NOT ");

                printf("\tCrank Length Adjustment %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_CRANK_LENGTH_ADJUSTMENT_SUPPORT ?
                                "" : "NOT ");

                printf("\tChain Length Adjustment %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_CHAIN_LENGTH_ADJUSTMENT_SUPPORT ?
                                "" : "NOT ");

                printf("\tChain Weight Adjustment %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_CHAIN_WEIGHT_ADJUSTMENT_SUPPORT ?
                                "" : "NOT ");

                printf("\tSpan Length Adjustment %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_SPAN_LENGTH_ADJUSTMENT_SUPPORT ?
                                        "" : "NOT ");

                if ((features & CPS_CLIENT_FEATURE_SENSOR_MEASUREMENT_CONTEXT) ==
                                CPS_CLIENT_FEATURE_SENSOR_MEASUREMNET_CONTEXT_TORQUE_BASED) {
                        printf("\tSensor Measurement Context: Torque based\r\n");
                } else {
                        printf("\tSensor Measurement Context: Force based\r\n");
                }

                printf("\tInstantaneous Measurement Direction %ssupported\r\n",
                        features & CPS_CLIENT_FEATURE_INSTANTANEOUS_MEASUREMENT_DIR_SUPPORT ?
                        "" : "NOT ");

                printf("\tFactory Calibration Date %ssupported\r\n",
                                features & CPS_CLIENT_FEATURE_FACTORY_CALIBRATION_DATE_SUPPORT ?
                                "" : "NOT ");

                printf("\tEnhanced Offset Compensation %ssupported\r\n",
                        features & CPS_CLIENT_FEATURE_ENHANCED_OFFSET_COMPENSATION_SUPPORT ?
                        "" : "NOT ");

                if ((features & CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT) ==
                                        CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT_UNSPECIFIED) {
                        printf("\tDistribute System: Unspecified (legacy sensor)\r\n");
                } else if ((features & CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT) ==
                                        CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT_NOT_FOR_USE) {
                        printf("\tDistribute System: Not for use in a distributed system\r\n");
                } else if ((features & CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT) ==
                                        CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT_CAN_BE_USED) {
                        printf("\tDistribute System: Can be used in a distributed system\r\n");
                } else {
                        printf("\tDistribute System: Unknown\r\n");
                }
        } else {
                printf("\tFailed to read features (0x%02x)\r\n", status);
                printf("\tConnection index: %u\r\n", client->conn_idx);
        }

        clear_pending_action(peer_info, PENDING_ACTION_READ_FEATURES, status);
}

static char *sensor_location2str(cps_client_sensor_location_t location)
{
        switch (location) {
        case CPS_CLIENT_SENSOR_LOCATION_OTHER:
                return "other";
        case CPS_CLIENT_SENSOR_LOCATION_TOP_OF_SHOE:
                return "top of shoe";
        case CPS_CLIENT_SENSOR_LOCATION_IN_SHOE:
                return "in shoe";
        case CPS_CLIENT_SENSOR_LOCATION_HIP:
                return "hip";
        case CPS_CLIENT_SENSOR_LOCATION_FRONT_WHEEL:
                return "front wheel";
        case CPS_CLIENT_SENSOR_LOCATION_LEFT_CRANK:
                return "left crank";
        case CPS_CLIENT_SENSOR_LOCATION_RIGHT_CRANK:
                return "right crank";
        case CPS_CLIENT_SENSOR_LOCATION_LEFT_PEDAL:
                return "left pedal";
        case CPS_CLIENT_SENSOR_LOCATION_RIGHT_PEDAL:
                return "right pedal";
        case CPS_CLIENT_SENSOR_LOCATION_FRONT_HUB:
                return "front hub";
        case CPS_CLIENT_SENSOR_LOCATION_REAR_DROPOUT:
                return "rear dropout";
        case CPS_CLIENT_SENSOR_LOCATION_CHAINSTAY:
                return "chainstay";
        case CPS_CLIENT_SENSOR_LOCATION_REAR_WHEEL:
                return "rear wheel";
        case CPS_CLIENT_SENSOR_LOCATION_REAR_HUB:
                return "rear hub";
        case CPS_CLIENT_SENSOR_LOCATION_CHEST:
                return "chest";
        default:
                return "unknown";
        }
}

static void cps_read_sensor_location_completed_cb(ble_client_t *client, att_error_t status,
                                                        cps_client_sensor_location_t location)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                printf("Failed to read Sensor Location - unknown device\r\n");
                return;
        }

        if (status == ATT_ERROR_OK) {
                printf("Sensor Location: %s\r\n", sensor_location2str(location));
        } else {
                printf("Failed to read Sensor Location (0x%02X)\r\n", status);
        }
        printf("\tConnection index: %u\r\n", client->conn_idx);

        clear_pending_action(peer_info, PENDING_ACTION_READ_SENSOR_LOCATION, status);
}

static void cps_measuremnet_notif_cb(ble_client_t *client,
                                                const cps_client_measurement_t *measurement)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("CP Measurement notification received\r\n");

        printf("\tInstantaneous power: %d [W]\r\n", measurement->instant_power);

        if (measurement->pedal_power_balance_present) {
                const char *reference = ((measurement->pedal_power_balance_reference ==
                           CPS_CLIENT_PEDAL_POWER_BALANCE_REFERENCE_UNKNOWN) ? "Unknown" : "Left");

                printf("\tPedal power balance: %u [%%]\r\n", measurement->pedal_power_balance >> 1);
                printf("\tPedal power balance reference: %s\r\n", reference);
        }

        if (measurement->accumulated_torque_present) {
                printf("\tAccumulated torque source: %s\r\n",
                        measurement->accumulated_torque_source ==
                        CPS_CLIENT_ACCUMULATED_TORQUE_SOURCE_WHEEL_BASED ? "Wheel" : "Crank");
                printf("\tAccumulated torque 0x%04x\r\n", measurement->accumulated_torque);
        }

        if (measurement->wheel_revolution_data_present) {
                printf("\tWheel revolution cumulative value %" PRIu32 "\r\n",
                                                                measurement->wrd_cumulative_revol);
                printf("\tWheel revolution last event time 0x%04x [1/2048 s]\r\n",
                                                        measurement->wrd_last_wheel_evt_time);
        }

        if (measurement->crank_revolution_data_present) {
                printf("\tCrank revolution cumulative value 0x%04x\r\n",
                                                                measurement->crd_cumulative_revol);
                printf("\tCrank revolution last event time 0x%04x [1/1024 s]\r\n",
                                                        measurement->crd_last_crank_evt_time);
        }

        if (measurement->extreme_force_magnitude_present) {
                printf("\tMaximal force magnitude: %d [N]\r\n",
                                                             measurement->efm_max_force_magnitude);
                printf("\tMinimal force magnitude: %d [N]\r\n",
                                                             measurement->efm_min_force_magnitude);
        }

        if (measurement->extreme_torque_magnitude_present) {
                printf("\tMaximal torque magnitude: %d.%03u [Nm]\r\n",
                                                          measurement->etm_max_torq_magnitude >> 5,
                                       ((measurement->etm_max_torq_magnitude & 0x1F) * 1000) >> 5);
                printf("\tMinimal torque magnitude: %d.%03u [Nm]\r\n",
                                                          measurement->etm_min_torq_magnitude >> 5,
                                       ((measurement->etm_min_torq_magnitude & 0x1F) * 1000) >> 5);
        }
        if (measurement->extreme_angle_present) {
                printf("\tMaximal angle: %u [deg]\r\n", measurement->ea_maximum_angle);
                printf("\tMinimal angle: %u [deg]\r\n", measurement->ea_minimum_angle);
        }

        if (measurement->top_dead_spot_angle_present) {
                printf("\tTop dead spot angle: %u [deg]\r\n", measurement->top_dead_spot_angle);
        }

        if (measurement->bottom_dead_spot_angle_present) {
                printf("\tBottom dead spot angle: %u [deg]\r\n", measurement->bottom_dead_spot_angle);
        }

        if (measurement->accumulated_energy_present) {
                printf("\tAccumulated energy: 0x%04x [kJ]\r\n", measurement->accumulated_energy);
        }
}

static void cps_update_sensor_location_completed_cb(ble_client_t *client,
                                                                cps_client_status_t status)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Update sensor location completed\r\n");
        printf("\tStatus: 0x%04x\r\n", status);
}

static void cps_start_timer_cb(ble_client_t *client, uint32_t ms)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        OS_TIMER_CHANGE_PERIOD(peer_info->cps_tmo_timer, OS_MS_2_TICKS(ms), OS_TIMER_FOREVER);
        OS_TIMER_START(peer_info->cps_tmo_timer, OS_TIMER_FOREVER);
}

static void cps_cancel_timer_cb(ble_client_t *client)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        OS_TIMER_STOP(peer_info->cps_tmo_timer, OS_TIMER_FOREVER);
}

static void cps_request_supported_sensor_locations_completed_cb(ble_client_t *client,
                                        cps_client_status_t status, uint8_t locations_count,
                                        const cps_client_sensor_location_t *locations)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Request supported sensor locations complete:\r\n");
        printf("\tStatus: 0x%04x\r\n", status);

        for (uint8_t i = 0; i < locations_count; i++) {
                printf("\t%s\r\n", sensor_location2str(locations[i]));
        }
}

static void cps_set_crank_length_completed_cb(ble_client_t *client, cps_client_status_t status)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Set crank length complete:\r\n");
        printf("\tStatus: 0x%04x\r\n", status);
}

static void cps_set_chain_length_completed_cb(ble_client_t *client, cps_client_status_t status)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Set chain length complete:\r\n");
        printf("\tStatus: 0x%04x\r\n", status);
}

static void cps_set_chain_weight_completed_cb(ble_client_t *client, cps_client_status_t status)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Set chain weight complete:\r\n");
        printf("\tStatus: 0x%04x\r\n", status);
}

static void cps_set_span_length_completed_cb(ble_client_t *client, cps_client_status_t status)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(client->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Set span length complete:\r\n");
        printf("\tStatus: 0x%04x\r\n", status);
}

static const cps_client_callbacks_t cps_callbacks = {
        .set_event_state_completed = cps_set_event_state_completed_cb,
        .set_cycling_power_cp_state_completed = cps_set_cycling_power_cp_state_completed_cb,
        .read_feature_completed = cps_read_feature_completed_cb,
        .read_sensor_location_completed = cps_read_sensor_location_completed_cb,
        .measurement_notif = cps_measuremnet_notif_cb,
        .update_sensor_location_completed = cps_update_sensor_location_completed_cb,
        .start_timer = cps_start_timer_cb,
        .cancel_timer = cps_cancel_timer_cb,
        .request_supported_sensor_locations_completed = cps_request_supported_sensor_locations_completed_cb,
        .set_crank_length_completed = cps_set_crank_length_completed_cb,
        .set_chain_length_completed = cps_set_chain_length_completed_cb,
        .set_chain_weight_completed = cps_set_chain_weight_completed_cb,
        .set_span_length_completed = cps_set_span_length_completed_cb,
};

static ble_client_t *get_stored_client(uint16_t conn_idx, ble_storage_key_t key)
{
        ble_error_t err;
        uint16_t len = 0;
        void *buffer;

        err = ble_storage_get_buffer(conn_idx, key, &len, &buffer);
        if (err) {
                return NULL;
        }

        switch (key) {
        case GATT_CLIENT_STORAGE_ID:
                return gatt_client_init_from_data(conn_idx, &gatt_callbacks, buffer, len);
        case CPS_CLIENT_STORAGE_ID:
                return cps_client_init_from_data(conn_idx, &cps_callbacks, buffer, len);
        default:
                return NULL;
        }
}

static void handle_evt_gap_connected(const ble_evt_gap_connected_t *evt)
{
        peer_info_t *peer_info;
        bool bonded;

        printf("Device connected\r\n");
        printf("\tConnection index: %u\r\n", evt->conn_idx);
        printf("\tAddress: %s %s\r\n",
                        evt->peer_address.addr_type == PUBLIC_ADDRESS ? "public" : "private",
                        ble_address_to_string(&evt->peer_address));

        app_state = APP_STATE_IDLE;

        peer_info = OS_MALLOC(sizeof(*peer_info));
        memset(peer_info, 0, sizeof(*peer_info));

        peer_info->addr = evt->peer_address;
        peer_info->conn_idx = evt->conn_idx;

        queue_init(&peer_info->pending_browse_queue);

        peer_info->cps_tmo_timer = OS_TIMER_CREATE("cpstmo", 1, OS_FAIL, peer_info, timer_cb);

        add_peer_info(peer_info);

        ble_gap_is_bonded(peer_info->conn_idx, &bonded);
        if (bonded) {
                peer_info->gatt_client = get_stored_client(peer_info->conn_idx, GATT_CLIENT_STORAGE_ID);
                peer_info->cps_client = get_stored_client(peer_info->conn_idx, CPS_CLIENT_STORAGE_ID);
        }

        if (start_auth(peer_info, AUTH_TYPE_ENCRYPT, false)) {
                if (!peer_info->cps_client) {
                        add_browse_req(peer_info, 0x0001, 0xffff);
                } else {
                        cps_client_cap_t cps_cap;

                        add_pending_action(peer_info, PENDING_ACTION_ENABLE_MEASUREMENT_NOTIF);
                        add_pending_action(peer_info, PENDING_ACTION_READ_FEATURES);
                        add_pending_action(peer_info, PENDING_ACTION_READ_SENSOR_LOCATION);

                        cps_cap = cps_client_get_capabilities(peer_info->cps_client);
                        if (cps_cap & CPS_CLIENT_CAP_POWER_VECTOR) {
                                add_pending_action(peer_info, PENDING_ACTION_ENABLE_POWER_VECTOR_NOTIF);
                        }

                        if (cps_cap & CPS_CLIENT_CAP_CYCLING_POWER_CONTROL_POINT) {
                                add_pending_action(peer_info, PENDING_ACTION_ENABLE_CTRL_POINT_IND);
                        }
                }
                return;
        }

        if (start_auth(peer_info, AUTH_TYPE_BOND, false)) {
                add_browse_req(peer_info, 0x0001, 0xffff);
                return;
        }

        add_browse_req(peer_info, 0x0001, 0xffff);
        start_browse(peer_info);
}

static void handle_evt_gap_connection_completed(const ble_evt_gap_connection_completed_t *evt)
{
        /* Successful connections are handled in separate event */
        if (evt->status == BLE_STATUS_OK) {
                return;
        }

        printf("Connection failed\r\n");
        printf("\tStatus: 0x%02x\r\n", evt->status);

        app_state = APP_STATE_IDLE;
}

static void handle_evt_gap_disconnected(const ble_evt_gap_disconnected_t *evt)
{
        peer_info_t *peer_info;

        printf("Device disconnected\r\n");
        printf("\tConnection index: %u\r\n", evt->conn_idx);
        printf("\tBD address of disconnected device: %s, %s\r\n",
                                evt->address.addr_type == PUBLIC_ADDRESS ? "public" : "private",
                                ble_address_to_string(&evt->address));
        printf("\tReason of disconnection: 0x%02x\r\n", evt->reason);

        peer_info = remove_peer_info(evt->conn_idx);
        if (!peer_info) {
                return;
        }

        if (peer_info->gatt_client) {
                ble_client_cleanup(peer_info->gatt_client);
        }

        if (peer_info->cps_client) {
                ble_client_cleanup(peer_info->cps_client);
        }

        OS_TIMER_DELETE(peer_info->cps_tmo_timer, OS_TIMER_FOREVER);

        queue_remove_all(&peer_info->pending_browse_queue, OS_FREE_FUNC);

        OS_FREE(peer_info);
}

static void handle_evt_gap_security_request(const ble_evt_gap_security_request_t *evt)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(evt->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Security request\r\n");
        printf("\tConnection index: %u\r\n", evt->conn_idx);
        printf("\tBond: %s\r\n", evt->bond ? "True" : "False");
        printf("\tMITM: %s\r\n", evt->mitm ? "True" : "False");

        start_auth(peer_info, evt->bond ? AUTH_TYPE_BOND : AUTH_TYPE_PAIR, evt->mitm);
}

static void handle_evt_gap_passkey_notify(const ble_evt_gap_passkey_notify_t *evt)
{
        printf("Passkey notification\r\n");
        printf("\tConnection index: %u\r\n", evt->conn_idx);
        printf("\tPasskey: %06" PRIu32 "\r\n", evt->passkey);
}

static void handle_evt_gap_pair_completed(const ble_evt_gap_pair_completed_t *evt)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(evt->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Pair completed\r\n");
        printf("\tConnection index: %u\r\n", evt->conn_idx);
        printf("\tStatus: 0x%02u\r\n", evt->status);
        printf("\tBond: %s\r\n", evt->bond ? "true" : "false");
        printf("\tMITM: %s\r\n", evt->mitm ? "true" : "false");

        finish_auth(peer_info, evt->status);
}

static void handle_evt_gap_sec_level_changed(const ble_evt_gap_sec_level_changed_t *evt)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(evt->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Security level changed\r\n");
        printf("\tConnection index: %u\r\n", evt->conn_idx);
        printf("\tSecurity level: %u\r\n",  evt->level + 1);

        finish_auth(peer_info, BLE_STATUS_OK);
}

static void handle_evt_gap_set_sec_level_failed(const ble_evt_gap_set_sec_level_failed_t *evt)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(evt->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Failed to set security level (0x%02x)\r\n", evt->status);
        printf("\tConnection index: %u\r\n", evt->conn_idx);

        finish_auth(peer_info, evt->status);
}

static void handle_evt_gap_address_resolved(ble_evt_gap_address_resolved_t *evt)
{
        found_device_t *dev;
        size_t index;

        if (evt->address.addr_type == PUBLIC_ADDRESS) {
                // Address is already known
                return;
        }

        dev = get_found_device(&evt->address, &index);
        if (dev) {
                printf("[%02d] Identity Address: %s %s\r\n", index,
                        evt->resolved_address.addr_type == PUBLIC_ADDRESS ? "public" : "private",
                        ble_address_to_string(&evt->resolved_address));
        } else {
                printf("Private address: %s %s, ",
                                 evt->address.addr_type == PUBLIC_ADDRESS ? "public" : "private",
                                 ble_address_to_string(&evt->address));
                printf("Identity Address: %s %s\r\n",
                        evt->resolved_address.addr_type == PUBLIC_ADDRESS ? "public" : "private",
                        ble_address_to_string(&evt->resolved_address));
        }
}

static void handle_evt_gattc_browse_completed(const ble_evt_gattc_browse_completed_t *evt)
{
        peer_info_t *peer_info;
        cps_client_cap_t cps_cap;

        peer_info = find_peer_info(evt->conn_idx);
        if (!peer_info) {
                return;
        }

        printf("Browse completed\r\n");
        printf("\tConnection index: %u\r\n", evt->conn_idx);

        if (queue_length(&peer_info->pending_browse_queue)) {
                browse_req_t *req = queue_pop_front(&peer_info->pending_browse_queue);

                printf("Services changed, browsing at range from 0x%04x to 0x%04x...\r\n",
                                                                         req->start_h, req->end_h);

                ble_gattc_browse_range(evt->conn_idx, req->start_h, req->end_h, NULL);

                OS_FREE(req);
                return;
        }

        peer_info->browsing = false;

        if (!peer_info->cps_client) {
                printf("\tCycling Power service not found\r\n");

                printf("Disconnecting...\r\n");
                ble_gap_disconnect(evt->conn_idx, BLE_HCI_ERROR_REMOTE_USER_TERM_CON);
                return;
        }

        printf("\tCycling Power service found\r\n");

        add_pending_action(peer_info, PENDING_ACTION_ENABLE_MEASUREMENT_NOTIF);
        add_pending_action(peer_info, PENDING_ACTION_READ_FEATURES);
        add_pending_action(peer_info, PENDING_ACTION_READ_SENSOR_LOCATION);

        cps_cap = cps_client_get_capabilities(peer_info->cps_client);
        if (cps_cap & CPS_CLIENT_CAP_POWER_VECTOR) {
                add_pending_action(peer_info, PENDING_ACTION_ENABLE_POWER_VECTOR_NOTIF);
        }

        if (cps_cap & CPS_CLIENT_CAP_CYCLING_POWER_CONTROL_POINT) {
                add_pending_action(peer_info, PENDING_ACTION_ENABLE_CTRL_POINT_IND);
        }

        if (peer_info->gatt_client) {
                gatt_client_cap_t cap = gatt_client_get_capabilites(peer_info->gatt_client);

                printf("\tGATT Service found\r\n");

                /* Register for Service Changed indications */
                if (cap & GATT_CLIENT_CAP_SERVICE_CHANGED) {
                        add_pending_action(peer_info, PENDING_ACTION_ENABLE_SVC_CHANGED_IND);
                }
        } else {
                printf("\tGATT Service not found\r\n");
        }
}

static void handle_evt_gattc_browse_svc(const ble_evt_gattc_browse_svc_t *evt)
{
        peer_info_t *peer_info;

        peer_info = find_peer_info(evt->conn_idx);
        if (!peer_info) {
                return;
        }

        /* We are not interested in any service with 128-bit UUID */
        if (evt->uuid.type != ATT_UUID_16) {
                return;
        }

        switch (evt->uuid.uuid16) {
        case UUID_SERVICE_GATT:
                if (peer_info->gatt_client) {
                        return;
                }

                peer_info->gatt_client = gatt_client_init(&gatt_callbacks, evt);
                if (!peer_info->gatt_client) {
                        return;
                }

                ble_client_add(peer_info->gatt_client);
                store_client(evt->conn_idx, peer_info->gatt_client, GATT_CLIENT_STORAGE_ID);

                break;
        case UUID_SERVICE_CPS:
                if (peer_info->cps_client) {
                        return;
                }

                peer_info->cps_client = cps_client_init(&cps_callbacks, evt);
                if (!peer_info->cps_client) {
                        return;
                }

                ble_client_add(peer_info->cps_client);
                store_client(evt->conn_idx, peer_info->cps_client, CPS_CLIENT_STORAGE_ID);

                break;
        }
}

void cpp_collector_task(void *params)
{
        cli_t cli;

        /* Register CLI */
        cli = cli_register(CLI_NOTIF, clicmd, clicmd_default_handler);

        /* Set device name */
        ble_gap_device_name_set("Black Orca CPP Collector", ATT_PERM_READ);

        /* Setup application in BLE Manager */
        ble_register_app();
        ble_central_start();

        /*
         * We have keyboard support (for CLI) but since support for passkey entry is missing, let's
         * just declare "Display Only" capabilities for now.
         */
        ble_gap_set_io_cap(GAP_IO_CAP_DISP_ONLY);

        queue_init(&peer_info_queue);

        OS_QUEUE_CREATE(peer_cps_tmo_queue, sizeof(uint16_t), BLE_GAP_MAX_CONNECTED);

        /* Initial application task */
        app_task = OS_GET_CURRENT_TASK();

        printf("CPP Collector ready.\r\n");

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                OS_ASSERT(ret == OS_OK);

                /* Notified from BLE manager, can get event */
                if (notif & BLE_APP_NOTIFY_MASK) {
                        ble_evt_hdr_t *hdr;

                        /*
                         * No need to wait for event, should be already there since we were notified
                         * from manager
                         */
                        hdr = ble_get_event(false);

                        if (!hdr) {
                                goto no_event;
                        }

                        ble_client_handle_event(hdr);

                        switch (hdr->evt_code) {
                        case BLE_EVT_GAP_ADV_REPORT:
                                handle_evt_gap_adv_report((ble_evt_gap_adv_report_t *) hdr);
                                break;
                        case BLE_EVT_GAP_SCAN_COMPLETED:
                                handle_evt_gap_scan_completed((ble_evt_gap_scan_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_CONNECTED:
                                handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_CONNECTION_COMPLETED:
                                handle_evt_gap_connection_completed((ble_evt_gap_connection_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_DISCONNECTED:
                                handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_SECURITY_REQUEST:
                                handle_evt_gap_security_request((ble_evt_gap_security_request_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PASSKEY_NOTIFY:
                                handle_evt_gap_passkey_notify((ble_evt_gap_passkey_notify_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PAIR_COMPLETED:
                                handle_evt_gap_pair_completed((ble_evt_gap_pair_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_SEC_LEVEL_CHANGED:
                                handle_evt_gap_sec_level_changed((ble_evt_gap_sec_level_changed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_SET_SEC_LEVEL_FAILED:
                                handle_evt_gap_set_sec_level_failed((ble_evt_gap_set_sec_level_failed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_ADDRESS_RESOLVED:
                                handle_evt_gap_address_resolved((ble_evt_gap_address_resolved_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_BROWSE_SVC:
                                handle_evt_gattc_browse_svc((ble_evt_gattc_browse_svc_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_BROWSE_COMPLETED:
                                handle_evt_gattc_browse_completed((ble_evt_gattc_browse_completed_t *) hdr);
                                break;
                        default:
                                ble_handle_event_default(hdr);
                                break;
                        }

                        /* Free event buffer (it's not needed anymore) */
                        OS_FREE(hdr);

no_event:
                        /*
                         * If there are more events waiting in queue, application should process
                         * them now.
                         */
                        if (ble_has_event()) {
                                OS_TASK_NOTIFY(OS_GET_CURRENT_TASK(), BLE_APP_NOTIFY_MASK,
                                                                                OS_NOTIFY_SET_BITS);
                        }
                }

                if (notif & CLI_NOTIF) {
                        cli_handle_notified(cli);
                }

                if (notif & CPS_OPERATION_TMO_NOTIF) {
                        peer_info_t *peer_info;
                        uint16_t conn_idx;

                        if (OS_QUEUE_GET(peer_cps_tmo_queue, &conn_idx, OS_QUEUE_FOREVER) != OS_QUEUE_OK) {
                                return;
                        }

                        peer_info = find_peer_info(conn_idx);
                        if (!peer_info) {
                                return;
                        }

                        cps_client_timer_notif(peer_info->cps_client);
                }
        }
}
