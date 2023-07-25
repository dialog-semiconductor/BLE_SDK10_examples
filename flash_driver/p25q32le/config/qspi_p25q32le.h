/**
 ****************************************************************************************
 *
 * @file qspi_p25q32le.h
 *
 * @brief Driver for flash Puya P25Q32LE
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

#ifndef _QSPI_P25Q32LE_H_
#define _QSPI_P25Q32LE_H_

/**
 * \brief The Flash vendor JEDEC ID
 *
 * This is the first byte returned by the 0x9F command
 */
#ifndef PUYA_ID
#define PUYA_ID         0x85
#endif

/**
 * \brief The Flash type JEDEC ID
 *
 * This is the second byte returned by the 0x9F command
 */
#define P25Q32LE        0x60

/**
 * \brief The Flash density JEDEC ID
 *
 * This is the third byte returned by the 0x9F command
 */
#define P25Q_32Mb_SIZE  0x16


#include "qspi_common.h"
#include "sdk_defs.h"
#include "qspi_puya.h"

static void flash_p25q32le_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk);
static uint8_t flash_p25q32le_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk);

/**
 * \brief This structs configures the system for the specific flash
 *
 * \note This struct MUST be const for this to work. Therefore, assignments must
 *       not change (must be read-only)
 */
static const qspi_flash_config_t flash_p25q32le_config = {
        .manufacturer_id                  = PUYA_ID,
        .device_type                      = P25Q32LE,
        .device_density                   = P25Q_32Mb_SIZE,
        .is_suspended                     = flash_p25q_is_suspended,
        .initialize                       = flash_p25q_initialize,
        .sys_clk_cfg                      = flash_p25q32le_sys_clock_cfg,
        .get_dummy_bytes                  = flash_p25q32le_get_dummy_bytes,
        .break_seq_size                   = HW_QSPI_BREAK_SEQ_SIZE_1B,
        .address_size                     = HW_QSPI_ADDR_SIZE_24,
        .fast_read_opcode                 = CMD_FAST_READ_QUAD,
        .page_program_opcode              = CMD_QUAD_PAGE_PROGRAM,
        .page_qpi_program_opcode          = CMD_QPI_PAGE_PROGRAM,
        .quad_page_program_address        = false,
        .erase_opcode                     = CMD_SECTOR_ERASE,
        .erase_suspend_opcode             = P25Q_ERASE_PROGRAM_SUSPEND,
        .erase_resume_opcode              = P25Q_ERASE_PROGRAM_RESUME,
        .read_erase_progress_opcode       = CMD_READ_STATUS_REGISTER,
        .erase_in_progress_bit            = FLASH_STATUS_BUSY_BIT,
        .erase_in_progress_bit_high_level = true,
        .send_once                        = 1,
        .extra_byte                       = 0xA0,
        .ucode_wakeup                     = {p25q_ucode_wakeup, sizeof(p25q_ucode_wakeup)},
        .power_down_delay                 = P25Q_POWER_DOWN_DELAY_US,
        .release_power_down_delay         = P25Q_RELEASE_POWER_DOWN_DELAY_US,
        .power_up_delay                   = P25Q_POWER_UP_DELAY_US,
        .is_ram                           = false,
        .qpi_mode                         = false,
        .enter_qpi_opcode                 = CMD_ENTER_QPI_MODE,
        .memory_size                      = MEMORY_SIZE_32Mb,
};

/**
 * \brief This is called each time the system clock is changed
 *
 * This can be used e.g. to change the qspi controller divider or modify the
 * flash dummy bytes, if e.g. the flash cannot cope with the higher clock
 * frequency.
 */
__RETAINED_CODE static void flash_p25q32le_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk)
{

}
/**
 * \brief This must return the number of dummy bytes required.
 *
 * In most cases, this can return a static value (usually 2). Sometimes,
 * however, the dummy bytes must change (e.g. according to the system clock
 * frequency).
 */
__RETAINED_CODE static uint8_t flash_p25q32le_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk)
{
        return 2;
}




#endif /* _QSPI_P25Q32LE_H_ */
/**
 * \}
 * \}
 */
