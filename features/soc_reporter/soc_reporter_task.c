/**
 ****************************************************************************************
 *
 * @file soc_reporter_task.c
 *
 * @brief SOC report application implementation
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdbool.h>
#include <string.h>
#include "osal.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_l2cap.h"
#include "bas.h"
#include "sys_power_mgr.h"
#include "hw_gpio.h"
#include "sys_watchdog.h"
#include "socf_client.h"
/*
 * Notification bits reservation
 *
 */
#define BAS_TMO_NOTIF   (1 << 1)

/*
 * SOC Update connection parameters notif mask
 */
#define SOC_UPDATE_CONN_PARAM_NOTIF     (1 << 2)
#define MAX_NAME_LEN    (BLE_SCAN_RSP_LEN_MAX - 2)
#define SOC_REPORTER_DEFAULT_NAME        "Dialog SOC Reporter"
#define SOC_REPORTER_BATTERY_CHECK_INTERVAL 60000
/*
 * BLE SOC demo advertising data
 */
static const uint8_t adv_data[] = {
        0x10, GAP_DATA_TYPE_LOCAL_NAME,
        'D', 'i', 'a', 'l', 'o', 'g', ' ', 'S', 'O', 'C', ' ', 'D', 'e', 'm', 'o'
};
/* Battery Service instance */
PRIVILEGED_DATA static ble_service_t *bas;

/* Timer used for battery monitoring */
PRIVILEGED_DATA static OS_TIMER bas_tim;

static void bas_update(void)
{
        uint8_t level;

        level = socf_get_soc();

        bas_set_level(bas, level, true);
}

static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        /* Start battery monitoring if not yet started, but first update current battery level */
        if (!OS_TIMER_IS_ACTIVE(bas_tim)) {
                bas_update();
                OS_TIMER_START(bas_tim, OS_TIMER_FOREVER);
        }
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
        // Restart advertising
         ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

static void handle_evt_gap_adv_completed(ble_evt_gap_adv_completed_t *evt)
{
}

static void handle_evt_gap_pair_req(ble_evt_gap_pair_req_t *evt)
{
        /* Just accept the pairing request, set bond flag to what peer requested */
        ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
}

static void handle_evt_gap_pair_completed(ble_evt_gap_pair_completed_t *evt)
{
}

static void handle_evt_gap_sec_level_changed(ble_evt_gap_sec_level_changed_t *evt)
{
}

static void bas_tim_cb(OS_TIMER timer)
{
        OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);

        OS_TASK_NOTIFY(task, BAS_TMO_NOTIF, OS_NOTIFY_SET_BITS);
}

void soc_reporter_task(void *params)
{
        int8_t wdog_id;
        uint16_t name_len = 0;
        char name_buf[MAX_NAME_LEN + 1];        /* 1 byte for '\0' character */
        uint8_t scan_rsp[BLE_SCAN_RSP_LEN_MAX] = {
                0x00, GAP_DATA_TYPE_LOCAL_NAME, /* Parameter length must be set after */
                /* Name will be put here */
        };

        /* Register soc_reporter_task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        /*************************************************************************************************\
         * Initialize BLE
         */
        /* Start BLE device as peripheral */
        ble_peripheral_start();

        /* Register task to BLE framework to receive BLE event notifications */
        ble_register_app();

        strcpy(name_buf, SOC_REPORTER_DEFAULT_NAME);
        ble_gap_device_name_set(name_buf, ATT_PERM_READ);

        /*
         * Add Battery Service
         *
         * \note Battery Service is not included in PXP specification, but it is included in
         *       pxp_reporter demo application in order to also monitor the device battery level.
         */
        bas = bas_init(NULL, NULL);

        /* Create timer for battery monitoring */
        bas_tim = OS_TIMER_CREATE("bas", OS_MS_2_TICKS(SOC_REPORTER_BATTERY_CHECK_INTERVAL), true,
                                                (void *) OS_GET_CURRENT_TASK(), bas_tim_cb);

        /* Set advertising data */
        name_len = strlen(SOC_REPORTER_DEFAULT_NAME) + 1;
        scan_rsp[0] = name_len + 1; /* 1 byte for data type */
        memcpy(&scan_rsp[2], name_buf, name_len);
        ble_gap_adv_data_set(sizeof(adv_data), adv_data, name_len + 2, scan_rsp);

        /* Start advertising */
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        for (;;) {
                OS_BASE_TYPE ret __attribute__((unused));
                uint32_t notif;

                /* Notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* Suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for the task notification. Therefore, the return value must
                 * always be OS_OK
                 */
                OS_ASSERT(ret == OS_OK);

                /* Resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                /* Notified from BLE manager? */
                if (notif & BLE_APP_NOTIFY_MASK) {
                        ble_evt_hdr_t *hdr;

                        hdr = ble_get_event(false);
                        if (!hdr) {
                                goto no_event;
                        }

                        /*
                         * First, the application needs to check if the event is handled by the
                         * ble_service framework. If it is not handled, the application may handle
                         * it by defining a case for it in the `switch ()` statement below. If the
                         * event is not handled by the application either, it is handled by the
                         * default event handler.
                         */
                        if (!ble_service_handle_event(hdr)) {
                                switch (hdr->evt_code) {
                                case BLE_EVT_GAP_CONNECTED:
                                        handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_DISCONNECTED:
                                        handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_ADV_COMPLETED:
                                        handle_evt_gap_adv_completed((ble_evt_gap_adv_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_PAIR_REQ:
                                        handle_evt_gap_pair_req((ble_evt_gap_pair_req_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_PAIR_COMPLETED:
                                        handle_evt_gap_pair_completed((ble_evt_gap_pair_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_SEC_LEVEL_CHANGED:
                                        handle_evt_gap_sec_level_changed((ble_evt_gap_sec_level_changed_t *) hdr);
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
                                OS_TASK_NOTIFY(OS_GET_CURRENT_TASK(), BLE_APP_NOTIFY_MASK,
                                                                                OS_NOTIFY_SET_BITS);
                        }
                }

                /* Notified battery timer? */
                if (notif & BAS_TMO_NOTIF) {
                        /* Read battery level, and notify clients if the level has changed */
                        bas_update();
                }
        }
}

