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

#include "ad_gpadc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configGPADC_ADAPTER == 1)


/*
 * Define sources connected to GPADC
 */

const ad_gpadc_controller_conf_t BATTERY_LEVEL;
#endif


#endif /* dg_configGPADC_ADAPTER */

#if (dg_configUART_ADAPTER == 1)
#include "ad_uart.h"

extern const ad_uart_controller_conf_t uart2_uart_conf;
extern const ad_uart_controller_conf_t uart3_uart_conf;
#endif

#ifdef __cplusplus
}
#endif /* PLATFORM_DEVICES_H_ */
