/**
 ****************************************************************************************
 *
 * @file qspi_is25wp032d.h
 *
 * @brief QSPI flash driver for the ISSI IS25WP032D
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
#ifndef _QSPI_IS25WP032D_H_
#define _QSPI_IS25WP032D_H_

#ifndef IS25LP_IS25WP_SERIES
#define IS25LP_IS25WP_SERIES  0x70
#endif

#define IS25WP032D_SIZE      0x16

#include "qspi_common.h"
#include "sdk_defs.h"

#include "qspi_issi.h"

// Flash power up/down timings
#define IS25WP032D_POWER_DOWN_DELAY_US          3

#define IS25WP032D_RELEASE_POWER_DOWN_DELAY_US  5

#define IS25WP032D_POWER_UP_DELAY_US            300

#if (dg_configFLASH_POWER_OFF == 1)
/**
 * \brief uCode for handling the QSPI FLASH activation from power off.
 */
        /*
         * Delay 3000usec
         * 0x01   // CMD_NBYTES = 0, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         * 0x80   // CMD_WT_CNT_LS = 0x80 --> 3000000 / 62.5 = 48000 // 3000usec
         * 0xBB   // CMD_WT_CNT_MS = 0xBB
         * Exit from Fast Read mode
         * 0x11   // CMD_NBYTES = 2, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         * 0x00   // CMD_WT_CNT_LS = 0
         * 0x00   // CMD_WT_CNT_MS = 0
         * 0xFF   // Enable Reset
         * 0xFF   // Enable Reset
         * (up to 16 words)
         */
        const uint32_t is25wp032d_ucode_wakeup[] = {
                0x11000001 | (((uint16_t)(is25wp032d_POWER_UP_DELAY_US*1000/62.5) & 0xFFFF) << 8),
                0xFFFF0000,
        };
#elif (dg_configFLASH_POWER_DOWN == 1)
/**
 * \brief uCode for handling the QSPI FLASH release from power-down.
 */
        /*
         * 0x09   // CMD_NBYTES = 1, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         * 0xD0   // CMD_WT_CNT_LS = 0xD0 --> 45000 / 62.5 = 720   // 45usec for worst case (MX25R3235F)
         * 0x02   // CMD_WT_CNT_MS = 0x02
         * 0xAB   // Release Power Down
         * (up to 16 words)
         */
        const uint32_t is25wp032d_ucode_wakeup[] = {
                0xAB000009 | (((uint16_t)(IS25WP032D_RELEASE_POWER_DOWN_DELAY_US*1000/62.5) & 0xFFFF) << 8),
        };
#else
/**
 * \brief uCode for handling the QSPI FLASH exit from the "Continuous Read Mode".
 */
        /*
         * 0x45   // CMD_NBYTES = 8, CMD_TX_MD = 2 (Quad), CMD_VALID = 1
         * 0x00   // CMD_WT_CNT_LS = 0
         * 0x00   // CMD_WT_CNT_MS = 0
         * 0xFF
         * 0xFF
         * 0xFF
         * 0xFF
         * 0xFF
         * 0xFF
         * 0xFF
         * 0xFF
         */
        const uint32_t is25wp032d_ucode_wakeup[] = {
                0xFF000025,
                0x00FFFFFF,
        };

        const uint32_t is25wp032d_ucode_wakeup_32bit_addressing[] = {
                0xFF000045,
                0xFFFFFFFF,
                0x00FFFFFF,
        };

#endif

static void flash_is25wp032d_initialize(HW_QSPIC_ID id);
static void flash_is25wp032d_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk);
static uint8_t flash_is25wp032d_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk);

static const qspi_flash_config_t flash_is25wp032d_config = {
        .manufacturer_id               = ISSI_ID,
        .device_type                   = IS25LP_IS25WP_SERIES,
        .device_density                = IS25WP032D_SIZE,
        .is_suspended                  = flash_issi_is_suspended,
        .initialize                    = flash_is25wp032d_initialize,
        .sys_clk_cfg                   = flash_is25wp032d_sys_clock_cfg,
        .get_dummy_bytes               = flash_is25wp032d_get_dummy_bytes,
        .break_seq_size                = HW_QSPI_BREAK_SEQ_SIZE_1B,
        .address_size                  = HW_QSPI_ADDR_SIZE_24,
        .fast_read_opcode              = CMD_FAST_READ_QUAD,
        .page_program_opcode           = ISSI_QUAD_IO_PAGE_PROGRAM,
        .erase_opcode                  = CMD_SECTOR_ERASE,
        .erase_suspend_opcode          = ISSI_ERASE_PROGRAM_SUSPEND,
        .erase_resume_opcode           = ISSI_ERASE_PROGRAM_RESUME,
        .quad_page_program_address     = false,
        .read_erase_progress_opcode    = CMD_READ_STATUS_REGISTER,
        .erase_in_progress_bit         = FLASH_STATUS_BUSY_BIT,
        .erase_in_progress_bit_high_level = true,
        .send_once                     = 1,
        .extra_byte                    = 0xA5,
        .ucode_wakeup                  = {is25wp032d_ucode_wakeup, sizeof(is25wp032d_ucode_wakeup)},
        .power_down_delay              = IS25WP032D_POWER_DOWN_DELAY_US,
        .release_power_down_delay      = IS25WP032D_RELEASE_POWER_DOWN_DELAY_US,
        .power_up_delay                = IS25WP032D_POWER_UP_DELAY_US,
        .qpi_mode                      = false,
        .is_ram                        = false,
        .memory_size                   = MEMORY_SIZE_32Mb, /* 32Mb bits */
};

__RETAINED_CODE static void flash_is25wp032d_initialize(HW_QSPIC_ID id)
{
        flash_issi_enable_quad_mode(id);
}

__RETAINED_CODE static void flash_is25wp032d_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk)
{
}

__RETAINED_CODE static uint8_t flash_is25wp032d_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk)
{
        return 2;
}

#endif /* _QSPI_IS25WP032D_H_ */
/**
 * \}
 * \}
 */
