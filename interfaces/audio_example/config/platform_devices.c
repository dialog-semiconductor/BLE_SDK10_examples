/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board data structures
 *
 * Copyright (C) 2017-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <ad_i2c.h>
#include "peripheral_setup.h"
#include "platform_devices.h"

/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if dg_configI2C_ADAPTER

/* I2C1 I/O configuration */
const ad_i2c_io_conf_t i2c_codec = {
        .scl = {
                .port = CODEC_I2C_PORT, .pin = CODEC_I2C_SCL_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .sda = {
                .port = CODEC_I2C_PORT, .pin = CODEC_I2C_SDA_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .voltage_level = CODEC_GPIO_LEVEL
};

/*
 * PLATFORM PERIPHERALS CONTROLLER CONFIGURATION
 *****************************************************************************************
 */

/* CODEC I2C driver configuration */
const ad_i2c_driver_conf_t drv_codec = {
        I2C_DEFAULT_CLK_CFG,
        .i2c.speed              = HW_I2C_SPEED_STANDARD,
        .i2c.mode               = HW_I2C_MODE_MASTER,
        .i2c.addr_mode          = HW_I2C_ADDRESSING_7B,
        .i2c.address            = CODEC_I2C_ADDRESS,
        .dma_channel            = HW_DMA_CHANNEL_INVALID //not used
};

/* CODEC I2C controller configuration */
const ad_i2c_controller_conf_t dev_codec = {
        .id     = HW_I2C1,
        .io     = &i2c_codec,
        .drv    = &drv_codec
};

i2c_device CODEC_DA721x = &dev_codec;

#endif /* dg_configI2C_ADAPTER */

