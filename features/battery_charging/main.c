/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Battery charging example application
 *
 * Copyright (C) 2015-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "hw_watchdog.h"
#include "hw_charger.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "ad_gpadc.h"

#if (dg_configUSE_SYS_USB == 1)
#include "sys_usb.h"
#endif /* (dg_configUSE_SYS_USB == 1) */
#if (dg_configUSE_SYS_CHARGER == 1)
#include "sys_charger.h"
#include "custom_charging_profile.h"
#endif /* dg_configUSE_SYS_CHARGER */

/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/* The rate at which data is template task counter is incremented. */
#define mainCOUNTER_FREQUENCY_MS                OS_MS_2_TICKS(10000)
/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
static void battery_display_task( void *pvParameters );

static OS_TASK xHandle;

extern const ad_gpadc_controller_conf_t BATTERY_LEVEL;

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

#if (dg_configUSE_SYS_CHARGER == 1)
        sys_usb_init();
        sys_charger_init(&sys_charger_conf);
#endif /* dg_configUSE_SYS_CHARGER */

//        pm_set_wakeup_mode(true);
        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);

        /* Start main task here (text menu available via UART1 to control application) */
        OS_TASK_CREATE( "battery_display_task",            /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        battery_display_task,                /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        4096,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
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
 * @brief battery_display_task, display battery voltage every mainCOUNTER_FREQUENCY_MS
 */
static void battery_display_task( void *pvParameters )
{
        OS_TICK_TIME xNextWakeTime;
        uint16_t batt_mv = 0;
        uint16_t value;
        ad_gpadc_handle_t handle;

        static const char *states[] = {
                "HW_CHARGER_MAIN_FSM_STATE_POWER_UP" ,         /**< Power up state */
                "HW_CHARGER_MAIN_FSM_STATE_INIT",                 /**< Initial state */
                "HW_CHARGER_MAIN_FSM_STATE_DISABLED",             /**< Disabled state */
                "HW_CHARGER_MAIN_FSM_STATE_PRE_CHARGE",           /**< Pre-charge state */
                "HW_CHARGER_MAIN_FSM_STATE_CC_CHARGE",            /**< Constant Current state */
                "HW_CHARGER_MAIN_FSM_STATE_CV_CHARGE",            /**< Constant Voltage state */
                "HW_CHARGER_MAIN_FSM_STATE_END_OF_CHARGE",        /**< End of Charge state */
                "HW_CHARGER_MAIN_FSM_STATE_TDIE_PROT",            /**< Die temperature protection state */
                "HW_CHARGER_MAIN_FSM_STATE_TBAT_PROT",            /**< Battery temperature protection state */
                "HW_CHARGER_MAIN_FSM_STATE_BYPASSED",             /**< Bypass state */
                "HW_CHARGER_MAIN_FSM_STATE_ERROR"                 /**< Error state */};

        /* Initialize xNextWakeTime - this only needs to be done once. */
        xNextWakeTime = OS_GET_TICK_COUNT();

        for ( ;; ) {
                // delay
                vTaskDelayUntil( &xNextWakeTime, mainCOUNTER_FREQUENCY_MS );

                // Read battery voltage
                handle = ad_gpadc_open(&BATTERY_LEVEL);
                ad_gpadc_read(handle, &value);
                batt_mv = ad_gpadc_conv_to_batt_mvolt(BATTERY_LEVEL.drv, value);
                ad_gpadc_close(handle, false);

                // Display battery voltage
                printf("[Main task] Battery level %d mV,charger_state  %s\r\n",batt_mv,states[hw_charger_get_main_fsm_state()]);

        }

        OS_TASK_DELETE( xHandle );
}

/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{

#if (dg_configUSE_SYS_CHARGER == 1)
        /* USB data pin configuration */
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_14, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_15, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
#endif /* dg_configUSE_SYS_CHARGER */
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


