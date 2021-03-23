/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2016-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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
