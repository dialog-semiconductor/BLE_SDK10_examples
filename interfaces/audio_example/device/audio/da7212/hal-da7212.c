/*
 * hal-da7212.c - DA7212 audio CODEC Hardware Abstraction Layer functions
 *
 * Copyright (c) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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
 */

#include "hal-da7212.h"
#include "da7212_user_data.h"
#include "da721x_user_data.h"


/* An example for how to configure use cases..
 *
 * // declaration for path table variable.
 * u8 path_ctrl_table[DA7212_CTRL_MAX];
 *
 * // Set clock related functions
 * da7212_set_sr(CONFIG_SAMPLE_RATE);
 * da7212_set_dai_pll(CONFIG_SAMPLE_RATE);
 * da7212_set_dai_fmt();
 *
 * // Get current STATUS and set the Playback(spk) CASE.
 * da7212_get_codec_path_status(path_ctrl_table);
 * da7212_get_path_playback_spk(path_ctrl_table);
 *
 * // Set to UNMUTE and GAINs
 * da7212_mute(DA7212_DAC_R, CODEC_UNMUTE);
 * da7212_mute(DA7212_LINE, CODEC_UNMUTE);
 * da7212_set_vol_dB(DA7212_DAC_R, CODEC_SET_0dB);
 * da7212_set_vol_dB(DA7212_LINE, 2);
 *
 * da7212_codec_path_update(path_ctrl_table);
 *
 */

/*
 * Global Variables
 */
static u8 da7212_rout_tbl[DA7212_CTRL_MAX];

const struct codec_scr_type da7212_reg_defaults[] = {
        { DA7212_DIG_ROUTING_DAI, 0x10 },
        { DA7212_SR, 0x0A },
        { DA7212_REFERENCES, 0x80 },
        { DA7212_PLL_FRAC_TOP, 0x00 },
        { DA7212_PLL_FRAC_BOT, 0x00 },
        { DA7212_PLL_INTEGER, 0x20 },
        { DA7212_PLL_CTRL, 0x0C },
        { DA7212_DAI_CLK_MODE, 0x01 },
        { DA7212_DAI_CTRL, 0x08 },
        { DA7212_DIG_ROUTING_DAC, 0x32 },
        { DA7212_AUX_L_GAIN, 0x35 },
        { DA7212_AUX_R_GAIN, 0x35 },
        { DA7212_MIXIN_L_SELECT, 0x00 },
        { DA7212_MIXIN_R_SELECT, 0x00 },
        { DA7212_MIXIN_L_GAIN, 0x03 },
        { DA7212_MIXIN_R_GAIN, 0x03 },
        { DA7212_ADC_L_GAIN, 0x6F },
        { DA7212_ADC_R_GAIN, 0x6F },
        { DA7212_ADC_FILTERS1, 0x80 },
        { DA7212_MIC_1_GAIN, 0x01 },
        { DA7212_MIC_2_GAIN, 0x01 },
        { DA7212_DAC_FILTERS5, 0x00 },
        { DA7212_DAC_FILTERS2, 0x88 },
        { DA7212_DAC_FILTERS3, 0x88 },
        { DA7212_DAC_FILTERS4, 0x08 },
        { DA7212_DAC_FILTERS1, 0x80 },
        { DA7212_DAC_L_GAIN, 0x6F },
        { DA7212_DAC_R_GAIN, 0x6F },
        { DA7212_CP_CTRL, 0x61 },
        { DA7212_HP_L_GAIN, 0x39 },
        { DA7212_HP_R_GAIN, 0x39 },
        { DA7212_LINE_GAIN, 0x30 },
        { DA7212_MIXOUT_L_SELECT, 0x00 },
        { DA7212_MIXOUT_R_SELECT, 0x00 },
        { DA7212_SYSTEM_MODES_INPUT, 0x00 },
        { DA7212_SYSTEM_MODES_OUTPUT, 0x00 },
        { DA7212_AUX_L_CTRL, 0x44 },
        { DA7212_AUX_R_CTRL, 0x44 },
        { DA7212_MICBIAS_CTRL, 0x11 },
        { DA7212_MIC_1_CTRL, 0x40 },
        { DA7212_MIC_2_CTRL, 0x40 },
        { DA7212_MIXIN_L_CTRL, 0x40 },
        { DA7212_MIXIN_R_CTRL, 0x40 },
        { DA7212_ADC_L_CTRL, 0x40 },
        { DA7212_ADC_R_CTRL, 0x40 },
        { DA7212_DAC_L_CTRL, 0x48 },
        { DA7212_DAC_R_CTRL, 0x40 },
        { DA7212_HP_L_CTRL, 0x41 },
        { DA7212_HP_R_CTRL, 0x40 },
        { DA7212_LINE_CTRL, 0x40 },
        { DA7212_MIXOUT_L_CTRL, 0x00 },
        { DA7212_MIXOUT_R_CTRL, 0x10 },
        { DA7212_LDO_CTRL, 0x00 },
        { DA7212_IO_CTRL, 0x00 },
        { DA7212_GAIN_RAMP_CTRL, 0x00},
        { DA7212_MIC_CONFIG, 0x00 },
        { DA7212_PC_COUNT, 0x00 },
        { DA7212_CP_VOL_THRESHOLD1, 0x32 },
        { DA7212_CP_DELAY, 0x95 },
        { DA7212_CP_DETECTOR, 0x00 },
        { DA7212_DAI_OFFSET, 0x00 },
        { DA7212_DIG_CTRL, 0x00 },
        { DA7212_ALC_CTRL2, 0x00 },
        { DA7212_ALC_CTRL3, 0x00 },
        { DA7212_ALC_NOISE, 0x3F },
        { DA7212_ALC_TARGET_MIN, 0x3F },
        { DA7212_ALC_TARGET_MAX, 0x00 },
        { DA7212_ALC_GAIN_LIMITS, 0xFF },
        { DA7212_ALC_ANA_GAIN_LIMITS, 0x71 },
        { DA7212_ALC_ANTICLIP_CTRL, 0x00 },
        { DA7212_ALC_ANTICLIP_LEVEL, 0x00 },
        { DA7212_ALC_OFFSET_MAN_M_L, 0x00 },
        { DA7212_ALC_OFFSET_MAN_U_L, 0x00 },
        { DA7212_ALC_OFFSET_MAN_M_R, 0x00 },
        { DA7212_ALC_OFFSET_MAN_U_R, 0x00 },
        { DA7212_ALC_CIC_OP_LVL_CTRL, 0x00 },
        { DA7212_DAC_NG_SETUP_TIME, 0x00 },
        { DA7212_DAC_NG_OFF_THRESHOLD, 0x00 },
        { DA7212_DAC_NG_ON_THRESHOLD, 0x00 },
        { DA7212_DAC_NG_CTRL, 0x00 },
};

/* Scripts */

const struct codec_scr_type da7212_codec_init_scr[] = {
        {DA7212_REFERENCES, 0x08},
        {DA7212_GAIN_RAMP_CTRL, 0x00},
        {0xF0, 0x8B},
        {0xF2, 0x03},
        {0xF0, 0x00},
        {CODEC_mDELAY, 30},
        {DA7212_PLL_CTRL, 0x02},
        {DA7212_IO_CTRL, HAL_IO_LVL},
        {DA7212_LDO_CTRL, 0x80 | HAL_LDO_LVL},
        {SEQ_END, 0}
};

const struct codec_scr_mask_type da7212_pdata_setup[] = {
        /* Mic1,2 P selected */
        {
                DA7212_MIC_1_CTRL,
                DA7212_MIC_IN_SEL_MASK,
                MIC_P_SEL << DA7212_MIC_AMP_IN_SEL_SHIFT
        },
        {
                DA7212_MIC_2_CTRL,
                DA7212_MIC_IN_SEL_MASK,
                MIC_P_SEL << DA7212_MIC_AMP_IN_SEL_SHIFT
        },
        /* MIC Bias1,2 level */
        {
                DA7212_MICBIAS_CTRL,
                DA7212_MICBIAS1_LEVEL_MASK |
                DA7212_MICBIAS2_LEVEL_MASK,
                DA7212_MICBIAS_2_5V | DA7212_MICBIAS_2_5V<<4
        },
        /* CP */
        {
                DA7212_CP_DELAY,
                0xff,
                HAL_CPDELAY
        },
        {
                DA7212_CP_VOL_THRESHOLD1,
                0xff,
                HAL_CP_VOL_THRESH
        },
        {
                DA7212_CP_CTRL,
                0x7f,
                0x71
        },
        {
                SEQ_END,
                0
        },
};

const struct codec_path_type da7212_path_tbl_ctrl[] = {
        [DA7212_MIC1]        = {DA7212_MIC_1_CTRL, DA7212_MIC_AMP_EN, DA7212_MIC_AMP_EN, 0x00},
        [DA7212_MIC2]        = {DA7212_MIC_2_CTRL, DA7212_MIC_AMP_EN, DA7212_MIC_AMP_EN, 0x00},
        [DA7212_MIXIN_L] = {DA7212_MIXIN_L_CTRL, 0xc8, 0x88, 0x40},
        [DA7212_MIXIN_R] = {DA7212_MIXIN_R_CTRL, 0xc8, 0x88, 0x40},
        [DA7212_AUX_L]        = {DA7212_AUX_L_CTRL, DA7212_AUX_AMP_EN, DA7212_AUX_AMP_EN, 0x00},
        [DA7212_AUX_R]        = {DA7212_AUX_R_CTRL, DA7212_AUX_AMP_EN, DA7212_AUX_AMP_EN, 0x00},
        [DA7212_ADC_L]        = {DA7212_ADC_L_CTRL, 0xA0, 0xA0, 0x00},
        [DA7212_ADC_R]        = {DA7212_ADC_R_CTRL, 0xA0, 0xA0, 0x00},
        [DA7212_DAC_L]        = {DA7212_DAC_L_CTRL, DA7212_DAC_EN, DA7212_DAC_EN, 0x00},
        [DA7212_DAC_R]        = {DA7212_DAC_R_CTRL, DA7212_DAC_EN, DA7212_DAC_EN, 0x00},
        [DA7212_MIXOUT_L] = {DA7212_MIXOUT_L_CTRL, 0xC8, 0x88, 0x40},
        [DA7212_MIXOUT_R] = {DA7212_MIXOUT_R_CTRL, 0xC8, 0x88, 0x40},
        [DA7212_HP_L]        = {DA7212_HP_L_CTRL, 0xA8, 0xA8, 0x00},
        [DA7212_HP_R]        = {DA7212_HP_R_CTRL, 0xA8, 0xA8, 0x00},
        [DA7212_LINE]        = {DA7212_LINE_CTRL, 0xA8, 0xA8, 0x00},
};

/*
 *  I/O control interface functions.
 */


static int da7212_execute_script(const struct codec_scr_type script[])
{
        int i;

        for (i = 0; script[i].reg != SEQ_END; i++) {
                if (script[i].reg == CODEC_mDELAY) {
                        mdelay(script[i].val);
                } else if (da721x_i2c_reg_write((u8)script[i].reg, script[i].val)) {
                        diag_err("codec script error in reg write\n");
                        return -EIO;
                }
        }
        return 0;
}

static int da7212_execute_script_mask(const struct codec_scr_mask_type script[])
{
        int i;

        for (i = 0; script[i].reg != SEQ_END; i++) {
                if (script[i].reg == CODEC_mDELAY) {
                        mdelay(script[i].val);
                } else if (da721x_i2c_update_bits((u8)script[i].reg,
                        script[i].mask, script[i].val)) {
                        diag_err("codec mask script error in reg write\n");
                        return -EIO;
                }
        }
        return 0;
}

int da7212_mute(enum hal_controls_t controls, int mute)
{
        u8 ctrl_reg = 0;

        diag_info("da7212 mute control(%s)\r\n",
                mute==1?"mute":mute==0?"Unmute":"unknown");
        switch(controls) {
        case DA7212_MIC1:
                ctrl_reg = DA7212_MIC_1_CTRL;
                break;
        case DA7212_MIC2:
                ctrl_reg = DA7212_MIC_2_CTRL;
                break;
        case DA7212_MIXIN_L:
                ctrl_reg = DA7212_MIXIN_L_CTRL;
                break;
        case DA7212_MIXIN_R:
                ctrl_reg = DA7212_MIXIN_R_CTRL;
                break;
        case DA7212_AUX_L:
                ctrl_reg = DA7212_AUX_L_CTRL;
                break;
        case DA7212_AUX_R:
                ctrl_reg = DA7212_AUX_R_CTRL;
                break;
        case DA7212_ADC_L:
                ctrl_reg = DA7212_ADC_L_CTRL;
                break;
        case DA7212_ADC_R:
                ctrl_reg = DA7212_ADC_R_CTRL;
                break;
        case DA7212_DAC_L:
                ctrl_reg = DA7212_DAC_L_CTRL;
                break;
        case DA7212_DAC_R:
                ctrl_reg = DA7212_DAC_R_CTRL;
                break;
        case DA7212_MIXOUT_L:
        case DA7212_MIXOUT_R:
                diag_info("MIXOUT has no mute function\n");
                return 0;
        case DA7212_HP_L:
                ctrl_reg = DA7212_HP_L_CTRL;
                break;
        case DA7212_HP_R:
                ctrl_reg = DA7212_HP_R_CTRL;
                break;
        case DA7212_LINE:
                ctrl_reg = DA7212_LINE_CTRL;
                break;
        default:
                diag_info("Invalid Item(%d)\n", controls);
                return -EINVAL;
        }

        return da721x_i2c_update_bits(ctrl_reg,
                DA7212_MUTE_EN, (mute ? DA7212_MUTE_EN : 0));
}

int hal_volume_check(enum hal_controls_t amp, int vol) 
{
        vol *= 100;

        switch(amp) {
        case DA7212_MIC1:
        case DA7212_MIC2:
                if (vol < MIC_PGA_MIN || MIC_PGA_MAX < vol)
                        return -EINVAL;
                break;
        case DA7212_AUX_L:
        case DA7212_AUX_R:
                if (vol < 100 || AUX_PGA_MAX < vol)
                        return -EINVAL;
                break;
        case DA7212_MIXIN_L:
        case DA7212_MIXIN_R:
                if (vol < MIXIN_PGA_MIN || MIXIN_PGA_MAX < vol)
                        return -EINVAL;
                break;
        case DA7212_ADC_L:
        case DA7212_ADC_R:
                if (vol < ADC_PGA_MIN || ADC_PGA_MAX < vol)
                        return -EINVAL;
                break;
        case DA7212_DAC_L:
        case DA7212_DAC_R:
                if (vol < DAC_PGA_MIN || DAC_PGA_MAX < vol)
                        return -EINVAL;
                break;
        case DA7212_HP_L:
        case DA7212_HP_R:
                if (vol < HP_PGA_MIN || HP_PGA_MAX < vol)
                        return -EINVAL;
                break;
        case DA7212_LINE:
                if (vol < SPK_PGA_MIN || SPK_PGA_MAX < vol)
                        return -EINVAL;
                break;
        default:
                diag_info("Invalid Item\n");
                return -EINVAL;
        }
        return 0;
}

int da7212_set_vol_dB(enum hal_controls_t amp, int vol)
{
        if(hal_volume_check(amp, vol) < 0) {
                return -EINVAL;
        }

        diag_info("PGA(%d):%ddB\n",amp, vol);

        vol *= 100;
        switch(amp) {
        case DA7212_MIC1:
                da721x_i2c_update_bits(0x39, 0x07, (vol - MIC_PGA_MIN) /MIC_PGA_STEP);
                break;
        case DA7212_MIC2:
                da721x_i2c_update_bits(0x3A, 0x07, (vol - MIC_PGA_MIN) /MIC_PGA_STEP);
                break;
        case DA7212_AUX_L:
                da721x_i2c_update_bits(0x30, 0x3F, 17+(vol - AUX_PGA_MIN) /AUX_PGA_STEP);
                break;
        case DA7212_AUX_R:
                da721x_i2c_update_bits(0x31, 0x3F, 17+(vol - AUX_PGA_MIN) /AUX_PGA_STEP);
                break;
        case DA7212_MIXIN_L:
                da721x_i2c_update_bits(0x34, 0x0F, (vol - MIXIN_PGA_MIN) /MIXIN_PGA_STEP);
                break;
        case DA7212_MIXIN_R:
                da721x_i2c_update_bits(0x35, 0x0F, (vol - MIXIN_PGA_MIN) /MIXIN_PGA_STEP);
                break;
        case DA7212_ADC_L:
                da721x_i2c_update_bits(0x36, 0x7F, 8+(vol - ADC_PGA_MIN) /DIGITAL_PGA_STEP);
                break;
        case DA7212_ADC_R:
                da721x_i2c_update_bits(0x37, 0x7F, 8+(vol - ADC_PGA_MIN) /DIGITAL_PGA_STEP);
                break;
        case DA7212_DAC_L:
                da721x_i2c_update_bits(0x45, 0x7F, 8+(vol - DAC_PGA_MIN )/DIGITAL_PGA_STEP);
                break;
        case DA7212_DAC_R:
                da721x_i2c_update_bits(0x46, 0x7F, 8+(vol - DAC_PGA_MIN) /DIGITAL_PGA_STEP);
                break;
        case DA7212_HP_L:
                da721x_i2c_update_bits(0x48, 0x3F, (vol - HP_PGA_MIN) /OUT_PGA_STEP);
                break;
        case DA7212_HP_R:
                da721x_i2c_update_bits(0x49, 0x3F, (vol - HP_PGA_MIN) /OUT_PGA_STEP);
                break;
        case DA7212_LINE:
                da721x_i2c_update_bits(0x4A, 0x3F, (vol - SPK_PGA_MIN) /OUT_PGA_STEP);
                break;
        default:
                break;
        }
        return 0;
}


/* Supported PLL input frequencies are 5MHz - 54MHz. */
int da7212_set_dai_pll(int sampling_rate)
{
        u8 indiv_bits = 0;
        u8 pll_ctrl = 0;
        u8 pll_frac_top = 0;
        u8 pll_frac_bot= 0;
        u8 pll_integer = 0;
        /* Reset PLL configuration */

        da721x_i2c_reg_write(DA7212_PLL_CTRL, 0x04);

        /* Workout input divider based on MCLK rate */
        if (DA7212_MCLK < 5000000) {
                goto pll_err;
        } else if (DA7212_MCLK <= 10000000) {
                indiv_bits = DA7212_PLL_INDIV_5_10_MHZ;

        } else if (DA7212_MCLK <= 20000000) {
                indiv_bits = DA7212_PLL_INDIV_10_20_MHZ;

        } else if (DA7212_MCLK <= 40000000) {
                indiv_bits = DA7212_PLL_INDIV_20_40_MHZ;

        } else if (DA7212_MCLK <= 54000000) {
                indiv_bits = DA7212_PLL_INDIV_40_54_MHZ;

        } else {
                goto pll_err;
        }
        pll_ctrl |= indiv_bits;

        /* SRM enabled when codec is slave.
         */
        if (AP_I2S_MASTER == DA7212_CODEC_SLAVE) {
                pll_ctrl |= DA7212_PLL_SRM_EN;
                pll_frac_top = 0x0D;
                pll_frac_bot = 0xFA;
                pll_integer = 0x1F;
        }else{
                switch (sampling_rate) {
                case DA7212_SR_8000:
                case DA7212_SR_12000:
                case DA7212_SR_16000:
                case DA7212_SR_24000:
                case DA7212_SR_32000:
                case DA7212_SR_48000:
                case DA7212_SR_96000:
                        pll_frac_top = 0x18;
                        pll_frac_bot = 0x93;
                        pll_integer = 0x20;
                        break;
                case DA7212_SR_11025:
                case DA7212_SR_22050:
                case DA7212_SR_44100:
                case DA7212_SR_88200:
                        pll_frac_top = 0x03;
                        pll_frac_bot = 0x61;
                        pll_integer = 0x1E;
                        break;
                default:
                        diag_info("codec_set_sample_rate: invalid parameter. \n");
                        return -1;
                }
        }

        /* Write PLL dividers */
        da721x_i2c_reg_write(DA7212_PLL_FRAC_TOP, pll_frac_top);
        da721x_i2c_reg_write(DA7212_PLL_FRAC_BOT, pll_frac_bot);
        da721x_i2c_reg_write(DA7212_PLL_INTEGER, pll_integer);

        /* Enable MCLK squarer if required */
#if (DA7212_MCLK_SQR_EN)
        pll_ctrl |= DA7212_PLL_MCLK_SQR_EN;
#endif
        /* Enable PLL */
        pll_ctrl |= DA7212_PLL_EN;

        da721x_i2c_reg_write(DA7212_PLL_CTRL, pll_ctrl);
        diag_info("da7212_set_dai_pll\n");
        mdelay(100);

        return 0;

        pll_err:
        diag_info("Unsupported PLL input frequency %d\n",DA7212_MCLK);
        return -1;
}

int da7212_set_sr(int sampling_rate) {
        u8 fs;

        switch (sampling_rate) {
        case 8000:
                fs = DA7212_SR_8000;
                break;
        case 11025:
                fs = DA7212_SR_11025;
                break;
        case 12000:
                fs = DA7212_SR_12000;
                break;
        case 16000:
                fs = DA7212_SR_16000;
                break;
        case 22050:
                fs = DA7212_SR_22050;
                break;
        case 24000:
                fs = DA7212_SR_24000;
                break;
        case 32000:
                fs = DA7212_SR_32000;
                break;
        case 44100:
                fs = DA7212_SR_44100;
                break;
        case 48000:
                fs = DA7212_SR_48000;
                break;
        case 88200:
                fs = DA7212_SR_88200;
                break;
        case 96000:
                fs = DA7212_SR_96000;
                break;
        default:
                diag_info("codec_set_sample_rate: invalid parameter. \n");
                return -1;
        }

        if (da721x_i2c_reg_write(DA7212_SR, fs) < 0) {
                diag_info("codec_set_sample_rate: error in write reg .\n");
                return-1;
        }
        return 0;
}

/*
 * This HAL configure as below for DAI format
 * 16 bit Word Length, I2S.
 */
int da7212_set_dai_fmt(void)
{
        u8 dai_clk_mode = 0;
        u8 dai_ctrl = 0;
        u8 dai_ctrl_mask = 0;
        u8 pll_ctrl = 0;
        u8 pll_status = 0;
        u8 srm_lock = 0;
        int i = 0;

        diag_info(" Start to get ready for working DAI with PLL\n");

        /* Set Codec Master/Slave, Polarity & clk */
#if (AP_I2S_MASTER == DA7212_CODEC_MASTER)
        /* Da7212 Codec is Master */
        dai_clk_mode |= DA7212_DAI_CLK_EN_MASTER_MODE;
        dai_clk_mode |= DA7212_DAI_BCLKS_PER_WCLK_32;
#else
        /* Da7212 Codec is slave */
        dai_clk_mode |= DA7212_DAI_CLK_EN_SLAVE_MODE;
        dai_clk_mode |= DA7212_DAI_BCLKS_PER_WCLK_64;
#endif
        da721x_i2c_reg_write(DA7212_DAI_CLK_MODE, dai_clk_mode);

        /* set DAI Word Length */
        dai_ctrl |= DA7212_DAI_WORD_LENGTH_S16_LE;
        dai_ctrl_mask |= DA7212_DAI_WORD_LENGTH_MASK;

        /* set DAI DATA Format */
        dai_ctrl |= DA7212_DAI_FMT;
        dai_ctrl_mask |= DA7212_DAI_FORMAT_MASK;

        /* set DAI DATA Format */
        dai_ctrl |= 1 << DA7212_DAI_OE_SHIFT;
        dai_ctrl_mask |= DA7212_DAI_OE_MASK;

        /* Enable DAI Ctrl */
        dai_ctrl |= 1 << DA7212_DAI_EN_SHIFT;
        dai_ctrl_mask |= DA7212_DAI_EN_MASK;

        da721x_i2c_update_bits(DA7212_DAI_CTRL, dai_ctrl_mask,
                dai_ctrl);

        pll_ctrl = da721x_i2c_reg_read(DA7212_PLL_CTRL);

        if (!(pll_ctrl & DA7212_PLL_SRM_EN)) {
                diag_info(" SRM is not enabled\n");
                return 0;
        }
#if (AP_I2S_MASTER == DA7212_CODEC_SLAVE)

        diag_info("** Current PLL status = %x\n", da721x_i2c_reg_read(DA7212_PLL_STATUS));

        /* Check SRM has locked */
        do {
                pll_status = 0;
                pll_status = da721x_i2c_reg_read(DA7212_PLL_STATUS);
                if (pll_status < 0)
                        return -EINVAL;

                if (pll_status & DA7212_PLL_SRM_STATUS_SRM_LOCK) {
                        srm_lock = 1;
                } else {
                        ++i;
                        mdelay(50);
                }
        } while ((i < DA7212_SRM_CHECK_RETRIES) & (!srm_lock));
#endif
        diag_info(" Time for SRM LOCK = %dms\n", i*50);
        diag_info(" Ready to work DAI with PLL\n");

        return 0;
}

/*
 * ALC
 */

static int da7212_get_alc_data(u8 reg_val)
{
        int mid_data, top_data;
        int sum = 0;
        u8 iteration;

        for (iteration = 0; iteration < DA7212_ALC_AVG_ITERATIONS;
                iteration++) {
                /* Select the left or right channel and capture data */
                da721x_i2c_reg_write(DA7212_ALC_CIC_OP_LVL_CTRL, reg_val);

                /* Select middle 8 bits for read back from data register */
                da721x_i2c_reg_write(DA7212_ALC_CIC_OP_LVL_CTRL,
                        reg_val | DA7212_ALC_DATA_MIDDLE);
                mid_data = da721x_i2c_reg_read(DA7212_ALC_CIC_OP_LVL_DATA);

                /* Select top 8 bits for read back from data register */
                da721x_i2c_reg_write(DA7212_ALC_CIC_OP_LVL_CTRL,
                        reg_val | DA7212_ALC_DATA_TOP);
                top_data = da721x_i2c_reg_read(DA7212_ALC_CIC_OP_LVL_DATA);

                sum += ((mid_data << 8) | (top_data << 16));
        }

        return sum / DA7212_ALC_AVG_ITERATIONS;
}

static void da7212_alc_calib_man(void)
{
        u8 reg_val;
        int avg_left_data, avg_right_data, offset_l, offset_r;

        /* Calculate average for Left and Right data */
        /* Left Data */
        avg_left_data =
                da7212_get_alc_data(DA7212_ALC_CIC_OP_CHANNEL_LEFT);
        /* Right Data */
        avg_right_data =
                da7212_get_alc_data(DA7212_ALC_CIC_OP_CHANNEL_RIGHT);

        /* Calculate DC offset */
        offset_l = -avg_left_data;
        offset_r = -avg_right_data;

        reg_val = (offset_l & DA7212_ALC_OFFSET_15_8) >> 8;
        da721x_i2c_reg_write(DA7212_ALC_OFFSET_MAN_M_L, reg_val);
        reg_val = (offset_l & DA7212_ALC_OFFSET_19_16) >> 16;
        da721x_i2c_reg_write(DA7212_ALC_OFFSET_MAN_U_L, reg_val);

        reg_val = (offset_r & DA7212_ALC_OFFSET_15_8) >> 8;
        da721x_i2c_reg_write(DA7212_ALC_OFFSET_MAN_M_R, reg_val);
        reg_val = (offset_r & DA7212_ALC_OFFSET_19_16) >> 16;
        da721x_i2c_reg_write(DA7212_ALC_OFFSET_MAN_U_R, reg_val);

        /* Enable analog/digital gain mode & offset cancellation */
        da721x_i2c_update_bits(DA7212_ALC_CTRL1,
                DA7212_ALC_OFFSET_EN | DA7212_ALC_SYNC_MODE,
                DA7212_ALC_OFFSET_EN | DA7212_ALC_SYNC_MODE);
}

#ifdef ALC_CALIB_AUTO
static void da7212_alc_calib_auto(void)
{
        u8 alc_ctrl1;

        /* Begin auto calibration and wait for completion */
        da721x_i2c_update_bits(DA7212_ALC_CTRL1, DA7212_ALC_AUTO_CALIB_EN,
                DA7212_ALC_AUTO_CALIB_EN);
        do {
                alc_ctrl1 = da721x_i2c_reg_read(DA7212_ALC_CTRL1);
        } while (alc_ctrl1 & DA7212_ALC_AUTO_CALIB_EN);

        /* If auto calibration fails, fall back to digital gain only mode */
        if (alc_ctrl1 & DA7212_ALC_CALIB_OVERFLOW) {
                diag_info("ALC auto calibration failed with overflow\r\n");
                da721x_i2c_update_bits(DA7212_ALC_CTRL1,
                        DA7212_ALC_OFFSET_EN | DA7212_ALC_SYNC_MODE,
                        0);
        } else {
                /* Enable analog/digital gain mode & offset cancellation */
                da721x_i2c_update_bits(DA7212_ALC_CTRL1,
                        DA7212_ALC_OFFSET_EN | DA7212_ALC_SYNC_MODE,
                        DA7212_ALC_OFFSET_EN | DA7212_ALC_SYNC_MODE);
        }

}
#endif //ALC_CALIB_AUTO

int da7212_alc_calib(void)
{
        u8 adc_l_ctrl, adc_r_ctrl;
        u8 mixin_l_sel, mixin_r_sel;
        u8 mic_1_ctrl, mic_2_ctrl;

        /* Save current values from ADC control registers */
        adc_l_ctrl = da721x_i2c_reg_read(DA7212_ADC_L_CTRL);
        adc_r_ctrl = da721x_i2c_reg_read(DA7212_ADC_R_CTRL);

        /* Save current values from MIXIN_L/R_SELECT registers */
        mixin_l_sel = da721x_i2c_reg_read(DA7212_MIXIN_L_SELECT);
        mixin_r_sel = da721x_i2c_reg_read(DA7212_MIXIN_R_SELECT);

        /* Save current values from MIC control registers */
        mic_1_ctrl = da721x_i2c_reg_read(DA7212_MIC_1_CTRL);
        mic_2_ctrl = da721x_i2c_reg_read(DA7212_MIC_2_CTRL);

        /* Enable ADC Left and Right */
        da721x_i2c_update_bits(DA7212_ADC_L_CTRL, DA7212_ADC_EN,
                DA7212_ADC_EN);
        da721x_i2c_update_bits(DA7212_ADC_R_CTRL, DA7212_ADC_EN,
                DA7212_ADC_EN);

        /* Enable MIC paths */
        da721x_i2c_update_bits(DA7212_MIXIN_L_SELECT,
                DA7212_MIXIN_L_MIX_SELECT_MIC_1 |
                DA7212_MIXIN_L_MIX_SELECT_MIC_2,
                DA7212_MIXIN_L_MIX_SELECT_MIC_1 |
                DA7212_MIXIN_L_MIX_SELECT_MIC_2);
        da721x_i2c_update_bits(DA7212_MIXIN_R_SELECT,
                DA7212_MIXIN_R_MIX_SELECT_MIC_2 |
                DA7212_MIXIN_R_MIX_SELECT_MIC_1,
                DA7212_MIXIN_R_MIX_SELECT_MIC_2 |
                DA7212_MIXIN_R_MIX_SELECT_MIC_1);

        /* Mute MIC PGAs */
        da721x_i2c_update_bits(DA7212_MIC_1_CTRL, DA7212_MUTE_EN,
                DA7212_MUTE_EN);
        da721x_i2c_update_bits(DA7212_MIC_2_CTRL, DA7212_MUTE_EN,
                DA7212_MUTE_EN);

        /* Perform calibration */
#ifdef DA7212_CAL_AUTO
        da7212_alc_calib_auto();
#else
        da7212_alc_calib_man();
#endif

        /* Restore MIXIN_L/R_SELECT registers to their original states */
        da721x_i2c_reg_write(DA7212_MIXIN_L_SELECT, mixin_l_sel);
        da721x_i2c_reg_write(DA7212_MIXIN_R_SELECT, mixin_r_sel);

        /* Restore ADC control registers to their original states */
        da721x_i2c_reg_write(DA7212_ADC_L_CTRL, adc_l_ctrl);
        da721x_i2c_reg_write(DA7212_ADC_R_CTRL, adc_r_ctrl);

        /* Restore original values of MIC control registers */
        da721x_i2c_reg_write(DA7212_MIC_1_CTRL, mic_1_ctrl);
        da721x_i2c_reg_write(DA7212_MIC_2_CTRL, mic_2_ctrl);
        return 0;
}

void da7212_cp_control(u8 seton)
{
        da721x_i2c_update_bits(DA7212_CP_CTRL,
                DA7212_CP_EN_MASK, seton << DA7212_CP_EN_SHIFT);
}

void da7212_micbias1_control(u8 seton)
{
        da721x_i2c_update_bits(DA7212_MICBIAS_CTRL,
                DA7212_MICBIAS1_EN, seton << DA7212_MICBIAS1_EN_SHIFT);
}

void da7212_micbias2_control(u8 seton)
{
        da721x_i2c_update_bits(DA7212_MICBIAS_CTRL,
                DA7212_MICBIAS2_EN, seton << DA7212_MICBIAS2_EN_SHIFT);
}

int da7212_codec_path_update(u8 *amp_tbl)
{
        int i;
        u8 val;
        diag_info("codec path enable for\r\n");

        if (amp_tbl[DA7212_HP_L] == TRUE
                || amp_tbl[DA7212_HP_R] == TRUE)
                da7212_cp_control(1);
        else
                da7212_cp_control(0);

        if (amp_tbl[DA7212_MIC1] == TRUE)
                da7212_micbias1_control(1);
        else
                da7212_micbias1_control(0);

        if (amp_tbl[DA7212_MIC2] == TRUE)
                da7212_micbias2_control(1);
        else
                da7212_micbias2_control(0);

        for(i = 0; i < DA7212_CTRL_MAX; i++) {

                if(da7212_rout_tbl[i] == amp_tbl[i])
                        continue;

                da7212_rout_tbl[i] = amp_tbl[i];
                diag_info("[%d] = %d\r\n", i, da7212_rout_tbl[i]);

                if (amp_tbl[i] == HAL_AMP_ON)
                        val = da7212_path_tbl_ctrl[i].turn_on;
                else
                        val = da7212_path_tbl_ctrl[i].turn_off;

                if (da721x_i2c_update_bits((u8)da7212_path_tbl_ctrl[i].reg,
                        da7212_path_tbl_ctrl[i].mask, val)) {
                        diag_err("codec mask script error in reg write\n");
                        return -EIO;
                }
        }

        diag_info("Codec path has updated\r\n");
        return 0;
}

void da7212_get_codec_path_status(u8 *amp_tbl)
{
        int i;
        for(i = 0; i < DA7212_CTRL_MAX; i++)
                amp_tbl[i] = da7212_rout_tbl[i];
}

void da7212_codec_path_clear(u8 *amp_tbl)
{
        int i;
        for(i = 0; i < DA7212_CTRL_MAX; i++) {
                if (amp_tbl)
                        amp_tbl[i] = HAL_AMP_OFF;
                da7212_rout_tbl[i] = HAL_AMP_OFF;
        }
}

/* set CODEC to Active Mode */
int da7212_suspend(void)
{
        diag_info("da7212_suspend\n");
        return da721x_i2c_reg_write(DA7212_CIF_CTRL, 0x80);
}

/* set CODEC to Standby Mode */
int da7212_resume(void)
{
        diag_info("da7212_resume\n");
        /* User : please set up again previous path */
        return 0;
}

/* Initialisation DA7212 */
int da7212_codec_init(void)
{
        int i;

        /* codec reset */
        da721x_i2c_reg_write(DA7212_CIF_CTRL, 0x80);
        mdelay(20);
        for(i = 0; i < DA7212_CTRL_MAX; i++)
                da7212_rout_tbl[i] = HAL_AMP_OFF;

        if (da7212_execute_script(da7212_codec_init_scr))
                return -EIO;
        if (da7212_set_user_data())
                return -EIO;
        return 0;
}

/* Set codec registers to default values */
int da7212_set_codec_default(void)
{
        return da7212_execute_script(da7212_reg_defaults);
}

/* Platform depencecies */
int da7212_set_user_data(void)
{
        return da7212_execute_script_mask(da7212_pdata_setup);
}


/*
 * DEBUG & TEST Functions
 */

/* Read back all registers and print out */
void da7212_dump_all_registers(char *phase)
{
#define BUFFER_BASE_DUMP 0
#ifdef DA721X_DEBUG
#if BUFFER_BASE_DUMP
        int i, j, t;
        char str_buf[60] = "reg[---] = 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F";
        char buf[60];

        diag_info("\n%s\n", phase);
        diag_info("%s\n", str_buf);

        for (j = 0; j < 16; j++) {

                for (t = 0; t < 60; t++)
                        buf[t] = str_buf[t];
                if (j == 0){
                        buf[4] = '0'; buf[5] = '0';
                } else {
                        if ( j < 10)
                                buf[4] = j + 48;
                        else
                                buf[4] = j + 55;
                }
                buf[5] = '0';
                t = 11;
                for (i = 0; i < 16; i++) {
                        u8 reg = da721x_i2c_reg_read(j * 16 + i);
                        u8 d1 = reg / 16;
                        u8 d2 = reg % 16;

                        buf[t++] = d1 < 10? d1+48: d1+55;
                        buf[t++] = d2 < 10? d2+48: d2+55;
                        buf[t++] = ' ';
                }
                diag_info("%s\n", buf);
        }
#else
        int i, j;

        diag_info("%s\n", phase);
        diag_info("reg[--..] = 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
        for (j = 0; j < 59; j++)
                diag_info("-");

        for (j = 0; j < 16; j++) {
                if (j == 0)
                        diag_info("\nreg[00..] = ");
                else
                        diag_info("reg[%x..] = ", j * 16);

                for (i = 0; i < 16; i++) {
                        u8 reg = da721x_i2c_reg_read(j * 16 + i);

                        if (reg < 0x10)
                                diag_info("0%x ", reg);
                        else
                                diag_info("%x ", (reg));
                }
                diag_info("\n");
        }
#endif
#else
        return;
#endif
}

/*
 * Sample code for use cases
 */
#if DA7212_SAMPLE_USE_CSES
void da7212_get_path_playback_hp(u8 *amp_tbl)
{
        amp_tbl[DA7212_DAC_L] = TRUE;
        amp_tbl[DA7212_DAC_R] = TRUE;
        amp_tbl[DA7212_MIXOUT_L] = TRUE;
        amp_tbl[DA7212_MIXOUT_R] = TRUE;
        amp_tbl[DA7212_HP_L] = TRUE;
        amp_tbl[DA7212_HP_R] = TRUE;

        /* Set sound source */
        SET_MIXOUT_L_SEL(MIXOUTL_DACL);
        SET_MIXOUT_R_SEL(MIXOUTR_DACR);
        SET_DAC_L_SRC(DACSRC_DIN_L);
        SET_DAC_R_SRC(DACSRC_DIN_R);
}

void da7212_get_path_playback_spk(u8 *amp_tbl)
{
        amp_tbl[DA7212_DAC_R] = TRUE;
        amp_tbl[DA7212_MIXOUT_R] = TRUE;
        amp_tbl[DA7212_LINE] = TRUE;

        /* Set sound source */
        SET_MIXOUT_R_SEL(MIXOUTR_DACR);
        SET_DAC_R_SRC(DACSRC_DIN_R);
}

void da7212_get_path_record_mic1(u8 *amp_tbl)
{
        amp_tbl[DA7212_MIC1] = TRUE;
        amp_tbl[DA7212_MIXIN_L] = TRUE;
        amp_tbl[DA7212_MIXIN_R] = TRUE;
        amp_tbl[DA7212_ADC_L] = TRUE;
        amp_tbl[DA7212_ADC_R] = TRUE;

        /* Set sound source */
        SET_MIXIN_L_SEL(MIXINL_MIC_1);
        SET_MIXIN_R_SEL(MIXINR_MIC_1);
        SET_DAI_L_SRC(DAISRC_ADC_L);
        SET_DAI_R_SRC(DAISRC_ADC_R);
}

void da7212_get_path_record_mic2(u8 *amp_tbl)
{
        amp_tbl[DA7212_MIC2] = TRUE;
        amp_tbl[DA7212_MIXIN_L] = TRUE;
        amp_tbl[DA7212_MIXIN_R] = TRUE;
        amp_tbl[DA7212_ADC_L] = TRUE;
        amp_tbl[DA7212_ADC_R] = TRUE;

        /* Set sound source */
        SET_MIXIN_L_SEL(MIXINL_MIC_2);
        SET_MIXIN_R_SEL(MIXINR_MIC_2);
        SET_DAI_L_SRC(DAISRC_ADC_L);
        SET_DAI_R_SRC(DAISRC_ADC_R);
}

void da7212_get_path_record_aux(u8 *amp_tbl)
{
        amp_tbl[DA7212_AUX_L] = TRUE;
        amp_tbl[DA7212_AUX_R] = TRUE;
        amp_tbl[DA7212_MIXIN_L] = TRUE;
        amp_tbl[DA7212_MIXIN_R] = TRUE;
        amp_tbl[DA7212_ADC_L] = TRUE;
        amp_tbl[DA7212_ADC_R] = TRUE;

        /* Set sound source */
        SET_MIXIN_L_SEL(MIXINL_AUX_L);
        SET_MIXIN_R_SEL(MIXINR_AUX_R);
        SET_DAI_L_SRC(DAISRC_ADC_L);
        SET_DAI_R_SRC(DAISRC_ADC_R);
}

void da7212_get_path_aux_mixout_to_hp(u8 *amp_tbl)
{
        amp_tbl[DA7212_AUX_L] = TRUE;
        amp_tbl[DA7212_AUX_R] = TRUE;
        amp_tbl[DA7212_MIXOUT_L] = TRUE;
        amp_tbl[DA7212_MIXOUT_R] = TRUE;
        amp_tbl[DA7212_HP_L] = TRUE;
        amp_tbl[DA7212_HP_R] = TRUE;

        /* Set sound source */
        SET_MIXOUT_R_SEL(MIXOUTR_AUX_R);
        SET_MIXOUT_R_SEL(MIXOUTL_AUX_L);
}

void da7212_get_path_aux_mixout_to_line(u8 *amp_tbl)
{
        amp_tbl[DA7212_AUX_R] = TRUE;
        amp_tbl[DA7212_MIXOUT_R] = TRUE;
        amp_tbl[DA7212_LINE] = TRUE;

        /* Set sound source */
        SET_MIXOUT_R_SEL(MIXOUTR_AUX_R);
}
#endif//DA7212_SAMPLE_USE_CSES

