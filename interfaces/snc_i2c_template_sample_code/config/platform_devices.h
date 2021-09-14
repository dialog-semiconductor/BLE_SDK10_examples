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

#include <ad_i2c.h>
#include "snc_hw_i2c.h"

#include "peripheral_setup.h"

#ifdef __cplusplus
extern "C" {
#endif


#if dg_configI2C_ADAPTER || dg_configUSE_SNC_HW_I2C

/**
 * \brief I2C bus handle
 */
typedef const void* i2c_bus_t;

/**
 * \brief I2C device handle
 */
typedef const void* i2c_device;

#endif /* dg_configI2C_ADAPTER || dg_configUSE_SNC_HW_I2C */

/* List of devices */
extern i2c_device I2C_CUSTOM_DEVICE;

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */

