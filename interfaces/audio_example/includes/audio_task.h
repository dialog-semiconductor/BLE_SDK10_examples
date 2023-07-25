/**
 ****************************************************************************************
 *
 * @file audio_task.h
 *
 * @brief Audio task
 *
 * Copyright (C) 2020-2021 Renesas Electronics Corporation and/or its affiliates
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
#ifndef _AUDIO_TASK_H_
#define _AUDIO_TASK_H_

#define DEMO_PDM_MIC                    (0) //loopback PDM microphones to PCM output
#define DEMO_PDM_RECORD_PLAYBACK        (1) //record from PDM microphones to flash and then play back
#define DEMO_PCM_MIC                    (2) //loopback PCM microphones to PCM speaker
#define DEMO_PCM_RECORD_PLAYBACK        (3) //record from PCM microphones to flash and then play back
#define DEMO_PCM_PLAYBACK               (4) //play back a file from flash to PCM speaker

#define DEMO_SELECTION                  DEMO_PCM_PLAYBACK

#define NOTIF_TASK_TEST_DONE          (1 << 1)
#define DEMO_CHANNEL_DATA_BUF_BASIC_SIZE (1024)
#define RECORDING_DURATION_SEC (2)
#define DEMO_CHANNEL_DATA_BUF_TOTAL_SIZE (DEMO_CHANNEL_DATA_BUF_BASIC_SIZE * RECORDING_DURATION_SEC * 16)//32 K buffers
#define DEMO_CHANNEL_DATA_BUF_CB_SIZE    (DEMO_CHANNEL_DATA_BUF_BASIC_SIZE * 2) //callback every 2K

#define FLASH_CHANNEL_L 0x0     //left channel in flash
#define FLASH_CHANNEL_R 0x80000 //right channel in flash

#define PRINTF_RECORDED_CHANNELS(ch) (ch == HW_PDM_CHANNEL_R ? "R" : \
                                       ch == HW_PDM_CHANNEL_L ? "L" : \
                                       ch == HW_PDM_CHANNEL_LR ? "L and R" : "none")

typedef enum {
        INPUT_DEVICE,
        OUTPUT_DEVICE
} device_direction_t;

/**
 * @brief required data from the context of the audio task
 */
typedef struct {
        void *audio_path;   //pointer to the APU open audio path
        OS_TASK audio_task;//the OS TASK identifier
        uint32_t available_to_read; //bytes available for audio transfer
        int audio_task_wdog_id;
} context_demo_apu_t;

extern context_demo_apu_t context_demo_apu;
/**
 * @brief visualize the APU I/O device settings
 * @param the device
 * @param direction (I/O)
 */
void printf_settings(sys_audio_device_t *dev, device_direction_t dir);

/**
 * @brief Callback called when audio DMA transfer is done
 * @param pointer to the DMA buffer
 * @param pointer to task context passed on from DMA user data
 */
void audio_buffer_ready_cb(sys_audio_mgr_buffer_data_block_t *buffer, void *app_ud);

/**
 * @brief the main audio task
 * @param eters
 */
void audio_task(void *pvParameters);

#endif /* _AUDIO_TASK_H_ */
