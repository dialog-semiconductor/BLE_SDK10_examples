/**
 ****************************************************************************************
 *
 * @file dlg_suouart.h
 *
 * @brief SUOUART service implementation API
 *
 * Copyright (C) 2016-2022 Renesas Electronics Corporation and/or its affiliates
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

#ifndef DLG_SUOUART_H_
#define DLG_SUOUART_H_

#if (dg_configSUOUART_SUPPORT == 1)
#include <stdint.h>

typedef enum {
        SUOUART_ERROR_OK,
        SUOUART_ERROR_READ_NOT_PERMITTED,
        SUOUART_ERROR_REQUEST_NOT_SUPPORTED,
        SUOUART_ERROR_ATTRIBUTE_NOT_LONG,
        SUOUART_ERROR_ATTRIBUTE_NOT_FOUND,
        SUOUART_ERROR_APPLICATION_ERROR,
} suouart_error_t;

/**
 * SUOTA active image enum
 */
typedef enum {
        SUOUART_ACTIVE_IMG_FIRST,
        SUOUART_ACTIVE_IMG_SECOND,
        SUOUART_ACTIVE_IMG_ERROR,
} suouart_active_img_t;

typedef bool (* suouart_ready_cb_t ) (void);
typedef void (* suouart_notify_cb_t) (const char *status);

typedef enum {
        SUOUART_READ_STATUS,
        SUOUART_READ_MEMINFO,
        SUOUART_WRITE_STATUS,
        SUOUART_WRITE_MEMDEV,
        SUOUART_WRITE_GPIO_MAP,
        SUOUART_WRITE_PATCH_LEN,
        SUOUART_WRITE_PATCH_DATA
} suouart_write_request_t;

/**
 * Register SUOUART Notify callback, and set size of buffer to use
 */
suouart_error_t suouart_write_req(suouart_write_request_t req, uint16_t offset, uint16_t length, const uint8_t *value);
suouart_error_t suouart_read_req(suouart_write_request_t req, uint32_t *value);

/**
 * Initialization of SUOUART Service instance
 *
 * \return error
 */
int suouart_init(suouart_notify_cb_t cb);

/**
 * Update CRC
 *
 * param [in] crc current value of CRC
 * param [in] data pointer to data to compute CRC over
 * param [in] len number of bytes pointed by data
 *
 * \return new value of CRC
 *
 */
uint32_t suouart_update_crc(uint32_t crc, const uint8_t *data, size_t len);
#endif
#endif /* DLG_SUOUART_H_ */
