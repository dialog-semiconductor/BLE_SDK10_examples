/**
 ****************************************************************************************
 *
 * @file dsps_port.h
 *
 * @brief DSPS port
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
#ifndef DSPS_PORT_H_
#define DSPS_PORT_H_

#if defined(DSPS_UART)
   #include "dsps_port_uart.h"
#elif defined(DSPS_USBD)
   #include "dsps_port_usbd.h"
#endif

#ifndef _SERIAL_PORT_READ_DATA
   #define _SERIAL_PORT_READ_DATA(_dev, _data, _len, _timeout)
#endif

#ifndef _SERIAL_PORT_WRITE_DATA
   #define _SERIAL_PORT_WRITE_DATA(_dev, _data_len, _timeout)
#endif

#ifndef _SERIAL_PORT_SET_FLOW_ON
   #define _SERIAL_PORT_SET_FLOW_ON(_dev)
#endif

#ifndef _SERIAL_PORT_SET_FLOW_OFF
   #define _SERIAL_PORT_SET_FLOW_OFF(_dev)
#endif

#ifndef _SERIAL_PORT_OPEN
   #define _SERIAL_PORT_OPEN(_dev)
#endif

#ifndef _SERIAL_PORT_CLOSE
   #define _SERIAL_PORT_CLOSE(_dev)
#endif

/**
 * Application-defined routine to read data over the serial interface
 *
 * \param[in] _dev       Handle of a valid serial device instance (typically acquired via \sa SERIAL_PORT_OPEN())
 * \param[in] _data      Pointer to a buffer where the received data will be stored
 * \param[in] _len       Number of bytes to read
 * \param[in] _timeout   Timeout after which the initiated read operation should be terminated
 *
 * \return The number of bytes received within the given timeout
 *
 */
#define SERIAL_PORT_READ_DATA(_dev, _data, _len, _timeout)   _SERIAL_PORT_READ_DATA(_dev, _data, _len, _timeout)

/**
 * Application-defined routine to write data over the serial interface
 *
 * \param[in] _dev       Handle of a valid serial device instance (typically acquired via \sa SERIAL_PORT_OPEN())
 * \param[in] _data      Pointer to data that should be sent over the serial interface
 * \param[in] _len       Number of bytes to be transmitted
 * \param[in] _timeout   Timeout after which the initiated write operation should be terminated
 *
 * \return The actual number of bytes transmitted before the timeout expiration.
 *
 */
#define SERIAL_PORT_WRITE_DATA(_dev, _data, _len, _timeout)  _SERIAL_PORT_WRITE_DATA(_dev, _data, _len, _timeout)

/**
 * Application-defined routine for the flow control activation (if supported by the serial interface)
 *
 * \param[in] _dev  Handle of a valid serial device instance (typically acquired via \sa SERIAL_PORT_OPEN())
 *
 */
#define SERIAL_PORT_SET_FLOW_ON(_dev)   _SERIAL_PORT_SET_FLOW_ON(_dev)

/**
 * Application-defined routine for the flow control de-activation (if supported by the serial interface)
 *
 * \param[in] _dev  Handle of a valid serial device instance (typically acquired via \sa SERIAL_PORT_OPEN())
 *
 */
#define SERIAL_PORT_SET_FLOW_OFF(_dev)  _SERIAL_PORT_SET_FLOW_OFF(_dev)

/**
 * Application-defined macro to initialize a serial interface.
 *
 * \param[in] _dev  Typically this is the device structure describing how the device instance should be initialized.
 *
 * \return The handle of the initialized/opened device instance
 *
 */
#define SERIAL_PORT_OPEN(_dev)   _SERIAL_PORT_OPEN(_dev)

/**
 * Application-defined macro to de-initialize a serial interface.
 *
 * \param[in] _dev  Handle of a valid serial device instance (typically acquired via \sa SERIAL_PORT_OPEN())
 *
 * \return Typically this should be an error code returned
 *
 */
#define SERIAL_PORT_CLOSE(_dev)  _SERIAL_PORT_CLOSE(_dev)

#endif /* DSPS_PORT_H_ */
