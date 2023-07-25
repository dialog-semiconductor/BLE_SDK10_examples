/**
 ****************************************************************************************
 *
 * @file pxp_reporter_task.c
 *
 * @brief PXP profile application implementation
 *
 * Copyright (C) 2015-2022 Renesas Electronics Corporation and/or its affiliates
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

#include "ad_nvparam.h"
#include "ad_gpadc.h"
#include "hw_gpio.h"


#include "sys_watchdog.h"
#include "app_nvparam.h"
#include "pxp_reporter_config.h"
#include "platform_devices.h"

#if dg_configSUOTA_SUPPORT
#include "dis.h"
#include "dlg_suota.h"
#include "sw_version.h"
#endif

#if dg_configSUOTA_SUPPORT
/*
 * Store information about ongoing SUOTA.
 */
__RETAINED_RW static bool suota_ongoing = false;
#endif

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



#define UUID_SUOTA_LSB                  (dg_configBLE_UUID_SUOTA_SERVICE & 0xFF)
#define UUID_SUOTA_MSB                  ((dg_configBLE_UUID_SUOTA_SERVICE & 0xFF00) >> 8)

/*
 * PXP advertising and scan response data
 *
 * While not required, the PXP specification states that a PX reporter device using the peripheral
 * role can advertise support for LLS. Device name is set in scan response to make it easily
 * recognizable.
 */
static const gap_adv_ad_struct_t adv_data[] = {
#if dg_configSUOTA_SUPPORT
        GAP_ADV_AD_STRUCT_BYTES(GAP_DATA_TYPE_UUID16_LIST_INC,
                                0x03, 0x18,  // = 0x1803 (LLS UUID)
                                0x02, 0x18,  // = 0x1802 (IAS UUID)
                                UUID_SUOTA_LSB, UUID_SUOTA_MSB)
#else
        GAP_ADV_AD_STRUCT_BYTES(GAP_DATA_TYPE_UUID16_LIST_INC,
                                0x03, 0x18,  // = 0x1803 (LLS UUID)
                                0x02, 0x18)  // = 0x1802 (IAS UUID)
#endif
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

#if (PX_REPORTER_INCLUDE_BAS == 1)
/* Battery Service instance */
__RETAINED static ble_service_t *bas;

/* Timer used for battery monitoring */
__RETAINED static OS_TIMER bas_tim;
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */

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
                /* Simply disable LED */
                hw_gpio_set_inactive(LED1_PORT, LED1_PIN);
                HW_GPIO_PAD_LATCH_ENABLE(LED1);
                HW_GPIO_PAD_LATCH_DISABLE(LED1);
                led_active = false;
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
        gap_device_t devices[BLE_GAP_MAX_CONNECTED];
        size_t length = ARRAY_LENGTH(devices);

        ble_gap_get_devices(GAP_DEVICE_FILTER_CONNECTED, NULL, &length, devices);
        if (length == ARRAY_LENGTH(devices)) {
                /*
                 * We reached maximum number of connected devices, don't start advertising
                 * now. Advertising will start again once any device will disconnect.
                 */
                return;
        }

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

#if dg_configSUOTA_SUPPORT && PX_REPORTER_SUOTA_POWER_SAVING
/* Update connection parameters for SUOTA */
static void conn_param_update_for_suota(uint16_t conn_idx)
{
        gap_conn_params_t cp;

        cp.interval_min = BLE_CONN_INTERVAL_FROM_MS(20);    // 20ms
        cp.interval_max = BLE_CONN_INTERVAL_FROM_MS(60);    // 60ms
        cp.slave_latency = 0;
        cp.sup_timeout = BLE_SUPERVISION_TMO_FROM_MS(2000); // 2000ms

        ble_gap_conn_param_update(conn_idx, &cp);
}
#endif

#if (PX_REPORTER_INCLUDE_BAS == 1)
#if (dg_configUSE_SOC == 1)
static uint8_t read_battery_level(void)
{
        int16_t level;

        /*
         * The return value from socf_get_soc is from 0(0%) to 1000(100.0%).
         * The input parameter of bas_set_level is from 0(0%) to 100(100%).
         */
        level = (socf_get_soc() + 5) / 10;

        return level;
}
#else
/*
 * The values depend on the battery type.
 * MIN_BATTERY_LEVEL (in mVolts) must correspond to dg_configBATTERY_LOW_LEVEL (in ADC units).
 */
#define MAX_BATTERY_LEVEL 4200
#define MIN_BATTERY_LEVEL 2800

static uint8_t bat_level(uint16_t voltage)
{
        if (voltage >= MAX_BATTERY_LEVEL) {
                return 100;
        } else if (voltage <= MIN_BATTERY_LEVEL) {
                return 0;
        }

        /*
         * For demonstration purposes discharging (Volt vs Capacity) is approximated
         * by a linear function. The exact formula depends on the specific battery being used.
         */
        return (uint8_t) ((int) (voltage - MIN_BATTERY_LEVEL) * 100 /
                                                        (MAX_BATTERY_LEVEL - MIN_BATTERY_LEVEL));
}

static uint8_t read_battery_level(void)
{
        uint16_t bat_voltage;
        uint16_t value;
        ad_gpadc_handle_t handle;

        handle = ad_gpadc_open(&BATTERY_LEVEL);
        ad_gpadc_read(handle, &value);
        bat_voltage = ad_gpadc_conv_to_batt_mvolt(BATTERY_LEVEL.drv, value);
        ad_gpadc_close(handle, false);

        return bat_level(bat_voltage);
}
#endif /* (dg_configUSE_SOC == 1) */

#ifdef DECLARE_DUMMY_READ_BATTERY_LEVEL
static uint8_t dummy_read_battery_level(void)
{
        static uint8_t dummy_level = 100;
        uint8_t level;

        level = dummy_level--;

        /* Handle wrap-around */
        if (dummy_level > 100) {
                dummy_level = 100;
        }

        return level;
}
#endif /* DECLARE_BATTERY_LEVEL_DUMMY_READ */

static void bas_update(void)
{
        uint8_t level;

        level = read_battery_level();

        bas_set_level(bas, level, true);
}
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */

static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        ble_error_t ret;
        struct device *dev;
        conn_dev_t *conn_dev;


#if (PX_REPORTER_INCLUDE_BAS == 1)
        /* Start battery monitoring if not yet started, but first update current battery level */
        if (!OS_TIMER_IS_ACTIVE(bas_tim)) {
                bas_update();
                OS_TIMER_START(bas_tim, OS_TIMER_FOREVER);
        }
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */

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
#if dg_configSUOTA_SUPPORT
        /* If SUOTA is ongoing, don't restart advertising */
        if (suota_ongoing) {
                return;
        }
#endif
        start_adv();
}

static void handle_evt_gap_pair_req(ble_evt_gap_pair_req_t *evt)
{
        /* Just accept the pairing request, set bond flag to what peer requested */
        ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
}


#if dg_configSUOTA_SUPPORT

#if (dg_configBLE_2MBIT_PHY == 1)
static void handle_ble_evt_gap_phy_set_completed(ble_evt_gap_phy_set_completed_t* evt)
{
        /*
         * Add code to handle the case where the preferred PHY settings
         * failed to be set (evt->status != BLE_STATUS_OK).
         */
}

static void handle_ble_evt_gap_phy_changed(ble_evt_gap_phy_changed_t* evt)
{
        /*
         * Add code to handle the PHY changed event.
         */
}
#endif /* (dg_configBLE_2MBIT_PHY == 1) */

static bool suota_ready_cb(uint16_t conn_idx)
{
        /*
         * This callback is used so that the application can accept or block SUOTA.
         * Also, before SUOTA starts, the user might want to do some actions,
         * e.g. disable sleep mode.
         *
         * If `true` is returned, then advertising is stopped and SUOTA is started.
         * Otherwise SUOTA is canceled.
         */
        suota_ongoing = true;

#if PX_REPORTER_SUOTA_POWER_SAVING
        /*
         * We need to decrease the connection interval for SUOTA so that data can be transferred
         * quickly.
         */
        conn_param_update_for_suota(conn_idx);
#endif

#if (dg_configBLE_2MBIT_PHY == 1)
        /* Switch to 2Mbit PHY during SUOTA */
        ble_gap_phy_set(conn_idx, BLE_GAP_PHY_PREF_2M, BLE_GAP_PHY_PREF_2M);
#endif /* (dg_configBLE_2MBIT_PHY == 1) */

        return true;
}

static void suota_status_changed_cb(uint16_t conn_idx, uint8_t status, uint8_t error_code)
{
        /* In case SUOTA finished with an error, we just restore default connection parameters. */
        if (status != SUOTA_ERROR) {
                return;
        }

#if PX_REPORTER_SUOTA_POWER_SAVING
        conn_param_update(conn_idx);
#endif

#if (dg_configBLE_2MBIT_PHY == 1)
        /* Switch to Auto PHY when SUOTA is completed */
        ble_gap_phy_set(conn_idx, BLE_GAP_PHY_PREF_AUTO, BLE_GAP_PHY_PREF_AUTO);
#endif /* (dg_configBLE_2MBIT_PHY == 1) */
}

static const suota_callbacks_t suota_cb = {
        .suota_ready = suota_ready_cb,
        .suota_status = suota_status_changed_cb,
};

/* Device Information Service data */
static const dis_device_info_t dis_info = {
        .manufacturer  = defaultBLE_DIS_MANUFACTURER,
        .model_number  = defaultBLE_DIS_MODEL_NUMBER,
        .serial_number = defaultBLE_DIS_SERIAL_NUMBER,
        .hw_revision   = defaultBLE_DIS_HW_REVISION,
        .fw_revision   = defaultBLE_DIS_FW_REVISION,
#ifdef SW_VERSION
        .sw_revision   = SW_VERSION,
#else
        .sw_revision   = defaultBLE_DIS_SW_REVISION,
#endif /* SW_VERSION */
};
#endif

#if (PX_REPORTER_INCLUDE_BAS == 1)
static void bas_tim_cb(OS_TIMER timer)
{
        OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(timer);

        OS_TASK_NOTIFY(task, BAS_TMO_NOTIF, OS_NOTIFY_SET_BITS);
}
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */

void pxp_reporter_task(void *params)
{
        int8_t tx_power_level;
        int8_t wdog_id;
#if dg_configSUOTA_SUPPORT
        ble_service_t *suota;
#endif
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

#if dg_configSUOTA_SUPPORT
        /* Set maximum allowed MTU to increase SUOTA throughput */
        ble_gap_mtu_size_set(512);
#endif

        /* Get device name from NVPARAM if valid or use default otherwise */
        name_len = read_name(MAX_NAME_LEN, name_buf);

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

#if (PX_REPORTER_INCLUDE_BAS == 1)
        /*
         * Add Battery Service
         *
         * \note Battery Service is not included in PXP specification, but it is included in
         *       pxp_reporter demo application in order to also monitor the device battery level.
         */
        bas = bas_init(NULL, NULL);
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */

#if dg_configSUOTA_SUPPORT
        /* Add SUOTA Service */
        suota = suota_init(&suota_cb);
        OS_ASSERT(suota != NULL);

        /* Add Device Information Service */
        dis_init(NULL, &dis_info);
#endif

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

#if (PX_REPORTER_INCLUDE_BAS == 1)
        /* Create timer for battery monitoring */
        bas_tim = OS_TIMER_CREATE("bas", OS_MS_2_TICKS(PX_REPORTER_BATTERY_CHECK_INTERVAL), true,
                                                (void *) OS_GET_CURRENT_TASK(), bas_tim_cb);
#endif /* (PX_REPORTER_INCLUDE_BAS == 1) */

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
#if (dg_configSUOTA_SUPPORT == 1)
#if (dg_configBLE_2MBIT_PHY == 1)
                                case BLE_EVT_GAP_PHY_SET_COMPLETED:
                                        handle_ble_evt_gap_phy_set_completed((ble_evt_gap_phy_set_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_PHY_CHANGED:
                                        handle_ble_evt_gap_phy_changed((ble_evt_gap_phy_changed_t *) hdr);
                                        break;
#endif /* (dg_configBLE_2MBIT_PHY == 1) */
#endif /* (dg_configSUOTA_SUPPORT == 1) */
#if dg_configSUOTA_SUPPORT && defined(SUOTA_PSM)
                                case BLE_EVT_L2CAP_CONNECTED:
                                case BLE_EVT_L2CAP_DISCONNECTED:
                                case BLE_EVT_L2CAP_DATA_IND:
                                        suota_l2cap_event(suota, hdr);
                                        break;
#endif
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

#if dg_configSUOTA_SUPPORT
                                /*
                                 * Ignore this if SUOTA is ongoing - it's possible to start SUOTA
                                 * before reduced power parameters are applied so this would switch
                                 * to a long connection interval.
                                 */
                                if (!suota_ongoing) {
#endif
#if !dg_configSUOTA_SUPPORT || PX_REPORTER_SUOTA_POWER_SAVING
                                        conn_param_update(conn_dev->conn_idx);
#endif
#if dg_configSUOTA_SUPPORT
                                }
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

