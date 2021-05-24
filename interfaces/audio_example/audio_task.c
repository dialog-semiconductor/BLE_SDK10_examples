/**
 ****************************************************************************************
 *
 * @file audio_tack.c
 *
 * @brief Audio task
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include <stdbool.h>

#include <ad_nvms.h>
#include <hw_gpio.h>

#include "osal.h"
#include "sys_watchdog.h"
#include "sys_audio_mgr.h"
#include "sys_power_mgr.h"
#include "peripheral_setup.h"
#include "codec_iface.h"
#include "audio_iface.h"
#include "audio_task.h"

/**
 * Globals
 */

context_demo_apu_t context_demo_apu;

static audio_quality_t _codec_mic = {
        .freq = CODEC_SAMPLE_RATE,
        .bits = PCM_BIT_DEPTH,
        .channels = AUDIO_CHANNEL_MONO,
};

static audio_quality_t _memory_recording = {
        .freq = RECORDING_SAMPLE_RATE,
        .bits = PCM_BIT_DEPTH,
        .channels = AUDIO_CHANNEL_STEREO,
};

static audio_quality_t _codec_playback = {
        .freq = CODEC_SAMPLE_RATE,
        .bits = PCM_BIT_DEPTH,
        .channels = AUDIO_CHANNEL_STEREO,
};

static audio_dma_config_t _dma_rec = {
        .buff_l = NULL,
        .buff_r = NULL,
        .dma_l = AUDIO_DMA_OUT_L, //dma recording path is IN to the system, but OUT to resampler
        .dma_r = AUDIO_DMA_OUT_R,
        .buff_len = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE,
        .buffer_len_cb = DEMO_CHANNEL_DATA_BUF_CB_SIZE,
        .cb = audio_buffer_ready_cb,
        .ud = &context_demo_apu,
        .circular = false,
};

static audio_dma_config_t _dma_play = {
        .buff_l = (uint8_t*)(MEMORY_QSPIF_S_BASE + NVMS_LOG_PART_START + FLASH_CHANNEL_L),
        .buff_r = (uint8_t*)(MEMORY_QSPIF_S_BASE + NVMS_LOG_PART_START + FLASH_CHANNEL_R),
        .dma_l = AUDIO_DMA_IN_L,
        .dma_r = AUDIO_DMA_IN_R,
        .buff_len = DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE,
        .buffer_len_cb = DEMO_CHANNEL_DATA_BUF_CB_SIZE,
        .cb = audio_buffer_ready_cb,
        .ud = &context_demo_apu,
        .circular = false,
};

static audio_iface_t _audio;

/**
 * @brief Copy size bytes from RAM to QSPI, or fail with a message.
 * @param RAM address
 * @param QSPI offset from QSPI start
 * @param no. of bytes to copy
 */
static void copy_ram_to_qspi(uint8_t *ram, uint32_t qspi, uint32_t size)
{
        bool success = false;
        if (ram) {
                nvms_t part = ad_nvms_open(NVMS_LOG_PART);

                /* addr is any address in partition address space
                 * buf can be any address in memory including QSPI mapped flash */
                ad_nvms_write(part, qspi, ram, size);
                uint32_t buf = (NVMS_LOG_PART_START + MEMORY_QSPIF_S_BASE + qspi);
                if (memcmp(ram, (uint8_t*)buf, size)) {
                        printf("Wrong write at addr : %ld\n\r", buf);
                } else {
                        success = true;
                }
        }
        printf("\r\nWrite %lu bytes, %s\r\n", size, (success ? "success" : "fail"));
}

/**
 * @brief Try to allocate size bytes from the heap.
 * @param allocation size
 * @return pointer to newly allocated memory, or NULL on failure.
 * This pointer should be freed with free()
 */
static uint8_t* try_alloc(uint32_t size)
{
        if (size > OS_GET_FREE_HEAP_SIZE()) {
                printf("Not enough heap size %ld, reduce channels or memory buffer", size);
                return NULL;
        }
        return (uint8_t*)OS_MALLOC(size);
}

/**
 * @brief Beepy demo.
 * Periodically play back some audio file from the QSPI flash on the PCM
 * To allow the system to go to sleep in between playbacks to measure power consumption.
 */
static void demo_beepy()
{
        uint32_t notified_value;
        while (true) {
                sys_watchdog_notify(context_demo_apu.audio_task_wdog_id); //notify wdog we are still alive
                //1 sec sleep in the beginning
                OS_DELAY_MS(1000);

                context_demo_apu.available_to_read = 0;
                context_demo_apu.audio_path = sys_audio_mgr_open(&_audio.dma_out,
                        &_audio.pcm_out);
                //2 sec dma copy (RECORDING_DURATION_SEC)
                codec_playback();
                bool success = sys_audio_mgr_start(context_demo_apu.audio_path);
                OS_ASSERT(success);
                printf("%s\n", "Start.");
                /*
                 * Wait for notification from audio_buffer_ready_cb()
                 * that the DMA is finished
                 */
                OS_BASE_TYPE xResult = OS_TASK_NOTIFY_WAIT(0x0,
                        OS_TASK_NOTIFY_ALL_BITS,
                        &notified_value,
                        OS_TASK_NOTIFY_FOREVER);

                OS_ASSERT(xResult == OS_OK);
                codec_shutdown();
                sys_audio_mgr_stop(context_demo_apu.audio_path);
                sys_audio_mgr_close(context_demo_apu.audio_path);
                //1 sec sleep in the end
                OS_DELAY_MS(1000);
                //total loop duration = 4 sec < ~80 sec WDOG timeout, so no worry if WDOG expires during the loop
                codec_resume();
        }
}

/**
 * @brief Loopback demo.
 * Play back a microphone input to a speaker output directly through the resampler
 */
static void demo_loopback()
{
        sys_watchdog_notify(context_demo_apu.audio_task_wdog_id); //notify wdog we are still alive

        if (DEMO_SELECTION == DEMO_PDM_MIC) {

                context_demo_apu.audio_path = sys_audio_mgr_open(&_audio.pdm_in,
                        &_audio.pcm_out);
                codec_playback();

        } else if (DEMO_SELECTION == DEMO_PCM_MIC) {

                context_demo_apu.audio_path = sys_audio_mgr_open(&_audio.pcm_in,
                        &_audio.pcm_out);
                printf_settings(&_audio.pcm_in, INPUT_DEVICE);
                printf_settings(&_audio.pcm_out, OUTPUT_DEVICE);
                codec_record();
                codec_playback();
        } else {
                OS_ASSERT(false); //invalid demo
        }
        bool success = sys_audio_mgr_start(context_demo_apu.audio_path);
        OS_ASSERT(success);
        printf("%s\n", "Start.");

        while (true){
                sys_watchdog_notify(context_demo_apu.audio_task_wdog_id); //notify wdog we are still alive
                pm_sleep_mode_request(pm_mode_idle);
                OS_DELAY_MS(10000); //yield
                pm_sleep_mode_release(pm_mode_idle);
        }
        codec_shutdown();
        sys_audio_mgr_stop(context_demo_apu.audio_path);
        sys_audio_mgr_close(context_demo_apu.audio_path);
}

/**
 * @brief Record and playback demo
 * Push button to talk to microphone and record the voice data to the QSPI,
 * then push again to play it back.
 */
void demo_record_playback()
{
        uint32_t notified_value;
        printf("%s\n", "Press button to record");
        for (;;) {
                sys_watchdog_notify(context_demo_apu.audio_task_wdog_id);//notify wdog we are still alive
                if (!hw_gpio_get_pin_status(BUTTON_PORT, BUTTON_PIN)) {

                        if (DEMO_SELECTION == DEMO_PDM_RECORD_PLAYBACK) {
                                context_demo_apu.audio_path = sys_audio_mgr_open(&_audio.pdm_in,
                                        &_audio.dma_in);
                                printf_settings(&_audio.pdm_in, INPUT_DEVICE);
                        } else if (DEMO_SELECTION == DEMO_PCM_RECORD_PLAYBACK) {
                                context_demo_apu.audio_path = sys_audio_mgr_open(&_audio.pcm_in,
                                        &_audio.dma_in);
                                printf_settings(&_audio.pcm_in, INPUT_DEVICE);
                                codec_record();
                        }
                        printf_settings(&_audio.dma_in, OUTPUT_DEVICE);
                        context_demo_apu.available_to_read = 0;

                        bool success = sys_audio_mgr_start(context_demo_apu.audio_path);
                        OS_ASSERT(success);

                        printf("%s\n", "Recording...");//RECORDING_DURATION_SEC (2)
                        /*
                         * Wait for notification from audio_buffer_ready_cb()
                         * that the DMA is finished
                         */
                        sys_watchdog_notify(context_demo_apu.audio_task_wdog_id);
                        sys_watchdog_suspend(context_demo_apu.audio_task_wdog_id);
                        OS_BASE_TYPE xResult = OS_TASK_NOTIFY_WAIT(0x0,
                                OS_TASK_NOTIFY_ALL_BITS,
                                &notified_value,
                                OS_TASK_NOTIFY_FOREVER);

                        OS_ASSERT(xResult == OS_OK);

                        sys_watchdog_notify_and_resume(context_demo_apu.audio_task_wdog_id);

                        sys_audio_mgr_stop(context_demo_apu.audio_path);
                        sys_audio_mgr_close(context_demo_apu.audio_path);
                        context_demo_apu.available_to_read = 0;
                        copy_ram_to_qspi(_dma_rec.buff_l, FLASH_CHANNEL_L,
                                DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE);
                        copy_ram_to_qspi(_dma_rec.buff_r, FLASH_CHANNEL_R,
                                DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE);

                        printf_settings(&_audio.dma_out, INPUT_DEVICE);
                        if (DEMO_SELECTION == DEMO_PDM_RECORD_PLAYBACK) {

                                context_demo_apu.audio_path = sys_audio_mgr_open(&_audio.dma_out,
                                        &_audio.pcm_out);
                                printf_settings(&_audio.pcm_out, OUTPUT_DEVICE);
                        } else if (DEMO_SELECTION == DEMO_PCM_RECORD_PLAYBACK) {
                                context_demo_apu.audio_path = sys_audio_mgr_open(&_audio.dma_out,
                                        &_audio.pcm_out);
                                printf_settings(&_audio.pcm_out, OUTPUT_DEVICE);
                                codec_stop_recording();
                        }

                        codec_playback(); //RECORDING_DURATION_SEC (2)
                        success = sys_audio_mgr_start(context_demo_apu.audio_path);
                        OS_ASSERT(success);

                        printf("%s\n", "Recorded:");

                        sys_watchdog_notify(context_demo_apu.audio_task_wdog_id);
                        sys_watchdog_suspend(context_demo_apu.audio_task_wdog_id);

                        xResult = OS_TASK_NOTIFY_WAIT(0x0,
                                OS_TASK_NOTIFY_ALL_BITS,
                                &notified_value,
                                OS_TASK_NOTIFY_FOREVER);

                        OS_ASSERT(xResult == OS_OK);

                        sys_watchdog_notify_and_resume(context_demo_apu.audio_task_wdog_id);

                        /**
                         * Due to a shortcoming in the SDK resource management system,
                         * (there is no public access to acquired_resources)
                         * the APU driver can not implement audio playback / recording Start/ Stop
                         * without shutting down completely to release its resources.
                         * So stop() and close() should be called always together.
                         * Essentially they could be merged in the same function.
                         */
                        sys_audio_mgr_stop(context_demo_apu.audio_path);
                        sys_audio_mgr_close(context_demo_apu.audio_path);
                        codec_stop_playback();
                        printf("%s\n", "Stop! hammertime");
                        //total loop duration = 4 sec < ~80 sec WDOG timeout, so no worry if WDOG expires during the loop
                }
        }
}

void audio_buffer_ready_cb(sys_audio_mgr_buffer_data_block_t *buffer, void *app_ud)
{
        context_demo_apu_t *demo_apu = app_ud;
        /*DMA buffer->buff_len_cb holds number of total bytes transferred from the DMA
         * channel that executed the callback */
        demo_apu->available_to_read += buffer->buff_len_cb;

        /* buffer->buff_len_total holds the number of bytes for transfer per channel
         * Therefore the current bytes available should be compared to the number of DMA channels
         * times the buff_len_total in order to know when the transfer is finished.
         */
        if (demo_apu->available_to_read == _memory_recording.channels * buffer->buff_len_total) {
                OS_TASK_NOTIFY_FROM_ISR(demo_apu->audio_task, 1, OS_NOTIFY_NO_ACTION);
        }
}

void audio_task(void *pvParameters)
{
        //register this task to wdog 
        context_demo_apu.audio_task_wdog_id = sys_watchdog_register(false);
        /* if the task is registered to the WD it has to be properly handled and at the end un-registered.*/

        prepare_i2s(&_audio.pcm_in, &_codec_mic);
        prepare_i2s(&_audio.pcm_out, &_codec_playback);
        prepare_pdm(&_audio.pdm_in, AUDIO_CHANNEL_STEREO);
        prepare_dma(&_audio.dma_out, &_memory_recording, &_dma_play);

        switch (DEMO_SELECTION) {
        case DEMO_PDM_RECORD_PLAYBACK: case DEMO_PCM_RECORD_PLAYBACK:
                _dma_rec.buff_r = try_alloc(DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE);
                _dma_rec.buff_l = try_alloc(DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE);
                prepare_dma(&_audio.dma_in, &_memory_recording, &_dma_rec);
                demo_record_playback();
                break;
        case DEMO_PDM_MIC: case DEMO_PCM_MIC:
                demo_loopback();
                break;
        default: case DEMO_PCM_PLAYBACK:
                demo_beepy();
                break;

        }
        OS_TASK_DELETE( NULL ); //should never get here
        sys_watchdog_unregister(context_demo_apu.audio_task_wdog_id); //unregister the task before leaving
}

