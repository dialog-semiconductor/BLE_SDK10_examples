/**
 ****************************************************************************************
 *
 * @file i2c.c
 *
 * @brief I2C functions
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include "osal.h"
#include "ad_i2c.h"
#include "periph_setup.h"
#include "i2c.h"

/****************************************************************************************************************
 * Defines
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Types
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Constants
 ****************************************************************************************************************/

// I2C2 I/O configuration
const ad_i2c_io_conf_t I2C2_IO = {
	.scl = {
		.port = PD_MAIN_SCL_PORT,
		.pin = PD_MAIN_SCL_PIN,
		.on = { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C2_SCL, false },
		.off = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true }
	},
	.sda = {
		.port = PD_MAIN_SDA_PORT,
		.pin = PD_MAIN_SDA_PIN,
		.on =  { HW_GPIO_MODE_OUTPUT_OPEN_DRAIN, HW_GPIO_FUNC_I2C2_SDA, false },
		.off = { HW_GPIO_MODE_INPUT, HW_GPIO_FUNC_GPIO, true }
	},
	.voltage_level = PD_MAIN_GPIO_POWER
};

/****************************************************************************************************************
 * Local Data
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Local functions
 ****************************************************************************************************************/

/****************************************************************************************************************
 * External functions
 ****************************************************************************************************************/

void i2c_init()
{
	ad_i2c_io_config(HW_I2C2, &I2C2_IO, AD_IO_CONF_ON);
}

bool i2c_read(i2c_device dev, uint8_t* buff, uint32_t size)
{
	bool res = true;
	ad_i2c_handle_t dev_hdr = ad_i2c_open(dev);

	// perform read
	int err = ad_i2c_read(dev_hdr, buff, size, HW_I2C_F_ADD_STOP);
	if (err == AD_I2C_ERROR_NONE)
	{
		printf("I2C r error: %d\n\r", err);
		res = false;
	}
	ad_i2c_close(dev_hdr, true);
	return res;
}

bool i2c_write(i2c_device dev, uint8_t* buff, uint32_t size)
{
	bool res = true;
	ad_i2c_handle_t dev_hdr = ad_i2c_open(dev);

	// perform write
	int err = ad_i2c_write(dev_hdr, (const uint8_t*) buff, size, HW_I2C_F_ADD_STOP);

	if (err != AD_I2C_ERROR_NONE)
	{
		// handle error
		printf("I2C w error: %d\n\r", err);
		res = false;
	}
	ad_i2c_close(dev_hdr, true);
	return res;
}

bool i2c_write_read(i2c_device dev, uint8_t* wbuff, uint32_t wbuff_size, uint8_t* rbuff, uint32_t rbuff_size)
{
	bool res = true;
	ad_i2c_handle_t dev_hdr = ad_i2c_open(dev);

	// perform write and read
	int err = ad_i2c_write_read(dev_hdr, (const uint8_t*) wbuff, wbuff_size, rbuff, rbuff_size, HW_I2C_F_ADD_STOP);
	if (err != AD_I2C_ERROR_NONE)
	{
		printf("I2C wr error: %d\n\r", err);
		res = false;
	}
	ad_i2c_close(dev_hdr, true);
	return res;
}
