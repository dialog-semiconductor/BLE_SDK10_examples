/**
 ****************************************************************************************
 *
 * @file spi_task.h
 *
 * @brief SPI task
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _SPI_TASK_H_
#define _SPI_TASK_H_

/* The rate at which data is template task counter is incremented. */
#define COUNTER_FREQUENCY_MS                OS_MS_2_TICKS(200)

/**
 * @brief SPI master task: sends requests, reads responses and prints them
 */
void spi_producer_task( void *pvParameters );

/**
 * @brief HW SPI slave task: reads requests, counts callbacks and sends response
 */
void spi_consumer_task( void *pvParameters );

#endif /* _SPI_TASK_H_ */
