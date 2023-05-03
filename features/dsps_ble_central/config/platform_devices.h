/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2016-2023 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include "ad_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#if dg_configUART_ADAPTER
/*
 * Define sources connected to UART
 */
extern const ad_uart_controller_conf_t UART_DEVICE;

#endif /* dg_configUART_ADAPTER */

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */
