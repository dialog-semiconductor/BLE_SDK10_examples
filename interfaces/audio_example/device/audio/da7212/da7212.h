/*
 * da7212.h - DA7212 audio CODEC Hardware Abstraction Layer functions
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

#ifndef _DA7212_H
#define _DA7212_H

/*
 * Registers
 */

/* Status Registers */
#define DA7212_STATUS1                                  0x02
#define DA7212_PLL_STATUS                               0x03
#define DA7212_AUX_L_GAIN_STATUS                        0x04
#define DA7212_AUX_R_GAIN_STATUS                        0x05
#define DA7212_MIC_1_GAIN_STATUS                        0x06
#define DA7212_MIC_2_GAIN_STATUS                        0x07
#define DA7212_MIXIN_L_GAIN_STATUS                      0x08
#define DA7212_MIXIN_R_GAIN_STATUS                      0x09
#define DA7212_ADC_L_GAIN_STATUS                        0x0A
#define DA7212_ADC_R_GAIN_STATUS                        0x0B
#define DA7212_DAC_L_GAIN_STATUS                        0x0C
#define DA7212_DAC_R_GAIN_STATUS                        0x0D
#define DA7212_HP_L_GAIN_STATUS                         0x0E
#define DA7212_HP_R_GAIN_STATUS                         0x0F
#define DA7212_LINE_GAIN_STATUS                         0x10
#define DA7212_CIF_CTRL                                 0x1D

/* System Initialisation Registers */
#define DA7212_DIG_ROUTING_DAI                          0x21
#define DA7212_SR                                       0x22
#define DA7212_REFERENCES                               0x23
#define DA7212_PLL_FRAC_TOP                             0x24
#define DA7212_PLL_FRAC_BOT                             0x25
#define DA7212_PLL_INTEGER                              0x26
#define DA7212_PLL_CTRL                                 0x27
#define DA7212_DAI_CLK_MODE                             0x28
#define DA7212_DAI_CTRL                                 0x29
#define DA7212_DIG_ROUTING_DAC                          0x2A
#define DA7212_ALC_CTRL1                                0x2B

/* Input - Gain, Select and Filter Registers */
#define DA7212_AUX_L_GAIN                               0x30
#define DA7212_AUX_R_GAIN                               0x31
#define DA7212_MIXIN_L_SELECT                           0x32
#define DA7212_MIXIN_R_SELECT                           0x33
#define DA7212_MIXIN_L_GAIN                             0x34
#define DA7212_MIXIN_R_GAIN                             0x35
#define DA7212_ADC_L_GAIN                               0x36
#define DA7212_ADC_R_GAIN                               0x37
#define DA7212_ADC_FILTERS1                             0x38
#define DA7212_MIC_1_GAIN                               0x39
#define DA7212_MIC_2_GAIN                               0x3A

/* Output - Gain, Select and Filter Registers */
#define DA7212_DAC_FILTERS5                             0x40
#define DA7212_DAC_FILTERS2                             0x41
#define DA7212_DAC_FILTERS3                             0x42
#define DA7212_DAC_FILTERS4                             0x43
#define DA7212_DAC_FILTERS1                             0x44
#define DA7212_DAC_L_GAIN                               0x45
#define DA7212_DAC_R_GAIN                               0x46
#define DA7212_CP_CTRL                                  0x47
#define DA7212_HP_L_GAIN                                0x48
#define DA7212_HP_R_GAIN                                0x49
#define DA7212_LINE_GAIN                                0x4A
#define DA7212_MIXOUT_L_SELECT                          0x4B
#define DA7212_MIXOUT_R_SELECT                          0x4C

/* System Controller Registers */
#define DA7212_SYSTEM_MODES_INPUT                       0x50
#define DA7212_SYSTEM_MODES_OUTPUT                      0x51

/* Control Registers */
#define DA7212_AUX_L_CTRL                               0x60
#define DA7212_AUX_R_CTRL                               0x61
#define DA7212_MICBIAS_CTRL                             0x62
#define DA7212_MIC_1_CTRL                               0x63
#define DA7212_MIC_2_CTRL                               0x64
#define DA7212_MIXIN_L_CTRL                             0x65
#define DA7212_MIXIN_R_CTRL                             0x66
#define DA7212_ADC_L_CTRL                               0x67
#define DA7212_ADC_R_CTRL                               0x68
#define DA7212_DAC_L_CTRL                               0x69
#define DA7212_DAC_R_CTRL                               0x6A
#define DA7212_HP_L_CTRL                                0x6B
#define DA7212_HP_R_CTRL                                0x6C
#define DA7212_LINE_CTRL                                0x6D
#define DA7212_MIXOUT_L_CTRL                            0x6E
#define DA7212_MIXOUT_R_CTRL                            0x6F

/* Configuration Registers */
#define DA7212_LDO_CTRL                                 0x90
#define DA7212_IO_CTRL                                  0x91
#define DA7212_GAIN_RAMP_CTRL                           0x92
#define DA7212_MIC_CONFIG                               0x93
#define DA7212_PC_COUNT                                 0x94
#define DA7212_CP_VOL_THRESHOLD1                        0x95
#define DA7212_CP_DELAY                                 0x96
#define DA7212_CP_DETECTOR                              0x97
#define DA7212_DAI_OFFSET                               0x98
#define DA7212_DIG_CTRL                                 0x99
#define DA7212_ALC_CTRL2                                0x9A
#define DA7212_ALC_CTRL3                                0x9B
#define DA7212_ALC_NOISE                                0x9C
#define DA7212_ALC_TARGET_MIN                           0x9D
#define DA7212_ALC_TARGET_MAX                           0x9E
#define DA7212_ALC_GAIN_LIMITS                          0x9F
#define DA7212_ALC_ANA_GAIN_LIMITS                      0xA0
#define DA7212_ALC_ANTICLIP_CTRL                        0xA1
#define DA7212_ALC_ANTICLIP_LEVEL                       0xA2

#define DA7212_ALC_OFFSET_AUTO_M_L                      0xA3
#define DA7212_ALC_OFFSET_AUTO_U_L                      0xA4
#define DA7212_ALC_OFFSET_MAN_M_L                       0xA6
#define DA7212_ALC_OFFSET_MAN_U_L                       0xA7
#define DA7212_ALC_OFFSET_AUTO_M_R                      0xA8
#define DA7212_ALC_OFFSET_AUTO_U_R                      0xA9
#define DA7212_ALC_OFFSET_MAN_M_R                       0xAB
#define DA7212_ALC_OFFSET_MAN_U_R                       0xAC
#define DA7212_ALC_CIC_OP_LVL_CTRL                      0xAD
#define DA7212_ALC_CIC_OP_LVL_DATA                      0xAE
#define DA7212_DAC_NG_SETUP_TIME                        0xAF
#define DA7212_DAC_NG_OFF_THRESHOLD                     0xB0
#define DA7212_DAC_NG_ON_THRESHOLD                      0xB1
#define DA7212_DAC_NG_CTRL                              0xB2

/*
 * Bit fields
 */

/* DA7212_PLL_STATUS = 0x03 */
#define DA7212_PLL_SRM_STATUS_SRM_LOCK                  (0x1 << 1)
#define DA7212_PLL_SRM_STATUS_PLL_LOCK                  (0x1 << 0)

/* DA7212_SR = 0x22 */
#define DA7212_SR_8000                                  (0x1 << 0)
#define DA7212_SR_11025                                 (0x2 << 0)
#define DA7212_SR_12000                                 (0x3 << 0)
#define DA7212_SR_16000                                 (0x5 << 0)
#define DA7212_SR_22050                                 (0x6 << 0)
#define DA7212_SR_24000                                 (0x7 << 0)
#define DA7212_SR_32000                                 (0x9 << 0)
#define DA7212_SR_44100                                 (0xA << 0)
#define DA7212_SR_48000                                 (0xB << 0)
#define DA7212_SR_88200                                 (0xE << 0)
#define DA7212_SR_96000                                 (0xF << 0)

/* DA7212_REFERENCES = 0x23 */
#define DA7212_BIAS_EN                                  (0x1 << 3)
#define DA7212_VMID_EN                                  (0x1 << 7)

/* DA7212_PLL_CTRL = 0x27 */
#define DA7212_PLL_INDIV_5_10_MHZ                       (0x0 << 2)
#define DA7212_PLL_INDIV_10_20_MHZ                      (0x1 << 2)
#define DA7212_PLL_INDIV_20_40_MHZ                      (0x2 << 2)
#define DA7212_PLL_INDIV_40_54_MHZ                      (0x3 << 2)
#define DA7212_PLL_INDIV_MASK                           (0x3 << 2)
#define DA7212_PLL_MCLK_SQR_EN                          (0x1 << 4)
#define DA7212_PLL_32K_MODE                             (0x1 << 5)
#define DA7212_PLL_SRM_EN                               (0x1 << 6)
#define DA7212_PLL_EN                                   (0x1 << 7)

/* DA7212_DAI_CLK_MODE = 0x28 */
#define DA7212_DAI_BCLKS_PER_WCLK_32                    (0x0 << 0)
#define DA7212_DAI_BCLKS_PER_WCLK_64                    (0x1 << 0)
#define DA7212_DAI_BCLKS_PER_WCLK_128                   (0x2 << 0)
#define DA7212_DAI_BCLKS_PER_WCLK_256                   (0x3 << 0)
#define DA7212_DAI_BCLKS_PER_WCLK_MASK                  (0x3 << 0)
#define DA7212_DAI_CLK_POL_INV                          (0x1 << 2)
#define DA7212_DAI_WCLK_POL_INV                         (0x1 << 3)
#define DA7212_DAI_CLK_EN_SLAVE_MODE                    (0x0 << 7)
#define DA7212_DAI_CLK_EN_MASTER_MODE                   (0x1 << 7)
#define DA7212_DAI_CLK_EN_MASK                          (0x1 << 7)

/* DA7212_DAI_CTRL = 0x29 */
#define DA7212_DAI_FORMAT_I2S_MODE                      (0x0 << 0)
#define DA7212_DAI_FORMAT_LEFT_J                        (0x1 << 0)
#define DA7212_DAI_FORMAT_RIGHT_J                       (0x2 << 0)
#define DA7212_DAI_FORMAT_MASK                          (0x3 << 0)
#define DA7212_DAI_WORD_LENGTH_S16_LE                   (0x0 << 2)
#define DA7212_DAI_WORD_LENGTH_S20_LE                   (0x1 << 2)
#define DA7212_DAI_WORD_LENGTH_S24_LE                   (0x2 << 2)
#define DA7212_DAI_WORD_LENGTH_S32_LE                   (0x3 << 2)
#define DA7212_DAI_WORD_LENGTH_MASK                     (0x3 << 2)
#define DA7212_DAI_OE_SHIFT                             6
#define DA7212_DAI_OE_MASK                              (0x1 << 6)
#define DA7212_DAI_EN_SHIFT                             7
#define DA7212_DAI_EN_MASK                              (0x1 << 7)

/* DA7212_DIG_ROUTING_DAI = 0x21 */
#define DA7212_DAI_L_SRC_SHIFT                          0
#define DA7212_DAI_L_SRC_MASK                           (0x3 << 0)
#define DA7212_DAI_R_SRC_SHIFT                          4
#define DA7212_DAI_R_SRC_MASK                           (0x3 << 4)
#define DA7212_DAI_SRC_MAX                              4

/* DA7212_DIG_ROUTING_DAC = 0x2A */
#define DA7212_DAC_L_SRC_SHIFT                          0
#define DA7212_DAC_L_SRC_MASK                           (0x3 << 0)
#define DA7212_DAC_L_MONO_SHIFT                         3
#define DA7212_DAC_R_SRC_SHIFT                          4
#define DA7212_DAC_R_SRC_MASK                           (0x3 << 4)
#define DA7212_DAC_R_MONO_SHIFT                         7
#define DA7212_DAC_SRC_MAX                              4
#define DA7212_DAC_MONO_MAX                             0x1

/* DA7212_ALC_CTRL1 = 0x2B */
#define DA7212_ALC_OFFSET_EN_SHIFT                      0
#define DA7212_ALC_OFFSET_EN_MAX                        0x1
#define DA7212_ALC_OFFSET_EN                            (0x1 << 0)
#define DA7212_ALC_SYNC_MODE                            (0x1 << 1)
#define DA7212_ALC_CALIB_MODE_MAN                       (0x1 << 2)
#define DA7212_ALC_L_EN_SHIFT                           3
#define DA7212_ALC_AUTO_CALIB_EN                        (0x1 << 4)
#define DA7212_ALC_CALIB_OVERFLOW                       (0x1 << 5)
#define DA7212_ALC_R_EN_SHIFT                           7
#define DA7212_ALC_EN_MAX                               0x1

/* DA7212_AUX_L/R_GAIN = 0x30/0x31 */
#define DA7212_AUX_AMP_GAIN_SHIFT                       0
#define DA7212_AUX_AMP_GAIN_MAX                         0x3F

/* DA7212_MIXIN_L/R_SELECT = 0x32/0x33 */
#define DA7212_DMIC_EN_SHIFT                            7
#define DA7212_DMIC_EN_MAX                              0x1

/* DA7212_MIXIN_L_SELECT = 0x32 */
#define DA7212_MIXIN_L_MIX_SELECT_AUX_L_SHIFT           0
#define DA7212_MIXIN_L_MIX_SELECT_MIC_1_SHIFT           1
#define DA7212_MIXIN_L_MIX_SELECT_MIC_1                 (0x1 << 1)
#define DA7212_MIXIN_L_MIX_SELECT_MIC_2_SHIFT           2
#define DA7212_MIXIN_L_MIX_SELECT_MIC_2                 (0x1 << 2)
#define DA7212_MIXIN_L_MIX_SELECT_MIXIN_R_SHIFT         3
#define DA7212_MIXIN_L_MIX_SELECT_MAX                   0x1

/* DA7212_MIXIN_R_SELECT =  0x33 */
#define DA7212_MIXIN_R_MIX_SELECT_AUX_R_SHIFT           0
#define DA7212_MIXIN_R_MIX_SELECT_MIC_2_SHIFT           1
#define DA7212_MIXIN_R_MIX_SELECT_MIC_2                 (0x1 << 1)
#define DA7212_MIXIN_R_MIX_SELECT_MIC_1_SHIFT           2
#define DA7212_MIXIN_R_MIX_SELECT_MIC_1                 (0x1 << 2)
#define DA7212_MIXIN_R_MIX_SELECT_MIXIN_L_SHIFT         3
#define DA7212_MIXIN_R_MIX_SELECT_MAX                   0x1
#define DA7212_MIC_BIAS_OUTPUT_SELECT_2                 (0x1 << 6)

/* DA7212_MIXIN_L/R_GAIN = 0x34/0x35 */
#define DA7212_MIXIN_AMP_GAIN_SHIFT                     0
#define DA7212_MIXIN_AMP_GAIN_MAX                       0xF

/* DA7212_ADC_L/R_GAIN = 0x36/0x37 */
#define DA7212_ADC_AMP_GAIN_SHIFT                       0
#define DA7212_ADC_AMP_GAIN_MAX                         0x7F

/* DA7212_ADC/DAC_FILTERS1 = 0x38/0x44 */
#define DA7212_VOICE_HPF_CORNER_SHIFT                   0
#define DA7212_VOICE_HPF_CORNER_MAX                     8
#define DA7212_VOICE_EN_SHIFT                           3
#define DA7212_VOICE_EN_MAX                             0x1
#define DA7212_AUDIO_HPF_CORNER_SHIFT                   4
#define DA7212_AUDIO_HPF_CORNER_MAX                     4
#define DA7212_HPF_EN_SHIFT                             7
#define DA7212_HPF_EN_MAX                               0x1

/* DA7212_MIC_1/2_GAIN = 0x39/0x3A */
#define DA7212_MIC_AMP_GAIN_SHIFT                       0
#define DA7212_MIC_AMP_GAIN_MAX                         0x7

/* DA7212_DAC_FILTERS5 = 0x40 */
#define DA7212_DAC_SOFTMUTE_EN_SHIFT                    7
#define DA7212_DAC_SOFTMUTE_EN_MAX                      0x1
#define DA7212_DAC_SOFTMUTE_RATE_SHIFT                  4
#define DA7212_DAC_SOFTMUTE_RATE_MAX                    7

/* DA7212_DAC_FILTERS2/3/4 = 0x41/0x42/0x43 */
#define DA7212_DAC_EQ_BAND_MAX                          0xF

/* DA7212_DAC_FILTERS2 = 0x41 */
#define DA7212_DAC_EQ_BAND1_SHIFT                       0
#define DA7212_DAC_EQ_BAND2_SHIFT                       4

/* DA7212_DAC_FILTERS2 = 0x42 */
#define DA7212_DAC_EQ_BAND3_SHIFT                       0
#define DA7212_DAC_EQ_BAND4_SHIFT                       4

/* DA7212_DAC_FILTERS4 = 0x43 */
#define DA7212_DAC_EQ_BAND5_SHIFT                       0
#define DA7212_DAC_EQ_EN_SHIFT                          7
#define DA7212_DAC_EQ_EN_MAX                            0x1

/* DA7212_DAC_L/R_GAIN = 0x45/0x46 */
#define DA7212_DAC_AMP_GAIN_SHIFT                       0
#define DA7212_DAC_AMP_GAIN_MAX                         0x7F

/* DA7212_HP_L/R_GAIN = 0x45/0x46 */
#define DA7212_HP_AMP_GAIN_SHIFT                        0
#define DA7212_HP_AMP_GAIN_MAX                          0x3F

/* DA7212_CP_CTRL = 0x47 */
#define DA7212_CP_EN_SHIFT                              7
#define DA7212_CP_EN_MASK                               (0x1 << 7)

/* DA7212_LINE_GAIN = 0x4A */
#define DA7212_LINE_AMP_GAIN_SHIFT                      0
#define DA7212_LINE_AMP_GAIN_MAX                        0x3F

/* DA7212_MIXOUT_L_SELECT = 0x4B */
#define DA7212_MIXOUT_L_SEL_AUX_L_SHIFT                 0
#define DA7212_MIXOUT_L_SEL_MIXIN_L_SHIFT               1
#define DA7212_MIXOUT_L_SEL_MIXIN_R_SHIFT               2
#define DA7212_MIXOUT_L_SEL_DAC_L_SHIFT                 3
#define DA7212_MIXLOUT_SEL_DACL_MASK\
        (1 << DA7212_MIXOUT_L_SEL_DAC_L_SHIFT)
#define DA7212_MIXOUT_L_SEL_AUX_L_INVERTED_SHIFT        4
#define DA7212_MIXOUT_L_SEL_MIXIN_L_INVERTED_SHIFT      5
#define DA7212_MIXOUT_L_SEL_MIXIN_R_INVERTED_SHIFT      6
#define DA7212_MIXOUT_L_SEL_MAX                         0x1

/* DA7212_MIXOUT_R_SELECT = 0x4C */
#define DA7212_MIXOUT_R_SEL_AUX_R_SHIFT                 0
#define DA7212_MIXOUT_R_SEL_MIXIN_R_SHIFT               1
#define DA7212_MIXOUT_R_SEL_MIXIN_L_SHIFT               2
#define DA7212_MIXOUT_R_SEL_DAC_R_SHIFT                 3
#define DA7212_MIXROUT_SEL_DACR_MASK\
        (1 << DA7212_MIXOUT_R_SEL_DAC_R_SHIFT)
#define DA7212_MIXOUT_R_SEL_AUX_R_INVERTED_SHIFT        4
#define DA7212_MIXOUT_R_SEL_MIXIN_R_INVERTED_SHIFT      5
#define DA7212_MIXOUT_R_SEL_MIXIN_L_INVERTED_SHIFT      6
#define DA7212_MIXOUT_R_SEL_MAX                         0x1

/*
 * DA7212_AUX_L/R_CTRL = 0x60/0x61,
 * DA7212_MIC_1/2_CTRL = 0x63/0x64,
 * DA7212_MIXIN_L/R_CTRL = 0x65/0x66,
 * DA7212_ADC_L/R_CTRL = 0x65/0x66,
 * DA7212_DAC_L/R_CTRL = 0x69/0x6A,
 * DA7212_HP_L/R_CTRL = 0x6B/0x6C,
 * DA7212_LINE_CTRL = 0x6D
 */
#define DA7212_MUTE_EN_SHIFT                            6
#define DA7212_MUTE_EN_MAX                              0x1
#define DA7212_MUTE_EN                                  (0x1 << 6)

/*
 * DA7212_AUX_L/R_CTRL = 0x60/0x61,
 */
#define DA7212_AUX_AMP_EN                               (0x1 << 7)

/*
 * DA7212_MIXIN_L/R_CTRL = 0x65/0x66,
 * DA7212_ADC_L/R_CTRL = 0x65/0x66,
 * DA7212_DAC_L/R_CTRL = 0x69/0x6A,
 * DA7212_HP_L/R_CTRL = 0x6B/0x6C,
 * DA7212_LINE_CTRL = 0x6D
 */
#define DA7212_GAIN_RAMP_EN_SHIFT                       5
#define DA7212_GAIN_RAMP_EN_MAX                         0x1
#define DA7212_GAIN_RAMP_EN                             (0x1 << 5)

/*
 * DA7212_AUX_L/R_CTRL = 0x60/0x61,
 * DA7212_MIXIN_L/R_CTRL = 0x65/0x66,
 * DA7212_HP_L/R_CTRL = 0x6B/0x6C,
 * DA7212_LINE_CTRL = 0x6D
 */
#define DA7212_ZC_EN_SHIFT                              4
#define DA7212_ZC_EN_MAX                                0x1

/*
 * DA7212_AUX_L/R_CTRL = 0x60/0x61,
 * DA7212_MIC_1/2_CTRL = 0x63/0x64,
 * DA7212_MIXIN_L/R_CTRL = 0x65/0x66,
 * DA7212_HP_L/R_CTRL = 0x6B/0x6C,
 * DA7212_MIXOUT_L/R_CTRL = 0x6E/0x6F,
 * DA7212_LINE_CTRL = 0x6D
 */
#define DA7212_AMP_EN_SHIFT                             7

/* DA7212_MIC_1/2_CTRL = 0x63/0x64 */
#define DA7212_MIC_AMP_IN_SEL_SHIFT                     2
#define DA7212_MIC_AMP_IN_SEL_MAX                       3
#define DA7212_MIC_IN_SEL_MASK                          (0x3 << 2)
#define DA7212_MIC_AMP_EN                               (0x1 << 7)

/* DA7212_MICBIAS_CTRL = 0x62 */
#define DA7212_MICBIAS1_LEVEL_SHIFT                     0
#define DA7212_MICBIAS1_LEVEL_MASK                      (0x3 << 0)
#define DA7212_MICBIAS1_EN_SHIFT                        3
#define DA7212_MICBIAS1_EN                              (0x1 << 3)
#define DA7212_MICBIAS2_LEVEL_SHIFT                     4
#define DA7212_MICBIAS2_LEVEL_MASK                      (0x3 << 4)
#define DA7212_MICBIAS2_EN_SHIFT                        7
#define DA7212_MICBIAS2_EN                              (0x1 << 7)

/* DA7212_MIXIN_L/R_CTRL = 0x65/0x66 */
#define DA7212_MIXIN_MIX_EN                             (0x1 << 3)

/* DA7212_ADC_L/R_CTRL = 0x67/0x68 */
#define DA7212_ADC_EN_SHIFT                             7
#define DA7212_ADC_EN                                   (0x1 << 7)

/* DA7212_DAC_L/R_CTRL =  0x69/0x6A*/
#define DA7212_DAC_EN_SHIFT                             7
#define DA7212_DAC_EN                                   (0x1 << 7)

/* DA7212_HP_L/R_CTRL = 0x6B/0x6C */
#define DA7212_HP_AMP_OE                                (0x1 << 3)

/* DA7212_LINE_CTRL = 0x6D */
#define DA7212_LINE_AMP_OE                              (0x1 << 3)

/* DA7212_MIXOUT_L/R_CTRL = 0x6E/0x6F */
#define DA7212_MIXOUT_MIX_EN                            (0x1 << 3)

/* DA7212_GAIN_RAMP_CTRL = 0x92 */
#define DA7212_GAIN_RAMP_RATE_SHIFT                     0
#define DA7212_GAIN_RAMP_RATE_MAX                       4

/* DA7212_MIC_CONFIG = 0x93 */
#define DA7212_DMIC_DATA_SEL_SHIFT                      0
#define DA7212_DMIC_DATA_SEL_MASK                       (0x1 << 0)
#define DA7212_DMIC_SAMPLEPHASE_SHIFT                   1
#define DA7212_DMIC_SAMPLEPHASE_MASK                    (0x1 << 1)
#define DA7212_DMIC_CLK_RATE_SHIFT                      2
#define DA7212_DMIC_CLK_RATE_MASK                       (0x1 << 2)

#define DA7212_CP_TAU_DELAY_SHIFT                       3

/* DA7212_DIG_CTRL = 0x99 */
#define DA7212_DAC_L_INV_SHIFT                          3
#define DA7212_DAC_R_INV_SHIFT                          7
#define DA7212_DAC_INV_MAX                              0x1

/* DA7212_ALC_CTRL2 = 0x9A */
#define DA7212_ALC_ATTACK_SHIFT                         0
#define DA7212_ALC_ATTACK_MAX                           13
#define DA7212_ALC_RELEASE_SHIFT                        4
#define DA7212_ALC_RELEASE_MAX                          11

/* DA7212_ALC_CTRL3 = 0x9B */
#define DA7212_ALC_HOLD_SHIFT                           0
#define DA7212_ALC_HOLD_MAX                             16
#define DA7212_ALC_INTEG_ATTACK_SHIFT                   4
#define DA7212_ALC_INTEG_RELEASE_SHIFT                  6
#define DA7212_ALC_INTEG_MAX                            4

/*
 * DA7212_ALC_NOISE = 0x9C,
 * DA7212_ALC_TARGET_MIN/MAX = 0x9D/0x9E
 */
#define DA7212_ALC_THRESHOLD_SHIFT                      0
#define DA7212_ALC_THRESHOLD_MAX                        0x3F

/* DA7212_ALC_GAIN_LIMITS = 0x9F */
#define DA7212_ALC_ATTEN_MAX_SHIFT                      0
#define DA7212_ALC_GAIN_MAX_SHIFT                       4
#define DA7212_ALC_ATTEN_GAIN_MAX_MAX                   0xF

/* DA7212_ALC_ANA_GAIN_LIMITS = 0xA0 */
#define DA7212_ALC_ANA_GAIN_MIN_SHIFT                   0
#define DA7212_ALC_ANA_GAIN_MAX_SHIFT                   4
#define DA7212_ALC_ANA_GAIN_MAX                         0x7

/* DA7212_ALC_ANTICLIP_CTRL = 0xA1 */
#define DA7212_ALC_ANTICLIP_EN_SHIFT                    7
#define DA7212_ALC_ANTICLIP_EN_MAX                      0x1

/* DA7212_ALC_ANTICLIP_LEVEL = 0xA2 */
#define DA7212_ALC_ANTICLIP_LEVEL_SHIFT                 0
#define DA7212_ALC_ANTICLIP_LEVEL_MAX                   0x7F

/* DA7212_ALC_CIC_OP_LVL_CTRL = 0xAD */
#define DA7212_ALC_DATA_MIDDLE                          (0x2 << 0)
#define DA7212_ALC_DATA_TOP                             (0x3 << 0)
#define DA7212_ALC_CIC_OP_CHANNEL_LEFT                  (0x0 << 7)
#define DA7212_ALC_CIC_OP_CHANNEL_RIGHT                 (0x1 << 7)

/* DA7212_DAC_NG_SETUP_TIME = 0xAF */
#define DA7212_DAC_NG_SETUP_TIME_SHIFT                  0
#define DA7212_DAC_NG_SETUP_TIME_MAX                    4
#define DA7212_DAC_NG_RAMPUP_RATE_SHIFT                 2
#define DA7212_DAC_NG_RAMPDN_RATE_SHIFT                 3
#define DA7212_DAC_NG_RAMP_RATE_MAX                     2

/* DA7212_DAC_NG_OFF/ON_THRESH = 0xB0/0xB1 */
#define DA7212_DAC_NG_THRESHOLD_SHIFT                   0
#define DA7212_DAC_NG_THRESHOLD_MAX                     0x7

/* DA7212_DAC_NG_CTRL = 0xB2 */
#define DA7212_DAC_NG_EN_SHIFT                          7
#define DA7212_DAC_NG_EN_MAX                            0x1

/*
 * General defines
 */

/* Register inversion */
#define DA7212_NO_INVERT                                0
#define DA7212_INVERT                                   1

/* Byte related defines */
#define DA7212_BYTE_SHIFT                               8
#define DA7212_BYTE_MASK                                0xFF

/* ALC related */
#define DA7212_ALC_OFFSET_15_8                          0x00FF00
#define DA7212_ALC_OFFSET_19_16                         0x0F0000
#define DA7212_ALC_AVG_ITERATIONS                       5

/* PLL related */
#define DA7212_SYSCLK_MCLK                              0
#define DA7212_SYSCLK_PLL                               1
#define DA7212_PLL_FREQ_OUT_90316800                    90316800
#define DA7212_PLL_FREQ_OUT_98304000                    98304000
#define DA7212_PLL_FREQ_OUT_94310400                    94310400
#define DA7212_PLL_INDIV_5_10_MHZ_VAL                   2
#define DA7212_PLL_INDIV_10_20_MHZ_VAL                  4
#define DA7212_PLL_INDIV_20_40_MHZ_VAL                  8
#define DA7212_PLL_INDIV_40_54_MHZ_VAL                  16

/* SRM */
#define DA7212_SRM_CHECK_RETRIES                        8

enum clk_src {
        DA7212_CLKSRC_MCLK
};

#endif /* _DA7212_H */
