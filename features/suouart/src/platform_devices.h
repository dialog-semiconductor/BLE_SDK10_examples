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

#if (dg_configUART_ADAPTER == 1)
#include "ad_uart.h"
const ad_uart_controller_conf_t uart_conf;
#endif

#ifdef __cplusplus
}
#endif /* PLATFORM_DEVICES_H_ */

#endif
