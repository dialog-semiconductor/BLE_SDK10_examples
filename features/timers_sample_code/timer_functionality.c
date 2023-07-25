/**
 ****************************************************************************************
 *
 * @file timer_functionality.c
 *
 * @brief Definition of APIs used for demonstrating TIMER functionality
 *
 * Copyright (c) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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


/* Header Files */
#include "osal.h"
#include "hw_timer.h"
#include "timer_functionality.h"


typedef enum {
        TIMER_CAPTURE_THRESHOLD_MIN     = 0UL,
        TIMER_CAPTURE_THRESHOLD_0_5_SEC = 500000UL,  // 0.5 second expressed in us
        TIMER_CAPTURE_THRESHOLD_1_0_SEC = 1000000UL, // 1.0 second expressed in us
        TIMER_CAPTURE_THRESHOLD_1_5_SEC = 1500000UL, // 1.5 seconds expressed in us
        TIMER_CAPTURE_THRESHOLD_2_0_SEC = 2000000UL, // 2.0 seconds expressed in us
        TIMER_CAPTURE_THRESHOLD_2_5_SEC = 2500000UL, // 2.5 seconds expressed in us
        TIMER_CAPTURE_THRESHOLD_3_0_SEC = 3000000UL, // 3.0 seconds expressed in us
        TIMER_CAPTURE_THRESHOLD_3_5_SEC = 3500000UL, // 3.5 seconds expressed in us
        TIMER_CAPTURE_THRESHOLD_4_0_SEC = 4000000UL, // 4.0 seconds expressed in us
        TIMER_CAPTURE_THRESHOLD_4_5_SEC = 4500000UL, // 4.5 seconds expressed in us
        TIMER_CAPTURE_THRESHOLD_5_0_SEC = 5000000UL, // 5.0 seconds expressed in us
} TIMER_CAPTURE_THRESHOLD;


#define TIMER1_INT_STATUS_GPIO1_EVENT_Msk   0x01
#define TIMER1_INT_STATUS_GPIO2_EVENT_Msk   0x02
#define TIMER1_INT_STATUS_GPIO3_EVENT_Msk   0x04
#define TIMER1_INT_STATUS_GPIO4_EVENT_Msk   0x08

#define TIMER1_INT_ENABLE_GPIO1_EVENT_Msk   0x01
#define TIMER1_INT_ENABLE_GPIO2_EVENT_Msk   0x02
#define TIMER1_INT_ENABLE_GPIO3_EVENT_Msk   0x04
#define TIMER1_INT_ENABLE_GPIO4_EVENT_Msk   0x08


/*
 * All the HW TIMERs are 24-bit in length and thus, the maximum value they can hold is 2^24
 */
#define TIMER_MAX_COUNTER_VALUE 16777216UL


/*
 * Max value of a 32-bit variable.
 */
#define WORD_MAX_VALUE  4294967296UL


/* Application task handle */
__RETAINED_RW OS_TASK task_h = NULL;


/* Variables used for storing the values of TIMERx capture counters */
__RETAINED_RW static volatile uint32_t _timer_gpio1_capture_val = 0;
__RETAINED_RW static volatile uint32_t _timer_gpio2_capture_val = 0;

/* Counter used for counting the number of TIMERx overflows */
__RETAINED_RW static volatile uint32_t _timer_overflow_cnt = 0;

/* Flag used for enabling/disabling measuring TIMERx overflow events */
__RETAINED_RW static volatile bool _timer_overflow_flag = 0;

/* Flag used for identifying the state of the GPIO pin used for debugging aid */
__RETAINED_RW volatile bool _capture_flag = 0;



/*
 * @brief TIMER1 overflow interrupt handler.
 *
 * This callback is triggered when TIMER1 wraps around. That is, either when 2^24
 * timer ticks has elapsed or when the reload value has been reached (if enabled).
 */
static void _timer_overflow_cb(void)
{

        /* Clear TIMER1 interrupt flag */
        hw_timer_clear_interrupt(HW_TIMER);

        /*
         * Monitor TIMER1 overflow events occurred between GPIO1 and GPIO2
         * input capture events.
         */
        if (_timer_overflow_flag) {
                /* Increment counter value */
                _timer_overflow_cnt++;
        }

}


/*
 * @brief TIMER1 input capture interrupt handler.
 *
 * \note \p gpio_event is a bitmask of GPIO timer capture events. A bit set to '1' means
 *       a capture event occurred on that GPIO
 *
 *       Bit:      |   3   |  2    |  1    |   0   |
 *                 +-------+-------+-------+-------+
 *                 | GPIO4 | GPIO3 | GPIO2 | GPIO1 |
 *                 +-------+-------+-------+-------+
 */
static void  _timer_input_capture_cb(uint8_t gpio_event)
{

        /* Check the source that triggered the callback */
        if (gpio_event & TIMER1_INT_STATUS_GPIO1_EVENT_Msk) {

                /* For debugging aid */
                hw_gpio_toggle(DBG_CAPTURE_ISR_PORT, DBG_CAPTURE_ISR_PIN);
                _capture_flag ^= 1; /* Update the status of the pin */

                /* Start monitoring TIMER1 overflow events */
                _timer_overflow_flag = true;

                /* Get the capture event on GPIO1 (expressed in clock ticks) */
                _timer_gpio1_capture_val = hw_timer_get_capture1(HW_TIMER);

                /* Clear the capture event on GPIO1 */
                hw_timer_clear_gpio_event(TIMER1_INT_STATUS_GPIO1_EVENT_Msk);

        }

        if (gpio_event & TIMER1_INT_STATUS_GPIO2_EVENT_Msk) {

                /* For debugging aid */
                hw_gpio_toggle(DBG_CAPTURE_ISR_PORT, DBG_CAPTURE_ISR_PIN);
                _capture_flag ^= 1; /* Update the status of the pin */

                /* Stop monitoring TIMER1 overflow events */
                _timer_overflow_flag = false;

                /* Get the capture event on GPIO2 (expressed in clock ticks) */
                _timer_gpio2_capture_val = hw_timer_get_capture2(HW_TIMER);

                /* Clear the capture event on GPIO2 */
                hw_timer_clear_gpio_event(TIMER1_INT_STATUS_GPIO2_EVENT_Msk);

                /* Notify the main task to calculate the time distance between the trigger edges */
                OS_TASK_NOTIFY_FROM_ISR(task_h, TIMER1_GPIO2_CAPTURE_EVENT_NOTIF,
                                                                        OS_NOTIFY_SET_BITS);
        }

}


/*
 * @brief Timer initialization function
 */
void _timer_init(HW_TIMER_ID id)
{
        timer_config cfg = {
             /*
              * Select the clock path
              *
              * \note In case the fast clock is selected and the sleep mode is set to extended mode,
              *       the fast clock will be switched to slow clock before the system enters sleep.
              *       (the XTAL32M crystal is turned off during sleep). If this is the case, then it
              *       is suggested you to select the slow clock as it is difficult to determine the
              *       exact time the clock switching takes place.
              */
             .clk_src = HW_TIMER_CLK_SRC_EXT,
             /*
              * Actual timer frequency is Timer Freq =  (\p clk_src / (\p prescaler + 1))
              *
              * In this example the actual timer frequency will be  (32MHz / (31 + 1)) = 1MHz
              *
              * \note Max allowable prescaler value is 0x1F.
              */
             .prescaler = 31,

             .mode = HW_TIMER_MODE_TIMER,

             /* Configure TIMER in capture mode */
             .timer = {
                     .direction  = HW_TIMER_DIR_UP,
                     /*
                      * When counting down, TIMERx's value is automatically reloaded to \p reload_val,
                      * as soon as the counter reaches zero. If enabled, an interrupt is issued.
                      *
                      * When counting up, an IRQ is fired when \p reload_val value is reached. However,
                      * the counter keeps counting and wraps around only when reaching its maximum
                      * value (2^24)
                      */
                     .reload_val = 0,
                     .free_run   = true, /* Timer never stops counting */

                     .gpio1    = TIMER1_GPIO1_CAPTURE_EVENT_PIN,
                     .trigger1 = (TIMER1_CAPTURE_DUTY_CYCLE_ON) ? HW_TIMER_TRIGGER_RISING :
                                             HW_TIMER_TRIGGER_FALLING,
                     .gpio2    = TIMER1_GPIO2_CAPTURE_EVENT_PIN,
                     .trigger2 = (TIMER1_CAPTURE_DUTY_CYCLE_ON) ? HW_TIMER_TRIGGER_FALLING :
                                             HW_TIMER_TRIGGER_RISING,
             },


             /* Configure PWM functionality */
             .pwm = {
                     .port = TIMER1_PWM_PORT,
                     .pin  = TIMER1_PWM_PIN,
                     /*
                      * When set to true, TIMER1 will keep generating PWM pulses on P1_1 pin,
                      * whilst, TIMER2 on P1_6 pin during sleep mode.
                      */
                     .pwm_active_in_sleep = true,
                     /*
                      * PWM Freq = (Timer Freq / (\p frequency + 1))
                      *
                      * \note: Timer Freq is the actual timer frequency after prescaling
                      *
                      * \note If \p prescaler has been set to maximum value (0x1F), then PWM clock divider has
                      *       no further effect and \p frequency defines the PWM period expressed in timer ticks.
                      *
                      * In this example: PWM Period = 65000 timer ticks * 1us = 65000us
                      */
                     .frequency = 64999, /* PWM prescaler */
                     /*
                      * duty cycle ON = (\p duty_cycle / (\p frequency + 1))
                      *
                      * \note If \p prescaler has been set to maximum value (0x1F), then \p duty_cycle defines
                      *       the PWM duty cycle ON expressed in timer ticks.
                      *
                      * In this example: 32500 timer ticks * 1us = 32500us
                      */
                     .duty_cycle = 32500,
             },
        };


        /*
         * Initialize TIMER1. After calling this function both TIMER1 and its interrupt are disabled.
         */
        hw_timer_init(id, &cfg);


        /*
         * Enable TIMER1 capture interrupts to hit following events on GPIO1 and GPIO2 event counters.
         *
         * \note Only HW_TIMER can generate capture event IRQs.
         */
        hw_timer_register_capture_int(_timer_input_capture_cb,
                                    (TIMER1_INT_ENABLE_GPIO1_EVENT_Msk |
                                     TIMER1_INT_ENABLE_GPIO2_EVENT_Msk));


        /* Enable interrupts to be triggered following TIMERx overflow events */
        hw_timer_register_int(id, _timer_overflow_cb);

}


/*
 * \brief Change the PWM duty cycle given a pulse width
 *
 * This function implements 3 different time windows
 *
 * \param [in] id           Timer id
 * \param [in] pulse_wdith  Time expressed in timer ticks
 */
void _configure_pwm_duty_cycle(HW_TIMER_ID id, uint32_t pulse_width)
{
        /* Get timer prescaling value */
        uint32_t pwm_prescaling = hw_timer_get_pwm_freq(id);
        uint32_t duty_cycle_on = 0;

        /*
         * Define a time window used for altering the PWM duty cycle:
         * short press < 0.5 sec
         * medium press >= 0.5 sec < 3 sec
         * long press >= 3 sec
         *
         **/
        if ((pulse_width >= TIMER_CAPTURE_THRESHOLD_MIN) && (pulse_width < TIMER_CAPTURE_THRESHOLD_0_5_SEC))  {
                /* Compute the new PWM duty cycle */
                duty_cycle_on = ((pwm_prescaling + 1) >> 5); // Divided by 32 (3.125% duty cycle)

                /* Change PWM duty cycle */
                hw_timer_set_pwm_duty_cycle(id, duty_cycle_on);

        } else if ((pulse_width >= TIMER_CAPTURE_THRESHOLD_0_5_SEC) && (pulse_width < TIMER_CAPTURE_THRESHOLD_3_0_SEC)) {
                /* Compute the new PWM duty cycle */
                duty_cycle_on = ((pwm_prescaling + 1) >> 2); // Divided by 4 (25% duty cycle)

                /* Change PWM duty cycle */
                hw_timer_set_pwm_duty_cycle(id, duty_cycle_on);

        } else if (pulse_width >= TIMER_CAPTURE_THRESHOLD_3_0_SEC) {
                /* Compute the new PWM duty cycle */
                duty_cycle_on = (pwm_prescaling + 1); // (100% duty cycle)

                /* Change PWM duty cycle */
                hw_timer_set_pwm_duty_cycle(id, duty_cycle_on);
        }
}


/*
 * @brief Compute the time distance between two successive timer capture events (e.g. GPIO1 and GPIO2)
 *
 * \param [in] id Timer id
 *
 * \return The time distance between the two timer capture events, expressed in timer ticks.
 */
uint32_t _compute_pulse_width(HW_TIMER_ID id)
{
        uint32_t capture_event_diff = 0;

        /* Get the reload value of HW TIMER */
        uint32_t on_reload = hw_timer_get_reload(id);

        /* Get the direction of HW TIMER1 */
        uint32_t direction = HW_TIMER_REG_GETF(TIMER, TIMER_CTRL_REG, TIM_COUNT_DOWN_EN);

        if ( (direction == HW_TIMER_DIR_UP) && on_reload ) OS_ASSERT(0); // not supported

        /* Counter is running upwards */
        if (direction == HW_TIMER_DIR_UP) {
                /*
                 * Mathematical formula: (TIMER_MAX_VALUE * R) + (GPIO2_EVENT - GPIO1_EVENT),
                 * where R = number of TIMERx overflow events.
                 */
                if (_timer_gpio2_capture_val > _timer_gpio1_capture_val) {
                        /* Check if there is any overflow (to avoid the multiplication) */
                        if (_timer_overflow_cnt) {
                                capture_event_diff = ( (TIMER_MAX_COUNTER_VALUE * _timer_overflow_cnt) +
                                                        (_timer_gpio2_capture_val - _timer_gpio1_capture_val) );
                        } else {
                                capture_event_diff = (_timer_gpio2_capture_val - _timer_gpio1_capture_val);
                        }
                /*
                 * Mathematical formula: ( (TIMER_MAX_VALUE * (R-1)) + (TIMER_MAX_VALUE - GPIO1_EVENT) + GPIO2_EVENT),
                 * where R = number of timer overflow events.
                 */
                } else if (_timer_gpio2_capture_val < _timer_gpio1_capture_val) {
                        /* Just to avoid the multiplication */
                        if (_timer_overflow_cnt == 1) {
                                capture_event_diff = ( (TIMER_MAX_COUNTER_VALUE - _timer_gpio1_capture_val) +
                                                                                        _timer_gpio2_capture_val );
                        } else {
                                capture_event_diff = ( (TIMER_MAX_COUNTER_VALUE * (--_timer_overflow_cnt)) +
                                        (TIMER_MAX_COUNTER_VALUE - _timer_gpio1_capture_val) + _timer_gpio2_capture_val );
                        }
                }
        /* Counter is running downwards */
        } else if (direction == HW_TIMER_DIR_DOWN) {
                /*
                 * Mathematical formula: (ON_RELOAD * R) + (GPIO1_EVENT - GPIO2_EVENT),
                 * where R = number of timer overflow events.
                 */
               if (_timer_gpio1_capture_val > _timer_gpio2_capture_val) {
                       /* Check if there is any overflow (to avoid the multiplication) */
                       if (_timer_overflow_cnt) {
                               capture_event_diff = ( (on_reload * _timer_overflow_cnt) +
                                                       (_timer_gpio1_capture_val - _timer_gpio2_capture_val) );
                       } else {
                               capture_event_diff = (_timer_gpio1_capture_val - _timer_gpio2_capture_val);
                       }
              /*
               * Mathematical formula: ( (ON_RELOAD * (R-1)) + (ON_RELOAD - GPIO2_EVENT) + GPIO1_EVENT),
               * where R = number of timer overflow events.
               */
               } else if (_timer_gpio1_capture_val < _timer_gpio2_capture_val) {
                       /* Just to avoid the multiplication */
                       if (_timer_overflow_cnt == 1) {
                               capture_event_diff = ( (on_reload - _timer_gpio2_capture_val) +
                                                                       _timer_gpio1_capture_val );
                       } else {
                               capture_event_diff = ( (on_reload * (--_timer_overflow_cnt)) +
                                       (on_reload - _timer_gpio2_capture_val) +  _timer_gpio1_capture_val);
                       }
               }
        /* Invalid value */
        } else {
                OS_ASSERT(0);
        }


        /* Sanity check */
        OS_ASSERT(capture_event_diff < WORD_MAX_VALUE);

        /* Reset timer overflow counter */
        _timer_overflow_cnt = 0;


        return capture_event_diff;
}


/*
 * @brief Compute the timer period, expressed in microseconds.
 *
 * \note The returned value is multiplied by 1024 to increase accuracy.
 */
static uint64_t _compute_timer_period_us(HW_TIMER_ID id)
{
        uint32_t timer_prescaler;
        HW_TIMER_CLK_SRC timer_clock_src;

        uint64_t timer_freq;
        uint64_t timer_period_us;

        /* Get timer clock prescaler  */
        timer_prescaler = hw_timer_get_prescaler(id);

        /* Get timer clock source */
        timer_clock_src = hw_timer_get_clk(id);

        /* If the LP clock (XTAL32K) is the selected timer clock source */
        if (timer_clock_src == HW_TIMER_CLK_SRC_INT) {
                /* Check the LP clock used */
                if ((dg_configUSE_LP_CLK == LP_CLK_32768) || (dg_configUSE_LP_CLK == LP_CLK_32000)) {
                        timer_freq = ( (uint64_t)(configSYSTICK_CLOCK_HZ / (timer_prescaler + 1)) << 10 ); // Multiplied by 1024 to increase accuracy
                } else if (dg_configUSE_LP_CLK == LP_CLK_RCX) {
                        /* Not supported */
                        OS_ASSERT(0);
                }
        /* If the DIVN (XTAL32M) is the selected timer clock source */
        } else if (timer_clock_src == HW_TIMER_CLK_SRC_EXT) {
                timer_freq = ( (uint64_t)(32000000UL / (timer_prescaler + 1)) << 10 ); // Multiplied by 1024 to increase accuracy

        }

        /* Timer period expressed in microseconds */
        timer_period_us = ( ((uint64_t)1000000UL << 20) / timer_freq ); // Multiplied by 1024 to increase accuracy

        return timer_period_us;
}


/* Convert microseconds to timer ticks */
uint32_t _convert_us_2_timer_ticks(HW_TIMER_ID id, uint32_t time_in_us)
{
        /* Compute timer period. The result is multiplied by 1024 */
        uint64_t timer_period_us = _compute_timer_period_us(id);

        /* Convert \p time_is_us to timer ticks. */
        uint32_t timer_ticks = (uint32_t)( ((uint64_t)time_in_us << 10) / timer_period_us );

        return timer_ticks;
}


/* Convert timer ticks to microseconds */
uint32_t _convert_timer_ticks_2_us(HW_TIMER_ID id, uint32_t timer_ticks)
{
        /* Compute timer period. The result is multiplied by 1024 */
        uint64_t timer_period_us = _compute_timer_period_us(id);

        /* Convert \p timer_ticks to time expressed in microseconds */
        uint32_t time_us = (uint32_t)( ((uint64_t)timer_ticks * timer_period_us) >> 10 );

        return time_us;
}
