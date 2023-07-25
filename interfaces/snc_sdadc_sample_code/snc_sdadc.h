/****************************************************************************************
 *
 * @file snc_sdadc.h
 *
 * @brief The set-up, SNC ucode and M33 task data collection
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

#ifndef SOC_SNC_ADC_H_
#define SOC_SNC_ADC_H_

#include "ad_snc.h"
#include "snc_hw_sys.h"
#include "platform_devices.h"

/**
 * \brief Function that configures the SDADC device
 *
 * \param [in] _adc_cb        a callback function that should be called
 *                              whenever the SDADC uCode-Block notifies CM33
 */
void snc_sdadc_config(ad_snc_interrupt_cb _adc_cb);

/**
 * \brief Function that dumps the SDADC measurement value or queue
 *
 */
void snc_sdadc_dump_data(void);

/**
 * \brief uCode-Block that reads measurements from the SDADC and notifies M33
 *
 */
SNC_UCODE_BLOCK_DECL(ucode_sdadc_read_notify);


#endif /* SOC_SNC_ADC_H_ */
