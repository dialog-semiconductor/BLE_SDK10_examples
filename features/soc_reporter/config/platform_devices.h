/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2016-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include "ad_uart.h"
#include "ad_gpadc.h"
#include "ad_sdadc.h"

#ifdef __cplusplus
extern "C" {
#endif
#if dg_configUART_ADAPTER

#define UART_TX_PORT    (HW_GPIO_PORT_0)
#define UART_TX_PIN     (HW_GPIO_PIN_9)
#define UART_TX_MODE    (HW_GPIO_MODE_OUTPUT)
#define UART_TX_FUNC    (HW_GPIO_FUNC_UART2_TX)

#define UART_RX_PORT    (HW_GPIO_PORT_0)
#define UART_RX_PIN     (HW_GPIO_PIN_8)
#define UART_RX_MODE    (HW_GPIO_MODE_INPUT)
#define UART_RX_FUNC    (HW_GPIO_FUNC_UART2_RX)

extern const ad_uart_io_conf_t uart_io_conf;
extern const ad_uart_driver_conf_t uart_driver_conf;
extern const ad_uart_controller_conf_t uart_controller_conf;

#endif /* dg_configUART_ADAPTER */
#if (dg_configSDADC_ADAPTER == 1)
extern const HW_SDADC_ID SDADC_ID;
extern const ad_sdadc_io_conf_t sdadc_io_conf;
extern const ad_sdadc_driver_conf_t sdadc_driver_conf;
extern const ad_sdadc_controller_conf_t BATTERY_LEVEL_SD;
#endif

#if (dg_configGPADC_ADAPTER == 1)
/*
 * Define sources connected to GPADC
 */

const ad_gpadc_controller_conf_t BATTERY_LEVEL_GP;
const ad_gpadc_controller_conf_t TEMP_LEVEL_GP;

#endif /* dg_configGPADC_ADAPTER */
#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */
