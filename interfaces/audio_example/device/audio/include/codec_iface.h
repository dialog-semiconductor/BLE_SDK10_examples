/**
 ****************************************************************************************
 *
 * @file codec_iface.h
 *
 * @brief Interface with the DA721X codecs
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _CODEC_IFACE_H_
#define _CODEC_IFACE_H_

#define CODEC_SAMPLE_RATE  (16000)   /* Sample rate of PCM CODEC */

/**
 * @brief initialize codec
 * @return OK or error
 */
int codec_init();
/**
 * @brief start audio playback
 * @return OK or error
 */
int codec_playback();
/**
 * @brief start audio recording
 * @return OK or error
 */
int codec_record();
/**
 * @brief stop playback
 * @return OK or error
 */
int codec_stop_playback();
/**
 * @brief stop recordingc
 * @return OK or error
 */
int codec_stop_recording();
/**
 * @brief shutdown codec, releasing resources
 * @return OK or error
 */
int codec_shutdown();
/**
 * @brief resume codec from shutdown, re-acquiring resources
 * @return OK or error
 */
int codec_resume();


#endif // _CODEC_IFACE_H_
