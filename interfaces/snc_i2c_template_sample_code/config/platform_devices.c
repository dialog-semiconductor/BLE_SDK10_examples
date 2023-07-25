/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board data structures
 *
 * Copyright (C) 2017-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#include "peripheral_setup.h"
#include "platform_devices.h"


/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if dg_configI2C_ADAPTER | dg_configUSE_SNC_HW_I2C

/* I2C1 IO */
const ad_i2c_io_conf_t bus_I2C1 = {

        .scl = {
                .port = I2C1_SCL_PORT,
                .pin  = I2C1_SCL_PIN,
                .on   = {HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, true},
                .off  = {HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true },
        },
        .sda = {
                .port = I2C1_SDA_PORT,
                .pin  = I2C1_SDA_PIN,
                .on   = {HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, true},
                .off  = {HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true },
        },
        .voltage_level = HW_GPIO_POWER_V33 //! Here you can define the required voltage level
};

/* I2C1 bus configurations */
i2c_bus_t I2C1 = &bus_I2C1;

/* External sensor/module I2C driver */
const ad_i2c_driver_conf_t drv_I2C1 = {
        .i2c = {
               .clock_cfg = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
               .speed     = HW_I2C_SPEED_STANDARD,
               .mode      = HW_I2C_MODE_MASTER,
               .addr_mode = HW_I2C_ADDRESSING_7B,
               .address   = I2C_CUSTOM_DEVICE_ADDRESS,
        },
        .dma_channel = HW_DMA_CHANNEL_0
};


/* External sensor/module device configuration */
const ad_i2c_controller_conf_t dev_I2C_CUSTOM_DEVICE = {
        .id  = HW_I2C1,
        .io  = &bus_I2C1,
        .drv = &drv_I2C1
};

i2c_device I2C_CUSTOM_DEVICE = &dev_I2C_CUSTOM_DEVICE;

#endif /* dg_configI2C_ADAPTER | dg_configUSE_SNC_HW_I2C */


