/**
 ****************************************************************************************
 *
 * @file rtc.h
 *
 * @brief RCT block demonstration
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

#ifndef RTC_H_
#define RTC_H_

#include "hw_rtc.h"

/*********************************************************************
 *
 *       Typedefs
 *
 *********************************************************************
 */

/* RTC second event callback function */
typedef void (* _rtc_sec_event_callback)(void);

/* RTC alarm event callback function */
typedef void (* _rtc_alarm_event_callback)(void);

/* RTC status enumeration */
typedef enum {
        RTC_STATUS_FAIL   = -1,
        RTC_STATUS_OK
} RTC_STATUS;

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */

/*
 * '1' to set the 24H display mode,
 * '0' to set the 12H display mode.
 */
#define RTC_DISPLAY_MODE_24H    1

/* Initial time values */
#define RTC_SECOND              0
#define RTC_MINUTE             58
#define RTC_HOUR               14

/* Initial date values */
#define RTC_MONTH               1
#define RTC_MONTH_DAY          20
#define RTC_WEEK_DAY            3
#define RTC_YEAR             2021

/*********************************************************************
 *
 *       API
 *
 *********************************************************************
 */

/*
 * \brief RTC Initialization
 *
 * This routine should be called at the very beginning and before performing
 * any RTC operations.
 *
 * \return The status of the target operation
 *
 * \sa RTC_STATUS_OK
 * \sa RTC_STATUS_FAIL
 */
int _rtc_init(void);

/**
 * \brief Get current date/calendar
 *
 * Wrapper function to get the current RTC counter values (time/date).
 *
 * \param[out]  time_cfg       Pointer to time structure to be filled
 * \param[out]  calendar_cfg   Pointer to calendar structure to be filled
 *
 * \sa hw_rtc_get_time_clndr()
 *
 */
void _rtc_get_value(hw_rtc_time_t * time_cfg, hw_rtc_calendar_t * calendar_cfg);

/*
 * \brief Set alarm event
 *
 * Wrapper function to set alarm event easier. To get notification, and once the alarm hits,
 * a callback routine should first be registered.
 *
 * \param[in] alarm_time_cfg      Pointer to alarm time structure
 * \param[in] alarm_calendar_cfg  Pointer to alarm calendar structure
 *
 * \return Status of the target operation
 *
 * \sa RTC_STATUS_OK
 * \sa RTC_STATUS_FAIL
 *
 * \sa _rtc_register_alarm_event_cb()
 * \sa _rtc_unregister_alarm_event_cb()
 * \sa hw_rtc_set_alarm()
 *
 */
int _rtc_set_alarm(hw_rtc_time_t * alarm_time_cfg, hw_rtc_alarm_calendar_t * alarm_calendar_cfg);

/**
 * \brief Callback function registration for second events
 *
 * The callback routine registered will be called following an RTC event.
 * Currently, every second event.
 *
 * \param[in] cb  Callback function to register.
 *
 * \note The task can get the current RTC counter values by calling
 *       \sa _rtc_get_log()
 *
 */
void _rtc_register_sec_event_cb(_rtc_sec_event_callback cb);

/**
 * \brief Unregister callback function for second events
 *
 * Calling this routine will result in getting no second events notifications.
 *
 */
void _rtc_unregister_sec_event_cb( void );

/**
 * \brief Callback function registration for alarm events
 *
 * The callback routine registered will be called following an alarm event
 * (as defined by the application).
 *
 * \param[in] cb  Callback function to register
 */
void _rtc_register_alarm_event_cb(_rtc_alarm_event_callback cb);

/**
 * \brief Unregister callback function for alarm events
 *
 * Calling this routine will result in getting no alarm events notifications.
 *
 */
void _rtc_unregister_alarm_event_cb(void);

#endif /* RTC_H_ */
