/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board data structures
 *
 * Copyright (C) 2017-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <ad_spi.h>
#include "peripheral_setup.h"
#include "platform_devices.h"

/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if (dg_configSPI_ADAPTER == 1) || (dg_configUSE_HW_SPI == 1)
#if (MCP482_MODULE_USED == 1)

/* SPI chip-select pins */
static const ad_io_conf_t cs_MCP4822[] = {{

        .port = MCP4822_CS_PORT,
        .pin  = MCP4822_CS_PIN,
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

/* SPI1 IO */
static const ad_spi_io_conf_t bus_MCP4822 = {

        .spi_do = {
                .port = MCP4822_DO_PORT,
                .pin  = MCP4822_DO_PIN,
                .on   = {HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_DO, false},
                .off  = {HW_GPIO_MODE_INPUT,            HW_GPIO_FUNC_GPIO,   true},
        },
        .spi_di = { // Not required
                .port = MCP4822_DI_PORT,
                .pin  = MCP4822_DI_PIN,
                .on   = {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI_DI, false},
                .off  = {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO,   true},
        },
        .spi_clk = {
                .port = MCP4822_CLK_PORT,
                .pin  = MCP4822_CLK_PIN,
                .on   = {HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_CLK, false},
                .off  = {HW_GPIO_MODE_INPUT,            HW_GPIO_FUNC_GPIO,    true},
        },

        /*
         * The number of pins in spi_master_cs array.
         *
         * \warning When the SPI bus is used by SNC \p cs_cnt must be always 1
         */
        .cs_cnt = 1,
        .spi_cs = cs_MCP4822,

        .voltage_level = HW_GPIO_POWER_V33
};

/* External sensor/module SPI driver */
static const ad_spi_driver_conf_t drv_MCP4822 = {
        .spi = {
                .cs_pad         = { MCP4822_CS_PORT, MCP4822_CS_PIN },
                .word_mode      = HW_SPI_WORD_16BIT,    /* Required by the module used */
                .smn_role       = HW_SPI_MODE_MASTER,
                .polarity_mode  = HW_SPI_POL_LOW,
                .phase_mode     = HW_SPI_PHA_MODE_0,
                .mint_mode      = HW_SPI_MINT_ENABLE,
                .xtal_freq      = HW_SPI_FREQ_DIV_8,    /* Operating freq. @ DIVN / xtal_freq = 4MHz */
                .fifo_mode      = HW_SPI_FIFO_RX_TX,
                .disabled       = 0,                    /* Should be disabled during initialization phase */
                .ignore_cs      = false,
                .use_dma        = false,
                .rx_dma_channel = HW_DMA_CHANNEL_0,
                .tx_dma_channel = HW_DMA_CHANNEL_1
        }
};

/* Sensor/module device configuration */
const ad_spi_controller_conf_t dev_MCP4822 = {
        .id  = HW_SPI1,
        .io  = &bus_MCP4822,
        .drv = &drv_MCP4822,
};

spi_device MCP4822_DEVICE = &dev_MCP4822;

#endif /* MCP482_MODULE_USED */
#endif /* dg_configSPI_ADAPTER || dg_configUSE_HW_SPI */

