/**
 ****************************************************************************************
 *
 * @file ls013b7dh03.h
 *
 * @brief LCD configuration for LS013B7DH03 LCD
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef LS013B7DH03_H_
#define LS013B7DH03_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_LS013B7DH03

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR        (HW_LCDC_OCM_L1)
#define GDI_DISP_RESX         (128)
#define GDI_DISP_RESY         (128)
#define GDI_LCDC_CONFIG       (&ls013b7dh03_cfg)

/*
 * static const ad_io_conf_t ls013b7dh03_gpio_cfg[] = {
 *       { LS013B7DH03_SCK_PORT,      LS013B7DH03_SCK_PIN,      { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *       { LS013B7DH03_SDA_PORT,      LS013B7DH03_SDA_PIN,      { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *       { LS013B7DH03_CS_PORT,       LS013B7DH03_CS_PIN,       { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
 *       { LS013B7DH03_EXTCOMIN_PORT, LS013B7DH03_EXTCOMIN_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,         false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
 *       { LS013B7DH03_DISP_PORT,     LS013B7DH03_DISP_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
 * };

 * const ad_lcdc_io_conf_t ls013b7dh03_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(ls013b7dh03_gpio_cfg) / sizeof(ls013b7dh03_gpio_cfg[0]),
 *       .io_list = ls013b7dh03_gpio_cfg,
 * };
 */

static const ad_lcdc_driver_conf_t ls013b7dh03_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_SHARP_SPI,
        .hw_init.format = GDI_DISP_COLOR,            //! Monochrome color mode
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.iface_freq = LCDC_FREQ_1MHz,        //! Typical clock frequency
        .ext_clk = HW_LCDC_EXT_CLK_1HZ,
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

static const ad_lcdc_controller_conf_t ls013b7dh03_cfg = {
        .io = &ls013b7dh03_io,
        .drv = &ls013b7dh03_drv,
};


static const uint8_t screen_init_cmds[] = {
        LCDC_JDI_CLEAR(),
};

static const uint8_t screen_power_on_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(LS013B7DH03_DISP_PORT, LS013B7DH03_DISP_PIN),
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_EXT_CLK_SET(true),
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_EXT_CLK_SET(false),
};

static const uint8_t screen_power_off_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(LS013B7DH03_DISP_PORT, LS013B7DH03_DISP_PIN),
};

static const uint8_t screen_clear_cmds[] = {
        LCDC_JDI_CLEAR(),
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        /* Minimum addressable item is one line */
        frame->startx = 0;
        frame->endx = GDI_DISP_RESX - 1;
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_LS013B7DH03 */

#endif /* LS013B7DH03_H_ */
