/**
 ****************************************************************************************
 *
 * @file app_nvparam.h
 *
 * @brief Configuration of non-volatile parameters for the application
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

#ifndef APP_NVPARAM_H_
#define APP_NVPARAM_H_

#include "platform_nvparam.h"

/**
 * Tags definition for 'ble_app' area.
 */
#define TAG_BLE_APP_NAME                        0x01 // up to 29 bytes value
#define TAG_SOC_BATT_LVOL_GAIN_CAL              0x02
#define TAG_SOC_BATT_HVOL_GAIN_CAL              0x03
#define TAG_SOC_BATT_LTMP_GAIN_CAL              0x04
#define TAG_SOC_BATT_HTMP_GAIN_CAL              0x05
#define TAG_SOC_ACCUMULATED_SOC                 0x06
#define TAG_SOC_AGED_CYCLE                      0x07

/**
 * 'ble_app' area definition
 *
 * Parameters length need to include 2 extra bytes for parameter header.
 * Variable-length parameters length need to include 4 extra bytes for parameter header and length.
 *
 * \note It does not follow 'ble_platform' area directly on flash to allow adding new parameters
 *       to 'ble_platform' without need to move 'ble_app'.
 */

NVPARAM_AREA(ble_app, NVMS_PARAM_PART, 0x0100)
        NVPARAM_VARPARAM(TAG_BLE_APP_NAME,                      0x0000, 33) // uint8[29]
        NVPARAM_VARPARAM(TAG_SOC_BATT_LVOL_GAIN_CAL,            0x0024, 6) // int8[2]
        NVPARAM_VARPARAM(TAG_SOC_BATT_HVOL_GAIN_CAL,            0x002A, 6) // int8[2]
        NVPARAM_VARPARAM(TAG_SOC_BATT_LTMP_GAIN_CAL,            0x0030, 6) // int8[2]
        NVPARAM_VARPARAM(TAG_SOC_BATT_HTMP_GAIN_CAL,            0x0036, 6) // int8[2]
        NVPARAM_VARPARAM(TAG_SOC_ACCUMULATED_SOC,               0x003C, 6) // int8[4]
        NVPARAM_VARPARAM(TAG_SOC_AGED_CYCLE,                    0x0042, 6) // int8[4]
NVPARAM_AREA_END()

#endif /* APP_NVPARAM_H_ */
