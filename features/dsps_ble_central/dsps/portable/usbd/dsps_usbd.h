/**
 ****************************************************************************************
 *
 * @file dsps_usbd.h
 *
 * @brief USB CDC device operations
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
