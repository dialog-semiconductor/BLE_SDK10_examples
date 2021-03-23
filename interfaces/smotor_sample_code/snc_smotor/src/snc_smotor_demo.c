/**
 ****************************************************************************************
 *
 * @file snc_smotor_demo.c
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

#include "hw_pdc.h"
#include "SeNIS.h"

#include "snc_smotor_demo.h"
#include "snc_smotor_ucodes.h"


/* uCode-Block Configuration */
uint32_t _smotor_ucode_init(ad_snc_interrupt_cb _smotor_snc_cb)
{
        uint32_t ucode_id;
        ad_snc_ucode_cfg_t cfg = { 0 };


        /* No priority. It can be used for round robin schemes */
        cfg.pdc_evt_pr = AD_SNC_PDC_EVT_PR_0;
        /* Highest priority */
        cfg.ucode_pr = AD_SNC_UCODE_PR_1;


        /* This callback will be triggered when the SNC notifies the ARM M33 */
        cfg.cb = _smotor_snc_cb;

        /* Add a PDC LUT entry so as the SNC is triggered following an RTC event */
        cfg.pdc_entry = HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                                             HW_PDC_PERIPH_TRIG_ID_RTC_TIMER,
                                             HW_PDC_MASTER_SNC,
                                             0);

        /* Register a uCode to the SNC scheduler and associate it with a PDC event */
        ucode_id = ad_snc_ucode_register(&cfg, SNC_UCODE_CTX(_ucode_smotor_set_and_trigger));

        ad_snc_ucode_enable(ucode_id); /* Important!!! */

        /* Register a PDC LUT entry so that the ARM M33 is triggered following motor events */
        uint32_t pdc_idx = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                                                                 HW_PDC_PERIPH_TRIG_ID_MOTORCTRL,
                                                                 HW_PDC_MASTER_CM33,
                                                                 0));
        OS_ASSERT(pdc_idx != HW_PDC_INVALID_LUT_INDEX);

        /* Do the trick! */
        hw_pdc_set_pending(pdc_idx);
        hw_pdc_acknowledge(pdc_idx);

        return ucode_id;
}


