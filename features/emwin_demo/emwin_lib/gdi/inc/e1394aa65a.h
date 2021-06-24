/**
 ****************************************************************************************
 *
 * @file e1394aa65a.h
 *
 * @brief LCD configuration for E1394AA65 AMOLED with IC RM69330
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef E1394AA65A_H_
#define E1394AA65A_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_E1394AA65A

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR           (HW_LCDC_OCM_8RGB332)
#define GDI_DISP_RESX            (468)
#define GDI_DISP_RESY            (454)
#define GDI_LCDC_CONFIG          (&e1394aa65a_cfg)

/*
 * static const ad_io_conf_t e1394aa65a_gpio_cfg[] = {
 *       { E1394AA65A_SCK_PORT, E1394AA65A_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_DO_PORT,  E1394AA65A_DO_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_DC_PORT,  E1394AA65A_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_CS_PORT,  E1394AA65A_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_RST_PORT, E1394AA65A_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { E1394AA65A_TE_PORT,  E1394AA65A_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
 * };

 * const ad_lcdc_io_conf_t e1394aa65a_io = {
 *       .voltage_level = HW_GPIO_POWER_VDD1V8P,
 *       .io_cnt = sizeof(e1394aa65a_gpio_cfg) / sizeof(e1394aa65a_gpio_cfg[0]),
 *       .io_list = e1394aa65a_gpio_cfg,
 * };
 */

static const ad_lcdc_driver_conf_t e1394aa65a_drv = {
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

static const ad_lcdc_controller_conf_t e1394aa65a_cfg = {
        .io = &e1394aa65a_io,
        .drv = &e1394aa65a_drv,
};

static const uint8_t screen_init_cmds[] = {
        /* LCD Rest */
        LCDC_GPIO_SET_INACTIVE(E1394AA65A_RST_PORT, E1394AA65A_RST_PIN),
        LCDC_DELAY_MS(50),
        LCDC_GPIO_SET_ACTIVE(E1394AA65A_RST_PORT, E1394AA65A_RST_PIN),
        LCDC_DELAY_MS(50),

        /* CMD & DATA */
        LCDC_MIPI_CMD(0xC4), LCDC_MIPI_DATA(0x80),  //! Enable SPI interface write RAM
        LCDC_MIPI_CMD(0x51), LCDC_MIPI_DATA(0x7F),  //! Write display brightness
        LCDC_MIPI_SET_POSITION(0x0, GDI_DISP_RESX - 1, 0x0, GDI_DISP_RESY - 1),
        LCDC_MIPI_SET_MODE(0x22),  //! RGB332
        LCDC_MIPI_SET_TEAR_ON(0x0),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_EXIT_SLEEP_MODE),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_SET_DISPLAY_ON),
        LCDC_DELAY_MS(50),
};

static const uint8_t screen_disable_cmds[] =
{
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_SET_DISPLAY_OFF),
        LCDC_DELAY_MS(120),
        LCDC_MIPI_CMD(HW_LCDC_MIPI_DCS_ENTER_SLEEP_MODE),
        LCDC_DELAY_MS(50),
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        /* The SC[9:0] and EC[9:0]-SC[9:0]+1 must be divisible by 2 */
        /* The SP[9:0] and EP[9:0]-SP[9:0]+1 must be divisible by 2 */
        if (frame->startx & 0x1) {
                frame->startx--;
        }
        if (frame->starty & 0x1) {
                frame->starty--;
        }
        if ((frame->endx - frame->startx + 1) & 0x1) {
                frame->endx++;
        }
        if ((frame->endy - frame->starty + 1) & 0x1) {
                frame->endy++;
        }
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_E1394AA65A */

#endif /* E1394AA65A_H_ */
