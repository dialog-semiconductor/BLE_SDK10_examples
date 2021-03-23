/**
 ****************************************************************************************
 *
 * @file i2c_task.h
 *
 * @brief I2C task
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _I2C_TASK_H_
#define _I2C_TASK_H_

#define I2C_REQUEST_SIZE        16
#define I2C_RESPONSE_SIZE       64
#define I2C_DATA_SENT_NOTIFY    (1 << 0)
#define I2C_DATA_RCVD_NOTIFY    (1 << 1)
#define I2C_DATA_RDY_NOTIFY     (1 << 2)
#define I2C_READ_REQ_NOTIFY     (1 << 3)

/* The rate at which data is template task counter is incremented. */
#define COUNTER_FREQUENCY_MS                OS_MS_2_TICKS(200)

#define I2C_ASYNC_EN (0)

/**
 * @brief I2C master task: sends requests, reads responses and prints them
 */
void i2c_master_task( void *pvParameters );

/**
 * @brief I2C slave task: reads requests, counts callbacks and sends response
 */
void i2c_slave_task( void *pvParameters );

#endif /* _I2C_TASK_H_ */
