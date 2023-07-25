/**
 ****************************************************************************************
 *
 * @file socf_pt_task.c
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

#if defined(SOCF_PERFORMANCE_TEST)
#include <stdio.h>
#include "sdk_defs.h"
#include "osal.h"
#include "sys_timer.h"
#include "socf_config.h"
#include "socfi.h"
#include "socf_hal.h"
#include "socf_client.h"

#define SOCF_PT_NOTIF           (1 << 16)
#define SOCF_PT_TASK_PRIORITY   ( tskIDLE_PRIORITY + 1 )
#define SOCF_PT_REPORT_TIME     1000
static uint64_t socf_start_time;
PRIVILEGED_DATA static OS_TASK xPtTaskHandle;
PRIVILEGED_DATA static TimerHandle_t xPtReport;

#if (dg_configUSE_LP_CLK == LP_CLK_32000)
#define SOCF_REF_LP_FACTOR ((1000 << 20) / 32000)
#elif (dg_configUSE_LP_CLK == LP_CLK_32768)
#define SOCF_REF_LP_FACTOR ((1000 << 20) / 32768)
#elif (dg_configUSE_LP_CLK == LP_CLK_RCX)
#define SOCF_REF_LP_FACTOR ((1000 << 20) / rcx_clock_hz)
#elif (dg_configUSE_LP_CLK == LP_CLK_ANY)
// Must be defined in the custom_config_<>.h file.
#else
# error "SOCF_REF_LP_FACTOR is not defined!"
#endif

#define SOCF_GET_DURATION(now,pre) \
        ((((now) - (pre)) * (int64_t)SOCF_REF_LP_FACTOR) >> 20)

static void socf_report_cb(TimerHandle_t timer)
{
        OS_TASK task = (OS_TASK)pvTimerGetTimerID(timer);
        OS_TASK_NOTIFY(task, SOCF_PT_NOTIF, eSetBits);
}

/**
 * \brief SOCF's Task function.
 *
 * \param [in] pvParameters ignored.
 *
 */
static void socf_pt_task(void *pvParameters)
{
        uint32_t ulNotifiedValue;
        OS_BASE_TYPE xResult __attribute__((unused));

        xPtReport = OS_TIMER_CREATE("SOC log",
                                OS_MS_2_TICKS(SOCF_PT_REPORT_TIME),
                                OS_TIMER_FAIL,
                                (void *) OS_GET_CURRENT_TASK(),
                                socf_report_cb);
        OS_ASSERT(xPtReport != NULL);

        OS_TIMER_START(xPtReport, OS_TIMER_FOREVER);

        do {
                xResult = OS_TASK_NOTIFY_WAIT(0x0, 0xFFFFFFFF, &ulNotifiedValue,
                        portMAX_DELAY);

                if (ulNotifiedValue & SOCF_PT_NOTIF) {
                        int16_t vol;
                        int16_t temp;
                        int16_t soc;
                        int32_t time;

                        time =(int32_t)((SOCF_GET_DURATION(socf_hal_get_time(), socf_start_time)) / 1000);
                        vol = socf_hal_measure_vbat(false);
                        temp = socf_hal_get_degree(false);
                        soc = socfi_get_soc();
#if (SOCF_USE_AGED_CYCLE == 1)
                        printf("[%8ld sec] DLG_SWFG; SOC=%4d; VOL=%4d; TMP=%2d; AGED=%d\r\n", time, soc, vol, temp, socf_get_aged_cycle_number());
#elif (SOCF_USE_AGED_RATE == 1)
                        printf("[%8ld sec] DLG_SWFG; SOC=%4d; VOL=%4d; TMP=%2d; AGED=%3d\r\n", time, soc, vol, temp, socf_get_aged_percent());
#else
                        printf("[%8ld sec] DLG_SWFG; SOC=%4d; VOL=%4d; TMP=%2d\r\n", time, soc, vol, temp);
#endif

                        OS_TIMER_START(xPtReport, OS_TIMER_FOREVER);
                }
        } while (1);
}

void socf_pt_start(void)
{
        OS_BASE_TYPE status;

        socf_start_time = socf_hal_get_time();
        if (xPtTaskHandle == NULL) {
                status = OS_TASK_CREATE("SOC_log",
                                        socf_pt_task,
                                        ( void * ) NULL,
                                        1024,
                                        ( SOCF_PT_TASK_PRIORITY ),
                                        xPtTaskHandle);
                OS_ASSERT(status == OS_OK);

                (void)status;
        } else {
                OS_TIMER_START(xPtReport, OS_TIMER_FOREVER);
        }
}
#endif
