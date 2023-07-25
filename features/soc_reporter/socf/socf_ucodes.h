/**
 ****************************************************************************************
 *
 * @file socf_ucodes.h
 *
 * @brief SOC function
 *
* Copyright (C) 2015-2021 Renesas Electronics Corporation and/or its affiliates
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
