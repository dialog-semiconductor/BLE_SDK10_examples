/**
 ****************************************************************************************
 *
 * @file periph_setup.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef _PERIPH_SETUP_H_
#define _PERIPH_SETUP_H_

#include "hw_gpio.h"

/* spi voltage level */
#define SPI_VOLTAGE_LVL                 HW_GPIO_POWER_V33

#define SPI_PORT                        HW_GPIO_PORT_0

#define SPI_MASTER_CLK_PIN              HW_GPIO_PIN_28
#define SPI_MASTER_CS_PIN               HW_GPIO_PIN_31
#define SPI_MASTER_DI_PIN               HW_GPIO_PIN_20
#define SPI_MASTER_DO_PIN               HW_GPIO_PIN_21

#define SPI_SLAVE_CLK_PIN               HW_GPIO_PIN_24
#define SPI_SLAVE_CS_PIN                HW_GPIO_PIN_25
#define SPI_SLAVE_DI_PIN                HW_GPIO_PIN_27
#define SPI_SLAVE_DO_PIN                HW_GPIO_PIN_26


#define AD_CONFIG_MASTER_SPI_CTRL       (HW_SPI1)
#define AD_CONFIG_SLAVE_SPI_CTRL        (HW_SPI2)

#endif /* _PERIPH_SETUP_H_ */
