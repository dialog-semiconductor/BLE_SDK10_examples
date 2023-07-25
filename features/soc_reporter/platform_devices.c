/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2016-2021 Renesas Electronics Corporation and/or its affiliates
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

#include <stdio.h>
#include <stdbool.h>
#include "platform_devices.h"

#ifdef __cplusplus
extern "C" {
#endif

#if dg_configUART_ADAPTER
const ad_uart_io_conf_t uart_io_conf = {
        /* Rx UART2 */
        {
                .port = UART_RX_PORT,
                .pin = UART_RX_PIN,
                /* On */
                {
                        .mode = UART_RX_MODE,
                        .function = UART_RX_FUNC,
                        .high = true,
                },
                /* Off */
                {
                        .mode = UART_RX_MODE,
                        .function = UART_RX_FUNC,
                        .high = true,
                },
        },
        /* Tx UART2 */
        {
                .port = UART_TX_PORT,
                .pin = UART_TX_PIN,
                /* On */
                {
                        .mode = UART_TX_MODE,
                       .function = UART_TX_FUNC,
                        .high = true,
                },
                /* Off */
                {
                        .mode = UART_TX_MODE,
                        .function = UART_TX_FUNC,
                        .high = true,
                },
        },

        /* Voltage Level */
        .voltage_level = HW_GPIO_POWER_V33,
};

const ad_uart_driver_conf_t uart_driver_conf = {
        {

                .baud_rate = HW_UART_BAUDRATE_115200,
                .data = HW_UART_DATABITS_8,
                .parity = HW_UART_PARITY_NONE,
                .stop = HW_UART_STOPBITS_1,
                .auto_flow_control = 0,
                .use_fifo = 1,
                .use_dma = 1,
                .tx_dma_channel = HW_DMA_CHANNEL_3,
                .rx_dma_channel = HW_DMA_CHANNEL_2,
                .tx_fifo_tr_lvl = 0,
                .rx_fifo_tr_lvl = 0,
        }
};

const ad_uart_controller_conf_t uart_controller_conf = {
        .id = HW_UART2,
        .io = &uart_io_conf,
        .drv = &uart_driver_conf,
};
#endif /* dg_cinfigUART_ADAPTER */

#if (dg_configSDADC_ADAPTER == 1)

const HW_SDADC_ID SDADC_ID = HW_SDADC;

const ad_sdadc_driver_conf_t sdadc_driver_conf = {
        .clock = 0,             /**< Clock source */
        .input_mode = HW_SDADC_INPUT_MODE_SINGLE_ENDED,        /**< Input mode */
        .inn = 0,               /**< ADC negative input */
        .inp = HW_SDADC_INP_VBAT,               /**< ADC positive input */
        .continuous = false,        /**< Continuous mode state */
        .over_sampling = HW_SDADC_OSR_128,     /**< Oversampling rate */
        .vref_selection = HW_SDADC_VREF_INTERNAL,    /**< VREF source selection (internal/external) */
        .vref_voltage = HW_SDADC_VREF_VOLTAGE_INTERNAL,      /**< Reference voltage (mV) - MUST be set to 1.2V when vref source is internal */
        .use_dma = false,           /**< DMA functionality enable/disable */
        .mask_int = false,          /**< Enable/Disable (mask) SDADC interrupt */
#if (dg_configDEVICE == DEVICE_DA1469x)
        .freq = HW_SDADC_CLOCK_FREQ_250K,
#endif
};

const ad_sdadc_io_conf_t sdadc_io_conf = {
        {
                .port = HW_GPIO_PORT_NONE,
                .pin = HW_GPIO_PIN_NONE,
        },
        {
                .port = HW_GPIO_PORT_NONE,
                .pin = HW_GPIO_PIN_NONE,
        },

        /* Voltage Level */
        .voltage_level = HW_GPIO_POWER_NONE,
};

const ad_sdadc_controller_conf_t BATTERY_LEVEL_SD = {
        HW_SDADC,
        &sdadc_io_conf,
        &sdadc_driver_conf
};
#endif

#if (dg_configGPADC_ADAPTER == 1)
/*
 * Define sources connected to GPADC
 */
const ad_gpadc_driver_conf_t battery_level_driver = {
        .clock                  = HW_GPADC_CLOCK_INTERNAL,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .input                  = HW_GPADC_INPUT_SE_VBAT,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = 15,
        .chopping               = true,
        .oversampling           = HW_GPADC_OVERSAMPLING_4_SAMPLES,
        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_1V2,
};

const ad_gpadc_controller_conf_t BATTERY_LEVEL_GP = {
        HW_GPADC_1,
        NULL,
        &battery_level_driver
};

const ad_gpadc_driver_conf_t temp_level_driver = {
        .clock                  = HW_GPADC_CLOCK_INTERNAL,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .input                  = HW_GPADC_INPUT_SE_P1_09,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = 15,
        .chopping               = true,
        .oversampling           = HW_GPADC_OVERSAMPLING_4_SAMPLES,
        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_3V6,
};

const ad_gpadc_io_conf_t temp_io_conf = {
        {
                .port = HW_GPIO_PORT_1,
                .pin = HW_GPIO_PIN_9,
                /* On */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_ADC,
                        .high = true,
                },
                /* Off */
                {
                        .mode = HW_GPIO_MODE_INPUT,
                        .function = HW_GPIO_FUNC_ADC,
                        .high = true,
                },
        },
        {
                .port = HW_GPIO_PORT_NONE,
                .pin = HW_GPIO_PIN_NONE,
        },

        /* Voltage Level */
        .voltage_level = HW_GPIO_POWER_V33,
};

const ad_gpadc_controller_conf_t TEMP_LEVEL_GP = {
        HW_GPADC_1,
        &temp_io_conf,
        &temp_level_driver
};

#endif /* dg_configGPADC_ADAPTER */
#ifdef __cplusplus
}
#endif
