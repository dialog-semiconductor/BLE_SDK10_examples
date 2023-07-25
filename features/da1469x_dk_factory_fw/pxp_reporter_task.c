/**
 ****************************************************************************************
 *
 * @file pxp_reporter_task.c
 *
 * @brief PXP profile application implementation
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
#include <string.h>
#include "osal.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_l2cap.h"
#include "sdk_list.h"
#include "bas.h"
#include "ias.h"
#include "lls.h"
#include "tps.h"

#include "sys_power_mgr.h"
//#include "sys_socf.h"
#include "ad_nvparam.h"
#include "ad_gpadc.h"
#include "hw_gpio.h"
#include "hw_led.h"
#include "sys_trng.h"


#include "sys_watchdog.h"
#include "app_nvparam.h"
#include "pxp_reporter_config.h"
#include "platform_devices.h"



/*
 * Notification bits reservation
 *
 * Bit #0 is always assigned to BLE event queue notification.
 */
#define ALERT_TMO_NOTIF                 (1 << 1)
#define ADV_TMO_NOTIF                   (1 << 2)
#define BAS_TMO_NOTIF                   (1 << 3)
#define BLINK_TMO_NOTIF                 (1 << 4)
#define PXP_UPDATE_CONN_PARAM_NOTIF     (1 << 5)

/*
 * The maximum length of name in scan response
 */
#define MAX_NAME_LEN    (BLE_SCAN_RSP_LEN_MAX - 2)

/*
 * PXP advertising and scan response data
 *
 * While not required, the PXP specification states that a PX reporter device using the peripheral
 * role can advertise support for LLS. Device name is set in scan response to make it easily
 * recognizable.
 */
static const gap_adv_ad_struct_t adv_data[] = {
        GAP_ADV_AD_STRUCT_BYTES(GAP_DATA_TYPE_UUID16_LIST_INC,
                                0x03, 0x18,  // = 0x1803 (LLS UUID)
                                0x02, 0x18)  // = 0x1802 (IAS UUID)
};

/*
 * PXP advertising interval values
 *
 * Recommended advertising interval values as defined by the PXP specification. By default,
 * "fast connection" is used.
 */
static const struct {
        uint16_t min;
        uint16_t max;
} adv_intervals[2] = {
        // "fast connection" interval values
        {
                .min = BLE_ADV_INTERVAL_FROM_MS(20),      // 20ms
                .max = BLE_ADV_INTERVAL_FROM_MS(30),      // 30ms
        },
        // "reduced power" interval values
        {
                .min = BLE_ADV_INTERVAL_FROM_MS(1000),    // 1000ms
                .max = BLE_ADV_INTERVAL_FROM_MS(1500),    // 1500ms
        }
};

typedef enum {
        ADV_INTERVAL_FAST = 0,
        ADV_INTERVAL_POWER = 1,
} adv_setting_t;

/* Current advertising setting */
__RETAINED_RW adv_setting_t adv_setting = ADV_INTERVAL_FAST;

/* Timer used to switch from "fast connection" to "reduced power" advertising intervals */
__RETAINED static OS_TIMER adv_tim;

/* Timer used to disable alert after timeout */
__RETAINED static OS_TIMER alert_tim;

/* LED state */
__RETAINED bool led_active;

/* Timer used to blink led */
__RETAINED static OS_TIMER led_blink_tim;

struct device {
        struct device *next;
        bd_address_t addr;
};

typedef struct {
        void           *next;

        bool            expired;

        uint16_t        conn_idx;

        OS_TIMER        param_timer;
        OS_TASK         current_task;
} conn_dev_t;

/* List of devices pending reconnection */
__RETAINED static void *reconnection_list;

/* List of devices waiting for connection parameters update */
__RETAINED static void *param_connections;

/* Buffer must have length at least max_len + 1 */
static uint16_t read_name(uint16_t max_len, char *name_buf)
{
        uint16_t read_len = 0;

#if dg_configNVPARAM_ADAPTER
        nvparam_t param;
        param = ad_nvparam_open("ble_app");
        read_len = ad_nvparam_read(param, TAG_BLE_APP_NAME, max_len, name_buf);
        ad_nvparam_close(param);
#endif /* dg_configNVPARAM_ADAPTER */

        if (read_len == 0) {
                strcpy(name_buf, PX_REPORTER_DEFAULT_NAME);
                return strlen(PX_REPORTER_DEFAULT_NAME);
        }

        name_buf[read_len] = '\0';

        return read_len;
}

static bool device_match_addr(const void *elem, const void *ud)
{
        const struct device *dev = elem;
        const bd_address_t *addr = ud;

        return !memcmp(&dev->addr, addr, sizeof(*addr));
}

/* Advertising intervals change timeout timer callback */
static void adv_tim_cb(OS_TIMER timer)
{
        OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);

        OS_TASK_NOTIFY(task, ADV_TMO_NOTIF, OS_NOTIFY_SET_BITS);
}

static void set_advertising_interval(adv_setting_t setting)
{
        uint16_t min = adv_intervals[setting].min;
        uint16_t max = adv_intervals[setting].max;

        /* Save current advertising setting */
        adv_setting = setting;

        ble_gap_adv_intv_set(min, max);
}

/* Alert timeout timer callback */
static void alert_tim_cb(OS_TIMER timer)
{
        OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);

        OS_TASK_NOTIFY(task, ALERT_TMO_NOTIF, OS_NOTIFY_SET_BITS);
}

/* Blink timeout timer callback */
static void led_blink_tim_cb(OS_TIMER timer)
{
        OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);

        OS_TASK_NOTIFY(task, BLINK_TMO_NOTIF, OS_NOTIFY_SET_BITS);
}




void print_own_address(void) {

        own_address_t addr;


        ble_gap_address_get(&addr);
        printf("\r\nDevice address: ");
        for(int i= sizeof(addr.addr) - 1;i >=0;i--) {
                printf("%02X",addr.addr[i]);
                if(i != 0){
                        printf(":");
                } else {
                        printf("\r\n");
                }

        }
        fflush(stdout);


}


static void led_toggle(void)
{
        if (!led_active) {
                return;
        }

        hw_gpio_toggle(LED1_PORT, LED1_PIN);
        HW_GPIO_PAD_LATCH_ENABLE(LED1);
        HW_GPIO_PAD_LATCH_DISABLE(LED1);

        OS_TIMER_START(led_blink_tim, OS_TIMER_FOREVER);
}

/* Configure alert for a given level */
static void do_alert(uint8_t level)
{
        switch (level) {
        case 1:
        case 2:
                OS_TIMER_STOP(led_blink_tim, OS_TIMER_FOREVER);
                if (level == 1) {
                        OS_TIMER_CHANGE_PERIOD(led_blink_tim, OS_MS_2_TICKS(SLOW_BLINKING), OS_TIMER_FOREVER);
                } else {
                        OS_TIMER_CHANGE_PERIOD(led_blink_tim, OS_MS_2_TICKS(FAST_BLINKING), OS_TIMER_FOREVER);
                }
                /* Enable LED and start timer for blinking */
                hw_gpio_set_active(LED1_PORT, LED1_PIN);
                HW_GPIO_PAD_LATCH_ENABLE(LED1);
                HW_GPIO_PAD_LATCH_DISABLE(LED1);
                led_active = true;
                OS_TIMER_START(led_blink_tim, OS_TIMER_FOREVER);
                break;
        default:
                OS_TIMER_STOP(led_blink_tim, OS_TIMER_FOREVER);
                OS_TIMER_CHANGE_PERIOD(led_blink_tim, OS_MS_2_TICKS(ALIVE_BLINKING), OS_TIMER_FOREVER);
                /* Simply disable LED */
                hw_gpio_set_active(LED1_PORT, LED1_PIN);
                HW_GPIO_PAD_LATCH_ENABLE(LED1);
                HW_GPIO_PAD_LATCH_DISABLE(LED1);
                led_active = true;
                hw_led_set_pwm_sw_pause(true);
                OS_TIMER_START(led_blink_tim, OS_TIMER_FOREVER);
                break;
        }
}

/* Alert callback from IAS (peer has written a new value) */
static void ias_alert_cb(uint16_t conn_idx, uint8_t level)
{
        do_alert(level);
}

static void start_adv(void)
{
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
        if (adv_setting == ADV_INTERVAL_FAST) {
                /* Start timer to switch to ADV_INTERVAL_POWER after 30 s */
                OS_TIMER_START(adv_tim, OS_TIMER_FOREVER);
        }
}

/* Alert callback from LLS (link to peer was lost) */
static void lls_alert_cb(uint16_t conn_idx, const bd_address_t *address, uint8_t level)
{
        ble_error_t ret;
        struct device *dev;

        if (level == 0) {
                return;
        }

        /* Add alerting device to reconnection list */
        dev = OS_MALLOC(sizeof(*dev));
        memcpy(&dev->addr, address, sizeof(dev->addr));
        list_add(&reconnection_list, dev);

        /* Trigger an alert */
        do_alert(level);

        /* (Re)start alert timeout timer */
        OS_TIMER_RESET(alert_tim, OS_TIMER_FOREVER);

        /*
         * Set interval values to "fast connect" and stop advertising timer. Stop advertising
         * so it can be restarted with new interval values.
         */
        OS_TIMER_STOP(adv_tim, OS_TIMER_FOREVER);
        set_advertising_interval(ADV_INTERVAL_FAST);
        ret = ble_gap_adv_stop();
        /* Advertising isn't started, start it now */
        if (ret == BLE_ERROR_NOT_ALLOWED) {
                start_adv();
        }

}

/* Match connection by connection index */
static bool conn_params_match(const void *elem, const void *ud)
{
        conn_dev_t *conn_dev = (conn_dev_t *) elem;
        uint16_t conn_idx = (uint16_t) (uint32_t) ud;

        return conn_dev->conn_idx == conn_idx;
}

/*
 * This timer callback notifies the task that time for discovery, bonding and encryption
 * elapsed, and connection parameters can be changed to the preferred ones.
 */
static void conn_params_timer_cb(OS_TIMER timer)
{
        conn_dev_t *conn_dev = (conn_dev_t *) OS_TIMER_GET_TIMER_ID(timer);;

        conn_dev = list_find(param_connections, conn_params_match,
                                                (const void *) (uint32_t) conn_dev->conn_idx);
        if (conn_dev) {
                conn_dev->expired = true;
                OS_TASK_NOTIFY(conn_dev->current_task, PXP_UPDATE_CONN_PARAM_NOTIF,
                                                                        OS_NOTIFY_SET_BITS);
        }
}

#if !dg_configSUOTA_SUPPORT || PX_REPORTER_SUOTA_POWER_SAVING
/* Update connection parameters */
static void conn_param_update(uint16_t conn_idx)
{
        gap_conn_params_t cp;

        cp.interval_min = defaultBLE_PPCP_INTERVAL_MIN;
        cp.interval_max = defaultBLE_PPCP_INTERVAL_MAX;
        cp.slave_latency = defaultBLE_PPCP_SLAVE_LATENCY;
        cp.sup_timeout = defaultBLE_PPCP_SUP_TIMEOUT;

        ble_gap_conn_param_update(conn_idx, &cp);
}
#endif



static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        ble_error_t ret;
        struct device *dev;
        conn_dev_t *conn_dev;


        /* Add timer that when expired will re-negotiate connection parameters */
        conn_dev = OS_MALLOC(sizeof(*conn_dev));
        if (conn_dev) {
                conn_dev->conn_idx = evt->conn_idx;
                conn_dev->expired = false;
                conn_dev->current_task = OS_GET_CURRENT_TASK();
                conn_dev->param_timer = OS_TIMER_CREATE("conn_param", OS_MS_2_TICKS(5000),
                                        OS_TIMER_FAIL, (uint32_t) conn_dev, conn_params_timer_cb);
                list_append(&param_connections, conn_dev);
                OS_TIMER_START(conn_dev->param_timer, OS_TIMER_FOREVER);
        }

        /*
         * Try to unlink the device with the same address from the reconnection list - if found,
         * this is a reconnection and we should stop alerting and also clear the reconnection list
         * and disable the timer.
         */
        dev = list_unlink(&reconnection_list, device_match_addr, &evt->peer_address);
        if (dev) {
                do_alert(0);

                list_free(&reconnection_list, NULL, NULL);

                OS_TIMER_STOP(alert_tim, OS_TIMER_FOREVER);

                /*
                 * The device is reconnected so set interval values immediately to "reduced power"
                 * and stop advertising timer. Stop advertising so it can be restarted with the new
                 * interval values.
                 */
                OS_TIMER_STOP(adv_tim, OS_TIMER_FOREVER);
                set_advertising_interval(ADV_INTERVAL_POWER);
                ret = ble_gap_adv_stop();
                /* Advertising isn't started, start it now */
                if (ret == BLE_ERROR_NOT_ALLOWED) {
                        start_adv();
                }
        }
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
        ble_error_t ret;
        conn_dev_t *conn_dev = list_unlink(&param_connections, conn_params_match,
                                                        (const void *) (uint32_t) evt->conn_idx);

        /*
         * The device is still in the list if the disconnection happened before the timer expiration.
         * In this case stop the timer and free the associated memory.
         */
        if (conn_dev) {
                OS_TIMER_DELETE(conn_dev->param_timer, OS_TIMER_FOREVER);
                OS_FREE(conn_dev);
        }

        /* Switch back to fast advertising interval */
        set_advertising_interval(ADV_INTERVAL_FAST);
        ret = ble_gap_adv_stop();
        /* Advertising isn't started, start it now */
        if (ret == BLE_ERROR_NOT_ALLOWED) {
                start_adv();
        }

#if (PX_REPORTER_INCLUDE_BAS == 1)
        size_t num_connected;
        /* Stop monitoring battery level if no one is connected */
        ble_gap_get_devices(GAP_DEVICE_FILTER_CONNECTED, NULL, &num_connected, NULL);
        if (num_connected == 0) {
                OS_TIMER_STOP(bas_tim, OS_TIMER_FOREVER);
        }
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */
}

static void handle_evt_gap_adv_completed(ble_evt_gap_adv_completed_t *evt)
{
        /*
         * If advertising is completed, just restart it. It's either because a new client connected
         * or it was cancelled in order to change the interval values.
         */
        start_adv();
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



void pxp_reporter_task(void *params)
{
        int8_t tx_power_level;
        int8_t wdog_id;
        uint16_t name_len;
        char name_buf[MAX_NAME_LEN + 1];        /* 1 byte for '\0' character */

        /* Scan Response object to be populated with <Complete Local Name> AD type */
        gap_adv_ad_struct_t *scan_rsp;

        /* Register pxp_reporter_task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        /*************************************************************************************************\
         * Initialize BLE
         */
        /* Start BLE device as peripheral */
        ble_peripheral_start();

        /* Register task to BLE framework to receive BLE event notifications */
        ble_register_app();


        /* Get device name from NVPARAM if valid or use default otherwise */
        name_len = read_name(MAX_NAME_LEN, name_buf);

        own_address_t addr = {
                .addr_type = PRIVATE_STATIC_ADDRESS,
        };

        sys_trng_get_bytes(addr.addr,sizeof(addr.addr));


        ble_gap_address_set(&addr, 0x7530); // 5 min



        sprintf(&name_buf[name_len - 4],"%02X%02X",addr.addr[1],addr.addr[0]);

        /* Set device name */
        ble_gap_device_name_set(name_buf, ATT_PERM_READ);

        /* Define Scan Response object internals dealing with retrieved name */
        scan_rsp = GAP_ADV_AD_STRUCT_DECLARE(GAP_DATA_TYPE_LOCAL_NAME, name_len, name_buf);

        /*************************************************************************************************\
         * Initialize BLE services
         */
        /* Add Immediate Alert Service */
        ias_init(ias_alert_cb);

        /* Add Link Loss Service */
        lls_init(lls_alert_cb);

        /* Add TX Power Service (use fixed TX power level) */
        tx_power_level = 0;
        tps_init(tx_power_level);

        /*************************************************************************************************\
         * Initialize timers
         */
        /*
         * Create timer for LLS which will be started to timeout alarm if no reconnection occurred
         * within 15 seconds.
         */
        alert_tim = OS_TIMER_CREATE("lls", OS_MS_2_TICKS(15000), OS_TIMER_FAIL,
                                                (void *) OS_GET_CURRENT_TASK(), alert_tim_cb);

        /*
         * Create timer for switching from "fast connection" to "reduced power" advertising
         * intervals after 30 seconds.
         */
        adv_tim = OS_TIMER_CREATE("adv", OS_MS_2_TICKS(30000), OS_TIMER_FAIL,
                                                (void *) OS_GET_CURRENT_TASK(), adv_tim_cb);

        /* Create timer to implement led blinking */
        led_blink_tim = OS_TIMER_CREATE("led", OS_MS_2_TICKS(500), OS_TIMER_FAIL,
                                                (void *) OS_GET_CURRENT_TASK(), led_blink_tim_cb);

        /*************************************************************************************************\
         * Start advertising
         *
         * Set advertising data and scan response, then start advertising.
         *
         * By default, advertising interval is set to "fast connect" and a timer is started to
         * switch to "reduced power" interval afterwards.
         */
        set_advertising_interval(ADV_INTERVAL_FAST);
        ble_gap_adv_ad_struct_set(ARRAY_LENGTH(adv_data), adv_data, 1 , scan_rsp);
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
        OS_TIMER_START(adv_tim, OS_TIMER_FOREVER);


        do_alert(0);

        printf("\r\n#######################\r\n");
        printf("DA1469x Proximity reporter\r\n");
        printf("#######################\r\n");

        fflush(stdout);

        print_own_address();



#if (PX_REPORTER_INCLUDE_BAS == 1)

        /* Update battery level exposed in BAS */
        bas_update();
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */

        for (;;) {
                OS_BASE_TYPE ret __UNUSED;
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

                /* Notified from LLS timer? */
                if (notif & ALERT_TMO_NOTIF) {
                        /* Stop alerting and clear reconnection list */
                        do_alert(0);
                        list_free(&reconnection_list, NULL, NULL);
                }

                /* Notified from advertising timer? */
                if (notif & ADV_TMO_NOTIF) {
                        ble_error_t ret;
                        /*
                         * Change interval values and stop advertising. Once it's stopped, it will
                         * be started again with the new parameters.
                         */
                        set_advertising_interval(ADV_INTERVAL_POWER);
                        ret = ble_gap_adv_stop();
                        /* Advertising isn't started, start it now */
                        if (ret == BLE_ERROR_NOT_ALLOWED) {
                                start_adv();
                        }
                }

#if (PX_REPORTER_INCLUDE_BAS == 1)
                /* Notified from battery timer? */
                if (notif & BAS_TMO_NOTIF) {
                        /* Read battery level, and notify clients if the level has changed */
                        bas_update();
                }
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */

                /* Notified from blink timer? */
                if (notif & BLINK_TMO_NOTIF) {
                        /* Toggle LED state */
                        led_toggle();
                }

                /* Fast connection timer expired, try to set reduced power connection parameters */
                if (notif & PXP_UPDATE_CONN_PARAM_NOTIF) {
                        conn_dev_t *conn_dev = param_connections;

                        if (conn_dev && conn_dev->expired) {
                                param_connections = conn_dev->next;

#if !dg_configSUOTA_SUPPORT || PX_REPORTER_SUOTA_POWER_SAVING
                                        conn_param_update(conn_dev->conn_idx);
#endif

                                OS_TIMER_DELETE(conn_dev->param_timer, OS_TIMER_FOREVER);
                                OS_FREE(conn_dev);

                                /*
                                 * If the queue is not empty, reset bit and check if timer expired
                                 * next time.
                                 */
                                if (param_connections) {
                                        OS_TASK_NOTIFY(OS_GET_CURRENT_TASK(),
                                                PXP_UPDATE_CONN_PARAM_NOTIF, OS_NOTIFY_SET_BITS);
                                }
                        }
                }
        }
}

