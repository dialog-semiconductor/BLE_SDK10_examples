/**
 ****************************************************************************************
 *
 * @file app_gpio.h
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

#ifndef GPIO_HANDLING_H_
#define GPIO_HANDLING_H_


#include "hw_gpio.h"


#define APP_GPIO_GET_PORT(pin)  ((pin&0x3F) >> HW_GPIO_PIN_BITS)
#define APP_GPIO_GET_PIN(pin)   (pin & ((1 << HW_GPIO_PIN_BITS) - 1))


/*
 * @brief Configures an array of GPIO pins, by setting pins mode, function, power rail
 *        and enabling its pad latch
 *
 * \param [in] gpio_list GPIO list with pins used in application level.
 *
 * \param [in] pin_power GPIO pin power source
 *
 * \note The number of calls to app_gpio_pins_set_active function should
 *       match the number of calls to app_gpio_pins_set_inactive function.
 *
 */
void app_gpio_pins_set_active(gpio_config *gpio_list, HW_GPIO_POWER pin_power);


/*
 * @brief De-configures an array of GPIO pins, by de-configuring pins mode, function and
 *        disabling its pad latch
 *
 * \param [in] gpio_list GPIO list with pins used in application level.
 *
 * \note The number of calls to app_gpio_pins_set_active function should
 *       match the number of calls to app_gpio_pins_set_inactive function.
 *
 */
void app_gpio_pins_set_inactive(gpio_config *gpio_list);


/*
 * @brief Configures a single GPIO pin, by configuring pin mode, power and
 *        enabling its pad latch
 *
 * \param [in] gpio_cfg GPIO pin configuration struct
 *
 * \param [in] pin_power GPIO pin power source
 *
 * \note The number of calls to app_gpio_pin_set_active function should
 *       match the number of calls to app_gpio_pin_set_inactive function.
 *
 * */
void app_gpio_pin_set_active(gpio_config gpio_cfg, HW_GPIO_POWER pin_power);


/*
 * @brief De-configures a single GPIO pin, by de-configuring pin mode, function and
 *        disabling its pad latch.
 *
 * \param [in] gpio_cfg GPIO pin configuration struct
 *
 * \note The number of calls to app_gpio_pin_set_active function should
 *       match the number of calls to app_gpio_pin_set_inactive function.
 *
 * */
void app_gpio_pin_set_inactive(gpio_config gpio_cfg);


/*
 * @brief Sets the state of a GPIO pin. Configures the pin, sets its state, then deconfigures it.
 *        This function must be used when PD_COM is handled dynamically to change the state of a
 *        GPIO OUTPUT pin.
 *
 * \param [in] gpio_cfg GPIO pin configuration struct
 *
 * \param [in] pin_power GPIO pin power source
 *
 * */
void app_gpio_pin_set(gpio_config gpio_cfg, HW_GPIO_POWER pin_power);


#endif /* GPIO_HANDLING_H_ */
