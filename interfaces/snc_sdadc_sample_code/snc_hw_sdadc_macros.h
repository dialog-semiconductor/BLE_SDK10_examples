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
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
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
