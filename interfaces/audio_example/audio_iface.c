/**
 ****************************************************************************************
 *
 * @file audio_iface.c
 *
 * @brief Interface with the Audio Processing Unit Implementations
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include "osal.h"
#include "sys_audio_mgr.h"
#include "audio_iface.h"

sys_audio_device_t * prepare_i2s(sys_audio_device_t * device, const audio_quality_t * quality)
{
        if(device && quality){
                device->device_type = AUDIO_PCM;
                device->pcm_param.format = I2S_MODE;
                device->pcm_param.bits_depth = quality->bits;
                device->pcm_param.fsc_length = (quality->bits) >> 3; //bytes
                device->pcm_param.sample_rate = quality->freq;
                device->pcm_param.total_channel_num = 2; //i2s only has two channels

                device->pcm_param.channel_delay = HW_PCM_FSC_STARTS_1_CYCLE_BEFORE_MSB_BIT;
                device->pcm_param.clock = HW_PCM_CLK_DIVN;
                device->pcm_param.cycle_per_bit = HW_PCM_ONE_CYCLE_PER_BIT;
                device->pcm_param.enable_dithering = true;

                device->pcm_param.fsc_delay = HW_PCM_FSC_STARTS_SYNCH_TO_MSB_BIT;
                device->pcm_param.inverted_clk_polarity = HW_PCM_CLK_POLARITY_INVERTED;
                device->pcm_param.inverted_fsc_polarity = HW_PCM_FSC_POLARITY_NORMAL;
                device->pcm_param.mode = MODE_MASTER;
                device->pcm_param.output_mode = HW_PCM_DO_OUTPUT_PUSH_PULL;
        }
        return device;
}

sys_audio_device_t * prepare_pcm(sys_audio_device_t * device, const audio_quality_t * quality)
{
        if(device && quality){
                device->device_type = AUDIO_PCM;
                device->pcm_param.format = PCM_MODE;
                device->pcm_param.bits_depth = quality->bits;
                device->pcm_param.fsc_length = 0; //bytes
                device->pcm_param.sample_rate = quality->freq;
                device->pcm_param.total_channel_num = 1;//quality->channels;

                device->pcm_param.channel_delay = HW_PCM_FSC_STARTS_1_CYCLE_BEFORE_MSB_BIT;
                device->pcm_param.clock = HW_PCM_CLK_DIVN;
                device->pcm_param.cycle_per_bit = HW_PCM_ONE_CYCLE_PER_BIT;
                device->pcm_param.enable_dithering = true;

                device->pcm_param.fsc_delay = HW_PCM_FSC_STARTS_SYNCH_TO_MSB_BIT;
                device->pcm_param.inverted_clk_polarity = HW_PCM_CLK_POLARITY_INVERTED;
                device->pcm_param.inverted_fsc_polarity = HW_PCM_FSC_POLARITY_NORMAL;
                device->pcm_param.mode = MODE_MASTER;
                device->pcm_param.output_mode = HW_PCM_DO_OUTPUT_PUSH_PULL;
        }
        return device;
}

sys_audio_device_t * prepare_pdm(sys_audio_device_t * device, uint8_t channels)
{
        if(device){
                device->device_type = AUDIO_PDM;
                device->pdm_param.mode = MODE_MASTER;
                device->pdm_param.clk_frequency = PDM_FREQ_2MHZ;
                if(channels == AUDIO_CHANNEL_MONO){
                        device->pdm_param.channel = HW_PDM_CHANNEL_R;
                } else {
                        device->pdm_param.channel = HW_PDM_CHANNEL_LR;
                }
                device->pdm_param.in_delay = HW_PDM_DI_NO_DELAY;
                device->pdm_param.out_delay = HW_PDM_DO_NO_DELAY;
        }
        return device;
}

sys_audio_device_t * prepare_dma(sys_audio_device_t * device,
                                const audio_quality_t * quality,
                                const audio_dma_config_t * dma)
{
        if(device && quality && dma){
                device->device_type = AUDIO_MEMORY;
                device->memory_param.total_buffer_len = dma->buff_len;
                device->memory_param.buffer_len_cb = dma->buffer_len_cb;
                device->memory_param.cb = dma->cb;
                device->memory_param.app_ud = dma->ud;
                device->memory_param.dma_channel[0] = dma->dma_l;
                device->memory_param.buff_addr[0] = (uint32_t)dma->buff_l;
                if(quality->channels == AUDIO_CHANNEL_MONO){
                        device->memory_param.stereo = false;
                } else {
                        device->memory_param.stereo = true;
                        device->memory_param.dma_channel[1] = dma->dma_r;
                        device->memory_param.buff_addr[1] = (uint32_t)dma->buff_r;
                }
                device->memory_param.bits_depth = quality->bits;
                device->memory_param.sample_rate = quality->freq;
        }
        return device;
}
