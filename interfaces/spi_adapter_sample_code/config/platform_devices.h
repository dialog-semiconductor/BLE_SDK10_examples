/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include <ad_spi.h>
#include "peripheral_setup.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configSPI_ADAPTER == 1) || (dg_configUSE_HW_SPI == 1)

#if (MCP482_MODULE_USED == 1)
/**
 * \brief SPI device handle
 */
typedef const void* spi_device;

/* List of devices */
extern spi_device MCP4822_DEVICE;

#endif /* MCP482_MODULE_USED */
#endif /* dg_configSPI_ADAPTER || dg_configUSE_HW_SPI */

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */
