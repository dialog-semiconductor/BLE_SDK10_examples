/**
 ****************************************************************************************
 *
 * @file custom_config_qspi.h
 *
 * @brief Board Support Package. User Configuration file for cached QSPI mode.
 *
 * Copyright (C) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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
#ifndef CUSTOM_CONFIG_QSPI_H_
#define CUSTOM_CONFIG_QSPI_H_

#include "bsp_definitions.h"
#include "user_power_measurement_configurations.h"

#define CONFIG_USE_BLE


/* Enable retarget functionality */
#define CONFIG_RETARGET

/*
 * UART2 (HW_UART2) is the default selected UART block. If needed, use the
 * following macro to change to UART1 (HW_UART1) serial block:
 *
 * #define CONFIG_RETARGET_UART  HW_UART1
 */

/* Change the static BLE address of the device */
#define defaultBLE_STATIC_ADDRESS   { 0x01, 0x01, 0x01, 0x06, 0x06, 0x06 }

/*************************************************************************************************\
 * System configuration optional
 */
#if(USER_LP_CLOCK == USER_LP_CLK_EXTERNAL_XTAL)
#define dg_configUSE_LP_CLK                     ( LP_CLK_32000 )
#elif(USER_LP_CLOCK == USER_LP_CLK_XTAL32K)
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#elif(USER_LP_CLOCK == USER_LP_CLK_RCX)
#define dg_configUSE_LP_CLK                     ( LP_CLK_RCX )
#endif

#ifdef USER_FLASH_POWERED_DOWN
#define dg_configFLASH_POWER_DOWN               ( 1 )
#else
#define dg_configFLASH_POWER_DOWN               ( 0 )
#endif

#if( USER_PRODUCTION_MODE_EN == 1 )
#define dg_configIMAGE_SETUP                    PRODUCTION_MODE
#define dg_configSKIP_MAGIC_CHECK_AT_START      (1)
#endif

/*************************************************************************************************\
 * System configuration
 */
#define dg_configEXEC_MODE                      ( MODE_IS_CACHED )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_FLASH )
#define dg_configEMULATE_OTP_COPY               ( 0 )

#define dg_configUSE_WDOG                       ( 0 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8P )

#define dg_configPOWER_1V8P_ACTIVE               ( 1 )
#define dg_configPOWER_1V8P_SLEEP                ( 1 )


#define dg_configBATTERY_TYPE                   ( BATTERY_TYPE_LIMN2O4 )
#define dg_configBATTERY_CHARGE_CURRENT         ( 2 )    // 30mA
#define dg_configBATTERY_PRECHARGE_CURRENT      ( 20 )   // 2.1mA
#define dg_configBATTERY_CHARGE_NTC             ( 1 )    // disabled

#define dg_configUSE_USB                        ( 0 )
#define dg_configUSE_USB_CHARGER                ( 0 )
#define dg_configALLOW_CHARGING_NOT_ENUM        ( 1 )

#define dg_configUSE_SW_CURSOR                  ( 0 )

#define dg_configCACHEABLE_QSPI_AREA_LEN        ( NVMS_PARAM_PART_start - MEMORY_QSPIF_BASE )

#define dg_configTESTMODE_MEASURE_SLEEP_CURRENT ( 0 )
/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    ( 22972 )   /* FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )
#define dg_configNVMS_VES                       ( 1 )
#define dg_configNVPARAM_ADAPTER                ( 1 )


/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_USE_BLE_SERVICES

#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_QSPI_H_ */
