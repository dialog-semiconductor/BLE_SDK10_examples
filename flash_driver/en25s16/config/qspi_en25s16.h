/**
 * \addtogroup PLA_BSP_SYSTEM
 * \{
 * \addtogroup PLA_MEMORY
 *
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file qspi_en25s16b.h
 *
 * @brief QSPI flash driver for the ESMT EN25S16B
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
#ifndef _QSPI_EN25S16B_H_
#define _QSPI_EN25S16B_H_

#ifndef ESMT_ID
#define ESMT_ID      0x1C
#endif

// Device type using command 0x9F
#define EN25S20        0x38

#ifndef S20_2Mb_SIZE
#define S20_2Mb_SIZE   0x15
#endif


#define EN25S16_READ_SUSPEND_STATUS      0x09
#define EN25S16_ERASE_PROGRAM_SUSPEND   0xB0
#define EN25S16_ERASE_PROGRAM_RESUME   0x30
#define EN25S16_ENABLE_QUAD_MODE_CMD    0x38

#define EN25S16_STATUS2_SUS_BIT            7
#define EN25S16_STATUS2_SUS_MASK           (1 << EN25S16_STATUS2_SUS_BIT)


#define EN25S16_PROGRAM_SUS_BIT          3
#define EN25S16_PROGRAM_SUS_MASK         (1 << EN25S16_PROGRAM_SUS_BIT)

#define EN25S16_ERASE_SUS_BIT            2
#define EN25S16_ERASE_SUS_MASK           (1 << EN25S16_ERASE_SUS_BIT)

#define EN25S16_POWER_DOWN_DELAY_US          3
#define EN25S16_RELEASE_POWER_DOWN_DELAY_US  3
#define EN25S16_POWER_UP_DELAY_US            10


#include "qspi_common.h"
#include "sdk_defs.h"


const uint32_t en25s16_ucode_wakeup[] = {
        0x55000025,
        0x00555555,
};

static void flash_en25s16_initialize(HW_QSPIC_ID id);
static bool flash_en25s16_is_suspended(HW_QSPIC_ID id);
static void flash_en25s16_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk);
static uint8_t flash_en25s16_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk);

static const qspi_flash_config_t flash_en25s16_config = {
        .manufacturer_id                  = ESMT_ID,
        .device_type                      = EN25S20,
        .device_density                   = S20_2Mb_SIZE,
        .is_suspended                     = flash_en25s16_is_suspended,
        .initialize                       = flash_en25s16_initialize,
        .sys_clk_cfg                      = flash_en25s16_sys_clock_cfg,
        .get_dummy_bytes                  = flash_en25s16_get_dummy_bytes,
        .break_seq_size                   = HW_QSPI_BREAK_SEQ_SIZE_1B,
        .address_size                     = HW_QSPI_ADDR_SIZE_24,
        .page_program_opcode              = CMD_QUAD_PAGE_PROGRAM,
        .page_qpi_program_opcode          = CMD_QPI_PAGE_PROGRAM,
        .quad_page_program_address        = false,
        .erase_opcode                     = CMD_SECTOR_ERASE,
        .erase_suspend_opcode             = EN25S16_ERASE_PROGRAM_SUSPEND,
        .erase_resume_opcode              = EN25S16_ERASE_PROGRAM_RESUME,
        .read_erase_progress_opcode       = CMD_READ_STATUS_REGISTER,
        .erase_in_progress_bit            = FLASH_STATUS_BUSY_BIT,
        .erase_in_progress_bit_high_level = true,
        .send_once                        = 0,
        .extra_byte                       = 0xA0,
        .ucode_wakeup                     = {en25s16_ucode_wakeup, sizeof(en25s16_ucode_wakeup)},
        .power_down_delay                 = EN25S16_POWER_DOWN_DELAY_US,
        .release_power_down_delay         = EN25S16_RELEASE_POWER_DOWN_DELAY_US,
        .power_up_delay                   = EN25S16_POWER_UP_DELAY_US,
        .is_ram                           = false,
        .qpi_mode                         = false,
        .enter_qpi_opcode                 = CMD_ENTER_QPI_MODE,
        .memory_size                      = MEMORY_SIZE_8Mb, /* 8M-bit Serial Flash 1024K -byte */
};

__STATIC_INLINE uint8_t flash_en25s16_read_suspend_status_register(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { EN25S16_READ_SUSPEND_STATUS };

        flash_transact(id, cmd, 1, &status, 1);



        return ((status) & (EN25S16_PROGRAM_SUS_MASK | EN25S16_ERASE_SUS_MASK));
}

__RETAINED_CODE static bool flash_en25s16_is_suspended(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;

        status = flash_en25s16_read_suspend_status_register(id);
        return ((status & EN25S16_STATUS2_SUS_MASK) != 0);
}

__RETAINED_CODE static void flash_en25s16_initialize(HW_QSPIC_ID id)
{
//        uint8_t cmd = EN25S_ENABLE_QUAD_MODE_CMD;
//
//        flash_write(id, &cmd, 1);
}


__RETAINED_CODE static void flash_en25s16_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk)
{

}

__RETAINED_CODE static uint8_t flash_en25s16_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk)
{
        return 2;
}

#endif /* _QSPI_EN25S20_H_ */
/**
 * \}
 * \}
 */
