/**
 ****************************************************************************************
 *
 * @file ble_profile_task.c
 *
 * @brief WeChat profile app implementation
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
#include "sdk_list.h"
#include "wechats.h"
#include "sys_power_mgr.h"
#include "ad_nvparam.h"
#include "hw_gpio.h"
#include "sys_watchdog.h"
#include "platform_nvparam.h"
#include "ble_wechat_config.h"
#include "mpbledemo2.h"
#include "ble_wechat_util.h"

static const uint8_t default_adv_data[] = {

        0x03, GAP_DATA_TYPE_UUID16_LIST,
        0xE7, 0xFE, // = 0xFEE7 (WeChat UUID)
};

static uint16_t connection_index = 0xFFFF;

/*
 * The maximum length of name in scan response
 */
#define MAX_NAME_LEN    (BLE_SCAN_RSP_LEN_MAX - 2)

#define INITIAL_ADVERTISING_MIN         BLE_ADV_INTERVAL_FROM_MS(20)
#define INITIAL_ADVERTISING_MAX         BLE_ADV_INTERVAL_FROM_MS(30)

/*****************************************************************************
* data handle
*****************************************************************************/
static data_info g_send_data   = {NULL, 0, 0};
static data_info g_rcv_data    = {NULL, 0, 0};

data_handler* pDataHandler;

static void ble_wechat_process_received_data(uint8_t* pData, uint32_t length);
extern void ad_ble_get_public_address(uint8_t address[6]);

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
                strcpy(name_buf, BLE_WECHAT_DEFAULT_NAME);
                return strlen(BLE_WECHAT_DEFAULT_NAME);
        }

        name_buf[read_len] = '\0';

        return read_len;
}

/* put the BD address into the vendor specific data */
static uint32_t config_adv_data(uint8_t *adv_data_buf)
{
        adv_data_buf[0] = 0x09;
        adv_data_buf[1] = GAP_DATA_TYPE_MANUFACTURER_SPEC;
        adv_data_buf[2] = 0x60;
        adv_data_buf[3] = 0x00;

        uint8_t bd_address[6];

        ad_ble_get_public_address(bd_address);

        for (uint32_t index = 0;index < sizeof(bd_address);index++)
        {
                adv_data_buf[4 + index] = bd_address[5 - index];
        }

        return 10;
}

int32_t ble_wechat_indicate_data_chunk(void)
{
	uint16_t chunk_len = 0;

	chunk_len = g_send_data.len - g_send_data.offset;
	chunk_len = chunk_len > WECHATS_MAX_DATA_LEN?WECHATS_MAX_DATA_LEN:chunk_len;

	if (chunk_len == 0)
	{
		pDataHandler->m_data_free_func(g_send_data.data, g_send_data.len);
		g_send_data.data = NULL;
		g_send_data.len = 0;
		g_send_data.offset = 0;
		return 0;
	}

	wechats_send_indication(connection_index, g_send_data.data + g_send_data.offset, chunk_len);
	g_send_data.offset += chunk_len;
	return 1;
}

//device sent data on the indicate characteristic
int32_t ble_wechat_indicate_data(uint8_t *data, int32_t len)
{
	if (data == NULL || len == 0)
	{
		return 0;
	}

	if (g_send_data.len != 0 && g_send_data.offset != g_send_data.len)
	{
		printf("\r\n offset:%d %d",g_send_data.offset,g_send_data.len);
		return 0;
	}

	g_send_data.data = data;
	g_send_data.len = len;
	g_send_data.offset = 0;
	return (ble_wechat_indicate_data_chunk());
}

void app_sample128_SetDatahandler(data_handler* pHandler)
{
        pDataHandler = pHandler;
}

static void ble_wechat_process_received_data(uint8_t* pData, uint32_t length)
{
	int32_t error_code;
	int32_t chunk_size = 0;
	if (length <= WECHATS_MAX_DATA_LEN)
	{
		if (g_rcv_data.len == 0)
		{
		        BpFixHead *fix_head = (BpFixHead *)pData;
			g_rcv_data.len = ntohs(fix_head->nLength);
			g_rcv_data.offset = 0;
			g_rcv_data.data = (uint8_t *)OS_MALLOC(g_rcv_data.len);
		}

		chunk_size = g_rcv_data.len - g_rcv_data.offset;
		chunk_size = chunk_size < length ? chunk_size : length;
		memcpy(g_rcv_data.data + g_rcv_data.offset, pData, chunk_size);
		g_rcv_data.offset += chunk_size;

		if (g_rcv_data.len <= g_rcv_data.offset)
		{
			error_code = pDataHandler->m_data_consume_func(g_rcv_data.data, g_rcv_data.len);
			pDataHandler->m_data_free_func(g_rcv_data.data,g_rcv_data.len);
			wechat_error_chack(pDataHandler, error_code);
			g_rcv_data.len = 0;
			g_rcv_data.offset = 0;

			pDataHandler->m_data_main_process_func();
		}
	}
}

//function for register all products
static void register_all_products(void)
{
	REGISTER(mpbledemo2);
	// could register more products if defined
}

void mpbledemo2_peripheral_init(void)
{
    if (0 != m_mpbledemo2_handler)
    {
        m_mpbledemo2_handler->m_data_init_peripheral_func();
    }
}

static void initialize_mpbledemo2(void)
{
	register_all_products();

	data_handler_init(&m_mpbledemo2_handler, PRODUCT_TYPE_MPBLEDEMO2);
	m_mpbledemo2_handler->m_data_init_func();
	app_sample128_SetDatahandler(m_mpbledemo2_handler);

	mpbledemo2_peripheral_init();
}

void wechats_cb(uint16_t conn_idx, WECHATS_CB_REASON_E reason, uint8_t* pData, uint32_t dataLen)
{
        switch (reason)
        {
                case WECHATS_CB_WRITE_VAL:
                {
                        ble_wechat_process_received_data(pData, dataLen);
                        break;
                }
                case WECHATS_CB_WRITE_IND_CFG:
                {
                        uint16_t isEnable = *(uint16_t *)pData;
                        mpbledemo2_indication_state(isEnable);
                        pDataHandler->m_data_main_process_func();
                        break;
                }
                case WECHATS_CB_DISCONNECT:
                {
                        mpbledemo2_reset();
                        break;
                }
                case WECHATS_CB_EVENT_SENT:
                {
                        ble_wechat_indicate_data_chunk();
                        break;
                }
                default:
                        break;
        }
}

static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        connection_index = evt->conn_idx;
}

static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{
        connection_index = 0xFFFF;
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

static void handle_evt_gap_adv_completed(ble_evt_gap_adv_completed_t *evt)
{
        /*
         * If advertising is completed, just restart it. It's either because new client connected
         * or it was cancelled in order to change interval values.
         */
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);
}

void ble_profile_task(void *params)
{
        ble_service_t *svc;
        int8_t wdog_id;

        uint16_t name_len;
        char name_buf[MAX_NAME_LEN + 1];        /* 1 byte for '\0' character */
        uint8_t scan_rsp[BLE_SCAN_RSP_LEN_MAX] = {
                0x00, GAP_DATA_TYPE_LOCAL_NAME, /* Parameter length must be set after */
                /* Name will be put here */
        };

        /* register pxp task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        ble_peripheral_start();
        ble_register_app();

        /* Get name from nvparam if exist or default otherwise */
        name_len = read_name(MAX_NAME_LEN, name_buf);

        uint8_t adv_data[BLE_ADV_DATA_LEN_MAX];
        uint32_t adv_data_len = 0;
        memcpy(adv_data, default_adv_data, sizeof(default_adv_data));
        adv_data_len += sizeof(default_adv_data);
        adv_data_len += config_adv_data(&adv_data[sizeof(default_adv_data)]);

        /*
         * Set device name
         */
        ble_gap_device_name_set(name_buf, ATT_PERM_READ);

        /*
         * Set scan response data
         */
        scan_rsp[0] = name_len + 1; /* 1 byte for data type */
        memcpy(&scan_rsp[2], name_buf, name_len);

        svc = wechats_init(wechats_cb);
        ble_service_add(svc);

        initialize_mpbledemo2();

        ble_gap_adv_data_set(adv_data_len, adv_data, name_len + 2, scan_rsp);
        ble_gap_adv_intv_set(INITIAL_ADVERTISING_MIN, INITIAL_ADVERTISING_MAX);
        ble_gap_adv_start(GAP_CONN_MODE_UNDIRECTED);

        printf("task init\r\n");


        for (;;) {
                OS_BASE_TYPE ret __attribute__((unused));
                uint32_t notif;

                /* notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for the task notification. Therefore, the return value must
                 * always be OS_OK
                 */
                OS_ASSERT(ret == OS_OK);

                /* resume watchdog */
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
                                case BLE_EVT_GAP_CONNECTED:
                                        handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_DISCONNECTED:
                                        handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_ADV_COMPLETED:
                                        handle_evt_gap_adv_completed((ble_evt_gap_adv_completed_t *) hdr);
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


        }
}

