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

/* CODEC721x Interface */
#define CODEC_GPIO_LEVEL HW_GPIO_POWER_V33
#define PDM_GPIO_LEVEL HW_GPIO_POWER_VDD1V8P

/* MCLK */
#define CODEC_MCLK_32M_PORT         HW_GPIO_PORT_1
#define CODEC_MCLK_32M_PIN          HW_GPIO_PIN_1

/* PCM Interface */
#define CODEC_PCM_PORT         HW_GPIO_PORT_0
#define CODEC_PCM_CLK_PIN      HW_GPIO_PIN_20
#define CODEC_PCM_FSC_PIN      HW_GPIO_PIN_21
#define CODEC_PCM_DO_PIN       HW_GPIO_PIN_24
#define CODEC_PCM_DI_PIN       HW_GPIO_PIN_26

/* PDM Interface */
#define PDM_CLK_PORT    HW_GPIO_PORT_0
#define PDM_CLK_PIN     HW_GPIO_PIN_7

#define PDM_DATA_PORT    HW_GPIO_PORT_1
#define PDM_DATA_PIN     HW_GPIO_PIN_0

#define PDM_MIC_PE_PORT      HW_GPIO_PORT_1
#define PDM_MIC_PE_PIN       HW_GPIO_PIN_10

/* I2C */
#define CODEC_I2C_PORT    HW_GPIO_PORT_0
#define CODEC_I2C_SCL_PIN     HW_GPIO_PIN_28
#define CODEC_I2C_SDA_PIN     HW_GPIO_PIN_29

#define CODEC_I2C_ADDRESS    ( 0x1A )

#define BUTTON_PORT   KEY1_PORT
#define BUTTON_PIN    KEY1_PIN

#endif /* _PERIPH_SETUP_H_ */
