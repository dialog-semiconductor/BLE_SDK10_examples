/**
 ****************************************************************************************
 *
 * @file dsps_port_uart.h
 *
 * @brief DSPS port UART
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

#ifndef DSPS_PORT_UART_H_
#define DSPS_PORT_UART_H_

#include "dsps_uart.h"

/**
 * Application-defined macro to initialize a serial interface.
 *
 * \param[in] _dev  Typically this is the device structure describing how the device instance should be initialized.
 *
 * \return The handle of the initialized/opened device instance
 *
 */
#define _SERIAL_PORT_OPEN(_dev)  uart_open(_dev)

/**
 * Application-defined macro to de-initialize a serial interface.
 *
 * \param[in] _dev  Handle of a valid serial device instance (typically acquired via \sa SERIAL_PORT_OPEN())
 *
 * \return Typically this should be an error code returned
 *
 */
#define _SERIAL_PORT_CLOSE(_dev) uart_close(_dev)

/**
 * Application-defined routine to read over the UART interface (blocking routine)
 *
 * \param[in] _dev       Handle of a valid UART instance. Should be retrieved via \sa SERIAL_PORT_OPEN()
 * \param[in] _data      Pointer to a buffer where the received data will be stored
 * \param[in] _len       Number of bytes to read
 * \param[in] _timeout   Timeout expressed in millisecond
 *
 * \return Number of bytes that have been read the given timeout
 *
 */
#define _SERIAL_PORT_READ_DATA(_dev, _data, _len, _timeout)    read_from_uart(_dev, _data, _len, _timeout)

/**
 * Application-defined routine to write over the UART interface (blocking routine)
 *
 * \param[in] _dev       Handle of a valid UART instance. Should be retrieved via \sa SERIAL_PORT_OPEN()
 * \param[in] _data      Pointer to data that should be sent
 * \param[in] _len       Number of bytes to be written
 * \param[in] _timeout   Timeout expressed in millisecond (not used)
 *
 * \return The error code returned upon completion
 *
 */
#define _SERIAL_PORT_WRITE_DATA(_dev, _data, _len, _timeout)   write_to_uart(_dev, _data, _len)

#if defined(CFG_UART_HW_FLOW_CTRL)
   #define _SERIAL_PORT_SET_FLOW_ON(_dev)   uart_hw_sps_flow_on(_dev)
#elif defined(CFG_UART_SW_FLOW_CTRL)
   #define _SERIAL_PORT_SET_FLOW_ON(_dev)   uart_sw_sps_flow_on(_dev)
#endif

#if defined(CFG_UART_HW_FLOW_CTRL)
   #define _SERIAL_PORT_SET_FLOW_OFF(_dev)  uart_hw_sps_flow_off(_dev)
#elif defined(CFG_UART_SW_FLOW_CTRL)
   #define _SERIAL_PORT_SET_FLOW_OFF(_dev)   uart_sw_sps_flow_off(_dev)
#endif

#endif /* DSPS_PORT_UART_H_ */
