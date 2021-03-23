/**
 ****************************************************************************************
 *
 * @file cpp_sensor_task.c
 *
 * @brief Cycling Power Profile Sensor demo task
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "osal.h"
#include "sys_watchdog.h"
#include "ble_common.h"
#include "ble_service.h"
#include "cps.h"
#include "dis.h"
#include "ble_gattc.h"

#define CP_MEASUREMENT_INTERVAL_MS (1000)

#define CP_MEASUREMENT_INTERVAL OS_MS_2_TICKS(CP_MEASUREMENT_INTERVAL_MS)

#define CP_MEASUREMENT_NOTIF (1 << 1)

/* Instance of CPS Service */
PRIVILEGED_DATA static ble_service_t *cps;
/* Register device's connection index for power measurements */
INITIALISED_PRIVILEGED_DATA static uint16_t measure_notif_conn_idx = BLE_CONN_IDX_INVALID;
/* Timer used for sending measurements */
PRIVILEGED_DATA static OS_TIMER cp_measure_timer;
/* Measurement value */
INITIALISED_PRIVILEGED_DATA static cps_measurement_t measurement = {
        .instant_power = 0,
};

/*
 * CPP advertising and scan response data
 *
 * As per CPP specification, sensor device should include CPS UUID in advertising data,
 * local name and appearance in either advertising data or scan response
 */
static const uint8_t adv_data[] = {
        0x03, GAP_DATA_TYPE_UUID16_LIST_INC,
        0x18, 0x18, // = 0x1818 (CPS UUID)
        0x03, GAP_DATA_TYPE_APPEARANCE,
        BLE_GAP_APPEARANCE_CYCLING_POWER_SENSOR & 0xFF,
        BLE_GAP_APPEARANCE_CYCLING_POWER_SENSOR >> 8,
};

static const uint8_t scan_rsp[] = {
        0x11, GAP_DATA_TYPE_LOCAL_NAME,
        'D', 'i', 'a', 'l', 'o', 'g',' ', 'C', 'P', ' ', 'S', 'e', 'n', 's', 'o', 'r',
};

static const dis_device_info_t dis_info = {
        .manufacturer  = defaultBLE_DIS_MANUFACTURER,
        .model_number  = defaultBLE_DIS_MODEL_NUMBER,
};

static const ble_service_config_t service_config = {
        .service_type = GATT_SERVICE_PRIMARY,
        .sec_level = GAP_SEC_LEVEL_1,
        .num_includes = 0,
};

static const uint8_t sensor_locations[] = {CPS_SENSOR_LOCATION_OTHER,
                                CPS_SENSOR_LOCATION_TOP_OF_SHOE, CPS_SENSOR_LOCATION_IN_SHOE,
                                CPS_SENSOR_LOCATION_HIP, CPS_SENSOR_LOCATION_FRONT_WHEEL,
                                CPS_SENSOR_LOCATION_LEFT_CRANK, CPS_SENSOR_LOCATION_RIGHT_CRANK,
                                CPS_SENSOR_LOCATION_LEFT_PEDAL, CPS_SENSOR_LOCATION_RIGHT_PEDAL,
                                CPS_SENSOR_LOCATION_FRONT_HUB, CPS_SENSOR_LOCATION_REAR_DROPOUT,
                                CPS_SENSOR_LOCATION_CHAINSTAY, CPS_SENSOR_LOCATION_REAR_WHEEL,
                                CPS_SENSOR_LOCATION_REAR_HUB, CPS_SENSOR_LOCATION_CHEST,
};

static const cps_config_t cps_config =  {
        .cfg_measurement_context_type = CPS_SENSOR_MEASUREMENT_CONTEXT_FORCE_BASED,
        .cfg_vector_direction = CPS_VECTOR_INSTANT_MEASUREMENT_DIRECTION_TANGENTIAL,
        .supported_sensor_locations = sensor_locations,
        .supported_sensor_locations_count = sizeof(sensor_locations),
        .init_location = CPS_SENSOR_LOCATION_OTHER,
        .sampling_rate = 50, /* Expected values are between 25 Hz and 50 Hz */
};

static void measure_timer_cb(TimerHandle_t timer)
{
        OS_TASK task = (OS_TASK) pvTimerGetTimerID(timer);

        OS_TASK_NOTIFY(task, CP_MEASUREMENT_NOTIF, OS_NOTIFY_SET_BITS);
}

static void measure_notification_changed_cb(ble_service_t *svc, uint16_t conn_idx, bool enabled)
{
        if (enabled) {
                measure_notif_conn_idx = conn_idx;
                /* Start measurement notification timer */
                OS_TIMER_START(cp_measure_timer, CP_MEASUREMENT_INTERVAL);
        } else {
                measure_notif_conn_idx = BLE_CONN_IDX_INVALID;
                /* Stop measurement notification timer */
                OS_TIMER_STOP(cp_measure_timer, 0);
        }
}

static const cps_callbacks_t cps_callbacks = {
        .measur_notif_changed       = measure_notification_changed_cb,
};

static void handle_evt_gap_pair_req(ble_evt_gap_pair_req_t *evt)
{
        ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

void cpp_sensor_task(void *params)
{
        int8_t wdog_id;

        /* Register cpp_sensor_task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        ble_peripheral_start();
        ble_register_app();

        /*
         * Set device name
         */
        ble_gap_device_name_set("Dialog CPP Sensor", ATT_PERM_READ);
        ble_gap_appearance_set(BLE_GAP_APPEARANCE_CYCLING_POWER_SENSOR, ATT_PERM_READ);

        /*
         * Register DIS
         *
         * DIS doesn't contain any dynamic data thus it doesn't need to be registered in ble_service
         * framework (but it's not an error to do so).
         */
        dis_init(&service_config, &dis_info);

        /*
         * Register CPS
         *
         * CPS instance should be registered in ble_service framework in order for events inside
         * service to be processed properly.
         */
        cps = cps_init(&service_config, &cps_config, &cps_callbacks);
        ble_service_add(cps);

        /*
         * Set timer with 1 sec delay
         */
        cp_measure_timer = OS_TIMER_CREATE("cp", CP_MEASUREMENT_INTERVAL, pdTRUE,
                                                (void *) OS_GET_CURRENT_TASK(), measure_timer_cb);

        /*
         * Set advertising data and scan response, then start advertising.
         */
        ble_gap_adv_data_set(sizeof(adv_data), adv_data, sizeof(scan_rsp), scan_rsp);
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        for (;;) {
                BaseType_t ret;
                uint32_t notif;

                /* Notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* Suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Guaranteed to succeed since we're waiting forever for the notification */
                OS_ASSERT(ret == OS_OK);

                /* Resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                /* Notified from BLE Manager? */
                if (notif & BLE_APP_NOTIFY_MASK) {
                        ble_evt_hdr_t *hdr;

                        hdr = ble_get_event(false);
                        if (!hdr) {
                                goto no_event;
                        }

                        /*
                         * First, application needs to try pass event through ble_framework.
                         * Then it can handle it itself and finally pass to default event handler.
                         */
                        if (!ble_service_handle_event(hdr)) {
                                switch (hdr->evt_code) {
                                case BLE_EVT_GAP_PAIR_REQ:
                                        handle_evt_gap_pair_req((ble_evt_gap_pair_req_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_DISCONNECTED:
                                        handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_CONNECTED:
                                        break;
                                default:
                                        ble_handle_event_default(hdr);
                                        break;
                                }
                        }

                        /* Free event buffer (it's not needed anymore) */
                        OS_FREE(hdr);

no_event:
                        /*
                         * If there are more events waiting in queue, application should process
                         * them now.
                         */
                        if (ble_has_event()) {
                                xTaskNotify(OS_GET_CURRENT_TASK(), BLE_APP_NOTIFY_MASK, eSetBits);
                        }
                }

                if (notif & CP_MEASUREMENT_NOTIF) {
                        /* Randomize power measurement values */
                        measurement.instant_power = rand() % 100;

                        if (measure_notif_conn_idx != BLE_CONN_IDX_INVALID) {
                                cps_send_cp_measurement(cps, measure_notif_conn_idx, &measurement);
                        }
                }
        }
}
