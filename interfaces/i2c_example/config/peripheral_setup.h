/**
 ****************************************************************************************
 *
 * @file periph_setup.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _PERIPH_SETUP_H_
#define _PERIPH_SETUP_H_

#include "hw_gpio.h"

/* I2X Interface */
#define I2C_GPIO_LEVEL HW_GPIO_POWER_V33


/* I2C */
#define I2C_PORT    HW_GPIO_PORT_0

#define I2C_MASTER_SCL_PIN  HW_GPIO_PIN_28
#define I2C_MASTER_SDA_PIN  HW_GPIO_PIN_29

#define I2C_SLAVE_SCL_PIN   HW_GPIO_PIN_18
#define I2C_SLAVE_SDA_PIN   HW_GPIO_PIN_19

#define I2C_SLAVE_ADDRESS    ( 0xA )

#define I2C_AD_CONFIG_MASTER_I2C_CTRL       (HW_I2C1)
#define I2C_AD_CONFIG_MASTER_DMA_CH         (HW_DMA_CHANNEL_2)
#define I2C_AD_CONFIG_SLAVE_I2C_CTRL        (HW_I2C2)
#define I2C_AD_CONFIG_SLAVE_DMA_CH          (HW_DMA_CHANNEL_4)

#endif /* _PERIPH_SETUP_H_ */
