/**
 ****************************************************************************************
 *
 * @file wechats.c
 *
 * @brief Wechat service implementation
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
#include <stddef.h>
#include <string.h>
#include "osal.h"
#include "ble_att.h"
#include "ble_attribdb.h"
#include "ble_bufops.h"
#include "ble_common.h"
#include "ble_gatts.h"
#include "ble_uuid.h"
#include "wechats.h"

#define UUID_WECHAT_SERVICE		(0xFEE7) 	// Wechat Service
#define UUID_WECHAT_WRITE_CH            (0xFEC7)	// Wechat write characteristic
#define UUID_WECHAT_IND_CH       	(0xFEC8)	// Wechat indication characteristic
#define UUID_WECHAT_READ_CH      	(0xFEC9)	// Wechat read characteristic


typedef struct {
        ble_service_t svc;

        // handles
        uint16_t write_val_h;
        uint16_t read_val_h;
        uint16_t ind_val_h;
        uint16_t ind_cfg_h;

        // callbacks
        wechats_cb_t cb;
} wechat_service_t;

static uint16_t ind_val_handle;

static void handle_disconnection_evt(ble_service_t *svc, const ble_evt_gap_disconnected_t *evt);
static void hanlde_event_sent(ble_service_t *svc, const ble_evt_gatts_event_sent_t *evt);
static void handle_write_req(ble_service_t *svc, const ble_evt_gatts_write_req_t *evt);

static void handle_disconnection_evt(ble_service_t *svc, const ble_evt_gap_disconnected_t *evt)
{
	wechat_service_t *pWechats = (wechat_service_t *) svc;
	if (pWechats->cb)
	{
		pWechats->cb(evt->conn_idx, WECHATS_CB_DISCONNECT, NULL, 0);
	}
}

static void hanlde_event_sent(ble_service_t *svc, const ble_evt_gatts_event_sent_t *evt)
{
	wechat_service_t *pWechats = (wechat_service_t *) svc;
	if (pWechats->cb)
	{
		pWechats->cb(evt->conn_idx, WECHATS_CB_EVENT_SENT, NULL, 0);
	}
}

static void handle_write_req(ble_service_t *svc, const ble_evt_gatts_write_req_t *evt)
{
		wechat_service_t *pWechats = (wechat_service_t *) svc;
        att_error_t err = ATT_ERROR_OK;

        // set the value of attribute
        ble_gatts_set_value(evt->handle, evt->length, evt->value);

        // check the reason code
        WECHATS_CB_REASON_E reason = WECHATS_CB_REASON_CNT;
        if (pWechats->write_val_h == evt->handle)
        {
        	reason = WECHATS_CB_WRITE_VAL;
        }
        else if (pWechats->ind_cfg_h == evt->handle)
        {
        	reason = WECHATS_CB_WRITE_IND_CFG;
        }

        if (pWechats->cb)
        {
        	pWechats->cb(evt->conn_idx, reason, (uint8_t *)(evt->value), evt->length);
        }

        ble_gatts_write_cfm(evt->conn_idx, evt->handle, err);
}

uint8_t wechats_send_indication(uint16_t conn_idx, uint8_t* pData, uint16_t length)
{
	return ble_gatts_send_event(conn_idx, ind_val_handle, GATT_EVENT_INDICATION, length, pData);
}

/// Characteristic Value Descriptor
struct att_char_desc
{
    /// properties
    uint8_t prop;
    /// attribute handle
    uint8_t attr_hdl[2];
    /// attribute type
    uint8_t attr_type[2];
};

ble_service_t *wechats_init(wechats_cb_t wechat_cb)
{
	wechat_service_t* pWechats;
        uint16_t num_attr;
        att_uuid_t uuid;

        pWechats = OS_MALLOC(sizeof(*pWechats));
        memset(pWechats, 0, sizeof(*pWechats));

        pWechats->svc.write_req = handle_write_req;
        pWechats->svc.event_sent = hanlde_event_sent;
        pWechats->svc.disconnected_evt = handle_disconnection_evt;

        num_attr = ble_gatts_get_num_attr(0, 3, 1);

        ble_uuid_create16(UUID_WECHAT_SERVICE, &uuid);
        ble_gatts_add_service(&uuid, GATT_SERVICE_PRIMARY, num_attr);

        ble_uuid_create16(UUID_WECHAT_WRITE_CH, &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_WRITE, ATT_PERM_WRITE,
        		WECHATS_MAX_DATA_LEN, 0, NULL, &pWechats->write_val_h);

        ble_uuid_create16(UUID_WECHAT_READ_CH, &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_READ, ATT_PERM_READ,
        		WECHATS_MAX_DATA_LEN, 0, NULL, &pWechats->read_val_h);

        ble_uuid_create16(UUID_WECHAT_IND_CH, &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_INDICATE, ATT_PERM_READ,
        		WECHATS_MAX_DATA_LEN, 0, NULL, &pWechats->ind_val_h);

        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ | ATT_PERM_WRITE,
        		sizeof(uint16_t), 0, &pWechats->ind_cfg_h);

        ble_gatts_register_service(&pWechats->svc.start_h,
        		&pWechats->write_val_h, &pWechats->read_val_h, &pWechats->ind_val_h,
				&pWechats->ind_cfg_h, 0);

        ind_val_handle = pWechats->ind_val_h;

        pWechats->svc.end_h = pWechats->svc.start_h + num_attr;
        pWechats->cb = wechat_cb;

        return &pWechats->svc;
}
