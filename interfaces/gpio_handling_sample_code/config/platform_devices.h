/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
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

