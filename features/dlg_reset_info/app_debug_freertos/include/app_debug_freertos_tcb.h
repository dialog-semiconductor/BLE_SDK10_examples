/**
 ****************************************************************************************
 *
 * @file app_debug_freertos_tcb.h
 *
 * @brief ADF API
 *
 * Copyright (C) 2015-2022 Renesas Electronics Corporation and/or its affiliates
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

/*
 *      Terminology:
 *              ADF: Application Debug FreeRTOS
 *
 *
 *      Usage:
 *              In custom_config_xxx.h, and defining the appropriate Macros. This enables additional 'adf' data
 *              to be captured and reported after a reset. The data can be serialized and then reported back via any
 *              application means.
 *
 *              * #define dg_configENABLE_ADF                             (1)
 *              * #define dg_configFREERTOS_ENABLE_THREAD_AWARENESS       (1)
 *              * #define dg_configRETAINED_UNINIT_SECTION_SIZE           (ADDTL_UNINIT)
 *
 * */

#ifndef __APP_DEBUG_FREERTOS_TCB_H__
#define __APP_DEBUG_FREERTOS_TCB_H__


#include <stdbool.h>
#include <stdint.h>


/**
 * \brief Initialize ADF Component
 *
 * \ Note: This should be called prior to any tasks that are created, that are desired to be tracked
 *
 */
void adf_start_tracking_init(void);

/**
 * \brief Get the ADF Serialized size for malloc-in (OS_MALLOC in SDK) or allocating a static buffer
 *
 *
 * \return: sizeof entire ADF packet.  If only the header is present (size 4), no resets have been detected.
 *
 */

uint16_t adf_get_serialized_size(void);

/**
 * \brief Get Serialized Reset Data
 * Must be used in conjunction with adf_get_serialized_size to provide the input.
 *
 * \param [out] data pointer to where reset data should be stored
 * \param [out] len   length return calculated by this function, could be checked against input.
 * \param [in] serial_len   length provided by adf_get_serialized_size, so redunantant call is not needed
 *
 *
 * \sa uint16_t adf_length = adf_get_serialized_size();
 *      uint8_t *reset_data = OS_MALLOC(adf_length);
 *      adf_get_serialized_reset_data(reset_data, &adf_actual_len, adf_length);
 *
 */

void adf_get_serialized_reset_data(uint8_t *data, uint16_t *len, uint16_t serial_len);

#ifdef ADF_PRINTF

/**
 * \brief Print verbose serialized data
 *
 * \param [in] data pointer to data returned by adf_get_serialized_reset_data(...)
 * \param [in] len   length of data
 *
 */
void adf_print_verbose(uint8_t *data, uint16_t len);
#endif



#endif //__APP_DEBUG_FREERTOS_TCB_H__


