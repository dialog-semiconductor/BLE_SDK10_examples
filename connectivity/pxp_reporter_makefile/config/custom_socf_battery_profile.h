/**
 ****************************************************************************************
 *
 * @file custom_socf_battery_profile.h
 *
 * @brief a battery profile data for State Of Charge
 *
 * Copyright (C) 2016-2022 Renesas Electronics Corporation and/or its affiliates
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

#ifndef CUSTOM_SOCF_BATTERY_PROFILE_H_
#define CUSTOM_SOCF_BATTERY_PROFILE_H_

#define VOL2SOC_LUT_SIZE 11
/* Battery profile data for 190mA battery PH12YT */
#define SOCF_BATT_LOW_CURRENT 1000
#define SOCF_BATT_HIGH_CURRENT 50000
#define SOCF_BATTERY_CAPACITANCE 190
#define SOCF_CHARGING_CURRENT 120
static const int16_t vol_dis_low_0[VOL2SOC_LUT_SIZE] = {
        3691, 3730, 3781, 3828, 3864, 3895, 3939, 3989, 4040, 4097, 4183
};
static const int16_t vol_dis_high_0[VOL2SOC_LUT_SIZE] = {
        3633, 3676, 3730, 3779, 3818, 3850, 3891, 3939, 3988, 4042, 4123
};
static const int16_t vol_chg_0_0 = 3767; //120mA charging

#endif /* CUSTOM_SOCF_BATTERY_PROFILE_H_ */
