/****************************************************************************************
 *
 * @file snc_sdadc.h
 *
 * @brief The set-up, SNC ucode and M33 task data collection
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
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
