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

/* Required libraries for the target application */
#include "hw_wkup_da1469x.h"
#include "hw_pdc.h"
#include "hw_pd.h"
#include "hw_sys.h"


/*
 * Macro used for selecting the trigger edge (active) of the WKUP controller.
 * Valid values are:
 *
 * 0 --> The WKUP controller is triggered by the falling edge of a pulse
 *       (the pin is connected to a pull-up resistor)
 * 1 --> The WKUP controller is triggered by the rising edge  of a pulse
 *       (the pin is connected to a pull-down resistor)
 *
 **/
#define WKUP_TRIGGER_EDGE           (0)


/*
 * Enable/disable the KEY sub block of the WKUP controller. Valid values are:
 *
 * 1 --> Enable the KEY block as well as debounced IO IRQs
 * 0 --> Disable the KEY block.
 *
 **/
#define WKUP_KEY_BLOCK_ENABLE       (1)

/*
 * Enable/disable the GPIO sub block of the WKUP controller. Valid values are:
 *
 * 1 --> Enable the GPIO block as well as non-debounced IO IRQs
 * 0 --> Disable the GPIO block.
 *
 **/
#define WKUP_GPIO_P0_BLOCK_ENABLE   (1)



#if (!WKUP_KEY_BLOCK_ENABLE && !WKUP_GPIO_P0_BLOCK_ENABLE)
#error "Activate at least one functionality of the WKUP controller!!!"
#endif


/******** Notification Bitmasks ********/
#define WKUP_KEY_PRESS_EVENT_NOTIF     (1 << 2)
#define WKUP_KEY_RELEASE_EVENT_NOTIF   (1 << 3)
#define WKUP_GPIO_P0_EVENT_NOTIF       (1 << 4)


/* Task priorities */
#define mainWKUP_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )


/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

/*
 * Task functions .
 */
static void prvWKUP_Task( void *pvParameters );


static OS_TASK xHandle;

/* PDC LUT entries */
__RETAINED uint32_t pdc_wkup_combo_id;
__RETAINED uint32_t pdc_wkup_gpio_id;


/* Application task handle */
__RETAINED static OS_TASK task_h;


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
         * Upon a wakeup cycle, wait for the XTAL32M crystal to settle.
         * BLE, USB and UART blocks require the XTAL32M to be up and
         * running to work properly.
         */
        pm_set_wakeup_mode(true);

        /*
         * Set the desired sleep mode.
         *
         * \warning When set in hibernation/deep sleep mode the device can reset only by the KEY block
         *          of the WKUP controller. A PDC entry, named HW_PDC_PERIPH_TRIG_ID_COMBO, is created
         *          by the SDK before entering sleep.
         *
         * \warning When set in deep sleep mode, the device can also reset by events produced  by RTC.
         *          A PDC entry, named HW_PDC_PERIPH_TRIG_ID_RTC_TIMER, is created by the SDK before
         *          entering sleep.
         *
         **/
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /*
         * Set the desired wakeup mode.
         *
         * \warning When set is Ultra-Fast wakeup mode, sleep voltage should be 0.9V
         *          and not less than that.
         *
         **/
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);


        /* WakeUp task */
        OS_TASK_CREATE( "WKUP_CONTROLLER",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvWKUP_Task,                   /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        1024,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainWKUP_TASK_PRIORITY,         /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(task_h);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}


/**
 * @brief Template main creates a SysInit task, which creates a WakeUp task
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



#if (WKUP_KEY_BLOCK_ENABLE)
/* WKUP KEY interrupt handler */
static void _wkup_key_cb(void)
{
        uint32_t event = 0;

        /* Clear the WKUP interrupt flag!!! */
        hw_wkup_reset_interrupt();


        /*
         * Check whether the button is pressed or released based on the
         * active trigger edge (WKUP_TRIGGER_EDGE)
         */
        if (!WKUP_TRIGGER_EDGE) {
                event = (hw_wkup_get_pin_trigger(KEY1_PORT, KEY1_PIN) == HW_WKUP_PIN_STATE_LOW) ?
                                        WKUP_KEY_PRESS_EVENT_NOTIF : WKUP_KEY_RELEASE_EVENT_NOTIF;

                /* Change the trigger polarity so that the WKUP controller is triggered on the opposite edge */
                hw_wkup_set_pin_trigger(KEY1_PORT, KEY1_PIN,
                       (event == WKUP_KEY_PRESS_EVENT_NOTIF) ? HW_WKUP_PIN_STATE_HIGH : HW_WKUP_PIN_STATE_LOW);
        } else {
                event = (hw_wkup_get_pin_trigger(KEY1_PORT, KEY1_PIN) == HW_WKUP_PIN_STATE_HIGH) ?
                                        WKUP_KEY_PRESS_EVENT_NOTIF : WKUP_KEY_RELEASE_EVENT_NOTIF;

                /* Change the trigger polarity so that the WKUP controller is triggered on the opposite edge */
                hw_wkup_set_pin_trigger(KEY1_PORT, KEY1_PIN,
                        (event == WKUP_KEY_PRESS_EVENT_NOTIF) ? HW_WKUP_PIN_STATE_LOW : HW_WKUP_PIN_STATE_HIGH);
        }


        /*
         * Avoid using printf() within ISR context!!! It may crash your code.
         * Instead, use notifications to notify a task to perform an action!!
         */

        /* Notify the application task */
        OS_TASK_NOTIFY_FROM_ISR(task_h, event, OS_NOTIFY_SET_BITS);
}
#endif


#if (WKUP_GPIO_P0_BLOCK_ENABLE)
/* WKUP GPIO P0 interrupt handler */
static void _wkup_gpio_p0_cb(void)
{
        uint32_t status;

        /* Clear the WKUP interrupt flag!!! */
        hw_wkup_reset_interrupt();


        /* Get the status of the selected port on last wakeup event. */
        status = hw_wkup_get_status(KEY1_PORT);
        /*
         * This function MUST be called by any GPIO interrupt handler,
         * to clear the interrupt latch status.
         */
        hw_wkup_clear_status(KEY1_PORT, status);


        /*
         * Avoid using printf() within ISR context!!! It may crash your code.
         * Instead, use notifications to notify a task to perform an action!!
         */


        /* Notify the application task */
        OS_TASK_NOTIFY_FROM_ISR(task_h, WKUP_GPIO_P0_EVENT_NOTIF,
                                                        OS_NOTIFY_SET_BITS);
}
#endif


/* Initialize the WKUP controller */
static void _wkup_init(void)
{

        /* Initialize the WKUP controller */
        hw_wkup_init(NULL);


#if (WKUP_KEY_BLOCK_ENABLE)

        /*
         * Set debounce time expressed in ms. Maximum allowable value is 63 ms.
         * A value set to 0 disables the debounce functionality.
         */
        hw_wkup_set_debounce_time(10);

        /*
         * Enable interrupts produced by the KEY block of the wakeup controller (debounce
         * circuitry) and register a callback function to hit following a KEY event.
         */
        hw_wkup_register_key_interrupt(_wkup_key_cb, 1);


        /*
         * Set the polarity (rising/falling edge) that triggers the WKUP controller.
         *
         * \note The polarity is applied both to KEY and GPIO blocks of the controller
         *
         */
        hw_wkup_configure_pin(KEY1_PORT, KEY1_PIN, 1,
                (WKUP_TRIGGER_EDGE ? HW_WKUP_PIN_STATE_HIGH : HW_WKUP_PIN_STATE_LOW));

#endif

#if (WKUP_GPIO_P0_BLOCK_ENABLE)
        /*
         * Enable interrupts produced by the GPIO P0 block of wakeup controller (non-debounce
         * circuitry) and register a callback function to hit following a GPIO event.
         */
        hw_wkup_register_gpio_p0_interrupt(_wkup_gpio_p0_cb, 2);


        /*
         * Set the polarity (rising/falling edge) that triggers the WKUP controller.
         *
         * \note The polarity is applied both to KEY and GPIO blocks of the controller
         *
         */
        hw_wkup_gpio_configure_pin(KEY1_PORT, KEY1_PIN, 1,
                (WKUP_TRIGGER_EDGE ? HW_WKUP_PIN_STATE_HIGH : HW_WKUP_PIN_STATE_LOW));

#endif

        /* Enable interrupts of WKUP controller */
        hw_wkup_enable_irq();
}


/**
 * @brief WakeUp task
 */
static void prvWKUP_Task( void *pvParameters )
{
        uint32_t notif, rtn;

        printf("\nWKUP Controller Demonstration Sample Code\n\n\r");


        /* Initialize the WKUP controller */
        _wkup_init();


#if (WKUP_GPIO_P0_BLOCK_ENABLE)
        /*
         * Add a PDC LUT entry so that to wake up the M33 core following an event on PORT0
         * This is important since M33 is turned off when the device enters sleep.
         */
        pdc_wkup_gpio_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(KEY1_PORT, KEY1_PIN,
                                                                 HW_PDC_MASTER_CM33, 0));
        OS_ASSERT(pdc_wkup_gpio_id != HW_PDC_INVALID_LUT_INDEX);

        /*
         * Trigger the PDC LUT entry by SW. This action guarantees that the PD_SYS
         * power domain stays ON in case the application invalidates all the other
         * activated PDC entries which are keeping the M33 alive.
         */
        hw_pdc_set_pending(pdc_wkup_gpio_id);

        /*
         * Acknowledging the newly created (and activated) PDC entry is recommended
         * although not strictly needed, since M33 will automatically acknowledge
         * all the M33 pending PDC entries before its next attempt to enter sleep.
         */
        hw_pdc_acknowledge(pdc_wkup_gpio_id);
#endif

#if (WKUP_KEY_BLOCK_ENABLE)
        /*
         * The IRQ produced by the KEY sub block of the wakeup controller (debounced IO IRQ)
         * is multiplexed with other trigger sources (VBUS IRQ, SYS2CMAC IRQ, JTAG present)
         * in a single PDC peripheral trigger ID (HW_PDC_PERIPH_TRIG_ID_COMBO).
         */
#if !defined(CONFIG_USE_BLE) && (!dg_configENABLE_DEBUGGER) && (!dg_configUSE_SYS_CHARGER)

        pdc_wkup_combo_id = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                                  HW_PDC_PERIPH_TRIG_ID_COMBO,
                                                                  HW_PDC_MASTER_CM33, 0));
        OS_ASSERT(pdc_wkup_combo_id != HW_PDC_INVALID_LUT_INDEX);

        /* Do the trick! */
        hw_pdc_set_pending(pdc_wkup_combo_id);
        hw_pdc_acknowledge(pdc_wkup_combo_id);
#endif
#endif


        for ( ;; ) {

                /* Event / Notification Loop */
                rtn = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &notif,
                                                                OS_TASK_NOTIFY_FOREVER);
                OS_ASSERT(rtn == OS_TASK_NOTIFY_SUCCESS);


                if (notif & WKUP_KEY_PRESS_EVENT_NOTIF) {
                        printf("KEY1 is pressed\n\r");
                }

                if (notif & WKUP_KEY_RELEASE_EVENT_NOTIF) {
                        printf("KEY1 is released\n\r");
                }

                if (notif & WKUP_GPIO_P0_EVENT_NOTIF) {
                        printf("An event is captured on GPIO P0 \n\r");
                }
                fflush(stdout);

        }
}


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

        /* Enable the COM power domain before handling any GPIO pin */
        hw_sys_pd_com_enable();

        /*
         * On Pro DevKit revision B there is no external pull-up resistor connected
         * to P0.6 pin. Thus, an internal pull-up resistor is enabled.
         */
        hw_gpio_set_pin_function(KEY1_PORT, KEY1_PIN,  HW_GPIO_MODE_INPUT_PULLUP,
                                                                        HW_GPIO_FUNC_GPIO);

        /* In order to make a GPIO active it must be set in latch enabled state */
        hw_gpio_pad_latch_enable (KEY1_PORT, KEY1_PIN);

        /*
         * A GPIO must be set in latch disabled state before M33 entering sleep.
         * Otherwise, the pin will be set to its default state (input pull down).
         */
        hw_gpio_pad_latch_disable(KEY1_PORT, KEY1_PIN);

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


