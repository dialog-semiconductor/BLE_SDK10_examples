/**
 ****************************************************************************************
 *
 * @file socf_config.h
 *
 * @brief SOC function
 *
 * Copyright (C) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef SOCF_CONFIG_H_
#define SOCF_CONFIG_H_

#define SOCF_USE_SNC            ( 0 )
#define SOCF_USE_OFFSET_COMP    ( 1 )
#define SOCF_USE_AGED_CYCLE     ( 1 )
#define SOCF_USE_AGED_RATE      ( 0 )

/* TIMER time for SOC calculation at active state */
#define SOCF_HAL_TIMER_TIME     1500
/* Sampling interval for SOC calculation (msec) */
#define SOCF_SAMPLING_TIME      1000
/* Current at the point for calculating first SOC from boot (mA)*/
#define SOCF_IBAT_BOOT          -2


/* the number of LUTs at each temperature */
#define SOCF_TEMP_NUM           1

/* When SDADC is used for the voltage of battery */
#define USE_SDADC_FOR_VBAT      ( 0 )

#if (SOCF_USE_SNC == 1) && (USE_SDADC_FOR_VBAT == 1)
#error "SDADC is not supported when SNC is used"
#endif

#if (SOCF_HAL_TIMER_TIME < SOCF_SAMPLING_TIME)
#error "Sampling time must be smaller than the period of HAL timer"
#endif
#endif
