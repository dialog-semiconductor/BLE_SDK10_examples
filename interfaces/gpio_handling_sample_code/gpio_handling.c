/**
 ****************************************************************************************
 *
 * @file app_gpio.c
 *
 * @brief Functions definition for configuring GPIO outside adapter context
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

#include "hw_sys.h"
#include "hw_gpio.h"
#include "gpio_handling.h"


/* This function activates an array of GPIO pins */
void app_gpio_pins_set_active(gpio_config *gpio_list, HW_GPIO_POWER pin_power)
{
        /*
         * COM power domain (PD_COM) is not always enabled during M33 runtime. Application
         * must make sure that it's enabled before accessing any GPIO register.
         */
        hw_sys_pd_com_enable();

        /* Scan all the elements of the array until the end is detected (HW_GPIO_PINCONFIG_END) */
        while (gpio_list->pin != 0xFF) {
                uint8_t port = APP_GPIO_GET_PORT(gpio_list->pin);
                uint8_t pin  = APP_GPIO_GET_PIN(gpio_list->pin);
                if (port < HW_GPIO_NUM_PORTS || pin < hw_gpio_port_num_pins[port]) {
                        hw_gpio_configure_pin(port, pin, gpio_list->mode, gpio_list->func, gpio_list->high);
                        hw_gpio_configure_pin_power(port,  pin, pin_power);
                        hw_gpio_pad_latch_enable(port,pin);
                }
                gpio_list++; // move on to the next element of the array
        }
}


/* This function de-activates an array of GPIO pins */
void app_gpio_pins_set_inactive(gpio_config *gpio_list)
{
        /* Scan all the elements of the array until the end is detected (HW_GPIO_PINCONFIG_END) */
        while (gpio_list->pin != 0xFF) {
                uint8_t port = APP_GPIO_GET_PORT(gpio_list->pin);
                uint8_t pin  = APP_GPIO_GET_PIN(gpio_list->pin);
                if (port < HW_GPIO_NUM_PORTS || pin < hw_gpio_port_num_pins[port]) {
                        hw_gpio_configure_pin(port, pin, gpio_list->mode, gpio_list->func, gpio_list->high);
                        hw_gpio_pad_latch_disable(port,pin); /* Lock the status of the pin(s) */
                }
                gpio_list++;
        }

        /* Make sure PD_COM is not already disabled (to avoid potential assertions). */
        if (!REG_GETF(CRG_TOP, PMU_CTRL_REG, COM_SLEEP)){
                hw_sys_pd_com_disable();
        }
}


/* This function activates a single GPIO pin */
void app_gpio_pin_set_active(gpio_config gpio_cfg, HW_GPIO_POWER pin_power)
{
        /*
         * COM power domain (PD_COM) is not always enabled during M33 runtime. Application
         * should make sure that it's enabled before accessing any GPIO register.
         */
        hw_sys_pd_com_enable();

        uint8_t port = APP_GPIO_GET_PORT(gpio_cfg.pin);
        uint8_t pin  = APP_GPIO_GET_PIN(gpio_cfg.pin);
        if (port < HW_GPIO_NUM_PORTS || pin < hw_gpio_port_num_pins[port]) {
                hw_gpio_configure_pin(port, pin, gpio_cfg.mode, gpio_cfg.func, gpio_cfg.high);
                hw_gpio_configure_pin_power(port,  pin, pin_power);
                hw_gpio_pad_latch_enable(port,pin);
        }
}

/* This function de-activates a single GPIO pin */
void app_gpio_pin_set_inactive(gpio_config gpio_cfg)
{
        uint8_t port = APP_GPIO_GET_PORT(gpio_cfg.pin);
        uint8_t pin  = APP_GPIO_GET_PIN(gpio_cfg.pin);
        if (port < HW_GPIO_NUM_PORTS || pin < hw_gpio_port_num_pins[port]) {
                hw_gpio_configure_pin(port, pin, gpio_cfg.mode, gpio_cfg.func, gpio_cfg.high);
                hw_gpio_pad_latch_disable(port,pin); /* Lock the status of the pin */
        }

        /* Make sure PD_COM is not already disabled (to avoid potential assertions). */
        if (!REG_GETF(CRG_TOP, PMU_CTRL_REG, COM_SLEEP)){
                hw_sys_pd_com_disable();
        }
}


/*
 * This function activates and then de-activates a single GPIO pin. It should be used
 * when PD_COM is handled dynamically to change the status of an GPIO OUTPUT pin.
 */
void app_gpio_pin_set(gpio_config gpio_cfg, HW_GPIO_POWER pin_power)
{
        app_gpio_pin_set_active(gpio_cfg, pin_power);
        app_gpio_pin_set_inactive(gpio_cfg);
}


