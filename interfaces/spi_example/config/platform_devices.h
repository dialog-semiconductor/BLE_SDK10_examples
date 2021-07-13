/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2017-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */


#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include <ad_spi.h>
#include "peripheral_setup.h"

#if dg_configSPI_ADAPTER

/**
 * \brief SPI device handle
 */
typedef const void* spi_device;

/* List of devices */
extern spi_device SPI_MASTER_DEVICE;
extern spi_device SPI_SLAVE_DEVICE;

#endif /* dg_configSPI_ADAPTER */

#endif /* PLATFORM_DEVICES_H_ */
