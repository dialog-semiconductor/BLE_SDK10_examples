/*
 * hal-da7218.c - DA7218 audio CODEC Hardware Abstraction Layer functions
 *
 * Copyright (C) 2015 Dialog Semiconductor Ltd. and its Affiliates, unpublished
 * work. This computer program includes Confidential, Proprietary Information
 * and is a Trade Secret of Dialog Semiconductor Ltd. and its Affiliates.
 * All use, disclosure, and/or reproduction is prohibited unless authorized
 * in writing. All Rights Reserved.
 */

#include "hal-da7218.h"
#include "da7218_user_data.h"
#include "da721x_user_data.h"
/*
 * Global Variables
 */
const struct codec_scr_type da7218_reg_defaults[] = {
        {DA7218_SYSTEM_ACTIVE, 0x00},
        {DA7218_CIF_CTRL, 0x00},
        {DA7218_SPARE1, 0x00},
        {DA7218_SR, 0xAA},
        {DA7218_PC_COUNT, 0x02},
        {DA7218_GAIN_RAMP_CTRL, 0x00},
        {DA7218_CIF_TIMEOUT_CTRL, 0x01},
        {DA7218_SYSTEM_MODES_INPUT, 0x00},
        {DA7218_SYSTEM_MODES_OUTPUT, 0x00},
        {DA7218_IN_1L_FILTER_CTRL, 0x00},
        {DA7218_IN_1R_FILTER_CTRL, 0x00},
        {DA7218_IN_2L_FILTER_CTRL, 0x00},
        {DA7218_IN_2R_FILTER_CTRL, 0x00},
        {DA7218_OUT_1L_FILTER_CTRL, 0x40},
        {DA7218_OUT_1R_FILTER_CTRL, 0x40},
        {DA7218_OUT_1_HPF_FILTER_CTRL, 0x80},
        {DA7218_OUT_1_EQ_12_FILTER_CTRL, 0x77},
        {DA7218_OUT_1_EQ_34_FILTER_CTRL, 0x77},
        {DA7218_OUT_1_EQ_5_FILTER_CTRL, 0x07},
        {DA7218_OUT_1_BIQ_5STAGE_CTRL, 0x40},
        {DA7218_OUT_1_BIQ_5STAGE_DATA, 0x00},
        {DA7218_OUT_1_BIQ_5STAGE_ADDR, 0x00},
        {DA7218_MIXIN_1_CTRL, 0x48},
        {DA7218_MIXIN_1_GAIN, 0x03},
        {DA7218_MIXIN_2_CTRL, 0x48},
        {DA7218_MIXIN_2_GAIN, 0x03},
        {DA7218_ALC_CTRL1, 0x00},
        {DA7218_ALC_CTRL2, 0x00},
        {DA7218_ALC_CTRL3, 0x00},
        {DA7218_ALC_NOISE, 0x3F},
        {DA7218_ALC_TARGET_MIN, 0x3F},
        {DA7218_ALC_TARGET_MAX, 0x00},
        {DA7218_ALC_GAIN_LIMITS, 0xFF},
        {DA7218_ALC_ANA_GAIN_LIMITS, 0x71},
        {DA7218_ALC_ANTICLIP_CTRL, 0x00},
        {DA7218_AGS_ENABLE, 0x00},
        {DA7218_AGS_TRIGGER, 0x09},
        {DA7218_AGS_ATT_MAX, 0x00},
        {DA7218_AGS_TIMEOUT, 0x00},
        {DA7218_AGS_ANTICLIP_CTRL, 0x00},
        {DA7218_ENV_TRACK_CTRL, 0x00},
        {DA7218_LVL_DET_CTRL, 0x00},
        {DA7218_LVL_DET_LEVEL, 0x7F},
        {DA7218_DGS_TRIGGER, 0x24},
        {DA7218_DGS_ENABLE, 0x00},
        {DA7218_DGS_RISE_FALL, 0x50},
        {DA7218_DGS_SYNC_DELAY, 0xA3},
        {DA7218_DGS_SYNC_DELAY2, 0x31},
        {DA7218_DGS_SYNC_DELAY3, 0x11},
        {DA7218_DGS_LEVELS, 0x01},
        {DA7218_DGS_GAIN_CTRL, 0x74},
        {DA7218_DROUTING_OUTDAI_1L, 0x01},
        {DA7218_DMIX_OUTDAI_1L_INFILT_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1L_INFILT_1R_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1L_INFILT_2L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1L_INFILT_2R_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1L_TONEGEN_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1L_INDAI_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1L_INDAI_1R_GAIN, 0x1C},
        {DA7218_DROUTING_OUTDAI_1R, 0x04},
        {DA7218_DMIX_OUTDAI_1R_INFILT_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1R_INFILT_1R_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1R_INFILT_2L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1R_INFILT_2R_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1R_TONEGEN_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1R_INDAI_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_1R_INDAI_1R_GAIN, 0x1C},
        {DA7218_DROUTING_OUTFILT_1L, 0x01},
        {DA7218_DMIX_OUTFILT_1L_INFILT_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1L_INFILT_1R_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1L_INFILT_2L_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1L_INFILT_2R_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1L_TONEGEN_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1L_INDAI_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1L_INDAI_1R_GAIN, 0x1C},
        {DA7218_DROUTING_OUTFILT_1R, 0x04},
        {DA7218_DMIX_OUTFILT_1R_INFILT_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1R_INFILT_1R_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1R_INFILT_2L_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1R_INFILT_2R_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1R_TONEGEN_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1R_INDAI_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTFILT_1R_INDAI_1R_GAIN, 0x1C},
        {DA7218_DROUTING_OUTDAI_2L, 0x04},
        {DA7218_DMIX_OUTDAI_2L_INFILT_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2L_INFILT_1R_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2L_INFILT_2L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2L_INFILT_2R_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2L_TONEGEN_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2L_INDAI_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2L_INDAI_1R_GAIN, 0x1C},
        {DA7218_DROUTING_OUTDAI_2R, 0x08},
        {DA7218_DMIX_OUTDAI_2R_INFILT_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2R_INFILT_1R_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2R_INFILT_2L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2R_INFILT_2R_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2R_TONEGEN_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2R_INDAI_1L_GAIN, 0x1C},
        {DA7218_DMIX_OUTDAI_2R_INDAI_1R_GAIN, 0x1C},
        {DA7218_DAI_CTRL, 0x28},
        {DA7218_DAI_TDM_CTRL, 0x40},
        {DA7218_DAI_OFFSET_LOWER, 0x00},
        {DA7218_DAI_OFFSET_UPPER, 0x00},
        {DA7218_DAI_CLK_MODE, 0x01},
        {DA7218_PLL_CTRL, 0x04},
        {DA7218_PLL_FRAC_TOP, 0x00},
        {DA7218_PLL_FRAC_BOT, 0x00},
        {DA7218_PLL_INTEGER, 0x20},
        {DA7218_DAC_NG_CTRL, 0x00},
        {DA7218_DAC_NG_SETUP_TIME, 0x00},
        {DA7218_DAC_NG_OFF_THRESH, 0x00},
        {DA7218_DAC_NG_ON_THRESH, 0x00},
        {DA7218_TONE_GEN_CFG2, 0x00},
        {DA7218_TONE_GEN_FREQ1_L, 0x55},
        {DA7218_TONE_GEN_FREQ1_U, 0x15},
        {DA7218_TONE_GEN_FREQ2_L, 0x00},
        {DA7218_TONE_GEN_FREQ2_U, 0x40},
        {DA7218_TONE_GEN_CYCLES, 0x00},
        {DA7218_TONE_GEN_ON_PER, 0x02},
        {DA7218_TONE_GEN_OFF_PER, 0x01},
        {DA7218_CP_CTRL, 0x60},
        {DA7218_CP_DELAY, 0x15},
        {DA7218_CP_VOL_THRESHOLD1, 0x0E},
        {DA7218_MIC_1_CTRL, 0x40},
        {DA7218_MIC_1_GAIN, 0x01},
        {DA7218_MIC_1_SELECT, 0x00},
        {DA7218_MIC_2_CTRL, 0x40},
        {DA7218_MIC_2_GAIN, 0x01},
        {DA7218_MIC_2_SELECT, 0x00},
        {DA7218_IN_1_HPF_FILTER_CTRL, 0x80},
        {DA7218_IN_2_HPF_FILTER_CTRL, 0x80},
        {DA7218_ADC_1_CTRL, 0x07},
        {DA7218_ADC_2_CTRL, 0x07},
        {DA7218_MIXOUT_L_CTRL, 0x00},
        {DA7218_MIXOUT_L_GAIN, 0x03},
        {DA7218_MIXOUT_R_CTRL, 0x00},
        {DA7218_MIXOUT_R_GAIN, 0x03},
        {DA7218_HP_L_CTRL, 0x40},
        {DA7218_HP_L_GAIN, 0x3B},
        {DA7218_HP_R_CTRL, 0x40},
        {DA7218_HP_R_GAIN, 0x3B},
        {DA7218_HP_DIFF_CTRL, 0x00},
        {DA7218_HP_DIFF_UNLOCK, 0xC3},
        {DA7218_HPLDET_JACK, 0x0B},
        {DA7218_HPLDET_CTRL, 0x00},
        {DA7218_REFERENCES, 0x08},
        {DA7218_IO_CTRL, 0x00},
        {DA7218_LDO_CTRL, 0x00},
        {DA7218_SIDETONE_CTRL, 0x40},
        {DA7218_SIDETONE_IN_SELECT, 0x00},
        {DA7218_SIDETONE_GAIN, 0x1C},
        {DA7218_DROUTING_ST_OUTFILT_1L, 0x01},
        {DA7218_DROUTING_ST_OUTFILT_1R, 0x02},
        {DA7218_SIDETONE_BIQ_3STAGE_DATA, 0x0},
        {DA7218_SIDETONE_BIQ_3STAGE_ADDR, 0x0},
        {DA7218_EVENT_MASK, 0x00},
        {DA7218_DMIC_1_CTRL, 0x00},
        {DA7218_DMIC_2_CTRL, 0x00},
        {DA7218_IN_1L_GAIN, 0x6F},
        {DA7218_IN_1R_GAIN, 0x6F},
        {DA7218_IN_2L_GAIN, 0x6F},
        {DA7218_IN_2R_GAIN, 0x6F},
        {DA7218_OUT_1L_GAIN, 0x6F},
        {DA7218_OUT_1R_GAIN, 0x6F},
        {DA7218_MICBIAS_CTRL, 0x00},
        {DA7218_MICBIAS_EN, 0x00},
        {SEQ_END, 0}
};

static int da7218_paths_table[CODEC_MAX_PATHS];

/* Scripts */

const struct codec_scr_type da7218_codec_init_scr[] = {
        {DA7218_SYSTEM_ACTIVE, 0x01},
        {DA7218_REFERENCES, 0x08},
        {CODEC_REG_DELAY, 10},
        {DA7218_PLL_CTRL, 0x02},
        {DA7218_IO_CTRL, HAL_IO_LVL},
        {DA7218_LDO_CTRL, 0x80 | HAL_LDO_LVL},
        {DA7218_DAI_OFFSET_LOWER, 0x00},
        {DA7218_DAI_OFFSET_UPPER, 0x00},
        {SEQ_END, 0}
};

const struct codec_scr_type da7218_dai_to_HP[] = {
        /* OUTFILT_1L/R_SOURCE = DAI INPUT L/R */
        {DA7218_DROUTING_OUTFILT_1L, 0x20},
        {DA7218_DROUTING_OUTFILT_1R, 0x40},
        /* Enable OUT1L/R FILTER */
        {DA7218_OUT_1L_FILTER_CTRL, 0x80},
        {DA7218_OUT_1R_FILTER_CTRL, 0x80},
        /* Mixout L/R enable */
        {DA7218_MIXOUT_L_CTRL, 0x80},
        {DA7218_MIXOUT_R_CTRL, 0x80},
        /* HPL/R enable */
        {DA7218_HP_L_CTRL, 0xe8},
        {DA7218_HP_R_CTRL, 0xe8},
        {SEQ_END, 0}
};
const struct codec_scr_type da7218_dai_to_HP_off[] = {
        /* Set to Default */
        /* HPL/R */
        {DA7218_HP_L_CTRL, 0xe8},
        {DA7218_HP_R_CTRL, 0xe8},
        {CODEC_REG_DELAY, 50},
        {DA7218_HP_L_CTRL, 0x40},
        {DA7218_HP_R_CTRL, 0x40},
        /* Mixout L/R */
        {DA7218_MIXOUT_L_CTRL, 0x00},
        {DA7218_MIXOUT_R_CTRL, 0x00},
        /* OUT1L/R FILTER */
        {DA7218_OUT_1L_FILTER_CTRL, 0x00},
        {DA7218_OUT_1R_FILTER_CTRL, 0x00},
        /* OUTFILT_1L/R_SOURCE */
        {DA7218_DROUTING_OUTFILT_1L, 0x01},
        {DA7218_DROUTING_OUTFILT_1R, 0x04},
        {SEQ_END, 0}
};
const struct codec_scr_type da7218_mic_to_dai[] = {
        /* MIC Bias 1,2 Enable */
        {DA7218_MICBIAS_EN, DA7218_HAL_MIC_EN},
        /* MIC1,2 Enable */
        {DA7218_MIC_1_CTRL, 0x80},
#ifdef DA7218_USE_AMIC2
        {DA7218_MIC_2_CTRL, 0x80},
#endif
        /* Mixin1,2 enable */
        {DA7218_MIXIN_1_CTRL, 0xA8},
#ifdef DA7218_USE_AMIC2
        {DA7218_MIXIN_2_CTRL, 0xA8},
#endif
        /* Disable IN_1, IN_2 HPF */
        {DA7218_IN_1_HPF_FILTER_CTRL, 0x00},
        {DA7218_IN_2_HPF_FILTER_CTRL, 0x00},
        /* INFILT_1/2_L Enable */
        {DA7218_IN_1L_FILTER_CTRL, 0xA0},
#ifdef DA7218_USE_AMIC2
        {DA7218_IN_2L_FILTER_CTRL, 0xA0},
#endif
        /* INFILT_1_L/R to DAI_1_L/R */
        {DA7218_DROUTING_OUTDAI_1L, 0x01 | DA7218_DMIC2_ROUTINGL},
        {DA7218_DROUTING_OUTDAI_1R, 0x04 | DA7218_DMIC2_ROUTINGL},
        {SEQ_END, 0}
};
const struct codec_scr_type da7218_mic_to_dai_off[] = {
        /* Set to Default */
        /* INFILT_1/2_L */
        {DA7218_IN_1L_FILTER_CTRL, 0xE0},
        {DA7218_IN_2L_FILTER_CTRL, 0xE0},
        {CODEC_REG_DELAY, 50},
        {DA7218_IN_1L_FILTER_CTRL, 0x00},
        {DA7218_IN_2L_FILTER_CTRL, 0x00},
        /* Mixin1,2 */
        {DA7218_MIXIN_1_CTRL, 0x48},
        {DA7218_MIXIN_2_CTRL, 0x48},
        {DA7218_MICBIAS_EN, 0x00},
        {DA7218_MICBIAS_CTRL, 0x00},
        /* MIC1,2 */
        {DA7218_MIC_1_CTRL, 0x00},
        {DA7218_MIC_2_CTRL, 0x00},
        {SEQ_END, 0}
};
const struct codec_scr_type da7218_d_mic_to_dai[] = {
        /* DMic1,2 MIC BIas en & level (2.6V) */
        {DA7218_DMIC_1_CTRL, 0xc0 | HAL_D_MIC1_CTRL_VAL},
#ifdef DA7218_USE_DMIC2
        {DA7218_DMIC_2_CTRL, 0xc0 | HAL_D_MIC2_CTRL_VAL},
#endif
        {DA7218_MICBIAS_EN, DA7218_HAL_MIC_EN},
        /* Mixin1,2 enable */
        {DA7218_MIXIN_1_CTRL, 0xA8},
#ifdef DA7218_USE_DMIC2
        {DA7218_MIXIN_2_CTRL, 0xA8},
#endif
        /* Disable IN_1, IN_2 HPF */
        {DA7218_IN_1_HPF_FILTER_CTRL, 0x00},
        {DA7218_IN_2_HPF_FILTER_CTRL, 0x00},
        /* INFILT_1/2_L/R Enable */
        {DA7218_IN_1L_FILTER_CTRL, 0xA0},
        {DA7218_IN_1R_FILTER_CTRL, 0xA0},
#ifdef DA7218_USE_DMIC2
        {DA7218_IN_2L_FILTER_CTRL, 0xA0},
        {DA7218_IN_2R_FILTER_CTRL, 0xA0},
#endif
        /* INFILT_1/2_L/R to DAI_1_L/R */
        {DA7218_DROUTING_OUTDAI_1L, 0x01 | DA7218_DMIC2_ROUTINGL},
        {DA7218_DROUTING_OUTDAI_1R, 0x02 | DA7218_DMIC2_ROUTINGR},
        {SEQ_END, 0}
};
const struct codec_scr_type da7218_d_mic_to_dai_off[] = {
        /* Set to Default */
        /* INFILT_1/2_L/R */
        {DA7218_IN_1L_FILTER_CTRL, 0xE0},
        {DA7218_IN_2L_FILTER_CTRL, 0xE0},
#ifdef DA7218_USE_DMIC2
        {DA7218_IN_1R_FILTER_CTRL, 0xE0},
        {DA7218_IN_2R_FILTER_CTRL, 0xE0},
#endif
        {CODEC_REG_DELAY, 50},
        {DA7218_IN_1L_FILTER_CTRL, 0x00},
        {DA7218_IN_1R_FILTER_CTRL, 0x00},
        {DA7218_IN_2L_FILTER_CTRL, 0x00},
        {DA7218_IN_2R_FILTER_CTRL, 0x00},
        {DA7218_DROUTING_OUTDAI_1L, 0x01},
        {DA7218_DROUTING_OUTDAI_1R, 0x04},
        /* Mixin1,2 */
        {DA7218_MIXIN_1_CTRL, 0x48},
        {DA7218_MIXIN_2_CTRL, 0x48},
        {DA7218_MICBIAS_EN, 0x00},
        {DA7218_MICBIAS_CTRL, 0x00},
        {DA7218_DMIC_1_CTRL, 0x00},
        {DA7218_DMIC_2_CTRL, 0x00},
        /* INFILT_1/2_L/R */
        {DA7218_DROUTING_OUTDAI_1L, 0x01},
        {DA7218_DROUTING_OUTDAI_1R, 0x04},
        {SEQ_END, 0}
};
const struct codec_scr_type da7218_adc_to_dac[] = {
        /* OUTFILT_1L/R_SOURCE = INFILT1/2L */
        {DA7218_DROUTING_OUTFILT_1L, 0x01},
        {DA7218_DROUTING_OUTFILT_1R, 0x04},
        {SEQ_END, 0}
};
const struct codec_scr_type da7218_adc_to_dac_off[] = {
        {SEQ_END, 0}
};

const struct codec_scr_mask_type da7218_pdata_setup[] = {
        /* Mic1,2 P selected */
        {DA7218_MIC_1_SELECT,
                DA7218_MIC_1_AMP_IN_SEL_MASK,
                HAL_MICAMP_SEL},
#ifdef DA7218_USE_AMIC2
                {DA7218_MIC_2_SELECT,
                        DA7218_MIC_2_AMP_IN_SEL_MASK,
                        HAL_MICAMP_SEL},
#endif
                        /* MIC Bias1,2 level */
                        {DA7218_MICBIAS_CTRL,
                                DA7218_MICBIAS_1_LEVEL_MASK |
                                DA7218_MICBIAS_2_LEVEL_MASK,
                                DA7218_HAL_MIC_BIAS},
                                /* CP */
                                {DA7218_CP_DELAY, 0xff, HAL_CPDELAY},
                                {DA7218_CP_VOL_THRESHOLD1, 0xff, HAL_CP_VOL_THRESH},
                                {DA7218_HP_DIFF_CTRL,
                                        DA7218_HP_AMP_DIFF_MODE_EN_MASK,
                                        HAL_HP_DIFF_CTRL},
                                        {SEQ_END, 0}
};

static int da7218_execute_script(const struct codec_scr_type script[])
{
        int i;

        for (i = 0; script[i].reg != SEQ_END; i++) {
                if (script[i].reg == CODEC_REG_DELAY) {
                        mdelay(script[i].val);
                } else if (da721x_i2c_reg_write((u8)script[i].reg, script[i].val)!= DA721X_OK) {
                        diag_err("codec script error in reg write\n");
                        return -EIO;
                }
        }
        return 0;
}

static int da7218_execute_script_mask(const struct codec_scr_mask_type script[])
{
        int i;

        for (i = 0; script[i].reg != SEQ_END; i++) {
                if (script[i].reg == CODEC_REG_DELAY) {
                        mdelay(script[i].val);
                } else if (da721x_i2c_update_bits((u8)script[i].reg,
                        script[i].mask, script[i].val)) {
                        diag_err("codec mask script error in reg write\n");
                        return -EIO;
                }
        }
        return 0;
}

int da7218_mute(enum hal_controls_t controls, int mute)
{
        diag_info("da7218_mute: %s. device: %d\n",
                mute == 1 ? "mute":mute == 0 ? "Unmute":"unknown", controls);

        switch (controls) {
        case DA7218_MIC1:
                da721x_i2c_update_bits(DA7218_MIC_1_CTRL,
                        DA7218_MIC_1_AMP_MUTE_EN_MASK,
                        (mute ? DA7218_MIC_1_AMP_MUTE_EN_MASK : 0));
                break;
        case DA7218_MIC2:
                da721x_i2c_update_bits(DA7218_MIC_2_CTRL,
                        DA7218_MIC_2_AMP_MUTE_EN_MASK,
                        (mute ? DA7218_MIC_2_AMP_MUTE_EN_MASK : 0));
                break;
        case DA7218_MIXIN:
                da721x_i2c_update_bits(DA7218_MIXIN_1_CTRL,
                        DA7218_MIXIN_1_AMP_MUTE_EN_MASK,
                        (mute ? DA7218_MIXIN_1_AMP_MUTE_EN_MASK : 0));
                da721x_i2c_update_bits(DA7218_MIXIN_2_CTRL,
                        DA7218_MIXIN_1_AMP_MUTE_EN_MASK,
                        (mute ? DA7218_MIXIN_1_AMP_MUTE_EN_MASK : 0));
                break;
        case DA7218_ADC1:
                da721x_i2c_update_bits(DA7218_IN_1L_FILTER_CTRL,
                        DA7218_IN_1L_MUTE_EN_MASK,
                        (mute ? DA7218_IN_1L_MUTE_EN_MASK : 0));
                da721x_i2c_update_bits(DA7218_IN_1R_FILTER_CTRL,
                        DA7218_IN_1R_MUTE_EN_MASK,
                        (mute ? DA7218_IN_1R_MUTE_EN_MASK : 0));
                break;
        case DA7218_ADC2:
                da721x_i2c_update_bits(DA7218_IN_2L_FILTER_CTRL,
                        DA7218_IN_2L_MUTE_EN_MASK,
                        (mute ? DA7218_IN_2L_MUTE_EN_MASK : 0));
                da721x_i2c_update_bits(DA7218_IN_2R_FILTER_CTRL,
                        DA7218_IN_2R_MUTE_EN_MASK,
                        (mute ? DA7218_IN_2R_MUTE_EN_MASK : 0));
                break;
        case DA7218_SIDE_TONE:
                da721x_i2c_update_bits(DA7218_SIDETONE_CTRL,
                        DA7218_SIDETONE_MUTE_EN_MASK,
                        (mute ? DA7218_SIDETONE_MUTE_EN_MASK : 0));
                break;
        case DA7218_DACL:
                da721x_i2c_update_bits(DA7218_OUT_1L_FILTER_CTRL,
                        DA7218_OUT_1L_MUTE_EN_MASK,
                        (mute ? DA7218_OUT_1L_MUTE_EN_MASK : 0));
                break;
        case DA7218_DACR:
                da721x_i2c_update_bits(DA7218_OUT_1R_FILTER_CTRL,
                        DA7218_OUT_1R_MUTE_EN_MASK,
                        (mute ? DA7218_OUT_1L_MUTE_EN_MASK : 0));
                break;
        case DA7218_HP:
                da721x_i2c_update_bits(DA7218_HP_L_CTRL,
                        DA7218_HP_L_AMP_MUTE_EN_MASK,
                        (mute ? DA7218_HP_L_AMP_MUTE_EN_MASK : 0));
                da721x_i2c_update_bits(DA7218_HP_R_CTRL,
                        DA7218_HP_R_AMP_MUTE_EN_MASK,
                        (mute ? DA7218_HP_R_AMP_MUTE_EN_MASK : 0));
                break;
        default:
                diag_err("Invalid control parameter.\n");
                break;
        }
        return 0;
}

int da7218_set_vol_dB(enum hal_controls_t controls, int vol)
{
        switch (controls) {
        case DA7218_MIC1:
                if (vol < MIC_PGA_MIN/100 || vol > MIC_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_MIC_1_GAIN,
                        DA7218_MIC_1_AMP_GAIN_MASK,
                        (vol*100 - MIC_PGA_MIN)/MIC_PGA_STEP);
                break;
        case DA7218_MIC2:
                if (vol < MIC_PGA_MIN/100 || vol > MIC_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_MIC_2_GAIN,
                        DA7218_MIC_2_AMP_GAIN_MASK,
                        (vol*100 - MIC_PGA_MIN)/MIC_PGA_STEP);
                break;
        case DA7218_MIXIN:
                if (vol < MIXIN_PGA_MIN/100 || vol > MIXIN_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_MIXIN_1_GAIN,
                        DA7218_MIXIN_1_AMP_GAIN_MASK,
                        (vol*100 - MIXIN_PGA_MIN)/MIXIN_PGA_STEP);
                da721x_i2c_update_bits(DA7218_MIXIN_2_GAIN,
                        DA7218_MIXIN_2_AMP_GAIN_MASK,
                        (vol*100 - MIXIN_PGA_MIN)/MIXIN_PGA_STEP);
                break;
        case DA7218_ADC1:
                if (vol < ADC_PGA_MIN/100 || vol > ADC_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_IN_1L_GAIN,
                        DA7218_IN_1L_DIGITAL_GAIN_MASK,
                        (vol*100 - ADC_PGA_MIN)/DIGITAL_PGA_STEP);
                da721x_i2c_update_bits(DA7218_IN_1R_GAIN,
                        DA7218_IN_1R_DIGITAL_GAIN_MASK,
                        (vol*100 - ADC_PGA_MIN)/DIGITAL_PGA_STEP);
                break;
        case DA7218_ADC2:
                if (vol < ADC_PGA_MIN/100 || vol > ADC_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_IN_2L_GAIN,
                        DA7218_IN_1L_DIGITAL_GAIN_MASK,
                        (vol*100 - ADC_PGA_MIN)/DIGITAL_PGA_STEP);
                da721x_i2c_update_bits(DA7218_IN_2R_GAIN,
                        DA7218_IN_1R_DIGITAL_GAIN_MASK,
                        (vol*100 - ADC_PGA_MIN)/DIGITAL_PGA_STEP);
                break;
        case DA7218_SIDE_TONE:
                if (vol < DMIX_PGA_MIN/100 || vol > DMIX_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_SIDETONE_GAIN,
                        DA7218_SIDETONE_GAIN_MASK,
                        (vol*100 - DMIX_PGA_MIN)/DMIX_PGA_STEP);
                break;
        case DA7218_DACL:
                if (vol < DAC_PGA_MIN/100 || vol > DAC_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_OUT_1L_GAIN,
                        DA7218_OUT_1L_DIGITAL_GAIN_MASK,
                        (vol*100 - DAC_PGA_MIN)/DIGITAL_PGA_STEP);
                break;
        case DA7218_DACR:
                if (vol < DAC_PGA_MIN/100 || vol > DAC_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_OUT_1R_GAIN,
                        DA7218_OUT_1R_DIGITAL_GAIN_MASK,
                        (vol*100 - DAC_PGA_MIN)/DIGITAL_PGA_STEP);
                break;
        case DA7218_HP:
                if (vol < HP_PGA_MIN/100 || vol > HP_PGA_MAX/100)
                        goto set_vol_err;
                da721x_i2c_update_bits(DA7218_HP_L_GAIN,
                        DA7218_HP_L_AMP_GAIN_MASK,
                        0x15+(vol*100-HP_PGA_MIN)/HP_PGA_STEP);
                da721x_i2c_update_bits(DA7218_HP_R_GAIN,
                        DA7218_HP_R_AMP_GAIN_MASK,
                        0x15+(vol*100-HP_PGA_MIN)/HP_PGA_STEP);
                break;
        default:
                diag_err("invalid device:%d\n", controls);
                break;
        }
        diag_info("Da7218 Set device(%d) to %ddB\n", controls, vol);
        return 0;

        set_vol_err:
        diag_err("invalid volume device:%d, vol:%ddB\n", controls, vol);
        return -EINVAL;
}

/* control all dmix gain registers */
int dmix_gain_register_check(u8 reg)
{
        int ret;

        switch (reg) {
        case DA7218_DMIX_OUTDAI_1L_INFILT_1L_GAIN:
        case DA7218_DMIX_OUTDAI_1R_INFILT_1L_GAIN:
        case DA7218_DMIX_OUTDAI_2L_INFILT_1L_GAIN:
        case DA7218_DMIX_OUTDAI_2R_INFILT_1L_GAIN:

        case DA7218_DMIX_OUTDAI_1L_INFILT_1R_GAIN:
        case DA7218_DMIX_OUTDAI_1R_INFILT_1R_GAIN:
        case DA7218_DMIX_OUTDAI_2L_INFILT_1R_GAIN:
        case DA7218_DMIX_OUTDAI_2R_INFILT_1R_GAIN:

        case DA7218_DMIX_OUTDAI_1L_INFILT_2L_GAIN:
        case DA7218_DMIX_OUTDAI_1R_INFILT_2L_GAIN:
        case DA7218_DMIX_OUTDAI_2L_INFILT_2L_GAIN:
        case DA7218_DMIX_OUTDAI_2R_INFILT_2L_GAIN:

        case DA7218_DMIX_OUTDAI_1L_INFILT_2R_GAIN:
        case DA7218_DMIX_OUTDAI_1R_INFILT_2R_GAIN:
        case DA7218_DMIX_OUTDAI_2L_INFILT_2R_GAIN:
        case DA7218_DMIX_OUTDAI_2R_INFILT_2R_GAIN:

        case DA7218_DMIX_OUTDAI_1L_TONEGEN_GAIN:
        case DA7218_DMIX_OUTDAI_1R_TONEGEN_GAIN:
        case DA7218_DMIX_OUTDAI_2L_TONEGEN_GAIN:
        case DA7218_DMIX_OUTDAI_2R_TONEGEN_GAIN:

        case DA7218_DMIX_OUTDAI_1L_INDAI_1L_GAIN:
        case DA7218_DMIX_OUTDAI_1R_INDAI_1L_GAIN:
        case DA7218_DMIX_OUTDAI_2L_INDAI_1L_GAIN:
        case DA7218_DMIX_OUTDAI_2R_INDAI_1L_GAIN:

        case DA7218_DMIX_OUTDAI_1L_INDAI_1R_GAIN:
        case DA7218_DMIX_OUTDAI_1R_INDAI_1R_GAIN:
        case DA7218_DMIX_OUTDAI_2L_INDAI_1R_GAIN:
        case DA7218_DMIX_OUTDAI_2R_INDAI_1R_GAIN:

        case DA7218_DMIX_OUTFILT_1L_INFILT_1L_GAIN:
        case DA7218_DMIX_OUTFILT_1R_INFILT_1L_GAIN:
        case DA7218_DMIX_OUTFILT_1L_INFILT_1R_GAIN:
        case DA7218_DMIX_OUTFILT_1R_INFILT_1R_GAIN:
        case DA7218_DMIX_OUTFILT_1L_INFILT_2L_GAIN:
        case DA7218_DMIX_OUTFILT_1R_INFILT_2L_GAIN:
        case DA7218_DMIX_OUTFILT_1L_INFILT_2R_GAIN:
        case DA7218_DMIX_OUTFILT_1R_INFILT_2R_GAIN:

        case DA7218_DMIX_OUTFILT_1L_TONEGEN_GAIN:
        case DA7218_DMIX_OUTFILT_1R_TONEGEN_GAIN:

        case DA7218_DMIX_OUTFILT_1L_INDAI_1L_GAIN:
        case DA7218_DMIX_OUTFILT_1R_INDAI_1L_GAIN:
        case DA7218_DMIX_OUTFILT_1L_INDAI_1R_GAIN:
        case DA7218_DMIX_OUTFILT_1R_INDAI_1R_GAIN:
        case DA7218_SIDETONE_GAIN:
                ret = 0;
                break;
        default:
                ret = -EINVAL;
                diag_err("invalid dmix register: 0x%x\n", reg);
                break;
        }
        return ret;
}

int da7218_set_dmix_gain_dB(u8 reg , int vol)
{
        int ret;

        if (dmix_gain_register_check(reg))
                return -EINVAL;

        if (vol < DMIX_PGA_MIN/100 ||
                vol > DMIX_PGA_MAX/100)
                diag_err("invalid dmix gain: %ddB\n", vol);

        ret = da721x_i2c_update_bits(reg,
                DMIX_GAIN_MASK,
                (vol*100 - DMIX_PGA_MIN)/DMIX_PGA_STEP);
        if (ret < 0)
                return -EIO;

        diag_info("Da7218 ctrl device(%d) to %ddB\n", reg, vol);
        return 0;
}

/* Supported PLL input frequencies are 5MHz - 54MHz. */
int da7218_set_dai_pll(int sampling_rate)
{

        u8 pll_ctrl = 0;
        u8 pll_frac_top, pll_frac_bot, pll_integer;

        if (DA7218_MCLK == 32768) {
                pll_ctrl = DA7218_PLL_INDIV_2_5_MHZ;
        } else if (DA7218_MCLK < 2000000) {
                goto pll_err;
        } else if (DA7218_MCLK <= 5000000) {
                pll_ctrl = DA7218_PLL_INDIV_2_5_MHZ;
        } else if (DA7218_MCLK <= 10000000) {
                pll_ctrl = DA7218_PLL_INDIV_5_10_MHZ;
        } else if (DA7218_MCLK <= 20000000) {
                pll_ctrl = DA7218_PLL_INDIV_10_20_MHZ;
        } else if (DA7218_MCLK <= 40000000) {
                pll_ctrl = DA7218_PLL_INDIV_20_40_MHZ;
        } else if (DA7218_MCLK <= 54000000) {
                pll_ctrl = DA7218_PLL_INDIV_40_54_MHZ;
        } else {
                diag_err("PLL input clock %d above valid range\n",
                        DA7218_MCLK);
                return -EINVAL;
        }
        /*
         * Set PLL mode as header file is defined
         */
        pll_ctrl |= DA7218_PLL_MOD_VAL;

        /* Set PLL dividers for 12M/24M
         * If MCLK is not 12M or 24M, need to update dividers
         */
        if (sampling_rate % 11025) {
                /* 8K sr base */
                pll_frac_top = 0x18;
                pll_frac_bot = 0x93;
                pll_integer = 0x20;
        } else { /* 11.025K base */
                pll_frac_top = 0x03;
                pll_frac_bot = 0x61;
                pll_integer = 0x1E;
        }

        /* Write PLL dividers */
        da721x_i2c_reg_write(DA7218_PLL_FRAC_TOP, pll_frac_top);
        da721x_i2c_reg_write(DA7218_PLL_FRAC_BOT, pll_frac_bot);
        da721x_i2c_reg_write(DA7218_PLL_INTEGER, pll_integer);

        /* Enable MCLK squarer if required */
#if (DA7218_MCLK_SQR_EN)
        pll_ctrl |= (1<<DA7218_PLL_MCLK_SQR_EN_SHIFT);
#endif
        /* Enable PLL */
        da721x_i2c_update_bits(DA7218_PLL_CTRL,
                DA7218_PLL_MODE_MASK
                | DA7218_PLL_INDIV_MASK
                | DA7218_PLL_MCLK_SQR_EN_MASK,
                pll_ctrl);
        return 0;

        pll_err:
        diag_info("Unsupported PLL input frequency %d\n", DA7218_MCLK);
        return -EINVAL;
}

int da7218_set_dai_tdm_slot(unsigned int tx_mask,
        unsigned int rx_mask, int slots, int slot_width)
{
        u8 dai_bclks_per_wclk;
        u32 frame_size;

        /* No channels enabled so disable TDM, revert to 64-bit frames */
        if (!tx_mask) {
                da721x_i2c_update_bits(DA7218_DAI_TDM_CTRL,
                        DA7218_DAI_TDM_CH_EN_MASK |
                        DA7218_DAI_TDM_MODE_EN_MASK, 0);
                da721x_i2c_update_bits(DA7218_DAI_CLK_MODE,
                        DA7218_DAI_BCLKS_PER_WCLK_MASK,
                        DA7218_DAI_BCLKS_PER_WCLK_64);
                return 0;
        }

        /* Check we have valid slots */
        if (tx_mask >> DA7218_DAI_TDM_MAX_SLOTS) {
                diag_err("Invalid number of slots, max = %d\n",
                        DA7218_DAI_TDM_MAX_SLOTS);
                return -EINVAL;
        }

        /* Check we have a valid offset given (first 2 bytes of rx_mask) */
        if (rx_mask >> DA7218_2BYTE_SHIFT) {
                diag_err("Invalid slot offset, max = %d\n", DA7218_2BYTE_SHIFT);
                return -EINVAL;
        }

        /* Calculate & validate frame size based on slot info provided. */
        frame_size = slots * slot_width;
        switch (frame_size) {
        case 32:
                dai_bclks_per_wclk = DA7218_DAI_BCLKS_PER_WCLK_32;
                break;
        case 64:
                dai_bclks_per_wclk = DA7218_DAI_BCLKS_PER_WCLK_64;
                break;
        case 128:
                dai_bclks_per_wclk = DA7218_DAI_BCLKS_PER_WCLK_128;
                break;
        case 256:
                dai_bclks_per_wclk = DA7218_DAI_BCLKS_PER_WCLK_256;
                break;
        default:
                diag_err("Invalid frame size\n");
                return -EINVAL;
        }

        da721x_i2c_update_bits(DA7218_DAI_CLK_MODE,
                DA7218_DAI_BCLKS_PER_WCLK_MASK,
                dai_bclks_per_wclk);
        da721x_i2c_reg_write(DA7218_DAI_OFFSET_LOWER,
                (rx_mask & DA7218_BYTE_MASK));
        da721x_i2c_reg_write(DA7218_DAI_OFFSET_UPPER,
                ((rx_mask >> DA7218_BYTE_SHIFT) & DA7218_BYTE_MASK));
        da721x_i2c_update_bits(DA7218_DAI_TDM_CTRL,
                DA7218_DAI_TDM_CH_EN_MASK |
                DA7218_DAI_TDM_MODE_EN_MASK,
                (tx_mask << DA7218_DAI_TDM_CH_EN_SHIFT) |
                DA7218_DAI_TDM_MODE_EN_MASK);

        return 0;
}

static char get_sample_rate(u32 sr)
{
        char fs;

        switch (sr) {
        case 8000:
                fs = DA7218_SR_8000;
                break;
        case 11025:
                fs = DA7218_SR_11025;
                break;
        case 12000:
                fs = DA7218_SR_12000;
                break;
        case 16000:
                fs = DA7218_SR_16000;
                break;
        case 22050:
                fs = DA7218_SR_22050;
                break;
        case 24000:
                fs = DA7218_SR_24000;
                break;
        case 32000:
                fs = DA7218_SR_32000;
                break;
        case 44100:
                fs = DA7218_SR_44100;
                break;
        case 48000:
                fs = DA7218_SR_48000;
                break;
        case 88200:
                fs = DA7218_SR_88200;
                break;
        case 96000:
                fs = DA7218_SR_96000;
                break;
        default:
                diag_err("Invalid SR parameter.\n");
                return -EINVAL;
        }
        return fs;
}

char da7218_verify_sr(char fs_adc, char fs_dac)
{
        if (fs_adc < 0 || fs_dac < 0)
                return -EINVAL;

        if (fs_dac == fs_adc)
                return 0;

        if (fs_adc > fs_dac) {
                if (!(fs_adc % fs_dac))
                        return 0;
        } else {
                if (!(fs_dac % fs_adc))
                        return 0;
        }

        diag_err("ADC(%d)DAC(%d) sample rates error!\n",
                fs_adc, fs_dac);
        return -EINVAL;
}

/* CP / Single Supply control */
int da7218_cp_control(u8 onoff)
{
#ifndef DA7218_SINGLE_SUPPLY_MODE
        u8 read_val;

        if (onoff) {
                /* check power source to avoid damage */
                read_val = da721x_i2c_reg_read(DA7218_HP_DIFF_CTRL);
                if (read_val & DA7218_HP_AMP_SINGLE_SUPPLY_EN_MASK) {
                        diag_err("Invalid command for HP Amp power\n");
                        return -EINVAL;
                }
        }

        da721x_i2c_reg_write(DA7218_CP_CTRL,
                onoff << DA7218_CP_EN_SHIFT
                | HAL_CP_CTRL_VAL);
        return 0;
#endif
}

int da7218_set_sample_rate(u32 adc_sr, u32 dac_sr)
{
        char fs_adc, fs_dac;
        u8 fs;

        fs_adc = get_sample_rate(adc_sr);
        fs_dac = get_sample_rate(dac_sr);
        if (da7218_verify_sr(fs_adc, fs_dac) < 0)
                return -EINVAL;

        fs = fs_adc | fs_dac<<4;
        da721x_i2c_reg_write(DA7218_SR, fs);
        diag_info(" sampling rate: adc[0x%x] dac(0x%x)\n",
                fs_adc, fs_dac);
        if (da7218_set_dai_pll(adc_sr) < 0)
                return -EINVAL;
        return 0;
}

/*
 * This HAL configure as below for DAI format
 * 16 bit Word Length, I2S.
 */
int da7218_set_dai_fmt(void)
{
        u8 dai_clk_mode = 0;
        u8 dai_ctrl = 0;
        u8 dai_ctrl_mask = 0;
        u8 pll_ctrl = 0;
        u8 pll_status = 0;
        u8 srm_lock = 0;
        int i = 0;

        diag_info("Getting ready for working DAI, 16bit Word Length, I2S...\n");

        /* Set Codec Master/Slave, Polarity & clk */
#if (AP_I2S_MASTER == DA7218_CODEC_MASTER)
        /* Da7218 Codec is Master */
        dai_clk_mode |= DA7218_DAI_CLK_EN_MASTER_MODE;
        dai_clk_mode |= DA7218_DAI_BCLKS_PER_WCLK_32;
        diag_info(" Da7218 Codec is master...\n");
#else
        /* Da7218 Codec is slave */
        dai_clk_mode |= DA7218_DAI_CLK_EN_SLAVE_MODE;
        dai_clk_mode |= DA7218_DAI_BCLKS_PER_WCLK_64;
        diag_info("Da7218 Codec is slave...\n");
#endif
        da721x_i2c_reg_write(DA7218_DAI_CLK_MODE, dai_clk_mode);

        /* set DAI Word Length */
        dai_ctrl |= DA7218_DAI_WORD_LENGTH_S16_LE;
        dai_ctrl_mask |= DA7218_DAI_WORD_LENGTH_MASK;
        /* set DAI DATA Format */
        dai_ctrl |= DA7218_DAI_FMT;
        dai_ctrl_mask |= DA7218_DAI_FORMAT_MASK;
        /* Enable DAI Ctrl */
        dai_ctrl |= 1 << DA7218_DAI_EN_SHIFT;
        dai_ctrl_mask |= DA7218_DAI_EN_MASK;

#ifdef DA7218_TUNE_REF_OSC
        u8 refosc_cal = 0;
        /* Tune reference oscillator */
        da721x_i2c_update_bits(DA7218_PLL_REFOSC_CAL,
                DA7218_PLL_REFOSC_CAL_START_MASK);
        da721x_i2c_update_bits(DA7218_PLL_REFOSC_CAL,
                DA7218_PLL_REFOSC_CAL_START_MASK |
                DA7218_PLL_REFOSC_CAL_EN_MASK);

        /* Check tuning complete */
        do {
                refosc_cal = 0;
                refosc_cal = da721x_i2c_reg_read(DA7218_PLL_REFOSC_CAL);
        } while (refosc_cal & DA7218_PLL_REFOSC_CAL_START_MASK);
#endif
        da721x_i2c_update_bits(DA7218_DAI_CTRL, dai_ctrl_mask,
                dai_ctrl);

        pll_ctrl = da721x_i2c_reg_read(DA7218_PLL_CTRL);
        if ((pll_ctrl & DA7218_PLL_MODE_MASK) != DA7218_PLL_MODE_SRM){
                diag_info("Ready.\n");
                return DA721X_OK;
        }
        /* Check SRM has locked */
        do {
                pll_status = 0;
                pll_status = da721x_i2c_reg_read(DA7218_PLL_STATUS);
                if (pll_status < 0)
                        return -EINVAL;

                if (pll_status & DA7218_PLL_SRM_STATUS_SRM_LOCK) {
                        srm_lock = 1;
                } else {
                        ++i;
                        mdelay(50);
                }
        } while ((i < DA7218_SRM_CHECK_RETRIES) & (!srm_lock));
        diag_info("Ready to work DAI with PLL.\n");

        return DA721X_OK;
}

/* Sidetone control:
 * onoff: Turn on(1)/off(0)
 * in_sel: sidtone_sel_t
 * vol: -42 dB ~ 4.5 dB
 */
int da7218_sidetone_ctrl(u8 onoff, enum sidtone_sel_t in_sel, int vol)
{
        if (onoff > 1 || onoff < 0) {
                diag_err("Invalid sidetone control\n");
                return -EINVAL;
        }

        /* enable path */
        switch (in_sel) {
        case IN_FILTER1L:
        case IN_FILTER1R:
                da721x_i2c_update_bits(DA7218_DROUTING_ST_OUTFILT_1L,
                        1 << DA7218_DMIX_ST_SRC_SIDETONE,
                        onoff << DA7218_DMIX_ST_SRC_SIDETONE);
                break;
        case IN_FILTER2L:
        case IN_FILTER2R:
                da721x_i2c_update_bits(DA7218_DROUTING_ST_OUTFILT_1R,
                        1 << DA7218_DMIX_ST_SRC_SIDETONE,
                        onoff << DA7218_DMIX_ST_SRC_SIDETONE);
                break;
        default:
                diag_err("Invalid in_sel value\n");
                return -EINVAL;
        }

        da721x_i2c_update_bits(DA7218_SIDETONE_IN_SELECT,
                DA7218_SIDETONE_IN_SELECT_MASK,
                in_sel);

        da721x_i2c_update_bits(DA7218_SIDETONE_CTRL,
                DA7218_SIDETONE_FILTER_EN_MASK,
                onoff << DA7218_SIDETONE_FILTER_EN_SHIFT);

        /* Enable side tone */
        da7218_set_vol_dB(DA7218_SIDE_TONE, vol);

        /* Unmute side tone */
        da7218_mute(DA7218_SIDE_TONE, CODEC_UNMUTE);
        return 0;
}

/*
 * ALC
 */
int da7218_alc_calib(u8 set_on)
{
        u8 mic_1_ctrl, mic_2_ctrl;
        u8 mixin_1_ctrl, mixin_2_ctrl;
        u8 in_1l_filt_ctrl, in_1r_filt_ctrl, in_2l_filt_ctrl, in_2r_filt_ctrl;
        u8 in_1_hpf_ctrl;// unused? in_2_hpf_ctrl;
        u8 dai_ctrl, calib_ctrl;
        static u8 alc_cal_done;

        if (alc_cal_done && set_on) {
                diag_info("ALC calibration was already done\n");
                return 0;
        }

        /* set_on == 0 means that going to do alc calibration
         * next time due to some reason like system process
         * or codec status.
         */
        if (!set_on) {
                alc_cal_done = 0;
                return 0;
        }

        /* Save current state of MIC control registers */
        mic_1_ctrl = da721x_i2c_reg_read(DA7218_MIC_1_CTRL);
        mic_2_ctrl = da721x_i2c_reg_read(DA7218_MIC_2_CTRL);

        /* Save current state of input mixer control registers */
        mixin_1_ctrl = da721x_i2c_reg_read(DA7218_MIXIN_1_CTRL);
        mixin_2_ctrl = da721x_i2c_reg_read(DA7218_MIXIN_2_CTRL);

        /* Save current state of input filter control registers */
        in_1l_filt_ctrl = da721x_i2c_reg_read(DA7218_IN_1L_FILTER_CTRL);
        in_1r_filt_ctrl = da721x_i2c_reg_read(DA7218_IN_1R_FILTER_CTRL);
        in_2l_filt_ctrl = da721x_i2c_reg_read(DA7218_IN_2L_FILTER_CTRL);
        in_2r_filt_ctrl = da721x_i2c_reg_read(DA7218_IN_2R_FILTER_CTRL);

        /* Save current state of input HPF control registers */
        in_1_hpf_ctrl = da721x_i2c_reg_read(DA7218_IN_1_HPF_FILTER_CTRL);

        /* Enable then Mute MIC PGAs */
        da721x_i2c_update_bits(DA7218_MIC_1_CTRL, DA7218_MIC_1_AMP_EN_MASK,
                DA7218_MIC_1_AMP_EN_MASK);
        da721x_i2c_update_bits(DA7218_MIC_2_CTRL, DA7218_MIC_2_AMP_EN_MASK,
                DA7218_MIC_2_AMP_EN_MASK);
        da721x_i2c_update_bits(DA7218_MIC_1_CTRL,
                DA7218_MIC_1_AMP_MUTE_EN_MASK,
                DA7218_MIC_1_AMP_MUTE_EN_MASK);
        da721x_i2c_update_bits(DA7218_MIC_2_CTRL,
                DA7218_MIC_2_AMP_MUTE_EN_MASK,
                DA7218_MIC_2_AMP_MUTE_EN_MASK);

        /* Enable input mixers unmuted */
        da721x_i2c_update_bits(DA7218_MIXIN_1_CTRL,
                DA7218_MIXIN_1_AMP_EN_MASK |
                DA7218_MIXIN_1_AMP_MUTE_EN_MASK,
                DA7218_MIXIN_1_AMP_EN_MASK);
        da721x_i2c_update_bits(DA7218_MIXIN_2_CTRL,
                DA7218_MIXIN_2_AMP_EN_MASK |
                DA7218_MIXIN_2_AMP_MUTE_EN_MASK,
                DA7218_MIXIN_2_AMP_EN_MASK);

        /* Enable input filters unmuted */
        da721x_i2c_update_bits(DA7218_IN_1L_FILTER_CTRL,
                DA7218_IN_1L_FILTER_EN_MASK |
                DA7218_IN_1L_MUTE_EN_MASK,
                DA7218_IN_1L_FILTER_EN_MASK);
        da721x_i2c_update_bits(DA7218_IN_1R_FILTER_CTRL,
                DA7218_IN_1R_FILTER_EN_MASK |
                DA7218_IN_1R_MUTE_EN_MASK,
                DA7218_IN_1R_FILTER_EN_MASK);
        da721x_i2c_update_bits(DA7218_IN_2L_FILTER_CTRL,
                DA7218_IN_2L_FILTER_EN_MASK |
                DA7218_IN_2L_MUTE_EN_MASK,
                DA7218_IN_2L_FILTER_EN_MASK);
        da721x_i2c_update_bits(DA7218_IN_2R_FILTER_CTRL,
                DA7218_IN_2R_FILTER_EN_MASK |
                DA7218_IN_2R_MUTE_EN_MASK,
                DA7218_IN_2R_FILTER_EN_MASK);

        /*
         * Make sure input HPFs voice mode is disabled, otherwise for sampling
         * rates above 32KHz the ADC signals will be stopped and will cause
         * calibration to lock up.
         */
        da721x_i2c_update_bits(DA7218_IN_1_HPF_FILTER_CTRL,
                DA7218_IN_1_VOICE_EN_MASK, 0);
        da721x_i2c_update_bits(DA7218_IN_2_HPF_FILTER_CTRL,
                DA7218_IN_2_VOICE_EN_MASK, 0);


        /* If DAI not enabled, use free running PC counter for clock */
        dai_ctrl = da721x_i2c_reg_read(DA7218_DAI_CTRL);
        if (!(dai_ctrl & DA7218_DAI_EN_MASK))
                da721x_i2c_update_bits(DA7218_PC_COUNT,
                        DA7218_PC_FREERUN_MASK,
                        DA7218_PC_FREERUN_MASK);

        /* Perform auto calibration */
        da721x_i2c_update_bits(DA7218_CALIB_CTRL, DA7218_CALIB_AUTO_EN_MASK,
                DA7218_CALIB_AUTO_EN_MASK);
        do {
                calib_ctrl = da721x_i2c_reg_read(DA7218_CALIB_CTRL);
        } while (calib_ctrl & DA7218_CALIB_AUTO_EN_MASK);

        /* If auto calibration fails, disable DC offset, hybrid ALC */
        if (calib_ctrl & DA7218_CALIB_OVERFLOW_MASK) {
                diag_info("ALC auto calibration failed with overflow\n");
                da721x_i2c_update_bits(DA7218_CALIB_CTRL,
                        DA7218_CALIB_OFFSET_EN_MASK, 0);
                da721x_i2c_update_bits(DA7218_ALC_CTRL1,
                        DA7218_ALC_SYNC_MODE_MASK, 0);

        } else {
                /* Enable DC offset cancellation */
                da721x_i2c_update_bits(DA7218_CALIB_CTRL,
                        DA7218_CALIB_OFFSET_EN_MASK,
                        DA7218_CALIB_OFFSET_EN_MASK);

                /* Enable ALC hybrid mode */
                da721x_i2c_update_bits(DA7218_ALC_CTRL1,
                        DA7218_ALC_SYNC_MODE_MASK,
                        DA7218_ALC_SYNC_MODE_CH1 |
                        DA7218_ALC_SYNC_MODE_CH2);
        }

        /* Restore PC count synchronisation to DAI, if previously disabled */
        if (!(dai_ctrl & DA7218_DAI_EN_MASK))
                da721x_i2c_update_bits(DA7218_PC_COUNT,
                        DA7218_PC_FREERUN_MASK, 0);

        /* Restore input HPF control registers to original states */
        da721x_i2c_reg_write(DA7218_IN_1_HPF_FILTER_CTRL, in_1_hpf_ctrl);
        da721x_i2c_reg_write(DA7218_IN_1_HPF_FILTER_CTRL, in_1_hpf_ctrl);

        /* Restore input filter control registers to original states */
        da721x_i2c_reg_write(DA7218_IN_1L_FILTER_CTRL, in_1l_filt_ctrl);
        da721x_i2c_reg_write(DA7218_IN_1R_FILTER_CTRL, in_1r_filt_ctrl);
        da721x_i2c_reg_write(DA7218_IN_2L_FILTER_CTRL, in_2l_filt_ctrl);
        da721x_i2c_reg_write(DA7218_IN_2R_FILTER_CTRL, in_2r_filt_ctrl);

        /* Restore input mixer control registers to original state */
        da721x_i2c_reg_write(DA7218_MIXIN_1_CTRL, mixin_1_ctrl);
        da721x_i2c_reg_write(DA7218_MIXIN_2_CTRL, mixin_2_ctrl);

        /* Restore MIC control registers to original states */
        da721x_i2c_reg_write(DA7218_MIC_1_CTRL, mic_1_ctrl);
        da721x_i2c_reg_write(DA7218_MIC_2_CTRL, mic_2_ctrl);

        alc_cal_done = 1;
        return 0;
}

int da7218_alc_control(enum da7218_alc_ctrl_t ctrl, u8 set_val)
{
        u8 alc_val = 0, mask, reg;

        switch (ctrl) {

        case HAL_ALC_CH1_SW:
                if (set_val > 1)
                        goto alc_err_return;
                alc_val = set_val ? 0x3:0;
                mask = 0x03;
                reg = DA7218_ALC_CTRL1;
                break;
        case HAL_ALC_CH2_SW:
                if (set_val > 1)
                        goto alc_err_return;
                alc_val = set_val ? 0xc:0;
                mask = 0x0c;
                reg = DA7218_ALC_CTRL1;
                break;
        case HAL_ALC_ANTICLIP_SW:
                if (set_val > 1)
                        goto alc_err_return;
                alc_val = set_val;
                alc_val <<= DA7218_ALC_ANTICLIP_EN_SHIFT;
                mask = DA7218_ALC_ANTICLIP_EN_MASK;
                reg = DA7218_ALC_ANTICLIP_CTRL;
                break;
        case HAL_ALC_ANTICLIP_STEP:
                /* val should be 0 ~ 4 */
                if (set_val > DA7218_ALC_ANTICLIP_STEP_MAX)
                        goto alc_err_return;

                /* 0.034 dB/fs step,
                 * 0~4: 0.034, 0.068, 0.136, 0.272
                 */
                alc_val = set_val;
                mask = DA7218_ALC_ANTICLIP_STEP_MASK;
                reg = DA7218_ALC_ANTICLIP_CTRL;
                break;
        case HAL_ALC_ANA_GAINMAX:
                /* val should be 1 ~ 7 */
                if (set_val > DA7218_ALC_ANA_GAIN_MAX
                        || set_val < DA7218_ALC_ANA_GAIN_MIN)
                        goto alc_err_return;

                /* 6 dB step, 1: 0 dB, 7: 36dB
                 */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_ANA_GAIN_MAX_SHIFT;
                mask = DA7218_ALC_ANA_GAIN_MAX_MASK;
                reg = DA7218_ALC_ANA_GAIN_LIMITS;
                break;
        case HAL_ALC_ANA_GAINMIN:
                /* val should be 1 ~ 7 */
                if (set_val > DA7218_ALC_ANA_GAIN_MAX
                        || set_val < DA7218_ALC_ANA_GAIN_MIN)
                        goto alc_err_return;

                /* 6 dB step, 1: 0 dB, 7: 36dB
                 */
                alc_val = set_val;
                mask = DA7218_ALC_ANA_GAIN_MIN_MASK;
                reg = DA7218_ALC_ANA_GAIN_LIMITS;
                break;
        case HAL_ALC_ATTEN_MAX:
                /* val should be 0 ~ 15(0xF) */
                if (set_val > DA7218_ALC_ATTEN_GAIN_MAX)
                        goto alc_err_return;

                /* 6 dB step, 0: 0 dB, 63: 90dB
                 */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_ATTEN_MAX_SHIFT;
                mask = DA7218_ALC_ATTEN_MAX_MASK;
                reg = DA7218_ALC_GAIN_LIMITS;
                break;
        case HAL_ALC_GAIN_MAX:
                /* val should be 0 ~ 15(0xF) */
                if (set_val > DA7218_ALC_ATTEN_GAIN_MAX)
                        goto alc_err_return;

                /* 6 dB step, 0: 0 dB, 63: 90dB
                 */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_GAIN_MAX_SHIFT;
                mask = DA7218_ALC_GAIN_MAX_MASK;
                reg = DA7218_ALC_GAIN_LIMITS;
                break;
        case HAL_ALC_THRESH_MAX:
                /* val should be 0 ~ 63(0x3F) */
                if (set_val > DA7218_ALC_THRESHOLD_MAX_MASK)
                        goto alc_err_return;

                /* -1.5 dBFS step, 0: 0 dBFS, 63: -94.5dBFS
                 */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_THRESHOLD_MAX_SHIFT;
                mask = DA7218_ALC_THRESHOLD_MAX_MASK;
                reg = DA7218_ALC_TARGET_MAX;
                break;
        case HAL_ALC_THRESH_MIN:
                /* val should be 0 ~ 63(0x3F) */
                if (set_val > DA7218_ALC_THRESHOLD_MIN_MASK)
                        goto alc_err_return;

                /* -1.5 dBFS step, 0: 0 dBFS, 63: -94.5dBFS
                 */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_THRESHOLD_MIN_SHIFT;
                mask = DA7218_ALC_THRESHOLD_MIN_MASK;
                reg = DA7218_ALC_TARGET_MIN;
                break;
        case HAL_ALC_NOISE:
                /* val should be 0 ~ 63(0x3F) */
                if (set_val > DA7218_ALC_THRESHOLD_MAX)
                        goto alc_err_return;

                /* -1.5 dBFS step, 0: 0 dBFS, 63: -94.5dBFS
                 */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_NOISE_SHIFT;
                mask = DA7218_ALC_NOISE_MASK;
                reg = DA7218_ALC_NOISE;
                break;
        case HAL_ALC_HOLD:
                /* val should be 0 ~ 15(0xF) */
                if (set_val > DA7218_ALC_HOLD_MAX)
                        goto alc_err_return;
                /* set_val: da7218_alc_hold_t */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_HOLD_SHIFT;
                mask = DA7218_ALC_HOLD_MASK;
                reg = DA7218_ALC_CTRL3;
                break;
        case HAL_ALC_ATT:
                /* val should be 0 ~ 13 */
                if (set_val > DA7218_ALC_ATTACK_MAX)
                        goto alc_err_return;
                /* set_val: da7218_alc_att_t */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_ATTACK_SHIFT;
                mask = DA7218_ALC_ATTACK_MASK;
                reg = DA7218_ALC_CTRL2;
                break;
        case HAL_ALC_REL:
                /* val should be 0 ~ 13 */
                if (set_val > DA7218_ALC_RELEASE_MAX)
                        goto alc_err_return;
                /* set_val: da7218_alc_rel_t */
                alc_val = set_val;
                alc_val <<= DA7218_ALC_RELEASE_SHIFT;
                mask = DA7218_ALC_RELEASE_MASK;
                reg = DA7218_ALC_CTRL2;
                break;

        default:
                goto alc_err_return;
        }

        da721x_i2c_update_bits(reg, mask, alc_val);
        if (reg == DA7218_ALC_CTRL1)
                da7218_alc_calib(1);
        return 0;

        alc_err_return:
        diag_err("Invalid ctrl\n");
        return -EINVAL;

}

/*
 * High Pass filter
 */

/*
 * voice high pass filter corner freq : freq_sel
 * voc_hpf_corn_t
 */
int da7218_set_voice_hpf_corner(enum hpf_sel_t filter_sel, u8 freq_sel)
{
        u8 reg;

        switch (filter_sel) {
        case DA7218_IN_1_HPF:
                reg = DA7218_IN_1_HPF_FILTER_CTRL;
                break;
        case DA7218_IN_2_HPF:
                reg = DA7218_IN_2_HPF_FILTER_CTRL;
                break;
        case DA7218_OUT_1_HPF:
                reg = DA7218_OUT_1_HPF_FILTER_CTRL;
                break;
        default:
                return -EINVAL;
        }

        da721x_i2c_update_bits(reg,
                DA7218_OUT_1_VOICE_HPF_CORNER_MASK,
                freq_sel<<DA7218_OUT_1_VOICE_HPF_CORNER_SHIFT);
        return 0;
}

/*
 * audio high pass filter corner freq : freq_sel
 * aud_hpf_corn_t
 */
int da7218_set_aud_hpf_corner(enum hpf_sel_t filter_sel, u8 freq_sel)
{
        u8 reg;

        switch (filter_sel) {
        case DA7218_IN_1_HPF:
                reg = DA7218_IN_1_HPF_FILTER_CTRL;
                break;
        case DA7218_IN_2_HPF:
                reg = DA7218_IN_2_HPF_FILTER_CTRL;
                break;
        case DA7218_OUT_1_HPF:
                reg = DA7218_OUT_1_HPF_FILTER_CTRL;
                break;
        default:
                return -EINVAL;
        }

        da721x_i2c_update_bits(reg,
                DA7218_OUT_1_AUDIO_HPF_CORNER_MASK,
                freq_sel<<DA7218_OUT_1_AUDIO_HPF_CORNER_SHIFT);
        return 0;
}

/*
 * mode_sel : hpf_mod_t
 * Disable(0)/ audio hpf en(1)/ voice hpf en(2).
 */
int da7218_filt_hpf_put(enum hpf_sel_t filter_sel, u8 mode_sel)
{
        unsigned int bits;
        u8 reg;

        switch (mode_sel) {
        case DA7218_HPF_MODE_DISABLED:
                bits = DA7218_HPF_DISABLED;
                break;
        case DA7218_HPF_MODE_AUDIO:
                bits = DA7218_HPF_AUDIO_EN;
                break;
        case DA7218_HPF_MODE_VOICE:
                bits = DA7218_HPF_VOICE_EN;
                break;
        default:
                return -EINVAL;
        }
        switch (filter_sel) {
        case DA7218_IN_1_HPF:
                reg = DA7218_IN_1_HPF_FILTER_CTRL;
                break;
        case DA7218_IN_2_HPF:
                reg = DA7218_IN_2_HPF_FILTER_CTRL;
                break;
        case DA7218_OUT_1_HPF:
                reg = DA7218_OUT_1_HPF_FILTER_CTRL;
                break;
        default:
                return -EINVAL;
        }

        da721x_i2c_update_bits(reg,
                DA7218_HPF_MODE_MASK, bits);
        return 0;
}

int da7218_filt_hpf_get(enum hpf_sel_t filter_sel, u8 *mode_sel)
{
        u8 reg;
        int val;

        switch (filter_sel) {
        case DA7218_IN_1_HPF:
                reg = DA7218_IN_1_HPF_FILTER_CTRL;
                break;
        case DA7218_IN_2_HPF:
                reg = DA7218_IN_2_HPF_FILTER_CTRL;
                break;
        case DA7218_OUT_1_HPF:
                reg = DA7218_OUT_1_HPF_FILTER_CTRL;
                break;
        default:
                return -EINVAL;
        }

        val = da721x_i2c_reg_read(reg) & DA7218_HPF_MODE_MASK;
        if (val < 0)
                return -EINVAL;

        switch (val) {
        case DA7218_HPF_AUDIO_EN:
                *mode_sel = DA7218_HPF_MODE_AUDIO;
                break;
        case DA7218_HPF_VOICE_EN:
                *mode_sel = DA7218_HPF_MODE_VOICE;
                break;
        default:
                *mode_sel = DA7218_HPF_MODE_DISABLED;
                break;
        }
        return 0;
}

#ifdef DA7218_JACK_DETECTION
int da7218_hpldet_ctrl(u8 jack)
{
        if (jack) {
                da721x_i2c_update_bits(DA7218_HPLDET_JACK,
                        DA7218_HPLDET_JACK_EN_MASK,
                        DA7218_HPLDET_JACK_EN_MASK);
        } else {
                da721x_i2c_update_bits(DA7218_HPLDET_JACK,
                        DA7218_HPLDET_JACK_EN_MASK, 0);
        }
        return 0;
}

int da7218_hpldet_irq_report(void)
{
        u8 jack_status;
        int report;

        jack_status = da721x_i2c_reg_read(DA7218_EVENT_STATUS);

        if (jack_status & DA7218_HPLDET_JACK_STS_MASK)
                report = DA7218_JACK_HEADPHONE;
        else
                report = DA7218_NO_JACK;

        /* report Jack detection */
        diag_info("Jack detection report = %x\n", report);
        return report;
}

int da7218_hpldet_irq_call_back(int *report)
{
        u8 status;

        diag_info("da7218_hpldet_irq_call_back\n");

        /* Read IRQ status reg */
        status = da721x_i2c_reg_read(DA7218_EVENT);
        if (!status)
                return 0;

        /* Handle HP detect related interrupts */
        if (status & DA7218_HPLDET_JACK_EVENT_MASK)
                *report = da7218_hpldet_irq_report();

        /* Clear interrupts */
        da721x_i2c_reg_write(DA7218_EVENT, status);

        return 0;
}

int da7218_hpldet_status(void)
{
        return da721x_i2c_reg_read(DA7218_EVENT_STATUS);
}
#endif

int da7218_codec_path_enable(enum path_ctrl_type route)
{
        int ret = 0;

        /* Sanity check */
        if (route >= CODEC_MAX_PATHS) {
                diag_err("Invalid Path\n");
                return -EINVAL;
        }

        /* check path enabled before */
        if (da7218_paths_table[route] == CODEC_PATH_ON) {
                diag_info("path [%d] is already enabled\n", route);
                return 0;
        }

        /* Check register */
        switch (route) {
        case CODEC_MIC_TO_DAI:
                ret = da7218_execute_script(da7218_mic_to_dai);
                break;
        case CODEC_D_MIC_TO_DAI:
                ret = da7218_execute_script(da7218_d_mic_to_dai);
                break;
        case CODEC_DAI_TO_HP:
                da7218_cp_control(1);
                ret = da7218_execute_script(da7218_dai_to_HP);
                break;
        case CODEC_ADC_TO_DAC:
                ret = da7218_execute_script(da7218_adc_to_dac);
                break;
        default:
                diag_info("Invalid route:%d\n", route);
                break;
        }

        if (ret < 0)
                return ret;

        /* Update table */
        da7218_paths_table[route] = CODEC_PATH_ON;
        diag_info("codec path rount(%d) has been enabled\n", route);
        return ret;
}

int da7218_codec_path_disable(enum path_ctrl_type route)
{
        int ret = 0;

        /* Sanity check */
        if (route >= CODEC_MAX_PATHS) {
                diag_info("%s\n", "Invalid Path");
                return -EINVAL;
        }

        /* check path enabled before */
        if (da7218_paths_table[route] == CODEC_PATH_OFF) {
                diag_info("path [%d] is already disabled\n", route);
                return 0;
        }
        diag_info("da7218_codec_path_disable(%d)\n", route);

        /* Check register */
        switch (route) {
        case CODEC_MIC_TO_DAI:
                ret = da7218_execute_script(da7218_mic_to_dai_off);
                break;
        case CODEC_D_MIC_TO_DAI:
                ret = da7218_execute_script(da7218_d_mic_to_dai_off);
                break;
        case CODEC_DAI_TO_HP:
                ret = da7218_execute_script(da7218_dai_to_HP_off);
                da7218_cp_control(0);
                break;
        case CODEC_ADC_TO_DAC:
                ret = da7218_execute_script(da7218_adc_to_dac_off);
                break;
        default:
                diag_info("Invalid route:%d\n", route);
                break;
        }

        if (ret < 0)
                return ret;

        /* Update table */
        da7218_paths_table[route] = CODEC_PATH_OFF;
        return 0;
}

/* set CODEC to Active Mode */
int da7218_suspend(void)
{
        diag_info("da7218_suspend\n");
        return da721x_i2c_reg_write(DA7218_SYSTEM_ACTIVE, 0x00);
}

/* set CODEC to Standby Mode */
int da7218_resume(void)
{
        diag_info("da7218_resume\n");
        return da721x_i2c_reg_write(DA7218_SYSTEM_ACTIVE, 0x01);
}

/* Initialisation DA7218 */
int da7218_codec_init(void)
{
        /* codec reset */
        da721x_i2c_reg_write(DA7218_SOFT_RESET, 0x80);
        mdelay(20);
        if (da7218_execute_script(da7218_codec_init_scr))
                return -EIO;
        if (da7218_set_user_data())
                return -EIO;

#ifdef DA7218_SINGLE_SUPPLY_MODE
        da721x_i2c_reg_write(DA7218_HP_DIFF_UNLOCK,
                DA7218_HP_DIFF_UNLOCK_VAL);
        da721x_i2c_update_bits(DA7218_HP_DIFF_CTRL,
                DA7218_HP_AMP_SINGLE_SUPPLY_EN_MASK,
                1 << DA7218_HP_AMP_SINGLE_SUPPLY_EN_SHIFT);
#endif

#ifdef DA7218_JACK_DETECTION
        da7218_hpldet_ctrl(1);
#endif
        return 0;
}

/* Set codec registers to default values */
int da7218_set_codec_default(void)
{
        return da7218_execute_script(da7218_reg_defaults);
}

/* Platform depencecies */
int da7218_set_user_data(void)
{
        int i = 0;

        /* BiQuad Filters init */
        da721x_i2c_update_bits(DA7218_OUT_1L_FILTER_CTRL,
                DA7218_OUT_1L_FILTER_EN_MASK,
                DA7218_OUT_1L_FILTER_EN_MASK);

        diag_info("sizeof(da7218_5biquad_cfg) = %d\n",
                sizeof(da7218_5biquad_cfg));
        while (i < sizeof(da7218_5biquad_cfg)) {
                da721x_i2c_reg_write(DA7218_OUT_1_BIQ_5STAGE_DATA,
                        da7218_5biquad_cfg[i++]);
                da721x_i2c_reg_write(DA7218_OUT_1_BIQ_5STAGE_ADDR,
                        da7218_5biquad_cfg[i++]);
        }
        i = 0;
        diag_info("sizeof(da7218_st_3biquad_cfg) = %d\n",
                sizeof(da7218_st_3biquad_cfg));
        while (i < sizeof(da7218_st_3biquad_cfg)) {
                da721x_i2c_reg_write(DA7218_SIDETONE_BIQ_3STAGE_DATA,
                        da7218_st_3biquad_cfg[i++]);
                da721x_i2c_reg_write(DA7218_SIDETONE_BIQ_3STAGE_ADDR,
                        da7218_st_3biquad_cfg[i++]);
        }
        da721x_i2c_update_bits(DA7218_OUT_1L_FILTER_CTRL,
                DA7218_OUT_1L_FILTER_EN_MASK, 0);

#ifdef DA7218_JACK_DETECTION
        da721x_i2c_update_bits(DA7218_HPLDET_JACK,
                DA7218_HPLDET_JACK_RATE_MASK
                | DA7218_HPLDET_JACK_DEBOUNCE_MASK
                | DA7218_HPLDET_JACK_THR_MASK,
                DA7218_OF_JACK_RATE
                << DA7218_HPLDET_JACK_RATE_SHIFT
                | DA7218_OF_JACK_DEBOUNC
                << DA7218_HPLDET_JACK_DEBOUNCE_SHIFT
                | DA7218_OF_JACK_THR
                << DA7218_HPLDET_JACK_THR_SHIFT);

        da721x_i2c_reg_write(DA7218_HPLDET_CTRL,
                DA7218_OF_COMP_INV
                << DA7218_HPLDET_COMP_INV_SHIFT
                | DA7218_OF_HYST
                << DA7218_HPLDET_HYST_EN_SHIFT
                | DA7218_OF_DISCHARGE
                << DA7218_HPLDET_DISCHARGE_EN_SHIFT);
#endif

        return da7218_execute_script_mask(da7218_pdata_setup);
}

/*
 * DEBUG & TEST Functions
 */

/* Read back all registers and print out */
void da7218_dump_all_registers(char *phase)
{
#ifdef DA721X_DEBUG
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
#else
        return;
#endif
}
