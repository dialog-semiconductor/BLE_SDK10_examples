/**
 ****************************************************************************************
 *
 * @file dsps_port_usbd.h
 *
 * @brief DSPS port for USB CDC devices
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

#ifndef DSPS_PORT_USBD_H_
#define DSPS_PORT_USBD_H_

/**
 * Application-defined routine for the USB CDC initialization
 *
 * \note All the necessary USB initialization actions are performed upon USB attach in the USB system service.
 *
 */
#define _SERIAL_PORT_OPEN(_dev)

/**
 * Application-defined routine for the USB CDC de-initialization
 *
 * \note All the necessary USB initialization actions are performed upon USB attach in the USB system service.
 *
 */
#define _SERIAL_PORT_CLOSE(_dev)

/**
 * Application-defined routine for the USB CDC read operations
 *
 * \param[in] _dev       Handle of a valid CDC instance. Should be retrieved via \sa cdc_usbd_get_handle()
 * \param[in] _data      Pointer to a buffer where the received data will be stored
 * \param[in] _len       Number of bytes to read
 * \param[in] _timeout   Timeout expressed in millisecond
 *
 * \return Number of bytes that have been read the given timeout
 *
 */
#define _SERIAL_PORT_READ_DATA(_dev, _data, _len, _timeout)   read_from_cdc_usbd(_dev, _data, _len, _timeout)

/**
 * Application-defined routine for the USB CDC read operations
 *
 * \param[in] _dev       Handle of a valid CDC instance. Should be retrieved via \sa cdc_usbd_get_handle()
 * \param[in] _data      Pointer to data that should be sent to the host
 * \param[in] _len       Number of bytes to be written
 * \param[in] _timeout   Timeout expressed in millisecond
 *
 * \return Number of bytes that have been written before a timeout occurs
 *
 */
#define _SERIAL_PORT_WRITE_DATA(_dev, _data, _len, _timeout)  write_to_cdc_usbd(_dev, _data, _len, _timeout)

/**
 * Application-defined routine for the USB flow control activation
 *
 * \note No USB flow control is supported on application level. All the basic flow control mechanisms
 *       are provided by the USB controller intrinsically and as defined by the USB protocol.
 */
#define _SERIAL_PORT_SET_FLOW_ON(_dev)

/**
 * Application-defined routine for the USB flow control de-activation
 *
 * \note No USB flow control is supported on application level. All the basic flow control mechanisms
 *       are provided by the USB controller intrinsically and as defined by the USB protocol.
 */
#define _SERIAL_PORT_SET_FLOW_OFF(_dev)

#endif /* DSPS_PORT_USBD_H_ */
