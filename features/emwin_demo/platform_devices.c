/**
 ****************************************************************************************
 *
 * @file platform_devices.c
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2017-2021 Renesas Electronics Corporation and/or its affiliates
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
#include "ad_i2c.h"
#include "ad_spi.h"
#include "ad_lcdc.h"
#include "peripheral_setup.h"
#include "platform_devices.h"



/*
 * PLATFORM PERIPHERALS GPIO CONFIGURATION
 *****************************************************************************************
 */

#if dg_configLCDC_ADAPTER

#if dg_configUSE_HM80160A090
static const ad_io_conf_t hm80160a090_gpio_cfg[] = {
        { HM80160A090_SCK_PORT, HM80160A090_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { HM80160A090_SDA_PORT, HM80160A090_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { HM80160A090_RST_PORT, HM80160A090_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { HM80160A090_DC_PORT,  HM80160A090_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { HM80160A090_CS_PORT,  HM80160A090_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
};

const ad_lcdc_io_conf_t hm80160a090_io = {
        .voltage_level = HW_GPIO_POWER_VDD1V8P,
        .io_cnt = sizeof(hm80160a090_gpio_cfg) / sizeof(hm80160a090_gpio_cfg[0]),
        .io_list = hm80160a090_gpio_cfg,
};

#elif dg_configUSE_E1394AA65A
static const ad_io_conf_t e1394aa65a_gpio_cfg[] = {
        { E1394AA65A_SCK_PORT, E1394AA65A_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { E1394AA65A_DO_PORT,  E1394AA65A_DO_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { E1394AA65A_DC_PORT,  E1394AA65A_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { E1394AA65A_CS_PORT,  E1394AA65A_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { E1394AA65A_RST_PORT, E1394AA65A_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { E1394AA65A_TE_PORT,  E1394AA65A_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
};

const ad_lcdc_io_conf_t e1394aa65a_io = {
        .voltage_level = HW_GPIO_POWER_VDD1V8P,
        .io_cnt = sizeof(e1394aa65a_gpio_cfg) / sizeof(e1394aa65a_gpio_cfg[0]),
        .io_list = e1394aa65a_gpio_cfg,
};

#elif dg_configUSE_LPM012M134B
static const ad_io_conf_t lpm012m134b_gpio_cfg[] = {
        { LPM012M134B_GREEN_0_PORT, LPM012M134B_GREEN_0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_GREEN_1_PORT, LPM012M134B_GREEN_1_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_RED_0_PORT,   LPM012M134B_RED_0_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_RED_1_PORT,   LPM012M134B_RED_1_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_BLUE_0_PORT,  LPM012M134B_BLUE_0_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_BLUE_1_PORT,  LPM012M134B_BLUE_1_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_XRST_PORT,    LPM012M134B_XRST_PIN,    { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_HST_PORT,     LPM012M134B_HST_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_HCK_PORT,     LPM012M134B_HCK_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_VST_PORT,     LPM012M134B_VST_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_VCK_PORT,     LPM012M134B_VCK_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_ENB_PORT,     LPM012M134B_ENB_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_FRP_PORT,     LPM012M134B_FRP_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_XFRP_PORT,    LPM012M134B_XFRP_PIN,    { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { LPM012M134B_PEN_PORT,     LPM012M134B_PEN_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
};

const ad_lcdc_io_conf_t lpm012m134b_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(lpm012m134b_gpio_cfg) / sizeof(lpm012m134b_gpio_cfg[0]),
        .io_list = lpm012m134b_gpio_cfg,
};

#elif dg_configUSE_NHD43480272EFASXN
static const ad_io_conf_t nhd43480272efasxn_gpio_cfg[] = {
        { NHD43480272EFASXN_GREEN_0_PORT, NHD43480272EFASXN_GREEN_0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { NHD43480272EFASXN_GREEN_1_PORT, NHD43480272EFASXN_GREEN_1_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { NHD43480272EFASXN_RED_0_PORT,   NHD43480272EFASXN_RED_0_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { NHD43480272EFASXN_RED_1_PORT,   NHD43480272EFASXN_RED_1_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { NHD43480272EFASXN_BLUE_0_PORT,  NHD43480272EFASXN_BLUE_0_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { NHD43480272EFASXN_BLUE_1_PORT,  NHD43480272EFASXN_BLUE_1_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { NHD43480272EFASXN_VSYNC_PORT,   NHD43480272EFASXN_VSYNC_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  } },
        { NHD43480272EFASXN_HSYNC_PORT,   NHD43480272EFASXN_HSYNC_PIN,   { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  } },
        { NHD43480272EFASXN_DEN_PORT,     NHD43480272EFASXN_DEN_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { NHD43480272EFASXN_CLK_PORT,     NHD43480272EFASXN_CLK_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,  false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
        { NHD43480272EFASXN_DISP_PORT,    NHD43480272EFASXN_DISP_PIN,    { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false } },
};

const ad_lcdc_io_conf_t nhd43480272efasxn_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(nhd43480272efasxn_gpio_cfg) / sizeof(nhd43480272efasxn_gpio_cfg[0]),
        .io_list = nhd43480272efasxn_gpio_cfg,
};

#if dg_configUSE_FT5306
/* I2C I/O pins configurations */
const ad_i2c_io_conf_t ft5306_io = {
        .scl = {
              .port = FT5306_SCL_PORT, .pin = FT5306_SCL_PIN,
              .on  = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
              .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
        },
        .sda = {
             .port = FT5306_SDA_PORT, .pin = FT5306_SDA_PIN,
             .on   = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
             .off  = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
        },
        .voltage_level = HW_GPIO_POWER_V33
};
#endif /* dg_configUSE_FT5306 */

#elif dg_configUSE_LS013B7DH06
static const ad_io_conf_t ls013b7dh06_gpio_cfg[] = {
        { LS013B7HD06_SCK_PORT,      LS013B7HD06_SCK_PIN,      { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
        { LS013B7HD06_SDA_PORT,      LS013B7HD06_SDA_PIN,      { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
        { LS013B7HD06_CS_PORT,       LS013B7HD06_CS_PIN,       { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
        { LS013B7HD06_EXTCOMIN_PORT, LS013B7HD06_EXTCOMIN_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD, false        }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
        { LS013B7HD06_DISP_PORT,     LS013B7HD06_DISP_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false       }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
};

const ad_lcdc_io_conf_t ls013b7dh06_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(ls013b7dh06_gpio_cfg) / sizeof(ls013b7dh06_gpio_cfg[0]),
        .io_list = ls013b7dh06_gpio_cfg,
};

#elif dg_configUSE_LS013B7DH03
static const ad_io_conf_t ls013b7dh03_gpio_cfg[] = {
        { LS013B7DH03_SCK_PORT,      LS013B7DH03_SCK_PIN,      { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
        { LS013B7DH03_SDA_PORT,      LS013B7DH03_SDA_PIN,      { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
        { LS013B7DH03_CS_PORT,       LS013B7DH03_CS_PIN,       { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
        { LS013B7DH03_EXTCOMIN_PORT, LS013B7DH03_EXTCOMIN_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD,         false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
        { LS013B7DH03_DISP_PORT,     LS013B7DH03_DISP_PIN,     { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
};

const ad_lcdc_io_conf_t ls013b7dh03_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(ls013b7dh03_gpio_cfg) / sizeof(ls013b7dh03_gpio_cfg[0]),
        .io_list = ls013b7dh03_gpio_cfg,
};

#elif dg_configUSE_DT280QV10CT
static const ad_io_conf_t dt280qv10ct_gpio_cfg[] = {
        { DT280QV10CT_SCK_PORT, DT280QV10CT_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { DT280QV10CT_SDA_PORT, DT280QV10CT_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { DT280QV10CT_RST_PORT, DT280QV10CT_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { DT280QV10CT_DC_PORT,  DT280QV10CT_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { DT280QV10CT_CS_PORT,  DT280QV10CT_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { DT280QV10CT_TE_PORT,  DT280QV10CT_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
        { DT280QV10CT_BL_PORT,  DT280QV10CT_BL_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
};

const ad_lcdc_io_conf_t dt280qv10ct_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(dt280qv10ct_gpio_cfg) / sizeof(dt280qv10ct_gpio_cfg[0]),
        .io_list = dt280qv10ct_gpio_cfg,
};

#if dg_configUSE_FT6206
/* I2C I/O pins configurations */
const ad_i2c_io_conf_t ft6206_io = {
        .scl = {
              .port = FT6206_SCL_PORT, .pin = FT6206_SCL_PIN,
              .on  = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SCL, false },
              .off = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
        },
        .sda = {
             .port = FT6206_SDA_PORT, .pin = FT6206_SDA_PIN,
             .on   = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C_SDA, false },
             .off  = { HW_GPIO_MODE_INPUT,             HW_GPIO_FUNC_GPIO,    true  },
        },
        .voltage_level = HW_GPIO_POWER_V33
};
#endif /* dg_configUSE_FT6206 */

#elif dg_configUSE_T1D3BP006
static const ad_io_conf_t t1d3bp006_gpio_cfg[] = {
        { T1D3BP006_SCK_PORT, T1D3BP006_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { T1D3BP006_SDA_PORT, T1D3BP006_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { T1D3BP006_RST_PORT, T1D3BP006_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { T1D3BP006_DC_PORT,  T1D3BP006_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { T1D3BP006_CS_PORT,  T1D3BP006_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { T1D3BP006_TE_PORT,  T1D3BP006_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
};

const ad_lcdc_io_conf_t t1d3bp006_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(t1d3bp006_gpio_cfg) / sizeof(t1d3bp006_gpio_cfg[0]),
        .io_list = t1d3bp006_gpio_cfg,
};

#elif dg_configUSE_T1D54BP002
static const ad_io_conf_t t1d54bp002_gpio_cfg[] = {
        { T1D54BP002_SCK_PORT, T1D54BP002_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { T1D54BP002_SDA_PORT, T1D54BP002_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { T1D54BP002_RST_PORT, T1D54BP002_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { T1D54BP002_CS_PORT,  T1D54BP002_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
};

const ad_lcdc_io_conf_t t1d54bp002_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(t1d54bp002_gpio_cfg) / sizeof(t1d54bp002_gpio_cfg[0]),
        .io_list = t1d54bp002_gpio_cfg,
};

#elif dg_configUSE_PSP27801
static const ad_io_conf_t psp27801_gpio_cfg[] = {
        {PSP27801_SDI_PORT, PSP27801_SDI_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {PSP27801_SCK_PORT, PSP27801_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {PSP27801_CS_PORT,  PSP27801_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {PSP27801_DC_PORT,  PSP27801_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {PSP27801_EN_PORT,  PSP27801_EN_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
        {PSP27801_RST_PORT, PSP27801_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {PSP27801_RW_PORT,  PSP27801_RW_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false}},
};

const ad_lcdc_io_conf_t psp27801_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(psp27801_gpio_cfg) / sizeof(psp27801_gpio_cfg[0]),
        .io_list = psp27801_gpio_cfg,
};

#elif dg_configUSE_MCT024L6W240320PML
static const ad_io_conf_t mct024l6w240320pml_gpio_cfg[] = {
        {MCT024L6W240320PML_SDA_PORT, MCT024L6W240320PML_SDA_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {MCT024L6W240320PML_SCK_PORT, MCT024L6W240320PML_SCK_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {MCT024L6W240320PML_CS_PORT,  MCT024L6W240320PML_CS_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {MCT024L6W240320PML_DC_PORT,  MCT024L6W240320PML_DC_PIN,  { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        {MCT024L6W240320PML_RST_PORT, MCT024L6W240320PML_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
};

const ad_lcdc_io_conf_t mct024l6w240320pml_io = {
        .voltage_level = HW_GPIO_POWER_V33,
        .io_cnt = sizeof(mct024l6w240320pml_gpio_cfg) / sizeof(mct024l6w240320pml_gpio_cfg[0]),
        .io_list = mct024l6w240320pml_gpio_cfg,
};

#elif dg_configUSE_LPM013M091A
static const ad_io_conf_t lpm013m091a_gpio_cfg[] = {
        { LPM013M091A_SCL_PORT, LPM013M091A_SCL_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_CLK, true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { LPM013M091A_SDI_PORT, LPM013M091A_SDI_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DO,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { LPM013M091A_RST_PORT, LPM013M091A_RST_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
#if 0 /* SPI3 */
        { LPM013M091A_DCX_PORT, LPM013M091A_DCX_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
        { LPM013M091A_IM0_PORT, LPM013M091A_IM0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        false}, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, false }},
#else
        { LPM013M091A_DCX_PORT, LPM013M091A_DCX_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_DC,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
        { LPM013M091A_IM0_PORT, LPM013M091A_IM0_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO,        true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true  }},
#endif
        { LPM013M091A_CSX_PORT, LPM013M091A_CSX_PIN, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_LCD_SPI_EN,  true }, { HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO, true }},
        { LPM013M091A_TE_PORT,  LPM013M091A_TE_PIN,  { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_LCD,         true }, { HW_GPIO_MODE_INPUT,  HW_GPIO_FUNC_GPIO, true }},
};

const ad_lcdc_io_conf_t lpm013m091a_io = {
        .voltage_level = HW_GPIO_POWER_VDD1V8P,
        .io_cnt = sizeof(lpm013m091a_gpio_cfg) / sizeof(lpm013m091a_gpio_cfg[0]),
        .io_list = lpm013m091a_gpio_cfg,
};

#endif

#endif /* dg_configLCDC_ADAPTER */

/*
 * PLATFORM PERIPHERALS CONTROLLER CONFIGURATION
 *****************************************************************************************
 */


