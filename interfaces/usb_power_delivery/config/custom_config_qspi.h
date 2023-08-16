/**
 ****************************************************************************************
 *
 * @file custom_config_qspi.h
 *
 * @brief Board Support Package. User Configuration file for cached QSPI mode.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_QSPI_H_
#define CUSTOM_CONFIG_QSPI_H_

#include "bsp_definitions.h"

/*************************************************************************************************\
 * Board config
 */

#define BOARD_TEST
//#define BOARD_SINK_ONLY

/*************************************************************************************************\
 * UART config
 */

#define CONFIG_RETARGET

#ifdef BOARD_TEST
// BSP Board I/O configuration disabled
#define dg_configUSE_BOARD
// Remap UART to USB pins
#define CONFIG_RETARGET_UART                    HW_UART2
#define CONFIG_RETARGET_UART_TX_PORT            HW_GPIO_PORT_0
#define CONFIG_RETARGET_UART_TX_PIN             HW_GPIO_PIN_14
#define CONFIG_RETARGET_UART_TX_MODE            HW_GPIO_MODE_OUTPUT
#define CONFIG_RETARGET_UART_TX_FUNC            HW_GPIO_FUNC_UART2_TX
#define CONFIG_RETARGET_UART_RX_PORT            HW_GPIO_PORT_0
#define CONFIG_RETARGET_UART_RX_PIN             HW_GPIO_PIN_15
#define CONFIG_RETARGET_UART_RX_MODE            HW_GPIO_MODE_INPUT
#define CONFIG_RETARGET_UART_RX_FUNC            HW_GPIO_FUNC_UART2_RX
#endif

/*************************************************************************************************\
 * Generic config
 */

// BSP Board I/O configuration disabled
#define dg_configUSE_BOARD

// Provide custom printf
#define CONFIG_CUSTOM_PRINT

#define dg_configDEFAULT_CLK                    (sysclk_PLL96)
#define dg_configUSE_LP_CLK                     LP_CLK_32768
#define dg_configEXEC_MODE                      MODE_IS_CACHED
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_FLASH

#define dg_configUSE_WDOG                       (0)

#define dg_configFLASH_CONNECTED_TO             (FLASH_CONNECTED_TO_1V8)
#define dg_configFLASH_POWER_DOWN               (0)

#define dg_configPOWER_1V8_ACTIVE               (1)
#define dg_configPOWER_1V8_SLEEP                (1)

#define dg_configUSE_SW_CURSOR                  (1)

#define dg_configENABLE_CMAC_DEBUGGER           (0)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS
#define configTOTAL_HEAP_SIZE                    262144

#define configTIMER_TASK_STACK_DEPTH            ( 512 )
#define configUSB_TASK_STACK_DEPTH            	( 512 )

// Heap size for libc malloc
#define __HEAP_SIZE				 32768

/*************************************************************************************************\
 * Peripheral specific config
 */

#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (1)
#define dg_configNVMS_ADAPTER                   (1)
#define dg_configNVMS_VES                       (0)

#define dg_configUSE_HW_I2C                     (1)
#define dg_configI2C_ADAPTER                    (1)

#define dg_configSPI_ADAPTER                    (1)
#define dg_configUSE_HW_SPI                     (1)

#define dg_configUART_ADAPTER                   (1)
#define dg_configUART_DMA_SUPPORT		(1)
#define dg_configUSE_HW_UART                    (1)

#define dg_configGPADC_ADAPTER                  (0)
#define dg_configUSE_SYS_CHARGER                (0)

/*************************************************************************************************\
 * USB specific config
 */
#define dg_configUSB_SUSPEND_MODE               USB_SUSPEND_MODE_NONE

#define dg_configUSB_DMA_SUPPORT                (1)
#define dg_configUSB_TX_DMA_EP                  (1)
#define dg_configUSB_RX_DMA_EP                  (2)

#define dg_configUSE_USB_ENUMERATION            (0)

/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_USE_BLE

#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_PERIPHERAL                 ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"

/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_QSPI_H_ */
