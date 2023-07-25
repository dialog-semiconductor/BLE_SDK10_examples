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

#define CONFIG_USE_BLE

/*************************************************************************************************\
 * System configuration
 */
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      ( MODE_IS_CACHED )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_FLASH )


#define dg_configUSE_WDOG                       ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8P )
#define dg_configFLASH_POWER_DOWN               ( 1 )
#define dg_configPOWER_1V8P_ACTIVE              ( 1 )
#define dg_configPOWER_1V8P_SLEEP               ( 1 )

#define dg_configUSE_SYS_CHARGER                ( 1 )

#define dg_configUSE_SW_CURSOR                  ( 1 )

/* Uncomment the following line to do performance test. */
#define SOCF_PERFORMANCE_TEST
#if defined(SOCF_PERFORMANCE_TEST)
        #define CONFIG_RETARGET
        #define dg_configUART_ADAPTER                   ( 1 )
        #define dg_configUART_SOFTWARE_FIFO             ( 1 )
        #define dg_configUART2_SOFTWARE_FIFO            ( 1 )
        #ifndef dg_configUART2_SOFTWARE_FIFO_SIZE
        #define dg_configUART2_SOFTWARE_FIFO_SIZE       ( 128 )
        #endif /* dg_configUART2_SOFTWARE_FIFO_SIZE */
        #define dg_configUSE_CLI                        ( 1 )
        #define dg_configUSE_CONSOLE                    ( 1 )
        #ifndef CONFIG_CLI_LINEBUF_SIZE
        #define CONFIG_CLI_LINEBUF_SIZE                 ( 128 )
        #endif /* CONFIG_CLI_LINEBUF_SIZE */
#else
        #define __HEAP_SIZE  0x0200
		#define dg_configUART_ADAPTER                   ( 0 )
#endif

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                             /* Define this to use FreeRTOS */
#if defined(SOCF_PERFORMANCE_TEST)
#define configTOTAL_HEAP_SIZE                    24200   /* FreeRTOS Total Heap Size */
#else
#define configTOTAL_HEAP_SIZE                    19248   /* FreeRTOS Total Heap Size */
#endif


/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )
#define dg_configNVMS_VES                       ( 1 )
#define dg_configNVPARAM_ADAPTER                ( 1 )
#define dg_configNVPARAM_APP_AREA               ( 1 )
#define dg_configGPADC_ADAPTER                  ( 1 )
#define dg_configSDADC_ADAPTER                  ( 0 )
#define dg_configUSE_HW_SENSOR_NODE             ( 0 )
#define dg_configUSE_SNC_HW_GPADC               ( 0 )
#define dg_configSNC_ADAPTER                    ( 0 )
#define dg_configUSE_SNC_QUEUES                 ( 0 )
#define dg_configUSE_HW_UART                    ( 1 )

/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_USE_BLE_SERVICES

#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )

#define defaultBLE_ATT_DB_CONFIGURATION         ( 0x10 )  // Peripheral Pref. Conn. Param. attribute
#define defaultBLE_PPCP_INTERVAL_MIN            ( BLE_CONN_INTERVAL_FROM_MS( 500 ) )    // 500 ms
#define defaultBLE_PPCP_INTERVAL_MAX            ( BLE_CONN_INTERVAL_FROM_MS( 750 ) )    // 750 ms
#define defaultBLE_PPCP_SLAVE_LATENCY           ( 0 )                                   // 0 events
#define defaultBLE_PPCP_SUP_TIMEOUT             ( BLE_SUPERVISION_TMO_FROM_MS( 6000 ) ) // 6000 ms

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"
#endif /* CUSTOM_CONFIG_QSPI_H_ */
