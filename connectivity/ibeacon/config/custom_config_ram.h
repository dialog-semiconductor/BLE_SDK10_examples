/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for execution from RAM.
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
#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_USE_BLE

/*************************************************************************************************\
 * System configuration
 */
#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  ( NON_VOLATILE_IS_NONE )
#define dg_configEMULATE_OTP_COPY               ( 0 )

#define dg_configOPTIMAL_RETRAM                 ( 0 )

#define dg_configUSE_WDOG                       ( 1 )

#define __HEAP_SIZE                             ( 0x0c00 )
#define __STACK_SIZE                            ( 0x0400 )

#define dg_configFLASH_CONNECTED_TO             ( FLASH_IS_NOT_CONNECTED )

#define dg_configUSE_USB                        ( 0 )
#define dg_configUSE_USB_CHARGER                ( 0 )
#define dg_configALLOW_CHARGING_NOT_ENUM        ( 1 )



#define dg_configUSE_SW_CURSOR                  ( 1 )

//
// Enable the settings below to track OS heap usage, for profiling
//
//#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
//#define dg_configTRACK_OS_HEAP                  ( 1 )
//#else
//#define dg_configTRACK_OS_HEAP                  ( 0 )
//#endif

/*************************************************************************************************\
 * FreeRTOS configuration
 */
#define OS_FREERTOS                             /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                   ( 16828 )   /* FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripherals configuration
 */
#define dg_configFLASH_ADAPTER                  ( 0 )
#define dg_configNVMS_ADAPTER                   ( 0 )
#define dg_configNVMS_VES                       ( 0 )
#define dg_configNVPARAM_ADAPTER                ( 0 )
#define dg_configGPADC_ADAPTER                  ( 1 )
#define dg_configUSE_HW_SENSOR_NODE             ( 1 )
#define dg_configUSE_SNC_HW_GPADC               ( 1 )
#define dg_configSNC_ADAPTER                    ( 1 )


/*************************************************************************************************\
 * BLE configuration
 */
#define dg_configBLE_CENTRAL                    ( 0 )
#define dg_configBLE_GATT_CLIENT                ( 0 )
#define dg_configBLE_GATT_SERVER                ( 0 )
#define dg_configBLE_OBSERVER                   ( 0 )
#define dg_configBLE_BROADCASTER                ( 0 )
#define dg_configBLE_L2CAP_COC                  ( 0 )

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */
