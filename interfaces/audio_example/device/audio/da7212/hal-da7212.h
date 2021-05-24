/*
 * hal-da7212.h - DA7212 audio CODEC Hardware Abstraction Layer functions
 *
 * Copyright (C) 2015 Dialog Semiconductor Ltd. and its Affiliates, unpublished
 * work. This computer program includes Confidential, Proprietary Information
 * and is a Trade Secret of Dialog Semiconductor Ltd. and its Affiliates.
 * All use, disclosure, and/or reproduction is prohibited unless authorized
 * in writing. All Rights Reserved.
 */

#ifndef _HAL_DA7212_H
#define _HAL_DA7212_H

#include "da7212.h"

/* Check types or define it */
#ifndef u8
typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
#endif

/*
 * DA7212 HAL Definitions
 */

/* use case related */
#define DA7212_SAMPLE_USE_CSES  1

/* DAI, PLL related */
#define DA7212_CODEC_SLAVE      1
#define DA7212_CODEC_MASTER     0
#define MCLK24M                 24000000
#define MCLK12M                 12000000
#define MCLK32M                 32000000

#define DA7212_SYSCLK_MCLK      0
#define DA7212_SYSCLK_PLL       1
#define DA7212_SYSCLK_PLL_SRM   2
#define DA7212_SYSCLK_PLL_32KHZ 3

/* Mic Bias */
enum micbias_voltage_t {
        DA7212_MICBIAS_1_6V = 0,
        DA7212_MICBIAS_2_2V,
        DA7212_MICBIAS_2_5V,
        DA7212_MICBIAS_3_0V,
};

/* Charge pump */
enum{
        CPVDD_STANDBY = 0,
        CPVDD_RESERVED,
        CPVDD_DIV2,
        CPVDD_DIV1,
        CPVDD_MAX
};

enum cp_mchange_t {
        DA7212_CP_MCHANGE_LARGEST_VOL = 1,
        DA7212_CP_MCHANGE_DAC_VOL,
        DA7212_CP_MCHANGE_SIGNAL_MAG,
};

enum cp_fcontrol_t {
        DA7212_CP_FCONTROL_1MHZ = 0,
        DA7212_CP_FCONTROL_500KHZ,
        DA7212_CP_FCONTROL_250KHZ,
        DA7212_CP_FCONTROL_125KHZ,
        DA7212_CP_FCONTROL_63KHZ,
        DA7212_CP_FCONTROL_0KHZ,
};

enum cp_tau_delay_t {
        DA7212_CP_TAU_DELAY_0MS = 0,
        DA7212_CP_TAU_DELAY_2MS,
        DA7212_CP_TAU_DELAY_4MS,
        DA7212_CP_TAU_DELAY_16MS,
        DA7212_CP_TAU_DELAY_64MS,
        DA7212_CP_TAU_DELAY_128MS,
        DA7212_CP_TAU_DELAY_256MS,
        DA7212_CP_TAU_DELAY_512MS,
};

/* controls list */
enum hal_controls_t {
        DA7212_MIC1,
        DA7212_MIC2,
        DA7212_MIXIN_L,
        DA7212_MIXIN_R,
        DA7212_AUX_L,
        DA7212_AUX_R,
        DA7212_ADC_L,
        DA7212_ADC_R,
        DA7212_DAC_L,
        DA7212_DAC_R,
        DA7212_MIXOUT_L,
        DA7212_MIXOUT_R,
        DA7212_HP_L,
        DA7212_HP_R,
        DA7212_LINE,
        DA7212_CTRL_MAX,
};

enum da7212_amp_ctrl_t {
        HAL_AMP_OFF = 0,
        HAL_AMP_ON = 1,
        HAL_AMP_MAX,
};

#define SYSMOD_DACR     0x80
#define SYSMOD_DACL     0x40
#define SYSMOD_HP_R     0x20
#define SYSMOD_HP_L     0x10
#define SYSMOD_SPK      0x08
#define SYSMOD_AUXR     0x04
#define SYSMOD_AUXL     0x02
#define SYSMOD_DONE     0x01

#define PATHSYS_SPK     (SYSMOD_DACR | SYSMOD_SPK | SYSMOD_DONE)
#define PATHSYS_HPL     (SYSMOD_DACL | SYSMOD_HP_L | SYSMOD_DONE)
#define PATHSYS_HPR     (SYSMOD_DACR | SYSMOD_HP_R | SYSMOD_DONE)
#define PATHSYS_HPLR    (SYSMOD_DACL | SYSMOD_DACR | SYSMOD_HP_L | SYSMOD_HP_R | SYSMOD_DONE)

enum mixout_right_sel_t {
        MIXOUTR_NONE = 0x00,
        MIXOUTR_AUX_R = 0x01,
        MIXOUTR_MIXINR = 0x02,
        MIXOUTR_MIXINL = 0x04,
        MIXOUTR_DACR = 0x08,
        MIXOUTR_AUXR_INV = 0x10,
        MIXOUTR_MIXINR_INV = 0x20,
        MIXOUTR_MIXINL_INV = 0x40,
        MIXOUTR_MAX = 0xFF,
};
enum mixout_left_sel_t {
        MIXOUTL_NONE = 0x00,
        MIXOUTL_AUX_L = 0x01,
        MIXOUTL_MIXINL = 0x02,
        MIXOUTL_MIXINR = 0x04,
        MIXOUTL_DACL = 0x08,
        MIXOUTL_AUXL_INV = 0x10,
        MIXOUTL_MIXINL_INV = 0x20,
        MIXOUTL_MIXINR_INV = 0x40,
        MIXOUTL_MAX = 0xFF,
};

enum mixin_left_sel_t {
        MIXINL_NONE = 0x00,
        MIXINL_AUX_L = 0x01,
        MIXINL_MIC_1 = 0x02,
        MIXINL_MIC_2 = 0x04,
        MIXINL_MIXIN_R = 0x08,
        MIXINL_DMIC_L = 0x80,
        MIXINL_MAX = 0xFF,
};
enum mixin_right_sel_t {
        MIXINR_NONE = 0x00,
        MIXINR_AUX_R = 0x01,
        MIXINR_MIC_2 = 0x02,
        MIXINR_MIC_1 = 0x04,
        MIXINR_MIXIN_L = 0x08,
        MIXINR_DMIC_R = 0x80,
        MIXINR_MAX = 0xFF,
};
enum rout_dac_src_t {
        DACSRC_ADC_L = 0x00,
        DACSRC_ADC_R = 0x01,
        DACSRC_DIN_L = 0x02,
        DACSRC_DIN_R = 0x03,
        DACSRC_MAX = 0x04,
};
enum rout_dai_src_t {
        DAISRC_ADC_L = 0x00,
        DAISRC_ADC_R = 0x01,
        DAISRC_DIN_L = 0x02,
        DAISRC_DIN_R = 0x03,
        DAISRC_MAX = 0x04,
};

enum sidtone_sel_t {
        IN_FILTER1L = 0,
        IN_FILTER1R,
        IN_FILTER2L,
        IN_FILTER2R,
        INVAL_SIDE_TONE,
};

enum mic_insel_t {
        MIC_IN_DIFF = 0,
        MIC_P_SEL,
        MIC_N_SEL,
        MIC_SEL_INVAL,
};

/* DMIC */
enum dmic_data_sel_t {
        DA7212_DMIC_DATA_LRISE_RFALL = 0,
        DA7212_DMIC_DATA_LFALL_RRISE,
};

enum dmic_samplephase_t {
        DA7212_DMIC_SAMPLE_ON_CLKEDGE = 0,
        DA7212_DMIC_SAMPLE_BETWEEN_CLKEDGE,
};

enum dmic_clk_rate_t {
        DA7212_DMIC_CLK_3_0MHZ = 0,
        DA7212_DMIC_CLK_1_5MHZ,
};

/* High-Pass Filter */
enum hpf_mod_t {
        DA7212_HPF_MODE_DISABLED = 0,
        DA7212_HPF_MODE_AUDIO,
        DA7212_HPF_MODE_VOICE,
        DA7212_HPF_MODE_MAX,
};

enum hpf_sel_t {
        DA7212_IN_1_HPF = 0,
        DA7212_IN_2_HPF,
        DA7212_OUT_1_HPF,
};

enum aud_hpf_corn_t {
        DA7212_AUD_2HZ = 0,
        DA7212_AUD_4HZ,
        DA7212_AUD_8HZ,
        DA7212_AUD_16HZ,
};

enum voc_hpf_corn_t {
        DA7212_VOC_2_5HZ = 0,
        DA7212_VOC_25HZ,
        DA7212_VOC_50HZ,
        DA7212_VOC_100HZ,
        DA7212_VOC_150HZ,
        DA7212_VOC_200HZ,
        DA7212_VOC_300HZ,
        DA7212_VOC_400HZ,
};

/* IO Voltage */
enum da7212_io_voltage_lvl {
        DA7212_IO_VOLTAGE_LVL_1_2V_2_5V = 0,
        DA7212_IO_VOLTAGE_LVL_2_5V_3_6V,
};

/* LDO */
enum da7212_ldo_lvl_sel {
        DA7212_LDO_LVL_SEL_1_05V = 0,
        DA7212_LDO_LVL_SEL_1_10V,
        DA7212_LDO_LVL_SEL_1_20V,
        DA7212_LDO_LVL_SEL_1_40V,
};

/* ALC */
enum da7212_alc_ctrl_t {
        HAL_ALC_INVAL,
        HAL_ALC_CH1_SW,
        HAL_ALC_CH2_SW,
        HAL_ALC_ANTICLIP_SW,
        HAL_ALC_ANTICLIP_STEP,
        HAL_ALC_ANA_GAINMAX,
        HAL_ALC_ANA_GAINMIN,
        HAL_ALC_ATTEN_MAX,
        HAL_ALC_GAIN_MAX,
        HAL_ALC_THRESH_MAX,
        HAL_ALC_THRESH_MIN,
        HAL_ALC_NOISE,
        HAL_ALC_HOLD,
        HAL_ALC_ATT,
        HAL_ALC_REL,
        HAL_ALC_MAX,
};

/* HAL_ALC_HOLD  */
enum da7212_alc_hold_t {
        HAL_ALCHOLD_62_fs = 0,
        HAL_ALCHOLD_124_fs,
        HAL_ALCHOLD_248_fs,
        HAL_ALCHOLD_496_fs,
        HAL_ALCHOLD_992_fs,
        HAL_ALCHOLD_1984_fs,
        HAL_ALCHOLD_3968_fs,
        HAL_ALCHOLD_7936_fs,
        HAL_ALCHOLD_15872_fs,
        HAL_ALCHOLD_31744_fs,
        HaL_ALCHOLD_63488_fs,
        HAL_ALCHOLD_126976_fs,
        HAL_ALCHOLD_253952_fs,
        HAL_ALCHOLD_507904_fs,
        HAL_ALCHOLD_1015808_fs,
        HAL_ALCHOLD_2031616_fs,
        HAL_ALCHOLD_MAX,
};

/* ALC Attack Rate */
enum da7212_alc_att_t {
        HAL_ALCATT_7_33_fs = 0,
        HAL_ALCATT_14_66_fs,
        HAL_ALCATT_29_32_fs,
        HAL_ALCATT_58_64_fs,
        HAL_ALCATT_117_3_fs,
        HAL_ALCATT_234_6_fs,
        HAL_ALCATT_469_1_fs,
        HAL_ALCATT_938_2_fs,
        HAL_ALCATT_1876_fs,
        HAL_ALCATT_3753_fs,
        HAL_ALCATT_7506_fs,
        HAL_ALCATT_15012_fs,
        HAL_ALCATT_30024_fs,
        HAL_ALCATT_MAX,
};

/* HAL_ALC_ATTREL  */
enum da7212_alc_rel_t {
        HAL_ALCREL_28_66_fs = 0,
        HAL_ALCREL_57_33_fs,
        HAL_ALCREL_114_6_fs,
        HAL_ALCREL_229_3_fs,
        HAL_ALCREL_458_6_fs,
        HAL_ALCREL_917_1_fs,
        HAL_ALCREL_1834_fs,
        HAL_ALCREL_3668_fs,
        HAL_ALCREL_7337_fs,
        HAL_ALCREL_14674_fs,
        HAL_ALCREL_29348_fs,
        HAL_ALCREL_MAX,
};

/* script type */
struct codec_scr_type {
        u16 reg;
        u8  val;
};

struct codec_scr_mask_type {
        u16 reg;
        u8  mask;
        u8  val;
};

struct codec_path_type {
        u8 reg;
        u8 mask;
        u8 turn_on;
        u8 turn_off;
};

/*
 * Local Constant Definitions
 */

#define EMPTY                   0xFF
#define CODEC_PATH_ON           1
#define CODEC_PATH_OFF          0

#define SEQ_END                 0xFFFF
#define CODEC_mDELAY            0xFF00

/* Definitions for each Gains(val/100 = dB) */
#define MIC_PGA_MIN             (-600)
#define MIC_PGA_MAX             (3600)
#define MIC_PGA_STEP            600
#define AUX_PGA_MIN             (-5400)
#define AUX_PGA_MAX             (1500)
#define AUX_PGA_STEP            150

#define MIXIN_PGA_MIN           (-450)
#define MIXIN_PGA_MAX           (1800)
#define MIXIN_PGA_STEP          150

#define ADC_PGA_MIN             (-7725)
#define ADC_PGA_MAX             (1200)
#define DAC_PGA_MIN             (-7725)
#define DAC_PGA_MAX             (1200)
#define DIGITAL_PGA_STEP        75

#define HP_PGA_MIN              (-5700)
#define HP_PGA_MAX              (600)
#define HP_PGA_STEP             100
#define SPK_PGA_MIN             (-4800)
#define SPK_PGA_MAX             (1500)
#define OUT_PGA_STEP            100

#define DMIX_PGA_MIN            (-4200)
#define DMIX_PGA_MAX            (450)
#define DMIX_PGA_STEP           150
#define DMIX_GAIN_MASK          0x1f

#define CODEC_UNMUTE            0
#define CODEC_MUTE              1

/*
 * Function list
 */

int da7212_codec_init(void);
int da7212_i2c_init(void);
int da7212_mute(enum hal_controls_t controls, int mute);
int da7212_set_vol_dB(enum hal_controls_t controls, int vol);
int da7212_set_sr(int sampling_rate);
int da7212_set_dai_fmt(void);
int da7212_alc_calib(void);
void da7212_get_path_record_mic1(u8 *amp_tbl);
void da7212_get_path_record_mic2(u8 *amp_tbl);
int da7212_set_user_data(void);
int da7212_codec_path_update(u8 *amp_tbl);
void da7212_get_codec_path_status(u8 *amp_tbl);

#if DA7212_SAMPLE_USE_CSES
void da7212_get_path_playback_hp(u8 *amp_tbl);
void da7212_get_path_playback_spk(u8 *amp_tbl);
void da7212_get_path_record_aux(u8 *amp_tbl);
void da7212_get_path_record_mic1(u8 *amp_tbl);
void da7212_get_path_record_mic2(u8 *amp_tbl);
void da7212_get_path_record_aux(u8 *amp_tbl);
void da7212_get_path_aux_mixout_to_hp(u8 *amp_tbl);
void da7212_get_path_aux_mixout_to_line(u8 *amp_tbl);
#endif

int da7212_resume(void);
int da7212_suspend(void);
int da7212_set_codec_default(void);
void da7212_codec_path_clear(u8 *amp_tbl);
int da7212_set_dai_pll(int sampling_rate);
void da7212_dump_all_registers(char *phase);

#endif /* _HAL_DA7212_H */
