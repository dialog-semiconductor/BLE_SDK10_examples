/**
 ****************************************************************************************
 *
 * @file dsps_queue.h
 *
 * @brief DSPS queue header
 *
 * Copyright (C) 2023. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef DSPS_QUEUE_H_
#define DSPS_QUEUE_H_

#include "msg_queues.h"

typedef struct {
        msg_queue       q;
        int             low_watermark;
        int             high_watermark;
        bool            hwm_reached;
} sps_queue_t;

/**
 * \brief Create SPS queue
 *
 * \param [in] size               size of msg queue
 * \param [in] low_watermark      queue low water mark
 * \param [in] high_watermark     queue high water mark
 *
 * \return SPS queue instance
 */
sps_queue_t *sps_queue_new(int size, int low_watermark, int high_watermark);

/**
 * \brief Delete SPS queue
 *
 * \param [in] sps_queue           SPS queue instance
 */
void sps_queue_free(sps_queue_t *sps_queue);

/**
 * \brief Check the number of messages stored in a SPS queue.
 *
 * \param [in] sps_queue           SPS queue instance
 *
 * \return number of msg
 */
int sps_queue_item_count(sps_queue_t *sps_queue);

/**
 * \brief Write one msg to the SPS queue
 *
 * \param [in] sps_queue            SPS queue instance
 * \param [in] size                 size of msg
 * \param [in] data                 ptr to the data
 *
 */
void sps_queue_write_items(sps_queue_t *sps_queue, MSG_SIZE size, const uint8_t *data);

/**
 * \brief Pop one msg from the SPS queue
 *
 * \param [in] sps_queue           SPS queue instance
 * \param [in] pMsg                message instance
 *
 * \return if the process succeeds
 */
bool sps_queue_pop_items(sps_queue_t *sps_queue, msg* pMsg);

/**
 * \brief Read one msg from the SPS queue
 *
 * \param [in] sps_queue           SPS queue instance
 * \param [in] pMsg                message instance
 *
 * \return if the process succeeds
 */
bool sps_queue_read_items(sps_queue_t *sps_queue, msg* pMsg);

/**
 * \brief Check if the SPS queue is almost empty (< low water mark)
 *
 * \param [in] sps_queue           SPS queue instance
 *
 * \return true for almost empty
 */
bool sps_queue_check_almost_empty(sps_queue_t* sps_queue);

/**
 * \brief Check if the SPS queue is almost full (> low water mark)
 *
 * \param [in] sps_queue           SPS queue instance
 *
 * \return true for almost full
 */
bool sps_queue_check_almost_full(sps_queue_t* sps_queue);

#endif /* DSPS_QUEUE_H_ */
