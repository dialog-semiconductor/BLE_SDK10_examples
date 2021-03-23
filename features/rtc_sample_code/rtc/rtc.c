/**
 ****************************************************************************************
 *
 * @file rtc.c
 *
 * @brief RCT block demonstration
 *
 * Copyright (C) 2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "osal.h"
#include "hw_rtc.h"
#include "hw_pdc.h"
#include "rtc.h"

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */

/*
 * Magic value indicating the RTC has been initialized.
 *
 * \note Applicable as long as the device is powered!!!
 */
#define RTC_INIT_MAGIC_VALUE  0xAABBCCDD

/*********************************************************************
 *
 *       Retained symbols
 *
 *********************************************************************
 */
__RETAINED_RW static _rtc_sec_event_callback _rtc_sec_event_cb = NULL;

__RETAINED_RW static _rtc_alarm_event_callback _rtc_alrm_event_cb = NULL;

__RETAINED_UNINIT static uint32_t _rtc_magic_value;


/* RTC PDC registration */
static int _rtc_pdc_registration(void)
{
        /*
         * Add a PDC LUT entry so that the system can wake up following an RTC event.
         * This is essential as Cortex-M33 is completely powered off when the system
         * enters sleep.
         * The Power Domain Controller (PDC) will trigger the system to wake up
         * and thus, handling the RTC event.
         */
        uint32_t pdc_rtc_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                                    HW_PDC_PERIPH_TRIG_ID_RTC_ALARM,
                                                                    HW_PDC_MASTER_CM33, 0));

        if (pdc_rtc_id == HW_PDC_INVALID_LUT_INDEX) {
                ASSERT_WARNING(0);
                return RTC_STATUS_FAIL;
        }
        hw_pdc_set_pending(pdc_rtc_id);
        hw_pdc_acknowledge(pdc_rtc_id);

        return RTC_STATUS_OK;
}

/*
 * RTC interrupt callback function
 *
 * \p events is a bit-mask value. '1' indicates an event occurred since the last time
 * the event status register was read.
 *
 *          -----------------------------------------------------------------------
 *  Bit:    |     6    |     5    |    4    |    3    |   2    |   1    |    0    |
 *          +----------+----------+---------+---------+--------+--------+---------+
 *  Event:  | on alarm | on month | on mday | on hour | on min | on sec | on hsec |
 *          +----------+----------+---------+---------+--------+--------+---------+
 */
static void _rtc_int_cb(uint8_t events)
{
        /* Get the interrupt sources mask */
        uint8_t int_mask = hw_rtc_get_interrupt_mask();

        /* Exercise the interrupt source */
        if ( (events & HW_RTC_EVENT_SEC) && !(int_mask & HW_RTC_INT_SEC) ) { //! Make sure the interrupt mask is set
                /* Notify the task to further process the event */
                if (_rtc_sec_event_cb) {
                        _rtc_sec_event_cb();
                }
        }
        if ( (events & HW_RTC_EVENT_ALRM) && !(int_mask & HW_RTC_INT_ALRM) ) {
                /* Notify the task to further process the alarm event */
                if (_rtc_alrm_event_cb) {
                        _rtc_alrm_event_cb();
                }
        }
}

/* RTC Initialization */
int _rtc_init(void)
{
        int ret;

        hw_rtc_set_hour_clk_mode(RTC_DISPLAY_MODE_24H ? RTC_24H_CLK : RTC_12H_CLK);

        /* Declare default timer values */
        hw_rtc_time_t rtc_time_cfg = {
                .hour   = RTC_HOUR,
                .minute = RTC_MINUTE,
                .sec    = RTC_SECOND,
                .hsec   = 0,
        };

        /* Declare default calendar values */
        hw_rtc_calendar_t rtc_clndr_cfg = {
                .year   = RTC_YEAR,
                .month  = RTC_MONTH,
                .mday   = RTC_MONTH_DAY,

                /*
                 * User is free to map this value as preferred. For instance:
                 * 1 --> Monday
                 * ...
                 * 7 --> Sunday
                 */
                .wday   = RTC_WEEK_DAY
        };

        /*
         * RTC contents are being retained following a HW/SW reset
         *
         * \note The magic value stored in SySRAM is valid as long as the
         *       device is powered.
         *       Alternately, the magic can be stored in the flash memory
         *       used so the RTC is not re-configured after power failure
         *       or the device is turned off.
         */
        if (_rtc_magic_value != RTC_INIT_MAGIC_VALUE) {

                ret = hw_rtc_set_time_clndr(&rtc_time_cfg, &rtc_clndr_cfg);
                if (ret != HW_RTC_VALID_ENTRY) { //! Any error should cause failure
                        ASSERT_WARNING(0);
                        return RTC_STATUS_FAIL;
                }
                _rtc_magic_value = RTC_INIT_MAGIC_VALUE; //! Set magic value to indicate RTC is initialized.
        }

        /* Clear the RTC event status register */
        hw_rtc_get_event_flags();

        /*
         * Enable RTC interrupts. One or more interrupt sources can be unmasked (enabled)
         * The selected interrupt sources should be bitwise-ORed.
         */
        hw_rtc_register_intr(_rtc_int_cb, HW_RTC_INT_SEC);

        hw_rtc_clock_enable(); //! Make sure the RTC clock gating is activated
        hw_rtc_start(); //! Then start both time and calendar timers

        /* Register RTC to PDC so the main core can be notified following RTC events */
        ret = _rtc_pdc_registration();
        if (ret != RTC_STATUS_OK) {
                ASSERT_WARNING(0);
        }
        return ret;
}

/* Register callback function for the second events */
void _rtc_register_sec_event_cb(_rtc_sec_event_callback cb)
{
        if (cb != NULL) {
                _rtc_sec_event_cb = cb;
        }
}

/* Unregister callback function for the second events */
void _rtc_unregister_sec_event_cb(void)
{
        _rtc_sec_event_cb = NULL;
}

/* Register callback function for the alarm events */
void _rtc_register_alarm_event_cb(_rtc_alarm_event_callback cb)
{
        if (cb != NULL) {
                _rtc_alrm_event_cb = cb;
        }
}

/* Unregister callback function for the alarm events */
void _rtc_unregister_alarm_event_cb(void)
{
        _rtc_alrm_event_cb = NULL;
}

/* Get the current time and calendar values */
void _rtc_get_value(hw_rtc_time_t * time_cfg, hw_rtc_calendar_t * calendar_cfg)
{
        /* Get the current time and date in decimal format */
        hw_rtc_get_time_clndr(time_cfg, calendar_cfg);
}

/* Set an alarm event */
int _rtc_set_alarm(hw_rtc_time_t * alarm_time_cfg, hw_rtc_alarm_calendar_t * alarm_calendar_cfg)
{
        int ret;
        uint8_t mask;

        mask = 0xFF;
        if (alarm_time_cfg == NULL) { //! Check if time values are not declared and clear time mask bits
                mask &= (~(HW_RTC_ALARM_HOUR | HW_RTC_ALARM_MIN | HW_RTC_ALARM_SEC | HW_RTC_ALARM_HSEC));
        }
        if (alarm_calendar_cfg == NULL) { //! Check if calendar values are not declared and clear calendar mask bits
                mask &= (~(HW_RTC_ALARM_MONTH | HW_RTC_ALARM_MDAY));
        }

        ret = hw_rtc_set_alarm(alarm_time_cfg, alarm_calendar_cfg, mask);
        if (ret != HW_RTC_VALID_ENTRY) {
                ASSERT_WARNING(0);
                return RTC_STATUS_FAIL;
        }
        return RTC_STATUS_OK;
}
