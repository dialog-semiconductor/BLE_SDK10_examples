/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief User Configuration file for execution from RAM.
 *
 * Copyright (c) 2018-2021 Renesas Electronics Corporation and/or its affiliates
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

/* Not using an OS */
#define OS_BAREMETAL

/* Running code from internal RAM */
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_NONE
#define dg_configFLASH_CONNECTED_TO             (FLASH_IS_NOT_CONNECTED)

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

#endif /* CUSTOM_CONFIG_RAM_H_ */
