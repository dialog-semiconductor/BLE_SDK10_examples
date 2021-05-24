/*
 * hal-da7218.h - DA7218 audio CODEC Hardware Abstraction Layer functions
 *
 * Copyright (C) 2015 Dialog Semiconductor Ltd. and its Affiliates, unpublished
 * work. This computer program includes Confidential, Proprietary Information
 * and is a Trade Secret of Dialog Semiconductor Ltd. and its Affiliates.
 * All use, disclosure, and/or reproduction is prohibited unless authorized
 * in writing. All Rights Reserved.
 */

#ifndef _HAL_DA7218_H
#define _HAL_DA7218_H

#include "da7218.h"

/* Check types or define it */
#ifndef u8
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#endif

/*
 * Definitions for using either DA7217 or DA7218 device
 */
/* define this when you use DA7217 Device */
#undef DA7217_CODEC_HAL
/* define this when you use DA7218 Device */
#define DA7218_CODEC_HAL

#ifdef DA7218_CODEC_HAL
/* Support DA7218 only */
#define DA7218_JACK_DETECTION
#endif

/*
 * DA7218 HAL Definitions
 */

/* DAI, PLL related */
#define DA7218_CODEC_SLAVE      1
#define DA7218_CODEC_MASTER     0
#define MCLK32M                 32000000
#define MCLK24M                 24000000
#define MCLK12M                 12000000

#define DA7218_SYSCLK_MCLK      0
#define DA7218_SYSCLK_PLL       1
#define DA7218_SYSCLK_PLL_SRM   2
#define DA7218_SYSCLK_PLL_32KHZ 3

/* input components */
#define DA7218_AMIC1            (1<<0)
#define DA7218_AMIC2            (1<<1)
#define DA7218_AMIC12           (1<<2)

#define DA7218_DMIC1            (1<<4)
#define DA7218_DMIC2            (1<<5)
#define DA7218_DMIC12           (1<<6)

/* Mic Bias */
enum micbias_voltage_t {
        DA7218_MICBIAS_1_6V = 0,
        DA7218_MICBIAS_1_8V,
        DA7218_MICBIAS_2_0V,
        DA7218_MICBIAS_2_2V,
        DA7218_MICBIAS_2_4V,
        DA7218_MICBIAS_2_6V,
        DA7218_MICBIAS_2_8V,
        DA7218_MICBIAS_3_0V,
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
        DA7218_CP_MCHANGE_LARGEST_VOL = 1,
        DA7218_CP_MCHANGE_DAC_VOL,
        DA7218_CP_MCHANGE_SIGNAL_MAG,
};

enum cp_fcontrol_t {
        DA7218_CP_FCONTROL_1MHZ = 0,
        DA7218_CP_FCONTROL_500KHZ,
        DA7218_CP_FCONTROL_250KHZ,
        DA7218_CP_FCONTROL_125KHZ,
        DA7218_CP_FCONTROL_63KHZ,
        DA7218_CP_FCONTROL_0KHZ,
};

enum cp_tau_delay_t {
        DA7218_CP_TAU_DELAY_0MS = 0,
        DA7218_CP_TAU_DELAY_2MS,
        DA7218_CP_TAU_DELAY_4MS,
        DA7218_CP_TAU_DELAY_16MS,
        DA7218_CP_TAU_DELAY_64MS,
        DA7218_CP_TAU_DELAY_128MS,
        DA7218_CP_TAU_DELAY_256MS,
        DA7218_CP_TAU_DELAY_512MS,
};

/* controls list */
enum hal_controls_t {
        DA7218_INVALID,
        DA7218_MIC1,
        DA7218_MIC2,
        DA7218_MIXIN,
        DA7218_ADC1,
        DA7218_ADC2,
        DA7218_SIDE_TONE,
        DA7218_DACL,
        DA7218_DACR,
        DA7218_HP,
        DA7218_CTRL_MAX,
};

enum path_ctrl_type {
        CODEC_NO_PATHS,
        CODEC_MIC_TO_DAI,
        CODEC_D_MIC_TO_DAI,
        CODEC_DAI_TO_HP,
        CODEC_ADC_TO_DAC,
        CODEC_MAX_PATHS
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
        DA7218_DMIC_DATA_LRISE_RFALL = 0,
        DA7218_DMIC_DATA_LFALL_RRISE,
};

enum dmic_samplephase_t {
        DA7218_DMIC_SAMPLE_ON_CLKEDGE = 0,
        DA7218_DMIC_SAMPLE_BETWEEN_CLKEDGE,
};

enum dmic_clk_rate_t {
        DA7218_DMIC_CLK_3_0MHZ = 0,
        DA7218_DMIC_CLK_1_5MHZ,
};

/* High-Pass Filter */
enum hpf_mod_t {
        DA7218_HPF_MODE_DISABLED = 0,
        DA7218_HPF_MODE_AUDIO,
        DA7218_HPF_MODE_VOICE,
        DA7218_HPF_MODE_MAX,
};

enum hpf_sel_t {
        DA7218_IN_1_HPF = 0,
        DA7218_IN_2_HPF,
        DA7218_OUT_1_HPF,
};

enum aud_hpf_corn_t {
        DA7218_AUD_2HZ = 0,
        DA7218_AUD_4HZ,
        DA7218_AUD_8HZ,
        DA7218_AUD_16HZ,
};

enum voc_hpf_corn_t {
        DA7218_VOC_2_5HZ = 0,
        DA7218_VOC_25HZ,
        DA7218_VOC_50HZ,
        DA7218_VOC_100HZ,
        DA7218_VOC_150HZ,
        DA7218_VOC_200HZ,
        DA7218_VOC_300HZ,
        DA7218_VOC_400HZ,
};

/* IO Voltage */
enum da7218_io_voltage_lvl {
        DA7218_IO_VOLTAGE_LVL_2_5V_3_6V = 0,
        DA7218_IO_VOLTAGE_LVL_1_5V_2_5V,
};

/* LDO */
enum da7218_ldo_lvl_sel {
        DA7218_LDO_LVL_SEL_1_05V = 0,
        DA7218_LDO_LVL_SEL_1_10V,
        DA7218_LDO_LVL_SEL_1_20V,
        DA7218_LDO_LVL_SEL_1_40V,
};

/* HP Differential/Single Ended */
enum da7218_hp_diff_single {
        DA7218_HP_SINGLE_ENDED = 0,
        DA7218_HP_DIFFERENTIAL,
};

/* ALC */
enum da7218_alc_ctrl_t {
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
enum da7218_alc_hold_t {
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
enum da7218_alc_att_t {
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
enum da7218_alc_rel_t {
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
        u8 val;
};

struct codec_scr_mask_type {
        u16 reg;
        u8 mask;
        u8 val;
};

#ifdef DA7218_JACK_DETECTION
/* Headphone Detect */
enum da7218_hpldet_jack_rate {
        DA7218_HPLDET_JACK_RATE_5US = 0,
        DA7218_HPLDET_JACK_RATE_10US,
        DA7218_HPLDET_JACK_RATE_20US,
        DA7218_HPLDET_JACK_RATE_40US,
        DA7218_HPLDET_JACK_RATE_80US,
        DA7218_HPLDET_JACK_RATE_160US,
        DA7218_HPLDET_JACK_RATE_320US,
        DA7218_HPLDET_JACK_RATE_640US,
};

enum da7218_hpldet_jack_debounce {
        DA7218_HPLDET_JACK_DEBOUNCE_OFF = 0,
        DA7218_HPLDET_JACK_DEBOUNCE_2,
        DA7218_HPLDET_JACK_DEBOUNCE_3,
        DA7218_HPLDET_JACK_DEBOUNCE_4,
};

enum da7218_hpldet_jack_thr {
        DA7218_HPLDET_JACK_THR_84PCT = 0,
        DA7218_HPLDET_JACK_THR_88PCT,
        DA7218_HPLDET_JACK_THR_92PCT,
        DA7218_HPLDET_JACK_THR_96PCT,
};

enum da7218_jack_types {
        DA7218_NO_JACK = 0x0000,
        DA7218_JACK_HEADPHONE = 0x0001,
        DA7218_JACK_MICROPHONE = 0x0002,
        DA7218_JACK_HEADSET = DA7218_JACK_HEADPHONE | DA7218_JACK_MICROPHONE,
        DA7218_JACK_LINEOUT = 0x0004,
};
#endif

/*
 * Local Constant Definitions
 */

#define EMPTY 0xFF
#define CODEC_PATH_ON 1
#define CODEC_PATH_OFF 0

#define SEQ_END 0xFFFF
#define CODEC_REG_DELAY 0xFF00

/* Definitions for each Gains(val/100 = dB) */
#define MIC_PGA_MIN (-600)
#define MIC_PGA_MAX (3600)
#define MIC_PGA_STEP 600

#define MIXIN_PGA_MIN (-450)
#define MIXIN_PGA_MAX (1800)
#define MIXIN_PGA_STEP 150

#define ADC_PGA_MIN (-8325)
#define ADC_PGA_MAX (1200)
#define DAC_PGA_MIN (-8325)
#define DAC_PGA_MAX (1200)
#define DIGITAL_PGA_STEP 75

#define HP_PGA_MIN (-5700)
#define HP_PGA_MAX (600)
#define HP_PGA_STEP 150

#define DMIX_PGA_MIN (-4200)
#define DMIX_PGA_MAX (450)
#define DMIX_PGA_STEP 150
#define DMIX_GAIN_MASK 0x1f

#define CODEC_UNMUTE 0
#define CODEC_MUTE 1

/*
 * Function list
 */

int da7218_codec_init(void);
int da7218_mute(enum hal_controls_t controls, int mute);
int da7218_set_vol_dB(enum hal_controls_t controls, int vol);
int da7218_set_dmix_gain_dB(u8 reg , int vol);
int da7218_set_dai_tdm_slot(unsigned int tx_mask,
        unsigned int rx_mask, int slots, int slot_width);
int da7218_cp_control(u8 onoff);
int da7218_set_sample_rate(u32 adc_sr, u32 dac_sr);
int da7218_set_dai_fmt(void);
int da7218_sidetone_ctrl(u8 onoff, enum sidtone_sel_t in_sel, int vol);
int da7218_alc_calib(u8 set_on);
int da7218_alc_control(enum da7218_alc_ctrl_t ctrl, u8 set_val);
int da7218_set_voice_hpf_corner(enum hpf_sel_t filter_sel, u8 freq_sel);
int da7218_set_aud_hpf_corner(enum hpf_sel_t filter_sel, u8 freq_sel);
int da7218_filt_hpf_put(enum hpf_sel_t filter_sel, u8 mode_sel);
int da7218_filt_hpf_get(enum hpf_sel_t filter_sel, u8 *mode_sel);
#ifdef DA7218_JACK_DETECTION
int da7218_hpldet_ctrl(u8 jack);
int da7218_hpldet_irq_report(void);
int da7218_hpldet_irq_call_back(int *report);
int da7218_hpldet_status(void);
#endif
int da7218_set_user_data(void);
int da7218_codec_path_disable(enum path_ctrl_type route);
int da7218_codec_path_enable(enum path_ctrl_type route);
int da7218_resume(void);
int da7218_suspend(void);
int da7218_set_codec_default(void);
void da7218_dump_all_registers(char *phase);

#endif /* _HAL_DA7218_H */
