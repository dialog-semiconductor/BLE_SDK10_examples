/**
 ****************************************************************************************
 *
 * @file ble_central_task.c
 *
 * @brief BLE central task
 *
 * Copyright (C) 2015-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "osal.h"
#include "hw_uart.h"
#include "sys_watchdog.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_config.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_gattc.h"
#include "ble_uuid.h"
#include "ble_central_config.h"
#include "sdk_queue.h"

/*
 * Notification bits reservation
 * bit #0 is always assigned to BLE event queue notification
 */
#define DISCOVER_NOTIF                          (1 << 1)
#define RECONNECT_NOTIF                         (1 << 2)
#define WRITE_NO_RESPONSE_T_NOTIF               (1 << 3)
#define START_MEASURE_T_NOTIF                   (1 << 4)

/*
 * Desired connection interval after service browse action (in milliseconds)
 */
#define USER_CONNECTION_INTERVAL_MS             (500)

/*
 * BDAddress to automatically connect to
 */
#define customBLE_STATIC_ADDRESS                {0x01, 0x01, 0x01, 0x06, 0x06, 0x06}

/*
 * Read Mode   : Disabled (0), Enabled(1)
 */
#define USER_READ_BLE_OP_EN                     (1)

/*
 * Write Mode   : Disabled (0), Enabled(1)
 */
#define USER_WRITE_BLE_OP_EN                    (0)

/*
 * Write No Response Mode   : Disabled (0), Enabled(1)
 */
#define USER_WRITE_NO_RESPONSE_BLE_OP_EN        (0)

/*
 * Notifications/Indications Request : Disabled (0), Enabled(1)
 */
#define USER_BLE_NOTIFICATIONS_EN               (1)

/*
 * Suppress Messages Mode : Disabled (0), Enabled(1)
 */
#define USER_SUPPRESS_LOGS_EN                   (1)

/*
 * Maximum MTU allowed on the central.
 * Maximum Allowed Value : 512
 * Minimum Value         : 65
 * If a value lower than 65 is assigned, the default value of 65 Bytes will be applied
 */
#define USER_MTU                                (512)

#if( USER_READ_BLE_OP_EN || USER_WRITE_NO_RESPONSE_BLE_OP_EN || USER_WRITE_BLE_OP_EN )
__RETAINED_RW static uint16_t first_char_value_handle = 0x0b;
#endif

#if(USER_WRITE_NO_RESPONSE_BLE_OP_EN || USER_WRITE_BLE_OP_EN )
        /*
         * The value to be written to the characteristic.
         */
		uint8_t written_value[] = { 'A', ' ', 's' ,'t', 'r', 'i', 'n', 'g', ' ', 'o', 'f', ' ', '2', '0', ' ', 'c', 'h', 'a', 'r', 's'} ;
#endif

__RETAINED OS_TIMER start_measurement_timer_h;

#if(USER_WRITE_NO_RESPONSE_BLE_OP_EN == 1)
        __RETAINED OS_TIMER write_no_response_timer_h;
#endif
#if (!CFG_USE_BROWSE_API)
typedef struct {
        void *next;
        uint16_t start_h;
        uint16_t end_h;
} service_t;

typedef struct {
        void *next;
        uint16_t handle;
        uint16_t val_h;
} characteristic_t;

__RETAINED static queue_t services;
__RETAINED static queue_t characteristics;
#endif

/* Flag byte to ble operations */
__RETAINED static uint8_t start_ble_operations_flag;

__RETAINED static OS_TASK ble_central_task_handle;

__RETAINED static uint16_t devname_val_h;

__RETAINED_RW static uint16_t mtu_size = 1;

/* return static buffer with formatted address */
static const char *format_bd_address(const bd_address_t *addr)
{
        static char buf[19];
        int i;

        for (i = 0; i < sizeof(addr->addr); i++) {
                int idx;

                // for printout, address should be reversed
                idx = sizeof(addr->addr) - i - 1;
                sprintf(&buf[i * 3], "%02x:", addr->addr[idx]);
        }

        buf[sizeof(buf) - 2] = '\0';

        return buf;
}

/* return static buffer with formatted UUID */
static const char *format_uuid(const att_uuid_t *uuid)
{
        static char buf[37];

        if (uuid->type == ATT_UUID_16) {
                sprintf(buf, "0x%04x", uuid->uuid16);
        } else {
                int i;
                int idx = 0;

                for (i = ATT_UUID_LENGTH; i > 0; i--) {
                        if (i == 12 || i == 10 || i == 8 || i == 6) {
                                buf[idx++] = '-';
                        }

                        idx += sprintf(&buf[idx], "%02x", uuid->uuid128[i - 1]);
                }
        }

        return buf;
}

/* return static buffer with characteristics properties mask */
static const char *format_properties(uint8_t properties)
{
        static const char props_str[] = "BRXWNISE"; // each letter corresponds to single property
        static char buf[9];
        int i;

        // copy full properties mask
        memcpy(buf, props_str, sizeof(props_str));

        for (i = 0; i < 8; i++) {
                // clear letter from mask if property not present
                if ((properties & (1 << i)) == 0) {
                        buf[i] = '-';
                }
        }

        return buf;
}

/* print buffer in formatted hexdump and ASCII */
static void format_value(uint16_t length, const uint8_t *value)
{
        static char buf1[49]; // buffer for hexdump (16 * 3 chars + \0)
        static char buf2[17]; // buffer for ASCII (16 chars + \0)

        while (length) {
                int i;

                memset(buf1, 0, sizeof(buf1));
                memset(buf2, 0, sizeof(buf2));

                for (i = 0; i < 16 && length > 0; i++, length--, value++) {
                        sprintf(&buf1[i * 3], "%02x ", (int) *value);

                        // any character outside standard ASCII is presented as '.'
                        if (*value < 32 || *value > 127) {
                                buf2[i] = '.';
                        } else {
                                buf2[i] = *value;
                        }
                }

                printf("\r\n\t%-49s %-17s\r\n\r\n", buf1, buf2);
        }
}

static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        //
        printf("%s: conn_idx=%04x\r\n", __func__, evt->conn_idx);

        const gap_conn_params_t cp = {
                .interval_min = BLE_CONN_INTERVAL_FROM_MS(40),
                .interval_max = BLE_CONN_INTERVAL_FROM_MS(60),
                .slave_latency = 0,
                .sup_timeout = BLE_SUPERVISION_TMO_FROM_MS(400),
        };

        ble_gap_conn_param_update( 0x0,&cp);
        ble_gattc_exchange_mtu(0x0);

        // notify main thread, we'll start discovery from there
        OS_TASK_NOTIFY(ble_central_task_handle, DISCOVER_NOTIF, eSetBits);
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
#if(USER_WRITE_NO_RESPONSE_BLE_OP_EN == 1)

        OS_TIMER_STOP(write_no_response_timer_h,OS_TIMER_FOREVER);
#endif
        printf("%s: conn_idx=%04x address=%s reason=%d\r\n", __func__, evt->conn_idx,
                                                format_bd_address(&evt->address), evt->reason);

#if (!CFG_USE_BROWSE_API)
        queue_remove_all(&services, OS_FREE_FUNC);
        queue_remove_all(&characteristics, OS_FREE_FUNC);
#endif

        // notify main thread, we'll start reconnect from there
        OS_TASK_NOTIFY(ble_central_task_handle, RECONNECT_NOTIF, eSetBits);
}

static void handle_evt_gap_security_request(ble_evt_gap_security_request_t *evt)
{
        printf("%s: conn_idx=%04x bond=%d\r\n", __func__, evt->conn_idx, evt->bond);

        // trigger pairing
        ble_gap_pair(evt->conn_idx, evt->bond);
}

static void handle_evt_gap_pair_completed(ble_evt_gap_pair_completed_t *evt)
{
        printf("%s: conn_idx=%04x status=%d bond=%d mitm=%d\r\n", __func__, evt->conn_idx,
                                                                evt->status, evt->bond, evt->mitm);
}

#if CFG_USE_BROWSE_API
static void handle_evt_gattc_browse_svc(ble_evt_gattc_browse_svc_t *evt)
{
#if (USER_BLE_NOTIFICATIONS_EN == 1)
        uint8_t prop = 0;
#endif
        int i;

        printf("%s: conn_idx=%04x start_h=%04x end_h=%04x\r\n", __func__, evt->conn_idx,
                                                                        evt->start_h, evt->end_h);

        printf("\t%04x serv %s\r\n", evt->start_h, format_uuid(&evt->uuid));

        for (i = 0; i < evt->num_items; i++) {
                gattc_item_t *item = &evt->items[i];
                att_uuid_t uuid;

                switch (item->type) {
                case GATTC_ITEM_TYPE_INCLUDE:
                        printf("\t%04x incl %s\r\n", item->handle, format_uuid(&item->uuid));
                        break;
                case GATTC_ITEM_TYPE_CHARACTERISTIC:
                        printf("\t%04x char %s prop=%02x (%s)\r\n", item->handle,
                                                format_uuid(&evt->uuid), item->c.properties,
                                                format_properties(item->c.properties));

                        printf("\t%04x ---- %s\r\n", item->c.value_handle, format_uuid(&item->uuid));

#if(USER_BLE_NOTIFICATIONS_EN == 1)
                        prop = item->c.properties;
#endif
                        break;
                case GATTC_ITEM_TYPE_DESCRIPTOR:
                        printf("\t%04x desc %s\r\n", item->handle, format_uuid(&item->uuid));

                        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
                        if (ble_uuid_equal(&uuid, &item->uuid)) {
#if(USER_BLE_NOTIFICATIONS_EN == 1)
                                if (prop & GATT_PROP_NOTIFY) {
                                        uint16_t ccc = GATT_CCC_NOTIFICATIONS;
                                        ble_gattc_write(evt->conn_idx, item->handle, 0,
                                                                        sizeof(ccc), (uint8_t *) &ccc);
                                }

                                if (prop & GATT_PROP_INDICATE) {
                                        uint16_t ccc = GATT_CCC_INDICATIONS;
                                        ble_gattc_write(evt->conn_idx, item->handle, 0,
                                                                        sizeof(ccc), (uint8_t *) &ccc);
                                }
#endif
                        }
                        break;
                default:
                        printf("\t%04x ????\r\n", item->handle);
                        break;
                }
        }
}

static void handle_evt_gattc_browse_completed(ble_evt_gattc_browse_completed_t *evt)
{
        printf("%s: conn_idx=%04x status=%d\r\n", __func__, evt->conn_idx, evt->status);

        /*Connection Parameters after service browse is completed*/
        const gap_conn_params_t svc_browse_completed_conn_params = {
                .interval_min = BLE_CONN_INTERVAL_FROM_MS(USER_CONNECTION_INTERVAL_MS-10),
                .interval_max = BLE_CONN_INTERVAL_FROM_MS(USER_CONNECTION_INTERVAL_MS),
                .slave_latency = 0,
                .sup_timeout = BLE_SUPERVISION_TMO_FROM_MS(USER_CONNECTION_INTERVAL_MS*4),
        };

        ble_error_t err = ble_gap_conn_param_update( 0x0,&svc_browse_completed_conn_params);
        OS_ASSERT(err == BLE_STATUS_OK);
        printf("\n\rParameters Changed\n\r");

}

#else
static void handle_evt_gattc_discover_svc(ble_evt_gattc_discover_svc_t *evt)
{
        service_t *service;

        printf("%s: conn_idx=%04x uuid=%s start_h=%04x end_h=%04x\r\n", __func__, evt->conn_idx,
                                                format_uuid(&evt->uuid), evt->start_h, evt->end_h);

        service = OS_MALLOC(sizeof(*service));
        service->start_h = evt->start_h;
        service->end_h = evt->end_h;

        queue_push_back(&services, service);
}

static void handle_evt_gattc_discover_char(ble_evt_gattc_discover_char_t *evt)
{
        characteristic_t *characteristic;
        att_uuid_t uuid;

        printf("%s: conn_idx=%04x uuid=%s handle=%04x value_handle=%04x properties=%02x (%s)\r\n",
                __func__, evt->conn_idx, format_uuid(&evt->uuid), evt->handle, evt->value_handle,
                evt->properties, format_properties(evt->properties));

        ble_uuid_create16(0x2a00, &uuid); // Device Name
        if (ble_uuid_equal(&uuid, &evt->uuid)) {
                // store handle if Device Name is writable - once read is completed we'll write new
                // value there and read again
                if (evt->properties & GATT_PROP_WRITE) {
                        devname_val_h = evt->value_handle;
                }

                ble_gattc_read(evt->conn_idx, evt->value_handle, 0);
        }

        characteristic = OS_MALLOC(sizeof(*characteristic));
        characteristic->handle = evt->handle;
        characteristic->val_h = evt->value_handle;

        queue_push_back(&characteristics, characteristic);
}

static void handle_evt_gattc_discover_desc(ble_evt_gattc_discover_desc_t *evt)
{
        printf("%s: conn_idx=%04x uuid=%s handle=%04x\r\n", __func__, evt->conn_idx,
                                                        format_uuid(&evt->uuid), evt->handle);
}

static void handle_evt_gattc_discover_completed(ble_evt_gattc_discover_completed_t *evt)
{
        service_t *service;

        printf("%s: conn_idx=%04x type=%d status=%d\r\n", __func__, evt->conn_idx, evt->type,
                                                                                        evt->status);


        service = queue_peek_front(&services);

        if (evt->type == GATTC_DISCOVERY_TYPE_SVC && service) {
                ble_gattc_discover_char(evt->conn_idx, service->start_h, service->end_h, NULL);
        } else if (evt->type == GATTC_DISCOVERY_TYPE_CHARACTERISTICS && service) {
                characteristic_t *charac, *next = NULL;

                for (charac = queue_peek_front(&characteristics); charac; charac = next) {
                        next = charac->next;

                        /*
                         * Check if there is enough room for at least one descriptor.
                         * Range start from next handle after characteristic value handle,
                         * ends before next characteristic or service's end handle
                         */
                        if (charac->val_h < (next ? next->handle - 1 : service->end_h)) {
                                ble_gattc_discover_desc(evt->conn_idx, charac->val_h + 1, next ?
                                                        next->handle - 1 : service->end_h);
                        }
                }

                queue_remove_all(&characteristics, OS_FREE_FUNC);
                queue_pop_front(&services);
                OS_FREE(service);

                service = queue_peek_front(&services);
                if (service) {
                        ble_gattc_discover_char(evt->conn_idx, service->start_h,
                                                                        service->end_h, NULL);
                }
        }
}
#endif

static void handle_evt_gattc_read_completed(ble_evt_gattc_read_completed_t *evt)
{
#if CFG_UPDATE_NAME
        static bool devname_written = false;
#endif

#if (USER_SUPPRESS_LOGS_EN == 0)
        printf("%s: conn_idx=%04x handle=%04x status=%d\r\n", __func__, evt->conn_idx, evt->handle,
                                                                                        evt->status);
        if (evt->status == ATT_ERROR_OK) {
                format_value(evt->length, evt->value);
        }
#endif

#if (USER_READ_BLE_OP_EN == 1)
                if(start_ble_operations_flag)
                {
                        ble_gattc_read(0x0,first_char_value_handle,0x0);
                }
#endif

#if CFG_UPDATE_NAME
        if (evt->handle == devname_val_h && !devname_written) {
                static const uint8_t name[] = "DA1468x was here!";
                devname_written = true;
                ble_gattc_write(evt->conn_idx, evt->handle, 0, sizeof(name) - 1, name);
        }
#endif
}

static void handle_evt_gattc_write_completed(ble_evt_gattc_write_completed_t *evt)
{
#if (USER_SUPPRESS_LOGS_EN == 0)
        printf("%s: conn_idx=%04x handle=%04x status=%d\r\n", __func__, evt->conn_idx, evt->handle,
                                                                                        evt->status);
#endif

        if (evt->handle == devname_val_h) {
                ble_gattc_read(evt->conn_idx, evt->handle, 0);
        }

#if (USER_WRITE_BLE_OP_EN == 1)
                if(start_ble_operations_flag)
                {
                        ble_gattc_write(0x0, first_char_value_handle, 0, sizeof(written_value), written_value);
                }
#endif
#if (USER_WRITE_NO_RESPONSE_BLE_OP_EN == 1)
                if(start_ble_operations_flag)
                {
                        OS_TIMER_START(write_no_response_timer_h, OS_TIMER_FOREVER);
                }
#endif

}

static void handle_evt_gattc_notification(ble_evt_gattc_notification_t *evt)
{

        printf("%s: conn_idx=%04x handle=%04x length=%d\r\n", __func__, evt->conn_idx, evt->handle,
                                                                                       evt->length);

        format_value(evt->length, evt->value);
}

static void handle_evt_gattc_indication(ble_evt_gattc_indication_t *evt)
{
        printf("%s: conn_idx=%04x handle=%04x length=%d\r\n", __func__, evt->conn_idx, evt->handle,
                                                                                       evt->length);

        format_value(evt->length, evt->value);
}

static void handle_evt_gattc_mtu_changed(ble_evt_gattc_mtu_changed_t* evt)
{
        mtu_size = evt->mtu;
        printf("\n\n\n%s: conn_idx=%04x new MTU=%d Bytes\r\n\n\n", __func__, evt->conn_idx, evt->mtu);
}


#if(USER_WRITE_NO_RESPONSE_BLE_OP_EN == 1)
static void write_no_response_timer_cb(OS_TIMER xTimer)
{

        OS_TASK_NOTIFY(ble_central_task_handle, WRITE_NO_RESPONSE_T_NOTIF, eSetBits);

}
#endif


static void start_measurement_timer_cb(OS_TIMER xTimer)
{
        start_ble_operations_flag = 1;
        OS_TASK_NOTIFY(ble_central_task_handle, START_MEASURE_T_NOTIF, eSetBits);
}


void ble_central_task(void *params)
{
        int8_t wdog_id;
        const bd_address_t addr = {
                .addr_type = PUBLIC_ADDRESS,
                .addr = customBLE_STATIC_ADDRESS ,    // set custom BD address
        };

        const gap_conn_params_t cp = {
                        .interval_min = BLE_CONN_INTERVAL_FROM_MS(40),
                        .interval_max = BLE_CONN_INTERVAL_FROM_MS(60),
                        .slave_latency = 0,
                        .sup_timeout = BLE_SUPERVISION_TMO_FROM_MS(400),
                };

        ble_central_task_handle = OS_GET_CURRENT_TASK();

        /* register ble_central task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

#if (!CFG_USE_BROWSE_API)
        queue_init(&services);
        queue_init(&characteristics);
#endif

        ble_central_start();
        ble_register_app();

        ble_gap_mtu_size_set(USER_MTU);

        ble_gap_mtu_size_get(&mtu_size);
        printf("Device Max MTU: %d \n\r", mtu_size);

        printf("\n\n\rConnection Interval: %d milliseconds\n\n\r", USER_CONNECTION_INTERVAL_MS);

        printf("BLE Central application started\r\n");

        ble_gap_connect(&addr, &cp);



#if (USER_WRITE_NO_RESPONSE_BLE_OP_EN == 1)
        /*Create the timer for writing to the characteristic*/
        write_no_response_timer_h=OS_TIMER_CREATE("write_no_response_trigger", OS_MS_2_TICKS(USER_CONNECTION_INTERVAL_MS),
                OS_TIMER_SUCCESS, (void *)OS_GET_CURRENT_TASK(),
                write_no_response_timer_cb);
#endif

        start_measurement_timer_h=OS_TIMER_CREATE("start_measurement", OS_MS_2_TICKS(30000),
                OS_TIMER_SUCCESS, (void *)OS_GET_CURRENT_TASK(),
                start_measurement_timer_cb);

        OS_TIMER_START(start_measurement_timer_h, OS_TIMER_FOREVER);

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /* notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for task notification. The return value must be OS_OK */
                OS_ASSERT(ret == OS_OK);

                /* resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                /* notified from BLE manager, can get event */
                if (notif & BLE_APP_NOTIFY_MASK) {
                        ble_evt_hdr_t *hdr;

                        hdr = ble_get_event(false);
                        if (!hdr) {
                                goto no_event;
                        }

                        switch (hdr->evt_code) {
                        case BLE_EVT_GAP_CONNECTED:
                                handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_DISCONNECTED:
                                handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_SECURITY_REQUEST:
                                handle_evt_gap_security_request((ble_evt_gap_security_request_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PAIR_COMPLETED:
                                handle_evt_gap_pair_completed((ble_evt_gap_pair_completed_t *) hdr);
                                break;
#if CFG_USE_BROWSE_API
                        case BLE_EVT_GATTC_BROWSE_SVC:
                                handle_evt_gattc_browse_svc((ble_evt_gattc_browse_svc_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_BROWSE_COMPLETED:
                                handle_evt_gattc_browse_completed((ble_evt_gattc_browse_completed_t *) hdr);
                                break;
#else
                        case BLE_EVT_GATTC_DISCOVER_SVC:
                                handle_evt_gattc_discover_svc((ble_evt_gattc_discover_svc_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_DISCOVER_CHAR:
                                handle_evt_gattc_discover_char((ble_evt_gattc_discover_char_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_DISCOVER_DESC:
                                handle_evt_gattc_discover_desc((ble_evt_gattc_discover_desc_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_DISCOVER_COMPLETED:
                                handle_evt_gattc_discover_completed((ble_evt_gattc_discover_completed_t *) hdr);
                                break;
#endif
                        case BLE_EVT_GATTC_READ_COMPLETED:
                                handle_evt_gattc_read_completed((ble_evt_gattc_read_completed_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_WRITE_COMPLETED:
                                handle_evt_gattc_write_completed((ble_evt_gattc_write_completed_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_NOTIFICATION:
                                handle_evt_gattc_notification((ble_evt_gattc_notification_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_INDICATION:
                                handle_evt_gattc_indication((ble_evt_gattc_indication_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_MTU_CHANGED:
                                handle_evt_gattc_mtu_changed((ble_evt_gattc_mtu_changed_t *) hdr);
                                break;
                        default:
                                ble_handle_event_default(hdr);
                                break;
                        }

                        OS_FREE(hdr);

no_event:
                        // notify again if there are more events to process in queue
                        if (ble_has_event()) {
                                OS_TASK_NOTIFY(OS_GET_CURRENT_TASK(), BLE_APP_NOTIFY_MASK, eSetBits);
                        }
                }

                if (notif & DISCOVER_NOTIF) {
#if CFG_USE_BROWSE_API
                        ble_gattc_browse(0, NULL);
#else
                        ble_gattc_discover_svc(0, NULL);
#endif
                }

                if (notif & RECONNECT_NOTIF) {
                        ble_gap_connect(&addr, &cp);
                }

#if(USER_WRITE_NO_RESPONSE_BLE_OP_EN == 1)
                if (notif & WRITE_NO_RESPONSE_T_NOTIF)
                {
                        /* Write the first characteristic of the Service*/
                        ble_error_t err = ble_gattc_write_no_resp(0x0,first_char_value_handle, false, sizeof(written_value), written_value);
                        OS_ASSERT(err == BLE_STATUS_OK);

                        OS_TIMER_STOP(write_no_response_timer_h,OS_TIMER_FOREVER);
                }
#endif

                if (notif & START_MEASURE_T_NOTIF)
                {
                        OS_TIMER_STOP(start_measurement_timer_h, OS_TIMER_FOREVER);
                        printf("\n\n=============Start Measuring=============\n\n\r");

#if(USER_READ_BLE_OP_EN == 1)
                        ble_gattc_read(0x0,first_char_value_handle,0x0);
#endif
#if(USER_WRITE_BLE_OP_EN == 1)
                        ble_gattc_write(0x0, first_char_value_handle, 0, sizeof(written_value), written_value);
#endif

#if(USER_WRITE_NO_RESPONSE_BLE_OP_EN == 1)

                        ble_gattc_write_no_resp(0x0,first_char_value_handle, false, sizeof(written_value), written_value);
#endif
                }
        }
}
