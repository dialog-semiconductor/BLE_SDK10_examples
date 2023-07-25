/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for RAM mode.
 *
 * Copyright (C) 2022-2023 Renesas Electronics Corporation and/or its affiliates
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
#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE

/*************************************************************************************************\
 * DSPS specific config
 *
 *************************************************************************************************/
/* Enable the log on SEGGER RTT, or printf will be empty function */
#define DBG_LOG_ENABLE          ( 1 )

#if DBG_LOG_ENABLE
   #define CONFIG_RTT
#endif

/* set one UART flow control mode */
#define CFG_UART_HW_FLOW_CTRL

/* Enable circular DMA so no data are overwritten in the RX HW FIFOs upon file transfers. */
#define dg_configUART_RX_CIRCULAR_DMA                ( 1 )
/* This value has been adjusted so that no data loss is encountered in file transfer operations. */
#define dg_configUART2_RX_CIRCULAR_DMA_BUF_SIZE      1024

/* Set UART baudrate */
#define CFG_UART_SPS_BAUDRATE                   HW_UART_BAUDRATE_1000000

/* Necessary for the external MCU UART communication */
#define CONFIG_UART_IGNORE_BUSY_DETECT

/* CPU clock set to max 96MHz */
#define CUSTOM_SYS_CLK                          sysclk_PLL96

#define GENERATE_RANDOM_DEVICE_ADDRESS          ( 1 )

/* Default connection parameter */
#define defaultBLE_PPCP_INTERVAL_MIN            (BLE_CONN_INTERVAL_FROM_MS(30))
#define defaultBLE_PPCP_INTERVAL_MAX            (BLE_CONN_INTERVAL_FROM_MS(30))
#define defaultBLE_PPCP_SLAVE_LATENCY           (0)
#define defaultBLE_PPCP_SUP_TIMEOUT             (BLE_SUPERVISION_TMO_FROM_MS(1000))
/* Min. connection event length decides number of packets/connection
 * increase this value will increase throughput if device is master */
#define defaultBLE_CONN_EVENT_LENGTH_MIN        BLE_CONN_EVENT_LENGTH_FROM_MS(25)

/*************************************************************************************************\
 * System configuration
 */
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_NONE )
#define dg_configEMULATE_OTP_COPY               ( 0 )

#define dg_configUSE_WDOG                       ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8P )
#define dg_configFLASH_POWER_DOWN               ( 0 )
#define dg_configPOWER_1V8P_ACTIVE              ( 1 )
#define dg_configPOWER_1V8P_SLEEP               ( 1 )

#if defined(DSPS_USBD)
# undef dg_configUSE_USB_ENUMERATION
# undef dg_configUSB_DMA_SUPPORT
# undef dg_configUSE_SYS_CHARGER
# define dg_configUSE_USB_ENUMERATION            ( 1 )
# define dg_configUSB_DMA_SUPPORT                ( 1 )
# define dg_configUSE_SYS_CHARGER                ( 0 )
#endif

#define dg_configWDOG_MAX_TASKS_CNT             ( 9 )

#define dg_configUSE_SW_CURSOR                  ( 1 )

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                             /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                   ( 38000 )   /* FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )
#define dg_configNVMS_VES                       ( 1 )
#define dg_configNVPARAM_ADAPTER                ( 1 )
#define dg_configGPADC_ADAPTER                  ( 1 )
#define dg_configUSE_HW_SENSOR_NODE             ( 1 )
#define dg_configUSE_SNC_HW_GPADC               ( 1 )
#define dg_configSNC_ADAPTER                    ( 1 )
#define dg_configUSE_SYS_ADC                    ( 1 )
#if defined(DSPS_UART)
# undef dg_configUART_ADAPTER
# define dg_configUART_ADAPTER                  ( 1 )
#endif

/*************************************************************************************************\
 * BLE configuration
 */
#define dg_configBLE_PERIPHERAL                 ( 0 )
#define dg_configBLE_GATT_SERVER                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */
