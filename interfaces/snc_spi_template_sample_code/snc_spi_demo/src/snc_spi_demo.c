/**
 ****************************************************************************************
 *
 * @file snc_spi_demo.c
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

#include "platform_devices.h"
#include "custom_macros.h"

#include "snc_spi_demo.h"
#include "snc_spi_ucodes.h"


/*
 * @brief SNC initialization and uCode registration
 *
 * This function initializes the SNC as well as declares PDC event entries.
 *
 * \param[in] _snc_ucode_cb  A callback function that will be triggered when SNC notifies CM33
 *
 * \return The uCode ID of the created uCode-Block
 *
 **/
uint32_t _snc_reader_init(ad_snc_interrupt_cb _snc_ucode_cb)
{
        uint32_t ucode_id;
        ad_snc_ucode_cfg_t cfg = { 0 };

        /* Configure PDC event and uCode priorities */
        cfg.pdc_evt_pr = USER_PDC_EVENT_ENTRY_PRIORITY;
        cfg.ucode_pr   = USER_SNC_UCODE_BLOCK_PRIORITY;

        /*
         * SNC to CM33 notification callback function.
         **/
        cfg.cb = _snc_ucode_cb;


        /*
         * Declare an SNC-to-CM33 queue used for data exchange between SNC and CM33 masters.
         *
         * \note Two types of queues can be created and associated with each uCode:
         *       an SNC-to-CM33 queue for passing data from SNC towards CM33 and an
         *       CM33-to-SNC queue for passing data from CM33 towards SNC.
         */
        cfg.snc_to_cm33_queue_cfg.element_weight  = USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE;
        cfg.snc_to_cm33_queue_cfg.max_chunk_bytes = USER_SNC_TO_CM33_QUEUE_MAX_CHUNK_BYTES;
        cfg.snc_to_cm33_queue_cfg.num_of_chunks   = USER_SNC_TO_CM33_QUEUE_NUM_OF_CHUNK;

        // Check whether timestamp functionality should be enabled or not
        ((USER_SNC_TIMESTAMP_ENABLE) ? (cfg.snc_to_cm33_queue_cfg.enable_data_timestamp = true) :
                                       (cfg.snc_to_cm33_queue_cfg.enable_data_timestamp = false));

        // Check whether swapping should be enabled for data popped from the SNC-to-CM33 queue.
        ((USER_SNC_TO_CM33_QUEUE_SWAP_POPPED_DATA) ?
                                (cfg.snc_to_cm33_queue_cfg.swap_popped_data_bytes = true) :
                                (cfg.snc_to_cm33_queue_cfg.swap_popped_data_bytes = false));


        /* Here you can declare (if needed) an CM33-to-SNC queue */


#if  (USER_SNC_TRIGGER_ON_RTC_EVENT)
        /* Declare a valid PDC event entry */
        cfg.pdc_entry = HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,    /* Select the source that can trigger the Power Domain Controller (PDC).
                                                                                  This can be either externally produced signals or signals produced
                                                                                  internally by the system itself.

                                                                                  In our case study, the wakeup source is a peripheral. */

                                             HW_PDC_PERIPH_TRIG_ID_RTC_TIMER,  /* Select the id of the trigger source.

                                                                                  In our case study, this is the RTC timer module. */


                                             HW_PDC_MASTER_SNC,                /* Select which master (SNC, CM33 or M0+) should be woken up following
                                                                                  the PDC event. Valid values can be selected from the HW_PDC_MASTER
                                                                                  enum.

                                                                                  In our case study, the SNC master should be triggered to start
                                                                                  reading data from externally connected sensors/modules. */

                                             0);                               /* Select what resource(s) should be enabled by the PDC following the
                                                                                  PDC event. Valid values can be selected from HW_PDC_LUT_ENTRY_EN
                                                                                  enum. User can select one or multiple resources using the Bitwise
                                                                                  OR operator. */


#elif (USER_SNC_TRIGGER_ON_WKUP_EVENT)
        cfg.pdc_entry = HW_PDC_LUT_ENTRY_VAL(WKUP_TRIGGER_PORT, /* In our case study, the PDC trigger source is an external signal mapped on PORT0 */
                                             WKUP_TRIGGER_PIN,  /* Select which pin of the selected PORT0 can trigger PDC events */
                                             HW_PDC_MASTER_SNC,
                                             0);
#endif


        /*
         * Register and associate a uCode with a PDC event. As soon as the PDC
         * event is issued, the SNC scheduler will start executing that uCode.
         */
        ucode_id = ad_snc_ucode_register(&cfg, SNC_UCODE_CTX(_snc_reader_ucode));

        ad_snc_ucode_enable(ucode_id); /* Important!!! */

        return ucode_id;
}

