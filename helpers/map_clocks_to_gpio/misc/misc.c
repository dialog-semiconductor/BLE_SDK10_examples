/**
 ****************************************************************************************
 *
 * @file misc.c
 *
 * @brief Miscellaneous
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
#include "misc.h"

#define GET_TIMESTAMP_MULTIPLIER          1000000UL
#define GET_TIMESTAMP_DIVIDER             configSYSTICK_CLOCK_HZ

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
