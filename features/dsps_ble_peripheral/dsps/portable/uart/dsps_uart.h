/**
 ****************************************************************************************
 *
 * @file dsps_uart.h
 *
 * @brief DSPS UART
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

#ifndef DSPS_UART_H_
#define DSPS_UART_H_

#if dg_configUART_ADAPTER

#include "ad_uart.h"

#if defined(CFG_UART_HW_FLOW_CTRL) && defined(CFG_UART_SW_FLOW_CTRL)
    #error "Only one UART flow control mode should be used!"
#endif

#ifndef UART_TX_PORT
   #define UART_TX_PORT             ( SER1_TX_PORT )
#endif

#ifndef UART_TX_PIN
   #define UART_TX_PIN              ( SER1_TX_PIN )
#endif

#ifndef UART_RX_PORT
   #define UART_RX_PORT             ( SER1_RX_PORT )
#endif

#ifndef UART_RX_PIN
   #define UART_RX_PIN              ( SER1_RX_PIN )
#endif

#ifndef UART_RTS_PORT
   #define UART_RTS_PORT            ( SER1_RTS_PORT )
#endif

#ifndef UART_RTS_PIN
   #define UART_RTS_PIN             ( SER1_RTS_PIN )
#endif

#ifndef UART_CTS_PORT
   #define UART_CTS_PORT            ( SER1_CTS_PORT )
#endif

#ifndef UART_CTS_PIN
   #define UART_CTS_PIN             ( SER1_CTS_PIN )
#endif

/* SW flow control bytes */
#define UART_XON_BYTE        0x11
#define UART_XOFF_BYTE       0x13

ad_uart_handle_t uart_open(const ad_uart_controller_conf_t *ctr);

int uart_close(ad_uart_handle_t handle);

uint32_t uart_read_timeout(HW_UART_BAUDRATE baud, uint32_t rx_size);

int read_from_uart(ad_uart_handle_t handle, char *buf, uint32_t len, OS_TICK_TIME timeout);

int write_to_uart(ad_uart_handle_t handle, const char *buf, uint32_t len);

void uart_hw_sps_flow_off(const ad_uart_controller_conf_t *ctr);

void uart_sw_sps_flow_off(ad_uart_handle_t handle);

void uart_hw_sps_flow_on(const ad_uart_controller_conf_t *ctr);

void uart_sw_sps_flow_on(ad_uart_handle_t handle);

#endif
#endif /* DSPS_UART_H_ */
