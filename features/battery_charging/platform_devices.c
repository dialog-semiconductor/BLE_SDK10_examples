/**
 ****************************************************************************************
 *
 * @file platform_devices.c
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

#include "ad_gpadc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configGPADC_ADAPTER == 1)

/*
 * Define sources connected to GPADC
 */


const ad_gpadc_driver_conf_t battery_level_driver = {
        .clock                  = HW_GPADC_CLOCK_INTERNAL,
        .input_mode             = HW_GPADC_INPUT_MODE_SINGLE_ENDED,
        .input                  = HW_GPADC_INPUT_SE_VBAT,
        .temp_sensor            = HW_GPADC_NO_TEMP_SENSOR,
        .sample_time            = 15,
        .chopping               = true,
        .oversampling           = HW_GPADC_OVERSAMPLING_4_SAMPLES,
        .input_attenuator       = HW_GPADC_INPUT_VOLTAGE_UP_TO_1V2,
};

const ad_gpadc_controller_conf_t BATTERY_LEVEL = {
        HW_GPADC_1,
        NULL,
        &battery_level_driver
};

#endif /* dg_configGPADC_ADAPTER */
#ifdef __cplusplus
}
#endif
