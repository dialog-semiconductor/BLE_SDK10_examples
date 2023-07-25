/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (c) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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

#include <stdio.h>
#include <stdbool.h>
#include "ad_gpadc.h"
#include "ad_sdadc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configSDADC_ADAPTER == 1)

/*
 * Define sources connected to SDADC
 */

const ad_sdadc_driver_conf_t sdadc_battery_level_driver = {
        .input_mode             = HW_SDADC_INPUT_MODE_SINGLE_ENDED,
        .inn                    = 0,
        .inp                    = HW_SDADC_INP_VBAT,
        .continuous             = 0,
        .over_sampling          = HW_SDADC_OSR_1024,
        .vref_selection         = HW_SDADC_VREF_INTERNAL,
        .vref_voltage           = 1200,
        .use_dma                = 0,
        .mask_int               = 0,
        .freq                   = HW_SDADC_CLOCK_FREQ_250K,
};

const ad_sdadc_controller_conf_t SDADC_VBAT = {
        HW_SDADC,
        NULL,
        &sdadc_battery_level_driver
};


#endif /* dg_configGPADC_ADAPTER */

#ifdef __cplusplus
}
#endif
