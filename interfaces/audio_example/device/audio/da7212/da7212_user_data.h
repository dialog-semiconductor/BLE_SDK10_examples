/*
 * da7212_user_data.h - DA7212 audio CODEC Hardware Abstraction Layer functions
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

#ifndef _DA7212_USER_DATA_H
#define _DA7212_USER_DATA_H

#include "da7212.h"
#include "hal-da7212.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*
 * Customer Platform Definitions start :
 * These definitons should be represented for customer's system.
 */

/* Master/Slave setting, can be set to either
  'DA7212_CODEC_MASTER' or 'DA7212_CODEC_SLAVE' */
#define AP_I2S_MASTER DA7212_CODEC_SLAVE

/* CLK, PLL, DAI */
#define DA7212_MCLK MCLK32M
#define DA7212_MCLK_SQR_EN	0
#define DA7212_DAI_FMT	DA7212_DAI_FORMAT_I2S_MODE

#if (AP_I2S_MASTER == DA7212_CODEC_MASTER)
#define DA7212_CONFIG_PLL_MODE	DA7212_SYSCLK_PLL
#else
#define DA7212_CONFIG_PLL_MODE	DA7212_SYSCLK_PLL_SRM
#endif

/* I/O Level */
#define HAL_IO_LVL DA7212_IO_VOLTAGE_LVL_1_2V_2_5V

/* LDO Level */
#define HAL_LDO_LVL DA7212_LDO_LVL_SEL_1_05V

/* CP */
#define HAL_CPDELAY\
	(DA7212_CP_TAU_DELAY_256MS << DA7212_CP_TAU_DELAY_SHIFT\
	| DA7212_CP_FCONTROL_0KHZ)

#define HAL_CP_SMALL_SWI_FREQ_EN 1
#define HAL_CP_MCHANGE_SIG	DA7212_CP_MCHANGE_SIGNAL_MAG
#define HAL_CPVDD_SET	CPVDD_STANDBY
#define HAL_CP_VOL_THRESH 0x34

#define DA7212_MIC_AMP_SEL MIC_P_SEL

/*
 * End of Customer Platform Definitions.
 */

/*
 * Macros
 */

//Set PC sync to resync
#define PC_SYNC_RESYNC(x)\
da721x_i2c_reg_write(DA7212_PC_COUNT, x)

#define SET_DAI_16BIT_I2S_MODE()\
da721x_i2c_reg_write(DA7212_DAI_CTRL, 0xC0)

#define SET_MIXIN_L_SEL(x)\
da721x_i2c_update_bits(DA7212_MIXIN_L_SELECT, x, x)
#define SET_MIXIN_L_UnSEL(x)\
da721x_i2c_update_bits(DA7212_MIXIN_L_SELECT, x, 0)

#define SET_MIXIN_R_SEL(x)\
da721x_i2c_update_bits(DA7212_MIXIN_R_SELECT, x, x)
#define SET_MIXIN_R_UnSEL(x)\
da721x_i2c_update_bits(DA7212_MIXIN_R_SELECT, x, 0)

#define SET_MIXOUT_L_SEL(x)\
da721x_i2c_update_bits(DA7212_MIXOUT_L_SELECT, x, x)
#define SET_MIXOUT_L_UnSEL(x)\
da721x_i2c_update_bits(DA7212_MIXOUT_L_SELECT, x, 0)

#define SET_MIXOUT_R_SEL(x)\
da721x_i2c_update_bits(DA7212_MIXOUT_R_SELECT, x, x)
#define SET_MIXOUT_R_UnSEL(x)\
da721x_i2c_update_bits(DA7212_MIXOUT_R_SELECT, x, 0)

#define SET_DAC_L_SRC(x)\
da721x_i2c_update_bits(DA7212_DIG_ROUTING_DAC,\
DA7212_DAC_L_SRC_MASK, x)
#define SET_DAC_R_SRC(x)\
da721x_i2c_update_bits(DA7212_DIG_ROUTING_DAC,\
DA7212_DAC_R_SRC_MASK, x << DA7212_DAC_R_SRC_SHIFT)

#define SET_DAI_L_SRC(x)\
da721x_i2c_update_bits(DA7212_DIG_ROUTING_DAI,\
DA7212_DAI_L_SRC_MASK, x)

#define SET_DAI_R_SRC(x)\
da721x_i2c_update_bits(DA7212_DIG_ROUTING_DAI,\
DA7212_DAI_R_SRC_MASK, x << DA7212_DAI_R_SRC_SHIFT)

#endif /* _DA7212_USER_DATA_H */
