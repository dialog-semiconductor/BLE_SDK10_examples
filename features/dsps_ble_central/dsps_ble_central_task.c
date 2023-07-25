/**
 ****************************************************************************************
 *
 * @file dsps_ble_central_task.c
 *
 * @brief DSPS BLE central demo implements client role of Dialog Serial Port Service.
 *        It reads from input serial port, sends data to DSPS server and finally writes
 *        received data to output serial port.
 *
 * Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
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
#include "osal.h"
#include "sys_watchdog.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_config.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_gattc.h"
#include "ble_service.h"
#include "ble_uuid.h"
#include "dsps_queue.h"
#include "dsps.h"
#if defined(DSPS_UART)
   #include "dsps_uart.h"
#endif
#include "misc.h"
#include "dsps_common.h"
#include "dsps_port.h"
#include "platform_devices.h"
#if defined(DSPS_USBD)
   #include "USB_CDC.h"
   #include <dsps_usbd.h>
#endif
#if GENERATE_RANDOM_DEVICE_ADDRESS
# include "sys_trng.h"
# include "ad_nvms.h"
# include "platform_nvparam.h"
# include "gap.h"
#endif

/* SPS handle instance to store the handles */
typedef struct {
        uint16_t sps_tx_val_h;
        uint16_t sps_tx_ccc_h;

        uint16_t sps_rx_val_h;

        uint16_t sps_flow_ctrl_val_h;
        uint16_t sps_flow_ctrl_ccc_h;
} dsps_central_t;

#define APP_BLE_GAP_CALL_FUNC_UNTIL_NO_ERR(_func, args...) \
        {                                                  \
                ble_error_t ret;                           \
                uint32_t loop = 0;                         \
                                                           \
                while (true) {                             \
                        ret = (_func)(args);               \
                                                           \
                        if (ret == BLE_STATUS_OK) {        \
                               break;                      \
                        }                                  \
                        OS_DELAY_MS(5);                    \
                        loop++;                            \
                                                           \
                        DBG_LOG("%s failed with status = %d (%lu)", #_func, ret, loop); \
                }                                          \
        }

#define CONN_TIMEOUT_MS        ( 15000 )

/**
 * Task notifications and handles
 */
#define SPS_DATA_READ_NOTIF    (1 << 1)
#define SPS_START_READ_NOTIF   (1 << 2)
#define SPS_BLE_TX_NOTIF       (1 << 3)
#define SPS_DATA_WRITE_NOTIF   (1 << 4)
#define BLE_DISCOVER_NOTIF     (1 << 5)
#define BLE_SCAN_START_NOTIF   (1 << 6)
#define BLE_CONN_TIMEOUT_NOTIF (1 << 7)

#define BLE_SCAN_INTERVAL      (BLE_SCAN_INTERVAL_FROM_MS(30))
#define BLE_SCAN_WINDOW        (BLE_SCAN_WINDOW_FROM_MS(15))

__RETAINED static sps_queue_t *rx_queue;
__RETAINED static sps_queue_t *tx_queue;
__RETAINED static dsps_central_t *dsps;
__RETAINED static OS_TASK ble_central_task_handle;
__RETAINED static OS_TASK dsps_rx_task_handle;
__RETAINED static OS_TASK dsps_tx_task_handle;
#if defined(DSPS_UART)
   __RETAINED static ad_uart_handle_t uart_handle;
#endif
__RETAINED static bd_address_t peer_addr;
__RETAINED static uint8_t dsps_data[DSPS_RX_SIZE];
__RETAINED static OS_TIMER conn_timeout_h;

__RETAINED_RW static gap_conn_params_t cp = {
        .interval_min  = defaultBLE_PPCP_INTERVAL_MIN,   // in unit of 1.25ms
        .interval_max  = defaultBLE_PPCP_INTERVAL_MAX,   // in unit of 1.25ms
        .slave_latency = defaultBLE_PPCP_SLAVE_LATENCY,
        .sup_timeout   = defaultBLE_PPCP_SUP_TIMEOUT,    // in unit of 10ms
};

/*  Current connection index */
__RETAINED_RW static uint16_t conn_idx = BLE_CONN_IDX_INVALID;

/* Current SPS flow control status */
__RETAINED_RW static uint8_t dsps_flow_ctrl = DSPS_FLOW_CONTROL_OFF;

/* Flag for TX in progress */
__RETAINED_RW static bool dsps_tx_in_inprogress = false;

/*  Serial RX size */
__RETAINED_RW static uint32_t dsps_rx_size = DSPS_RX_SIZE;

#if defined(DSPS_UART)
   /* Serial RX timeout */
   __RETAINED_RW static uint32_t uart_rx_timeout = 1000;
#endif

/*  flag for indicating UART is ready to read */
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

/* Return static buffer with formatted UUID */
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

/* Return static buffer with characteristics properties mask */
static const char *format_properties(uint8_t properties)
{
        static const char props_str[] = "BRXWNISE"; // Each letter corresponds to single property
        static char buf[9];
        int i;

        /* Copy full properties mask */
        memcpy(buf, props_str, sizeof(props_str));

        for (i = 0; i < 8; i++) {
                /* Clear letter from mask if property not present */
                if ((properties & (1 << i)) == 0) {
                        buf[i] = '-';
                }
        }

        return buf;
}

/* Function send available TX data to server by write without response approach. */
static bool dsps_send_tx_data_host(dsps_central_t *sps, uint16_t conn_idx, uint8_t *data, uint16_t length)
{
        uint8_t status;

        status = ble_gattc_write_no_resp(conn_idx, sps->sps_rx_val_h, false, length, data);

        return status == BLE_STATUS_OK ? true : false;
}

/* Function sends SPS flow control signal to server. */
static bool dsps_set_flow_control_host(dsps_central_t *sps, uint16_t conn_idx, DSPS_FLOW_CONTROL value)
{
        uint8_t status;

        status = ble_gattc_write_no_resp(conn_idx, sps->sps_flow_ctrl_val_h, false, sizeof(value), &value);

        return status == BLE_STATUS_OK ? true : false;
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
                dsps_set_flow_control_host(dsps, conn_idx, DSPS_FLOW_CONTROL_ON);

                DBG_LOG("SPS flow on due to LWM\r\n");
        }

        /* More data in queue -> notify TX task for write */
        if (sps_queue_item_count(rx_queue)) {
                OS_TASK_NOTIFY(dsps_tx_task_handle, SPS_DATA_WRITE_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

/* This callback notifies us that length number of bytes have been received from client */
static void rx_data_cb(dsps_central_t *sps, uint16_t conn_idx, const uint8_t *value, uint16_t length)
{
        bool send_flow_off = false;

        sps_queue_write_items(rx_queue, length, value);

        /* Check if queue is almost full and issue flow off, if so. */
        send_flow_off = sps_queue_check_almost_full(rx_queue);
        if (send_flow_off) {
                /* Note: Certain number of on-the-fly packets might come even after SPS flow off */
                dsps_set_flow_control_host(sps, conn_idx, DSPS_FLOW_CONTROL_OFF);

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

        if (dsps_flow_ctrl != DSPS_FLOW_CONTROL_ON) {
                return;
        }

        /* Read one msg from TX queue */
        ret = sps_queue_read_items(tx_queue, &tx_msg);
        if (tx_msg.size == 0 || ret == false) {
                return;
        }

        throughput_calculation(tx_msg.size, SPS_DIRECTION_IN);

        ret = dsps_send_tx_data_host(dsps, conn_idx, tx_msg.data, tx_msg.size);
        if (ret) {
                dsps_tx_in_inprogress = true;
        }
}

/* This callback notifies us that length number of bytes have been transferred to client. */
static void tx_done_cb(dsps_central_t *sps, uint16_t conn_idx)
{
        dsps_tx_in_inprogress = false;
        msg m;
        bool send_flow_on = false;

        sps_queue_pop_items(tx_queue, &m);
        msg_release(&m);

        /* Check if queue is almost empty and send SPS flow off if necessary */
        send_flow_on = sps_queue_check_almost_empty(tx_queue);
        if (send_flow_on) {

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
                OS_TASK_NOTIFY(ble_central_task_handle, SPS_BLE_TX_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

static void handle_evt_gap_adv_report(ble_evt_gap_adv_report_t *evt)
{
        DBG_LOG("%s\n\r", __func__);

        gap_device_t gap_device;
        size_t index = 0;

        ble_error_t ret = ble_gap_get_device_by_addr((const bd_address_t *)&evt->address, &gap_device);

        /* Do not process peers already connected */
        if (ret == BLE_STATUS_OK && gap_device.connected) {
                return;
        }

        att_uuid_t uuid128;
        ble_uuid_from_string(UUID_DSPS, &uuid128);

        /* Process TLVs */
        while (index + 2 < evt->length) {
                if (evt->data[index + 1] == GAP_DATA_TYPE_UUID128_LIST) {
                        if (memcmp(&evt->data[index + 2], uuid128.uuid128, ATT_UUID_LENGTH) == 0) {
                                DBG_LOG("DSPS device found = %s\n\r",
                                        ble_address_to_string((const bd_address_t *)&evt->address));

                                OPT_MEMCPY(&peer_addr, &evt->address, sizeof(evt->address));

                                /* Stop scanner. We will attemp to connect to the peer device from there. */
                                ble_gap_scan_stop();
                                break;
                        }
                }
                index += (evt->data[index] + 1); // go to the next TLV
        }
}

static void handle_evt_gap_connection_completed(ble_evt_gap_connection_completed_t *evt)
{
        DBG_LOG("%s, Status = %d\n\r", __func__, evt->status);

        /*
         * Should reach here if a connection request is aborted by the user
         * explicitly by calling ble_gap_connect_cancel().
         */
        if (evt->status == BLE_ERROR_CANCELED) {
                OS_TASK_NOTIFY(ble_central_task_handle, BLE_SCAN_START_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

static void handle_evt_gap_scan_completed(ble_evt_gap_scan_completed_t *evt)
{
        DBG_LOG("%s, Status = %d\n\r", __func__, evt->status);

        /* Process only if scanner was canceled by user. */
        if (evt->status == BLE_ERROR_CANCELED) {
                ble_error_t ret = ble_gap_connect_ce((const bd_address_t *)&peer_addr, &cp,
                                                                defaultBLE_CONN_EVENT_LENGTH_MIN, 0);

                if (ret == BLE_ERROR_BUSY) {
                        DBG_LOG("%s failed with status BLE_ERROR_CANCELED.\n\r", __func__);

                        /*
                         * ble_gap_connect_ce() will return a BLE_ERROR_BUSY status when another connection
                         * procedure is already ongoing. To be able to connect to another device, cancel
                         * the last connection request to retry connecting with the new device.
                         */
                        ble_gap_connect_cancel();
                } else {
                        ASSERT_WARNING(ret == BLE_STATUS_OK);

                        /*
                         * A connection establishment should now be initiated. Start a timer
                         * which will be in charge of making sure that the connection request
                         * is canceled after some time has elapsed and the connection has yet
                         * to be established. It might happen that the requested peer address
                         * is no longer available.
                         */
                        OS_TIMER_START(conn_timeout_h, OS_TIMER_FOREVER);
                }
        } else {
                /* Scanning operations should timeout. Re-start scanner. */
                OS_TASK_NOTIFY(ble_central_task_handle, BLE_SCAN_START_NOTIF, OS_NOTIFY_SET_BITS);
        }
}

/*
 * BLE event handlers
 */
static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        DBG_LOG("%s: conn_idx=%04x address=%s CI max is %u. \r\n", __func__, evt->conn_idx, \
                                format_bd_address(&evt->peer_address), evt->conn_params.interval_max);

        conn_idx = evt->conn_idx;

        ASSERT_WARNING(OS_TIMER_IS_ACTIVE(conn_timeout_h));
        /* Connection has been established; stop connection timer. */
        OS_TIMER_STOP(conn_timeout_h, OS_TIMER_FOREVER);

#if (dg_configBLE_2MBIT_PHY == 1)
        /* Switch to 2Mbit PHY during SUOTA */
        ble_gap_phy_set(conn_idx, BLE_GAP_PHY_PREF_2M, BLE_GAP_PHY_PREF_2M);
#endif /* (dg_configBLE_2MBIT_PHY == 1) */

        /* Notify main thread, we'll start discovery from there. */
        OS_TASK_NOTIFY(ble_central_task_handle, BLE_DISCOVER_NOTIF, OS_NOTIFY_SET_BITS);
}

static void handle_evt_gap_mtu_exchanged(ble_evt_gattc_mtu_changed_t *evt)
{
        /* Update the UART read size and timeout accordingly */
        dsps_rx_size = evt->mtu - 3;

#if defined(DSPS_UART)
        uart_rx_timeout = uart_read_timeout(CFG_UART_SPS_BAUDRATE, dsps_rx_size);
#endif

        DBG_LOG("Central exchanged MTU size is %u\r\n", evt->mtu);
}

static void handle_evt_gap_conn_param_updated_req(ble_evt_gap_conn_param_update_req_t * evt)
{
        DBG_LOG("Central rejected connection parameter update.\r\n");

        /* Reject the connection parameter update to have optimal throughput */
        ble_gap_conn_param_update_reply(evt->conn_idx, false);
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
        DBG_LOG("%s: conn_idx=%04x address=%s reason=%d\r\n", __func__,
                                        evt->conn_idx, format_bd_address(&evt->address), evt->reason);

        /* Reset connection index (this will also stop sending SPS_START_READ_NOTIF) */
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
         * Reset variable here. It might happen that the peer device (peripheral) is disconnected
         * while the latter receives bytes and tx_done_cb() is never called to reset it.
         */
        dsps_tx_in_inprogress = false;

#if defined(DSPS_UART)
        /* Let serial activity to finish */
        OS_DELAY_MS(uart_rx_timeout);

        SERIAL_PORT_CLOSE(uart_handle);
#elif defined(DSPS_USBD)
#endif

        /* Delete TX and RX queue */
        sps_queue_free(tx_queue);
        sps_queue_free(rx_queue);
        tx_queue = NULL;
        rx_queue = NULL;

        /* Notify main thread, we'll start reconnection from there */
        OS_TASK_NOTIFY(ble_central_task_handle, BLE_SCAN_START_NOTIF, OS_NOTIFY_SET_BITS);
}

#if (dg_configBLE_2MBIT_PHY == 1)
static void handle_ble_evt_gap_phy_set_completed(ble_evt_gap_phy_set_completed_t* evt)
{
        /*
         * Add code to handle the case where the preferred PHY settings
         * failed to be set (evt->status != BLE_STATUS_OK).
         */
        DBG_LOG("Peripheral PHY set status is %u.\r\n", evt->status);
}

static void handle_ble_evt_gap_phy_changed(ble_evt_gap_phy_changed_t* evt)
{
        /*
         * Add code to handle the PHY changed event.
         */
        DBG_LOG("Peripheral exchanged tx PHY is %u, rx PHY is %u.\r\n", evt->tx_phy, evt->rx_phy);
}
#endif /* (dg_configBLE_2MBIT_PHY == 1) */

static void handle_evt_gap_security_request(ble_evt_gap_security_request_t *evt)
{
        DBG_LOG("%s: conn_idx=%04x bond=%d\r\n", __func__, evt->conn_idx, evt->bond);

        /* Trigger pairing */
        ble_gap_pair(evt->conn_idx, evt->bond);
}

static void handle_evt_gap_pair_completed(ble_evt_gap_pair_completed_t *evt)
{
        DBG_LOG("%s: conn_idx=%04x status=%d bond=%d mitm=%d\r\n",
                                __func__, evt->conn_idx, evt->status, evt->bond, evt->mitm);
}

static void handle_evt_gattc_browse_svc(ble_evt_gattc_browse_svc_t *evt)
{
        uint8_t prop = 0;
        uint16_t char_handle = 0;
        int i;

        DBG_LOG("%s: conn_idx=%04x start_h=%04x end_h=%04x\r\n",
                                        __func__, evt->conn_idx, evt->start_h, evt->end_h);

        DBG_LOG("\t%04x serv %s\r\n", evt->start_h, format_uuid(&evt->uuid));

        for (i = 0; i < evt->num_items; i++) {
                gattc_item_t *item = &evt->items[i];
                att_uuid_t uuid;

                switch (item->type) {
                case GATTC_ITEM_TYPE_INCLUDE:
                        DBG_LOG("\t%04x incl %s\r\n", item->handle, format_uuid(&item->uuid));
                        break;
                case GATTC_ITEM_TYPE_CHARACTERISTIC:
                        DBG_LOG("\t%04x char %s prop=%02x (%s)\r\n", item->handle,
                                                format_uuid(&item->uuid), item->c.properties,
                                                format_properties(item->c.properties));

                        /*
                         * Compare UUID and find the necessary service characteristic handles
                         * item->handle is the characteristic handle, need to +1 to get the actual value handle.
                         */
                        ble_uuid_from_string(UUID_DSPS_SERVER_TX, &uuid);
                        if (ble_uuid_equal(&uuid, &item->uuid)) {
                                dsps->sps_tx_val_h = item->handle + 1;
                        }
                        ble_uuid_from_string(UUID_DSPS_SERVER_RX, &uuid);
                        if (ble_uuid_equal(&uuid, &item->uuid)) {
                                dsps->sps_rx_val_h = item->handle + 1;
                        }
                        ble_uuid_from_string(UUID_DSPS_FLOW_CTRL, &uuid);
                        if (ble_uuid_equal(&uuid, &item->uuid)) {
                                dsps->sps_flow_ctrl_val_h = item->handle + 1;
                        }

                        /* Store properties, useful when handling descriptor later */
                        prop = item->c.properties;
                        char_handle = item->handle + 1;
                        break;
                case GATTC_ITEM_TYPE_DESCRIPTOR:
                        DBG_LOG("\t%04x desc %s\r\n", item->handle, format_uuid(&item->uuid));

                        /* Enable server notifications by writing CCC handle */
                        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
                        if (ble_uuid_equal(&uuid, &item->uuid) && (prop & GATT_PROP_NOTIFY)) {
                                if (char_handle == dsps->sps_tx_val_h)
                                {
                                        dsps->sps_tx_ccc_h = item->handle;
                                }
                                if (char_handle == dsps->sps_flow_ctrl_val_h)
                                {
                                        dsps->sps_flow_ctrl_ccc_h = item->handle;
                                }
                                uint16_t ccc = GATT_CCC_NOTIFICATIONS;
                                ble_gattc_write(evt->conn_idx, item->handle, 0,
                                                                sizeof(ccc), (uint8_t *) &ccc);
                        }
                        break;
                default:
                        DBG_LOG("\t%04x ????\r\n", item->handle);
                        break;
                }
        }
}

static void handle_evt_gattc_browse_completed(ble_evt_gattc_browse_completed_t *evt)
{
        DBG_LOG("%s: conn_idx=%04x status=%d\r\n", __func__, evt->conn_idx, evt->status);

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

        dsps_set_flow_control_host(dsps, conn_idx, DSPS_FLOW_CONTROL_ON);

        /* Start reading from serial interface */
        OS_TASK_NOTIFY(dsps_rx_task_handle, SPS_START_READ_NOTIF, OS_NOTIFY_SET_BITS);
}

static void handle_evt_gattc_read_completed(ble_evt_gattc_read_completed_t *evt)
{
        (void)evt;
}

static void handle_evt_gattc_write_completed(ble_evt_gattc_write_completed_t *evt)
{
        if (evt->handle == dsps->sps_rx_val_h) {
                tx_done_cb(dsps, evt -> conn_idx);
        }
}

static void handle_evt_gattc_notification(ble_evt_gattc_notification_t *evt)
{
        if (dsps->sps_tx_val_h == evt -> handle) {
                /* Call RX data callback if SPS flow is on */
                if (dsps_flow_ctrl == DSPS_FLOW_CONTROL_ON) {
                        if (evt->length){
                                rx_data_cb(dsps, conn_idx, evt->value, evt->length);
                         }
                }
        }
        if (dsps->sps_flow_ctrl_val_h == evt->handle)
        {
                /* Save the latest SPS flow status */
                dsps_flow_ctrl = evt->value[0];
                switch(dsps_flow_ctrl) {
                        case DSPS_FLOW_CONTROL_ON:
                                DBG_LOG("SPS flow control is ON\r\n");
                                OS_TASK_NOTIFY(ble_central_task_handle, SPS_BLE_TX_NOTIF, OS_NOTIFY_SET_BITS); // Kickoff BLE TX when SPS flow is on
                                break;
                        case DSPS_FLOW_CONTROL_OFF:
                                DBG_LOG("SPS flow control is OFF\r\n");
                                break;
                        default:
                                DBG_LOG("Unknown flow control notification\r\n");
                                break;
                }
        }
}

static void scan_start(void)
{
        APP_BLE_GAP_CALL_FUNC_UNTIL_NO_ERR(ble_gap_scan_start,
                GAP_SCAN_ACTIVE, GAP_SCAN_GEN_DISC_MODE, BLE_SCAN_INTERVAL, BLE_SCAN_WINDOW, false, false);
}

/* Connection timer callback */
static void conn_timeout_cb(OS_TIMER xTimer)
{
        OS_TASK task = (OS_TASK) OS_TIMER_GET_TIMER_ID(xTimer);

        OS_TASK_NOTIFY(task, BLE_CONN_TIMEOUT_NOTIF, OS_NOTIFY_SET_BITS);
}

void dsps_BLE_task(void *params)
{
        int8_t wdog_id;

        ble_central_task_handle = OS_GET_CURRENT_TASK();

        /* register ble_central task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        /**
         * Set device name
         */
        ble_gap_device_name_set("Dialog SPS Demo Host", ATT_PERM_READ);

        ble_central_start();
        ble_register_app();
        ble_gap_mtu_size_set(MTU_SIZE);

        update_device_address();
        print_device_address();

        /* initialize SPS handle struct */
        dsps = OS_MALLOC(sizeof(*dsps));
        memset(dsps, 0, sizeof(*dsps));

        scan_start();

        conn_timeout_h = OS_TIMER_CREATE("CONN_TIMEOUT", OS_MS_2_TICKS(CONN_TIMEOUT_MS), OS_TIMER_FAIL,
                                                                        OS_GET_CURRENT_TASK(), conn_timeout_cb);
        ASSERT_WARNING(conn_timeout_h);

        DBG_LOG("DSPS Central Task Ready.\r\n");

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
                        case BLE_EVT_GATTC_MTU_CHANGED:
                                handle_evt_gap_mtu_exchanged((ble_evt_gattc_mtu_changed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_ADV_REPORT:
                                handle_evt_gap_adv_report((ble_evt_gap_adv_report_t *) hdr);
                                break;
                        case BLE_EVT_GAP_SCAN_COMPLETED:
                                handle_evt_gap_scan_completed((ble_evt_gap_scan_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_CONNECTION_COMPLETED:
                                handle_evt_gap_connection_completed((ble_evt_gap_connection_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_CONN_PARAM_UPDATE_REQ:
                                handle_evt_gap_conn_param_updated_req((ble_evt_gap_conn_param_update_req_t *) hdr);
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
                        case BLE_EVT_GATTC_BROWSE_SVC:
                                handle_evt_gattc_browse_svc((ble_evt_gattc_browse_svc_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_BROWSE_COMPLETED:
                                handle_evt_gattc_browse_completed((ble_evt_gattc_browse_completed_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_READ_COMPLETED:
                                handle_evt_gattc_read_completed((ble_evt_gattc_read_completed_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_WRITE_COMPLETED:
                                handle_evt_gattc_write_completed((ble_evt_gattc_write_completed_t *) hdr);
                                break;
                        case BLE_EVT_GATTC_NOTIFICATION:
                                handle_evt_gattc_notification((ble_evt_gattc_notification_t *) hdr);
                                break;
#if (dg_configBLE_2MBIT_PHY == 1)
                        case BLE_EVT_GAP_PHY_SET_COMPLETED:
                                handle_ble_evt_gap_phy_set_completed((ble_evt_gap_phy_set_completed_t *) hdr);
                                break;
                        case BLE_EVT_GAP_PHY_CHANGED:
                                handle_ble_evt_gap_phy_changed((ble_evt_gap_phy_changed_t *) hdr);
                                break;
#endif /* (dg_configBLE_2MBIT_PHY == 1) */
                        default:
                                ble_handle_event_default(hdr);
                                break;
                        }

                        OS_FREE(hdr);

no_event:
                        /* Notify again if there are more events to process in queue */
                        if (ble_has_event()) {
                                OS_TASK_NOTIFY(ble_central_task_handle, BLE_APP_NOTIFY_MASK, OS_NOTIFY_SET_BITS);
                        }
                }

                if (notif & SPS_BLE_TX_NOTIF) {
                        tx_data_available();
                }

                if (notif & BLE_DISCOVER_NOTIF) {
                        att_uuid_t sps_uuid;

                        ble_uuid_from_string(UUID_DSPS, &sps_uuid);
                        ble_gattc_browse(conn_idx, &sps_uuid);
                }

                if (notif & BLE_SCAN_START_NOTIF) {
                        scan_start();
                }

                if (notif & BLE_CONN_TIMEOUT_NOTIF) {
                        ble_error_t status __UNUSED;
                        status = ble_gap_connect_cancel();

                        ASSERT_WARNING(status == BLE_STATUS_OK);
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
                        bool send_flow_off = false;

                        sps_queue_write_items(tx_queue, ReadSize, dsps_data);

                        /* Check if queue is almost full and issue to send a SPS flow off, if so */
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
                         OS_TASK_NOTIFY(ble_central_task_handle, SPS_BLE_TX_NOTIF, OS_NOTIFY_SET_BITS);

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
