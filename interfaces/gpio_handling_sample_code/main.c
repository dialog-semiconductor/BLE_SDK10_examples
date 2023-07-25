/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief GPIO pins handling
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
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

/* Required libraries for the target application */
#include "platform_devices.h"
#include "gpio_handling.h"


/* Task priorities */
#define mainGPIO_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )


/*
 * There are two approaches in enabling the COM power domain (PD_COM) while the ARM M33 is running:
 *      - PD_COM is enabled while M33 is active and deactivated before entering sleep.
 *        This approach imposes increased power consumption (static).
 *
 *      - PD_COM is enabled only when it is required and disabled when not needed anymore
 *        (dynamic).
 *
 * Use this macro to select the desired approach:
 *      - A value set to '1' will handle the PD_COM dynamically
 *      - A value set to '0' will handle the PD_COM statically
 *
 **/
#define GPIO_HANDLING_DYNAMICALLY           (0)


/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
static void prvTemplateTask( void *pvParameters );


static OS_TASK xHandle;

static void system_init( void *pvParameters )
{
        OS_TASK task_h = NULL;

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
         * Upon a wakeup cycle, wait for the XTAL32M crystal to settle.
         * BLE, USB and UART blocks require the XTAL32M to be up and
         * running to work properly.
         */
        pm_set_wakeup_mode(true);

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);



        /* Task responsible for handling GPIO pins */
        OS_TASK_CREATE( "GPIO_Handling",                /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvTemplateTask,                /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainGPIO_TASK_PRIORITY,         /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(task_h);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}

/**
 * @brief Template main creates a SysInit task, which creates a GPIO handling task
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


/* Variable used for storing LED1 status */
__RETAINED_RW static bool led_status = 0;


#if (!GPIO_HANDLING_DYNAMICALLY)
/* Callback function triggered following a sleep cycle */
static bool _ad_prepare_for_sleep(void)
{
        /*
         * GPIO pins must be set in latch disabled state before the ARM M33 enters sleep.
         * Otherwise they may have unexpected behavior.
         */
        app_gpio_pins_set_inactive((gpio_config *)output_gpio_cfg);

        return true; // allow M33 to enter sleep

}

/* Callback function triggered following a sleep cancellation cycle */
static void _ad_sleep_canceled(void)
{
        /* GPIOs must be set in latch enabled state in case sleep is cancelled */
        app_gpio_pins_set_active((gpio_config *)output_gpio_cfg, HW_GPIO_POWER_V33);

}
#endif

/**
 * @brief GPIO handling task
 */
static void prvTemplateTask( void *pvParameters )
{

#if (!GPIO_HANDLING_DYNAMICALLY)
        /*
         * Register user-defined callback functions to Power Manager (PM)
         */
        const adapter_call_backs_t ad_pm_call_backs = {

                /* This callback function is fired when a sleep cycle is issued. */
                .ad_prepare_for_sleep = _ad_prepare_for_sleep,

                /* This callback function is fired when a wakeup cycle is issued.  */
                .ad_wake_up_ind       = NULL,

                /*
                 * This callback function is triggered in case the system was about
                 * to enter sleep but the sleep cycle was eventually aborted.
                 */
                .ad_sleep_canceled    = _ad_sleep_canceled,

                /*
                 * This callback function is triggered when the XTAL32M crystal
                 * settles (after a wakeup cycle).
                 */
                .ad_xtalm_ready_ind   = NULL,

                /*
                 * This parameter should be used for declaring extra time
                 * required for the system to stay awake (before entering
                 * sleep).
                 */
                .ad_sleep_preparation_time = 0,
        };


        /*
         * Register callback functions to Power Manager (PM).
         */
        pm_register_adapter(&ad_pm_call_backs);
#endif


        for ( ;; ) {

                /* Block task execution for 1 second (just to allow the system to enter sleep). */
                OS_DELAY_MS(1000);

                /*
                 * Toggle LED1 status and update the corresponding pin configuration
                 * array. Otherwise, the old status will be used at M33 wakeup.
                 */
                led_status ^= 1;
                output_gpio_cfg[0].high = led_status;

#if (GPIO_HANDLING_DYNAMICALLY)
                /*
                 * API used for changing the status of an GPIO OUUPUT pin when
                 * PD_COM is handled dynamically.
                 */
                app_gpio_pin_set(output_gpio_cfg[0], HW_GPIO_POWER_V33);
#else
                hw_gpio_toggle(LED1_PORT, LED1_PIN);
#endif

        }
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{

#if (!GPIO_HANDLING_DYNAMICALLY)
        /* GPIOs must be set in latch enabled state at M33 wakeup. */
        app_gpio_pins_set_active((gpio_config *)output_gpio_cfg, HW_GPIO_POWER_V33);
#endif

}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware( void )
{

        /* Init hardware */
        pm_system_init(periph_init);

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


