/**
 ****************************************************************************************
 *
 * @file dlg_suouart.h
 *
 * @brief SUOUART service implementation API
 *
 * Copyright (C) 2016-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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
