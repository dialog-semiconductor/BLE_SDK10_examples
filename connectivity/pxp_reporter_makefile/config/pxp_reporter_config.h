/**
 ****************************************************************************************
 *
 * @file pxp_reporter_config.h
 *
 * @brief Proximity Reporter configuration
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

#ifndef PXP_REPORTER_CONFIG_H_
#define PXP_REPORTER_CONFIG_H_

/* Name can not be longer than 29 bytes (BLE_SCAN_RSP_LEN_MAX - 2 bytes)*/
#define PX_REPORTER_DEFAULT_NAME        "Dialog PX Reporter"

/* Interval (in ms) for checking battery level (can be configured externally as well) */
#ifndef PX_REPORTER_BATTERY_CHECK_INTERVAL
#define PX_REPORTER_BATTERY_CHECK_INTERVAL      60000
#endif

/*
 * When set to non-zero, SUOTA build will request longer connection interval in order to reduce
 * power consumption when connection is active and change back to short connection interval before
 * starting SUOTA process. This however seem to cause interoperability problems with some devices.
 *
 * This parameter is not applicable for non-SUOTA builds since they will always use longer connection
 * interval when possible.
 */
#define PX_REPORTER_SUOTA_POWER_SAVING  (0)

/* Include Battery Service */
#define PX_REPORTER_INCLUDE_BAS                 ( 1 )

/* LED blinking periods in ms (applicable only in DA1469x) */
#define SLOW_BLINKING    (750)
#define FAST_BLINKING    (250)


#endif /* PXP_REPORTER_CONFIG_H_ */
