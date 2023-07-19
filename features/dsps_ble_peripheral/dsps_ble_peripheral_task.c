/**
 ****************************************************************************************
 *
 * @file dsps_ble_peripheral_task.c
 *
 * @brief DSPS BLE peripheral demo implements server role of Dialog Serial Port Service.
 *        It reads from input serial port, sends data to client, and finally writes
 *        received data to output serial port.
 *
 * Copyright (C) 2023. Dialog Semiconductor, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "osal.h"
#include "sys_watchdog.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_service.h"
#include "ble_uuid.h"
#include "ble_l2cap.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_gattc.h"
#include "hw_gpio.h"
#include "dsps.h"
#if defined(DSPS_UART)
# include "dsps_uart.h"
#endif
#include "dsps_queue.h"
#include "misc.h"
#include "dsps_common.h"
#include "dsps_port.h"
#include "platform_devices.h"
#if dg_configSUOTA_SUPPORT
# include "dis.h"
# include "dlg_suota.h"
# include "sw_version.h"
#endif /* dg_configSUOTA_SUPPORT */
#if defined(DSPS_USBD)
# include "USB_CDC.h"
# include "dsps_usbd.h"
#endif
#if GENERATE_RANDOM_DEVICE_ADDRESS
# include "sys_trng.h"
# include "ad_nvms.h"
# include "platform_nvparam.h"
# include "gap.h"
#endif

/**
 * Task notifications and handles
 */
#define SPS_DATA_READ_NOTIF     (1 << 1)
#define SPS_START_READ_NOTIF    (1 << 2)
#define SPS_BLE_TX_NOTIF        (1 << 3)
#define SPS_DATA_WRITE_NOTIF    (1 << 4)
#define UPDATE_CONN_PARAM_NOTIF (1 << 5)

#if dg_configSUOTA_SUPPORT
/*
 * Store information about ongoing SUOTA.
 */
__RETAINED_RW static bool suota_ongoing = false;
#endif /* dg_configSUOTA_SUPPORT */

__RETAINED static sps_queue_t *rx_queue;
__RETAINED static sps_queue_t *tx_queue;
/* SPS Service instance */
__RETAINED static dsps_service_t *dsps;
__RETAINED static OS_TASK ble_periph_task_handle;
__RETAINED static OS_TASK dsps_rx_task_handle;
__RETAINED static OS_TASK dsps_tx_task_handle;
#if defined(DSPS_UART)
__RETAINED static ad_uart_handle_t uart_handle;
#endif
__RETAINED static uint8_t dsps_data[DSPS_RX_SIZE];

/* OS timer for connection parameter update */
__RETAINED static OS_TIMER conn_param_timer;

/* Current connection index */
__RETAINED_RW static uint16_t conn_idx = BLE_CONN_IDX_INVALID;

/* Flag for TX in progress */
__RETAINED_RW static bool dsps_tx_in_inprogress = false;

/* Serial RX size */
__RETAINED_RW static uint32_t dsps_rx_size = DSPS_RX_SIZE;

#if defined(DSPS_UART)
/* UART RX timeout */
__RETAINED_RW static uint32_t uart_rx_timeout = 1000;
#endif

/* Flag for indicating UART is ready to read */
__RETAINED_RW static bool dsps_read_ready = false;

static void update_device_address(void)
{
#if GENERATE_RANDOM_DEVICE_ADDRESS
        own_address_t addr;
        ble_gap_address_get(&addr);

        static const uint8_t default_addr[] = defaultBLE_STATIC_ADDRESS;

        if (memcmp(addr.addr, default_addr, BD_ADDR_LEN) == 0) {

                uint8_t offset = NVPARAM_OFFSET_BLE_PLATFORM_BD_ADDRESS;
                nvms_t nvms;

                /* Generate a random address */
                sys_trng_get_bytes(addr.addr, BD_ADDR_LEN);

                /* The two MSBs of a random static address have to be 1 */
                addr.addr[BD_ADDR_LEN - 1] |= GAP_STATIC_ADDR;

                nvms = ad_nvms_open(NVMS_PARAM_PART);
                if (!nvms) {
                        DBG_LOG("\r\nCannot open NVMS_PARAM_PART, storing device address failed.\r\n");
                        return;
                }

                ad_nvms_write(nvms, offset, addr.addr, BD_ADDR_LEN);
                ad_nvms_write(nvms, BD_ADDR_LEN, &offset, sizeof(offset));

                /*
                 * SDK assumes that a stored address is public so we need to
                 * check for random static address and set the correct type.
                 */
                if ((addr.addr[BD_ADDR_LEN - 1] & 0xC0) == GAP_STATIC_ADDR) {
                        addr.addr_type = PRIVATE_STATIC_ADDRESS;
                        ble_gap_address_set(&addr, 0);
                }
        }
#endif /* GENERATE_RANDOM_DEVICE_ADDRESS */
}

static void print_device_address(void)
{
        own_address_t addr;
        ble_gap_address_get(&addr);

        DBG_LOG("Device address: ");

        /* Address should be displayed inverted */
        for (int i = BD_ADDR_LEN - 1; i >= 0; --i) {
                DBG_LOG("%02X", addr.addr[i]);
                if (i != 0) {
                        DBG_LOG(":");
                }
        }
        DBG_LOG("\n\rAddress type = %d\n\r", addr.addr_type);
}

/* Throughput calculation function: based on SIZE_TO_CAL_THROUGHPUT and OS clock */
static void throughput_calculation(uint32_t dataSize, SPS_DIRECTION direction)
{
        (void)dataSize;

#if THROUGHPUT_CALCULATION_ENABLE
        static uint32_t former_ticks[SPS_DIRECTION_MAX] = { [0 ... SPS_DIRECTION_MAX - 1] = 0 };
        static uint32_t accumulated_size[SPS_DIRECTION_MAX] = { [0 ... SPS_DIRECTION_MAX - 1] = 0 };

        if (accumulated_size[direction] > DATA_THRESHOLD_TO_CAL_THROUGHPUT)
        {
                uint32_t passed_ms;

                TickType_t currentTicks = OS_GET_TICK_COUNT();

                if (currentTicks > former_ticks[direction])
                {
                        passed_ms = OS_TICKS_2_MS(currentTicks - former_ticks[direction]);
                }
                else
                {
                        passed_ms = OS_TICKS_2_MS(OS_TIMER_FOREVER - former_ticks[direction] + 1 + currentTicks);
                }
                DBG_LOG("%s throughput is %ld bytes/s.\r\n", direction == SPS_DIRECTION_IN ? "IN" : "OUT",
                                                                        accumulated_size[direction] * 1000 / passed_ms);

                accumulated_size[direction] = 0;
        }
        else
        {
                if (0 == accumulated_size[direction])
                {
                        former_ticks[direction] = OS_GET_TICK_COUNT();
                }

                accumulated_size[direction] += dataSize;
        }
#endif /* THROUGHPUT_CALCULATION_ENABLE */
}

/* Return static buffer with formatted address */
static const char *format_bd_address(const bd_address_t *addr)
{
        static char buf[19];
        int i;

        for (i = 0; i < sizeof(addr->addr); i++) {
                int idx;

                /* For printout, address should be reversed */
                idx = sizeof(addr->addr) - i - 1;
                sprintf(&buf[i * 3], "%02x:", addr->addr[idx]);
        }

        buf[sizeof(buf) - 2] = '\0';

        return buf;
}

/* Timer callback to notify task for connection parameters update */
static void conn_params_timer_cb(OS_TIMER timer)
{
        OS_TASK_NOTIFY(ble_periph_task_handle, UPDATE_CONN_PARAM_NOTIF, OS_NOTIFY_SET_BITS);
}

/* Update connection parameters with pre-defined macro*/
static void conn_param_update(uint16_t conn_idx)
{
        gap_conn_params_t cp;

        cp.interval_min = defaultBLE_PPCP_INTERVAL_MIN;
        cp.interval_max = defaultBLE_PPCP_INTERVAL_MAX;
        cp.slave_latency = defaultBLE_PPCP_SLAVE_LATENCY;
        cp.sup_timeout = defaultBLE_PPCP_SUP_TIMEOUT;

        ble_gap_conn_param_update(conn_idx, &cp);
}

/* Function sets SPS flow control signal to server. */
static void set_flow_control_cb(ble_service_t *svc, uint16_t conn_idx, DSPS_FLOW_CONTROL value)
{
        dsps_set_flow_control((dsps_service_t *)svc, conn_idx, value);

        switch(value) {
        case DSPS_FLOW_CONTROL_ON:
                DBG_LOG("SPS flow control is ON\r\n");

                OS_TASK_NOTIFY(dsps_rx_task_handle, SPS_START_READ_NOTIF,
                                        OS_NOTIFY_SET_BITS); // Kickoff input SPS read when SPS flow on
                OS_TASK_NOTIFY(ble_periph_task_handle, SPS_BLE_TX_NOTIF,
                                        OS_NOTIFY_SET_BITS); // Kickoff BLE TX when SPS flow on
                break;
        case DSPS_FLOW_CONTROL_OFF:
                DBG_LOG("SPS flow control is OFF\r\n");
                break;
        default:
                DBG_LOG("unknown flow control notification\r\n");
                break;
        }
}

static void rx_data_available(void)
{
        bool send_flow_on = false;
        msg rx_msg;
        bool ret;

        /**
         * Pop data from RX queue. Make sure msg is not 0 size or queue is not empty.
         */
        ret = sps_queue_pop_items(rx_queue, &rx_msg);
        if (rx_msg.size == 0 || ret == false) {
                return;
        }

#if defined(DSPS_UART)
        SERIAL_PORT_WRITE_DATA(uart_handle, (const char *)rx_msg.data, rx_msg.size, 0/*Not used*/);
#elif defined(DSPS_USBD)
        USB_CDC_HANDLE usbd_handle = cdc_usbd_get_handle();

        /* -1 means that the USB device has yet to be initialized (i.e. the device is not attached/enumerated). */
        if (usbd_handle >= 0) {
                /*
                 * 0 timeout indicates an infinite timeout (blocking operation).
                 * -1 timeout indicates that the underlying USB routine will not block and return immediately as the write
                 * operation will be performed asynchronously.
                 * Otherwise, the timeout is interpreted in millisecond (blocking operation).
                 */
                SERIAL_PORT_WRITE_DATA(usbd_handle, (const void *)rx_msg.data, (unsigned)rx_msg.size, 0);
        }

#endif
        /* Here you can add some kind of check to make sure that all bytes requested were transmitted. */

        throughput_calculation(rx_msg.size, SPS_DIRECTION_OUT);

        msg_release(&rx_msg);

        /* Check if queue is almost empty and send SPS flow on if necessary */
        send_flow_on = sps_queue_check_almost_empty(rx_queue);
        if (send_flow_on) {
                set_flow_control_cb((ble_service_t *)dsps, conn_idx, DSPS_FLOW_CONTROL_ON);

                DBG_LOG("SPS flow on due to LWM\r\n");
        }

        /* More data in queue -> notify TX task for write */
        if (sps_queue_item_count(rx_queue)) {
                OS_TASK_NOTIFY(dsps_tx_task_handle, SPS_DATA_WRITE_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

/* This callback notifies us that length number of bytes have been received from client */
static void rx_data_cb(ble_service_t *svc, uint16_t conn_idx, const uint8_t *value, uint16_t length)
{
        dsps_service_t *sps = (dsps_service_t *) svc;
        bool send_flow_off = false;

        sps_queue_write_items(rx_queue, length, value);

        /* Check if queue is almost full and issue flow off, if so. */
        send_flow_off = sps_queue_check_almost_full(rx_queue);
        if (send_flow_off) {
                /* Note: Certain number of on-the-fly packets might come even after SPS flow off */
                sps->cb->set_flow_control(svc, conn_idx, DSPS_FLOW_CONTROL_OFF);

                DBG_LOG("SPS flow off due to HWM\r\n");
        }

        /* Write data to output serial port */
        OS_TASK_NOTIFY(dsps_tx_task_handle, SPS_DATA_WRITE_NOTIF, OS_NOTIFY_SET_BITS);
}

static void tx_data_available(void)
{
        __UNUSED msg tx_msg;
        bool ret;

        /* Do not continue if TX is already in progress */
        if (dsps_tx_in_inprogress) {
                return;
        }

        /* Read one msg from TX queue */
        ret = sps_queue_read_items(tx_queue, &tx_msg);
        if (tx_msg.size == 0 || ret == false) {
                return;
        }

        throughput_calculation(tx_msg.size, SPS_DIRECTION_IN);

        /* Send data through BLE */
        ret = dsps_tx_data(dsps, conn_idx, tx_msg.data, tx_msg.size);

        if (ret) {
                dsps_tx_in_inprogress = true;
        }
}

/* This callback notifies us that length number of bytes have been transferred to client. */
static void tx_done_cb(ble_service_t *svc, uint16_t conn_idx)
{
        dsps_tx_in_inprogress = false;

        msg m;
        bool send_flow_on = false;

        /* Pop and release the transmitted msg */
        sps_queue_pop_items(tx_queue, &m);
        msg_release(&m);

        /* Check if queue is almost empty and send SPS flow off if necessary */
        send_flow_on = sps_queue_check_almost_empty(tx_queue);
        if(send_flow_on) {

#if defined(DSPS_UART)
# if defined(CFG_UART_HW_FLOW_CTRL)
                SERIAL_PORT_SET_FLOW_ON(&UART_DEVICE);
# elif defined(CFG_UART_SW_FLOW_CTRL)
                SERIAL_PORT_SET_FLOW_ON(uart_handle);
# endif
#elif defined(DSPS_USBD)
                USB_CDC_HANDLE usbd_handle = cdc_usbd_get_handle();

                /* -1 means that the USB device has yet to be initialized (i.e. the device is not attached/enumerated). */
                if (usbd_handle >= 0) {
                        SERIAL_PORT_SET_FLOW_ON(usbd_handle);
                }
#endif

                dsps_read_ready = true;
                OS_TASK_NOTIFY(dsps_rx_task_handle, SPS_START_READ_NOTIF, OS_NOTIFY_SET_BITS); // Kickoff input serial port read when SPS flow is on

                DBG_LOG("SERIAL flow on due to LWM\r\n");
        }

        /* More data in queue -> notify BLE task for TX */
        if (sps_queue_item_count(tx_queue)) {
                OS_TASK_NOTIFY(ble_periph_task_handle, SPS_BLE_TX_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

static dsps_callbacks_t sps_callbacks = {
        .set_flow_control = set_flow_control_cb,
        .rx_data = rx_data_cb,
        .tx_done = tx_done_cb,
};

/*
 * BLE event handlers
 */
static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        conn_idx = evt->conn_idx;
        DBG_LOG("%s: conn_idx=%04x address=%s CI max is %u. \r\n", __func__, evt->conn_idx, \
                format_bd_address(&evt->peer_address), evt->conn_params.interval_max);

        /* Create and start one-time timer for connection parameter update */
        conn_param_timer = OS_TIMER_CREATE("conn_param", OS_MS_2_TICKS(500),
                                OS_TIMER_FAIL, (uint32_t) conn_idx, conn_params_timer_cb);
        OS_TIMER_START(conn_param_timer, OS_TIMER_FOREVER);

        /**
         * Create TX and RX SPS queues
         */
        rx_queue = sps_queue_new(RX_SPS_QUEUE_SIZE, RX_QUEUE_LWM, RX_QUEUE_HWM);
        tx_queue = sps_queue_new(TX_SPS_QUEUE_SIZE, TX_QUEUE_LWM, TX_QUEUE_HWM);

#if defined(DSPS_UART)
        uart_handle = SERIAL_PORT_OPEN(&UART_DEVICE);
        ASSERT_WARNING(uart_handle);
#elif defined(DSPS_USBD)
#endif

#if defined(DSPS_UART)
# if defined(CFG_UART_HW_FLOW_CTRL)
        SERIAL_PORT_SET_FLOW_ON(&UART_DEVICE);
# elif defined(CFG_UART_SW_FLOW_CTRL)
        SERIAL_PORT_SET_FLOW_ON(uart_handle);
# endif
#elif defined(DSPS_USBD)
        USB_CDC_HANDLE usbd_handle = cdc_usbd_get_handle();

        /* -1 means that the USB device has yet to be initialized (i.e. the device is not attached/enumerated). */
        if (usbd_handle >= 0) {
                SERIAL_PORT_SET_FLOW_ON(usbd_handle);
        }
#endif

       dsps_read_ready = true;
}

static void handle_evt_gap_mtu_exchanged(ble_evt_gattc_mtu_changed_t *evt)
{
        /* Update the UART read size and timeout accordingly */
        dsps_rx_size = evt->mtu - 3;

#if defined(DSPS_UART)
        uart_rx_timeout = uart_read_timeout(CFG_UART_SPS_BAUDRATE, dsps_rx_size);
#endif

        DBG_LOG("Peripheral exchanged MTU size is %u.\r\n", evt->mtu);
}

static void handle_evt_gap_datalength_changed(ble_evt_gap_data_length_changed_t * evt)
{
        (void)evt;

        DBG_LOG("Peripheral exchanged tx data length is %u, rx data length is %u.\r\n",
                                                                evt->max_tx_length, evt->max_rx_length);
}

static void handle_evt_gap_conn_param_updated(ble_evt_gap_conn_param_updated_t * evt)
{
        (void)evt;

        DBG_LOG("Peripheral updated CI min is %u, CI max is %u.\r\n",
                                evt->conn_params.interval_min, evt->conn_params.interval_max);
        ble_gattc_exchange_mtu(conn_idx); // Exchange MTU with peer
}

static void handle_evt_gap_conn_param_update_completed(ble_evt_gap_conn_param_update_completed_t * evt)
{
        if (evt->status != BLE_STATUS_OK) {
                DBG_LOG("Peripheral update unsuccessful, status is %u.\r\n", evt->status);
                ble_gattc_exchange_mtu(conn_idx); // Exchange MTU with peer
        }
}

static void handle_disconnected(ble_evt_gap_disconnected_t *evt)
{
        (void)evt;

        DBG_LOG("%s: conn_idx=%04x address=%s reason=%d\r\n", __func__, evt->conn_idx, format_bd_address(&evt->address), evt->reason);

        /* Reset connection index (this will also stop sending UART_START_READ_NOTIF */
        conn_idx = BLE_CONN_IDX_INVALID;

#if defined(DSPS_UART)
# if defined(CFG_UART_HW_FLOW_CTRL)
        SERIAL_PORT_SET_FLOW_OFF(&UART_DEVICE);
# elif defined(CFG_UART_SW_FLOW_CTRL)
        SERIAL_PORT_SET_FLOW_OFF(uart_handle);
# endif
#elif defined(DSPS_USBD)
        USB_CDC_HANDLE usbd_handle = cdc_usbd_get_handle();

        /* -1 means that the USB device has yet to be initialized (i.e. the device is not attached/enumerated). */
        if (usbd_handle >= 0) {
                SERIAL_PORT_SET_FLOW_OFF(usbd_handle);
        }
#endif

        dsps_read_ready = false;
        /*
         * Reset variable here. It might happen that the peer device (central) is disconnected
         * while the latter receives bytes and tx_done_cb() is never called to reset it.
         */
        dsps_tx_in_inprogress = false;

#if defined(DSPS_UART)
        /* Let UART activity finish */
        OS_DELAY_MS(uart_rx_timeout);

        SERIAL_PORT_CLOSE(uart_handle);
#elif defined(DSPS_USBD)
#endif

        /* Delete TX and RX queue */
        sps_queue_free(tx_queue);
        sps_queue_free(rx_queue);
        tx_queue = NULL;
        rx_queue = NULL;

        /* Delete timer for connection parameter update */
        OS_TIMER_DELETE(conn_param_timer, OS_TIMER_FOREVER);

        /* Start advertising */
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

#if (dg_configBLE_2MBIT_PHY == 1)
static void handle_ble_evt_gap_phy_set_completed(ble_evt_gap_phy_set_completed_t* evt)
{
        (void)evt;

        /*
         * Add code to handle the case where the preferred PHY settings
         * failed to be set (evt->status != BLE_STATUS_OK).
         */
        DBG_LOG("Peripheral PHY set status is %u.\r\n", evt->status);
}

static void handle_ble_evt_gap_phy_changed(ble_evt_gap_phy_changed_t* evt)
{
        (void)evt;

        /*
         * Add code to handle the PHY changed event.
         */
        DBG_LOG("Peripheral exchanged tx PHY is %u, rx PHY is %u.\r\n", evt->tx_phy, evt->rx_phy);
}
#endif /* (dg_configBLE_2MBIT_PHY == 1) */

#if dg_configSUOTA_SUPPORT
/* Callback from SUOTA implementation */
static bool suota_ready_cb(uint16_t conn_idx)
{
        /*
         * This callback is used so application can accept/block SUOTA.
         * Also, before SUOTA starts, user might want to do some actions
         * e.g. disable sleep mode.
         *
         * If true is returned, then advertising is stopped and SUOTA
         * is started. Otherwise SUOTA is canceled.
         *
         */
        suota_ongoing = true;

        return true;
}

static void suota_status_changed_cb(uint16_t conn_idx, uint8_t status, uint8_t error_code)
{
        /* In case SUOTA finished with an error, we just restore default connection parameters. */
        if (status != SUOTA_ERROR) {
                return;
        }
}

static const suota_callbacks_t suota_cb = {
        .suota_ready = suota_ready_cb,
        .suota_status = suota_status_changed_cb,
};

/*
 * Device Information Service data
 *
 * Manufacturer Name String is mandatory for devices supporting HRP.
 */
static const dis_device_info_t dis_info = {
        .manufacturer  = defaultBLE_DIS_MANUFACTURER,
        .model_number  = defaultBLE_DIS_MODEL_NUMBER,
        .serial_number = defaultBLE_DIS_SERIAL_NUMBER,
        .hw_revision   = defaultBLE_DIS_HW_REVISION,
        .fw_revision   = defaultBLE_DIS_FW_REVISION,
        .sw_revision   = defaultBLE_DIS_SW_REVISION,
};
#endif /* dg_configSUOTA_SUPPORT */

/*
 * main task code
 */
void dsps_BLE_task(void *params)
{
        att_uuid_t sps_uuid;
        int8_t wdog_id;

#if dg_configSUOTA_SUPPORT
        ble_service_t *suota;
#endif

        /* Scan response data containing name   */
        static const uint8_t scan_rsp[] = {
                0x11, GAP_DATA_TYPE_LOCAL_NAME,
                'R', 'e', 'n', 'e', 's', 'a','s', ' ', 'S', 'P', 'S', ' ', 'D', 'e', 'm', 'o'
        };

        /* register sps task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        ble_periph_task_handle = OS_GET_CURRENT_TASK();

        ble_peripheral_start();
        ble_register_app();
        ble_gap_mtu_size_set(MTU_SIZE);

        update_device_address();
        print_device_address();

        /**
         * Set device name
         */
        ble_gap_device_name_set("Renesas SPS Demo", ATT_PERM_READ);

        /**
         * Start Serial Port Service
         */
        ble_service_t *svc = dsps_init(&sps_callbacks);
        ble_service_add(svc);
        dsps = (dsps_service_t *) svc;

#if dg_configSUOTA_SUPPORT
        /*
         * Register SUOTA
         *
         * SUOTA instance should be registered in ble_service framework in order for events inside
         * service to be processed properly.
         */
        suota = suota_init(&suota_cb);
        OS_ASSERT(suota != NULL);
        ble_service_add(suota);

        /*
         * Register DIS
         *
         * DIS doesn't contain any dynamic data thus it doesn't need to be registered in ble_service
         * framework (but it's not an error to do so).
         */
        dis_init(NULL, &dis_info);
#endif
        /* Setup advertising */
        ble_uuid_from_string(UUID_DSPS, &sps_uuid);

#if dg_configSUOTA_SUPPORT
        /* Advertising data     */
        static uint8_t adv_data[22] = {
                0x03, GAP_DATA_TYPE_UUID16_LIST, 0xF5, 0xFE, // = 0xFEF5 (DIALOG SUOTA UUID)
                0x11, GAP_DATA_TYPE_UUID128_LIST, /* SPS UUID will be put here */
        };
        memcpy(adv_data + 6, sps_uuid.uuid128, sizeof(sps_uuid.uuid128));
#else
        /* Advertising data */
        static uint8_t adv_data[18] = {
                0x11, GAP_DATA_TYPE_UUID128_LIST,/* UUID will be put here */
        };
        memcpy(adv_data + 2, sps_uuid.uuid128, sizeof(sps_uuid.uuid128));
#endif /* dg_configSUOTA_SUPPORT */

        ble_gap_adv_data_set(sizeof(adv_data), adv_data, sizeof(scan_rsp), scan_rsp);

        /* Start advertising */
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        DBG_LOG("DSPS Peripheral Task Ready.\r\n");

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /* Notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* Suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for task notification. The return value must be OS_OK */
                OS_ASSERT(ret == OS_OK);

                /* Resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                /* Notified from BLE manager, can get event */
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
                        case BLE_EVT_GAP_DATA_LENGTH_CHANGED:
                                handle_evt_gap_datalength_changed((ble_evt_gap_data_length_changed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_CONN_PARAM_UPDATED:
                                handle_evt_gap_conn_param_updated((ble_evt_gap_conn_param_updated_t *) hdr);
                                break;
                        case BLE_EVT_GAP_CONN_PARAM_UPDATE_COMPLETED:
                                handle_evt_gap_conn_param_update_completed((ble_evt_gap_conn_param_update_completed_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_MTU_CHANGED:
                                handle_evt_gap_mtu_exchanged((ble_evt_gattc_mtu_changed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_DISCONNECTED:
                                handle_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PAIR_REQ:
                        {
                                ble_evt_gap_pair_req_t *evt = (ble_evt_gap_pair_req_t *) hdr;
                                ble_gap_pair_reply(evt->conn_idx, true, evt->bond);
                                break;
                        }
#if (dg_configBLE_2MBIT_PHY == 1)
                        case BLE_EVT_GAP_PHY_SET_COMPLETED:
                                handle_ble_evt_gap_phy_set_completed((ble_evt_gap_phy_set_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PHY_CHANGED:
                                handle_ble_evt_gap_phy_changed((ble_evt_gap_phy_changed_t *) hdr);
                                break;
#endif /* (dg_configBLE_2MBIT_PHY == 1) */
#if dg_configSUOTA_SUPPORT && defined (SUOTA_PSM)
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

handled:
                        OS_FREE(hdr);

no_event:
                        /* Notify again if there are more events to process in queue */
                        if (ble_has_event()) {
                                OS_TASK_NOTIFY(ble_periph_task_handle, BLE_APP_NOTIFY_MASK, OS_NOTIFY_SET_BITS);
                        }
                }

                if (notif & SPS_BLE_TX_NOTIF) {
                        tx_data_available();
                }

                if (notif & UPDATE_CONN_PARAM_NOTIF) {
                        conn_param_update(conn_idx);
                }
        }
}

void dsps_rx_task(void *params)
{
        static uint32_t ReadSize = 0;

        dsps_rx_task_handle = OS_GET_CURRENT_TASK();

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Guaranteed to return since we're waiting forever */
                OS_ASSERT(ret == OS_OK);

                if (notif & SPS_DATA_READ_NOTIF) {
                        sps_queue_write_items(tx_queue, ReadSize, dsps_data);

                        bool send_flow_off = false;
                        /* Check if queue is almost full and issue to send a SPS flow off, if so. */
                        send_flow_off = sps_queue_check_almost_full(tx_queue);

                        if (send_flow_off) {

#if defined(DSPS_UART)
# if defined(CFG_UART_HW_FLOW_CTRL)
                                SERIAL_PORT_SET_FLOW_OFF(&UART_DEVICE);
# elif defined(CFG_UART_SW_FLOW_CTRL)
                                SERIAL_PORT_SET_FLOW_OFF(uart_handle);
# endif
#elif defined(DSPS_USBD)
                                USB_CDC_HANDLE usbd_handle = cdc_usbd_get_handle();

                                /* -1 means that the USB device has yet to be initialized (i.e. the device is not attached/enumerated). */
                                if (usbd_handle >= 0) {
                                        SERIAL_PORT_SET_FLOW_OFF(usbd_handle);
                                }
#endif

                                dsps_read_ready = false;

                                DBG_LOG("SERIAL flow off due to HWM\r\n");
                        }

                        /* Notify BLE task for TX */
                        OS_TASK_NOTIFY(ble_periph_task_handle, SPS_BLE_TX_NOTIF, OS_NOTIFY_SET_BITS);

                        /* Continue reading from input serial port */
                        OS_TASK_NOTIFY(dsps_rx_task_handle, SPS_START_READ_NOTIF, OS_NOTIFY_SET_BITS);
                }
                if (notif & SPS_START_READ_NOTIF) {
                        /* Must be connected with peer and the SPS flow should be ON */
                        if ((conn_idx != BLE_CONN_IDX_INVALID) && dsps_read_ready) {

                                /* Read from input serial port with calculated timeout */
#if defined(DSPS_UART)
                                ReadSize = SERIAL_PORT_READ_DATA(uart_handle,
                                        (char *)dsps_data, dsps_rx_size, OS_MS_2_TICKS(uart_rx_timeout));
#elif defined(DSPS_USBD)
                                USB_CDC_HANDLE usbd_handle = cdc_usbd_get_handle();

                                /* -1 means that the USB device has yet to be initialized (i.e. the device is not attached/enumerated). */
                                if (usbd_handle >= 0) {
                                        /*
                                         * 0 timeout indicates an infinite timeout (blocking operation).
                                         * -1 timeout means the underlying USB routine will never block and will only read data
                                         * from the internal EndPoint buffer (if any and up to the size requested or less).
                                         * Otherwise the timeout is interpreted in millisecond.
                                         */
                                        ReadSize = SERIAL_PORT_READ_DATA(usbd_handle, (void *)dsps_data,
                                                                                        (unsigned)dsps_rx_size, 0);
                                }
#endif
                                if (ReadSize > 0 /* In USB device the returned value might be negative indicating some kind of error */) {
                                        OS_TASK_NOTIFY(dsps_rx_task_handle, SPS_DATA_READ_NOTIF, OS_NOTIFY_SET_BITS);
                                }
                                else {
                                        OS_TASK_NOTIFY(dsps_rx_task_handle, SPS_START_READ_NOTIF, OS_NOTIFY_SET_BITS);
                                }
                        }
                }
        }
}

void dsps_tx_task(void *params)
{
        dsps_tx_task_handle = OS_GET_CURRENT_TASK();

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Guaranteed to return since we're waiting forever */
                OS_ASSERT(ret == OS_OK);

                if (notif & SPS_DATA_WRITE_NOTIF) {
                        rx_data_available();
                }
        }
}
