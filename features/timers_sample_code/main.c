/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief TIMER1 Demonstration
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

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "hw_pd.h"
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

/* Required libraries for the target application */
#include "hw_timer.h"
#include "timer_functionality.h"
#include "hw_sys.h"


/* Task priorities */
#define mainTIMER_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )


/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
static void prvTimersTask( void *pvParameters );


static OS_TASK xHandle;

/* Main application task handle */
extern OS_TASK task_h;


static void system_init( void *pvParameters )
{

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        cm_sys_clk_init(sysclk_XTAL32M);

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();


        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /*
         * Upon a wakeup cycle, wait for the XTAL16M crystal to settle.
         * BLE, USB and UART blocks require the XTAL16M to be up and
         * running to work properly.
         */
        pm_set_wakeup_mode(true);


        /* Set the desired sleep mode.
         *
         * PDC cannot be triggered by the input capture functionality of TIMER1 and
         * thus, activating the ARM M33 core. The only timer related event that can
         * trigger the PDC is overflow events.
         */
        pm_sleep_mode_set(pm_mode_active);


        /* Start main application task here  */
        OS_TASK_CREATE( "TIMERS",                     /* The text name assigned to the task, for
                                                         debug only; not used by the kernel. */
                        prvTimersTask,                /* The function that implements the task. */
                        NULL,                         /* The parameter passed to the task. */
                        1024,
                                                      /* The number of bytes to allocate to the
                                                         stack of the task. */
                        mainTIMER_TASK_PRIORITY,     /* The priority assigned to the task. */
                        task_h );                     /* The task handle */
        OS_ASSERT(task_h);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}

/**
 * @brief Template main creates a SysInit task, which creates a Template task
 */
int main( void )
{
        OS_BASE_TYPE status;

        /* Start the two tasks as described in the comments at the top of this
        file. */
        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        ( void * ) 0,                   /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);



        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );

}



/**
 * @brief TIMER1 application task
 */
static void prvTimersTask( void *pvParameters )
{
        printf("*** TIMER1 Demonstration ***\n\r");

        uint32_t notif, rtn;
        uint32_t timer1_capture_gpio_diff;


        /* Initialize TIMER1 to input capture mode using GPIO1 and GPIO2 event counters. */
        _timer_init(HW_TIMER);


        /* TIMER1 will start running after the execution of this function */
        hw_timer_enable(HW_TIMER);


        for (;;) {

                // Event / Notification Loop
                rtn = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &notif,
                                                                OS_TASK_NOTIFY_FOREVER);
                OS_ASSERT(rtn == OS_TASK_NOTIFY_SUCCESS);


                /* GPIO2 timer capture event has been triggered */
                if (notif & TIMER1_GPIO2_CAPTURE_EVENT_NOTIF) {

                        /*
                         * Compute the time distance,expressed in timer ticks,
                         * between the GPIO1 and GPIO2 timer capture events.
                         */
                        timer1_capture_gpio_diff = _compute_pulse_width(HW_TIMER);

                        /*
                         * Based on the measured time distance, modify the duty cycle
                         * of the PWM pulse produced by TIMER1.
                         */
                        _configure_pwm_duty_cycle(HW_TIMER, timer1_capture_gpio_diff);

                        /*
                         * Print on the serial console the time distance, expressed in timer ticks,
                         * between the GPIO1 and GPIO2 timer capture events.
                         */
                        printf("Pulse Width: %lu (timer ticks)\n\r", timer1_capture_gpio_diff);

                        /* Convert timer ticks to milliseconds */
                        printf("Pulse Width: %lu (ms)\n\r",
                                _convert_timer_ticks_2_us(HW_TIMER, timer1_capture_gpio_diff) / 1000);

                        printf("\n\n\r");
                        fflush(stdout);

                }

        }
}



/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
}

/* Status of pin used for debugging aid */
extern volatile bool _capture_flag;

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware( void )
{

        /* Init hardware */
        pm_system_init(periph_init);

        /* Enable the COM power domain before handling any GPIO pin */
        hw_sys_pd_com_enable();


        /* PWM functionality */
        hw_gpio_set_pin_function(TIMER1_PWM_PORT, TIMER1_PWM_PIN, HW_GPIO_MODE_OUTPUT,
                        HW_GPIO_FUNC_TIM_PWM);
        hw_gpio_pad_latch_enable (TIMER1_PWM_PORT, TIMER1_PWM_PIN);
        hw_gpio_pad_latch_disable(TIMER1_PWM_PORT, TIMER1_PWM_PIN);

        /* Pin used for debugging aid */
        hw_gpio_configure_pin(DBG_CAPTURE_ISR_PORT, DBG_CAPTURE_ISR_PIN, HW_GPIO_MODE_OUTPUT,
                        HW_GPIO_FUNC_GPIO, _capture_flag);
        hw_gpio_pad_latch_enable(DBG_CAPTURE_ISR_PORT, DBG_CAPTURE_ISR_PIN);


        /*
         * Push button K1 is connected to logic high. When pressing it, the P1_1 line
         * is connected to ground.
         */
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, (TIMER1_GPIO1_CAPTURE_EVENT_PIN - 1),
                        HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO);
        hw_gpio_pad_latch_enable (HW_GPIO_PORT_0, (TIMER1_GPIO1_CAPTURE_EVENT_PIN - 1));
        hw_gpio_pad_latch_disable(HW_GPIO_PORT_0, (TIMER1_GPIO1_CAPTURE_EVENT_PIN - 1)); /* Lock the mode of the pin */

        /* Disable the COM power domain after configuring all the GPIO pins */
        hw_sys_pd_com_disable();

}

/**
 * @brief Malloc fail hook
 */
void vApplicationMallocFailedHook( void )
{
        /* vApplicationMallocFailedHook() will only be called if
        configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
        function that will get called if a call to OS_MALLOC() fails.
        OS_MALLOC() is called internally by the kernel whenever a task, queue,
        timer or semaphore is created.  It is also called by various parts of the
        demo application.  If heap_1.c or heap_2.c are used, then the size of the
        heap available to OS_MALLOC() is defined by configTOTAL_HEAP_SIZE in
        FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
        to query the size of free heap space that remains (although it does not
        provide information on how the remaining heap might be fragmented). */
        ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 */
void vApplicationIdleHook( void )
{
        /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
        to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
        task.  It is essential that code added to this hook function never attempts
        to block in any way (for example, call OS_QUEUE_GET() with a block time
        specified, or call OS_DELAY()).  If the application makes use of the
        OS_TASK_DELETE() API function (as this demo application does) then it is also
        important that vApplicationIdleHook() is permitted to return to its calling
        function, because it is the responsibility of the idle task to clean up
        memory allocated by the kernel to any task that has since been deleted. */
}

/**
 * @brief Application stack overflow hook
 */
void vApplicationStackOverflowHook( OS_TASK pxTask, char *pcTaskName )
{
        ( void ) pcTaskName;
        ( void ) pxTask;

        /* Run time stack overflow checking is performed if
        configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
        function is called if a stack overflow is detected. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 */
void vApplicationTickHook( void )
{
}


