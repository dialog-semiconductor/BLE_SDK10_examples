/**
 ****************************************************************************************
 *
 * @file ble_wechat_util.h
 *
 * @brief Wechat application utilities
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

#ifndef BLE_WECHAT_UTIL
#define BLE_WECHAT_UTIL

#include <stdint.h>
#include <string.h>

#define PRODUCT_TYPE_UNKNOWN 0
#define PRODUCT_TYPE_MPBLEDEMO2 1

typedef void (*data_produce_func)(void *args, uint8_t **r_data, uint32_t *r_len);
typedef void (*data_free_func)(uint8_t *data, uint32_t len);
typedef int32_t (*data_consume_func)(uint8_t *data, uint32_t len);
typedef void (*data_error_func)(int32_t error_code);

typedef int32_t (*data_button_handler_func)(uint8_t pin_no);
typedef void (*data_main_process_func)(void);
typedef int32_t (*data_init_func)(void);
typedef int32_t (*data_init_peripheral_func)(void);

typedef struct data_handler{
		int m_product_type;
		data_produce_func 				m_data_produce_func;
		data_free_func 						m_data_free_func;
		data_consume_func 				m_data_consume_func;
		data_error_func 					m_data_error_func;
        data_init_peripheral_func          m_data_init_peripheral_func;
		data_init_func 						m_data_init_func;
		data_main_process_func 		m_data_main_process_func;
		data_button_handler_func 	m_data_button_handler_func;
		void *m_data_produce_args;
		struct data_handler *next;
} data_handler;

extern data_handler first_handler;

#define REGISTER(NAME)	\
		do {								\
				data_handler *tmp = &NAME##_data_handler;	\
				tmp->next = first_handler.next; \
				first_handler.next = tmp;	\
		} while(0)

#define ARGS_ITEM_SET(ARGS_TYPE, ARGS_POINTER, ITEM_NAME, ITEM_VALUE)	\
		do {	\
				ARGS_TYPE *tmp = (ARGS_TYPE *)ARGS_POINTER;	\
				tmp->ITEM_NAME = ITEM_VALUE;	\
		} while(0)


typedef struct
{
		unsigned char bMagicNumber;
		unsigned char bVer;
		unsigned short nLength;
		unsigned short nCmdId;
		unsigned short nSeq;
} BpFixHead;

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef struct
{
		uint8_t *data;
		uint16_t len;
		uint16_t offset;
} data_info;

data_handler* get_handler_by_type(int type);
unsigned short htons(unsigned short val);
unsigned short ntohs(unsigned short val);
unsigned long t_htonl(unsigned long h);
unsigned long t_ntohl(unsigned long n);

void wechat_error_chack(data_handler *p_data_handler, int error_code);
void data_handler_init(data_handler** p_data_handler, uint8_t product_type);

#endif
