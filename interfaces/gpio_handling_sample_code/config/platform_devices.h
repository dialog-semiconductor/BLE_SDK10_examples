/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include "hw_gpio.h"


/*
 * Pins can be grouped according to their functionalities (e.g. input, output). This is
 * convenient in cases where latch/unlatch functionality is not performed on all pins.
 */

/* Input GPIO pins configuration array */
__UNUSED static const gpio_config input_gpio_cfg[] = {

        HW_GPIO_PINCONFIG(KEY1_PORT, KEY1_PIN, INPUT_PULLUP, GPIO, false),

        HW_GPIO_PINCONFIG_END // important!!!
};

/*
 * Output GPIO pins configuration array.
 *
 * \note: Since the elements of the array might be modified during the code execution,
 *        you should not assign the attribute 'const'. */
static gpio_config output_gpio_cfg[] = {

        HW_GPIO_PINCONFIG(LED1_PORT, LED1_PIN, OUTPUT, GPIO, false),

        HW_GPIO_PINCONFIG_END // important!!!
};

/* Miscellaneous pins configuration array (e.g. PWM pins) */
__UNUSED static gpio_config miscellaneous_gpio_cfg[] = {

        HW_GPIO_PINCONFIG_END // important!!!
};

#endif /* PLATFORM_DEVICES_H_ */

