/**
 ****************************************************************************************
 *
 * @file custom_macros.h
 *
 * @brief Custom macros header file.
 *
 * Copyright (C) 2017-2021 Renesas Electronics Corporation and/or its affiliates
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
#ifndef CONFIG_CUSTOM_MACROS_H_
#define CONFIG_CUSTOM_MACROS_H_


/*
 * @brief Enable/disable debugging aid
 *
 * This mechanism aims in facilitating the developer to validate the correct behavior
 * of the application. Instead of collecting data from a serial interface (e.g. SPI),
 * this mechanism creates fake data, starting from zero value and incrementing by one.
 *
 * \warning: This macro should be set to zero when performing actual operations.
 *
 */
#define USER_BDG_ENABLE                       (1)


/*
 * @brief Set RTC timer events to trigger the PDC
 *
 *
 * Set this macro to '1' to trigger PDC events following RTC timer events.
 * Upon triggering the PDC, SNC will wake up and start reading data via
 * the SPI interface.
 *
 * \warning Either USER_SNC_TRIGGER_ON_RTC_EVENT or USER_SNC_TRIGGER_ON_WKUP_EVENT
 *          macro can be set at a time.
 */
#define USER_SNC_TRIGGER_ON_RTC_EVENT         (1)


/*
 * @brief Set external events to trigger the PDC
 *
 * Set this macro to '1' to trigger PDC events following an external event
 * (e.g. the press of a button). Upon triggering the PDC, SNC will wake
 * up and start reading data via the SPI interface.
 *
 * \warning Either USER_SNC_TRIGGER_ON_RTC_EVENT or USER_SNC_TRIGGER_ON_WKUP_EVENT
 *          macro can be set at a time.
 */
#define USER_SNC_TRIGGER_ON_WKUP_EVENT        (0)



/*
 * @brief Enable/disable timestamp functionality
 *
 * Set this macro to '1' to enable timestamp functionality for the data acquired via SNC.
 * When PDC is triggered by external events the input capture functionality of TIMER1 is
 * used for acquiring timestamps. When PDC is triggered by RTC events, timestamps are
 * produced by the RTC time counter.
 */
#define USER_SNC_TIMESTAMP_ENABLE             (1)



/*
 * @brief Set the RTC trigger time interval
 *
 *
 * This macro is used to declare the RTC timer event interval. An RTC event
 * will be triggered every (USER_RTC_TRIGGER_INTERVAL * 10) ms.
 *
 * \note This macro has a meaning only when USER_SNC_TRIGGER_ON_RTC_EVENT macro is set.
 *
 * \warning The provided RTC interval value should be multiple of 10. Otherwise, an
 *          assertion will hit. This is because the granularity of the RTC timer is
 *          10ms.
 */
#define USER_RTC_TRIGGER_INTERVAL             100UL



/*
 * Use this macro to declare a priority level for the PDC event entry.
 *
 * \note Valid values can be selected from AD_SNC_PDC_EVT_PRIORITY enum.
 */
#define USER_PDC_EVENT_ENTRY_PRIORITY         AD_SNC_PDC_EVT_PR_0



/*
 * Use this macro to declare a priority level for the uCode-Block.
 *
 * \note Valid values can be selected from AD_SNC_UCODE_PRIORITY enum.
 */
#define USER_SNC_UCODE_BLOCK_PRIORITY         AD_SNC_UCODE_PR_1



/*
 * Use this macro to declare the max number of retries that should be performed
 * in case the SNC-to-CM33 queue is full.
 *
 * \note The more the retries he bigger the latency introduced in the code.
 */
#define USER_UCODE_FIFO_FULL_CHECK_RETRIES   5



/*
 * @brief Select the number of uCode-Block iterations before notifying the ARM M33
 *        master to wakeup.
 *
 * As soon as the requested number has been reached, SNC will notify CM33 to further process
 * the already acquired data and thus, offloading the SNC-to-CM33 queue. This queue will be
 * used for storing all the data samples acquired by SNC and it is the main mechanism for
 * exchanging data between SNC and CM33 masters.
 */
#define USER_UCODE_MAX_NUM_OF_ITERATION      3



/*
 * @brief Set the number of chunks from which the SNC-to-CM33 queue consists of.
 *
 * An SNC queue is characterized by the number of chunks that it can hold. Use
 * this macro to declare the number of chunks of the SNC-to-CM33 queue.
 *
 *
 *\note It is recommended to set this value, at least, equal to USER_UCODE_MAX_NUM_OF_ITERATION.
 *      macro. Thus, ensuring the SNC-to-CM33 queue has enough space to accommodate all the
 *      data samples before notifying the CM33 master to start offloading the queue and
 *      processing the data.
 *
 * \warning Minimum allowable value is 2, otherwise an assertion is hit.
 *
 *
   ------------------------------------- SNC queue -------------------------------------
   -------------------------------------------------------------------------------------
   -                          -                           -                            -
   -         Chunk1           -       Chunk2              -       Chunkn               -
   -                          -                           -                            -
   -   [Data samples stored   -   [Data samples stored    -   [Data samples stored     -
   -   at first uCode-Block   -   at second uCode-Block   -   at n uCode-Block         -
   -   execution]             -   execution]              -   execution]               -
   -                          -                           -                            -
   -------------------------------------------------------------------------------------
*/
#define USER_SNC_TO_CM33_QUEUE_NUM_OF_CHUNK   USER_UCODE_MAX_NUM_OF_ITERATION



/*
 * @brief Set the maximum number of bytes a chunk structure can store.
 *
 *
 * A chunk is a logical unit that can hold a number of elements (32-bit). Use this
 * macro to declare the number of max number of bytes a chunk can hold.
 *
 *
 * \note  It is recommended to set this value, at least, equal to the max number of bytes that can
 *        be read by SNC during a uCode execution. For instance, if SNC reads 5 data samples, each
 *        16-bit in length, then set this value to (5 data * 2 bytes = 10 bytes).
 *
 *
 * \warning  Minimum allowable value is 2. Otherwise an assertion is hit.
 *
 */
#define USER_SNC_TO_CM33_QUEUE_MAX_CHUNK_BYTES    10


/*
 * @brief Define the weight of an element stored in the SNC queue
 *
 * Each element of a chunk has a weight, either 1 byte, 2 bytes or 4 bytes.
 *
 *
 * \note    It is recommended to set this value equal to the length of data samples
 *          read by SNC. For instance, if SNC reads 16-bit data samples, set this
 *          macro to SNC_QUEUE_ELEMENT_SIZE_HWORD value.
 *
 * \note    Valid values can be selected from SNC_QUEUE_ELEMENT_SIZE enum.
 *
 */

/*
 *   Given that 4 bytes are pushed in the queue, this is how bytes are aligned based on the element weight:
 *
 *   %%%%%% Element weight byte %%%%%%
     +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
     |       *       *       *       |       *       *       *       |       *       *       *       |       *       *       *       |
     | byte0 *       *       *       | byte1 *       *       *       | byte2 *       *       *       | byte3 *       *       *       |
     |       *       *       *       |       *       *       *       |       *       *       *       |       *       *       *       |
     +-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+-------+

     %%%%%% Element weight half word %%%%%%

     +-------+-------+-------+-------+-------+-------+-------+-------+
     |       *       *       *       |       *       *       *       |
     | byte0 * byte1 *       *       | byte2 * byte3 *       *       |
     |       *       *       *       |       *       *       *       |
     +-------+-------+-------+-------+-------+-------+-------+-------+

     %%%%%% Element weight word %%%%%%

     +-------+-------+-------+-------+
     |       *       *       *       |
     | byte0 * byte1 * byte2 * byte3 |
     |       *       *       *       |
     +-------+-------+-------+-------+
 */
#define USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE   SNC_QUEUE_ELEMENT_SIZE_HWORD



/*
 * @ Enable/disable swapping functionality for data popped from an SNC-to-CM33 queue.
 *
 * If set, it changes the endianness of data being popped/read from the SNC-to-CM33 queue.
 * This mechanism is useful when working with the SPI interface. In specific, when SNC is
 * accessing the SPI bus, data are being read and stored in the chunk in big-endian mode.
 * To convert data to little-endian mode, when CM33 is popping samples from the queue, data
 * swapping functionality should be enabled.
 *
 * \note Data swapping can be used for half-word and word element sizes.
 *
 */
#define USER_SNC_TO_CM33_QUEUE_SWAP_POPPED_DATA       (0)


/* Error messages declarations */

#if ( (USER_SNC_TRIGGER_ON_RTC_EVENT) && (USER_SNC_TRIGGER_ON_WKUP_EVENT) )
#error "Select one source to trigger the PDC!!!"
#endif


#if (USER_SNC_TO_CM33_QUEUE_NUM_OF_CHUNK < USER_UCODE_MAX_NUM_OF_ITERATION)
#warning "The SNC queue has inadequate space to store all data samples from all uCode iterations!!!"
#endif


#if (USER_SNC_TO_CM33_QUEUE_MAX_CHUNK_BYTES < USER_SNC_TO_CM33_QUEUE_ELEMENT_SIZE)
#error "Chunk size has inadequate space to store the declared weight element!!!"
#endif


#endif /* CONFIG_CUSTOM_MACROS_H_ */
