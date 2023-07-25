/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief SDADC VBAT reading Demo application
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

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_pd.h"
#include "hw_watchdog.h"
#include "hw_wkup.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_adc.h"

#include "snc_sdadc.h"

/* Task priorities */
#define mainSNC_TASK_PRIORITY           ( OS_TASK_PRIORITY_NORMAL )

/******** Notification Bitmasks *************/
#define SDADC_NOTIFICATION              ( 1 << 1 )  //Avoid (1<<0)/BLE_APP_NOTIFY_MASK, in case merge example with a BLE task


__RETAINED static OS_TASK prvSNCTask_h;

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);
/*
 * Task function
 */
static void GPADC_Demo_Task(void *pvParameters);

static OS_TASK xHandle;


static void system_init(void *pvParameters)
{
#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif /* CONFIG_RETARGET */

        /*
         * Prepare clocks. Note: cm_cpu_clk_set() and cm_sys_clk_set() can be called only from a
         * task since they will suspend the task until the XTAL32M has settled and, maybe, the PLL
         * is locked.
         */
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Prepare the hardware to run this demo */
        prvSetupHardware();

        /* Set the desired sleep mode. */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_extended_sleep);

#if defined CONFIG_RETARGET
        retarget_init();
#endif /* CONFIG_RETARGET */

        /* Start main GPADC demo task here  */
        OS_TASK_CREATE("GPADC_DEMO_APP",        /* The text name assigned to the task, for
                                                debug only; not used by the kernel. */
                GPADC_Demo_Task,                /* The function that implements the task. */
                NULL,                           /* The parameter passed to the task. */
                1024 * OS_STACK_WORD_SIZE,
                                                /* The number of bytes to allocate to the
                                                stack of the task. */
                mainSNC_TASK_PRIORITY,          /* The priority assigned to the task. */
                prvSNCTask_h);                  /* The task handle */
        OS_ASSERT(prvSNCTask_h);
        /* The work of the SysInit task is done */
        OS_TASK_DELETE(xHandle);
}

/**
 * @brief Basic initialization and creation of the system initialization task.
 */
int main(void)
{
        OS_BASE_TYPE status;

        /* Start the two tasks as described in the comments at the top of this
        file. */
        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                        debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        ( void * ) 0,                   /* The parameter passed to the task. */
                        256 * OS_STACK_WORD_SIZE,
                                                        /* The number of bytes to allocate to the
                                                        stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle);                       /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
         line will never be reached.  If the following line does execute, then
         there was insufficient FreeRTOS heap memory available for the idle and/or
         timer tasks     to be created.  See the memory management section on the
         FreeRTOS web site for more details. */
        for (;;);
}

/**
 * @brief GPADC SNC notification callback
 *
 */
static void snc_demo_app_sdadc_cb()
{
        OS_TASK_NOTIFY_FROM_ISR(prvSNCTask_h, SDADC_NOTIFICATION, OS_NOTIFY_SET_BITS);
}

/**
 * @brief GPADC_Demo_Task task initializes GPADC and the Sensor Node. The task registers a uCode block
 * that reads the on-chip radio temperature sensor, checks the temperature drift and notifies the task
 */
static void GPADC_Demo_Task(void *pvParameters)
{
        uint32_t notif, rtn;
        printf("\"SOC For SDADC Reads Test APP\" \n\r\n\r");

        // Power up the Communications Power Domain
        hw_pd_power_up_com();

        snc_sdadc_config(snc_demo_app_sdadc_cb);

        for (;;)
                {
                // Event / Notification Loop
                rtn = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                configASSERT(rtn == OS_TASK_NOTIFY_SUCCESS);
                // If a temperature drift notification has occurred
                if (notif & SDADC_NOTIFICATION) {
                        snc_sdadc_dump_data();
                }
              }
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{

        /* Configure PIN P0_6 for the button */
        HW_GPIO_SET_PIN_FUNCTION(KEY1);
        HW_GPIO_PAD_LATCH_ENABLE(KEY1);
        HW_GPIO_PAD_LATCH_DISABLE(KEY1);

        REG_SETF(SDADC, SDADC_CTRL_REG, SDADC_EN, 1);                   // SDADC on
        while (REG_GETF(SDADC, SDADC_CTRL_REG, SDADC_LDO_OK) == 0);     // wait until LDO is up
        REG_SETF(SDADC, SDADC_CTRL_REG, SDADC_EN, 0);                   // SDADC off
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware(void)
{
        /* Init hardware */
        pm_system_init(periph_init);

}

/**
 * @brief Malloc fail hook
 */
void vApplicationMallocFailedHook(void)
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
        ASSERT_ERROR(0);                ;
}

/**
 * @brief Application idle task hook
 */
void vApplicationIdleHook(void)
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
void vApplicationStackOverflowHook( OS_TASK pxTask, char *pcTaskName)
{
        (void)pcTaskName;
        (void)pxTask;

        /* Run time stack overflow checking is performed if
         configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
        function is called if a stack overflow is detected. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 */
void vApplicationTickHook(void)
{
}
