/**
 ****************************************************************************************
 *
 * @file psp27801.h
 *
 * @brief LCD configuration for PSP27801 LCD (c-click) with IC SSD1351
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PSP27801_H_
#define PSP27801_H_

#include <stdint.h>
#include "platform_devices.h"

#if dg_configUSE_PSP27801

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define GDI_DISP_COLOR                  (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_RESX                   (96)
#define GDI_DISP_RESY                   (96)
#define GDI_DISP_OFFSETX                (16)
#define GDI_DISP_OFFSETY                (0)
#define GDI_LCDC_CONFIG                 (&psp27801_cfg)
#define GDI_DISP_PRE_DRAW               (screen_pre_draw)
#define GDI_DISP_SET_PARTIAL_UPDATE     (screen_set_partial_update)

#define PSP27801_ROTATE_180D            (1)

/*********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef enum {
        SSD1351_CMD_SET_COLUMN_ADDRESS    = 0x15,
        SSD1351_CMD_WRITE_MEMORY_START    = 0x5C,
        SSD1351_CMD_SET_PAGE_ADDRESS      = 0x75,
        SSD1351_CMD_SET_REMAP_COLOR       = 0xA0,
        SSD1351_CMD_SET_DISP_START_LINE   = 0xA1,
        SSD1351_CMD_SET_DISP_OFFSET       = 0xA2,
        SSD1351_CMD_SET_NORMAL_DISP       = 0xA6,
        SSD1351_CMD_ENTER_INVERT_MODE     = 0xA7,
        SSD1351_CMD_NOP                   = 0xAD, //Also 0xB0. 0xD1, 0xE3
        SSD1351_CMD_SET_DISPLAY_OFF       = 0xAE,
        SSD1351_CMD_SET_DISPLAY_ON        = 0xAF,
        SSD1351_CMD_SET_PHASE_PERIOD      = 0xB1,
        SSD1351_CMD_SET_FRONT_CLK_DIV     = 0xB3,
        SSD1351_CMD_SET_VSL               = 0xB4,
        SSD1351_CMD_SET_2ND_PRECHARGE     = 0xB6,
        SSD1351_CMD_SET_VCOMH_VOLTAGE     = 0xBE,
        SSD1351_CMD_SET_CONTRAST_CUR      = 0xC1,
        SSD1351_CMD_SET_MASTER_CONTRAST   = 0xC7,
        SSD1351_CMD_SET_MUX_RATIO         = 0xCA,
        SSD1351_CMD_SET_LOCK              = 0xFD,
} SSD1351_CMD;

/*
 * static const ad_io_conf_t psp27801_gpio_cfg[] = {
 *       {PSP27801_SDI_PORT, PSP27801_SDI_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_SCK_PORT, PSP27801_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_CS_PORT,  PSP27801_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_DC_PORT,  PSP27801_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_EN_PORT,  PSP27801_EN_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
 *       {PSP27801_RST_PORT, PSP27801_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       {PSP27801_RW_PORT,  PSP27801_RW_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
 * };

 * const ad_lcdc_io_conf_t psp27801_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(psp27801_gpio_cfg) / sizeof(psp27801_gpio_cfg[0]),
 *       .io_list = psp27801_gpio_cfg,
 * };
 */

static const ad_lcdc_driver_conf_t psp27801_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_CUSTOM,
        .hw_init.format = GDI_DISP_COLOR,
        .hw_init.cfg_extra_flags = HW_LCDC_MIPI_CFG_RESET | HW_LCDC_MIPI_CFG_SPI4
                | HW_LCDC_MIPI_CFG_DMA | HW_LCDC_MIPI_CFG_TE_DIS,
        .hw_init.mode = HW_LCDC_MODE_DISABLE,
        .hw_init.iface_freq = LCDC_FREQ_8MHz,
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

static const ad_lcdc_controller_conf_t psp27801_cfg = {
        .io = &psp27801_io,
        .drv = &psp27801_drv,
};

static const uint8_t screen_init_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(PSP27801_EN_PORT, PSP27801_EN_PIN),
        LCDC_DELAY_MS(1),
        LCDC_GPIO_SET_INACTIVE(PSP27801_RST_PORT, PSP27801_RST_PIN),
        LCDC_DELAY_US(10),
        LCDC_GPIO_SET_ACTIVE(PSP27801_RST_PORT, PSP27801_RST_PIN),
        LCDC_DELAY_US(10),
        LCDC_GPIO_SET_ACTIVE(PSP27801_EN_PORT, PSP27801_EN_PIN),

        LCDC_MIPI_CMD(SSD1351_CMD_SET_LOCK), LCDC_MIPI_DATA(0x12),                                      /* Command Lock */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_LOCK), LCDC_MIPI_DATA(0xB1),                                      /* Command Lock */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_DISPLAY_OFF),                                                     /* Display Off */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_FRONT_CLK_DIV), LCDC_MIPI_DATA(0xF1),                             /* Front Clock Div */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_MUX_RATIO), LCDC_MIPI_DATA(0x5F),                                 /* Set Mux Ratio */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_REMAP_COLOR), LCDC_MIPI_DATA(PSP27801_ROTATE_180D ? 0x72 : 0x60),
        LCDC_MIPI_CMD(SSD1351_CMD_SET_COLUMN_ADDRESS), LCDC_MIPI_DATA(0x10), LCDC_MIPI_DATA(0x6F),      /* Set Column Address */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_PAGE_ADDRESS), LCDC_MIPI_DATA(0x00), LCDC_MIPI_DATA(0x5F),        /* Set Row Address */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_DISP_START_LINE), LCDC_MIPI_DATA(0x80),                           /* Set Display Start Line */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_DISP_OFFSET), LCDC_MIPI_DATA(PSP27801_ROTATE_180D ? 0x20 : 0x60), /* Set Display Offset */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_PHASE_PERIOD), LCDC_MIPI_DATA(0x32),                              /* Set Phase Length */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_VCOMH_VOLTAGE), LCDC_MIPI_DATA(0x05),                             /* Set VComH Voltage */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_NORMAL_DISP),                                                     /* Set Display Mode Reset */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_CONTRAST_CUR), LCDC_MIPI_DATA(0x8A), LCDC_MIPI_DATA(0x51), LCDC_MIPI_DATA(0x8A),  /* Set Contrast */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_MASTER_CONTRAST), LCDC_MIPI_DATA(0xCF),                           /* Set Master Contrast */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_VSL), LCDC_MIPI_DATA(0xA0), LCDC_MIPI_DATA(0xB5), LCDC_MIPI_DATA(0x55),           /* Set Segment Low Voltage */
        LCDC_MIPI_CMD(SSD1351_CMD_SET_2ND_PRECHARGE), LCDC_MIPI_DATA(0x01),                             /* Set Second Precharge Period */

        LCDC_MIPI_CMD(SSD1351_CMD_SET_DISPLAY_ON),                                                      /* Set Sleep Mode Display On */
};

static const uint8_t screen_power_on_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(PSP27801_EN_PORT, PSP27801_EN_PIN)
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_MIPI_CMD(SSD1351_CMD_SET_DISPLAY_ON),                                                      /* Set Sleep Mode Display On */
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_MIPI_CMD(SSD1351_CMD_SET_DISPLAY_OFF),                                                     /* Display off */
};

static const uint8_t screen_power_off_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(PSP27801_EN_PORT, PSP27801_EN_PIN)
};

static const uint8_t screen_clear_cmds[] = {
};

static const uint8_t screen_custom_set_partial_update_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        (void)frame;
}

__STATIC_INLINE void screen_pre_draw(gdi_t *gdi, ad_lcdc_handle_t dev, const hw_lcdc_layer_t *layer)
{
        /* Just to suppress compiler warnings */
        (void)gdi;
        (void)dev;
        (void)layer;

        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_CMD, SSD1351_CMD_WRITE_MEMORY_START);
}

__STATIC_INLINE void screen_set_partial_update(gdi_t *gdi, ad_lcdc_handle_t dev, hw_lcdc_frame_t *frame)
{
        /* Just to suppress compiler warnings */
        (void)gdi;
        (void)dev;

        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_CMD, SSD1351_CMD_SET_COLUMN_ADDRESS);
        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_DATA, frame->startx + GDI_DISP_OFFSETX);
        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_DATA, frame->endx + GDI_DISP_OFFSETX);

        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_CMD, SSD1351_CMD_SET_PAGE_ADDRESS);
        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_DATA, frame->starty + GDI_DISP_OFFSETY);
        hw_lcdc_mipi_cmd(HW_LCDC_MIPI_DATA, frame->endy + GDI_DISP_OFFSETY);
}

#endif /* dg_configLCDC_ADAPTER */

#endif /* dg_configUSE_PSP27801 */

#endif /* PSP27801_H_ */
