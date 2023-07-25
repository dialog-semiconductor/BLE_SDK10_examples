/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief FreeRTOS template application with retarget
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

#include "ble_common.h"
#include "sys_watchdog.h"
#include "ble_mgr.h"
#include "ble_gap.h"

#ifdef LOGGING_ACTIVE
#include "logging.h"
#endif

/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )
/* Task stack allocation */
#define taskSTACK_ALLOCATION                    ( 200 )
/* The rate at which data is template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                OS_MS_2_TICKS(5000)

/*
 * BLE adv demo advertising data
 */
#if !defined (LOGGING_MODE_QUEUE)
const uint8_t adv_data[] = {
#if defined (LOGGING_MODE_RTT)
        0x0C, GAP_DATA_TYPE_LOCAL_NAME,
        'R','T','T',' ','L', 'o', 'g', 'g', 'i', 'n', 'g'
#else
        0x0D, GAP_DATA_TYPE_LOCAL_NAME,
        'U','A','R','T',' ','L', 'o', 'g', 'g', 'i', 'n', 'g'
#endif
};
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
static void prvTemplateTask_1( void *pvParameters );    // Test task 1
static void prvTemplateTask_2( void *pvParameters );    // Test task 2
static void prvTemplateTask_3( void *pvParameters );    // Test task 3
static void prvTemplateTask_4( void *pvParameters );    // Test task 4
static void prvTemplateTask_5( void *pvParameters );    // Test task 5

#if defined (LOGGING_MODE_QUEUE)
void queued_ble_task( void *pvParameters );             // BLE task for logging queued mode
#else
static void adv_ble_task( void *pvParameters );             // BLE task for logging queued mode
#endif

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

static OS_TASK xHandle;

static void system_init( void *pvParameters )
{
        OS_TASK task_1_h = NULL;
        OS_TASK task_2_h = NULL;
        OS_TASK task_3_h = NULL;
        OS_TASK task_4_h = NULL;
        OS_TASK task_5_h = NULL;
#if defined (LOGGING_MODE_QUEUE)
        OS_TASK task_ble_queue_t = NULL;
#else
        OS_TASK task_ble_adv_t = NULL;
#endif

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        cm_sys_clk_init(sysclk_XTAL32M);

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Initialize platform watchdog */
        sys_watchdog_init();

#if dg_configUSE_WDOG
        /* Register the Idle task first */
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

#if ( LOGGING_ACTIVE == 1 )
        /* Initialize logging mechanism */
        log_init();
#elif defined CONFIG_RETARGET || defined LOGGING_MODE_RETARGET
        retarget_init();
#endif

        pm_set_wakeup_mode(true);
        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);

        /* Start first task here */
        OS_TASK_CREATE( "Task_1",                                       /* The text name assigned to the task, for debug only; not used by the kernel. */
                        prvTemplateTask_1,                              /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        taskSTACK_ALLOCATION * OS_STACK_WORD_SIZE,      /* The number of bytes to allocate to the stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,                     /* The priority assigned to the task. */
                        task_1_h );                                     /* The task handle */
        OS_ASSERT(task_1_h);

        /* Start second task here */
        OS_TASK_CREATE( "Task_2",                                       /* The text name assigned to the task, for debug only; not used by the kernel. */
                        prvTemplateTask_2,                              /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        taskSTACK_ALLOCATION * OS_STACK_WORD_SIZE,      /* The number of bytes to allocate to the stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,                     /* The priority assigned to the task. */
                        task_2_h );                                     /* The task handle */
        OS_ASSERT(task_2_h);

        /* Start third task here */
        OS_TASK_CREATE( "Task_3",                                       /* The text name assigned to the task, for debug only; not used by the kernel. */
                        prvTemplateTask_3,                              /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        taskSTACK_ALLOCATION * OS_STACK_WORD_SIZE,      /* The number of bytes to allocate to the stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,                     /* The priority assigned to the task. */
                        task_3_h );                                     /* The task handle */
        OS_ASSERT(task_3_h);

        /* Start third task here */
        OS_TASK_CREATE( "Task_4",                                       /* The text name assigned to the task, for debug only; not used by the kernel. */
                        prvTemplateTask_4,                              /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        taskSTACK_ALLOCATION * OS_STACK_WORD_SIZE,      /* The number of bytes to allocate to the stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,                     /* The priority assigned to the task. */
                        task_4_h );                                     /* The task handle */
        OS_ASSERT(task_4_h);

        /* Start third task here */
        OS_TASK_CREATE( "Task_5",                                       /* The text name assigned to the task, for debug only; not used by the kernel. */
                        prvTemplateTask_5,                              /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        taskSTACK_ALLOCATION * OS_STACK_WORD_SIZE,      /* The number of bytes to allocate to the stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,                     /* The priority assigned to the task. */
                        task_5_h );                                     /* The task handle */
        OS_ASSERT(task_5_h);

        /* Create a BLE task to push logging data as notifications to a central */

        /* Initialize BLE Manager */
        ble_mgr_init();

#if defined (LOGGING_MODE_QUEUE)

        /* Start the BLE adv demo application task */
        OS_TASK_CREATE("Logging BLE task",                              /* The text name assigned to the task, for debug only; not used by the kernel. */
                        queued_ble_task,                                /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        2048,                                            /* The number of bytes to allocate to the stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,                       /* The priority assigned to the task. */
                        task_ble_queue_t);
        OS_ASSERT(task_ble_queue_t);
#else
        /* Start the BLE adv demo application task */
        OS_TASK_CREATE("Advertising BLE task",                          /* The text name assigned to the task, for debug only; not used by the kernel. */
                        adv_ble_task,                                   /* The function that implements the task. */
                        NULL,                                           /* The parameter passed to the task. */
                        2048,                                           /* The number of bytes to allocate to the stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,                       /* The priority assigned to the task. */
                        task_ble_adv_t);
        OS_ASSERT(task_ble_adv_t);
#endif

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
                        //configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                        200 * OS_STACK_WORD_SIZE,
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

static void dummy_log(logging_severity_e severity, uint8_t id, uint16_t* counter)
{
        (*counter)++;
#if !defined (LOGGING_MODE_QUEUE)
        log_printf(severity, id, "Task %d is running, place some extra characters for fun current counter value %d.\n\r", id, *counter);
#else
        log_printf(severity, id, "Task %d is running, place some extra characters for fun current counter value %d.", id, *counter);
#endif
}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static void prvTemplateTask_1( void *pvParameters )
{
        uint16_t counter = 0;
        for ( ;; ) {
                OS_DELAY(mainCOUNTER_FREQUENCY_MS);
                dummy_log(LOG_DEBUG, 1, &counter);
        }
}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static void prvTemplateTask_2( void *pvParameters )
{
        uint16_t counter = 0;
        for ( ;; ) {
                OS_DELAY(mainCOUNTER_FREQUENCY_MS);
                dummy_log(LOG_NOTICE, 2, &counter);
        }
}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static void prvTemplateTask_3( void *pvParameters )
{
        uint16_t counter = 0;
        for ( ;; ) {
                OS_DELAY(mainCOUNTER_FREQUENCY_MS);
                dummy_log(LOG_WARNING, 3, &counter);
        }
}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static void prvTemplateTask_4( void *pvParameters )
{
        uint16_t counter = 0;
        for ( ;; ) {
                OS_DELAY(mainCOUNTER_FREQUENCY_MS);
                dummy_log(LOG_ERROR, 4, &counter);
        }
}

/**
 * @brief Template task increases a counter every mainCOUNTER_FREQUENCY_MS ms
 */
static void prvTemplateTask_5( void *pvParameters )
{
        uint16_t counter = 0;
        for ( ;; ) {
                OS_DELAY(mainCOUNTER_FREQUENCY_MS);
                dummy_log(LOG_CRITICAL, 5, &counter);
        }
}

#if !defined (LOGGING_MODE_QUEUE)
/**
 * @brief BLE Advertising task
 */
static void adv_ble_task(void *pvParameters)
{
        int8_t wdog_id;

        /* Just remove compiler warnings about the unused parameter */
        ( void ) pvParameters;

        /* Register queue_ble_task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);

        /* Start BLE device as a peripheral */
        ble_peripheral_start();

        /* Set device name */
        ble_gap_device_name_set("Dialog Logging", ATT_PERM_READ);

        /* Set advertising data */
        ble_gap_adv_data_set(sizeof(adv_data), adv_data, 0, NULL);

        ble_gap_adv_start(GAP_CONN_MODE_NON_CONN);

        for( ;; ){
                ble_evt_hdr_t *hdr;

                /* Notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* Suspend watchdog while blocking on ble_get_event() */
                sys_watchdog_suspend(wdog_id);

                /*
                 * Wait for a BLE event - this task will block
                 * indefinitely until something is received.
                 */
                 hdr = ble_get_event(true);

                 /* Resume watchdog */
                 sys_watchdog_notify_and_resume(wdog_id);

                 if (!hdr) {
                         continue;
                 }

                 switch (hdr->evt_code) {
                 default:
                         ble_handle_event_default(hdr);
                         break;
                 }

                 /* Free event buffer (it's not needed anymore) */
                 OS_FREE(hdr);
        }
}
#endif

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
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


