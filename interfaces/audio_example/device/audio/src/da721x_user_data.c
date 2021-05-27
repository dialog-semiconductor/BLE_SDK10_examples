/**
 ****************************************************************************************
 *
 * @file da721x_user_data.c
 *
 * @brief DA721X HAL driver user data implementations
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "da721x_user_data.h"
/**
 *  I/O control interface handle.
 */
static ad_i2c_handle_t _codec_handle;
/* I2C Functions, Speed limitation: up to 3.4MHz */
int da721x_i2c_init(void)
{
        _codec_handle = ad_i2c_open(CODEC_DA721x);
        if(_codec_handle){
                return DA721X_OK;
        }
        _codec_handle = NULL;

        return -EIO;

}

void da721x_i2c_deinit(void)
{
         ad_i2c_close(_codec_handle, true);
        _codec_handle = NULL;
}

int da721x_i2c_reg_read(uint8_t reg)
{
        uint8_t val = 0;
        int err = ad_i2c_write_read(_codec_handle, &reg, 1, &val, 1, HW_I2C_F_ADD_STOP);
        if(DA721X_OK == err){
                //diag_info("Successfully read I2C:  [%x %x]\n", reg, val);
                return val;
        } else {
                diag_err("i2c read error(%d) [%x %x]\n", err, reg, val);
        }

        return -EIO;
}

int da721x_i2c_reg_write(uint8_t reg, uint8_t val)
{
        uint8_t buff[2] = {reg, val};

        int err = ad_i2c_write(_codec_handle, buff, 2, HW_I2C_F_ADD_STOP);
        if(DA721X_OK == err){
                diag_i2c("WRITE DA721%d 0x%2x 0x%02x\n", DA721X, reg, val);
                return DA721X_OK;
        } else {
                diag_err("i2c write error(%d) [%x %x]\n",
                                        err, reg, val);
        }

        return -EIO;
}

int da721x_i2c_update_bits(uint8_t reg, uint8_t mask, uint8_t bits)
{
        uint8_t val = da721x_i2c_reg_read(reg);

        if (val < 0){
                return -EIO;
        }
        val = val & ~mask;
        val |= bits;
        if (da721x_i2c_reg_write(reg, val) < DA721X_OK){
                return -EIO;
        }
        return DA721X_OK;
}

