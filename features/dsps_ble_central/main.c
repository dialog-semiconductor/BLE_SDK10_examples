/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief BLE central application
 *
 * Copyright (C) 2015-2023 Renesas Electronics Corporation and/or its affiliates
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
#include <stdbool.h>
#include "osal.h"
#include "resmgmt.h"
#include "ad_ble.h"
#include "ad_nvms.h"
#include "ble_mgr.h"
#include "hw_gpio.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"
#if dg_configUART_ADAPTER
   #include "ad_uart.h"
#endif
#include "platform_devices.h"
#if dg_configUSB_DMA_SUPPORT
   #include "hw_dma.h"
#endif /* dg_configUSB_DMA_SUPPORT */
#if dg_configUSE_SYS_CHARGER
   #include "sys_charger.h"
   #include "custom_charging_profile.h"
#endif /* dg_configUSE_SYS_CHARGER */
#if dg_configUSE_USB_ENUMERATION
   #include "sys_usb.h"
#endif /* dg_configUSE_USB_ENUMERATION */

/* Task priorities */
#define mainBLE_DSPS_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

#if dg_configUSE_WDOG
__RETAINED_RW int8_t idle_task_wdog_id = -1;
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
void dsps_BLE_task(void *params);
void dsps_rx_task(void *params);
void dsps_tx_task(void *params);

/**
 * @brief System Initialization and creation of the BLE task
 */
static void system_init( void *pvParameters )
{
#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        OS_TASK handle;

        /* Prepare clocks. Note: cm_cpu_clk_set() and cm_sys_clk_set() can be called only from a
         * task since they will suspend the task until the XTAL16M has settled and, maybe, the PLL
         * is locked.
         */
        cm_sys_clk_init(CUSTOM_SYS_CLK);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /*
         * Initialize platform watchdog
         */
        sys_watchdog_init();

#if dg_configUSE_WDOG
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* Set the desired sleep mode. */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_extended_sleep);

#if dg_configUSE_USB_ENUMERATION
# if dg_configUSB_DMA_SUPPORT
        sys_usb_driver_conf_t cfg;

        /*
         * Set the desired DMA channels and parameters
         * to be used with USB DATA interface.
         * The USB subsystem will try to use these resources on
         * every plug-in.
         * However, if the DMA resources are not available
         * when the USB plug-in event occurs, then it will
         * automatically fallback to USB operation in interrupt
         * mode to ensure that the USB will be functional.
         */

        cfg.usb.use_dma = true;
        cfg.usb.rx_dma_channel = HW_DMA_CHANNEL_0; /* Select the DMA channel for USB RX.
                                                    * Avoid channel 7 which is the only
                                                    * way to write the keys to Crypto Engine
                                                    */
        cfg.usb.tx_dma_channel = HW_DMA_CHANNEL_1; /* Select the DMA channel for USB RX.
                                                    * Avoid channel 7 which is the only
                                                    * way to write the keys to Crypto Engine
                                                    */
        cfg.usb.rx_dma_prio = HW_DMA_PRIO_4;       /* Set the priority of the RX DMA */
        cfg.usb.tx_dma_prio = HW_DMA_PRIO_5;       /* Set the priority of the TX DMA */

        sys_usb_cfg(&cfg);                         /* Apply the configuration.
                                                    * Call this before the sys_usb_init()
                                                    */
# endif /* dg_configUSB_DMA_SUPPORT */
        sys_usb_init();
#endif /* dg_configUSE_USB_ENUMERATION */

#if defined CONFIG_RETARGET
        retarget_init();
#endif

#if dg_configUSE_SYS_CHARGER
        sys_charger_init(&sys_charger_conf);
#endif /* dg_configUSE_SYS_CHARGER */

        /* Initialize BLE Manager */
        ble_mgr_init();

        /* Start the DSPS BLE application task. */
        OS_TASK_CREATE("DSPS-BLE",                 /* The text name assigned to the task, for
                                                      debug only; not used by the kernel. */
                       dsps_BLE_task,              /* The function that implements the task. */
                       NULL,                       /* The parameter passed to the task. */
                       1024,                       /* The number of bytes to allocate to the
                                                      stack of the task. */
                       mainBLE_DSPS_TASK_PRIORITY, /* The priority assigned to the task. */
                       handle);                    /* The task handle. */
        OS_ASSERT(handle);

        /* Start the DSPS read application task. */
        OS_TASK_CREATE("DSPS-RX",                  /* The text name assigned to the task, for
                                                      debug only; not used by the kernel. */
                       dsps_rx_task,               /* The function that implements the task. */
                       NULL,                       /* The parameter passed to the task. */
                       768,                        /* The number of bytes to allocate to the
                                                      stack of the task. */
                       mainBLE_DSPS_TASK_PRIORITY, /* The priority assigned to the task. */
                       handle);                    /* The task handle. */
        OS_ASSERT(handle);

        /* Start the DSPS write application task. */
        OS_TASK_CREATE("DSPS-TX",                  /* The text name assigned to the task, for
                                                      debug only; not used by the kernel. */
                       dsps_tx_task,               /* The function that implements the task. */
                       NULL,                       /* The parameter passed to the task. */
                       768,                        /* The number of bytes to allocate to the
                                                      stack of the task. */
                       mainBLE_DSPS_TASK_PRIORITY, /* The priority assigned to the task. */
                       handle);                    /* The task handle. */
        OS_ASSERT(handle);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());
}
/*-----------------------------------------------------------*/

/**
 * @brief Basic initialization and creation of the system initialization task.
 */
int main( void )
{
        OS_BASE_TYPE status;
        OS_TASK handle;

        /* Start SysInit task. */
        status = OS_TASK_CREATE("SysInit",                /* The text name assigned to the task, for
                                                             debug only; not used by the kernel. */
                                system_init,              /* The System Initialization task. */
                                ( void * ) 0,             /* The parameter passed to the task. */
                                1200,                     /* The number of bytes to allocate to the
                                                             stack of the task. */
                                OS_TASK_PRIORITY_HIGHEST, /* The priority assigned to the task. */
                                handle );                 /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks     to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );
}

/**
 * \brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
#if dg_configUSE_USB_ENUMERATION
        /* USB data pin configuration */
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_14, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_15, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
#endif /* dg_configUSE_USB_ENUMERATION */
}

static void prvSetupHardware( void )
{
        /* Init hardware */
        pm_system_init(periph_init);

#if dg_configUART_ADAPTER
        ad_uart_io_config(UART_DEVICE.id, UART_DEVICE.io, AD_IO_CONF_OFF);
#endif
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
        taskDISABLE_INTERRUPTS();
        for ( ;; );
}

/**
 * @brief Application idle task hook
 */
void vApplicationIdleHook( void )
{
        /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
           to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
           task. It is essential that code added to this hook function never attempts
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
        taskDISABLE_INTERRUPTS();
        for ( ;; );
}

/**
 * @brief Application tick hook
 */
void vApplicationTickHook( void )
{
}
