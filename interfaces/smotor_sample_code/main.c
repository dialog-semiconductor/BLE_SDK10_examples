/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Step Motor Controller Demonstration
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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

#include "snc.h"
#include "ad_snc.h"
#include "SeNIS.h"

#include "snc_hw_gpio.h"
#include "snc_hw_sys.h"

/* Required libraries for the target application */
#include "hw_rtc.h"
#include "hw_smotor.h"
#include "hw_pdc.h"
#include "hw_pd.h"
#include "hw_sys.h"

#include "motor_sample_code.h"
#include "snc_smotor_demo.h"
#include "peripheral_setup.h"


/* Bitmask Notifications */
#define PGx_SIGx_START_IRQ_NOTIF    (1 << 1)
#define PGx_SIGx_END_IRQ_NOTIF      (1 << 2)


/* Task priorities */
#define mainSMOTOR_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )


/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );
/*
 * Task functions .
 */
static void prvMotorTask( void *pvParameters );

static OS_TASK xHandle;


/* Task Handle */
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

        pm_set_wakeup_mode(true);


        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);



        /* SMOTOR Application */
        OS_TASK_CREATE( "SMOTOR",                       /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvMotorTask,                   /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        1024,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainSMOTOR_TASK_PRIORITY,       /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(task_h);

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}

/**
 * @brief The main creates a SysInit task, which creates the SMOTOR task
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



/*
 * Waveform Generation Start IRQ handler. The returned \p event is always zero.
 *
 * \note The IRQ status is automatically cleared when the callback returns.
 *
 */
static void _smotor_start_cb(uint8_t event)
{
        OS_TASK_NOTIFY_FROM_ISR(task_h, PGx_SIGx_START_IRQ_NOTIF,
                                                OS_NOTIFY_SET_BITS);

}

/*
 * Waveform Generation Stop IRQ handler. The returned \p event is always zero.
 *
 * \note The IRQ status is automatically cleared when the callback returns.
 *
 **/
static void _smotor_stop_cb(uint8_t event)
{
        OS_TASK_NOTIFY_FROM_ISR(task_h, PGx_SIGx_END_IRQ_NOTIF,
                                                OS_NOTIFY_SET_BITS);
}


/*
 * The wave memory space consists of 12 entries 32-bit each. The writing of each entry should be done in 5-bit chunks
 * according to the four corresponding bit fields of each entry. Where XXX denotes a don't care bit field. The rest
 * bit fields can contain either PuN or ToD values.
 *
 * _________________________________________________________________________________________________________________
 * |             |             |             |             |             |             |             |             |
 * |     XXX     |    28:24    |     XXX     |    20:16    |     XXX     |     12:8    |    XXX      |     4:0     | 1st write at 0x50030E40
 * |__bit field__|__bit field__|__bit field__|__bit field__|__bit field__|__bit field__|__bit field__|__bit field__|
 *
 *                                          ....
 * ________________________________________________________________________________________________________________
 * |             |             |             |             |             |             |             |             |
 * |     XXX     |    28:24    |     XXX     |    20:16    |     XXX     |     12:8    |    XXX      |     4:0     | 12th write at 0x50030EC6
 * |__bit field__|__bit field__|__bit field__|__bit field__|__bit field__|__bit field__|__bit field__|__bit field__|                                  ....
 *
 */
#define WAVE_MEMORY_MAX_ENTRIES     12


/*
 *  - The Pulses Number (PuN) field determines the number of ToDs. The following formula should be used for
 *    calculating the number of ToDs required: ((PuN * 2) + 1).
 *  - The Toggle Duration (ToD) field indicates the number of slots before a wave toggles from high-to-low
 *    or/and vice versa. A "slot" is a configurable value and multiple of 30.5us (XTAL32K LP clock period).
 *  - Since the waves are always return-to-zero the last ToD indicates elapsed time from the last toggle to
 *    the end of the wave.
 *
 *
 * \note The 1st element of each wave array must be the PuN value and the rest elements should be the required
 *       ToDs values.
 **/
const uint8_t waveform_0[] = {0x2, 0x10, 0x10, 0x15, 0x15, 0x5}; // Waveform #0 definition

/*
 * | 0x10 slots | 0x10 slots |   0x15 slots  |   0x15 slots  |  0x5 slots |
 *
 *               ____________                 _______________
 *              |            |               |               |
 *              |            |               |               | (return to zero)
 * _____________|            |_______________|               |_________
 *
 **/
const uint8_t waveform_1[] = {0x2, 0x10, 0x10, 0x15, 0x15, 0x5}; // Waveform #1 definition



/* Indexes of waves as stored in to the Wave Table */
__RETAINED_RW static uint8_t wave_idx[WAVE_MEMORY_MAX_ENTRIES] = { 0xFF };


/**
 * @brief SMOTOR task
 */
static void prvMotorTask( void *pvParameters )
{
        uint8_t rd_ptr = 0;

        /* Motor controller initialization */
        _motor_init();


        /* Store the previously defined waveforms in an array  */
        const uint8_t *waves[] = { waveform_0, waveform_1 };

        /*
         * Add waveforms into the Wave Table. In this example two wave definitions are being stored and thus:
         *
         * wave_idx[0] --> contains the index of waveform_0[]
         * wave_idx[1] --> contains the index of waveform_1[]
         *
         * The index designates the exact position of the target waveform as stored into the Wave Table. This parameter
         * should be used when storing commands into the commandFIFO.
         *
         **/
        _motor_fill_wave_table(waves, wave_idx, sizeof(waves)/sizeof(waves[0]));


        /* Motor commands */
        _motor_commandFIFO command_0 = {
               .pg_id  = PG0_IDX,
               .pg_sig = HW_SMOTOR_OUTPUT_SIGNAL_0,
               .n_cmds = 1,
               .w_ptr  = wave_idx[0],
        };
        _motor_commandFIFO command_1 = {
                .pg_id  = PG0_IDX,
                .pg_sig = HW_SMOTOR_OUTPUT_SIGNAL_1,
                .n_cmds = 0,
                .w_ptr  = wave_idx[1],
        };


        /* Store the previously defined commands in an array */
        _motor_commandFIFO *commands[] = { &command_0, &command_1 };

        /*
         * Write command(s) into the Command FIFO. Each command is actually addressing a PG,
         * one of the 4 signals within this PG and specifies whether more commands along with
         * the current should be pushed at the target PG.
         *
         * \note The N_CMDs field specifies only the number of additional commands.
         */
        _motor_push_commandFIFO(commands, sizeof(commands)/sizeof(commands[0]));


        /* Register ISR handlers for the motor IRQs used */
        hw_smotor_register_intr(_smotor_stop_cb, HW_SMOTOR_GENEND_IRQ_HANDLE_ID);
        hw_smotor_register_intr(_smotor_start_cb, HW_SMOTOR_GENSTART_IRQ_HANDLE_ID);



        /* Register a PDC LUT entry and associate it with a uCode-block */
        _smotor_ucode_init(NULL);


        /*
         *  RTC initialization. An RTC event towards SNC will be generated every (100 * 10) ms.
         */
        _rtc_init(100);


        uint32_t notif, rtn;

        printf("\n\r***Motor Controller Demonstration***\n\r");

        printf("\n\rCommandFIFO contents: Index 0: 0x%X, Index 1: 0x%X, Index 2: 0x%X\n\n\r",
                                                               _read_commandFIFO_by_index(0),
                                                               _read_commandFIFO_by_index(1),
                                                               _read_commandFIFO_by_index(2));

        for ( ;; ) {

                // Event / Notification Loop
                rtn = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &notif,
                                                                OS_TASK_NOTIFY_FOREVER);
                OS_ASSERT(rtn == OS_TASK_NOTIFY_SUCCESS);


                if (notif & PGx_SIGx_START_IRQ_NOTIF) {
                        printf("\nGeneration-Start IRQ\n\r");
                }

                if (notif & PGx_SIGx_END_IRQ_NOTIF) {
                        printf("Generation-End IRQ\n\r");


                        /* Print out the index of the next command to be popped out of the commandFIFO  */
                        rd_ptr = hw_smotor_get_read_pointer();
                        printf("Next command to be popped out of commandFIFO: %d\n\r", rd_ptr);

                        /* Depending on the next command to be executed enable/disable the corresponding PG signals */
                        if (rd_ptr == 0) { // command_0 & command_1 are executed simultaneously and require PG0_SIG0 & PG0_SIG1 signals respectively
                                hw_smotor_pg_signals_enable(PG0_IDX, true, true, false, false);
                        } else if (rd_ptr == 2) {
                                hw_smotor_pg_signals_enable(PG0_IDX, true, false, false, false);
                        }

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


