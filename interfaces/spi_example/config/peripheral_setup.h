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

/* spi voltage level */
#define SPI_VOLTAGE_LVL                 HW_GPIO_POWER_V33

#define SPI_PORT                        HW_GPIO_PORT_0

#define SPI_MASTER_CLK_PIN              HW_GPIO_PIN_30
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
