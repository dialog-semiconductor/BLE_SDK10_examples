/**
 ****************************************************************************************
 *
 * @file dsps_queue.c
 *
 * @brief DSPS queue implementation
 *
 * Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "osal.h"
#include "dsps_queue.h"

#define QUEUE_WAIT_WRITE_TICKS    OS_MS_2_TICKS(100)
#define QUEUE_WAIT_READ_TICKS     OS_QUEUE_NO_WAIT

sps_queue_t *sps_queue_new(int size, int low_watermark, int high_watermark)
{
        sps_queue_t* sps_queue;

        sps_queue = (sps_queue_t *)OS_MALLOC(sizeof(sps_queue_t));

        msg_queue_create(&(sps_queue->q), size, DEFAULT_OS_ALLOCATOR);
        OS_ASSERT(NULL != (sps_queue->q.queue));

        sps_queue->high_watermark = high_watermark;
        sps_queue->low_watermark  = low_watermark;
        sps_queue->hwm_reached = false;

        return sps_queue;
}

void sps_queue_free(sps_queue_t *sps_queue)
{
        __unused msg m;

        if (sps_queue == NULL) {
                return;
        }

        /* Need to pop all the messages in the queue and release them */
        while (msg_queue_get(&sps_queue->q, &m, QUEUE_WAIT_READ_TICKS) != OS_QUEUE_EMPTY){
                msg_release(&m);
        }

        msg_queue_delete(&sps_queue->q);
        OS_FREE(sps_queue);
}

int sps_queue_item_count(sps_queue_t *sps_queue)
{
        if(sps_queue == NULL){
                return 0;
        }

        return (uxQueueMessagesWaiting(sps_queue->q.queue));
}

void sps_queue_write_items(sps_queue_t *sps_queue, MSG_SIZE size, const uint8_t *data)
{
        __unused OS_BASE_TYPE ret;

        if (sps_queue == NULL) {
                return;
        }

        ret = msg_queue_send(&sps_queue->q, 0, 0, (uint8_t*) data, size, QUEUE_WAIT_WRITE_TICKS);

        /* Queue full solution:
         * 1.Increase queue size or decrease queue high water mark
         * 2.Change serial speed and BLE throughput so that there is not great speed mismatch
         */
        OS_ASSERT(ret != OS_QUEUE_FULL);
}

bool sps_queue_pop_items(sps_queue_t *sps_queue, msg* pMsg)
{
        bool isSuccess = false;
        __unused OS_BASE_TYPE ret;

        if(sps_queue == NULL){
                return isSuccess;
        }

        ret = msg_queue_get(&sps_queue->q, pMsg, QUEUE_WAIT_READ_TICKS);
        if (ret != OS_QUEUE_EMPTY) {
                isSuccess = true;
        }

        return isSuccess;
}

bool sps_queue_read_items(sps_queue_t *sps_queue, msg* pMsg)
{
        bool isSuccess = false;
        __unused OS_BASE_TYPE ret;

        if (sps_queue == NULL) {
                return isSuccess;
        }

        ret = OS_QUEUE_PEEK(sps_queue->q.queue, pMsg, QUEUE_WAIT_READ_TICKS);
        if (ret != OS_QUEUE_EMPTY) {
                isSuccess = true;
        }

        return isSuccess;
}

bool sps_queue_check_almost_empty(sps_queue_t* sps_queue)
{
        /* Check if less than low watermark */
        if ((sps_queue_item_count(sps_queue) <= sps_queue->low_watermark) && (sps_queue->hwm_reached == true))
        {
            sps_queue->hwm_reached = false;
            return true;
        }
        return false;
}

bool sps_queue_check_almost_full(sps_queue_t* sps_queue)
{
        /* Check if high watermark exceeded */
        if ((sps_queue_item_count(sps_queue) > sps_queue->high_watermark) && (sps_queue->hwm_reached == false))
        {
            sps_queue->hwm_reached = true;
            return true;
        }
        return false;
}
