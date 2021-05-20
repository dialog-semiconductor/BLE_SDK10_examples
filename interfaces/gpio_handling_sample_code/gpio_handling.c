/**
 ****************************************************************************************
 *
 * @file app_gpio.c
 *
 * @brief Functions definition for configuring GPIO outside adapter context
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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


