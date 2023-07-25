/**
 ****************************************************************************************
 *
 * @file snc_sdadc.c
 *
 * @brief The set-up, SNC ucode and M33 task data collection
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

#include <stdio.h>

#include "ad_snc.h"

#include "hw_pdc.h"
#include "hw_rtc.h"

#include "snc_sdadc.h"

#include "SeNIS.h"
#include "snc_queues.h"
#include "snc_hw_sdadc.h"

#include "platform_devices.h"

/***************************************************************************/
#if 0   //For a quick test of operation without queue
#  undef dg_configUSE_SNC_QUEUES
#  define dg_configUSE_SNC_QUEUES 0
#endif
/***************************************************************************/

#if dg_configUSE_SNC_QUEUES
#  define SNC_SDADC_SAMPLE_SIZE           4
#  define SNC_SDADC_SAMPLES_TO_QUEUE      4
#  define SNC_SDADC_QUEUE_CHUNK_SIZE      (SNC_SDADC_SAMPLE_SIZE*SNC_SDADC_SAMPLES_TO_QUEUE)
#endif

#if !dg_configUSE_SNC_QUEUES
_SNC_RETAINED static snc_cm33_mutex_t soc_snc_adc_mutex;
#else
_SNC_RETAINED static uint32_t sdadc_num_of_samples = 0;
_SNC_RETAINED static uint32_t sdadc_notify_samples = SNC_SDADC_SAMPLES_TO_QUEUE;
#endif

PRIVILEGED_DATA static uint32_t sdadc_ucode_id;

static __RETAINED struct {
        uint32_t sdadc_gain_corr_reg;
        uint32_t sdadc_offs_corr_reg;
#if !dg_configUSE_SNC_QUEUES
        uint32_t current_value;
#endif
} soc_snc_adc_ucode_env;

static void ucode_adc_init()
{
        int16_t gain_corr;
        int16_t offs_corr;

        const ad_sdadc_controller_conf_t *cfg = &SDADC_VBAT;

#if !dg_configUSE_SNC_QUEUES
        snc_mutex_SNC_create(&soc_snc_adc_mutex);
#endif

        memset( &soc_snc_adc_ucode_env, 0, sizeof(soc_snc_adc_ucode_env) );

        //Get the trim values, to write from SNC after configuration of SDADC before reading
        //Just declare this - it is actually in sdk\bsp\system\sys_man\sys_tcs_da1469x.c
        //It is what hw_sdadc.c does also
        __WEAK void hw_sdadc_get_trimmed_values(uint8_t mode, int16_t *gain, int16_t *offs);

        /* retrieve gain and offset from TCS */
        hw_sdadc_get_trimmed_values(cfg->drv->input_mode, &gain_corr, &offs_corr);
        soc_snc_adc_ucode_env.sdadc_gain_corr_reg = (int32_t) gain_corr;
        soc_snc_adc_ucode_env.sdadc_offs_corr_reg = (int32_t) offs_corr;
}

void snc_sdadc_config(ad_snc_interrupt_cb _adc_cb)

{
        ad_snc_ucode_cfg_t cfg = { 0 };

        // Configure the PDC event and uCode priorities
        cfg.pdc_evt_pr = AD_SNC_PDC_EVT_PR_0;
        cfg.ucode_pr = AD_SNC_UCODE_PR_1;

        // Set the SNC to CM33 notification callback
        cfg.cb = _adc_cb;

        //This PDC entry will lead the RTC interrupt to trigger the execution of our SNC ucode
        //It is identical to the entry in sys_adc.c, so if dg_configUSE_SYS_ADC is defined our
        //ucode will be added to the list for the original entry, but if not we will
        //configure the RTC below, and this entry will be created exclusively for our ucode
        cfg.pdc_entry = HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                HW_PDC_PERIPH_TRIG_ID_RTC_TIMER, HW_PDC_MASTER_SNC,
                HW_PDC_LUT_ENTRY_EN_PER);

#if dg_configUSE_SNC_QUEUES
        cfg.snc_to_cm33_queue_cfg.max_chunk_bytes = SNC_SDADC_QUEUE_CHUNK_SIZE;

        cfg.snc_to_cm33_queue_cfg.enable_data_timestamp = true;
        cfg.snc_to_cm33_queue_cfg.element_weight = SNC_QUEUE_ELEMENT_SIZE_WORD;
        cfg.snc_to_cm33_queue_cfg.num_of_chunks = 2;
        cfg.snc_to_cm33_queue_cfg.swap_popped_data_bytes = false;       //true;
#endif

        ucode_adc_init();

        // Register uCode
        sdadc_ucode_id = ad_snc_ucode_register(&cfg, SNC_UCODE_CTX(ucode_sdadc_read_notify));

        if ( -1 == sdadc_ucode_id) {

                printf("ad_snc_ucode_register FAILED\r\n");
                return;

        } else {

//If no BLE, undefine this RF feature - ad_ble.c will NOT be starting the RTC for us,
//so we need to start ourselves. Surplus for this demo, but important to bear in mind
//if integrating with other code.
#ifndef CONFIG_USE_BLE
#undef dg_configRF_ENABLE_RECALIBRATION
#define dg_configRF_ENABLE_RECALIBRATION        0
#endif

#if !dg_configRF_ENABLE_RECALIBRATION
                hw_rtc_config_pdc_evt_t rtc_cfg = { 0 };
#endif

                ad_snc_ucode_enable(sdadc_ucode_id);

                //We want to use the RTC to trigger the SDADC readings
                //If for some reason we have no current RTC event driven for the
                //RFTEMP GPADC reading, we need to set this up here
#if !dg_configRF_ENABLE_RECALIBRATION
                //RTC initialization function
                // Enable the RTC PDC event
                rtc_cfg.pdc_evt_en = true;

                // Set the RTC PDC event period
                rtc_cfg.pdc_evt_period = (1000 / 10) - 1;       //1 sec : 1000ms into N-1 10ms ticks

                // Initialize the RTC peripheral with the given configuration
                hw_rtc_config_RTC_to_PDC_evt(&rtc_cfg);

                // Enable the RTC peripheral clock
                hw_rtc_clock_enable();

                // Start the RTC
                hw_rtc_time_start();
#endif

        }
}

#if dg_configUSE_SNC_QUEUES
_SNC_RETAINED static uint32_t queue_is_full;
_SNC_RETAINED static uint32_t *writeQp = 0;
_SNC_RETAINED static uint32_t sdadc_timestamp;
#endif

SNC_UCODE_BLOCK_DEF(ucode_sdadc_read_notify)
{
        // Clear the RTC PDC event
        SNC_hw_sys_clear_rtc_pdc_event();

#if dg_configUSE_SNC_QUEUES
        SENIS_assign(da(&queue_is_full), 0);
        SNC_queues_snc_wq_is_full(da(&queue_is_full));
        // If queue is full
        SENIS_if (da(&queue_is_full), EQ, 1) {
                SNC_CM33_NOTIFY();
                SENIS_return;
        }

        SENIS_if (da(&writeQp), EQ, 0) {
                SNC_queues_snc_get_wq(da(&writeQp), SNC_SDADC_QUEUE_CHUNK_SIZE, da(&sdadc_timestamp));
        }

        //timestamp is just a counter
        SENIS_inc1(da(&sdadc_timestamp));
#endif

        // Initialize - Acquire the SDADC source device
        SNC_sdadc_open(&SDADC_VBAT);

        SENIS_assign(da(&SDADC->SDADC_GAIN_CORR_REG), da(&soc_snc_adc_ucode_env.sdadc_gain_corr_reg) );
        SENIS_assign(da(&SDADC->SDADC_OFFS_CORR_REG), da(&soc_snc_adc_ucode_env.sdadc_offs_corr_reg) );

        // Get adc measurement - we seem safe to assume the upper bits will be zero, i.e. 0x0000xxxx, no errors
#if dg_configUSE_SNC_QUEUES
        SNC_sdadc_read(&SDADC_VBAT, ia(&writeQp));
#else
        SNC_sdadc_read(&SDADC_VBAT, da(adc_reading));
        SNC_MUTEX_LOCK(&soc_snc_adc_mutex);
        SENIS_assign(da(&soc_snc_adc_ucode_env.current_value), da(adc_reading));
        SNC_MUTEX_UNLOCK(&soc_snc_adc_mutex);
#endif

        SNC_sdadc_close(&SDADC_VBAT);

#if dg_configUSE_SNC_QUEUES
        // Push the recently acquired data into the SNC queue
        SNC_queues_snc_push();

        // ++sample pointer;
        SENIS_inc4(da(&writeQp));

        // ++num_of_samples;
        SENIS_inc1(da(&sdadc_num_of_samples));

        // If SNC_SDADC_QUEUE_CHUNK_SIZE number of samples have been collected
        SENIS_if (da(&sdadc_num_of_samples), EQ, da(&sdadc_notify_samples)) {

                // Set the data pointer to 0
                SENIS_assign(da(&writeQp), 0);

                // Set the number of samples to 0
                SENIS_assign(da(&sdadc_num_of_samples), 0);

                // Notify CM33 that collecting the desired amount of samples has finished
                SNC_CM33_NOTIFY();
        }
#else
        //Notify for every sample if no queues
        SNC_CM33_NOTIFY();
#endif
}

static void snc_sdadc_dump_sample(uint32_t sample)
{
        if(sample & SDADC_RET_BITMASK_ERRORS)
                printf("SDADC ERROR");
        else
                printf("0x%08x", (unsigned int)(sample & SDADC_RET_BITMASK_VALUE) );
}

void snc_sdadc_dump_data(void)
{
#if dg_configUSE_SNC_QUEUES
        uint32_t sdadc_bytes;
        uint32_t sdadc_data[SNC_SDADC_SAMPLES_TO_QUEUE];
        uint32_t sdadc_time_s;

        while (ad_snc_queue_pop(sdadc_ucode_id, (uint8_t*)sdadc_data, &sdadc_bytes,
               &sdadc_time_s)) {
                printf("SDADC Samples : %lu, TS: 0x%06lx\r\n", sdadc_bytes / SNC_SDADC_SAMPLE_SIZE, sdadc_time_s);

                // Print the acquired samples
                for (uint32_t i = 0; i < (sdadc_bytes / SNC_SDADC_SAMPLE_SIZE); i++) {
                        printf("Measurement-%d: ", (unsigned int)i );
                        snc_sdadc_dump_sample(sdadc_data[i]);
                        printf("\r\n");
                }
        }
#else
        uint32_t tmp;
        snc_mutex_SNC_lock(&soc_snc_adc_mutex);
        tmp = soc_snc_adc_ucode_env.current_value;
        snc_mutex_SNC_unlock(&soc_snc_adc_mutex);
        printf("Measurement: ");
        snc_sdadc_dump_sample( tmp );
        printf("\r\n");
#endif
}
