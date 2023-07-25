/*
 * da7218_user_data.h - DA7218 audio CODEC Hardware Abstraction Layer functions
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

#ifndef _DA7218_USER_DATA_H
#define _DA7218_USER_DATA_H

#include "da7218.h"
#include "hal-da7218.h"

/*
 * Customer Platform Definitions start :
 * These definitons should be represented for customer's system.
 */

/* Master/Slave setting, can be set to either
  'DA7218_CODEC_MASTER' or 'DA7218_CODEC_SLAVE' */
#define AP_I2S_MASTER DA7218_CODEC_SLAVE

/* CLK, PLL, DAI */
#define DA7218_MCLK MCLK32M
#define DA7218_MCLK_SQR_EN 0
#define DA7218_DAI_FMT DA7218_DAI_FORMAT_I2S

#if (AP_I2S_MASTER == DA7218_CODEC_MASTER)
#define DA7218_CONFIG_PLL_MODE DA7218_SYSCLK_PLL
#else
#define DA7218_CONFIG_PLL_MODE DA7218_SYSCLK_PLL_SRM
#endif

/* Set MIC(input) components. i.e, if use -
 * only Analogue MIC 1: set to DA7218_AMIC1
 * both Analogue MIC 1,2: set to DA7218_AMIC12
 * only Digital MIC 1: set to DA7218_DMIC1
 * both Digital MIC 1,2: set to DA7218_DMIC12
 */
#define DA7218_MIC_INPUT DA7218_AMIC12

/* Set Input Differential/Single ended */
#define DA7218_HAL_MIC_SINGLE_E

/* Set input Differential/Single ended -
    can be set to one of mic_insel_t values */
#ifdef DA7218_HAL_MIC_SINGLE_E
#define HAL_MICAMP_SEL MIC_P_SEL
#else
#define HAL_MICAMP_SEL MIC_IN_DIFF
#endif

/* Set input Differential/Single ended */
#define DA7218_MIC_BIAS1_V DA7218_MICBIAS_2_6V
#define DA7218_MIC_BIAS2_V DA7218_MICBIAS_2_6V

/* DMIC */
#define HAL_D_MIC1_DAT_SEL DA7218_DMIC_DATA_LRISE_RFALL
#define HAL_D_MIC2_DAT_SEL DA7218_DMIC_DATA_LRISE_RFALL

#define HAL_D_MIC1_SAMPL_PH DA7218_DMIC_SAMPLE_ON_CLKEDGE
#define HAL_D_MIC2_SAMPL_PH DA7218_DMIC_SAMPLE_ON_CLKEDGE

#define HAL_D_MIC1_CLK_RATE DA7218_DMIC_CLK_3_0MHZ
#define HAL_D_MIC2_CLK_RATE DA7218_DMIC_CLK_3_0MHZ

/* Set output to Differential or Single ended -
    Default is Differential */
#ifdef DA7217_CODEC_HAL
#define HAL_HP_DIFF_CTRL DA7218_HP_DIFFERENTIAL
#else
#define HAL_HP_DIFF_CTRL DA7218_HP_SINGLE_ENDED
#endif

/* if CP is used, Single supply mode must be disalbed */
#undef DA7218_SINGLE_SUPPLY_MODE

/* I/O Level */
#define HAL_IO_LVL DA7218_IO_VOLTAGE_LVL_1_5V_2_5V

/* LDO Level */
#define HAL_LDO_LVL DA7218_LDO_LVL_SEL_1_05V

/* CP */
#define HAL_CPDELAY\
        (DA7218_CP_TAU_DELAY_256MS << DA7218_CP_TAU_DELAY_SHIFT\
                | DA7218_CP_FCONTROL_0KHZ)

#define HAL_CP_SMALL_SWI_FREQ_EN 1
#define HAL_CP_MCHANGE_SIG DA7218_CP_MCHANGE_SIGNAL_MAG
#define HAL_CPVDD_SET CPVDD_STANDBY
#define HAL_CP_VOL_THRESH 0x34

#ifdef DA7218_JACK_DETECTION
#define DA7218_OF_JACK_RATE DA7218_HPLDET_JACK_RATE_40US
#define DA7218_OF_JACK_DEBOUNC DA7218_HPLDET_JACK_DEBOUNCE_2
#define DA7218_OF_JACK_THR DA7218_HPLDET_JACK_THR_84PCT

#define DA7218_OF_COMP_INV 0
#define DA7218_OF_HYST 1
#define DA7218_OF_DISCHARGE 1
#endif

/*
 * End of Customer Platform Definitions.
 */


/*
 * Set Definitions as Customer definitions above
 */

/* set PLL Mode Value */
#if DA7218_CONFIG_PLL_MODE == DA7218_SYSCLK_MCLK
#define DA7218_PLL_MOD_VAL DA7218_PLL_MODE_BYPASS
#elif DA7218_CONFIG_PLL_MODE == DA7218_SYSCLK_PLL
#define DA7218_PLL_MOD_VAL DA7218_PLL_MODE_NORMAL
#elif (DA7218_CONFIG_PLL_MODE == DA7218_SYSCLK_PLL_SRM)
#define DA7218_PLL_MOD_VAL DA7218_PLL_MODE_SRM
#elif (DA7218_CONFIG_PLL_MODE == DA7218_SYSCLK_PLL_32KHZ)
#define DA7218_PLL_MOD_VAL DA7218_PLL_MODE_32KHZ
#endif

/* Define MIC enable & Bias level */
#if (DA7218_MIC_INPUT == DA7218_AMIC12)
#define DA7218_HAL_MIC_BIAS \
        ((DA7218_MIC_BIAS2_V<<DA7218_MICBIAS_2_LEVEL_SHIFT)\
                | DA7218_MIC_BIAS1_V)
#define DA7218_HAL_MIC_EN 0x11
#define DA7218_USE_AMIC1
#define DA7218_USE_AMIC2

#elif (DA7218_MIC_INPUT == DA7218_AMIC1)
#define DA7218_HAL_MIC_BIAS\
        (DA7218_MIC_BIAS1_V)
#define DA7218_HAL_MIC_EN 0x01
#define DA7218_USE_AMIC1

#elif (DA7218_MIC_INPUT == DA7218_AMIC2)
#define DA7218_HAL_MIC_BIAS\
        (DA7218_MIC_BIAS2_V<<DA7218_MICBIAS_2_LEVEL_SHIFT)
#define DA7218_HAL_MIC_EN 0x10
#define DA7218_USE_AMIC2

#elif (DA7218_MIC_INPUT == DA7218_DMIC12)
#define DA7218_HAL_MIC_BIAS\
        ((DA7218_MIC_BIAS2_V<<DA7218_MICBIAS_2_LEVEL_SHIFT)\
                | DA7218_MIC_BIAS1_V)
#define DA7218_HAL_MIC_EN 0x11
#define DA7218_USE_DMIC1
#define DA7218_USE_DMIC2

#elif (DA7218_MIC_INPUT == DA7218_DMIC1)
#define DA7218_HAL_MIC_BIAS\
        (DA7218_MIC_BIAS1_V)
#define DA7218_HAL_MIC_EN 0x01
#define DA7218_USE_DMIC1

#elif (DA7218_MIC_INPUT == DA7218_DMIC2)
#define DA7218_HAL_MIC_BIAS\
        (DA7218_MIC_BIAS2_V<<DA7218_MICBIAS_2_LEVEL_SHIFT)
#define DA7218_HAL_MIC_EN 0x10
#define DA7218_USE_DMIC2
#endif

#ifdef DA7218_USE_DMIC2
#define DA7218_DMIC2_ROUTINGL 0x04
#define DA7218_DMIC2_ROUTINGR 0x08
#else
#define DA7218_DMIC2_ROUTINGL 0x00
#define DA7218_DMIC2_ROUTINGR 0x00
#endif

#define HAL_D_MIC1_CTRL_VAL\
        (HAL_D_MIC1_DAT_SEL << DA7218_DMIC_1_DATA_SEL_SHIFT\
                |HAL_D_MIC1_SAMPL_PH << DA7218_DMIC_1_SAMPLEPHASE_SHIFT\
                |HAL_D_MIC1_CLK_RATE << DA7218_DMIC_1_CLK_RATE_SHIFT)
#define HAL_D_MIC2_CTRL_VAL\
        (HAL_D_MIC1_DAT_SEL << DA7218_DMIC_1_DATA_SEL_SHIFT\
                |HAL_D_MIC1_SAMPL_PH << DA7218_DMIC_1_SAMPLEPHASE_SHIFT\
                |HAL_D_MIC1_CLK_RATE << DA7218_DMIC_1_CLK_RATE_SHIFT)

#define HAL_CP_CTRL_VAL\
        (HAL_CP_SMALL_SWI_FREQ_EN\
                <<DA7218_CP_SMALL_SWITCH_FREQ_EN_SHIFT\
                | HAL_CP_MCHANGE_SIG<<DA7218_CP_MCHANGE_SHIFT\
                | HAL_CPVDD_SET<<DA7218_CP_MOD_SHIFT)

#ifdef DA7218_JACK_DETECTION
/* JACK detection user data range check */
#if DA7218_OF_JACK_RATE < DA7218_HPLDET_JACK_RATE_5US
#undef DA7218_OF_JACK_RATE
#define DA7218_OF_JACK_RATE DA7218_HPLDET_JACK_RATE_40US
#endif
#if DA7218_OF_JACK_RATE > DA7218_HPLDET_JACK_RATE_640US
#undef DA7218_OF_JACK_RATE
#define DA7218_OF_JACK_RATE DA7218_HPLDET_JACK_RATE_40US
#endif

#if DA7218_OF_JACK_DEBOUNC < DA7218_HPLDET_JACK_DEBOUNCE_OFF
#undef DA7218_OF_JACK_DEBOUNC
#define DA7218_OF_JACK_DEBOUNC DA7218_HPLDET_JACK_DEBOUNCE_2
#endif
#if DA7218_OF_JACK_DEBOUNC > DA7218_HPLDET_JACK_DEBOUNCE_4
#undef DA7218_OF_JACK_DEBOUNC
#define DA7218_OF_JACK_DEBOUNC DA7218_HPLDET_JACK_DEBOUNCE_2
#endif

#if DA7218_OF_JACK_THR < DA7218_HPLDET_JACK_THR_84PCT
#undef DA7218_OF_JACK_THR
#define DA7218_OF_JACK_THR DA7218_HPLDET_JACK_THR_84PCT
#endif
#if DA7218_OF_JACK_THR > DA7218_HPLDET_JACK_THR_96PCT
#undef DA7218_OF_JACK_THR
#define DA7218_OF_JACK_THR DA7218_HPLDET_JACK_THR_84PCT
#endif

#if DA7218_OF_COMP_INV < 0
#undef DA7218_OF_COMP_INV
#define DA7218_OF_COMP_INV 1
#endif
#if DA7218_OF_COMP_INV > 1
#undef DA7218_OF_COMP_INV
#define DA7218_OF_COMP_INV 1
#endif

#if DA7218_OF_HYST < 0
#undef DA7218_OF_HYST
#define DA7218_OF_HYST 1
#endif
#if DA7218_OF_HYST > 1
#undef DA7218_OF_HYST
#define DA7218_OF_HYST 1
#endif

#if DA7218_OF_DISCHARGE < 0
#undef DA7218_OF_DISCHARGE
#define DA7218_OF_DISCHARGE 1
#endif
#if DA7218_OF_DISCHARGE > 1
#undef DA7218_OF_DISCHARGE
#define DA7218_OF_DISCHARGE 1
#endif

#endif

/* BiQuad filter configured to pass
 * through audio un-modified */
const u8 da7218_5biquad_cfg[] = {
        0x00, 0x00, 0x40, 0x01, 0x00, 0x02, 0x00, 0x03,
        0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07,
        0x00, 0x08, 0x00, 0x09, 0x00, 0x0a, 0x40, 0x0b,
        0x00, 0x0c, 0x00, 0x0d, 0x00, 0x0e, 0x00, 0x0f,
        0x00, 0x10, 0x00, 0x11, 0x00, 0x12, 0x00, 0x13,
        0x00, 0x14, 0x40, 0x15, 0x00, 0x16, 0x00, 0x17,
        0x00, 0x18, 0x00, 0x19, 0x00, 0x1a, 0x00, 0x1b,
        0x00, 0x1c, 0x00, 0x1d, 0x00, 0x1e, 0x40, 0x1f,
        0x00, 0x20, 0x00, 0x21, 0x00, 0x22, 0x00, 0x23,
        0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27,
        0x00, 0x28, 0x40, 0x29, 0x00, 0x2a, 0x00, 0x2b,
        0x00, 0x2c, 0x00, 0x2d, 0x00, 0x2e, 0x00, 0x2f,
        0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x40, 0x33,
        0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37,
        0x00, 0x38, 0x00, 0x39, 0x00, 0x3a, 0x00, 0x3b
};

const u8 da7218_st_3biquad_cfg[] = {
        0x00, 0x00, 0x40, 0x01, 0x00, 0x02, 0x00, 0x03,
        0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07,
        0x00, 0x08, 0x00, 0x09, 0x00, 0x0a, 0x40, 0x0b,
        0x00, 0x0c, 0x00, 0x0d, 0x00, 0x0e, 0x00, 0x0f,
        0x00, 0x10, 0x00, 0x11, 0x00, 0x12, 0x00, 0x13,
        0x00, 0x14, 0x40, 0x15, 0x00, 0x16, 0x00, 0x17,
        0x00, 0x18, 0x00, 0x19, 0x00, 0x1a, 0x00, 0x1b,
        0x00, 0x1c, 0x00, 0x1d
};

#endif /* _DA7218_USER_DATA_H */
