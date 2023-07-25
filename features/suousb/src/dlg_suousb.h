/**
 ****************************************************************************************
 *
 * @file dlg_suousb.h
 *
 * @brief Dialog SUoUSB service implementation API
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

#ifndef DLG_SUOUSB_H_
#define DLG_SUOUSB_H_
#if (dg_configSUOUSB_SUPPORT == 1)
#include <stdint.h>

typedef enum {
        SUOUSB_ERROR_OK,
        SUOUSB_ERROR_READ_NOT_PERMITTED,
        SUOUSB_ERROR_REQUEST_NOT_SUPPORTED,
        SUOUSB_ERROR_ATTRIBUTE_NOT_LONG,
        SUOUSB_ERROR_ATTRIBUTE_NOT_FOUND,
        SUOUSB_ERROR_APPLICATION_ERROR,
} suousb_error_t;

/**
 * SUOTA active image enum
 */
typedef enum {
        SUOUSB_ACTIVE_IMG_FIRST,
        SUOUSB_ACTIVE_IMG_SECOND,
        SUOUSB_ACTIVE_IMG_ERROR,
} suousb_active_img_t;

typedef bool (* suousb_ready_cb_t ) (void);
typedef void (* suousb_notify_cb_t) (const char *status);

typedef enum {
        SUOUSB_READ_STATUS,
        SUOUSB_READ_MEMINFO,
        SUOUSB_WRITE_STATUS,
        SUOUSB_WRITE_MEMDEV,
        SUOUSB_WRITE_GPIO_MAP,
        SUOUSB_WRITE_PATCH_LEN,
        SUOUSB_WRITE_PATCH_DATA
} suousb_write_request_t;

/**
 * Register SUOUSB Notify callback, and set size of buffer to use
 */
suousb_error_t suousb_write_req(suousb_write_request_t req, uint16_t offset, uint16_t length, const uint8_t *value);
suousb_error_t suousb_read_req(suousb_write_request_t req, uint32_t *value);

/**
 * Initialization of SUOUSB Service instance
 *
 * \return error
 */
int suousb_init(suousb_notify_cb_t cb);

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
uint32_t suousb_update_crc(uint32_t crc, const uint8_t *data, size_t len);
#endif
#endif /* DLG_SUOUSB_H_ */
