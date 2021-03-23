/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include "ad_i2c.h"
#include "ad_spi.h"
#include "ad_lcdc.h"
#include "peripheral_setup.h"

#ifdef __cplusplus
extern "C" {
#endif

#if dg_configLCDC_ADAPTER

#if dg_configUSE_HM80160A090
extern const ad_lcdc_io_conf_t hm80160a090_io;
#elif dg_configUSE_E1394AA65A
extern const ad_lcdc_io_conf_t e1394aa65a_io;
#elif dg_configUSE_LPM012M134B
extern const ad_lcdc_io_conf_t lpm012m134b_io;
#elif dg_configUSE_NHD43480272EFASXN
extern const ad_lcdc_io_conf_t nhd43480272efasxn_io;
#if dg_configUSE_FT5306
extern const ad_i2c_io_conf_t ft5306_io;
#endif
#elif dg_configUSE_LS013B7DH06
extern const ad_lcdc_io_conf_t ls013b7dh06_io;
#elif dg_configUSE_LS013B7DH03
extern const ad_lcdc_io_conf_t ls013b7dh03_io;
#elif dg_configUSE_DT280QV10CT
extern const ad_lcdc_io_conf_t dt280qv10ct_io;
#if dg_configUSE_FT6206
extern const ad_i2c_io_conf_t ft6206_io;
#endif
#elif dg_configUSE_T1D3BP006
extern const ad_lcdc_io_conf_t t1d3bp006_io;
#elif dg_configUSE_T1D54BP002
extern const ad_lcdc_io_conf_t t1d54bp002_io;
#elif dg_configUSE_PSP27801
extern const ad_lcdc_io_conf_t psp27801_io;
#elif dg_configUSE_MCT024L6W240320PML
extern const ad_lcdc_io_conf_t mct024l6w240320pml_io;
#elif dg_configUSE_LPM013M091A
extern const ad_lcdc_io_conf_t lpm013m091a_io;
#endif

#endif /* dg_configLCDC_ADAPTER */

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */
