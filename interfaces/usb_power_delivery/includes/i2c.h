/**
 ****************************************************************************************
 *
 * @file i2c.h
 *
 * @brief I2C header
 *
 ****************************************************************************************
 */
#ifndef I2C_H
#define I2C_H

#include "ad_i2c.h"

extern const ad_i2c_io_conf_t I2C2_IO;

/**
 * \brief I2C device handle
 */
typedef const ad_i2c_controller_conf_t* i2c_device;

void i2c_init(void);
bool i2c_read(i2c_device dev, uint8_t* buff, uint32_t size);
bool i2c_write(i2c_device dev, uint8_t* buff, uint32_t size);
bool i2c_write_read(i2c_device dev, uint8_t* wbuff, uint32_t wbuff_size, uint8_t* rbuff, uint32_t rbuff_size);

#endif //I2C_H
