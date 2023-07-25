/**
 ****************************************************************************************
 *
 * @file rtc_demo_task.c
 *
 * @brief RTC demo task
 *
 * Copyright (C) 2021 Renesas Electronics Corporation and/or its affiliates
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
#include "osal.h"
#include "rtc.h"

/*********************************************************************
 *
 *       Defines/macros
 *
 *********************************************************************
 */

/* Notification bit-masks */
#define NOTIF_RTC_SEC_EVENT     ( 1 << 1 )
#define NOTIF_RTC_ALARM_EVENT   ( 1 << 2 )
#define NOTIF_SET_ALARM_EVENT   ( 1 << 3 )

/* Alarm timer configuration timeout */
#define SET_ALARM_EVENT_TIMEOUT_MS    ( 5000 )

/*
 * '1' to enable printing debugging info on the serial console,
 * '0' otherwise.
 */
#define DBG_SERIAL_CONSOLE      ( 1 )

#define _NEWLINE_    "\n\r"
#define _VERBOSE_(state, format, args...)                      \
                                                               \
        if (state) {                                           \
                printf(_NEWLINE_ #format _NEWLINE_, ##args);   \
        }

/*********************************************************************
 *
 *       Retained symbols
 *
 *********************************************************************
 */

__RETAINED_RW static OS_TASK task_handle = NULL;

/*********************************************************************
 *
 *       Static code
 *
 *********************************************************************
 */

/* Alarm timer callback function */
static void _alarm_timer_cb(OS_TIMER timer)
{
        /* Notify task to further process the event */
        OS_TASK_NOTIFY(task_handle, NOTIF_SET_ALARM_EVENT, OS_NOTIFY_SET_BITS);
}

/* Callback function called following a second RTC event */
static void _rtc_sec_event_cb(void)
{
        (in_interrupt()) ?
                OS_TASK_NOTIFY_FROM_ISR(task_handle, NOTIF_RTC_SEC_EVENT, OS_NOTIFY_SET_BITS) :
                                        OS_TASK_NOTIFY(task_handle, NOTIF_RTC_SEC_EVENT, OS_NOTIFY_SET_BITS);
}

static void _rtc_alarm_event_cb(void)
{
        (in_interrupt()) ?
                OS_TASK_NOTIFY_FROM_ISR(task_handle, NOTIF_RTC_ALARM_EVENT, OS_NOTIFY_SET_BITS) :
                                        OS_TASK_NOTIFY(task_handle, NOTIF_RTC_ALARM_EVENT, OS_NOTIFY_SET_BITS);
}

/*********************************************************************
 *
 *       Non static code
 *
 *********************************************************************
 */

/* RTC demo task */
void prvRTCTask(void *params)
{
        OS_BASE_TYPE ret;
        OS_TIMER alarm_timer_handle;
        uint32_t notif;
        hw_rtc_time_t time_info;
        hw_rtc_calendar_t calendar_info;

        _VERBOSE_(DBG_SERIAL_CONSOLE, ***** RTC Demonstration ***)

        task_handle = OS_GET_CURRENT_TASK();

        ret = _rtc_init();
        ASSERT_WARNING(ret == RTC_STATUS_OK);

        /* Register callback function to be called following RTC second events. */
        _rtc_register_sec_event_cb(_rtc_sec_event_cb);
        /* Register callback function to be called following RTC alarm events. */
        _rtc_register_alarm_event_cb(_rtc_alarm_event_cb);

        /* Create an one-shot OS timer used to set alarm events */
        alarm_timer_handle = OS_TIMER_CREATE("ALARM EVENT", OS_MS_2_TICKS(SET_ALARM_EVENT_TIMEOUT_MS),
                                                                                OS_TIMER_FAIL, NULL, _alarm_timer_cb);
        ASSERT_WARNING(alarm_timer_handle != NULL);
        OS_TIMER_START(alarm_timer_handle, OS_TIMER_FOREVER);

        for (;;) {
                /* Block task as long as events are not present */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                ASSERT_WARNING(ret == OS_TASK_NOTIFY_SUCCESS);

                if (notif & NOTIF_RTC_SEC_EVENT) {

                        /* Get the current RTC contents */
                        _rtc_get_value(&time_info, &calendar_info);

                        _VERBOSE_(DBG_SERIAL_CONSOLE, Weekday:%02d Date:%02d-%02d-%02d Time:%02d:%02d:%02d,
                                                        calendar_info.wday, calendar_info.mday,
                                                        calendar_info.month, calendar_info.year,
                                                        time_info.hour, time_info.minute, time_info.sec);
                }
                if (notif & NOTIF_RTC_ALARM_EVENT) {
                        _VERBOSE_(DBG_SERIAL_CONSOLE, Alarm event hit!)
                }
                if (notif & NOTIF_SET_ALARM_EVENT) {
                        int status;

                        /* Get the current RTC contents */
                        _rtc_get_value(&time_info, &calendar_info);

                        /*
                         * Set alarm event based on the current RTC contents.
                         * Here, an alarm event is set in one minute.
                         */
                        time_info.minute += 1;

                        /* Set the alarm event */
                        status = _rtc_set_alarm(&time_info, NULL);
                        ASSERT_WARNING(status == RTC_STATUS_OK);

                        _VERBOSE_(DBG_SERIAL_CONSOLE, Setting alarm event in one minute...)
                }
        }
}
