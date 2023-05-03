/**
 ****************************************************************************************
 *
 * @file dsps_usbd.h
 *
 * @brief USB CDC device operations
 *
 * Copyright (C) 2023. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef DSPS_USBD_H_
#define DSPS_USBD_H_

#if defined(DSPS_USBD)

#include "USB_CDC.h"

#define USBD_CDC_RX_BUF_SIZE     1024

/**
 * Application-defined routine to get the USB CDC instance created upon USB attach in the USB system service.
 *
 * \return The USB CDC instance created; should be used for further USB CDC device operations.
 */
USB_CDC_HANDLE cdc_usbd_get_handle(void);

/**
 * Application-defined routine to read over the USB interface
 *
 * \param[in] _dev       Handle of a valid CDC instance. Should be retrieved via \sa cdc_usbd_get_handle()
 * \param[in] _data      Pointer to a buffer where the received data will be stored
 * \param[in] _len       Number of bytes to read
 * \param[in] _timeout   Timeout expressed in millisecond
 *
 * \return Number of bytes that have been read the given timeout
 *
 */
int read_from_cdc_usbd(USB_CDC_HANDLE handle, void *buf, unsigned len, int timeout);

/**
 * Application-defined routine to write over the USB interface
 *
 * \param[in] _dev       Handle of a valid CDC instance. Should be retrieved via \sa cdc_usbd_get_handle()
 * \param[in] _data      Pointer to data that should be sent to the host
 * \param[in] _len       Number of bytes to be written
 * \param[in] _timeout   Timeout expressed in millisecond
 *
 * \return Number of bytes that have been written before a timeout occurs
 *
 */
int write_to_cdc_usbd(USB_CDC_HANDLE handle, const void *buf, unsigned len, int timeout);

#endif
#endif /* DSPS_USBD_H_ */
