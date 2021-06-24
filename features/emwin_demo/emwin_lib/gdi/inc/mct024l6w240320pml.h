/**
 ****************************************************************************************
 *
 * @file mct024l6w240320pml.h
 *
 * @brief LCD configuration for MCT024L6W240320PML with IC ILI9341
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef MCT024L6W240320PML_H_
#define MCT024L6W240320PML_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_MCT024L6W240320PML

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR         (HW_LCDC_OCM_8RGB666)
#define GDI_DISP_RESX          (240)
#define GDI_DISP_RESY          (320)
#define GDI_LCDC_CONFIG        (&mct024l6w240320pml_cfg)

/*
 * static const ad_io_conf_t mct024l6w240320pml_gpio_cfg[] = {
 *       {MCT024L6W240320PML_SDA_PORT, MCT024L6W240320PML_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {MCT024L6W240320PML_SCK_PORT, MCT024L6W240320PML_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {MCT024L6W240320PML_CS_PORT,  MCT024L6W240320PML_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {MCT024L6W240320PML_DC_PORT,  MCT024L6W240320PML_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {MCT024L6W240320PML_RST_PORT, MCT024L6W240320PML_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 * };

 * const ad_lcdc_io_conf_t mct024l6w240320pml_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(mct024l6w240320pml_gpio_cfg) / sizeof(mct024l6w240320pml_gpio_cfg[0]),
 *       .io_list = mct024l6w240320pml_gpio_cfg,
 * };
 */

static const ad_lcdc_driver_conf_t mct024l6w240320pml_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_MIPI_SPI4,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.iface_freq = LCDC_FREQ_48MHz,
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

static const ad_lcdc_controller_conf_t mct024l6w240320pml_cfg = {
        .io = &mct024l6w240320pml_io,
        .drv = &mct024l6w240320pml_drv,
};

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(MCT024L6W240320PML_RST_PORT, MCT024L6W240320PML_RST_PIN),
        LCDC_DELAY_MS(10),
        LCDC_GPIO_SET_ACTIVE(MCT024L6W240320PML_RST_PORT, MCT024L6W240320PML_RST_PIN),
        LCDC_DELAY_MS(120),

        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(100),

        LCDC_MIPI_CMD(0xC0), LCDC_MIPI_DATA(0x23),                              // POWERCONTROL1 - GVDD=3.8V
        LCDC_MIPI_CMD(0xC1), LCDC_MIPI_DATA(0x10),                              // POWERCONTROL2
        LCDC_MIPI_CMD(0xC5), LCDC_MIPI_DATA(0x2B), LCDC_MIPI_DATA(0x2B),        // VCOMCONTROL1 - VMH=3.775V - VML=-1.425V
        LCDC_MIPI_CMD(0xC7), LCDC_MIPI_DATA(0xC0),                              // VCOMCONTROL2 -
        LCDC_MIPI_SET_ADDR_MODE(0x88),
        LCDC_MIPI_SET_MODE(0x66),
        LCDC_MIPI_CMD(0xB1), LCDC_MIPI_DATA(0x00), LCDC_MIPI_DATA(0x1B),        // FRAMECONTROL - 70Hz
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

#endif /* dg_configUSE_MCT024L6W240320PML */

#endif /* MCT024L6W240320PML_H_ */
