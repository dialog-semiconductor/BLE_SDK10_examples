/**
 ****************************************************************************************
 *
 * @file dt280qv10ct.h
 *
 * @brief LCD configuration for DT280QV10CT LCD with IC ILI9341
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef DT280QV10CT_H_
#define DT280QV10CT_H_

#include <stdio.h>
#include <stdint.h>
#include "platform_devices.h"
#include "ad_i2c.h"

#if dg_configUSE_DT280QV10CT

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 *********************************************************************
 */
#define GDI_DISP_COLOR                  (HW_LCDC_OCM_8RGB565)
#define GDI_DISP_RESX                   (240)
#define GDI_DISP_RESY                   (320)
#define GDI_LCDC_CONFIG                 (&dt280qv10ct_cfg)
#define GDI_TOUCH_ENABLE                (1)
#define GDI_TOUCH_INTERFACE             (GDI_TOUCH_INTERFACE_I2C)
#if dg_configUSE_FT6206
#define GDI_TOUCH_CONFIG                (&ft6206_cfg)
#define GDI_TOUCH_INIT                  (ft6206_init)
#define GDI_TOUCH_READ_EVENT            (ft6206_read_event)
#endif

/*
 * static const ad_io_conf_t dt280qv10ct_gpio_cfg[] = {
 *       { DT280QV10CT_SCK_PORT, DT280QV10CT_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { DT280QV10CT_SDA_PORT, DT280QV10CT_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { DT280QV10CT_RST_PORT, DT280QV10CT_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { DT280QV10CT_DC_PORT,  DT280QV10CT_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { DT280QV10CT_CS_PORT,  DT280QV10CT_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
 *       { DT280QV10CT_TE_PORT,  DT280QV10CT_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
 *       { DT280QV10CT_BL_PORT,  DT280QV10CT_BL_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
 * };

 * const ad_lcdc_io_conf_t dt280qv10ct_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(dt280qv10ct_gpio_cfg) / sizeof(dt280qv10ct_gpio_cfg[0]),
 *       .io_list = dt280qv10ct_gpio_cfg,
 * };
 */

/*********************************************************************
 *
 *       Static code
 *
 *********************************************************************
 */
static const ad_lcdc_driver_conf_t dt280qv10ct_drv = {
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

static const ad_lcdc_controller_conf_t dt280qv10ct_cfg = {
        .io = &dt280qv10ct_io,
        .drv = &dt280qv10ct_drv,
};

static const uint8_t screen_init_cmds[] = {
        /* Power on */
        LCDC_GPIO_SET_INACTIVE(DT280QV10CT_RST_PORT, DT280QV10CT_RST_PIN),
        LCDC_DELAY_MS(10),
        LCDC_GPIO_SET_ACTIVE(DT280QV10CT_RST_PORT, DT280QV10CT_RST_PIN),
        LCDC_DELAY_MS(120),

        LCDC_MIPI_SW_RST(),
        LCDC_DELAY_MS(100),

        LCDC_MIPI_CMD(0xC0), LCDC_MIPI_DATA(0x23),                              // POWERCONTROL1 - GVDD=3.8V
        LCDC_MIPI_CMD(0xC1), LCDC_MIPI_DATA(0x10),                              // POWERCONTROL2
        LCDC_MIPI_CMD(0xC5), LCDC_MIPI_DATA(0x2B), LCDC_MIPI_DATA(0x2B),        // VCOMCONTROL1 - VMH=3.775V - VML=-1.425V
        LCDC_MIPI_CMD(0xC7), LCDC_MIPI_DATA(0xC0),                              // VCOMCONTROL2 -
        LCDC_MIPI_SET_ADDR_MODE(0x48),

        LCDC_MIPI_SET_MODE(0x55), // Set color mode to RGB565
        LCDC_MIPI_SET_TEAR_ON(0x00),
        LCDC_MIPI_CMD(0xB1), LCDC_MIPI_DATA(0x00), LCDC_MIPI_DATA(0x1F),        // FRAMECONTROL
        LCDC_MIPI_CMD(0xB5), LCDC_MIPI_DATA(0x5F), LCDC_MIPI_DATA(0x60), LCDC_MIPI_DATA(0x1F), LCDC_MIPI_DATA(0x1F),

        LCDC_MIPI_SET_POSITION(0, 0, GDI_DISP_RESX - 1, GDI_DISP_RESY - 1),
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(DT280QV10CT_BL_PORT, DT280QV10CT_BL_PIN), // Enable the backlight
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
        LCDC_GPIO_SET_INACTIVE(DT280QV10CT_BL_PORT, DT280QV10CT_BL_PIN), // Disable the backlight
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        (void)frame;  //! Just to suppress compiler warnings
}

#endif /* dg_configLCDC_ADAPTER */

#if dg_configUSE_FT6206

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define FT6206_REG_DEVICE_MODE          0x00 // (R/W)
#define FT6206_REG_GEST_ID              0x01 // (R)
#define FT6206_REG_TD_STATUS            0x02 // (R)
#define FT6206_REG_P1_XH                0x03 // (R)
#define FT6206_REG_P1_XL                0x04 // (R)
#define FT6206_REG_P1_YH                0x05 // (R)
#define FT6206_REG_P1_YL                0x06 // (R)
#define FT6206_REG_P1_WEIGHT            0x07 // (R)
#define FT6206_REG_P1_MISC              0x08 // (R)
#define FT6206_REG_P2_XH                0x09 // (R)
#define FT6206_REG_P2_XL                0x0A // (R)
#define FT6206_REG_P2_YH                0x0B // (R)
#define FT6206_REG_P2_YL                0x0C // (R)
#define FT6206_REG_P2_WEIGHT            0x0D // (R)
#define FT6206_REG_P2_MISC              0x0E // (R)
#define FT6206_REG_TH_GROUP             0x80 // (R/W)
#define FT6206_REG_TH_DIFF              0x85 // (R/W)
#define FT6206_REG_CTRL                 0x86 // (R/W)
#define FT6206_REG_TIMEENTERMONITOR     0x87 // (R/W)
#define FT6206_REG_PERIODACTIVE         0x88 // (R/W)
#define FT6206_REG_PERIODMONITOR        0x89 // (R/W)
#define FT6206_REG_RADIAN_VALUE         0x91 // (R/W)
#define FT6206_REG_OFFSET_LEFT_RIGHT    0x92 // (R/W)
#define FT6206_REG_OFFSET_UP_DOWN       0x93 // (R/W)
#define FT6206_REG_DISTANCE_UP_DOWN     0x94 // (R/W)
#define FT6206_REG_DISTANCE_LEFT_RIGHT  0x95 // (R/W)
#define FT6206_REG_DISTANCE_ZOOM        0x96 // (R/W)
#define FT6206_REG_LIB_VER_H            0xA1 // (R)
#define FT6206_REG_LIB_VER_L            0xA2 // (R)
#define FT6206_REG_CIPHER               0xA3 // (R)
#define FT6206_REG_G_MODE               0xA4 // (R/W)
#define FT6206_REG_PWR_MODE             0xA5 // (R/W)
#define FT6206_REG_FIRMID               0xA6 // (R)
#define FT6206_REG_FOCALTECH_ID         0xA8 // (R)
#define FT6206_REG_RELEASE_CODE_ID      0xAF // (R)
#define FT6206_REG_STATE                0xDB // (R/W)

#define FT6206_I2C_ADDRESS              0x38

/*********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef enum {
        FT6206_EVENT_FLAG_PRESS_DOWN = 0,
        FT6206_EVENT_FLAG_LIFT_UP,
        FT6206_EVENT_FLAG_CONTACT,
        FT6206_EVENT_FLAG_NO_EVENT
} FT6206_EVENT_FLAG;

typedef enum {
        FT6206_GEST_ID_NO_GESTURE   = 0,
        FT6206_GEST_ID_MOVE_UP      = 0x10,
        FT6206_GEST_ID_MOVE_RIGHT   = 0x14,
        FT6206_GEST_ID_MOVE_DOWN    = 0x18,
        FT6206_GEST_ID_MOVE_LEFT    = 0x1C,
        FT6206_GEST_ID_ZOOM_IN      = 0x48,
        FT6206_GEST_ID_ZOOM_OUT     = 0x49,
} FT6206_GEST_ID;

/*********************************************************************
 *
 *       Macros
 *
 **********************************************************************
 */
#define FT6206_GET_EVENT_FLAG(Pn_H)         ((Pn_H & 0xC0) >> 6)
#define FT6206_GET_XY_COORDINATE(PnH, PnL)  (((PnH & 0x0F) << 8) | PnL)

/*
 * const ad_i2c_io_conf_t io_ft6206 = {
 *        .scl = {
 *              .port = FT6206_SCL_PORT, .pin = FT6206_SCL_PIN,
 *              .on  = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
 *              .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
 *        },
 *        .sda = {
 *             .port = FT6206_SDA_PORT, .pin = FT6206_SDA_PORT,
 *             .on  = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
 *             .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
 *        },
 *        .voltage_level = HW_GPIO_POWER_V33
 * };
 */

/*********************************************************************
 *
 *       Static code
 *
 *********************************************************************
 */

/* I2C driver configurations  */
static const ad_i2c_driver_conf_t ft6206_drv = {
        I2C_DEFAULT_CLK_CFG,
        .i2c.speed              = HW_I2C_SPEED_STANDARD, /* 100kb/s */
        .i2c.mode               = HW_I2C_MODE_MASTER,
        .i2c.addr_mode          = HW_I2C_ADDRESSING_7B,
        .i2c.address            = FT6206_I2C_ADDRESS,
        .dma_channel            = HW_DMA_CHANNEL_2
};

/* I2C controller configurations */
static const ad_i2c_controller_conf_t ft6206_cfg = {
        .id  = HW_I2C1,
        .drv = &ft6206_drv,
        .io  = &ft6206_io
};

/**
 * Map touch events to display coordinates (physical dimensions)
 */
__STATIC_INLINE uint16_t ft6206_map_touch_to_display(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min,
                                                                                                   uint16_t out_max)
{
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* User-defined routine used by GDI to initialize the target touch controller */
__STATIC_INLINE void ft6206_init(void *dev)
{
        uint8_t write_addr[] = { FT6206_REG_FOCALTECH_ID };

        uint8_t read_buf[1];
        ad_i2c_write_read(dev, (const uint8_t *)write_addr, sizeof(write_addr),  read_buf, sizeof(read_buf),
                                                                                                HW_I2C_F_ADD_STOP);

        if (read_buf[0] == 0x11) {
        }
        else {
                OS_ASSERT(0);
        }
}

/* User-defined routine used by GDI to read touch events */
__STATIC_INLINE void ft6206_read_event(void *dev, gdi_touch_data_t *touch_data)
{
        uint8_t write_addr[] = { FT6206_REG_GEST_ID };
        uint8_t read_buf[6];

        uint16_t touch_x, touch_y;

        ad_i2c_write_read(dev, (const uint8_t *)write_addr, sizeof(write_addr), read_buf, sizeof(read_buf),
                                                                                                HW_I2C_F_ADD_STOP);
        /* Get and exercise event flag */
        switch(FT6206_GET_EVENT_FLAG(read_buf[2])) {
        case FT6206_EVENT_FLAG_PRESS_DOWN:
                touch_data->pressed = 1;
                break;
        case FT6206_EVENT_FLAG_LIFT_UP:
                touch_data->pressed = 0;
                break;
        case FT6206_EVENT_FLAG_CONTACT:
                touch_data->pressed = 1;
                break;
        case FT6206_EVENT_FLAG_NO_EVENT:
                touch_data->pressed = 0;
                break;
        default:
                /* Invalid status */
                OS_ASSERT(0);
                break;
        }

        touch_x = FT6206_GET_XY_COORDINATE(read_buf[2], read_buf[3]);
        touch_y = FT6206_GET_XY_COORDINATE(read_buf[4], read_buf[5]);

        /* Convert touch data to display coordinates */
        touch_data->x = ft6206_map_touch_to_display(touch_x, 0, GDI_DISP_RESX, GDI_DISP_RESX, 0);
        touch_data->y = ft6206_map_touch_to_display(touch_y, 0, GDI_DISP_RESY, GDI_DISP_RESY, 0);
}

#endif /* dg_configUSE_FT6206 */

#endif /* dg_configUSE_DT280QV10CT */

#endif /* DT280QV10CT_H_ */
