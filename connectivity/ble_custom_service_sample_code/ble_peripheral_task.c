
/**
 ****************************************************************************************
 *
 * @file ble_peripheral_task.c
 *
 * @brief The BLE peripheral task
 *
 * Copyright (c) 2021 Dialog Semiconductor. All rights reserved.
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "osal.h"
#include "time.h"
#include "sys_watchdog.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_service.h"
#include "ble_uuid.h"
#include "ble_gattc.h"
#include "misc.h"
#include "ble_custom_service.h"

/* MTU value to be negotiated upon connection */
#define MTU_CHANGED_SIZE                        ( 125 )

/**
 * Max. attribute value size, expressed in bytes (for the custom characteristic attributes defined)
 *
 * \warning The remote device must not exceed the max. value defined. Otherwise, the system will crash!!!
 *
 */
#define CHARACTERISTIC_ATTR_VALUE_MAX_BYTES      ( 50 )

/* Notification characteristic OS timer timeout */
#define CHAR_NOTIF_TIMER_TIMEOUT_MS              ( 1000 )

/*
 * Notification bits reservation
 * bit #0 is always assigned to BLE event queue notification
 */
#define NOTIF_CHAR_NOTIF                ( 1 << 1 )

/* Dummy 128-bit UUIDs (used for the custom characteristics defined) */
#define CUSTOM_CHARACTERISTIC_1_UUID     NUM_TO_STRING(11111111-0000-0000-0000-000000000001)
#define CUSTOM_CHARACTERISTIC_2_UUID     NUM_TO_STRING(22222222-0000-0000-0000-000000000001)
#define CUSTOM_CHARACTERISTIC_3_UUID     NUM_TO_STRING(22222222-0000-0000-0000-000000000002)
#define CUSTOM_CHARACTERISTIC_4_UUID     NUM_TO_STRING(22222222-0000-0000-0000-000000000003)

/* Dummy 128-bit UUIDs (used for the custom services defined) */
#define CUSTOM_SERVICE_1_UUID            NUM_TO_STRING(11111111-0000-0000-0000-111111111111)
#define CUSTOM_SERVICE_2_UUID            NUM_TO_STRING(22222222-0000-0000-0000-222222222222)

/**
 * Attribute value of the first custom characteristic defined.
 *
 * \note Characteristic attribute values should be handled on application level by users.
 *
 * \note Similarly user can defined attribute values for the rest of characteristics defined.
 */
__RETAINED_RW uint8_t char_1_attr_val[CHARACTERISTIC_ATTR_VALUE_MAX_BYTES] = { 0 };

/* Notification characteristic OS timer handle */
__RETAINED static OS_TIMER char_notif_timer_h;

/* Task handle */
__RETAINED_RW static OS_TASK ble_task_handle = NULL;


/**
 * BLE peripheral advertising data
 */
static const uint8_t adv_data[] = {
        0x12, GAP_DATA_TYPE_LOCAL_NAME,
        'M', 'y', ' ', 'C', 'u', 's', 't', 'o', 'm', ' ', 'S', 'e', 'r', 'v', 'i', 'c', 'e'
};

/**
 *  Dummy notification messages sent to peer device after a specified time interval
 */
static const char *char_notif_msg_array[] = {
        "Char. notification round #1 !!!",
        "Char. notification round #2 !!!",
        "Char. notification round #3 !!!",
};

static const char *notif_type_array[] = {
        [GATT_EVENT_NOTIFICATION] = "NOTIFICATION",
        [GATT_EVENT_INDICATION] = "INDICATION"
};

/**
 * ATT read request user callback function This callback is fired when a peer device
 * issues a read request.
 *
 * \param [in] value: The value returned back to the peer device
 *
 * \param [in] length: The number of bytes to be read from \p value
 *
 * \warning: The callback function should have that specific prototype
 *
 * \warning: The BLE stack will not proceed with the next BLE event until the
 *           callback returns.
 */
static void get_char_value_cb(uint8_t **value, uint16_t *length)
{

        /* Return the requested data back to the peer device */
        *value  = char_1_attr_val;
        *length = sizeof(char_1_attr_val);

        /*
         * This is just for debugging purposes. UART is a slow interface
         * and will add significant delays to the BLE functionality.
         */
        DBG_PRINTF("\nRead callback function hit! - Returned value: %s\n\r", (char *)char_1_attr_val);
}

/**
 * ATT write request user callback function. This callback is fired when a peer device
 * issues a write request.
 *
 * \param [out] value:  The value written by the peer device.
 *
 * \param [out] length: Number of bytes written by the peer device
 *
 * \warning: The callback function should have that specific prototype
 *
 * \warning: The BLE stack will not proceed with the next BLE event until the
 *           callback returns.
 *
 */
static void set_char_value_cb(const uint8_t *value, uint16_t length)
{
        /* Clear the variable */
        memset((void *)char_1_attr_val, 0x20, sizeof(char_1_attr_val));

        /* Get the value written by the peer device */
        memcpy((void *)char_1_attr_val, (void *)value, length);

        /*
         * This is just for debugging purposes. UART is a slow interface
         * and will add significant delays to the BLE functionality.
         */
        DBG_PRINTF("\nWrite callback function hit! - Written value: %s, length: %d\n\r",
                                                                        char_1_attr_val, length);
}

/**
 * Notification event user callback function
 *
 * This callback function will be fired for each peer device that has explicitly
 * enabled notifications/indications (for the target characteristic attribute).
 *
 * \param [in] conn_idx: Connection index
 *
 * \param [in] status: The status of the aforementioned operation. 'True' when
 *                     notifications are executed successfully by CMAC,
 *                     otherwise 'false'.
 *
 * \param [in] type: Signifies whether a notification or indication operations
 *                   has been initiated.
 *
 * \warning: The BLE stack will not proceed with the next BLE event until the
 *           callback returns.
 */
static void event_sent_char_1_cb(uint16_t conn_idx, bool status, gatt_event_t type)
{
        /*
         * This is just for debugging purposes. UART is a slow interface
         * and will add significant delays to the BLE functionality.
         */
        DBG_PRINTF("\nNotify callback - Connection IDX: %d, Status: %d, Type: %s\n\r",
                                                                conn_idx, status, notif_type_array[type]);
}

static void event_sent_char_4_cb(uint16_t conn_idx, bool status, gatt_event_t type)
{
        /*
         * This is just for debugging purposes. UART is a slow interface
         * and will add significant delays to the BLE functionality.
         */
        DBG_PRINTF("\nNotify callback - Connection IDX: %d, Status: %d, Type: %s\n\r",
                                                                conn_idx, status, notif_type_array[type]);
}

/***************************** BLE event handlers *******************************/
static void handle_evt_gattc_mtu_changed(ble_evt_gattc_mtu_changed_t *evt)
{
        /*
         * This is just for debugging purposes. UART is a slow interface
         * and will add significant delays to the BLE functionality.
         */
        DBG_PRINTF("\n\rMTU CHANGED - CON IDX[%d] - MTU: %d\n\r", evt->conn_idx, evt->mtu);
}

static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        /*
         * Manage connection information
         */
        ble_gattc_exchange_mtu(evt->conn_idx);
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
        /*
         * Manage disconnection information
         */
}

static void handle_evt_gap_adv_completed(ble_evt_gap_adv_completed_t *evt)
{
        /* restart advertising so we can connect again */
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

/**
 *  Notification OS timer callback function.
 *
 *  \note Used to notify the peripheral task that the OS timer has been expired so,
 *        notifications are sent to the peer devices connected.
 *
 */
static void char_notif_timer_cb(OS_TIMER timer)
{
        OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);

        if (task) {
                OS_TASK_NOTIFY(task, NOTIF_CHAR_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

/* Peripheral OS task */
void ble_peripheral_task(void *params)
{
        int8_t wdog_id;

        DBG_PRINTF("\n\n\r*** Custom BLE Service Framework Demonstration ***\n\n\r");

        /* register ble_peripheral task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        /* Get task's handler */
        ble_task_handle = OS_GET_CURRENT_TASK();

        srand(time(NULL));

        ble_peripheral_start();
        ble_register_app();

        ble_gap_device_name_set("Custom BLE Service", ATT_PERM_READ);
        ble_gap_mtu_size_set(MTU_CHANGED_SIZE);


        //************ Characteristic declarations for the 1st custom BLE Service  *************
        const mcs_characteristic_config_t custom_service_1[] = {

                /* Initialized Characteristic Attribute */
                CHARACTERISTIC_DECLARATION(CUSTOM_CHARACTERISTIC_1_UUID, CHARACTERISTIC_ATTR_VALUE_MAX_BYTES,
                                          GATT_PROP_WRITE | GATT_PROP_READ | GATT_PROP_NOTIFY, ATT_PERM_RW,
                                                               NUM_TO_STRING(Service #1 Characteristic #1),
                                                        get_char_value_cb, set_char_value_cb, event_sent_char_1_cb),


                 // -----------------------------------------------------------------
                 // -- Here you can continue adding more Characteristic Attributes --
                 // -----------------------------------------------------------------


        };
        // ***************** Register the Bluetooth Service in Dialog BLE framework *****************
        SERVICE_DECLARATION(custom_service_1, CUSTOM_SERVICE_1_UUID)


        //************ Characteristic declarations for the 2nd BLE Service *************
        const mcs_characteristic_config_t custom_service_2[] = {

                /* Uninitialized Characteristic Attribute. You can defined your preferred settings */
                CHARACTERISTIC_DECLARATION(CUSTOM_CHARACTERISTIC_2_UUID, 0, GATT_PROP_NONE, ATT_PERM_NONE,
                                                                       NUM_TO_STRING(NULL), NULL, NULL, NULL),


                /* Uninitialized Characteristic Attribute. You can defined your preferred settings */
                CHARACTERISTIC_DECLARATION(CUSTOM_CHARACTERISTIC_3_UUID, 0, GATT_PROP_NONE, ATT_PERM_NONE,
                                                                       NUM_TO_STRING(NULL), NULL, NULL, NULL),


               /* Uninitialized Characteristic Attribute. You can defined your preferred settings */
                CHARACTERISTIC_DECLARATION(CUSTOM_CHARACTERISTIC_4_UUID, CHARACTERISTIC_ATTR_VALUE_MAX_BYTES,
                                                                           GATT_PROP_INDICATE, ATT_PERM_NONE,
                                                                 NUM_TO_STRING(Service #2 Characteristic #4),
                                                                                           NULL, NULL, event_sent_char_4_cb),


                // -----------------------------------------------------------------
                // -- Here you can continue adding more Characteristic attributes --
                // -----------------------------------------------------------------

        };
        // ****************** Register the Bluetooth Service in Dialog BLE framework *****************
        SERVICE_DECLARATION(custom_service_2, CUSTOM_SERVICE_2_UUID)

        ble_gap_tx_power_set(GAP_AIR_OP_ADV, GAP_TX_POWER_MAX);
        ble_gap_adv_data_set(sizeof(adv_data), adv_data, 0, NULL);
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        /* Create timer to notify peer devices at specified time intervals */
        char_notif_timer_h = OS_TIMER_CREATE("CHAR NOTIF", OS_MS_2_TICKS(CHAR_NOTIF_TIMER_TIMEOUT_MS), true,
                                                        (void *) OS_GET_CURRENT_TASK(), char_notif_timer_cb);
        ASSERT_WARNING(char_notif_timer_h);
        OS_TIMER_START(char_notif_timer_h, OS_TIMER_FOREVER);

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

                        if (ble_service_handle_event(hdr)) {
                                goto handled;
                        }

                        switch (hdr->evt_code) {
                        case BLE_EVT_GAP_CONNECTED:
                                handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_ADV_COMPLETED:
                                handle_evt_gap_adv_completed((ble_evt_gap_adv_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_DISCONNECTED:
                                handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PAIR_REQ:
                        {
                                ble_evt_gap_pair_req_t *evt = (ble_evt_gap_pair_req_t *) hdr;
                                ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
                                break;
                        }
                        case BLE_EVT_GATTC_MTU_CHANGED:
                                handle_evt_gattc_mtu_changed((ble_evt_gattc_mtu_changed_t *) hdr);
                                break;
                        default:
                                ble_handle_event_default(hdr);
                                break;
                        }

handled:
                        OS_FREE(hdr);

no_event:
                        // notify again if there are more events to process in queue
                        if (ble_has_event()) {
                                OS_TASK_NOTIFY(OS_GET_CURRENT_TASK(), BLE_APP_NOTIFY_MASK, eSetBits);
                        }

                }

                if (notif & NOTIF_CHAR_NOTIF) {
                        static int idx = 0;
                        mcs_send_notifications(CUSTOM_CHARACTERISTIC_1_UUID,
                             (const uint8_t *)char_notif_msg_array[idx], strlen(char_notif_msg_array[idx]));

                        mcs_send_notifications(CUSTOM_CHARACTERISTIC_4_UUID,
                             (const uint8_t *)char_notif_msg_array[idx], strlen(char_notif_msg_array[idx]));

                        if (++idx >= ARRAY_LENGTH(char_notif_msg_array)) {
                                idx = 0;
                        }
                }
        }
}
