/**
 ****************************************************************************************
 *
 * @file snc_smotor_demo.c
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


