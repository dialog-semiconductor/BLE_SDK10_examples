/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include <ad_spi.h>
#include "snc_hw_spi.h"

#include "peripheral_setup.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _SPI_CUSTOM_DEVICE_


#if dg_configSPI_ADAPTER || dg_configUSE_SNC_HW_SPI


/**
 * \brief SPI bus handle
 */
typedef const void* spi_bus_t;


/**
 * \brief SPI device handle
 */
typedef const void* spi_device;


#endif /* dg_configSPI_ADAPTER || dg_configUSE_SNC_HW_SPI */



/* List of devices */
extern spi_device SPI_CUSTOM_DEVICE;


#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */

