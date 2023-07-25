/**
 ****************************************************************************************
 *
 * @file codec_iface.h
 *
 * @brief Interface with the DA721X codecs
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
