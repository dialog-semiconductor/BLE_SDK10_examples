/**
 ****************************************************************************************
 *
 * @file audio_iface.h
 *
 * @brief Interface with the Audio Processing Unit
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _AUDIO_IFACE_H_
#define _AUDIO_IFACE_H_

#define AUDIO_DMA_OUT_L                 (0)// Channels 0, 2, 4 or 6 must be used for SRC output
#define AUDIO_DMA_OUT_R                 (2)
#define AUDIO_DMA_IN_L                  (1)// Channels 1, 3, 5 or 7 must be used for SRC input
#define AUDIO_DMA_IN_R                  (3)

#define AUDIO_CHANNEL_MONO              (1)
#define AUDIO_CHANNEL_STEREO            (2)

#define AUDIO_BUFFER_LINEAR             (0)
#define AUDIO_BUFFER_CIRCULAR           (1)

#define PDM_FREQ_500kHZ                 (500000) /* Frequency of PDM mic at 0.5 MHz */
#define PDM_FREQ_2MHZ                   (2000000) /* Frequency of PDM mic at 2 MHz */
#define PDM_FREQ_3MHZ                   (3072000) /* Frequency of PDM mic at 3 MHz */
#define RECORDING_SAMPLE_RATE           (8000)    /* Sample rate of PCM memory file */
#define PCM_BIT_DEPTH                   (16)      /* Bit depth of PCM at 16 */

/**
 * @brief Audio quality
 */
typedef struct {
        uint32_t freq; //samples per sec
        uint8_t bits;  //bits per channel
        uint8_t channels;
} audio_quality_t;

/**
 * @brief Audio DMA configuration
 */
typedef struct {
        uint8_t dma_l; //left channel dma
        uint8_t dma_r; //right channel dma
        uint8_t * buff_l; //left channel buffer address
        uint8_t * buff_r; //right channel buffer address
        uint32_t buff_len; //total buffer length in bytes
        uint32_t buffer_len_cb; //number of bytes transmitted before callback
        sys_audio_mgr_buffer_ready_cb cb; //the callback
        void *ud;       //the callback argument
        bool circular;
} audio_dma_config_t;

/**
 * @brief APU input and output devices
 */
typedef struct {
        sys_audio_device_t pcm_in;  //PCM input
        sys_audio_device_t pcm_out; //PCM output
        sys_audio_device_t pdm_in;  //PDM input
        sys_audio_device_t pdm_out; //PDM output
        sys_audio_device_t dma_in;  //memory input
        sys_audio_device_t dma_out; //memory output
} audio_iface_t;

/**
 *@brief Prepare PCM device as I2S interface
 *@param the device
 *@param audio quality
 *@param return pointer to the configured device
 */
sys_audio_device_t * prepare_i2s(sys_audio_device_t * device, const audio_quality_t * quality);

/**
 *@brief Prepare PCM device as generic PCM interface
 *@param the device
 *@param audio quality
 *@param return pointer to the configured device
 */
sys_audio_device_t * prepare_pcm(sys_audio_device_t * device, const audio_quality_t * quality);
/**
 *@brief Prepare PCM device as TDM interface
 *@param the device
 *@param audio quality
 *@param return pointer to the configured device
 */
sys_audio_device_t * prepare_tdm(sys_audio_device_t * device, const audio_quality_t * quality);

/**
 *@brief Prepare PCM device as IOM interface
 *@param the device
 *@param audio quality
 *@param return pointer to the configured device
 */
sys_audio_device_t * prepare_iom(sys_audio_device_t * device, const audio_quality_t * quality);

/**
 *@brief Prepare PDM device
 *@param the PDM
 *@param number of channels (1 = mono, 2 = stero)
 *@param return pointer to the configured device
 */
sys_audio_device_t * prepare_pdm(sys_audio_device_t * device, uint8_t channels);

/**
 *@brief Prepare audio DMA I/O
 *@param the device
 *@param audio quality
 *@param DMA settings
 *@param return pointer to the configured device
 */
sys_audio_device_t * prepare_dma(sys_audio_device_t * device,
                                 const audio_quality_t * quality,
                                 const audio_dma_config_t * dma);

#endif //_AUDIO_IFACE_H_
