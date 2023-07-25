/**
 ****************************************************************************************
 *
 * @file misc.c
 *
 * @brief Application code related to hardware
 *
 * Copyright (C) 2018-2023 Renesas Electronics Corporation and/or its affiliates
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
#include "osal.h"
#include "hw_pdc.h"
#include "misc.h"

#define GET_TIMESTAMP_MULTIPLIER          1000000UL
#define GET_TIMESTAMP_DIVIDER             configSYSTICK_CLOCK_HZ

#define APP_GPIO_GET_PORT(_pin)  ( (_pin) >> HW_GPIO_PIN_BITS)
#define APP_GPIO_GET_PIN(_pin)   ( (_pin) & ((1 << HW_GPIO_PIN_BITS) - 1) )

/* This function activates an array of GPIO pins */
void app_gpio_pins_cfg(const gpio_config *gpio_list, HW_GPIO_POWER pin_power)
{
        /*
         * COM power domain (PD_COM) is not always enabled during M33 runtime. Application
         * must make sure that it's enabled before accessing any GPIO register.
         */
        hw_sys_pd_com_enable();

        /* Scan all the elements of the array until the end is detected (HW_GPIO_PINCONFIG_END) */
        while (gpio_list->pin != 0xFF) {
                uint8_t port = APP_GPIO_GET_PORT(gpio_list->pin);
                uint8_t pin = APP_GPIO_GET_PIN(gpio_list->pin);

                if (port < HW_GPIO_NUM_PORTS && pin < hw_gpio_port_num_pins[port]) {
                        hw_gpio_configure_pin(port, pin, gpio_list->mode, gpio_list->func, gpio_list->high);
                        hw_gpio_configure_pin_power(port,  pin, pin_power);
                        hw_gpio_pad_latch_enable(port,pin);
                        hw_gpio_pad_latch_disable(port, pin);
                }
                gpio_list++; // move on to the next element of the array
        }
        hw_sys_pd_com_disable();
}

void app_gpio_pins_pdc_cfg(const gpio_config *gpio_list)
{
        /* Scan all the elements of the array until the end is detected (HW_GPIO_PINCONFIG_END) */
        while (gpio_list->pin != 0xFF) {
                uint8_t port = APP_GPIO_GET_PORT(gpio_list->pin);
                uint8_t pin = APP_GPIO_GET_PIN(gpio_list->pin);

                if (port < HW_GPIO_NUM_PORTS && pin < hw_gpio_port_num_pins[port]) {
                        uint32_t pdc_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(port, pin,
                                                                            HW_PDC_MASTER_CM33,
                                    (dg_configENABLE_XTAL32M_ON_WAKEUP ? HW_PDC_LUT_ENTRY_EN_XTAL : 0)));
                        ASSERT_WARNING(pdc_id != HW_PDC_INVALID_LUT_INDEX);

                        /* Do the trick! */
                        hw_pdc_set_pending(pdc_id);
                        hw_pdc_acknowledge(pdc_id);
                }
                gpio_list++; // move on to the next element of the array
        }
}

/*********************************************************************
 *
 *       Non static code
 *
 *********************************************************************
 */

uint64_t get_sys_timestamp_us(SYS_TIMESTAMP state)
{
#if !defined(OS_BAREMETAL)
        static uint64_t timestamp;

        if (state == SYS_TIMESTAMP_START) {
                timestamp = __sys_ticks_timestamp();

                return 0; // Just to suppress compiler warnings
        }
        else if (state == SYS_TIMESTAMP_STOP) {
                return ((__sys_ticks_timestamp() - timestamp) *
                                        GET_TIMESTAMP_MULTIPLIER / GET_TIMESTAMP_DIVIDER);
        }
        else {
                ASSERT_WARNING(0); // Invalid value
                return 0;
        }
#endif
}

bool user_button_is_pressed(void)
{
#if defined(HARDWARE_PLATFORM_USB_VB)
        return  hw_gpio_get_pin_status(KEY1_PORT, KEY1_PIN);
#else
        return !hw_gpio_get_pin_status(KEY1_PORT, KEY1_PIN);
#endif
}
