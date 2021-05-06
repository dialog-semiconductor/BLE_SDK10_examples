/**
 ****************************************************************************************
 *
 * @file custom_config_qspi.h
 *
 * @brief Board Support Package. User Configuration file for cached QSPI mode.
 *
 * Copyright (C) 2016-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_QSPI_H_
#define CUSTOM_CONFIG_QSPI_H_

#include "bsp_definitions.h"

#undef CONFIG_USE_BLE

#define CONFIG_RETARGET

#define dg_configUSB_SUSPEND_MODE               USB_SUSPEND_MODE_IDLE

#define dg_configUSB_DMA_SUPPORT                ( 0 )
#define dg_configUSB_TX_DMA_EP                  ( 1 )
#define dg_configUSB_RX_DMA_EP                  ( 2 )

#define dg_configUSE_DCDC                       ( 1 )

#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      MODE_IS_CACHED
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_FLASH

#define dg_configUSE_WDOG                       ( 1 )
#define dg_configTRACK_OS_HEAP                  ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8P )
#define dg_configFLASH_POWER_DOWN               ( 1 )
#define dg_configPOWER_1V8P_ACTIVE              ( 1 )
#define dg_configPOWER_1V8P_SLEEP               ( 1 )

#define dg_config_USE_FATFS                     ( 1 )
/* Product Header     : @ 0x0000
 * Partition Table    : @ 0x2000
 * FW Image Partition : @ 0x3000
 */

#if (dg_config_USE_FATFS == 1)
#define USE_FATFS_PARTITION_TABLE_4M
#endif

#define USB_MAX_PACKET_SIZE                     ( 64u )
#define USB_SUPPORT_HIGH_SPEED                  ( 0 )
#define dg_configDISABLE_BACKGROUND_FLASH_OPS   ( 1 )


#define dg_configUSE_SYS_CHARGER                ( 0 )
#define dg_configUSE_USB_ENUMERATION            ( 1 )

#define dg_configUSE_HW_TRNG                    ( 1 )

#define dg_configDEFAULT_CLK                    ( sysclk_XTAL32M )

/*************************************************************************************************
 * FreeRTOS specific config
 */
#define OS_FREERTOS                                        /* Define this to use FreeRTOS       */
#define configTOTAL_HEAP_SIZE                   ( 64000 )  /* This is the FreeRTOS Heap Size    */

/*************************************************************************************************
 * Peripheral specific config
 */
#define dg_configGPADC_ADAPTER                  1

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_QSPI_H_ */
