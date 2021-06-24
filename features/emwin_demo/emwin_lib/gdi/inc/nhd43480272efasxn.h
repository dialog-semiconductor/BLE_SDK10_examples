/**
 ****************************************************************************************
 *
 * @file nhd43480272efasxn.h
 *
 * @brief LCD configuration for NHD-4.3-480272EF-ASXN# LCD with IC ST7282T2
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef NHD43480272EFASXN_H_
#define NHD43480272EFASXN_H_

#include <stdio.h>
#include <stdint.h>
#include "platform_devices.h"
#include "ad_i2c.h"

#if dg_configUSE_NHD43480272EFASXN

#if dg_configLCDC_ADAPTER

/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define GDI_DISP_COLOR                  (HW_LCDC_OCM_RGB222)
#define GDI_DISP_RESX                   (480)
#define GDI_DISP_RESY                   (272)
#define GDI_USE_CONTINUOUS_MODE         (1)
#define GDI_LCDC_CONFIG                 (&nhd43480272efasxn_cfg)
#define GDI_TOUCH_ENABLE                (1)
#define GDI_TOUCH_INTERFACE             (GDI_TOUCH_INTERFACE_I2C)
#if dg_configUSE_FT5306
#define GDI_TOUCH_CONFIG                (&ft5306_cfg)
#define GDI_TOUCH_INIT                  (ft5306_init)
#define GDI_TOUCH_READ_EVENT            (ft5306_read_event)
#endif

/*
 * static const ad_io_conf_t nhd43480272efasxn_gpio_cfg[] = {
 *       { NHD43480272EFASXN_GREEN_0_PORT, NHD43480272EFASXN_GREEN_0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_GREEN_1_PORT, NHD43480272EFASXN_GREEN_1_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_RED_0_PORT,   NHD43480272EFASXN_RED_0_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_RED_1_PORT,   NHD43480272EFASXN_RED_1_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_BLUE_0_PORT,  NHD43480272EFASXN_BLUE_0_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_BLUE_1_PORT,  NHD43480272EFASXN_BLUE_1_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_VSYNC_PORT,   NHD43480272EFASXN_VSYNC_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  } },
 *       { NHD43480272EFASXN_HSYNC_PORT,   NHD43480272EFASXN_HSYNC_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  } },
 *       { NHD43480272EFASXN_DEN_PORT,     NHD43480272EFASXN_DEN_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_CLK_PORT,     NHD43480272EFASXN_CLK_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 *       { NHD43480272EFASXN_DISP_PORT,    NHD43480272EFASXN_DISP_PIN,    { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
 * };

 * const ad_lcdc_io_conf_t nhd43480272efasxn_io = {
 *       .voltage_level = HW_GPIO_POWER_V33,
 *       .io_cnt = sizeof(nhd43480272efasxn_gpio_cfg) / sizeof(nhd43480272efasxn_gpio_cfg[0]),
 *       .io_list = nhd43480272efasxn_gpio_cfg,
 * };
 */

/*********************************************************************
 *
 *       Static code
 *
 *********************************************************************
 */
static const ad_lcdc_driver_conf_t nhd43480272efasxn_drv = {
        .hw_init.phy_type = HW_LCDC_PHY_CLASSIC_PARALLEL,
        .hw_init.format = HW_LCDC_OCM_8RGB888,  //! LCD controller requires that RGB888 is selected
        .hw_init.cfg_extra_flags = 0,
        .hw_init.mode =   HW_LCDC_MODE_MIPI_OFF | HW_LCDC_MODE_NEG_H | HW_LCDC_MODE_NEG_V,
        .hw_init.iface_freq = LCDC_FREQ_8MHz,
        .ext_clk = HW_LCDC_EXT_CLK_OFF,
        .te_enable = false,
        .te_polarity = HW_LCDC_TE_LOW,
        .display.resx = GDI_DISP_RESX,
        .display.resy = GDI_DISP_RESY,
        .display.fpx = 2 + 1,  //! Datasheet value increased by 1 for the LCD controller
        .display.bpx = 3 - 1,  //! Datasheet value decreased by 1 for the LCD controller
        .display.blx = 2,
        .display.fpy = 2,
        .display.bpy = 2,
        .display.bly = 2,
};

static const ad_lcdc_controller_conf_t nhd43480272efasxn_cfg = {
        .io = &nhd43480272efasxn_io,
        .drv = &nhd43480272efasxn_drv,
};

static const uint8_t screen_init_cmds[] = {
        /* RGB interface does not support commands. */
};

static const uint8_t screen_power_on_cmds[] = {
};

static const uint8_t screen_enable_cmds[] = {
        LCDC_GPIO_SET_ACTIVE(NHD43480272EFASXN_DISP_PORT, NHD43480272EFASXN_DISP_PIN), // Enable the display
};

static const uint8_t screen_disable_cmds[] = {
        LCDC_GPIO_SET_INACTIVE(NHD43480272EFASXN_DISP_PORT, NHD43480272EFASXN_DISP_PIN), // Disable the display
};

static const uint8_t screen_power_off_cmds[] = {
};

static const uint8_t screen_clear_cmds[] = {
        /* RGB interface does not support commands. */
};

__STATIC_INLINE void screen_set_partial_update_area(hw_lcdc_frame_t *frame)
{
        /* RGB interface does not support partial update  */
        frame->startx = 0;
        frame->starty = 0;
        frame->endx = GDI_DISP_RESX - 1;
        frame->endy = GDI_DISP_RESY - 1;
}

#endif /* dg_configLCDC_ADAPTER */

#if dg_configUSE_FT5306
/*********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define FT5306_REG_DEVICE_MODE               0x00 // (R/W)
#define FT5306_REG_GEST_ID                   0x01 // (R)
#define FT5306_REG_TD_STATUS                 0x02 // (R)
#define FT5306_REG_TOUCH1_XH                 0x03 // (R)
#define FT5306_REG_TOUCH1_XL                 0x04 // (R)
#define FT5306_REG_TOUCH1_YH                 0x05 // (R)
#define FT5306_REG_TOUCH1_YL                 0x06 // (R)
#define FT5306_REG_TOUCH2_XH                 0x09 // (R)
#define FT5306_REG_TOUCH2_XL                 0x0A // (R)
#define FT5306_REG_TOUCH2_YH                 0x0B // (R)
#define FT5306_REG_TOUCH2_YL                 0x0C // (R)
#define FT5306_REG_TOUCH3_XH                 0x0F // (R)
#define FT5306_REG_TOUCH3_XL                 0x10 // (R)
#define FT5306_REG_TOUCH3_YH                 0x11 // (R)
#define FT5306_REG_TOUCH3_YL                 0x12 // (R)
#define FT5306_REG_TOUCH4_XH                 0x15 // (R)
#define FT5306_REG_TOUCH4_XL                 0x16 // (R)
#define FT5306_REG_TOUCH4_YH                 0x17 // (R)
#define FT5306_REG_TOUCH4_YL                 0x18 // (R)
#define FT5306_REG_TOUCH5_XH                 0x1B // (R)
#define FT5306_REG_TOUCH5_XL                 0x1C // (R)
#define FT5306_REG_TOUCH5_YH                 0x1D // (R)
#define FT5306_REG_TOUCH5_YL                 0x1E // (R)
#define FT5306_REG_ID_G_THGROUP              0x80 // (R/W)
#define FT5306_REG_ID_G_THPEAK               0x81 // (R/W)
#define FT5306_REG_ID_G_THCAL                0x82 // (R/W)
#define FT5306_REG_ID_G_THWATER              0x83 // (R/W)
#define FT5306_REG_ID_G_TEMP                 0x84 // (R/W)
#define FT5306_REG_ID_G_THDIFF               0x85 // (R/W)
#define FT5306_REG_ID_G_CTRL                 0x86 // (R/W)
#define FT5306_REG_ID_G_TIME_ENTER_MONITOR   0x87 // (R/W)
#define FT5306_REG_ID_G_PERIODACTIVE         0x88 // (R/W)
#define FT5306_REG_ID_G_PERIODMONITOR        0x89 // (R/W)
#define FT5306_REG_ID_G_AUTO_CLB_MODE        0xA0 // (R)
#define FT5306_REG_ID_G_LIB_VERSION_H        0xA1 // (R)
#define FT5306_REG_ID_G_LIB_VERSION_L        0xA2 // (R)
#define FT5306_REG_ID_G_CIPHER               0xA3 // (R)
#define FT5306_REG_ID_G_MODE                 0xA4 // (R/W)
#define FT5306_REG_ID_G_PMODE                0xA5 // (R/W)
#define FT5306_REG_ID_G_FIRMID               0xA6 // (R)
#define FT5306_REG_ID_G_STATE                0xA7 // (R)
#define FT5306_REG_ID_G_FOCALTECH_ID         0xA8 // (R)
#define FT5306_REG_ID_G_ERR                  0xA9 // (R)
#define FT5306_REG_ID_G_CLB                  0xAA // (R/W)
#define FT5306_REG_LOG_MSG_CNT               0xFE // (R)
#define FT5306_REG_LOG_CUR_CHA               0xFF // (R)

#define FT5306_I2C_ADDRESS     (0x38)

/*********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef enum {
        FT5306_EVENT_FLAG_PRESS_DOWN = 0,
        FT5306_EVENT_FLAG_LIFT_UP,
        FT5306_EVENT_FLAG_CONTACT,
        FT5306_EVENT_FLAG_NO_EVENT
} FT5306_EVENT_FLAG;

typedef enum {
        FT5306_GEST_ID_NO_GESTURE   = 0,
        FT5306_GEST_ID_MOVE_UP      = 0x10,
        FT5306_GEST_ID_MOVE_RIGHT   = 0x14,
        FT5306_GEST_ID_MOVE_DOWN    = 0x18,
        FT5306_GEST_ID_MOVE_LEFT    = 0x1C,
        FT5306_GEST_ID_ZOOM_IN      = 0x48,
        FT5306_GEST_ID_ZOOM_OUT     = 0x49,
} FT5306_GEST_ID;

/*********************************************************************
 *
 *       Macros
 *
 **********************************************************************
 */
#define FT5306_GET_EVENT_FLAG(Pn_H)         ((Pn_H & 0xC0) >> 6)
#define FT5306_GET_XY_COORDINATE(PnH, PnL)  (((PnH & 0x0F) << 8) | PnL)

/*
 * const ad_i2c_io_conf_t ft5306_io = {
 *        .scl = {
 *              .port = FT5306_SCL_PORT, .pin = FT5306_SCL_PIN,
 *              .on  = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
 *              .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
 *        },
 *        .sda = {
 *             .port = FT5306_SDA_PORT, .pin = FT5306_SDA_PORT,
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
static const ad_i2c_driver_conf_t ft5306_drv = {
        I2C_DEFAULT_CLK_CFG,
        .i2c.speed              = HW_I2C_SPEED_STANDARD, /* 100kb/s */
        .i2c.mode               = HW_I2C_MODE_MASTER,
        .i2c.addr_mode          = HW_I2C_ADDRESSING_7B,
        .i2c.address            = FT5306_I2C_ADDRESS,
        .dma_channel            = HW_DMA_CHANNEL_2
};

/* I2C controller configurations */
static const ad_i2c_controller_conf_t ft5306_cfg = {
        .id  = HW_I2C1,
        .drv = &ft5306_drv,
        .io  = &ft5306_io
};

/**
 * Map touch events to display coordinates (physical dimensions)
 */
__STATIC_INLINE uint16_t ft5306_map_touch_to_display(uint16_t x, uint16_t in_min, uint16_t in_max,
                                                                        uint16_t out_min, uint16_t out_max)
{
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* User-defined routine used by GDI to initialize the target touch controller */
__STATIC_INLINE void ft5306_init(void *dev)
{
        uint8_t write_addr[] = { FT5306_REG_ID_G_FOCALTECH_ID };

        uint8_t read_buf[1];
        ad_i2c_write_read(dev, (const uint8_t *)write_addr, sizeof(write_addr), read_buf, sizeof(read_buf),
                                                                                                HW_I2C_F_ADD_STOP);

        if (read_buf[0] == 0x79) {
        }
        else {
                OS_ASSERT(0);
        }
}

/* User-defined routine used by GDI to read touch events */
__STATIC_INLINE void ft5306_read_event(void *dev, gdi_touch_data_t *touch_data)
{
        uint8_t write_addr[] = { FT5306_REG_GEST_ID };
        uint8_t read_buf[6];

        uint16_t touch_x, touch_y;

        ad_i2c_write_read(dev, (const uint8_t *)write_addr, sizeof(write_addr), read_buf, sizeof(read_buf),
                                                                                                HW_I2C_F_ADD_STOP);

        /* Get and exercise event flag */
        switch(FT5306_GET_EVENT_FLAG(read_buf[2])) {
        case FT5306_EVENT_FLAG_PRESS_DOWN:
                touch_data->pressed = 1;
                break;
        case FT5306_EVENT_FLAG_LIFT_UP:
                touch_data->pressed = 0;
                break;
        case FT5306_EVENT_FLAG_CONTACT:
                touch_data->pressed = 1;
                break;
        case FT5306_EVENT_FLAG_NO_EVENT:
                touch_data->pressed = 0;
                break;
        default:
                /* Invalid status */
                OS_ASSERT(0);
                break;
        }

        touch_x = FT5306_GET_XY_COORDINATE(read_buf[2], read_buf[3]);
        touch_y = FT5306_GET_XY_COORDINATE(read_buf[4], read_buf[5]);

        /* Convert touch data to display coordinates */
        touch_data->x = ft5306_map_touch_to_display(touch_x, 0, GDI_DISP_RESX, 0, GDI_DISP_RESX);
        touch_data->y = ft5306_map_touch_to_display(touch_y, 0, GDI_DISP_RESY, 0, GDI_DISP_RESY);
}

#endif /* dg_configUSE_FT5306 */

#endif /* dg_configUSE_NHD43480272EFASXN */

#endif /* NHD43480272EFASXN_H_ */
