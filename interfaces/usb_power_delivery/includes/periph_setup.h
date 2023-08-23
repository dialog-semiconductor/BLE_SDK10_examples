/**
 ****************************************************************************************
 *
 * @file periph_setup.h
 *
 * @brief GPIO defines
 *
 ****************************************************************************************
 */
#ifndef PERIPH_SETUP_H
#define PERIPH_SETUP_H

#include "hw_gpio.h"

// usbpd vin
#define PD_VIN_GPIO_POWER	HW_GPIO_POWER_VDD1V8P
#define PD_VIN_ENA_PORT		HW_GPIO_PORT_0
#define PD_VIN_ENA_PIN		HW_GPIO_PIN_5

// usbpd shared
#define PD_MAIN_GPIO_POWER	HW_GPIO_POWER_V33
#define PD_MAIN_CUR_PORT	HW_GPIO_PORT_1
#define PD_MAIN_CUR_PIN		HW_GPIO_PIN_9
#define PD_MAIN_CUR_ADC		HW_GPADC_INPUT_SE_P1_09
#define PD_MAIN_P5V_PORT	HW_GPIO_PORT_0
#define PD_MAIN_P5V_PIN		HW_GPIO_PIN_25
#define PD_MAIN_P5V_ADC		HW_GPADC_INPUT_SE_P0_25
#define PD_MAIN_SCL_PORT	HW_GPIO_PORT_0
#define PD_MAIN_SCL_PIN		HW_GPIO_PIN_29
#define PD_MAIN_SDA_PORT	HW_GPIO_PORT_0
#define PD_MAIN_SDA_PIN		HW_GPIO_PIN_31

// usbpd sink
#define PD_SINK_GPIO_POWER	HW_GPIO_POWER_V33
#define PD_SINK_RST_PORT	HW_GPIO_PORT_0
#define PD_SINK_RST_PIN		HW_GPIO_PIN_26
#define PD_SINK_IRQ_N_PORT	HW_GPIO_PORT_0
#define PD_SINK_IRQ_N_PIN	HW_GPIO_PIN_28
#define PD_SINK_SDO_PORT	HW_GPIO_PORT_1
#define PD_SINK_SDO_PIN		HW_GPIO_PIN_5
#define PD_SINK_SDI_PORT	HW_GPIO_PORT_0
#define PD_SINK_SDI_PIN		HW_GPIO_PIN_30
#define PD_SINK_CLK_PORT	HW_GPIO_PORT_1
#define PD_SINK_CLK_PIN		HW_GPIO_PIN_4
#define PD_SINK_CS_PORT		HW_GPIO_PORT_0
#define PD_SINK_CS_PIN		HW_GPIO_PIN_17
#define PD_SINK_TXE_PORT	HW_GPIO_PORT_1
#define PD_SINK_TXE_PIN		HW_GPIO_PIN_7

// usbpd source
#define PD_SRC_GPIO_POWER	HW_GPIO_POWER_V33
#define PD_SRC_RST_PORT		HW_GPIO_PORT_1
#define PD_SRC_RST_PIN		HW_GPIO_PIN_8
#define PD_SRC_IRQ_N_PORT	HW_GPIO_PORT_1
#define PD_SRC_IRQ_N_PIN	HW_GPIO_PIN_10
#define PD_SRC_SDO_PORT		HW_GPIO_PORT_1
#define PD_SRC_SDO_PIN		HW_GPIO_PIN_2
#define PD_SRC_SDI_PORT		HW_GPIO_PORT_0
#define PD_SRC_SDI_PIN		HW_GPIO_PIN_27
#define PD_SRC_CLK_PORT		HW_GPIO_PORT_1
#define PD_SRC_CLK_PIN		HW_GPIO_PIN_6
#define PD_SRC_CS_PORT		HW_GPIO_PORT_1
#define PD_SRC_CS_PIN		HW_GPIO_PIN_3
#define PD_SRC_TXE_PORT		HW_GPIO_PORT_1
#define PD_SRC_TXE_PIN		HW_GPIO_PIN_11

#endif //PERIPH_SETUP_H
