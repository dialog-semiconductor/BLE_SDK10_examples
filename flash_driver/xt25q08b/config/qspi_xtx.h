/**
 ****************************************************************************************
 *
 * @file qspi_xtx.h
 *
 * @brief QSPI flash driver for XTX flashes - common code
 *
 * Copyright (C) 2016-2021 Renesas Electronics Corporation and/or its affiliates
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
#ifndef _QSPI_XTX_H_
#define _QSPI_XTX_H_

#define XTX_ID    0x0B

#include "sdk_defs.h"
#include "qspi_common.h"

#define XTX_QUAD_IO_PAGE_PROGRAM                0x38
#define XTX_ERASE_PROGRAM_SUSPEND               0xB0
#define XTX_ERASE_PROGRAM_RESUME                0x30
#define XTX_READ_STATUS_REGISTER_1              0x35
#define XTX_WRITE_STATUS_REGISTER               0x01

#define XTX_STATUS_QE_BIT                       9 // Quad Enable Bit of Status Register( 0b_Byte1_Byte0 )
#define XTX_STATUS_QE_MASK                      (1 << XTX_STATUS_QE_BIT)

/* Suspend */
#define XTX_STATUS_REG1_SUS_BIT                 7 // Program/Erase suspend bit  of Status Register Byte1
#define XTX_STATUS_REG1_SUS_MASK                (1 << XTX_STATUS_REG1_SUS_BIT)


__STATIC_FORCEINLINE __UNUSED uint8_t flash_xtx_read_status_register_1(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { XTX_READ_STATUS_REGISTER_1 };

        flash_transact(id, cmd, 1, &status, 1);

        return status;
}

__STATIC_FORCEINLINE __UNUSED void flash_xtx_write_status_register(HW_QSPIC_ID id, uint16_t value)
{
        uint8_t cmd[] = { XTX_WRITE_STATUS_REGISTER, value, value >> 8 };

        flash_write(id, cmd, 3);

        /* Wait for the Flash to process the command */
        while (flash_is_busy(id));
}

__STATIC_FORCEINLINE void flash_xtx_enable_quad_mode(HW_QSPIC_ID id)
{
        uint16_t status;

        status = flash_read_status_register(id);
        status |= flash_xtx_read_status_register_1(id) << 8;
        if (!(status & XTX_STATUS_QE_MASK)) {
                flash_write_enable(id);
                flash_xtx_write_status_register(id, status | XTX_STATUS_QE_MASK);
        }
}

__RETAINED_CODE static bool flash_xtx_is_suspended(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;

        status = flash_xtx_read_status_register_1(id);
        return (status & XTX_STATUS_REG1_SUS_MASK) != 0;
}

#endif /* _QSPI_XTX_H_ */
/**
 * \}
 * \}
 */
