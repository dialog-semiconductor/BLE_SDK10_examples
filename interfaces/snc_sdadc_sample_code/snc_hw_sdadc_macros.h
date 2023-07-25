/**
 * \addtogroup BSP
 * \{
 * \addtogroup SNC
 * \{
 * \addtogroup SNC_HW_SDADC
 *
 * \brief SDADC LLD macros for SNC context
 *
 * \{
 */
/**
 ****************************************************************************************
 *
 * @file snc_hw_sdadc_macros.h
 *
 * @brief SNC definitions of Sigma/Delta ADC Low Level Driver Macros
 *
 * Copyright (C) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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
#ifndef SNC_HW_SDADC_MACROS_H_
#define SNC_HW_SDADC_MACROS_H_
#if dg_configUSE_HW_SENSOR_NODE
#if dg_configUSE_SNC_HW_SDADC

//==================== Peripheral Acquisition functions ========================
void snc_sdadc_open(b_ctx_t* b_ctx, const snc_sdadc_source_config_t * conf);
#define _SNC_sdadc_open(conf)                                                            \
        SNC_STEP_BY_STEP();                                                             \
        snc_sdadc_open(b_ctx, _SNC_OP_VALUE(const snc_sdadc_source_config_t * , conf))
void snc_sdadc_close(b_ctx_t* b_ctx, const snc_sdadc_source_config_t * conf);
#define _SNC_sdadc_close(conf)                                                           \
        SNC_STEP_BY_STEP();                                                             \
        snc_sdadc_close(b_ctx, _SNC_OP_VALUE(const snc_sdadc_source_config_t * , conf))
void snc_sdadc_read(b_ctx_t* b_ctx, const snc_sdadc_source_config_t * conf,
                        SENIS_OPER_TYPE value_type, uint32_t* value);
#define _SNC_sdadc_read(conf, value)                                                     \
        SNC_STEP_BY_STEP();                                                             \
        snc_sdadc_read(b_ctx, _SNC_OP_VALUE(const snc_sdadc_source_config_t * , conf),             \
                        _SNC_OP_ADDRESS(value))

#endif /* dg_configUSE_SNC_HW_SDADC */
#endif /* dg_configUSE_HW_SENSOR_NODE */
#endif /* SNC_HW_SDADC_MACROS_H_ */
/**
 \}
 \}
 \}
 */
