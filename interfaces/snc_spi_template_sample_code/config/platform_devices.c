/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board data structures
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <ad_spi.h>
#include "snc_hw_spi.h"

#include "peripheral_setup.h"
#include "platform_devices.h"


/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if dg_configSPI_ADAPTER | dg_configUSE_SNC_HW_SPI

#ifdef _SPI_CUSTOM_DEVICE_

/* SPI chip-select pins */
static const ad_io_conf_t spi_master_cs[] = {{

        .port = CUSTOM_DEVICE_CS_GPIO_PORT,
        .pin  = CUSTOM_DEVICE_CS_GPIO_PIN,
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
const ad_spi_io_conf_t bus_SPI1 = {

        .spi_do = {
                .port = SPI1_DO_GPIO_PORT,
                .pin  = SPI1_DO_GPIO_PIN,
                .on   = {HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_DO, false},
                .off  = {HW_GPIO_MODE_INPUT,            HW_GPIO_FUNC_GPIO,   true},
        },
        .spi_di = {
                .port = SPI1_DI_GPIO_PORT,
                .pin  = SPI1_DI_GPIO_PIN,
                .on   = {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_SPI_DI, false},
                .off  = {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO,   true},
        },
        .spi_clk = {
                .port = SPI1_CLK_GPIO_PORT,
                .pin  = SPI1_CLK_GPIO_PIN,
                .on   = {HW_GPIO_MODE_OUTPUT_PUSH_PULL, HW_GPIO_FUNC_SPI_CLK, false},
                .off  = {HW_GPIO_MODE_INPUT,            HW_GPIO_FUNC_GPIO,    true},
        },

        /*
         * The number of pins in spi_master_cs array.
         *
         * \warning When the SPI bus is used by SNC \p cs_cnt must be always 1
         */
        .cs_cnt = 1,
        .spi_cs = spi_master_cs,

        .voltage_level = HW_GPIO_POWER_V33 // Here you can define the required voltage level
};

/* SPI1 bus configurations */
spi_bus_t SPI1 = &bus_SPI1;


/* External sensor/module SPI driver */
const ad_spi_driver_conf_t drv_SPI1 = {
        .spi = {
                .cs_pad = {CUSTOM_DEVICE_CS_GPIO_PORT, CUSTOM_DEVICE_CS_GPIO_PIN},
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


/* Sensor/module device configuration */
const ad_spi_controller_conf_t dev_SPI_CUSTOM_DEVICE = {
        .id  = HW_SPI1,
        .io  = &bus_SPI1,
        .drv = &drv_SPI1
};



spi_device SPI_CUSTOM_DEVICE = &dev_SPI_CUSTOM_DEVICE;

#endif /* CUSTOM_SPI_EN */

#endif /* dg_configSPI_ADAPTER | dg_configUSE_SNC_HW_SPI */


