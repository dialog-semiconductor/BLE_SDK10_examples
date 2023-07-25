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

#include <ad_spi.h>
#include "hw_spi.h"
#include "peripheral_setup.h"
#include "platform_devices.h"

/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if dg_configSPI_ADAPTER

/* SPI Master chip-select pins */
static const ad_io_conf_t spi_master_cs[] = {{

        .port = SPI_PORT,
        .pin  = SPI_MASTER_CS_PIN,
        .on = {
                .mode     = HW_GPIO_MODE_OUTPUT_PUSH_PULL,
                .function = HW_GPIO_FUNC_SPI_EN,
                .high     = true
        },
        .off = {
                .mode     = HW_GPIO_MODE_OUTPUT_PUSH_PULL,
                .function = HW_GPIO_FUNC_SPI_EN,
                .high     = true
        }},
};

/* SPI Master I/O configuration */
const ad_spi_io_conf_t spi_master_io = {
        .spi_do =  { SPI_PORT, SPI_MASTER_DO_PIN,       { HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_DO, false },  { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true } },
        .spi_di =  { SPI_PORT, SPI_MASTER_DI_PIN,       { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI_DI, false },             { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true } },
        .spi_clk = { SPI_PORT, SPI_MASTER_CLK_PIN,      { HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_CLK, false }, { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true } },
        .cs_cnt = 1,
        .spi_cs = spi_master_cs,
        .voltage_level = SPI_VOLTAGE_LVL

};

/* SPI Master chip-select pins */
static const ad_io_conf_t spi_slave_cs[] = {{

        .port = SPI_PORT,
        .pin  = SPI_SLAVE_CS_PIN,
        .on = {
                .mode     = HW_GPIO_MODE_INPUT,
                .function = HW_GPIO_FUNC_SPI2_EN,
                .high     = false
        },
        .off = {
                .mode     = HW_GPIO_MODE_INPUT,
                .function = HW_GPIO_FUNC_SPI2_EN,
                .high     = false
        }},
};

/* SPI Slave I/O configuration */
const ad_spi_io_conf_t spi_slave_io = {
        .spi_do =  { SPI_PORT, SPI_SLAVE_DO_PIN,        { HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI2_DO, false },         { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true } },
        .spi_di =  { SPI_PORT, SPI_SLAVE_DI_PIN,        { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI2_DI, false },                    { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true } },
        .spi_clk = { SPI_PORT, SPI_SLAVE_CLK_PIN,       { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI2_CLK, false },                   { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true } },
        .cs_cnt = 1,
        .spi_cs = spi_slave_cs,
        .voltage_level = SPI_VOLTAGE_LVL
};

/*
 * PLATFORM PERIPHERALS CONTROLLER CONFIGURATION
 *****************************************************************************************
 */

const ad_spi_driver_conf_t master_driver_config = {
        .spi = {
                .cs_pad = {SPI_PORT, SPI_MASTER_CS_PIN},
                .word_mode = HW_SPI_WORD_8BIT,      // Here you can define the required
                .smn_role  = HW_SPI_MODE_MASTER,
                .polarity_mode = HW_SPI_POL_LOW,    // Here you can define the required SPI polarity
                .phase_mode    = HW_SPI_PHA_MODE_0, // Here you can define the required SPI phase
                .mint_mode = HW_SPI_MINT_DISABLE,
                .xtal_freq = HW_SPI_FREQ_DIV_8,     // Here you can declare your own SPI speed
                .fifo_mode = HW_SPI_FIFO_RX_TX,
                .disabled  = 0, /* Should be disabled during initialization phase */
                .ignore_cs = false,
                .use_dma   = true,
                .rx_dma_channel = HW_DMA_CHANNEL_0,
                .tx_dma_channel = HW_DMA_CHANNEL_1
        }
};

const ad_spi_driver_conf_t slave_driver_config = {
        .spi = {
                .cs_pad = {SPI_PORT, SPI_SLAVE_CS_PIN},
                .word_mode = HW_SPI_WORD_8BIT,      // Here you can define the required
                .smn_role  = HW_SPI_MODE_SLAVE,
                .polarity_mode = HW_SPI_POL_LOW,    // Here you can define the required SPI polarity
                .phase_mode    = HW_SPI_PHA_MODE_0, // Here you can define the required SPI phase
                .mint_mode = HW_SPI_MINT_DISABLE,
                .xtal_freq = HW_SPI_FREQ_DIV_2,     // Here you can declare your own SPI speed
                .fifo_mode = HW_SPI_FIFO_RX_TX,
                .disabled  = 0, /* Should be disabled during initialization phase */
                .ignore_cs = false,
                .use_dma   = true,
                .rx_dma_channel = HW_DMA_CHANNEL_2,
                .tx_dma_channel = HW_DMA_CHANNEL_3
        }
};

/* I2C controller configuration */
const ad_spi_controller_conf_t master_config = {
        .id = AD_CONFIG_MASTER_SPI_CTRL,
        .io = &spi_master_io,
        .drv = &master_driver_config
};

const ad_spi_controller_conf_t slave_config = {
        .id = AD_CONFIG_SLAVE_SPI_CTRL,
        .io = &spi_slave_io,
        .drv = &slave_driver_config
};

spi_device SPI_MASTER_DEVICE = &master_config;
spi_device SPI_SLAVE_DEVICE = &slave_config;

#endif /* dg_configI2C_ADAPTER */

