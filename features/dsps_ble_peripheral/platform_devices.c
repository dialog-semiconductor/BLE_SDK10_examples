/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2016-2023 Renesas Electronics Corporation and/or its affiliates
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

#include <stdio.h>
#include <stdbool.h>
#include "ad_uart.h"
#include "platform_devices.h"
#include "dsps_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#if dg_configUART_ADAPTER

/*
 * Define sources connected to UART
 */
const ad_uart_driver_conf_t uart_drv = {
        .hw_conf = {
                .baud_rate = CFG_UART_SPS_BAUDRATE,
                .data = HW_UART_DATABITS_8,
                .parity = HW_UART_PARITY_NONE,
                .stop = HW_UART_STOPBITS_1,
#ifdef CFG_UART_HW_FLOW_CTRL
                .auto_flow_control = 1,
#endif
#ifdef CFG_UART_SW_FLOW_CTRL
                .auto_flow_control = 0,
#endif
                .use_dma = 1,
                .use_fifo = 1,
                .tx_fifo_tr_lvl = 0,
                .rx_fifo_tr_lvl = 0,
                .tx_dma_burst_lvl = 0,
                .rx_dma_burst_lvl = 0,
                .tx_dma_channel = HW_DMA_CHANNEL_3,
                .rx_dma_channel = HW_DMA_CHANNEL_2,
        }
};

const ad_uart_io_conf_t uart_bus = {
        .rx = {
                .port = UART_RX_PORT, .pin = UART_RX_PIN,
                .on =  { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_UART2_RX, false },
                .off = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true },
        },
        .tx = {
                .port = UART_TX_PORT, .pin = UART_TX_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_UART2_TX, false },
                .off = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true },
        },
#ifdef CFG_UART_HW_FLOW_CTRL
        .rtsn = {
                .port = UART_RTS_PORT, .pin = UART_RTS_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_UART2_RTSN, false },
                /*
                 * Keep the RTS line de-asserted (logic high) as it might happen that the BLE
                 * connection with a peer device is dropped and at the same time the device
                 * is being reading data from the serial interface (e.g. sending a file).
                 * In this case and because the RTS line will no longer be driven high (de-asserted)
                 * the serial console will keep reading data over the UART interface.
                 * A drawback to this approach is the slight increase in power consumption as the
                 * pin is driven high.
                 */
                .off = { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true },
        },
        .ctsn = {
                .port = UART_CTS_PORT, .pin = UART_CTS_PIN,
                .on =  { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_UART2_CTSN, false },
                .off = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true },
        },
#endif
        .voltage_level = HW_GPIO_POWER_V33,
};

const ad_uart_controller_conf_t UART_DEVICE = {
        SER1_UART,
        &uart_bus,
        &uart_drv
};
#endif /* dg_configUART_ADAPTER */

#ifdef __cplusplus
}
#endif
