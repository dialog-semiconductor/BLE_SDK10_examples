/**
 ****************************************************************************************
 *
 * @file custom_config_qspi.h
 *
 * @brief User Configuration file for cached QSPI mode.
 *
 * Copyright (c) 2020 Dialog Semiconductor. All rights reserved.
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

#ifndef CUSTOM_CONFIG_QSPI_H_
#define CUSTOM_CONFIG_QSPI_H_

#include "bsp_definitions.h"

/* Not using an OS */
#define OS_BAREMETAL

/* Running code from external QSPI flash via internal cache */
#define dg_configEXEC_MODE                      MODE_IS_CACHED
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_FLASH
#define dg_configFLASH_CONNECTED_TO             (FLASH_CONNECTED_TO_1V8P)
#define dg_configFLASH_POWER_DOWN               (1)
#define dg_configPOWER_1V8P_ACTIVE              (1)
#define dg_configPOWER_1V8P_SLEEP               (1)

/* Not using an OS so adapters not required, disable those enabled by default */
#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)
#define dg_configPMU_ADAPTER                    (0)
#define dg_configCRYPTO_ADAPTER                 (0)

/* Not using the radio */
#define dg_configRF_ENABLE_RECALIBRATION        (0)

/* Low power clock is set to RCX as default, using 32KHz crystal decreases code size */
#define dg_configUSE_LP_CLK                     LP_CLK_32768

/* Don't use clock manager as this reduces code size by about 6kB */
#define dg_configUSE_CLOCK_MGR                  (0)

/* Don't use brown-out functionality to reduce code size (by about 1.2kB) */
#define dg_configUSE_BOD                        (0)

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_QSPI_H_ */
