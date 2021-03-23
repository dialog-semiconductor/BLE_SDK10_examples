/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for execution from RAM.
 *
 * Copyright (C) 2017-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_RTT

/*************************************************************************************************\
 * System configuration
 */
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_NONE )
#define dg_configIMAGE_SETUP                    ( DEVELOPMENT_MODE )

#define dg_configUSE_WDOG                       ( 0 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_IS_NOT_CONNECTED )
#define dg_configPOWER_1V8P_ACTIVE              ( 0 )
#define dg_configPOWER_1V8P_SLEEP               ( 0 )

#define dg_configUSE_SW_CURSOR                  ( 1 )
#define dg_configENABLE_CMAC_DEBUGGER           ( 0 )

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                             /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                   ( 30000 )  /* FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configUSE_HW_QSPI2                   ( 0 )
#define dg_configLCDC_ADAPTER                   ( 1 )
#define dg_configUSE_HW_LCDC                    ( 1 )
#define dg_configI2C_ADAPTER                    ( 1 )
#define dg_configUSE_HW_I2C                     ( 1 )
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )

/*************************************************************************************************\
 * Display model selection. Note that one display model can be selected at a time.
 */
#define dg_configUSE_DT280QV10CT                ( 1 )
#define dg_configUSE_HM80160A090                ( 0 )
#define dg_configUSE_LPM012M134B                ( 0 )
#define dg_configUSE_LPM013M091A                ( 0 )
#define dg_configUSE_NHD43480272EFASXN          ( 0 )
#define dg_configUSE_MCT024L6W240320PML         ( 0 )
#define dg_configUSE_PSP27801                   ( 0 )
#define dg_configUSE_E1394AA65A                 ( 0 )
#define dg_configUSE_T1D3BP006                  ( 0 )
#define dg_configUSE_T1D54BP002                 ( 0 )
#define dg_configUSE_LS013B7DH06                ( 0 )
#define dg_configUSE_LS013B7DH03                ( 0 )

/*************************************************************************************************\
 * Touch controller selection. Note that one touch driver can be selected at a time.
 */
#define dg_configUSE_FT6206                     ( 1 )
#define dg_configUSE_FT5306                     ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */
