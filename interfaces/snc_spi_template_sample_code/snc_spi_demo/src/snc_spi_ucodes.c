/**
 ****************************************************************************************
 *
 * @file snc_spi_ucodes.c
 *
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

#include <string.h>

#include "platform_devices.h"
#include "peripheral_setup.h"

#include "SeNIS.h"
#include "snc_hw_sys.h"
#include "snc_hw_gpio.h"
#include "snc_queues.h"

#include "custom_macros.h"
#include "snc_spi_ucodes.h"


/*
 * Warning: The SNC works only with 32-bit aligned data and addresses!!!!!!!
 */
_SNC_RETAINED static uint32_t timestamp;
_SNC_RETAINED static uint32_t queue_is_full_cnt = 0;
_SNC_RETAINED static uint32_t queue_is_full;
_SNC_RETAINED static uint32_t queue_max_chunk;
_SNC_RETAINED static uint32_t read_len;
_SNC_RETAINED static uint32_t timestampAcquired;
_SNC_RETAINED static uint32_t *writeQp;
_SNC_RETAINED static uint32_t num_of_iterations = 0;


#if (USER_BDG_ENABLE)
/* Array used for holding the emulated data */
_SNC_RETAINED static uint32_t emulated_data[(USER_SNC_TO_CM33_QUEUE_MAX_CHUNK_BYTES/USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE)] = { 0 };

_SNC_RETAINED static uint32_t *emulated_data_ptr = &emulated_data[0];

_SNC_RETAINED static uint32_t idx = 0;
#endif


/* uCode-Block declaration */
SNC_UCODE_BLOCK_DEF(_snc_reader_ucode)
{

#if (USER_SNC_TRIGGER_ON_WKUP_EVENT && !USER_SNC_TRIGGER_ON_RTC_EVENT)
        /* Clear the WKUP interrupt latch status */
        SNC_hw_sys_clear_wkup_status(WKUP_TRIGGER_PORT, (1 << WKUP_TRIGGER_PIN));
#elif (USER_SNC_TRIGGER_ON_RTC_EVENT && !USER_SNC_TRIGGER_ON_WKUP_EVENT)
        /* Clear the PDC event triggered by RTC */
        SNC_hw_sys_clear_rtc_pdc_event();
#endif

        /* Check whether timestamp functionality is enabled */
        SENIS_assign(da(&timestampAcquired), USER_SNC_TIMESTAMP_ENABLE);

        /* Reset the counter value used to check whether the SNC queue is full */
        SENIS_assign(da(&queue_is_full_cnt), 0);


        /* Check whether the SNC-to-CM33 queue is full for a predefined number of iterations */
        SENIS_while (da(&queue_is_full_cnt), NEQ, USER_UCODE_FIFO_FULL_CHECK_RETRIES) {
                /*
                 * Check whether the queue is full. The returned value is '1' in case
                 * the queue is full, '0' otherwise.
                 */
                SNC_queues_snc_wq_is_full(da(&queue_is_full));
                SENIS_if (da(&queue_is_full)) {
                        SENIS_inc1(da(&queue_is_full_cnt)); // If the queue is full increment the counter by one
                SENIS_else {
                        SENIS_break; // If the queue is not full exit from the loop and move forward
                }}
        }


        /*
         * If the max number of checks for full queue is reached, perform an error handling
         * operation e.g. flush the FIFO of the externally connected sensor.
         */
        SENIS_if (da(&queue_is_full_cnt), EQ, USER_UCODE_FIFO_FULL_CHECK_RETRIES) {

                /*
                   ##################### TAG: SNC_QUEUE_IS_FULL #########################
                   ########### User configurations should be placed here ################
                   ######################################################################

                   At this point, you should declare all the steps required for handling
                   the scenario where the queue is full and no further data can be pushed
                   into the queue.
                */


                /* Notify the CM33 that it should offload the queue */
                SNC_CM33_NOTIFY();

                /* Exit from the uCode */
                SENIS_return;
         }


        /*
           ###################### TAG: SNC_SPI_OPERATIONS #######################
           ############# User configurations should be placed here ##############
           ######################################################################

           At this point you should declare the number of bytes that should
           be read by SNC.

           For instance: SENIS_assign(da(&read_len), 10);
        */

#if (USER_BDG_ENABLE)
           SENIS_assign(da(&read_len), USER_SNC_TO_CM33_QUEUE_MAX_CHUNK_BYTES);
#endif

        /*
         * If the requested data bytes is greater than the declared max
         * chunk size, then read only the max allowed bytes.
         */
        SNC_queues_snc_get_wq_max_chunk_bytes(da(&queue_max_chunk));
        SENIS_if (da(&read_len), GT, da(&queue_max_chunk)) {
                SENIS_assign(da(&read_len), da(&queue_max_chunk));
         }


        /* Get a timestamp value (if enabled) that will accompany the acquired data */
        SENIS_if (da(&timestampAcquired)) {
                SENIS_assign(da(&timestampAcquired), 0);

#if (USER_SNC_TRIGGER_ON_WKUP_EVENT && !USER_SNC_TRIGGER_ON_RTC_EVENT)
                /* Acquire a timestamp value from a HW timer */
                SNC_hw_sys_timer_get_capture1(HW_TIMER, da(&timestamp));
#elif (USER_SNC_TRIGGER_ON_RTC_EVENT && !USER_SNC_TRIGGER_ON_WKUP_EVENT)
                /* Acquire a timestamp value from the RTC time counter */
                SNC_hw_sys_rtc_get_time_bcd(da(&timestamp));
#endif
        SENIS_else {
                SENIS_assign(da(&timestamp), 0xFFFFFFFF);
        }}


        /*
         * Get a write pointer used to push data into the SNC-to-CM33 queue.
         * The user should provide the number of bytes that are going to be
         * pushed into the queue as well as a timestamp.
         */
         SNC_queues_snc_get_wq(da(&writeQp), da(&read_len), da(&timestamp));



        /*
           ######################## TAG: SNC_SPI_OPERATIONS #####################
           ############# User configurations should be placed here ##############
           ######################################################################

           At this point you should perform your SPI transactions. For instance:

           // Data to be sent
           uint32_t write_data[] = { 0x05, 0x06 };
           uint32_t* write_data_ptr = &write_data[0];

           // Initialize the SPI bus
           SNC_spi_open((snc_spi_controller_conf_t *)SPI_CUSTOM_DEVICE);


           // Assert the chip select line
           SNC_spi_activate_cs((snc_spi_controller_conf_t *)SPI_CUSTOM_DEVICE);


           // Perform a write SPI transaction
           SNC_spi_write((snc_spi_controller_conf_t *)SPI_CUSTOM_DEVICE, ia(&write_data_ptr), sizeof(write_data)/sizeof(uint32_t);

           // Perform a read SPI transaction
           SNC_spi_read((snc_spi_controller_conf_t *)SPI_CUSTOM_DEVICE, ia(&writeQp), da(&read_len));


           // De-assert the chip select line
           SNC_spi_deactivate_cs((snc_spi_controller_conf_t *)SPI_CUSTOM_DEVICE);


           // De-initialize the SPI bus
           SNC_spi_close((snc_spi_controller_conf_t *)SPI_CUSTOM_DEVICE);

        */


#if (USER_BDG_ENABLE)
         /* Create the emulated data */
         SENIS_while(da(&idx), NEQ, sizeof(emulated_data)/sizeof(uint32_t)) {

                 /* Write a new value in the array used for storing the emulated data */
                 SENIS_assign(ia(&emulated_data_ptr), da(&idx));

                 /*
                  * Increment the pointer so that to point the next element in the array
                  * used for storing the emulated data.
                  */
                 SENIS_inc4(da(&emulated_data_ptr));

                 /* Increment index */
                 SENIS_inc1(da(&idx));
         }


        /* Copy the emulated data into the queue */
        SENIS_copy(ia(&writeQp), da(emulated_data),  sizeof(emulated_data)/sizeof(uint32_t));

        /* Reset the index */
        SENIS_assign(da(&idx), 0);

        /* Reset the array pointer */
        SENIS_assign(da(&emulated_data_ptr), emulated_data);
#endif


        /*
         * This function will perform the actual push operation of the
         * recently acquired data into the SNC queue.
         */
        SNC_queues_snc_push();


        /* Increment the counter value used for checking the number of uCode executions */
        SENIS_inc1(da(&num_of_iterations));


        /* Check whether the requested number of uCode iterations has been reached */
        SENIS_if (da(&num_of_iterations), EQ, USER_UCODE_MAX_NUM_OF_ITERATION) {

                /* Reset the counter value used for checking the uCode iterations */
                SENIS_assign(da(&num_of_iterations), 0);

                /*
                 * Send a notification to the CM33 master, designating
                 * that CM33 should start processing the data samples.
                 */
                SNC_CM33_NOTIFY();
        }
}


