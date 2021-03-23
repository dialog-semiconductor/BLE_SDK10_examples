/**
 ****************************************************************************************
 *
 * @file app_gpio.h
 *
 * @brief Functions definition for configuring GPIO outside adapter context
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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
