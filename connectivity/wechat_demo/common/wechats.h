/**
 ****************************************************************************************
 *
 * @file wechats.h
 *
 * @brief Wechat Service implementation API
 *
 * Copyright (c) 2018 Dialog Semiconductor. All rights reserved.
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

#ifndef WECHATS_H_
#define WECHATS_H_

#include <stdbool.h>
#include <stdint.h>
#include <ble_service.h>

#define WECHATS_MAX_DATA_LEN	(23 - 3)

/// Callback reason code
typedef enum
{
	WECHATS_CB_WRITE_VAL = 0,
	WECHATS_CB_WRITE_IND_CFG,
	WECHATS_CB_DISCONNECT,
	WECHATS_CB_EVENT_SENT,
	WECHATS_CB_REASON_CNT,
} WECHATS_CB_REASON_E;


typedef void (* wechats_cb_t) (uint16_t conn_idx, WECHATS_CB_REASON_E reason, uint8_t* pData, uint32_t dataLength);

ble_service_t *wechats_init(wechats_cb_t wechats_cb);

uint8_t wechats_send_indication(uint16_t conn_idx, uint8_t* pData, uint16_t length);

#endif /* WECHATS_H_ */
