/**
 ****************************************************************************************
 *
 * @file da721x_user_data.h
 *
 * @brief DA721X HAL driver user data definitions
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

#ifndef _DA721X_USER_DATA_H
#define _DA721X_USER_DATA_H

#include <stdio.h>
#include <osal.h>
#include <hw_clk.h>
#include <ad_i2c.h>

#include "platform_devices.h"

/* Delay Macro Definition */
#define DA721X_SET_DELAY

/* Debug/Troubleshooting  */
#define DA721X_DEBUG

#ifdef DA721X_DEBUG
#define diag_info printf
#define diag_i2c printf
#define diag_err printf
#else
#define diag_info
#define diag_i2c
#define diag_err
#endif //DA721X_DEBUG

/* Delay macros -
     have to be replaced to the code platform provide */
#ifdef DA721X_SET_DELAY
#define mdelay(v)       OS_DELAY_MS((v))
#define udelay(v)       hw_clk_delay_usec((v))
#else
#define mdelay(v)
#define udelay(v)
#endif//DA721X_SET_DELAY

#define DA721X_OK       (0) //success is zero
#define EINVAL          (1)
#define EIO             (2)
/**
 *  I/O control interface functions.
 */

/** I2C Functions, Speed limitation: up to 3.4MHz */

/**
 * @brief initialize I2C interface
 * @return OK or error
 */
int da721x_i2c_init(void);

/**
 * @brief de-initialize I2C inteface, releasing resources
 */
void da721x_i2c_deinit(void);

/**
 * @brief read a byte from an I2C register address
 * @param register address to read
 * @return read value, or error
 */
int da721x_i2c_reg_read(uint8_t reg);

/**
 * @brief write a byte to an I2C register address
 * @param register address to write to
 * @param value to write
 * @return OK or error
 */
int da721x_i2c_reg_write(uint8_t reg, uint8_t val);

/**
 * @brief update specific bits on an I2C register address
 * @param register address to write to
 * @param bits to mask out
 * @param bits to update
 * @return OK or error
 */
int da721x_i2c_update_bits(uint8_t reg, uint8_t mask, uint8_t bits);


#endif /* _DA721X_USER_DATA_H */
