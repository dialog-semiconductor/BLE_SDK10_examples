/**
 ****************************************************************************************
 *
 * @file misc.c
 *
 * @brief Miscellaneous
 *
 * Copyright (C) 2018-2023 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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
