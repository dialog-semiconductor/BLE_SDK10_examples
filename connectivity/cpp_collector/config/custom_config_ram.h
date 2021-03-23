/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for RAM mode.
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */
#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE
#undef CONFIG_USE_FTDF

/*************************************************************************************************\
 * System configuration
 */
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configEXEC_MODE                      ( MODE_IS_RAM )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_NONE )
#define dg_configEMULATE_OTP_COPY               ( 0 )

#define dg_configUSE_WDOG                       ( 1 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_CONNECTED_TO_1V8 )
#define dg_configFLASH_POWER_DOWN               ( 0 )
#define dg_configPOWER_1V8_ACTIVE               ( 1 )
#define dg_configPOWER_1V8_SLEEP                ( 1 )

#define dg_configBATTERY_TYPE                   ( BATTERY_TYPE_LIMN2O4 )
#define dg_configBATTERY_CHARGE_CURRENT         ( 2 )    // 30mA
#define dg_configBATTERY_PRECHARGE_CURRENT      ( 20 )   // 2.1mA
#define dg_configBATTERY_CHARGE_NTC             ( 1 )    // disabled

#define dg_configUSE_USB                        ( 0 )
#define dg_configUSE_USB_CHARGER                ( 0 )
#define dg_configALLOW_CHARGING_NOT_ENUM        ( 1 )

#define dg_configUSE_SW_CURSOR                  ( 1 )

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#if (dg_configDEVICE == DEVICE_DA14680)
#define configTOTAL_HEAP_SIZE                    ( 14678 )   /* FreeRTOS Total Heap Size */
#elif (dg_configDEVICE == DEVICE_DA1469x)
#define configTOTAL_HEAP_SIZE                    ( 30000 )   /* FreeRTOS Total Heap Size */
#endif

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 1 )
#define dg_configNVMS_ADAPTER                   ( 1 )
#define dg_configNVMS_VES                       ( 1 )
#define dg_configNVPARAM_ADAPTER                ( 1 )
#if (dg_configUSE_FPGA == 1)
#define dg_configGPADC_ADAPTER                  ( 0 )
#else
#define dg_configGPADC_ADAPTER                  ( 1 )
#if (dg_configDEVICE == DEVICE_DA1469x)
#define dg_configUSE_HW_SENSOR_NODE             ( 1 )
#define dg_configUSE_SNC_HW_GPADC               ( 1 )
#define dg_configSNC_ADAPTER                    ( 1 )
#endif
#endif

#define CONFIG_RETARGET
#define dg_configUART_ADAPTER                   ( 1 )
#define dg_configUART_SOFTWARE_FIFO             ( 1 )
#define dg_configUART2_SOFTWARE_FIFO_SIZE       ( 64 )
#define dg_configUSE_CLI                        ( 1 )
#define dg_configUSE_CONSOLE                    ( 1 )

/*************************************************************************************************\
 * BLE configuration
 */
#define CONFIG_USE_BLE_CLIENTS
#define CONFIG_BLE_STORAGE                      ( 1 )
#define dg_configBLE_PERIPHERAL                 ( 0 )
#define dg_configBLE_GATT_SERVER                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */
