/**
\addtogroup BSP
\{
\addtogroup CONFIG
\{
\addtogroup CUSTOM
\{
*/

/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for execution from RAM.
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_RETARGET

#define dg_configCRYPTO_ADAPTER                 (1)

#define dg_configUSE_HW_AES                     (1)
#define dg_configUSE_HW_HASH                    (1)
#define dg_configAES_USE_OTP_KEYS               (0)
#define dg_configAES_USE_SECURE_DMA_CHANNEL     (1)

#define dg_configUSE_LP_CLK                     LP_CLK_32768
#define dg_configEXEC_MODE                      MODE_IS_RAM
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_NONE

#define dg_configIMAGE_SETUP                    DEVELOPMENT_MODE
#define dg_configEMULATE_OTP_COPY               (0)

#define dg_configUSE_WDOG                       (0)

#define dg_configFLASH_CONNECTED_TO             (FLASH_IS_NOT_CONNECTED)

#define dg_configUSE_USB                        0
#define dg_configUSE_USB_CHARGER                0
#define dg_configALLOW_CHARGING_NOT_ENUM        1

#define dg_configUSE_ProDK                      (1)

#define dg_configUSE_SW_CURSOR                  (1)


/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    14000   /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */
#define dg_configRF_ADAPTER                     (0)
#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)

/* Include bsp default values */
#include "bsp_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */

/**
\}
\}
\}
*/
