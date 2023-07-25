/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief ADC example
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
#include "sys_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

#include "platform_devices.h"


static void prv_ADC_Read_Task( void *pvParameters );

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

/**
 * @brief system_init() initialize the system (e.g. selects the desired clocks), selects operation parameters (e.g. sleep mode)
 *               and creates the uart_test_task task which which actually uses all three UARTs.
 *               The system_init runs once and exits when finishes its work.
 */
static void system_init( void *pvParameters )
{
        static OS_TASK adc_task_Handle;
        OS_BASE_TYPE status;

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        cm_sys_clk_init(sysclk_XTAL32M);

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

#if dg_configUSE_WDOG
        /* Register the Idle task first */
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif


        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /* true - wait for the XTAL32M to be ready before run the app code on wake-up */
        pm_set_wakeup_mode(true);

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);

        /* Start the ADC task */
        status = OS_TASK_CREATE( "ADCTASK",                             /* The text name assigned to the task, for
                                                                           debug only; not used by the kernel. */
                        prv_ADC_Read_Task,                              /* The System Initialization task. */
                        ( void * ) 0,                                   /* The parameter passed to the task. */
                        2* configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,  /* The number of bytes to allocate to the
                                                                           stack of the task. */
                        OS_TASK_PRIORITY_NORMAL,                        /* The priority assigned to the task.
                                                                           we need this task to run first with
                                                                           no interruption, that is why choose
                                                                           HIGHEST priority*/
                        adc_task_Handle );                                         /* The task handle */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);                    /* Check that the task created OK */

        /* the work of the SysInit task is done
         * The task will be terminated */
        OS_TASK_DELETE( OS_GET_CURRENT_TASK() );
}

/**
 * @brief main() creates a 'system_init' task and starts the FreeRTOS scheduler.
 */
int main( void )
{
        OS_BASE_TYPE status;
        static OS_TASK xsystem_init_Handle;


        /* Start the two tasks as described in the comments at the top of this
        file. */
        status = OS_TASK_CREATE("SysInit",                              /* The text name assigned to the task, for
                                                                           debug only; not used by the kernel. */
                        system_init,                                    /* The System Initialization task. */
                        ( void * ) 0,                                   /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,  /* The number of bytes to allocate to the
                                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,                       /* The priority assigned to the task.
                                                                           we need this task to run first with
                                                                           no interruption, that is why choose
                                                                           HIGHEST priority*/
                        xsystem_init_Handle );                          /* The task handle */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);                    /* Check that the task created OK */

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
 * @brief UART 1 echo task without UART flow control.
 *        The task reads a character on RX and sends it back on TX
 */
static void prv_ADC_Read_Task( void *pvParameters )
{

        OS_TICK_TIME xNextWakeTime;

        ad_gpadc_handle_t adc;

        uint16_t adc_read_value;



        /* Initialise xNextWakeTime - this only needs to be done once. */
        xNextWakeTime = OS_GET_TICK_COUNT();

        for ( ;; ) {
                vTaskDelayUntil( &xNextWakeTime, 1000 );

                adc = ad_gpadc_open(&gpadc_configuration);
                ad_gpadc_read(adc, &adc_read_value);
                ad_gpadc_close(adc,false);

                printf("ADC result: %d\n",adc_read_value);



        }
        OS_TASK_DELETE( OS_GET_CURRENT_TASK() );                                /* Delete the task before exiting. It is not allowed in
                                                                                 * FreeRTOS a task to exit without being deleted from
                                                                                 * the OS's queues */
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
        /* since the UART adapter is used, all the GPIOs configuration for the UARTs
         * is done by the adapters. Also the latch/unlatch is handled by the adapter too.
         * The UART configuration is in the platform_devices.c and the necessary declarations
         * of the three UART configuration instances are in the platform_devices.h
         */
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
           task. It is essential that code added to this hook function never attempts
           to block in any way (for example, call xQueueReceive() with a block time
           specified, or call vTaskDelay()).  If the application makes use of the
           vTaskDelete() API function (as this demo application does) then it is also
           important that vApplicationIdleHook() is permitted to return to its calling
           function, because it is the responsibility of the idle task to clean up
           memory allocated by the kernel to any task that has since been deleted. */

#if (dg_configTRACK_OS_HEAP == 1)
        OS_BASE_TYPE i = 0;
        OS_BASE_TYPE uxMinimumEverFreeHeapSize;

        // Generate raw status information about each task.
        UBaseType_t uxNbOfTaskEntries = uxTaskGetSystemState(pxTaskStatusArray,
                                                        mainMAX_NB_OF_TASKS, &ulTotalRunTime);

        for (i = 0; i < uxNbOfTaskEntries; i++) {
                /* Check Free Stack*/
                OS_BASE_TYPE uxStackHighWaterMark;

                uxStackHighWaterMark = uxTaskGetStackHighWaterMark(pxTaskStatusArray[i].xHandle);
                OS_ASSERT(uxStackHighWaterMark >= mainMIN_STACK_GUARD_SIZE);
        }

        /* Check Minimum Ever Free Heap against defined guard. */
        uxMinimumEverFreeHeapSize = xPortGetMinimumEverFreeHeapSize();
        OS_ASSERT(uxMinimumEverFreeHeapSize >= mainTOTAL_HEAP_SIZE_GUARD);
#endif /* (dg_configTRACK_OS_HEAP == 1) */

#if dg_configUSE_WDOG
        sys_watchdog_notify(idle_task_wdog_id);
#endif
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


