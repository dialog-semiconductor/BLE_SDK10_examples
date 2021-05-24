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

#define DA7218  (8)
#define DA7212  (2)

#define DA721X DA7218

/**
 * \brief I2C device handle
 */
typedef const void* i2c_device;

/*
 * I2C DEVICES
 *****************************************************************************************
 */
#if dg_configI2C_ADAPTER

/**
 * \brief DA7218 CODEC device
 */
extern i2c_device CODEC_DA721x;

#endif /* dg_configI2C_ADAPTER */


#endif /* PLATFORM_DEVICES_H_ */
