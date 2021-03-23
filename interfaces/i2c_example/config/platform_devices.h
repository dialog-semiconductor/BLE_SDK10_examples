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
 * \brief I2C peer devices
 */
extern i2c_device I2C_MASTER_DEVICE;
extern i2c_device I2C_SLAVE_DEVICE;

#endif /* dg_configI2C_ADAPTER */


#endif /* PLATFORM_DEVICES_H_ */
