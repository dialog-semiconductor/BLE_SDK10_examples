/**
 ****************************************************************************************
 *
 * @file i2c_task.h
 *
 * @brief I2C task
 *
 * Copyright (C) 2020-2021 Renesas Electronics Corporation and/or its affiliates
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
