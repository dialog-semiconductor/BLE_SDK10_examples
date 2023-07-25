/**
 ****************************************************************************************
 *
 * @file hm80160a090.h
 *
 * @brief LCD configuration for HM80160A090 LCD with IC ST7735S
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

#ifndef HM80160A090_H_
#define HM80160A090_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_HM80160A090

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR          (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_RESX           (80)
#define GDI_DISP_RESY           (160)
#define GDI_DISP_OFFSETX        (24)
#define GDI_LCDC_CONFIG         (&hm80160a090_cfg)

/*
 * static const ad_io_conf_t hm80160a090_gpio_cfg[] = {
 *       { HM80160A090_SCK_PORT, HM80160A090_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { HM80160A090_SDA_PORT, HM80160A090_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { HM80160A090_RST_PORT, HM80160A090_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { HM80160A090_DC_PORT,  HM80160A090_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { HM80160A090_CS_PORT,  HM80160A090_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 * };

 * const ad_lcdc_io_conf_t hm80160a090_io = {
 *       .voltage_level = HW_GPIO_POWER_VDD1V8P,
 *       .io_cnt = sizeof(hm80160a090_gpio_cfg) / sizeof(hm80160a090_gpio_cfg[0]),
 *       .io_list = hm80160a090_gpio_cfg,
 * };
 */

static const ad_lcdc_driver_conf_t hm80160a090_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.iface_freq = LCDC_FREQ_16MHz,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
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

static const ad_lcdc_controller_conf_t hm80160a090_cfg = {
        .io = &hm80160a090_io,
        .drv = &hm80160a090_drv,
};

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(HM80160A090_RST_PORT, HM80160A090_RST_PIN),
        LCDC_DELAY_MS(100),
        LCDC_GPIO_SET_ACTIVE(HM80160A090_RST_PORT, HM80160A090_RST_PIN),
        LCDC_DELAY_MS(100),

        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(100),

        //ST7735R Frame Rate
        LCDC_MIPI_CMD(0xB1), LCDC_MIPI_DATA(0x01), LCDC_MIPI_DATA(0x2C), LCDC_MIPI_DATA(0x2D),
        LCDC_MIPI_CMD(0xB2), LCDC_MIPI_DATA(0x01), LCDC_MIPI_DATA(0x2C), LCDC_MIPI_DATA(0x2D),
        LCDC_MIPI_CMD(0xB3), LCDC_MIPI_DATA(0x01), LCDC_MIPI_DATA(0x2C), LCDC_MIPI_DATA(0x2D),
                           LCDC_MIPI_DATA(0x01), LCDC_MIPI_DATA(0x2C), LCDC_MIPI_DATA(0x2D),
        //Column inversion
        LCDC_MIPI_CMD(0xB4),LCDC_MIPI_DATA(0x07),

        //ST7735R Power Sequence
        LCDC_MIPI_CMD(0xC0), LCDC_MIPI_DATA(0xA2), LCDC_MIPI_DATA(0x02), LCDC_MIPI_DATA(0x84),
        LCDC_MIPI_CMD(0xC1), LCDC_MIPI_DATA(0xC5),
        LCDC_MIPI_CMD(0xC2), LCDC_MIPI_DATA(0x0A), LCDC_MIPI_DATA(0x00),
        LCDC_MIPI_CMD(0xC3), LCDC_MIPI_DATA(0x8A), LCDC_MIPI_DATA(0x2A),
        LCDC_MIPI_CMD(0xC4), LCDC_MIPI_DATA(0x8A), LCDC_MIPI_DATA(0xEE),
        LCDC_MIPI_CMD(0xC5), LCDC_MIPI_DATA(0x0E),

        LCDC_MIPI_SET_ADDR_MODE(0x04),

        //ST7735R Gamma Sequence
        LCDC_MIPI_CMD(0xe0), LCDC_MIPI_DATA(0x0f), LCDC_MIPI_DATA(0x1A), LCDC_MIPI_DATA(0x0f),
                           LCDC_MIPI_DATA(0x18), LCDC_MIPI_DATA(0x2f), LCDC_MIPI_DATA(0x28),
                           LCDC_MIPI_DATA(0x20), LCDC_MIPI_DATA(0x22), LCDC_MIPI_DATA(0x1f),
                           LCDC_MIPI_DATA(0x1b), LCDC_MIPI_DATA(0x23), LCDC_MIPI_DATA(0x37),
                           LCDC_MIPI_DATA(0x00), LCDC_MIPI_DATA(0x07), LCDC_MIPI_DATA(0x02),
                           LCDC_MIPI_DATA(0x10),


        LCDC_MIPI_CMD(0xe1), LCDC_MIPI_DATA(0x0f), LCDC_MIPI_DATA(0x1b), LCDC_MIPI_DATA(0x0f),
                           LCDC_MIPI_DATA(0x17), LCDC_MIPI_DATA(0x33), LCDC_MIPI_DATA(0x2C),
                           LCDC_MIPI_DATA(0x29), LCDC_MIPI_DATA(0x2e), LCDC_MIPI_DATA(0x30),
                           LCDC_MIPI_DATA(0x30), LCDC_MIPI_DATA(0x39), LCDC_MIPI_DATA(0x3f),
                           LCDC_MIPI_DATA(0x00), LCDC_MIPI_DATA(0x07), LCDC_MIPI_DATA(0x03),
                           LCDC_MIPI_DATA(0x10),

        LCDC_MIPI_SET_POSITION(0x18, 0, GDI_DISP_RESX - 1 + 0x18, GDI_DISP_RESY - 1),

        //Enable test command
        LCDC_MIPI_CMD(0xf0), LCDC_MIPI_DATA(0x01),

        //Disable ram power save mode
        LCDC_MIPI_CMD(0xf6), LCDC_MIPI_DATA(0x00),

        LCDC_MIPI_SET_MODE(0x05),//0x03 - RGB444 / 0x05 - RGB565 / 0x06 - RGB666
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON),
        LCDC_DELAY_MS(120),
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_SET_DISPLAY_OFF),
        LCDC_DELAY_MS(120),
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

#endif /* dg_configUSE_HM80160A090 */

#endif /* HM80160A090_H_ */
