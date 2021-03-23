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

/* I2C I/O configuration */
const ad_i2c_io_conf_t i2c_master_io = {
        .scl = {
                .port = I2C_PORT, .pin = I2C_MASTER_SCL_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .sda = {
                .port = I2C_PORT, .pin = I2C_MASTER_SDA_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .voltage_level = I2C_GPIO_LEVEL
};

const ad_i2c_io_conf_t i2c_slave_io = {
        .scl = {
                .port = I2C_PORT, .pin = I2C_SLAVE_SCL_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .sda = {
                .port = I2C_PORT, .pin = I2C_SLAVE_SDA_PIN,
                .on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
                .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  }
        },
        .voltage_level = I2C_GPIO_LEVEL
};

/*
 * PLATFORM PERIPHERALS CONTROLLER CONFIGURATION
 *****************************************************************************************
 */

const ad_i2c_driver_conf_t master_driver_config = {
                I2C_DEFAULT_CLK_CFG,
                .i2c.speed = HW_I2C_SPEED_STANDARD,
                .i2c.mode = HW_I2C_MODE_MASTER,
                .i2c.addr_mode = HW_I2C_ADDRESSING_7B,
                .i2c.address = I2C_SLAVE_ADDRESS,
                .i2c.event_cb = NULL,
                .dma_channel = HW_DMA_CHANNEL_INVALID
                /**
                 * I2C master is not configured to use DMA because in case we are connecting the
                 * same board in loopback, the DMA controller will block in case there are blocking
                 * transactions being handled from both the I2C master and slave controller at the same time.
                 *
                 * In case we are connecting two boards we can use DMA for the master as well.
                 */
};

const ad_i2c_driver_conf_t slave_driver_config = {
                I2C_DEFAULT_CLK_CFG,
                .i2c.speed = HW_I2C_SPEED_STANDARD,
                .i2c.mode = HW_I2C_MODE_SLAVE,
                .i2c.addr_mode = HW_I2C_ADDRESSING_7B,
                .i2c.address = I2C_SLAVE_ADDRESS,
                .i2c.event_cb = NULL,
                .dma_channel = I2C_AD_CONFIG_SLAVE_DMA_CH
};

/* I2C controller configuration */
const ad_i2c_controller_conf_t master_config = {
        .id = I2C_AD_CONFIG_MASTER_I2C_CTRL,
        .io = &i2c_master_io,
        .drv = &master_driver_config
};

const ad_i2c_controller_conf_t slave_config = {
        .id = I2C_AD_CONFIG_SLAVE_I2C_CTRL,
        .io = &i2c_slave_io,
        .drv = &slave_driver_config
};

i2c_device I2C_MASTER_DEVICE = &master_config;
i2c_device I2C_SLAVE_DEVICE = &slave_config;

#endif /* dg_configI2C_ADAPTER */

