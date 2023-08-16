/**
**************************************************************************************************
* @file    usbpd_peripherals.c
* @author  AMG -  Application Team
* @brief   This file provides a set of functions needed to manage the interface STUSB1602 and peripherals 
I2C for Registers , SPI DMA CRC Timers for Phy .
**************************************************************************************************
* @attention
*
* <h2><center>&copy; Copyright (c) 2019 STMicroelectronics International N.V.
* All rights reserved.</center></h2>
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted, provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. Neither the name of STMicroelectronics nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************************************
*/
#include <stdio.h>
#include "osal.h"
#include "i2c.h"
#include "usbpd_def.h"
#include "usbpd_porthandle.h"
#include "usbpd_registers.h"
#include "usbpd_peripherals.h"

extern STUSB16xx_PORT_HandleTypeDef Ports[USBPD_PORT_COUNT];

/****************************************************************************************************************
 * Defines
 ****************************************************************************************************************/

#define STUSB1602_I2C_ADDR_SINK		0x28
#define STUSB1602_I2C_ADDR_SOURCE	0x29

/****************************************************************************************************************
 * Types
 ****************************************************************************************************************/

/****************************************************************************************************************
 * Constants
 ****************************************************************************************************************/

// I2C drivers
static const ad_i2c_driver_conf_t STUSB1602_DRV_SINK = {
	I2C_DEFAULT_CLK_CFG,
	.i2c.speed			= HW_I2C_SPEED_STANDARD,
	.i2c.mode			= HW_I2C_MODE_MASTER,
	.i2c.addr_mode			= HW_I2C_ADDRESSING_7B,
	.i2c.address			= STUSB1602_I2C_ADDR_SINK,
	.dma_channel			= HW_DMA_CHANNEL_INVALID
};
static const ad_i2c_driver_conf_t STUSB1602_DRV_SOURCE = {
	I2C_DEFAULT_CLK_CFG,
	.i2c.speed			= HW_I2C_SPEED_STANDARD,
	.i2c.mode			= HW_I2C_MODE_MASTER,
	.i2c.addr_mode			= HW_I2C_ADDRESSING_7B,
	.i2c.address			= STUSB1602_I2C_ADDR_SOURCE,
	.dma_channel			= HW_DMA_CHANNEL_INVALID
};

// I2C controllers
static const ad_i2c_controller_conf_t STUSB1602_DEV_SINK = {
	.id		= HW_I2C2,
	.io		= &I2C2_IO,
	.drv	= &STUSB1602_DRV_SINK
};
static const ad_i2c_controller_conf_t STUSB1602_DEV_SOURCE = {
	.id		= HW_I2C2,
	.io		= &I2C2_IO,
	.drv	= &STUSB1602_DRV_SOURCE
};

// I2C devices
static const i2c_device I2C_USBPD_SINK = &STUSB1602_DEV_SINK;
static const i2c_device I2C_USBPD_SOURCE = &STUSB1602_DEV_SOURCE;

/****************************************************************************************************************
 * Local Data
 ****************************************************************************************************************/

/**
* @brief  Global hardware initialization
* @retval None
*/
void STUSB1602_GlobalHwInit(void)
{
	uint32_t cnt = 0;
	uint8_t PortNum;
	NVM_OK_TypeDef nvm_read;

	for (PortNum = 0; PortNum < USBPD_PORT_COUNT; PortNum++)
	{
		// check init phase complete
		cnt = 0;
		do
		{
			// NVM not ready
			nvm_read = STUSB1602_NVM_OK_Get(PortNum);
			OS_DELAY_MS(10);
			if (cnt++ == 300)
			{
				printf("usbpd (%d): nvm timeout\r\n", PortNum);
				break;
			}
		}
		while (nvm_read != NVM_OK);

		// check chip ID
		Ports[PortNum].Device_cut = STUSB1602_DEVICE_CUT_Get(PortNum);
		printf("usbpd (%d): device id %d\r\n", PortNum, Ports[PortNum].Device_cut);

		STUSB1602_VBUS_Discharge_State_Set(PortNum, VBUS_Discharge_Path_Disable);
		STUSB1602_SW_RESET_Set(PortNum, No_SW_RST);

		HAL_GPIO_Set_Txen(PortNum, false);
	}
}

/*
* @brief STUSB1602 registers reading function
* @param pBuffer      Pointer to data buffer
* @param Addr         I2C address of port controller device
* @param Reg          Address of first register to be read
* @param Size         Amount of bytes to be read
* @retval STUSB1602_StatusTypeDef Allowed values are STUSB1602_OK, STUSB1602_ERROR, STUSB1602_BUSY, STUSB1602_TIMEOUT
*/
STUSB1602_StatusTypeDef STUSB1602_ReadReg(uint8_t* pBuffer, uint8_t Addr, uint8_t Reg, uint16_t Size)
{
	if (i2c_write_read(Addr ? I2C_USBPD_SOURCE : I2C_USBPD_SINK, &Reg, 1, pBuffer, Size))
	{
		return STUSB1602_OK;
	}
	printf("usbpd (%d): i2c err readreg\r\n", Addr);
	return STUSB1602_TIMEOUT;
}


/**
* @brief STUSB1602 single register reading function
* @param Addr         I2C address of port controller device
* @param Reg          Address of register to be read
* @retval uint8_t     Register value
*/
uint8_t STUSB1602_ReadRegSingle(uint8_t Addr, uint8_t Reg)
{
	uint8_t value = 0x00;
	if (i2c_write_read(Addr ? I2C_USBPD_SOURCE : I2C_USBPD_SINK, &Reg, 1, &value, 1))
	{
		return value;
	}
	printf("usbpd (%d): i2c err readregsingle\r\n", Addr);
	return 0;
}


/**
* @brief STUSB1602 registers writing function
* @param pBuffer      Pointer to data buffer
* @param Addr         I2C address of port controller device
* @param Reg          Address of first register to be write
* @param Size         Amount of bytes to be write
* @retval STUSB1602_StatusTypeDef Allowed values are STUSB1602_OK, STUSB1602_ERROR, STUSB1602_BUSY, STUSB1602_TIMEOUT
*/
STUSB1602_StatusTypeDef STUSB1602_WriteReg(uint8_t* pBuffer, uint8_t Addr, uint8_t Reg, uint16_t Size)
{
	uint8_t buf[1 + Size];
	buf[0] = Reg;
	memcpy(&buf[1], pBuffer, Size);
	if (i2c_write(Addr ? I2C_USBPD_SOURCE : I2C_USBPD_SINK, buf, sizeof(buf)))
	{
		return STUSB1602_OK;
	}
	printf("usbpd (%d): i2c err writereg\r\n", Addr);
	return STUSB1602_TIMEOUT;
}

/**
* @brief STUSB1602 single register writing function
* @param Value        Value to write
* @param Addr         I2C address of port controller device
* @param Reg          Address of register to be write
* @retval STUSB1602_StatusTypeDef Allowed values are STUSB1602_OK, STUSB1602_ERROR, STUSB1602_BUSY, STUSB1602_TIMEOUT
*/
STUSB1602_StatusTypeDef STUSB1602_WriteRegSingle(const uint8_t Value, uint8_t Addr, uint8_t Reg)
{
	uint8_t buf[2];
	buf[0] = Reg;
	buf[1] = Value;
	if (i2c_write(Addr ? I2C_USBPD_SOURCE : I2C_USBPD_SINK, buf, sizeof(buf)))
	{
		return STUSB1602_OK;
	}
	printf("usbpd (%d): i2c err writeregsingle\r\n", Addr);
	return STUSB1602_TIMEOUT;
}
