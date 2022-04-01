/**
 ****************************************************************************************
 *
 * @file app_debug_freertos_tcb.h
 *
 * @brief ADF API
 *
 * Copyright (C) 2015-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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


