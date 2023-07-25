/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Bare Metal (no OS) Blinky Example Application
 *
 * Copyright (c) 2020-2021 Renesas Electronics Corporation and/or its affiliates
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

#include "hw_gpio.h"
#include "hw_watchdog.h"
#include "hw_clk.h"

int main(void)
{
    /* Watchdog is enabled by ROM based bootloader */
    hw_watchdog_freeze();

    /* Configure GPIO used to drive LED D9 on DA14695 Daughter Board */
    hw_gpio_set_pin_function(HW_GPIO_PORT_1, HW_GPIO_PIN_1, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO);
    hw_gpio_pad_latch_enable(HW_GPIO_PORT_1, HW_GPIO_PIN_1);

    /* Blink LED */
    while (1) {
        /* Turn LED on */
        hw_gpio_set_active(HW_GPIO_PORT_1, HW_GPIO_PIN_1);
        hw_clk_delay_usec(1000000);
        /* Turn LED off */
        hw_gpio_set_inactive(HW_GPIO_PORT_1, HW_GPIO_PIN_1);
        hw_clk_delay_usec(1000000);
    }
    return 0;
}
