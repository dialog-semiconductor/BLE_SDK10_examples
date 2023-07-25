/**
 * \addtogroup BSP
 * \{
 * \addtogroup SNC
 * \{
 * \addtogroup SNC_HW_SDADC
 *
 * \brief SDADC LLD for SNC context
 *
 * \{
 */
/**
 ****************************************************************************************
 *
 * @file snc_hw_sdadc.h
 *
 * @brief SNC-Definition of SDADC Low Level Driver API
 *
 * Copyright (c) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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
#ifndef SNC_HW_SDADC_H_
#define SNC_HW_SDADC_H_
#if dg_configUSE_HW_SENSOR_NODE
#if dg_configUSE_SNC_HW_SDADC
#include "sdk_defs.h"
#include "hw_sdadc.h"
#include "ad_sdadc.h"
typedef ad_sdadc_controller_conf_t snc_sdadc_source_config_t;

/**
 */
typedef const void *snc_sdadc_source_t;
#include "snc_hw_sdadc_macros.h"

#define SDADC_RET_BITMASK_VALUE      0x0000FFFF
#define SDADC_RET_BITMASK_ERRORS     0xF0000000
#define SDADC_RET_BITMASK_TIMEOUT    0x80000000      //error - timeout waiting for IRQ
#define SDADC_RET_BITMASK_LDO_NOK    0x40000000      //error - LDO found not ready after reading
#define SDADC_RET_BITMASK_SDADC_DIS  0x20000000      //error - sdadc found disabled after reading
#define SDADC_RET_BITMASK_START_SET  0x10000000      //error - start found set after reading

/*
 * FUNCTION DECLARATIONS
 *****************************************************************************************
 */
/**
 * \brief Function used in SNC context to initialize and acquire the SDADC peripheral
 *
 * \param [in] dev              (snc_sdadc_source_t: build-time-only value)
 *                              handle of SDADC source as defined in (sys_platform_devices_internal.h)
 *
 */
#define SNC_sdadc_open(dev)                                                \
        _SNC_sdadc_open(dev)
/**
 * \brief Function used in SNC context to de-initialize and release the SDADC peripheral
 *
 * \param [in] dev              (snc_sdadc_source_t: build-time-only value)
 *                              handle of SDADC source as defined in (sys_platform_devices_internal.h)
 */
#define SNC_sdadc_close(dev)                                                               \
        _SNC_sdadc_close(dev)
/**
 * \brief Function used in SNC context to get raw SDADC radio temperature value
 *
 * \param [in] dev              (snc_sdadc_source_t: build-time-only value)
 *                              handle of SDADC source as defined in (sys_platform_devices_internal.h)
 * \param [out] value           (uint32_t*: use da() or ia())
 *                              A pointer to the variable where the SDADC value will be returned
 *
 */
#define SNC_sdadc_read(dev, value)                                                        \
        _SNC_sdadc_read(dev,value)
#endif /* dg_configUSE_SNC_HW_SDADC */
#endif /* dg_configUSE_HW_SENSOR_NODE */
#endif /* SNC_HW_SDADC_H_ */
/**
 \}
 \}
 \}
 */
