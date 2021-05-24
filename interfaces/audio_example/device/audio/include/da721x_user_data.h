/**
 ****************************************************************************************
 *
 * @file da721x_user_data.h
 *
 * @brief DA721X HAL driver user data definitions
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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
