/**
 ****************************************************************************************
 *
 * @file socf_ucodes.h
 *
 * @brief SOC function
 *
* Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef SOCF_UCODES_H_
#define SOCF_UCODES_H_

#include "SeNIS.h"

#define SOCF_SNC_NUM_OF_SAMPLES     10

#define SOCF_SNC_GPADC_SAMPLE_SIZE      4
#define SOCF_SNC_GPADC_SAMPLES_TO_QUEUE 4
#define SOCF_SNC_GPADC_QUEUE_CHUNK_SIZE      (SOCF_SNC_GPADC_SAMPLE_SIZE * SOCF_SNC_GPADC_SAMPLES_TO_QUEUE)

/**
 * \brief uCode-Block that reads measurements from vbat adc using SNC-queue
 */
SNC_UCODE_BLOCK_DECL(ucode_socf_collect_samples_queue);

/**
 * \brief Set number of samples to collect before notifying CM33
 *
 * \param [in] notify_samples   Notifying samples number
 */
void ucode_socf_set_notify_samples(uint32_t notify_samples);

#endif /* SOCF_UCODES_H_ */
