/**
 ****************************************************************************************
 *
 * @file ble_queue_task.c
 *
 * @brief BLE queue custom profile
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
#if defined (LOGGING_MODE_QUEUE)

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "ble_att.h"
#include "ble_common.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_service.h"
#include "ble_uuid.h"
#include "osal.h"
#include "logging.h"
#include "sys_watchdog.h"
#include "ble_bufops.h"
#include "ble_storage.h"
#include "ble_gattc.h"

/* Define the maximum MTU that will be used from the application */
#define APP_MAX_MTU_SIZE                512
/* The rate at which the device checks and send notifications */
#define send_ntf_log_FREQUENCY_MS       OS_MS_2_TICKS(1000)
/*
 * BLE adv demo advertising data
 */
const uint8_t adv_data[] = {
        0x03, GAP_DATA_TYPE_UUID16_LIST,
        0x01, 0x23,
        0x0C, GAP_DATA_TYPE_LOCAL_NAME,
        'B','L','E',' ','L', 'o', 'g', 'g', 'i', 'n', 'g'
};

__RETAINED static OS_TIMER send_ntf_timer;

extern OS_QUEUE xLogQueue;                      // queue holding all the logging data
extern  uint32_t suppressed_messages;           // variable holding the suppressed messages
extern logging_severity_e logging_min_severity; // variable holding the current min severity

/* Characteristic value */
__RETAINED_RW uint8_t log_char_val = 0;

/* Logging service handle */
__RETAINED ble_service_t *log_svc_hnd;

static const char char_descr_log[]              = "Log Output";
static const char char_descr_disc_log_cnt[]     = "Discarded Log Count";
static const char char_descr_severity[]         = "Severity";

/* Callback functions */
typedef void (* queue_get_value_cb_t) (ble_service_t *svc, uint16_t conn_idx);

typedef void (* queue_set_value_cb_t) (ble_service_t *svc, uint16_t conn_idx, const uint8_t *value);


typedef struct {
        queue_get_value_cb_t read_log;
        queue_get_value_cb_t read_dis_cnt;
        queue_get_value_cb_t read_severity;
        queue_set_value_cb_t write_severity;
}logging_queue_service_cb_t;

typedef struct {
        ble_service_t                           svc;
        const logging_queue_service_cb_t        *cb;
        // Attribute handles of BLE service
        uint16_t                                log_char_value_h;
        uint16_t                                log_char_value_ccc_h;
        uint16_t                                dis_log_cnt_value_h;
        uint16_t                                severity_char_value_h;

}logging_queue_service_t;

/* Handler for read requests */
static void log_get_char_val_cb(ble_service_t *svc, uint16_t conn_idx)
{

}

static void dis_cnt_get_val_cb(ble_service_t *svc, uint16_t conn_idx)
{
        logging_queue_service_t *log = (logging_queue_service_t *)svc;
        ble_gatts_read_cfm(conn_idx, log->dis_log_cnt_value_h, ATT_ERROR_OK, sizeof(uint32_t), &suppressed_messages);
}

static void severity_get_val_cb(ble_service_t *svc, uint16_t conn_idx)
{
        logging_queue_service_t *log = (logging_queue_service_t *)svc;
        ble_gatts_read_cfm(conn_idx, log->severity_char_value_h, ATT_ERROR_OK, sizeof(logging_severity_e), &logging_min_severity);
}

/* Handler for write requests */
static void severity_set_val_cb(ble_service_t *svc, uint16_t conn_idx, const uint8_t *value)
{
        if ((logging_severity_e)*value > LOG_CRITICAL)
                log_set_severity(LOG_CRITICAL);
        else
                log_set_severity((logging_severity_e)*value);
}

/* Declare callback functions for specific BLE events */
static const logging_queue_service_cb_t log_callbacks = {
        .read_log = log_get_char_val_cb,
        .read_dis_cnt = dis_cnt_get_val_cb,
        .read_severity = severity_get_val_cb,
        .write_severity = severity_set_val_cb,
};

/* This function is called upon write requests to CCC attribute value */
static att_error_t do_char_value_ccc_write(logging_queue_service_t *log, uint16_t conn_idx,
                              uint16_t offset, uint16_t length, const uint8_t *value)
{
        uint16_t ccc;

        if (offset) {
                return ATT_ERROR_ATTRIBUTE_NOT_LONG;
        }

        if (length != sizeof(ccc)) {
                return ATT_ERROR_INVALID_VALUE_LENGTH;
        }

        ccc = get_u16(value);

        /* Store the envoy CCC value to the ble storage */
        ble_storage_put_u32(conn_idx, log->log_char_value_ccc_h, ccc, true);

        return ATT_ERROR_OK;
}

/* This function is called upon read requests to characteristic attribute value */
static void do_char_value_read(logging_queue_service_t *log, const ble_evt_gatts_read_req_t *evt)
{
        /* Check whether the application has defined a callback function for handling the event. */
        /* The application should provide the requested data to the peer device. */
        if(evt->handle == log->log_char_value_h && log->cb->read_log) {
                log->cb->read_log(&log->svc, evt->conn_idx);
        }
        else if (evt->handle == log->dis_log_cnt_value_h && log->cb->read_dis_cnt) {
                log->cb->read_dis_cnt(&log->svc, evt->conn_idx);
        }
        else if (evt->handle == log->severity_char_value_h && log->cb->read_severity) {
                log->cb->read_severity(&log->svc, evt->conn_idx);
        }
        else {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
        }
}

static att_error_t do_char_value_write(logging_queue_service_t *log, const ble_evt_gatts_write_req_t *evt)
{
        if(evt->handle == log->severity_char_value_h && log->cb->write_severity)
        {
                log->cb->write_severity(&log->svc, evt->conn_idx, evt->value);
                return ATT_ERROR_OK;
        }

        return ATT_ERROR_INVALID_HANDLE;
}

/* Notify the peer device that characteristic attribute value has been updated */
bool log_notify_char_value(ble_service_t *svc, uint16_t conn_idx, uint8_t *value, uint8_t size)
{
        logging_queue_service_t *log = (logging_queue_service_t *) svc;

        uint16_t ccc = 0x0000;

        ble_storage_get_u16(conn_idx, log->log_char_value_ccc_h, &ccc);


        /*
         * Check if the notifications are enabled from the peer device,
         * otherwise don't send anything.
         */
        if (!(ccc & GATT_CCC_NOTIFICATIONS)) {
                return false;
        }

        ble_gatts_send_event(conn_idx, log->log_char_value_h, GATT_EVENT_NOTIFICATION, size, value);

        return true;
}

/*
 * This function should be called by the application as a response to write requests
 */
void log_set_char_value_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status)
{
        logging_queue_service_t *log = (logging_queue_service_t *) svc;

        /* This function should be used as a response for every write request */
        ble_gatts_write_cfm(conn_idx, log->log_char_value_h, status);
}

/* Handler for read requests, that is BLE_EVT_GATTS_READ_REQ */
static void handle_read_req(ble_service_t *svc, const ble_evt_gatts_read_req_t *evt)
{
        logging_queue_service_t *log = (logging_queue_service_t *) svc;

        /*
         * Identify for which attribute handle the read request has been sent to
         * and call the appropriate function.
         */

        /* If the read is for one of the two readable characteristics */
        if (evt->handle == log->log_char_value_h || evt->handle == log->dis_log_cnt_value_h || evt->handle == log->severity_char_value_h) {
                do_char_value_read(log, evt);
        /* Else if the read is for the ccc descriptor */
        } else if (evt->handle == log->log_char_value_ccc_h) {
                uint16_t ccc = 0x0000;

                /* Extract the CCC value from the ble storage */
                ble_storage_get_u16(evt->conn_idx, log->log_char_value_ccc_h, &ccc);

                // We're little-endian - OK to write directly from uint16_t
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_OK,
                                                                  sizeof(ccc), &ccc);
        /* Otherwise read operations are not permitted */
        } else {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle,
                                                ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
        }

}

/* Handler for write requests, that is BLE_EVT_GATTS_WRITE_REQ */
static void handle_write_req(ble_service_t *svc, const ble_evt_gatts_write_req_t *evt)
{
        logging_queue_service_t *log = (logging_queue_service_t *) svc;
        att_error_t status = ATT_ERROR_WRITE_NOT_PERMITTED;

        /*
         * Identify for which attribute handle the write request has been sent to
         * and call the appropriate function.
         */
        if (evt->handle == log->log_char_value_ccc_h) {
                status = do_char_value_ccc_write(log, evt->conn_idx, evt->offset, evt->length, evt->value);
                // If the notifications are triggered start a timer for checking for available items in the queue
                if(get_u16(evt->value) == GATT_CCC_NOTIFICATIONS)
                        OS_TIMER_START(send_ntf_timer, OS_TIMER_FOREVER);
                else
                        OS_TIMER_STOP(send_ntf_timer, OS_TIMER_FOREVER);
                goto done;
        }

        if (evt->handle == log->severity_char_value_h) {
                status = do_char_value_write(log, evt);
                goto done;
        }

done:
        if (status == ((att_error_t) - 1)) {
                // Write handler executed properly, will be replied by cfm call
                return;
        }

        /* Otherwise write operations are not permitted */
        ble_gatts_write_cfm(evt->conn_idx, evt->handle, status);
}

static void handle_evt_send_ind(ble_service_t *svc, const ble_evt_gatts_event_sent_t *evt)
{
        logging_queue_service_t *log = (logging_queue_service_t *) svc;
        struct mcif_message_s *current_message;

        /* Check if the handle is the proper one and the status has no error */
        if (evt->handle == log->log_char_value_h && evt->status)
        {
                if(OS_QUEUE_MESSAGES_WAITING(xLogQueue) != 0)
                {
                        OS_QUEUE_GET(xLogQueue, &current_message, OS_QUEUE_NO_WAIT);
                        log_notify_char_value(log_svc_hnd, evt->conn_idx, (uint8_t*)current_message->buffer, current_message->len);
                        OS_FREE(current_message);
                }
                else
                {
                        // If no more messages in the queue restart the timer
                        OS_TIMER_START(send_ntf_timer, OS_TIMER_FOREVER);
                }
        }
}

/* Function to be called after a cleanup event */
static void cleanup(ble_service_t *svc)
{
        logging_queue_service_t *log = (logging_queue_service_t *) svc;

        ble_storage_remove_all(log->log_char_value_ccc_h);

        OS_FREE(log);
}

ble_service_t *log_service_init(const uint8_t *variable_value ,const logging_queue_service_cb_t *cb)
{

        logging_queue_service_t *log_srv;
        att_uuid_t uuid;

        uint16_t char_descr_log_h;
        uint16_t char_descr_disc_log_cnt_h;
        uint16_t char_descr_severity_h;

        log_srv = (logging_queue_service_t *)OS_MALLOC(sizeof(*log_srv));
        memset(log_srv, 0, sizeof(*log_srv));

        /* Declare handlers for specific BLE events */
        log_srv->svc.read_req   = handle_read_req;
        log_srv->svc.write_req  = handle_write_req;
        log_srv->svc.event_sent = handle_evt_send_ind;
        log_srv->svc.cleanup    = cleanup;
        log_srv->cb = cb;

        /********************************* Service declaration *********************************/
        ble_uuid_from_string("00000000-1111-2222-2222-333333333333", &uuid);
        ble_gatts_add_service(&uuid, GATT_SERVICE_PRIMARY, ble_gatts_get_num_attr(0, 3, 4));

        /******************************** 1st Characteristic declaration ********************************/
        ble_uuid_from_string("11111111-0000-0000-0000-111111111111", &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_NOTIFY, ATT_PERM_RW, LOGGING_MIN_MSG_SIZE, 0, NULL, &log_srv->log_char_value_h);

        /* Define descriptor of type Client Characteristic Configuration (CCC) */
        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_RW, 2, 0, &log_srv->log_char_value_ccc_h);

        /* Descriptor declaration - Characteristic User Description (CUD) */
        ble_uuid_create16(UUID_GATT_CHAR_USER_DESCRIPTION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ, sizeof(char_descr_log), 0, &char_descr_log_h);

        /******************************** 2nd Characteristic Declaration ********************************/
        ble_uuid_from_string("11111111-0000-0000-0000-222222222222", &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_READ, ATT_PERM_RW, 4, GATTS_FLAG_CHAR_READ_REQ, NULL, &log_srv->dis_log_cnt_value_h);

        /* Descriptor declaration - Characteristic User Description (CUD) */
        ble_uuid_create16(UUID_GATT_CHAR_USER_DESCRIPTION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ, sizeof(char_descr_disc_log_cnt), 0, &char_descr_disc_log_cnt_h);

        /******************************** 3rd Characteristic Declaration ********************************/
        ble_uuid_from_string("11111111-0000-0000-0000-333333333333", &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_READ | GATT_PROP_WRITE, ATT_PERM_RW, 1, GATTS_FLAG_CHAR_READ_REQ, NULL, &log_srv->severity_char_value_h);

        /* Descriptor declaration - Characteristic User Description (CUD) */
        ble_uuid_create16(UUID_GATT_CHAR_USER_DESCRIPTION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ, sizeof(char_descr_severity), 0, &char_descr_severity_h);

        /*
         * Register all the attribute handles so that they can be updated
         * by the BLE manager automatically.
         */
        ble_gatts_register_service(&log_srv->svc.start_h,               // Service start handle
                                        &log_srv->log_char_value_h,     // Log characteristic handle
                                        &log_srv->log_char_value_ccc_h, // Log ccc handle
                                        &char_descr_log_h,              // Log user descriptor handle
                                        &log_srv->dis_log_cnt_value_h,  // Discarded log count handle
                                        &char_descr_disc_log_cnt_h,     // Discarded log count user descriptor handle
                                        &log_srv->severity_char_value_h,// Severity characteristic handle
                                        &char_descr_severity_h,         // Severity user descriptor handle
                                        0);                             // Ending byte

        /* Calculate the last attribute handle of the BLE service */
        log_srv->svc.end_h = log_srv->svc.start_h + ble_gatts_get_num_attr(0, 3, 4);

        /* Set default attribute values TODO*/
        //ble_gatts_set_value(log_srv->log_char_value_h, 1, variable_value);

        /* Set the default values of the Characteristics */
        ble_gatts_set_value(log_srv->severity_char_value_h, 1, &logging_min_severity);
        /* Set the values of the descriptors */
        ble_gatts_set_value(char_descr_log_h,  sizeof(char_descr_log), char_descr_log);
        ble_gatts_set_value(char_descr_disc_log_cnt_h, sizeof(char_descr_disc_log_cnt), char_descr_disc_log_cnt);
        ble_gatts_set_value(char_descr_severity_h, sizeof(char_descr_severity), char_descr_severity);

        /* Register the BLE service in BLE framework */
        ble_service_add(&log_srv->svc);

        /* Return the service handle */
        return &log_srv->svc;
}

static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        ble_gattc_exchange_mtu(evt->conn_idx);
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
        // Stop the notification timer
        OS_TIMER_STOP(send_ntf_timer, OS_TIMER_FOREVER);
        // Restart advertising
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

static void handle_evt_gap_adv_completed(ble_evt_gap_adv_completed_t *evt)
{

}

static void send_ntf_timer_cb(OS_TIMER timer)
{
        struct mcif_message_s *current_message = NULL;
        uint8_t conn_dev_cnt = 0;
        uint16_t *conn_dev;
        // Get the current established connections
        ble_gap_get_connected(&conn_dev_cnt, &conn_dev);
        // Check if the device is connected and if there are messages in the queue
        if (conn_dev_cnt && OS_QUEUE_MESSAGES_WAITING(xLogQueue) != 0){
                OS_QUEUE_GET(xLogQueue, &current_message, OS_QUEUE_NO_WAIT);
                // Send the first notification message, the rest will be send out from the completion handler
                log_notify_char_value(log_svc_hnd, conn_dev[conn_dev_cnt-1], (uint8_t*)current_message->buffer, current_message->len);
                OS_FREE(current_message);
        }
        else
        {
                OS_TIMER_START(send_ntf_timer, OS_TIMER_FOREVER);
        }

        if(conn_dev)
                OS_FREE(conn_dev);
}

void queued_ble_task( void *pvParameters )
{
        int8_t wdog_id;

        /* Just remove compiler warnings about the unused parameter */
        ( void ) pvParameters;

        /* Register queue_ble_task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        /* Start BLE device as a peripheral */
        ble_peripheral_start();
        /* Set the MTU size for the connection  */
        ble_gap_mtu_size_set(APP_MAX_MTU_SIZE);

        ble_register_app();

        /* Set device name */
        ble_gap_device_name_set("Dialog Logging", ATT_PERM_READ);

        /* Set advertising data */
        ble_gap_adv_data_set(sizeof(adv_data), adv_data, 0, NULL);

        /* Initialize the queue custom service */
        log_svc_hnd = log_service_init(&log_char_val, &log_callbacks);

        /* Start advertising */
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        // Create timer for checking available entries in the queue, timer starts when notifications are enabled
        send_ntf_timer = OS_TIMER_CREATE("ntf_timer", send_ntf_log_FREQUENCY_MS, OS_TIMER_FAIL, (void *) OS_GET_CURRENT_TASK(), send_ntf_timer_cb);
        OS_ASSERT(send_ntf_timer);

        for( ;; ){

                OS_BASE_TYPE ret;
                uint32_t notif;

                /* Notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* Suspend watchdog while blocking on ble_get_event() */
                sys_watchdog_suspend(wdog_id);

                /* Wait on any of the notification bits, then clear them all */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for task notification. The return value must be OS_OK */
                OS_ASSERT(ret == OS_OK);

                /* resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

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
                        case BLE_EVT_GAP_DISCONNECTED:
                                handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_ADV_COMPLETED:
                                handle_evt_gap_adv_completed((ble_evt_gap_adv_completed_t *)hdr);
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
        }
}

#endif
