/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Sensor Node Controller Demo application tasks
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "platform_devices.h"

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "hw_pd.h"
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "ad_snc.h"
#include "hw_pdc.h"

#include "hw_wkup.h"
#include "hw_rtc.h"
#include "peripheral_setup.h"
#include "custom_macros.h"
#include "snc_spi_demo.h"
#include "ad_spi.h"


#if ( (USER_SNC_TRIGGER_ON_RTC_EVENT) && (USER_SNC_TIMESTAMP_ENABLE) )
/*
 * For the time being this is necessary, due to missing function prototypes in hw_rtc.h file.
 */
__UNUSED static void bdc_to_time(uint32_t time_bcd, hw_rtc_time_t *time)
{
        time->pm_flag = (time_bcd & RTC_RTC_TIME_REG_RTC_TIME_PM_Msk)     >> 30;
        time->hour    = (((time_bcd & RTC_RTC_TIME_REG_RTC_TIME_HR_T_Msk) >> 28) * 10) +
                                    ((time_bcd & RTC_RTC_TIME_REG_RTC_TIME_HR_U_Msk) >> 24);
        time->minute  = (((time_bcd & RTC_RTC_TIME_REG_RTC_TIME_M_T_Msk)  >> 20) * 10) +
                                    ((time_bcd & RTC_RTC_TIME_REG_RTC_TIME_M_U_Msk)  >> 16);
        time->sec     = (((time_bcd & RTC_RTC_TIME_REG_RTC_TIME_S_T_Msk)  >> 12) * 10) +
                                     ((time_bcd & RTC_RTC_TIME_REG_RTC_TIME_S_U_Msk)  >> 8);
        time->hsec    = (((time_bcd & RTC_RTC_TIME_REG_RTC_TIME_H_T_Msk)  >> 4)  * 10) +
                                             (time_bcd & RTC_RTC_TIME_REG_RTC_TIME_H_U_Msk);
}
#endif

/* Task priorities */
#define mainSPI_TASK_PRIORITY           ( OS_TASK_PRIORITY_NORMAL )


/******** Notification Bitmasks *************/
#define READER_TASK_NOTIFICATION       ( 1 << 1 )


/* Main task handler */
__RETAINED static OS_TASK prvSPI_Task_h;

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);


/*
 * Task functions
 */
static void prvSPI_Task(void *pvParameters);

static OS_TASK xHandle;


static void system_init(void *pvParameters)
{
        REG_SETF(GPREG, DEBUG_REG, SYS_CPU_FREEZE_EN, 0);

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

#if defined CONFIG_RETARGET
        retarget_init();
#endif /* CONFIG_RETARGET */

        /*
         * Upon a wakeup cycle, wait for the XTAL32M crystal to settle.
         * BLE, USB and UART blocks require the XTAL32M to be up and
         * running to work properly.
         */
        pm_set_wakeup_mode(true);

        /* Set the desired sleep mode */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode (recommended mode) */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);


        /* Start main SNC demo task here  */
        OS_TASK_CREATE( "SNC_SPI_TEMPLATE",             /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvSPI_Task,                    /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        1024 * OS_STACK_WORD_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainSPI_TASK_PRIORITY,           /* The priority assigned to the task. */
                        prvSPI_Task_h );                 /* The task handle */
        OS_ASSERT(prvSPI_Task_h);

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
                        300 * OS_STACK_WORD_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
         line will never be reached. If the following line does execute, then
         there was insufficient FreeRTOS heap memory available for the idle and/or
         timer tasks to be created. See the memory management section on the
         FreeRTOS web site for more details. */
        for (;;);
}


#include "SeNIS.h"
#include "snc_hw_sys.h"


/* The uCode ID of the registered uCode-Block */
__RETAINED static uint32_t reader_ucode_id;



/**
 * @brief SNC notification callback function.
 *
 * This callback function is triggered when invoking SNC_CM33_NOTIFY() routine (in SNC context)
 * to notify CM33 to start processing data samples acquired by SNC.
 *
 * \warning The SNC scheduler will not proceed with the execution of another uCode,
 *          until callback returns. To avoid any delay, it's recommended to trigger
 *          another task to further process data samples.
 */
static void reader_snc_cb()
{
        /*
         * Notify the main task that time for data processing has elapsed
         */
        OS_TASK_NOTIFY(prvSPI_Task_h, READER_TASK_NOTIFICATION,
                                                        OS_NOTIFY_SET_BITS);
}



#if ( (USER_SNC_TIMESTAMP_ENABLE) && (USER_SNC_TRIGGER_ON_WKUP_EVENT) )
/*
 * @brief TIMER1 initialization function in Input Capture mode
 *
 *        This function initializes TIMER1 in Input Capture mode. That is, the current
 *        timer's value is captured following an input signal. This value will be used
 *        as timestamp for data acquired by SNC.
 *
 **/
static void _timer1_capture_init(void)
{
        timer_config cfg = {
                /*
                 * Select the LP clock as timer clock source.
                 *
                 * \note When the system (CM33) enters sleep, HW timers' clock source is switched
                 *       to the LP clock (e.g. from XTAL32M to XTAL32K). In this case, it's
                 *       difficult to identify when that transition takes place and thus,
                 *       measuring time with accuracy.
                 */
                .clk_src = HW_TIMER_CLK_SRC_INT,
                /*
                 * Actual timer freq. = (\p clk_src) / (\p prescaler + 1)
                 *
                 * \note Max allowable value for \p prescaler is 0x1F (31)
                 **/
                .prescaler = 0,
                .mode = HW_TIMER_MODE_TIMER,
                .timer = {
                        .direction = HW_TIMER_DIR_UP,
                        .free_run = true,
                        .reload_val = 0,
                        /*
                         * Input Capture functionality for TIMER1 & TIMER3 is mapped on PORT0.
                         */
                        .gpio1    = TIMER1_CAPTURE_PIN,
                        .trigger1 = TIMER1_CAPTURE_POL,
                },
        };

        /* Initialize TIMER1 with the previous declared configurations */
        hw_timer_init(HW_TIMER, &cfg);
        hw_timer_register_int(HW_TIMER, NULL);

        /* Disable NVIC interrupt */
        NVIC_DisableIRQ(TIMER_IRQn);

        /* Start running TIMER1 */
        hw_timer_enable(HW_TIMER);
}
#endif



#if (USER_SNC_TRIGGER_ON_RTC_EVENT)
/*
 * \brief RTC initialization function
 *
 *        This function configures the RTC block to produce PDC events
 *        at a fixed interval
 *
 * \param[in] pdcIntervalms  The PDC event interval expressed in milliseconds
 *
 **/
static void _rtc_init(uint32_t period)
{
        __UNUSED HW_RTC_SET_REG_STATUS rtc_error;

        /*
         * Perform sanity checks: the provided PDC event interval should be multiple of 10.
         * This is because the RTC granularity is 10 milliseconds.
         */
        ASSERT_WARNING(period % 10 == 0); // Must be multiple of 10 ms

        hw_rtc_config_pdc_evt_t rtc_cfg = { 0 };


        /* Enable motor events produced by RTC */
        rtc_cfg.pdc_evt_en = true;

        /*
         * Configure the time interval of the motor events A motor event is produced every:
         * (value * 10) ms
         */
        rtc_cfg.pdc_evt_period = period;


        /* Initialize the RTC with the previously defined configurations */
        hw_rtc_config_RTC_to_PDC_evt(&rtc_cfg);

        /* Enable the RTC peripheral clock */
        hw_rtc_clock_enable();


#if (USER_SNC_TIMESTAMP_ENABLE)
        /*
         * Here you can declare your preferred time parameters (these values
         * will be the starting point for the time functionality).
         */
        hw_rtc_time_t rtc_time_cfg = {
                .hour   = 0, // In range of 0 to 23 in 24-hour mode, or 1 to 12 in 12-hour mode
                .minute = 0, // In range of 0 to 59
                .sec    = 0,  // In range of 0 to 59
        };

        /*
         * Set the previous defined time and calendar parameters. This function will call
         * hw_rtc_start() function that starts both the time and calendar functionality.
         *
         * \note Before calling this function make sure that hw_rtc_clock_enable()
         *       function has already been invoked.
         */
        rtc_error = hw_rtc_set_time_clndr(&rtc_time_cfg, NULL);

        OS_ASSERT( (rtc_error & HW_RTC_VALID_ENTRY) == HW_RTC_VALID_ENTRY );
#else
        /* Start running the RTC counters */
        hw_rtc_start();
#endif

}
#endif




/**
 * @brief prvSPI_Task task initializes the Sensor Node to perform SPI transactions
 */
static void prvSPI_Task(void *pvParameters)
{
        uint32_t notif, rtn;
        uint32_t read_bytes, timestamp;


#if ( (USER_SNC_TRIGGER_ON_RTC_EVENT) && (USER_SNC_TIMESTAMP_ENABLE) )
        hw_rtc_time_t rtc_bcd_to_decimal = { 0 };
#endif


        printf("SNC SPI Template Sample Code\n\r");



#if (USER_BDG_ENABLE)
        /*
         * Data popped from SNC-to-CM33 queues should be treated according to
         * the defined element weight parameter (SNC queues). For instance,
         * if the element weight has been set to 16-bit, each element popped
         * from the SNC queue should be treated as 16-bit variable.
         */
        uint32_t read_buff[USER_SNC_TO_CM33_QUEUE_MAX_CHUNK_BYTES/USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE];
#endif



#if (USER_SNC_TRIGGER_ON_WKUP_EVENT)

        /* Enable the WAKEUP controller for handling external events */
        hw_wkup_init(NULL);

        hw_wkup_configure_port(WKUP_TRIGGER_PORT,        /*  Declare a PORT (PORT0 or PORT1) that will trigger
                                                             the WKUP controller */

                              (0 << WKUP_TRIGGER_PIN),   /*  Declare a PIN of the previously declared PORT.

                                                             An CM33 interrupt will be triggered, when one or
                                                             more buttons are pressed.

                                                             \note: This circuit involves a debouncing counter. */

                              (1 << WKUP_TRIGGER_PIN),   /*  Declare a PIN of the previously defined PORT.

                                                             A signal towards PDC will be issued, when one or
                                                             more GPIOs are toggling. The edge of each GPIO
                                                             is captured and latched into a status register.

                                                             \note: This circuit doesn't involve  debouncing
                                                             counter. */

                              (0 << WKUP_TRIGGER_PIN));  /* Declare the polarity of the input signal, that is,
                                                            either the rising or falling edge */

#endif


#if ( (USER_SNC_TIMESTAMP_ENABLE) && (USER_SNC_TRIGGER_ON_WKUP_EVENT) )
        _timer1_capture_init();
#endif

        /*
         * This function initializes SNC to perform SPI operations following PDC events.
         *
         * \param[in] reader_snc_cb  A callback function triggered when SNC notifies CM33.
         *
         **/
        reader_ucode_id = _snc_reader_init(reader_snc_cb);



        /*
           #################### TAG: EXTERNAL_DEVICE_INITIALIZATION #################
           ############# User configurations should be placed here ##################
           ##########################################################################

           Here you should perform all the initializations (if required) of the
           externally connected sensors/modules. This task should take place in
           CM33 context.
        */


#if (USER_SNC_TRIGGER_ON_RTC_EVENT)
        _rtc_init((uint32_t)USER_RTC_TRIGGER_INTERVAL);
#endif



        for (;;) {
                // Event / Notification Loop
                rtn = OS_TASK_NOTIFY_WAIT(0x0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                configASSERT(rtn == OS_TASK_NOTIFY_SUCCESS);


                if (notif & READER_TASK_NOTIFICATION) {
                        /*
                         * Pop data from an SNC-to-CM33 queue. This API returns the number of read data
                         * \p read_bytes expressed in bytes, as well as a timestamp \p timestamp
                         * associated with the popped data.
                         */
                        while (ad_snc_queue_pop(reader_ucode_id, (uint8_t *)read_buff, &read_bytes, &timestamp)) {

                        uint32_t data_samples = read_bytes / USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE;


#if (USER_BDG_ENABLE)
#if (USER_SNC_TRIGGER_ON_RTC_EVENT) && (USER_SNC_TIMESTAMP_ENABLE)
                                /* Convert BCD format to Decimal formal */
                                bdc_to_time(timestamp, &rtc_bcd_to_decimal);

                                printf("\n\rSNC read: %lu samples, Timestamp: %d:%d:%d:%d\n\n\r",
                                                data_samples,
                                                rtc_bcd_to_decimal.hour, rtc_bcd_to_decimal.minute,
                                                rtc_bcd_to_decimal.sec,  rtc_bcd_to_decimal.hsec );

#elif (USER_SNC_TRIGGER_ON_WKUP_EVENT) && (USER_SNC_TIMESTAMP_ENABLE)
                                printf("\n\rSNC read: %lu samples, Timestamp (ms): %lu\n\n\r",
                                        data_samples, (uint32_t)((timestamp * 30.5) / 1000));

#else
                                printf("\n\rSNC read %lu samples:\n\n\r", data_samples);
#endif /* */

                                for (int i = 0; i < data_samples; i++) {
                                        /* Data is handled according to the defined SNC queue element weight */
                                        if (USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE == SNC_QUEUE_ELEMENT_SIZE_BYTE)
                                                printf("Sample[%d]: 0x%02X\n\r",  i, *(((uint8_t *)read_buff ) + i) );
                                        else if   (USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE == SNC_QUEUE_ELEMENT_SIZE_HWORD)
                                                printf("Sample[%d]: 0x%04X\n\r", i, *(((uint16_t *)read_buff) + i) );
                                        else if (USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE == SNC_QUEUE_ELEMENT_SIZE_WORD)
                                                printf("Sample[%d]: 0x%08lX\n\r", i, *(((uint32_t *)read_buff) + i) );

                                } /* end of for() loop */
#endif /* USER_BDG_ENABLE */


                                /*
                                   ###################### TAG: PROCESSING_DATA ##########################
                                   ############# User configurations should be placed here ##############
                                   ######################################################################

                                   At this point, data processing/handling should take place!!!!!
                                */


                        } /* while() */
                  } /* if() */

         } /* for (;;) */
  } /* prvSPI_Task */



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
static void prvSetupHardware(void)
{

        /* Init hardware */
        pm_system_init(periph_init);

        /* When handling GPIOs, the COM power domain should be switched on */
        hw_sys_pd_com_enable();

        /* Configure the SPI pins before opening an I2C device */
        ad_spi_io_config(((ad_spi_controller_conf_t *)SPI_CUSTOM_DEVICE)->id,
                ((ad_spi_controller_conf_t *)SPI_CUSTOM_DEVICE)->io,
                AD_IO_CONF_OFF);

        /* On DevKit version B there is no external pull up resistor connected to K1 push button */
        hw_gpio_configure_pin(WKUP_TRIGGER_PORT, WKUP_TRIGGER_PIN,
                             HW_GPIO_MODE_INPUT_PULLUP, HW_GPIO_FUNC_GPIO, true);

        hw_gpio_pad_latch_enable (WKUP_TRIGGER_PORT, WKUP_TRIGGER_PIN);
        hw_gpio_pad_latch_disable(WKUP_TRIGGER_PORT, WKUP_TRIGGER_PIN);

        /* When finishing with GPIOs handling, the COM power domain should be turned off */
        hw_sys_pd_com_disable();


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
        ASSERT_ERROR(0);
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

