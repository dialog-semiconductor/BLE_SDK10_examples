/**
 ****************************************************************************************
 *
 * @file socf_profile_data.h
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

#ifndef SOCF_PROFILE_DATA_H_
#define SOCF_PROFILE_DATA_H_

#include "socf_config.h"
#include "socfi.h"


/* Temperature at discharging state (Degree) */
static const int16_t socf_temp[SOCF_TEMP_NUM] = {25};
/* capacitance (mAh) at 25 C */
static const int16_t socf_cap[SOCF_TEMP_NUM] = {64};
/* charging current (mA) at 25C */
static const int16_t socf_chg_cc[SOCF_TEMP_NUM] = {40};
static const int16_t socf_lluts[SOCF_TEMP_NUM][VOL2SOC_LUT_SIZE] = {
        // 0     5      10   15    20    25    30    35    40    45    50    55   60     65    70    75    80     85   90     95   100
        { 3233, 3545, 3630, 3645, 3670, 3686, 3704, 3722, 3744, 3765, 3786, 3810, 3839, 3876, 3900, 3972, 4025, 4075, 4130, 4185, 4290 },//25
};
/* LUTs at high current at 25(mV)*/
static const int16_t socf_hluts[SOCF_TEMP_NUM][VOL2SOC_LUT_SIZE] = {
        // 0     5      10   15    20    25    30    35    40    45    50    55   60     65    70    75    80     85   90     95   100
        { 3213, 3525, 3610, 3625, 3650, 3666, 3684, 3702, 3724, 3745, 3766, 3790, 3819, 3856, 3880, 3952, 4001, 4052, 4106, 4160, 4290 }, //high discharging at 25
};

/* LUTs for charging at 25C (mV)*/
static const int16_t socf_cluts[SOCF_TEMP_NUM][VOL2SOC_LUT_SIZE] = {
            // 0     5      10   15    20    25    30    35    40    45    50    55   60     65    70    75    80     85   90     95   100
       { 3650 ,3815 ,3878 ,3906 ,3932 ,3954 ,3967 ,3981 ,3993 ,4010 ,4033 ,4061 ,4094 ,4124 ,4169 ,4240 ,4261 ,4315 ,4325 ,4329 ,4333 ,}
};

static const int16_t socf_cur_comp[SOCF_TEMP_NUM][SOCF_M_CAL_NUM] = {
        { 0, 1, 1, 1, 1, 1, }, //25
};

/* SOCF configuration */
static const socf_parameter_t socf_conf = {
        SOCF_TEMP_NUM,
        socf_temp,
        socf_cap,
        socf_lluts,
        socf_hluts,
        socf_cluts,
        /* charging current (mA)*/
        socf_chg_cc,
        /* EOC current (mA)*/
        10,
        /* Charging voltage (mV)*/
        4320,
        /* high current during battery profiling (mA)*/
        -20,
        /* low current during battery profiling (mA)*/
        0,
        /* current compensation values */
        socf_cur_comp,
        /* array number for 25C */
        1,
        /* possible resistance between ADC pin and battery. Normally 0 (mOHM)*/
        0,
        /* cutoff voltage */
        3200,
};
#endif /* SOCF_PROFILE_DATA_H_ */
