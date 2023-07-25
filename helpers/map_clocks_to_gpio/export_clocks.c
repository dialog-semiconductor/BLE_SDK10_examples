/**
 ****************************************************************************************
 *
 * @file export_clocks.c
 *
 * @brief Exporting clocks
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hw_gpio.h"
#include "sys_power_mgr.h"
#include "cli.h"
#include "export_clocks.h"
#include "misc.h"

/* Bit mask notifications */
#define CLI_NOTIF   (1 << 0)

#define EXPORT_CLOCKS_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

/*****************************************************************************************
 * Main application task.
 */
static void prvExportClocksTask( void *pvParameters );

/*****************************************************************************************
 * Pins that may affect the XTAL32K and/or XTAL32M and/or RF performance.
 */
static const uint8_t port0_buf_warning[] = { 6, 7, 16, 18, 27 };
static const uint8_t port1_buf_warning[] = { 0, 6, 9, 12, 18, 19 };

/*****************************************************************************************
 * User's clock configuration settings.
 */
__RETAINED_RW static struct {
        HW_GPIO_PORT port;      ///< Selected Port
        HW_GPIO_PIN pin;        ///< Selected Pin
        CLOCK_SOURCE src;       ///< Selected Clock Source
        CLOCK_MODE mode;        ///< Selected Mode
        bool status;            ///< Enabling selected Clock
} clock_cfg = {
        .port = CLOCK_PORT_DEFAULT,
        .pin = CLOCK_PIN_DEFAULT,
        .src = CLOCK_SOURCE_DEFAULT,
        .mode = CLOCK_MODE_DEFAULT,
        .status = CLOCK_STATUS_DEFAULT
};

__RETAINED static OS_TASK task_h;
__RETAINED_RW static HW_GPIO_PORT old_port = CLOCK_PORT_DEFAULT;
__RETAINED_RW static HW_GPIO_PIN old_pin = CLOCK_PIN_DEFAULT;

/* Invalidate current FUNC_CLOCK (PID) */
__STATIC_FORCEINLINE void port_invalidate_clock_func(void)
{
        hw_gpio_set_pin_function(clock_cfg.port, clock_cfg.pin, HW_GPIO_MODE_INPUT,
                                                                          HW_GPIO_FUNC_GPIO);
}

static PORT_STATUS port_status_read(HW_GPIO_PORT port, HW_GPIO_PIN pin)
{
        if (port == HW_GPIO_PORT_0) {
                for (int i = 0; i < sizeof(port0_buf_warning); i++) {
                        if (pin == port0_buf_warning[i]) {
                                DBG_LOG("\n\rSelected port requires your attention as "
                                                "it might affect crystal and/or RF performance.\n\r");

                                return PORT_STATUS_WARNING;
                        }
                }
        }
        else if (port == HW_GPIO_PORT_1) {
                for (int i = 0; i < sizeof(port1_buf_warning); i++) {
                        if (pin == port1_buf_warning[i]) {
                                DBG_LOG("\n\rSelected port requires your attention as "
                                                        "it might affect crystal and/or RF performance\n\r");

                                return PORT_STATUS_WARNING;
                        }
                }
        }

        return PORT_STATUS_AVAILABLE;
}

static void export_clock(void)
{
        /* Make sure PD_COM is up and running */
        hw_sys_pd_com_enable();
        port_invalidate_clock_func();

        if (clock_cfg.status) {
                uint32_t reg_val = 0;

               if (clock_cfg.mode == CLOCK_MODE_MANUAL) {
                       REG_SET_FIELD(GPIO, GPIO_CLK_SEL_REG, FUNC_CLOCK_SEL, reg_val, clock_cfg.src);
                       REG_SET_FIELD(GPIO, GPIO_CLK_SEL_REG, FUNC_CLOCK_EN, reg_val, 1);
               }
               else if (clock_cfg.mode == CLOCK_MODE_AUTO) {

                       /* Overwrite port settings as clock is exposed on dedicated ports */
                       clock_cfg.port = HW_GPIO_PORT_0;

                       switch (clock_cfg.src) {
                       case CLOCK_SOURCE_XTAL32K:
                               clock_cfg.pin = HW_GPIO_PIN_14;
                               REG_SET_FIELD(GPIO, GPIO_CLK_SEL_REG, XTAL32K_OUTPUT_EN, reg_val, 1);
                               break;
                       case CLOCK_SOURCE_RC32K:
                               clock_cfg.pin = HW_GPIO_PIN_17;
                               REG_SET_FIELD(GPIO, GPIO_CLK_SEL_REG, RC32K_OUTPUT_EN, reg_val, 1);
                               break;
                       case CLOCK_SOURCE_RCX:
                               clock_cfg.pin = HW_GPIO_PIN_16;
                               REG_SET_FIELD(GPIO, GPIO_CLK_SEL_REG, RCX_OUTPUT_EN, reg_val, 1);
                               break;
                       case CLOCK_SOURCE_XTAL32M:
                               clock_cfg.pin = HW_GPIO_PIN_12;
                               REG_SET_FIELD(GPIO, GPIO_CLK_SEL_REG, XTAL32M_OUTPUT_EN, reg_val, 1);
                               break;
                       case CLOCK_SOURCE_RC32M:
                               clock_cfg.pin = HW_GPIO_PIN_13;
                               REG_SET_FIELD(GPIO, GPIO_CLK_SEL_REG, RC32M_OUTPUT_EN, reg_val, 1);
                               break;
                       case CLOCK_SOURCE_DIVN:
                               clock_cfg.pin = HW_GPIO_PIN_15;
                               REG_SET_FIELD(GPIO, GPIO_CLK_SEL_REG, DIVN_OUTPUT_EN, reg_val, 1);
                               break;
                       default:
                               ASSERT_WARNING(0);
                               return;
                       }
                       /* Port might have been overwritten */
                       port_status_read(clock_cfg.port, clock_cfg.pin);
               }
               /* Apply new clock settings */
               GPIO->GPIO_CLK_SEL_REG = reg_val;

               hw_gpio_set_pin_function(clock_cfg.port, clock_cfg.pin, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_CLOCK);
               hw_gpio_pad_latch_enable(clock_cfg.port, clock_cfg.pin);
        } else {
                /*
                 * USBPAD_REG belongs to PD_SYS and so its value should be reset
                 * upon system sleep. No need to update it now.
                 */

                GPIO->GPIO_CLK_SEL_REG = 0;
        }

        hw_sys_pd_com_disable();
}

/* Command handler */
static void clock_port_handler(int argc, const char *argv[], void *user_data)
{
        HW_GPIO_PORT port;
        HW_GPIO_PIN pin;

        DBG_LOG("\n\rEntering %s\n\r", __func__);

        if (argc != 3) {
                DBG_LOG("\n\rMissing arguments: 'clock_port <port_number> <pin_number>'\n\r");
                return;
        }

        /* Convert strings to integers */
        port = atoi(argv[1]);
        pin  = atoi(argv[2]);

        /* Perform sanity checks and make sure that a valid port is declared */
        if (port >= HW_GPIO_NUM_PORTS || pin >= hw_gpio_port_num_pins[port]) {
                DBG_LOG("\n\rInvalid clock port selection\n\r");
                return;
        }

        /* Update only if a new port is selected */
        if (port != clock_cfg.port || pin != clock_cfg.pin) {

                /* Check if USB is enabled to avoid conflicts when USB data lines are used. */
                if ((pin == HW_GPIO_PIN_14 || pin == HW_GPIO_PIN_15) && port == HW_GPIO_PORT_0) {
                        if (!dg_configUSE_USB_ENUMERATION) {
                                /* Forcefully activate the USB pads */
                                REG_SETF(GPREG, USBPAD_REG, USBPAD_EN, 1);
                        }
                        else {
                               DBG_LOG("\n\rUSB functionality is enabled. Please select another I/O pin or disable USB.\n\r");
                               return;
                        }
                }

                port_status_read(port, pin);
                /* Explicitly reset the previously selected port */
                port_invalidate_clock_func();

                /* Update clock settings */
                clock_cfg.port = port;
                clock_cfg.pin = pin;

                /* Overwrite clock mode */
                if (clock_cfg.mode == CLOCK_MODE_AUTO) {
                        clock_cfg.mode = CLOCK_MODE_MANUAL;
                }

                /* Make sure the new settings are applied immediately */
                export_clock();
        }

        DBG_LOG("\n\rPORT%d.%d\n\r", port, pin);
}

/* Default CLI handler */
static void cli_default_handler(int argc, const char *argv[], void *user_data)
{
        DBG_LOG("\n\rEntering %s (invalid command)\n\r", __func__);
}

/* Command handler */
static void clock_source_handler(int argc, const char *argv[], void *user_data)
{
        CLOCK_SOURCE source;

        DBG_LOG("\n\rEntering %s\n\r", __func__);

        if (argc != 2) {
                DBG_LOG("\n\rMissing arguments: 'clock_source <arg1>', where <arg1> is 0 --> XTAL32K, "
                                           "1 --> RC32K, 2 --> RCX, 3 --> XTAL32M, 4 --> RC32M, 5 --> DIVN\n\r");
                return;
        }

        /* Convert string to integer */
        source = atoi(argv[1]);

        if (source < CLOCK_SOURCE_INVALID && source != clock_cfg.src) {

                switch (source) {
                case CLOCK_SOURCE_XTAL32K:
                        DBG_LOG("\n\rClock source is set to XTAL32K.\n\r");
                        break;
                case CLOCK_SOURCE_RC32K:
                        DBG_LOG("\n\rClock source is set to RC32K.\n\r");
                        break;
                case CLOCK_SOURCE_RCX:
                        if (!(dg_configUSE_LP_CLK == LP_CLK_RCX)) {
                                DBG_LOG("\n\rRCX is not enabled. Please select another clock source or enable RCX.\n\r");
                                return;
                        } else {
                                DBG_LOG("\n\rClock source is set to RCX.\n\r");
                        }
                        break;
                case CLOCK_SOURCE_XTAL32M:
                        DBG_LOG("\n\rClock source is set to XTAL32M.\n\r");
                        break;
                case CLOCK_SOURCE_RC32M:
                        DBG_LOG("\n\rClock source is set to RC32M.\n\r");
                        break;
                case CLOCK_SOURCE_DIVN:
                        DBG_LOG("\n\rClock source is set to DIVN.\n\r");
                        break;
                default:
                        ASSERT_WARNING(0);
                        break;
                }

                /* Update clock settings */
                clock_cfg.src = source;

                /* Make sure the new settings are applied immediately */
                export_clock();
        } else {
                DBG_LOG("\n\rInvalid or same clock source selection\n\r");
        }
}

/* Command handler */
static void clock_mode_handler(int argc, const char *argv[], void *user_data)
{
        CLOCK_MODE mode;

        DBG_LOG("\n\rEntering %s\n\r", __func__);

        if (argc != 2) {
                DBG_LOG("\n\rMissing arguments: 'clock_mode <arg1>', where <arg1> is 0 --> manual, 1 --> auto\n\r");
                return;
        }

        /* Convert string to integer */
        mode = atoi(argv[1]);

        if (mode < CLOCK_MODE_INVALID && mode != clock_cfg.mode) {

                switch (mode) {
                case CLOCK_MODE_MANUAL:
                        /* Explicitly reset the previously selected port */
                        port_invalidate_clock_func();

                        /* Restore the previous port selection */
                        clock_cfg.port = old_port;
                        clock_cfg.pin = old_pin;

                        DBG_LOG("\n\rClock mode is set to manual.\n\r");
                        break;
                case CLOCK_MODE_AUTO:
                        /* Port selection might be overwritten; keep the old port */
                        old_port = clock_cfg.port;
                        old_pin = clock_cfg.pin;

                        DBG_LOG("\n\rClock mode is set to auto.\n\r");
                        break;
                default:
                        ASSERT_WARNING(0);
                        break;
                }

                /* Update clock settings */
                clock_cfg.mode = mode;

                /* Make sure the new settings are applied immediately */
                export_clock();
        } else {
                DBG_LOG("\n\rInvalid or same clock mode selection\n\r");
        }
}

/* Command handler */
static void clock_enable_handler(int argc, const char *argv[], void *user_data)
{
        bool is_enabled;

        DBG_LOG("\n\rEntering %s\n\r", __func__);

        if (argc != 2) {
                DBG_LOG("\n\rMissing arguments: 'clock_enable <arg1>', where <arg1> is 0 --> disable clock output, 1 --> enable\n\r");
                return;
        }

        /* Convert string to integer */
        is_enabled = !!atoi(argv[1]);

        if (is_enabled != clock_cfg.status) {
                /* Enable clock settings */
                clock_cfg.status = is_enabled;
                /* Make sure the new settings are applied immediately */
                export_clock();
        }

        DBG_LOG("\n\rClock output is %s\n\r", is_enabled ? "enabled" : "disabled");
}

/* Command handler */
static void clock_status_handler(int argc, const char *argv[], void *user_data)
{
        DBG_LOG("\n\rEntering %s\n\r", __func__);

        DBG_LOG("\n\rClock mode: %d\n\rClock PORT%d.%d\n\rClock source: %d\n\rClock status: %d\n\r",
                                                                                        clock_cfg.mode,
                                                                                        clock_cfg.port,
                                                                                        clock_cfg.pin,
                                                                                        clock_cfg.src,
                                                                                        clock_cfg.status);
}

/* Registered commands along with their handlers */
static const cli_command_t cmd_handlers[] = {
        {"clock_port",   clock_port_handler,   NULL},
        {"clock_mode",   clock_mode_handler,   NULL},
        {"clock_source", clock_source_handler, NULL},
        {"clock_enable", clock_enable_handler, NULL},
        {"clock_status", clock_status_handler, NULL},
        {} /* A null entry is required. Otherwise, the chances are that a bus fault will be issued. */
};

/* Callback function triggered following a sleep cycle */
static bool _ad_prepare_for_sleep(void)
{
        /* Abort sleep, return false, if clock is exported */
        return !clock_cfg.status;
}

/*
 * Register user-defined callback functions to Power Manager (PM)
 */
static const adapter_call_backs_t ad_pm_call_backs = {

        /* This callback function is fired when a sleep cycle is issued. */
        .ad_prepare_for_sleep = _ad_prepare_for_sleep,

        /* This callback function is fired when a wakeup cycle is issued.  */
        .ad_wake_up_ind = NULL,

        /*
         * This callback function is triggered in case the system was about
         * to enter sleep but the sleep cycle was eventually aborted.
         */
        .ad_sleep_canceled = NULL,

        /*
         * This callback function is triggered when XTAL32M
         * settles (after a wakeup cycle).
         */
        .ad_xtalm_ready_ind = NULL,

        /*
         * This parameter should be used to declare extra time required
         * for the system to stay awake (before entering sleep).
         */
        .ad_sleep_preparation_time = 0,
};

void export_clocks_init(void)
{
        if (!task_h) {
                /* Initialize CLI framework */
                cli_init();

                /* Start ExportSystemClock task  */
                OS_TASK_CREATE( "ExportClocks",                         /* The text name assigned to the task, for
                                                                           debug only; not used by the kernel. */
                                prvExportClocksTask,                    /* The function that implements the task. */
                                NULL,                                   /* The parameter passed to the task. */
                                1024,
                                                                        /* The number of bytes to allocate to the
                                                                           stack of the task. */
                                EXPORT_CLOCKS_TASK_PRIORITY,            /* The priority assigned to the task. */
                                task_h );                               /* The task handle */
                OS_ASSERT(task_h);
        }
}

/* Main application task */
static void prvExportClocksTask( void *pvParameters )
{
        OS_BASE_TYPE ret;
        uint32_t notif;

        cli_t cli = cli_register(CLI_NOTIF, cmd_handlers, cli_default_handler);

        DBG_LOG("\n\rEnter a valid command and press enter...\n\r");

        /* Register callback functions to Power Manager (PM) */
        pm_register_adapter(&ad_pm_call_backs);

        /* Set clock to default state */
        export_clock();

        for (;;) {
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                OS_ASSERT(ret == OS_OK);

                if (notif & CLI_NOTIF) {
                      cli_handle_notified(cli);
                }
        }
}
