/**
 * \addtogroup BSP
 * \{
 * \addtogroup SNC
 * \{
 * \addtogroup SNC_HW_sdadc
 * \{
 */
/**
 ****************************************************************************************
 *
 * @file snc_hw_sdadc.c
 *
 * @brief SNC-Implementation of Sigma/Delta ADC Low Level Driver
 *
 * Copyright (c) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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
#if dg_configUSE_HW_SENSOR_NODE
#if dg_configUSE_SNC_HW_SDADC
#include "sdk_defs.h"
#include "SeNIS.h"
#include "snc_hw_sys.h"
#include "snc_utils.h"
#include "snc_hw_sdadc.h"


static void snc_hw_sdadc_init(b_ctx_t* b_ctx, const sdadc_config* cfg);
/**
 * \brief Function used in SNC context to initialize the sdadc peripheral
 *
 * \param [in] cfg              (sdadc_config*: build-time-only value)
 *                              pointer to the sdadc configuration structure
 */
#define SNC_hw_sdadc_init(cfg)                                             \
        snc_hw_sdadc_init(b_ctx, _SNC_OP_VALUE(const sdadc_config*, cfg))


static void snc_hw_sdadc_enable(b_ctx_t* b_ctx);
/**
 * \brief Function used in SNC context to enable the sdadc peripheral
 *
 */
#define SNC_hw_sdadc_enable()                                                           \
        snc_hw_sdadc_enable(b_ctx)


static void snc_hw_sdadc_disable(b_ctx_t* b_ctx);
/**
 * \brief Function used in SNC context to disable the sdadc peripheral
 *
 */
#define SNC_hw_sdadc_disable()                                                          \
        snc_hw_sdadc_disable(b_ctx)


//==================== Configuration functions =================================
static void snc_hw_sdadc_init(b_ctx_t* b_ctx, const sdadc_config* cfg)
{
        //This is following the hw_sdadc.c:hw_sdadc_configure() sequence

        //Bring in all the setting in sequence
        SENIS_wadva(da(&SDADC->SDADC_CTRL_REG),
                (cfg->input_mode << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_SE)) |
                (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN)) );

        SENIS_wadva(da(&SDADC->SDADC_CTRL_REG),
                (cfg->input_mode << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_SE)) |
                (cfg->inp << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INP_SEL)) |
                (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN)) );

        SENIS_wadva(da(&SDADC->SDADC_CTRL_REG),
                (cfg->input_mode << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_SE)) |
                ((cfg->inn<HW_SDADC_INP_VBAT)?(cfg->inn << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INN_SEL)):0) |
                (cfg->inp << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INP_SEL)) |
                (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN)) );

        SENIS_wadva(da(&SDADC->SDADC_CTRL_REG),
                (cfg->input_mode << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_SE)) |
                ((cfg->inn<HW_SDADC_INP_VBAT)?(cfg->inn << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INN_SEL)):0) |
                (cfg->inp << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INP_SEL)) |
                (cfg->continuous << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_CONT)) |
                (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN)) );

        SENIS_wadva(da(&SDADC->SDADC_CTRL_REG),
                (cfg->input_mode << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_SE)) |
                ((cfg->inn<HW_SDADC_INP_VBAT)?(cfg->inn << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INN_SEL)):0) |
                (cfg->inp << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INP_SEL)) |
                (cfg->continuous << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_CONT)) |
                (cfg->over_sampling << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_OSR)) |
                (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN)) );

        SENIS_wadva(da(&SDADC->SDADC_CTRL_REG),
                (cfg->input_mode << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_SE)) | ///XTRA
                ((cfg->inn<HW_SDADC_INP_VBAT)?(cfg->inn << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INN_SEL)):0) |
                (cfg->inp << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INP_SEL)) |
                (cfg->continuous << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_CONT)) |
                (cfg->over_sampling << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_OSR)) |
                (cfg->vref_selection << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_VREF_SEL)) |
                (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN)) );

        //Last, set MINT for end of read detect (START bit set/clear was not reliable)
        //DMA setting ignored
        SENIS_wadva(da(&SDADC->SDADC_CTRL_REG),
                (cfg->input_mode << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_SE)) |
                ((cfg->inn<HW_SDADC_INP_VBAT)?(cfg->inn << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INN_SEL)):0) |
                (cfg->inp << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INP_SEL)) |
                (cfg->continuous << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_CONT)) |
                (cfg->over_sampling << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_OSR)) |
                (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_MINT)) |
                (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN)) );


        //check SDADC_TEST_REG, if CLK_FREQ bits are set, clear them
        SENIS_if (da(&SDADC->SDADC_TEST_REG), BIT, REG_POS(SDADC, SDADC_TEST_REG, SDADC_CLK_FREQ)) {
                //Do need to clear LSB bit of field - MSB also?
                SENIS_if (da(&SDADC->SDADC_TEST_REG), BIT, REG_POS(SDADC, SDADC_TEST_REG, SDADC_CLK_FREQ)+1) {
                        SENIS_xor(da(&SDADC->SDADC_TEST_REG), 3 << REG_POS(SDADC, SDADC_TEST_REG, SDADC_CLK_FREQ));
                        SENIS_else {
                                SENIS_xor(da(&SDADC->SDADC_TEST_REG), 1 << REG_POS(SDADC, SDADC_TEST_REG, SDADC_CLK_FREQ));
                        }
                }
        SENIS_else {
                //LSB already clear - check MSB
                SENIS_if (da(&SDADC->SDADC_TEST_REG), BIT, REG_POS(SDADC, SDADC_TEST_REG, SDADC_CLK_FREQ)+1) {
                        SENIS_xor(da(&SDADC->SDADC_TEST_REG), (1 << (REG_POS(SDADC, SDADC_TEST_REG, SDADC_CLK_FREQ)+1)) );
                }
        }}

        //Now respective bits are clear, set the requested clock frequency
        SENIS_xor(da(&SDADC->SDADC_TEST_REG), (cfg->freq << (REG_POS(SDADC, SDADC_TEST_REG, SDADC_CLK_FREQ))) );

}

static void snc_hw_sdadc_enable(b_ctx_t* b_ctx)
{
        SENIS_labels(sdadc_en_check_loop, sdadc_en_done);

        //28.3 Programming - 1. Enable the SDADC block by setting the SDADC_CTRL_REG[SDADC_EN] bit.
        SENIS_label(sdadc_en_check_loop);
        SENIS_assign(da(&SDADC->SDADC_CTRL_REG), 1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN));
        SENIS_rdcbi(da(&SDADC->SDADC_CTRL_REG), REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN));
        SENIS_cobr_eq(l(sdadc_en_done));
        SENIS_goto(l(sdadc_en_check_loop));
        SENIS_label(sdadc_en_done);
}


static void snc_hw_sdadc_disable(b_ctx_t* b_ctx)
{
        SENIS_wadva(da(&SDADC->SDADC_CTRL_REG), 0);
}
//==================== Peripheral Acquisition functions ========================
void snc_sdadc_open(b_ctx_t* b_ctx, const snc_sdadc_source_config_t * conf)
{
        ASSERT_WARNING(b_ctx);
        // sdadc peripheral Initialization / Acquisition
        SNC_hw_sys_bsr_acquire(BSR_PERIPH_ID_SDADC);
        SNC_hw_sdadc_enable();
        SNC_hw_sdadc_init(conf->drv);
//        SNC_hw_sdadc_enable();        //Unlike GDADC, need to enable first - for SNC_hw_sdadc_init() settings to take
}
void snc_sdadc_close(b_ctx_t* b_ctx, const snc_sdadc_source_config_t * conf)
{
        ASSERT_WARNING(b_ctx);
        SNC_hw_sdadc_disable();
        SNC_hw_sys_bsr_release(BSR_PERIPH_ID_SDADC);
}
void snc_sdadc_read(b_ctx_t* b_ctx, const snc_sdadc_source_config_t * conf, SENIS_OPER_TYPE value_type, uint32_t* value)
{
        ASSERT_WARNING(b_ctx);

        SENIS_labels(check_rdy, check_rdy2, post_check_ldo_ok, post_check_sdadc_en, post_check_start_set, post_check_start_clr);

#define SCHEME4_TIMEOUT 5333    //Loop of 12? instructions, maybe 24 cycles, *5333 = 127992 want 32M/1000*4=128000=4ms
                                //The pre/post-loop work looks to complete the target

        //scheme 4b - YES :-) prevents lock-up, most readings good
        _SNC_TMP_ADD(uint32_t, int_loop_cnt, sizeof(uint32_t));
        _SNC_TMP_ADD(uint32_t, sdadc_ctrl, sizeof(uint32_t));
        _SNC_TMP_ADD(uint32_t, sdadc_ctrl_tmp, sizeof(uint32_t));

        //28.3 Programming - 8. Start the conversion by setting SDADC_CTRL_REG[SDADC_START] bit.
        SENIS_wadad(da(sdadc_ctrl),da(&SDADC->SDADC_CTRL_REG));
        SENIS_xor(da(sdadc_ctrl), (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_START)) );
        SENIS_wadad(da(&SDADC->SDADC_CTRL_REG),da(sdadc_ctrl));

        //check INT set
#ifdef SCHEME4_TIMEOUT
        SENIS_assign(da(int_loop_cnt), 0);
#endif
        SENIS_label(check_rdy);
        SENIS_wadad(da(sdadc_ctrl),da(&SDADC->SDADC_CTRL_REG));                         //EXTRA step, will extend timeout a bit
        SENIS_rdcbi(da(sdadc_ctrl), REG_POS(SDADC, SDADC_CTRL_REG, SDADC_INT) );                //1 - 1 opcode
        SENIS_cobr_eq(l(check_rdy2));                                                           //2 - 1 opcode
#ifdef SCHEME4_TIMEOUT
        SENIS_inc1(da(int_loop_cnt));                                                           //3 - 1 opcode
        SENIS_if (da(int_loop_cnt), GT, SCHEME4_TIMEOUT) {                                      //4 - 5? opcodes: massively expands
                SENIS_goto(l(check_rdy2));                                                      //5 - 2 opcodes: rdcbi/cobr_eq
        }
#endif
        SENIS_goto(l(check_rdy));                                                               //6 - 2 opcodes: rdcbi/cobr_eq

        SENIS_label(check_rdy2);

        //28.3 Programming - 10. Clear the ADC interrupt by writing any value to SDADC_CLEAR_INT_REG.
        SENIS_wadva(da(&SDADC->SDADC_CLEAR_INT_REG), 1);

        //28.3 Programming - 11. Get the ADC result from the SDADC_RESULT_REG.
        senis_assign(b_ctx, value_type, value, _SNC_OP(da(&SDADC->SDADC_RESULT_REG)));

        //clear SDADC_MINT - i.e. clone hw_sdadc_disable_interrupt()
        //copy to a tmp, so don't spoil post analysis by doing what needs to be done
        SENIS_wadad(da(sdadc_ctrl_tmp),da(sdadc_ctrl));
        //first clear START - this may not be set, it is just to handle timeout error state
        SENIS_if (da(sdadc_ctrl_tmp), BIT, REG_POS(SDADC, SDADC_CTRL_REG, SDADC_START)) {
                SENIS_xor(da(sdadc_ctrl_tmp), (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_START)) );
        }
        //then clear MINT - this should be set
        SENIS_if (da(sdadc_ctrl_tmp), BIT, REG_POS(SDADC, SDADC_CTRL_REG, SDADC_MINT)) {
                SENIS_xor(da(sdadc_ctrl_tmp), (1 << REG_POS(SDADC, SDADC_CTRL_REG, SDADC_MINT)) );
        }
        SENIS_wadad(da(&SDADC->SDADC_CTRL_REG),da(sdadc_ctrl_tmp));


        //
        //Now apply checks for various issues observed, setting top bits for consumer's information
        //

        SENIS_if (da(int_loop_cnt), GT, SCHEME4_TIMEOUT) {
                senis_xor(b_ctx, value_type, value, SENIS_OPER_TYPE_VALUE, (uint32_t*)(SDADC_RET_BITMASK_TIMEOUT) );
        }

        //LDO has been known to be lost during read...
        SENIS_rdcbi(da(sdadc_ctrl), REG_POS(SDADC, SDADC_CTRL_REG, SDADC_LDO_OK) );
        SENIS_cobr_eq(l(post_check_ldo_ok));
        senis_xor(b_ctx, value_type, value, SENIS_OPER_TYPE_VALUE, (uint32_t*)(SDADC_RET_BITMASK_LDO_NOK) );
        SENIS_label(post_check_ldo_ok);

        //...as has the enable bit (but other bits read as set - one has to wonder if read was somehow faulty)
        SENIS_rdcbi(da(sdadc_ctrl), REG_POS(SDADC, SDADC_CTRL_REG, SDADC_EN) );
        SENIS_cobr_eq(l(post_check_sdadc_en));
        senis_xor(b_ctx, value_type, value, SENIS_OPER_TYPE_VALUE, (uint32_t*)(SDADC_RET_BITMASK_SDADC_DIS) );
        SENIS_label(post_check_sdadc_en);

        //And start bit has been left on too, but that may just be early poll on it left to exit before write took effect
        SENIS_rdcbi(da(sdadc_ctrl), REG_POS(SDADC, SDADC_CTRL_REG, SDADC_START) );
        SENIS_cobr_eq(l(post_check_start_set));
        SENIS_goto(l(post_check_start_clr));
        SENIS_label(post_check_start_set);
        senis_xor(b_ctx, value_type, value, SENIS_OPER_TYPE_VALUE, (uint32_t*)(SDADC_RET_BITMASK_START_SET) );
        SENIS_label(post_check_start_clr);

        _SNC_TMP_RMV(sdadc_ctrl_tmp);
        _SNC_TMP_RMV(sdadc_ctrl);
        _SNC_TMP_RMV(int_loop_cnt);

}
#endif /* dg_configUSE_SNC_HW_SDADC */
#endif /* dg_configUSE_HW_SENSOR_NODE */
/**
 \}
 \}
 \}
 */
