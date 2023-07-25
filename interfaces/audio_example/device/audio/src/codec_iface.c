/**
 ****************************************************************************************
 *
 * @file codec_iface.c
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

#include "codec_iface.h"

#include <stdio.h>
#include <osal.h>
#include <hw_clk.h>
#include <ad_i2c.h>
#include "platform_devices.h"
#include "da721x_user_data.h"

#if (DA721X == DA7218)
#include "hal-da7218.h"

int codec_init()
{
        int ok = da721x_i2c_init();
        if (DA721X_OK == ok) {
                ok = da7218_codec_init();
        }
        if (DA721X_OK == ok) {       /* set dai, pll, clk, modes */
               ok = da7218_set_dai_fmt();
        }
        if (DA721X_OK == ok) {
                ok = da7218_set_sample_rate(CODEC_SAMPLE_RATE,CODEC_SAMPLE_RATE);
        }
        return ok;
}

int codec_playback()
{//same sr for adc and dac
        int ok = DA721X_OK;
        if (DA721X_OK == ok) {
                ok =da7218_codec_path_enable(CODEC_DAI_TO_HP);
        }
        if (DA721X_OK == ok) {
                ok = da7218_mute(DA7218_DACL, CODEC_UNMUTE);
        }
        if (DA721X_OK == ok) {
                ok = da7218_mute(DA7218_DACR, CODEC_UNMUTE);
        }
        if (DA721X_OK == ok) {
                ok = da7218_mute(DA7218_HP, CODEC_UNMUTE);
        }
        if (DA721X_OK == ok) {
                ok = da7218_set_vol_dB(DA7218_DACL, 0); // set to 0dB
        }
        if (DA721X_OK == ok){
                ok = da7218_set_vol_dB(DA7218_DACR, 0);
        }
        if (DA721X_OK == ok){
                ok = da7218_set_vol_dB(DA7218_HP, 0);
        }
        da7218_dump_all_registers("playback started:");
        return ok;
}

int codec_record()
{//same sr for adc and dac
        int ok = DA721X_OK;
                /* Init path from MIC to DAI */
#if (DA7218_MIC_INPUT < DA7218_DMIC1)// Analogue MIC
        ok = da7218_codec_path_enable(CODEC_MIC_TO_DAI);
#else // Digital MIC
        ok = da7218_codec_path_enable(CODEC_D_MIC_TO_DAI);
#endif //MIC

        if (DA721X_OK == ok) {
                ok = da7218_mute(DA7218_ADC1, CODEC_UNMUTE);
        }
        if (DA721X_OK == ok) {
                ok = da7218_mute(DA7218_ADC2, CODEC_UNMUTE);
        }
        if (DA721X_OK == ok) {
                ok = da7218_set_vol_dB(DA7218_MIC1, 6); // set to 6dB
        }
        if (DA721X_OK == ok) {
                ok = da7218_set_vol_dB(DA7218_ADC1, 0); // set to 0dB
        }
        if (DA721X_OK == ok) {
                ok = da7218_set_vol_dB(DA7218_ADC2, 0);
        }
        return ok;
}

int codec_stop_playback()
{ //stop should try to stop everything, even on error.
        int ok = da7218_mute(DA7218_DACL, CODEC_MUTE);
        ok += da7218_mute(DA7218_DACR, CODEC_MUTE);
        ok += da7218_mute(DA7218_HP, CODEC_MUTE);
        ok += da7218_codec_path_disable(CODEC_DAI_TO_HP);

        return ok;
}

int codec_stop_recording()
{
        int ok = da7218_mute(DA7218_ADC1, CODEC_MUTE);
        ok += da7218_mute(DA7218_ADC2, CODEC_MUTE);
        ok += da7218_codec_path_disable(CODEC_MIC_TO_DAI);
        ok += da7218_codec_path_disable(CODEC_D_MIC_TO_DAI);

        return ok;
}

int codec_shutdown()
{
        int ok = codec_stop_playback();
        ok += codec_stop_recording();
        ok += da7218_suspend();
        da721x_i2c_deinit();

        return ok;
}

int codec_resume()
{
        int ok = da721x_i2c_init();
        if(DA721X_OK == ok) {
                ok = da7218_resume();
        }
        return ok;
}
#else
#if (DA721X == DA7212)
#include "hal-da7212.h"

static unsigned char _path_ctrl_table[DA7212_CTRL_MAX];

int codec_init()
{
        int ok = da721x_i2c_init();
        OS_ASSERT(DA721X_OK == ok); {
                ok = da7212_codec_init();
        }
        OS_ASSERT(DA721X_OK == ok);{
                /* set dai, pll, clk, modes */
                ok = da7212_set_sr(CODEC_SAMPLE_RATE);
        }
        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_set_dai_pll(CODEC_SAMPLE_RATE);
        }
        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_set_dai_fmt();
        }
        return ok;
}

int codec_playback()
{
        da7212_get_codec_path_status(_path_ctrl_table);
        da7212_get_path_playback_hp(_path_ctrl_table);

        int ok = da7212_codec_path_update(_path_ctrl_table);

        OS_ASSERT(DA721X_OK == ok); {
                ok = da7212_mute(DA7212_DAC_L, CODEC_UNMUTE); // 0: Unmute, 1: Mute
        }
        OS_ASSERT(DA721X_OK == ok); {
                ok = da7212_mute(DA7212_DAC_R, CODEC_UNMUTE);
        }
        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_mute(DA7212_HP_L, CODEC_UNMUTE);
        }
        OS_ASSERT(DA721X_OK == ok); {
                ok = da7212_mute(DA7212_HP_R, CODEC_UNMUTE);
        }
        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_set_vol_dB(DA7212_DAC_L, 0); // set to 0dB
        }
        OS_ASSERT(DA721X_OK == ok); {
                ok = da7212_set_vol_dB(DA7212_DAC_R, 0);
        }
        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_set_vol_dB(DA7212_HP_L, 1); // set to 1dB
        }
        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_set_vol_dB(DA7212_HP_R, 1);
        }
        da7212_dump_all_registers("playback started:");

        return ok;
}

int codec_record()
{
        da7212_get_codec_path_status(_path_ctrl_table);
        da7212_get_path_record_mic1(_path_ctrl_table);

        int ok = da7212_codec_path_update(_path_ctrl_table);


        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_mute(DA7212_ADC_L, CODEC_UNMUTE); // 0: Unmute, 1: Mute
        }
        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_mute(DA7212_ADC_R, CODEC_UNMUTE);
        }
        OS_ASSERT(DA721X_OK == ok);{
                ok = da7212_mute(DA7212_MIC1, CODEC_UNMUTE);
        }
        OS_ASSERT(DA721X_OK == ok); {
                ok = da7212_set_vol_dB(DA7212_ADC_L, 0); // set to 0dB
        }
        OS_ASSERT(DA721X_OK == ok); {
                ok = da7212_set_vol_dB(DA7212_ADC_R, 0);
        }
        OS_ASSERT(DA721X_OK == ok);{
                da7212_set_vol_dB(DA7212_MIC1, 6); // set to 18dB
        }

        return ok;
}

int codec_stop_playback()
{
        int ok = da7212_mute(DA7212_DAC_L, CODEC_MUTE); // 0: Unmute, 1: Mute
        ok += da7212_mute(DA7212_DAC_R, CODEC_MUTE);
        ok += da7212_mute(DA7212_HP_L, CODEC_MUTE);
        ok += da7212_mute(DA7212_HP_R, CODEC_MUTE);

        return ok;
}

int codec_stop_recording()
{
        int ok = da7212_mute(DA7212_ADC_L, CODEC_MUTE); // 0: Unmute, 1: Mute
        ok += da7212_mute(DA7212_ADC_R, CODEC_MUTE);
        ok += da7212_mute(DA7212_MIC1, CODEC_MUTE);

        return ok;
}

int codec_shutdown()
{
        int ok = codec_stop_playback();
        ok += codec_stop_recording();
        da7212_codec_path_clear(_path_ctrl_table);
        ok += da7212_suspend();
        da721x_i2c_deinit();

        return ok;
}

int codec_resume()
{

        return codec_init();
}
#endif //DA7212
#endif //DA721X

