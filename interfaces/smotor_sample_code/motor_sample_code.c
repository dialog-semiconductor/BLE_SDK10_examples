/**
 ****************************************************************************************
 *
 * @file motor_sample_code.c
 *
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

#include "hw_smotor.h"
#include "motor_sample_code.h"
#include "hw_rtc.h"


/* Pater Generator (PG) #0 Configuration Parameters */
static hw_smotor_pg_cfg_t pg_0_cfg = {

        /* This PG contributes to the start/stop waveform IRQs */
        .genstart_irq_en = HW_SMOTOR_PG_GENSTART_IRQ_ENABLE,
        .genend_irq_en = HW_SMOTOR_PG_GENEND_IRQ_ENABLE,

#if (SMOTOR_MIRRORING_MODE_ENABLE)
        /*
         * There are two mirroring modes:
         *
         * Single mode : PGx_SIG0 signal is replicated on PGx_SIG2 pin
         * Pair mode   : PGx_SIG0 and PGx_SIG1 signals are replicated on PGx_SIG2 and PGx_SIG3 pins respectively
         *
         * \note There is no need for the mirroring signals to be enabled (e.g. HW_SMOTOR_SIGNAL_2_ENABLE).
         *       This is done automatically by the motor controller.
         *
         **/
        .pg_mode = HW_SMOTOR_PG_PAIR_MODE,
#else
        .pg_mode = HW_SMOTOR_PG_FLEX_MODE,
#endif


        /*
         * A command(s) stored in the commandFIFO is popped following a trigger event (RTC or LP clock).
         *
         * \note This mode can also be used in case the triggering of the motor controller is done manually (hw_smotor_trigger())
         *
         * \note In case the manual mode is selected the hw_smotor_pg_start() API should be invoked before triggering the controller
         *       using the hw_smotor_trigger() API
         **/
        .pg_start_mode = HW_SMOTOR_AUTO_START_MODE,


        /*
         * Enable/disable PG #0 signals.
         *
         * \note Mirroring signals are automatically enabled by the motor controller
         *       and so, there is no need to be enabled.
         *
         * \warning During wave generation not used signals must be disabled. Otherwise, the target PG will get stuck!!!
         *
         **/
        .sig0_en = HW_SMOTOR_SIGNAL_0_ENABLE,
        .sig1_en = HW_SMOTOR_SIGNAL_1_ENABLE,
        .sig2_en = HW_SMOTOR_SIGNAL_2_DISABLE,
        .sig3_en = HW_SMOTOR_SIGNAL_3_DISABLE,


        /* Select which signal of the PG #0 is routed to each output */
        .out0_sig = HW_SMOTOR_OUTPUT_SIGNAL_0, /* PG0_SIG0 is mapped on P0_26 pin */
        .out1_sig = HW_SMOTOR_OUTPUT_SIGNAL_1, /* PG0_SIG1 is mapped on P0_27 pin */
        .out2_sig = HW_SMOTOR_OUTPUT_SIGNAL_2, /* PG0_SIG2 is mapped on P0_28 pin */
        .out3_sig = HW_SMOTOR_OUTPUT_SIGNAL_3, /* PG0_SIG3 is mapped on P0_29 pin */
};


/* PG #1 Configuration Parameters */
__UNUSED hw_smotor_pg_cfg_t pg_1_cfg = { 0 };

/* PG #2 Configuration Parameters */
__UNUSED hw_smotor_pg_cfg_t pg_2_cfg = { 0 };

/* PG #3 Configuration Parameters */
__UNUSED hw_smotor_pg_cfg_t pg_3_cfg = { 0 };

/* PG #4 Configuration Parameters */
__UNUSED hw_smotor_pg_cfg_t pg_4_cfg = { 0 };



/* Push commands into the commandFIFO  */
void  _motor_push_commandFIFO(_motor_commandFIFO *commands[], uint8_t len)
{
        for (int i = 0; i < len; i++) {
                if (commands[i] != NULL) {
                        hw_smotor_push_command(commands[i]->pg_id, commands[i]->pg_sig,
                                                        commands[i]->n_cmds, commands[i]->w_ptr);
                }
        }
}


/* Initialize all the 64 entries of the command FIFO with zeros */
void _motor_clear_commandFIFO(void)
{
        /* Reset commandFIFO read/write pointers (both return to zero)  */
        hw_smotor_fifo_pointers_reset();

        /* Clean commandFIFO. It consists of 64 entries 16-bit each. */
        for (int i = 0; i <= MAX_W_PTR; i++) {
                hw_smotor_push_command(0x0, 0x0, 0x0, 0x0);
        }
}


/* Add wave definitions into the Wave Table */
void _motor_fill_wave_table(const uint8_t *waves[], uint8_t waves_idx[], uint8_t len)
{
        /* Up to 12 user-defined waves is allowed to be stored in the Wave Table */
        for (int i = 0; i < len; i++) {
                if (waves[i] != NULL) {
                        /*
                         * Add waveforms into the Wave Table. The function checks the PuN filed (the first
                         * element of the provided array) and uses the following formula to calculate the
                         * number of ToD required: ((2 * PuN) + 1)
                         *
                         * \note This API returns the starting index of the waveform pushed into the Wave Table.
                         *
                         **/
                        waves_idx[i] = hw_smotor_add_wave(waves[i]);
                        ASSERT_WARNING(waves_idx[i] != 0xFF); // There is no space for storing the current waveform into the Wave Table
                }
        }
}

/* Motor initialization */
void _motor_init(void)
{
        /* Motor controller configurations */
        hw_smotor_cfg_t smotor_cfg = {

                /* CommandFIFO operation mode */
                .operation_mode = HW_SMOTOR_CYCLIC_FIFO_MODE,

                /*
                 * Configure the command FIFO depth. The read pointer of the FIFO is reverted
                 * to the beginning after reaching the configured value. For instance if four
                 * commands are to be popped then set this value to 4.
                 *
                 * \note Valid only in cyclic FIFO mode.
                 *
                 **/
                .cyclic_size = SMOTOR_COMMAND_FIFO_DEPTH,

                /*
                 * Time interval (idle) added upon a waveform completion. This time is automatically
                 * added by the controller at the end of a waveform and can have zero value.
                 */
                .moi = 10,


                /* Enable/disable motor IRQs triggered upon a waveform completion */
                .genend_irq_en = HW_SMOTOR_GENEND_IRQ_ENABLE,

                /* Enable/disable motor IRQs triggered upon a waveform instantiation */
                .genstart_irq_en = HW_SMOTOR_GENSTART_IRQ_ENABLE,


                /*
                 * Enable/disable motor IRQs triggered when the read/write pointer of commandFIFO
                 * is at or below a predefined value.
                 */
                .threshold_irq_en = HW_SMOTOR_THRESHOLD_IRQ_DISABLE,
                .threshold = 0,


                /* Enable/disable motor IRQs triggered following a FIFO overflow event */
                .fifo_overflow_irq_en = HW_SMOTOR_FIFO_OVF_IRQ_DISABLE,

                /* Enable/disable motor IRQs triggered following a FIFO underrun event */
                .fifo_underrun_irq_en = HW_SMOTOR_FIFO_UNR_IRQ_DISABLE,


                /* Enable/disable triggering the motor controller following LP clock events (XTAL32K) */
                .sleep_clk_trigger_en = HW_SMOTOR_MC_LP_CLK_TRIG_DISABLE,

                /* Enable/disable triggering the motor controller following RTC events  */
                .rtc_trigger_en = HW_SMOTOR_TRIG_RTC_EVENT_DISABLE,


                /* Configure the PGs */
                .pg_cfg = {&pg_0_cfg, &pg_1_cfg, &pg_2_cfg, &pg_3_cfg, &pg_4_cfg}
        };



        /* Configures the motor controller as well as the patter generators */
        hw_smotor_initialization(&smotor_cfg);


        /* Clear commandFIFO */
        _motor_clear_commandFIFO();

        /*
         * Reset the index used for pointing to the next location in the Wave Table
         * where a new wave definition can be stored.
         */
        hw_smotor_rst_wave_idx();


        /* Clock divider of the XTAL32K clock used for generating motor "slots" */
        REG_SETF(CRG_PER, RESET_CLK_PER_REG, MC_CLK_DIV, 0x1F);
        REG_SETF(CRG_PER, CLK_PER_REG, MC_CLK_DIV, SMOTOR_SLOT_LP_CLK_DIVIDER);

}


/* RTC Initialization */
void _rtc_init(uint16_t period)
{
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

        /* Start running the RTC */
        hw_rtc_start();
}



/* Read commadFIFO by its index. The FIFO consists of 64 entries. Valid values are 0-63 */
__INLINE uint16_t _read_commandFIFO_by_index(uint8_t index)
{
        ASSERT_WARNING(index <= MAX_W_PTR);

        return *(((uint16_t *)&SMOTOR->CMD_TABLE_BASE) + index);
}

