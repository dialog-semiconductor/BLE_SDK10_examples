/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2016-2021 Renesas Electronics Corporation and/or its affiliates
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

#include "ad_gpadc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configGPADC_ADAPTER == 1)


/*
 * Define sources connected to GPADC
 */

ad_gpadc_io_conf_t adc_io_conf = {
        .input0 = {
                        HW_GPIO_PORT_1,
                        HW_GPIO_PIN_9,
                        {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, false},
                        {HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_ADC, false}
                },
        .voltage_level = HW_GPIO_POWER_V33
};

ad_gpadc_driver_conf_t gpdadc_conf =  {
        .clock            = HW_GPADC_CLOCK_DIGITAL,              /**< clock source */
        .input_mode       = HW_GPADC_INPUT_MODE_SINGLE_ENDED,    /**< input mode */
        .input            = HW_GPADC_INPUT_SE_P1_09,             /**< ADC input */
        .temp_sensor      = HW_GPADC_CHARGER_TEMPSENS_GND,       /**< Temperature sensor selection */
        .sample_time      = 1,                                   /**< sample time */
        .continuous       = false,                               /**< continuous mode state */
        .interval         = 0,                                   /**< interval between conversions in continuous mode */
        .input_attenuator = false,                               /**< input attenuator state */
        .chopping         = false,                               /**< chopping state */
        .oversampling     = 1                                    /**< oversampling value */
};



ad_gpadc_controller_conf_t gpadc_configuration = {
        .id = HW_GPADC_1,
        .io = &adc_io_conf,
        .drv = &gpdadc_conf,
};



#endif /* dg_configGPADC_ADAPTER */
#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */
