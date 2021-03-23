/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
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
