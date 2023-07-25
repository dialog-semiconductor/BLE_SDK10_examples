 /**
 ****************************************************************************************
 *
 * @file periph_setup.h
 *
 * @brief Peripheral Setup file.
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
#ifndef PERIPH_SETUP_H_
#define PERIPH_SETUP_H_

#include "hw_gpio.h"
#include "hw_wkup.h"

/*
 * Include definitions for configuring the hardware blocks.
 */
#if dg_configUSE_HM80160A090
#define HM80160A090_SCK_PORT                    HW_GPIO_PORT_1
#define HM80160A090_SCK_PIN                     HW_GPIO_PIN_11

#define HM80160A090_SDA_PORT                    HW_GPIO_PORT_1
#define HM80160A090_SDA_PIN                     HW_GPIO_PIN_17

#define HM80160A090_RST_PORT                    HW_GPIO_PORT_1
#define HM80160A090_RST_PIN                     HW_GPIO_PIN_16

#define HM80160A090_DC_PORT                     HW_GPIO_PORT_1
#define HM80160A090_DC_PIN                      HW_GPIO_PIN_7

#define HM80160A090_CS_PORT                     HW_GPIO_PORT_1
#define HM80160A090_CS_PIN                      HW_GPIO_PIN_2
#endif /* dg_configUSE_HM80160A090 */

#if dg_configUSE_E1394AA65A
#define E1394AA65A_TE_PORT                      HW_GPIO_PORT_0
#define E1394AA65A_TE_PIN                       HW_GPIO_PIN_24

#define E1394AA65A_SCK_PORT                     HW_GPIO_PORT_0
#define E1394AA65A_SCK_PIN                      HW_GPIO_PIN_26

#define E1394AA65A_DO_PORT                      HW_GPIO_PORT_0
#define E1394AA65A_DO_PIN                       HW_GPIO_PIN_27

#define E1394AA65A_CS_PORT                      HW_GPIO_PORT_0
#define E1394AA65A_CS_PIN                       HW_GPIO_PIN_28

#define E1394AA65A_DC_PORT                      HW_GPIO_PORT_0
#define E1394AA65A_DC_PIN                       HW_GPIO_PIN_29

#define E1394AA65A_RST_PORT                     HW_GPIO_PORT_0
#define E1394AA65A_RST_PIN                      HW_GPIO_PIN_31
#endif /* dg_configUSE_E1394AA65A */

#if dg_configUSE_LPM012M134B
#define LPM012M134B_BLUE_0_PORT                 HW_GPIO_PORT_1
#define LPM012M134B_BLUE_0_PIN                  HW_GPIO_PIN_2

#define LPM012M134B_BLUE_1_PORT                 HW_GPIO_PORT_1
#define LPM012M134B_BLUE_1_PIN                  HW_GPIO_PIN_3

#define LPM012M134B_GREEN_0_PORT                HW_GPIO_PORT_1
#define LPM012M134B_GREEN_0_PIN                 HW_GPIO_PIN_4

#define LPM012M134B_GREEN_1_PORT                HW_GPIO_PORT_1
#define LPM012M134B_GREEN_1_PIN                 HW_GPIO_PIN_5

#define LPM012M134B_RED_0_PORT                  HW_GPIO_PORT_1
#define LPM012M134B_RED_0_PIN                   HW_GPIO_PIN_7

#define LPM012M134B_RED_1_PORT                  HW_GPIO_PORT_1
#define LPM012M134B_RED_1_PIN                   HW_GPIO_PIN_8

#define LPM012M134B_FRP_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_FRP_PIN                     HW_GPIO_PIN_10

#define LPM012M134B_XFRP_PORT                   HW_GPIO_PORT_1
#define LPM012M134B_XFRP_PIN                    HW_GPIO_PIN_11

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.26
 */
#define LPM012M134B_VCK_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_VCK_PIN                     HW_GPIO_PIN_13

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.27
 */
#define LPM012M134B_ENB_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_ENB_PIN                     HW_GPIO_PIN_14

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.28
 */
#define LPM012M134B_VST_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_VST_PIN                     HW_GPIO_PIN_15

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.29
 */
#define LPM012M134B_HCK_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_HCK_PIN                     HW_GPIO_PIN_16

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.30
 */
#define LPM012M134B_HST_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_HST_PIN                     HW_GPIO_PIN_17

/*
 * \note Applicable on DA14697/9 SoC. On DA14691/5 the pin should be changed to P0.31
 */
#define LPM012M134B_XRST_PORT                   HW_GPIO_PORT_1
#define LPM012M134B_XRST_PIN                    HW_GPIO_PIN_21

#define LPM012M134B_PEN_PORT                    HW_GPIO_PORT_1
#define LPM012M134B_PEN_PIN                     HW_GPIO_PIN_19
#endif /* dg_configUSE_LPM012M134B */

#if dg_configUSE_NHD43480272EFASXN
#define NHD43480272EFASXN_BLUE_0_PORT           HW_GPIO_PORT_1
#define NHD43480272EFASXN_BLUE_0_PIN            HW_GPIO_PIN_2

#define NHD43480272EFASXN_BLUE_1_PORT           HW_GPIO_PORT_1
#define NHD43480272EFASXN_BLUE_1_PIN            HW_GPIO_PIN_3

#define NHD43480272EFASXN_GREEN_0_PORT          HW_GPIO_PORT_1
#define NHD43480272EFASXN_GREEN_0_PIN           HW_GPIO_PIN_4

#define NHD43480272EFASXN_GREEN_1_PORT          HW_GPIO_PORT_1
#define NHD43480272EFASXN_GREEN_1_PIN           HW_GPIO_PIN_5

#define NHD43480272EFASXN_RED_0_PORT            HW_GPIO_PORT_1
#define NHD43480272EFASXN_RED_0_PIN             HW_GPIO_PIN_7

#define NHD43480272EFASXN_RED_1_PORT            HW_GPIO_PORT_1
#define NHD43480272EFASXN_RED_1_PIN             HW_GPIO_PIN_8

#define NHD43480272EFASXN_DEN_PORT              HW_GPIO_PORT_0
#define NHD43480272EFASXN_DEN_PIN               HW_GPIO_PIN_27

#define NHD43480272EFASXN_VSYNC_PORT            HW_GPIO_PORT_0
#define NHD43480272EFASXN_VSYNC_PIN             HW_GPIO_PIN_28

#define NHD43480272EFASXN_CLK_PORT              HW_GPIO_PORT_0
#define NHD43480272EFASXN_CLK_PIN               HW_GPIO_PIN_29

#define NHD43480272EFASXN_HSYNC_PORT            HW_GPIO_PORT_0
#define NHD43480272EFASXN_HSYNC_PIN             HW_GPIO_PIN_30

#define NHD43480272EFASXN_DISP_PORT             HW_GPIO_PORT_0
#define NHD43480272EFASXN_DISP_PIN              HW_GPIO_PIN_31

#if dg_configUSE_FT5306
#define FT5306_SCL_PORT                         HW_GPIO_PORT_0
#define FT5306_SCL_PIN                          HW_GPIO_PIN_20

#define FT5306_SDA_PORT                         HW_GPIO_PORT_0
#define FT5306_SDA_PIN                          HW_GPIO_PIN_19
#endif /* dg_configUSE_FT5306 */

#endif /* dg_configUSE_NHD480272EF */

#if dg_configUSE_LS013B7DH06
#define LS013B7HD06_SCK_PORT                    HW_GPIO_PORT_0
#define LS013B7HD06_SCK_PIN                     HW_GPIO_PIN_28

#define LS013B7HD06_SDA_PORT                    HW_GPIO_PORT_0
#define LS013B7HD06_SDA_PIN                     HW_GPIO_PIN_29

#define LS013B7HD06_CS_PORT                     HW_GPIO_PORT_0
#define LS013B7HD06_CS_PIN                      HW_GPIO_PIN_30

#define LS013B7HD06_EXTCOMIN_PORT               HW_GPIO_PORT_1
#define LS013B7HD06_EXTCOMIN_PIN                HW_GPIO_PIN_10

#define LS013B7HD06_DISP_PORT                   HW_GPIO_PORT_0
#define LS013B7HD06_DISP_PIN                    HW_GPIO_PIN_31
#endif /* dg_configUSE_LS013B7DH06 */

#if dg_configUSE_LS013B7DH03
#define LS013B7DH03_SCK_PORT                    HW_GPIO_PORT_0
#define LS013B7DH03_SCK_PIN                     HW_GPIO_PIN_28

#define LS013B7DH03_SDA_PORT                    HW_GPIO_PORT_0
#define LS013B7DH03_SDA_PIN                     HW_GPIO_PIN_29

#define LS013B7DH03_CS_PORT                     HW_GPIO_PORT_0
#define LS013B7DH03_CS_PIN                      HW_GPIO_PIN_30

#define LS013B7DH03_EXTCOMIN_PORT               HW_GPIO_PORT_1
#define LS013B7DH03_EXTCOMIN_PIN                HW_GPIO_PIN_10

#define LS013B7DH03_DISP_PORT                   HW_GPIO_PORT_0
#define LS013B7DH03_DISP_PIN                    HW_GPIO_PIN_31
#endif /* dg_configUSE_LS013B7DH03 */

#if dg_configUSE_DT280QV10CT
#define DT280QV10CT_SDA_PORT                    HW_GPIO_PORT_0
#define DT280QV10CT_SDA_PIN                     HW_GPIO_PIN_24

#define DT280QV10CT_RST_PORT                    HW_GPIO_PORT_0
#define DT280QV10CT_RST_PIN                     HW_GPIO_PIN_12

#define DT280QV10CT_SCK_PORT                    HW_GPIO_PORT_0
#define DT280QV10CT_SCK_PIN                     HW_GPIO_PIN_27

#define DT280QV10CT_DC_PORT                     HW_GPIO_PORT_0
#define DT280QV10CT_DC_PIN                      HW_GPIO_PIN_20

#define DT280QV10CT_CS_PORT                     HW_GPIO_PORT_0
#define DT280QV10CT_CS_PIN                      HW_GPIO_PIN_21

#define DT280QV10CT_TE_PORT                     HW_GPIO_PORT_1
#define DT280QV10CT_TE_PIN                      HW_GPIO_PIN_22

/*
 * \note For the resistive touch shield the backlight is routed on P1.5
 */
#define DT280QV10CT_BL_PORT                     HW_GPIO_PORT_1
#define DT280QV10CT_BL_PIN                      HW_GPIO_PIN_8

#if dg_configUSE_FT6206
#define FT6206_SCL_PORT                         HW_GPIO_PORT_0
#define FT6206_SCL_PIN                          HW_GPIO_PIN_29

#define FT6206_SDA_PORT                         HW_GPIO_PORT_0
#define FT6206_SDA_PIN                          HW_GPIO_PIN_28
#endif /* dg_configUSE_FT6206 */

#endif /* dg_configUSE_DT280QV10CT */

#if dg_configUSE_T1D3BP006
#define T1D3BP006_CS_PORT                       HW_GPIO_PORT_0
#define T1D3BP006_CS_PIN                        HW_GPIO_PIN_20

#define T1D3BP006_SCK_PORT                      HW_GPIO_PORT_0
#define T1D3BP006_SCK_PIN                       HW_GPIO_PIN_21

#define T1D3BP006_SDA_PORT                      HW_GPIO_PORT_0
#define T1D3BP006_SDA_PIN                       HW_GPIO_PIN_30

#define T1D3BP006_RST_PORT                      HW_GPIO_PORT_0
#define T1D3BP006_RST_PIN                       HW_GPIO_PIN_31

#define T1D3BP006_TE_PORT                       HW_GPIO_PORT_0
#define T1D3BP006_TE_PIN                        HW_GPIO_PIN_24

#define T1D3BP006_DC_PORT                       HW_GPIO_PORT_0
#define T1D3BP006_DC_PIN                        HW_GPIO_PIN_26
#endif /* dg_configUSE_T1D3BP006 */

#if dg_configUSE_T1D54BP002
#define T1D54BP002_CS_PORT                      HW_GPIO_PORT_0
#define T1D54BP002_CS_PIN                       HW_GPIO_PIN_20

#define T1D54BP002_SCK_PORT                     HW_GPIO_PORT_0
#define T1D54BP002_SCK_PIN                      HW_GPIO_PIN_21

#define T1D54BP002_SDA_PORT                     HW_GPIO_PORT_0
#define T1D54BP002_SDA_PIN                      HW_GPIO_PIN_30

#define T1D54BP002_RST_PORT                     HW_GPIO_PORT_0
#define T1D54BP002_RST_PIN                      HW_GPIO_PIN_31
#endif /* dg_configUSE_T1D54BP002 */

#if dg_configUSE_PSP27801
#define PSP27801_RW_PORT                        HW_GPIO_PORT_1
#define PSP27801_RW_PIN                         HW_GPIO_PIN_9

#define PSP27801_CS_PORT                        HW_GPIO_PORT_0
#define PSP27801_CS_PIN                         HW_GPIO_PIN_20

#define PSP27801_SCK_PORT                       HW_GPIO_PORT_0
#define PSP27801_SCK_PIN                        HW_GPIO_PIN_21

#define PSP27801_SDO_PORT                       HW_GPIO_PORT_0
#define PSP27801_SDO_PIN                        HW_GPIO_PIN_24

#define PSP27801_SDI_PORT                       HW_GPIO_PORT_0
#define PSP27801_SDI_PIN                        HW_GPIO_PIN_26

#define PSP27801_DC_PORT                        HW_GPIO_PORT_1
#define PSP27801_DC_PIN                         HW_GPIO_PIN_1

#define PSP27801_EN_PORT                        HW_GPIO_PORT_0
#define PSP27801_EN_PIN                         HW_GPIO_PIN_27

#define PSP27801_RST_PORT                       HW_GPIO_PORT_0
#define PSP27801_RST_PIN                        HW_GPIO_PIN_12
#endif /* dg_configUSE_PSP27801 */

#if dg_configUSE_MCT024L6W240320PML
#define MCT024L6W240320PML_SDA_PORT             HW_GPIO_PORT_1
#define MCT024L6W240320PML_SDA_PIN              HW_GPIO_PIN_17

#define MCT024L6W240320PML_RST_PORT             HW_GPIO_PORT_1
#define MCT024L6W240320PML_RST_PIN              HW_GPIO_PIN_16

#define MCT024L6W240320PML_SCK_PORT             HW_GPIO_PORT_1
#define MCT024L6W240320PML_SCK_PIN              HW_GPIO_PIN_5

#define MCT024L6W240320PML_DC_PORT              HW_GPIO_PORT_1
#define MCT024L6W240320PML_DC_PIN               HW_GPIO_PIN_2

#define MCT024L6W240320PML_CS_PORT              HW_GPIO_PORT_1
#define MCT024L6W240320PML_CS_PIN               HW_GPIO_PIN_3
#endif /* dg_configUSE_MCT024L6W240320PML */

#if dg_configUSE_LPM013M091A
#define LPM013M091A_SDI_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_SDI_PIN                     HW_GPIO_PIN_13

#define LPM013M091A_SCL_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_SCL_PIN                     HW_GPIO_PIN_17

#define LPM013M091A_DCX_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_DCX_PIN                     HW_GPIO_PIN_14

#define LPM013M091A_CSX_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_CSX_PIN                     HW_GPIO_PIN_16

#define LPM013M091A_RST_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_RST_PIN                     HW_GPIO_PIN_7

#define LPM013M091A_TE_PORT                     HW_GPIO_PORT_1
#define LPM013M091A_TE_PIN                      HW_GPIO_PIN_22

#define LPM013M091A_IM0_PORT                    HW_GPIO_PORT_1
#define LPM013M091A_IM0_PIN                     HW_GPIO_PIN_3
#endif /* dg_configUSE_LPM013M091A */

/*********************************************************************************
 * Touch controller's interrupt line
 */
#define TOUCH_INT_PORT                          HW_GPIO_PORT_0
#define TOUCH_INT_PIN                           HW_GPIO_PIN_18
#define TOUCH_INT_POL                           HW_WKUP_PIN_STATE_HIGH

#endif /* PERIPH_SETUP_H_ */
