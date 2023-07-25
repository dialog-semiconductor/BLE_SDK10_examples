/**
 ****************************************************************************************
 *
 * @file t1d3bp006.h
 *
 * @brief LCD configuration for T1D3BP006 with IC ST7789V2
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

#ifndef T1D3BP006_H_
#define T1D3BP006_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_T1D3BP006

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR                  (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_RESX                   (240)
#define GDI_DISP_RESY                   (240)
#define GDI_LCDC_CONFIG                 (&t1d3bp006_cfg)

/*
 * static const ad_io_conf_t t1d3bp006_gpio_cfg[] = {
 *       { T1D3BP006_SCK_PORT, T1D3BP006_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { T1D3BP006_SDA_PORT, T1D3BP006_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { T1D3BP006_RST_PORT, T1D3BP006_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { T1D3BP006_DC_PORT,  T1D3BP006_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { T1D3BP006_CS_PORT,  T1D3BP006_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { T1D3BP006_TE_PORT,  T1D3BP006_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
 * };

 * const ad_lcdc_io_conf_t t1d3bp006_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(t1d3bp006_gpio_cfg) / sizeof(t1d3bp006_gpio_cfg[0]),
 *       .io_list = t1d3bp006_gpio_cfg,
 * };
 */

static const ad_lcdc_driver_conf_t t1d3bp006_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.iface_freq = LCDC_FREQ_48MHz,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = true,
        .te_polarity = HW_LCDC_TE_LOW,
        .display.resx = GDI_DISP_RESX,
        .display.resy = GDI_DISP_RESY,
        .display.fpx = 0,
        .display.fpy = 0,
        .display.bpx = 0,
        .display.bpy = 0,
        .display.blx = 2, //! Min. timing parameters required by LCDC */
        .display.bly = 1, //! Min. timing parameters required by LCDC */
};

static const ad_lcdc_controller_conf_t t1d3bp006_cfg = {
        .io = &t1d3bp006_io,
        .drv = &t1d3bp006_drv,
};

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(T1D3BP006_RST_PORT, T1D3BP006_RST_PIN),
        LCDC_DELAY_MS(10),
        LCDC_GPIO_SET_ACTIVE(T1D3BP006_RST_PORT, T1D3BP006_RST_PIN),
        LCDC_DELAY_MS(120),

        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(100),

        LCDC_MIPI_CMD(0xB2), LCDC_MIPI_DATA(0x3F), LCDC_MIPI_DATA(0x3F), LCDC_MIPI_DATA(0x00), LCDC_MIPI_DATA(0x00),

        LCDC_MIPI_EXIT_INVERT(),
        LCDC_MIPI_CMD(0xC0), LCDC_MIPI_DATA(0x3C), //inverse inversion command

        LCDC_MIPI_SET_POSITION(0, 0, GDI_DISP_RESX - 1, GDI_DISP_RESY - 1),

        LCDC_MIPI_SET_MODE(0x05),
        LCDC_MIPI_SET_TEAR_ON(0x00),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON),
        LCDC_DELAY_MS(80),
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_SET_DISPLAY_OFF),
        LCDC_DELAY_MS(50),
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_ENTER_SLEEP_MODE),
        LCDC_DELAY_MS(120),
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        (void)frame; //! Just to suppress compiler warnings
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_T1D3BP006 */

#endif /* T1D3BP006_H_ */
