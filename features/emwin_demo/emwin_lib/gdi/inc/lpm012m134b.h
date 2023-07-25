/**
 ****************************************************************************************
 *
 * @file lpm012m134b.h
 *
 * @brief LCD configuration for LPM012M134B LCD
 *
 * Copyright (C) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef LPM012M134B_H_
#define LPM012M134B_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_LPM012M134B

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR              (HW_LCDC_OCM_RGB222)
#define GDI_DISP_RESX               (240)
#define GDI_DISP_RESY               (240)
#define GDI_LCDC_CONFIG             (&lpm012m134b_cfg)
#define GDI_USE_CONTINUOUS_MODE     (0) //! It can also operate in continuous mode

/*
 * static const ad_io_conf_t lpm012m134b_gpio_cfg[] = {
 *       { LPM012M134B_GREEN_0_PORT, LPM012M134B_GREEN_0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_GREEN_1_PORT, LPM012M134B_GREEN_1_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_RED_0_PORT,   LPM012M134B_RED_0_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_RED_1_PORT,   LPM012M134B_RED_1_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_BLUE_0_PORT,  LPM012M134B_BLUE_0_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_BLUE_1_PORT,  LPM012M134B_BLUE_1_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_XRST_PORT,    LPM012M134B_XRST_PIN,    { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_HST_PORT,     LPM012M134B_HST_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_HCK_PORT,     LPM012M134B_HCK_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_VST_PORT,     LPM012M134B_VST_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_VCK_PORT,     LPM012M134B_VCK_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_ENB_PORT,     LPM012M134B_ENB_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_FRP_PORT,     LPM012M134B_FRP_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_XFRP_PORT,    LPM012M134B_XFRP_PIN,    { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { LPM012M134B_PEN_PORT,     LPM012M134B_PEN_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 * };

 * const ad_lcdc_io_conf_t lpm012m134b_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(lpm012m134b_gpio_cfg) / sizeof(lpm012m134b_gpio_cfg[0]),
 *       .io_list = lpm012m134b_gpio_cfg,
 * };
 */

static const ad_lcdc_driver_conf_t lpm012m134b_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_JDI_PARALLEL,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.iface_freq = LCDC_FREQ_1_6MHz,
        .ext_clk = HW_LCDC_EXT_CLK_62_5HZ,
        .te_enable = false,
        .te_polarity = HW_LCDC_TE_LOW,
        .display.resx = GDI_DISP_RESX,
        .display.resy = GDI_DISP_RESY,
        .display.fpx = 4,
        .display.fpy = 4,
        .display.bpx = 0,
        .display.bpy = 0,
        .display.blx = 4,
        .display.bly = 9,
        .par_timings.starty = 0,
        .par_timings.endy = GDI_DISP_RESY - 1,
        .par_timings.resx = GDI_DISP_RESX,
        .par_timings.resy = GDI_DISP_RESY,
        .par_timings.fxb = 2,
        .par_timings.fyb = 1,
        .par_timings.bxb = 2,
        .par_timings.byb = 8,
        .par_timings.hck_width = 2,
        .par_timings.xrst_width = 2,
};

static const ad_lcdc_controller_conf_t lpm012m134b_cfg = {
        .io = &lpm012m134b_io,
        .drv = &lpm012m134b_drv,
};

static const uint8_t screen_init_cmds[] = {
};

static const uint8_t screen_power_on_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(LPM012M134B_PEN_PORT, LPM012M134B_PEN_PIN),
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_EXT_CLK_SET(true),
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_EXT_CLK_SET(false),
};

static const uint8_t screen_power_off_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(LPM012M134B_PEN_PORT, LPM012M134B_PEN_PIN),
};

static const uint8_t screen_clear_cmds[] = {
        /* Screen does not have a special command for clearing */
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        frame->startx = 0;
        frame->endx = GDI_DISP_RESX - 1;
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_LPM012M134B */

#endif /* LPM012M134B_H_ */
