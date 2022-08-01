/**
 ****************************************************************************************
 *
 * @file qspi_XXX_template.h
 *
 * @brief Driver for flash XXXX
 *
 * Copyright (C) 2016-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _QSPI_XTX_XT25Q08B_H_
#define _QSPI_XTX_XT25Q08B_H_

/**
 * \brief The Flash type JEDEC ID
 *
 * This is the second byte returned by the 0x9F command
 */
#define XT25_SERIES                                     0x60

/**
 * \brief The Flash density JEDEC ID
 *
 * This is the third byte returned by the 0x9F command
 */
#define XT25Q08B_SIZE                                   0x14


#include "qspi_common.h"
#include "qspi_xtx.h"


// Flash power up/down timings
#define XT25Q08B_POWER_DOWN_DELAY_US                    3 /* cannot have less than 3us delay */
#define XT25Q08B_RELEASE_POWER_DOWN_DELAY_US            20
#define XT25Q08B_POWER_UP_DELAY_US                      10000

#if (dg_configFLASH_POWER_OFF == 1)
/**
 * \brief uCode for handling the QSPI FLASH activation from power off.
 */
        /*
         * Delay 10usec
         * 0x01   // CMD_NBYTES = 0, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         * 0xA0   // CMD_WT_CNT_LS = 160 --> 10000 / 62.5 = 160 = 10usec
         * 0x00   // CMD_WT_CNT_MS = 0
         * Exit from Fast Read mode
         * 0x09   // CMD_NBYTES = 1, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         * 0x00   // CMD_WT_CNT_LS = 0
         * 0x00   // CMD_WT_CNT_MS = 0
         * 0xFF   // Enable Reset
         * (up to 16 words)
         */
        const uint32_t xt25q08b_ucode_wakeup[] = {
                0x09000001 | (((uint16_t)(XT25Q08B_POWER_UP_DELAY_US*1000/62.5) & 0xFFFF) << 8),
                0x00FF0000,
        };
#elif (dg_configFLASH_POWER_DOWN == 1)
/**
 * \brief uCode for handling the QSPI FLASH release from power-down.
 */
        /*
         * 0x09   // CMD_NBYTES = 1, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         * 0x30   // CMD_WT_CNT_LS = 3000 / 62.5 = 48 // 3usec
         * 0x00   // CMD_WT_CNT_MS = 0
         * 0xAB   // Release Power Down
         * (up to 16 words)
         */
        const uint32_t xt25q08b_ucode_wakeup[] = {
                0xAB000009 | (((uint16_t)(XT25Q08B_RELEASE_POWER_DOWN_DELAY_US*1000/62.5) & 0xFFFF) << 8),
        };
#else
/**
 * \brief uCode for handling the QSPI FLASH exit from the "Continuous Read Mode".
 */
        /*
         * 0x25   // CMD_NBYTES = 4, CMD_TX_MD = 2 (Quad), CMD_VALID = 1
         * 0x00   // CMD_WT_CNT_LS = 0
         * 0x00   // CMD_WT_CNT_MS = 0
         * 0x55   // Clocks 0-1 (A23-16)
         * 0x55   // Clocks 2-3 (A15-8)
         * 0x55   // Clocks 4-5 (A7-0)
         * 0x55   // Clocks 6-7 (M7-0) : M5-4 != '10' ==> Disable "Continuous Read Mode"
         * (up to 16 words)
         */
        const uint32_t xt25q08b_ucode_wakeup[] = {
                0x55000025,
                0x00555555,
        };
#endif


/**
 * \brief This is called to initialize the flash
 */
static void flash_xt25q08b_initialize(HW_QSPIC_ID id);

/**
 * \brief This is called each time the system clock is changed
 *
 * This can be used e.g. to change the qspi controller divider or modify the
 * flash dummy bytes, if e.g. the flash cannot cope with the higher clock
 * frequency.
 */
static void flash_xt25q08b_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk);

/**
 * \brief This must return the number of dummy bytes required.
 *
 * In most cases, this can return a static value (usually 2). Sometimes,
 * however, the dummy bytes must change (e.g. according to the system clock
 * frequency).
 */
static uint8_t flash_xt25q08b_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk);

/**
 * \brief This structs configures the system for the specific flash
 *
 * \note This struct MUST be const for this to work. Therefore, assignments must
 *       not change (must be read-only)
 */
static const qspi_flash_config_t flash_xt25q08b_config = {
        .manufacturer_id                  = XTX_ID,
        .device_type                      = XT25_SERIES,
        .device_density                   = XT25Q08B_SIZE,
        .is_suspended                     = flash_xtx_is_suspended,
        .initialize                       = flash_xt25q08b_initialize,
        .sys_clk_cfg                      = flash_xt25q08b_sys_clock_cfg,
        .get_dummy_bytes                  = flash_xt25q08b_get_dummy_bytes,
        .break_seq_size                   = HW_QSPI_BREAK_SEQ_SIZE_1B,
        .address_size                     = HW_QSPI_ADDR_SIZE_24,
        .fast_read_opcode                 = CMD_FAST_READ_QUAD,
        .page_program_opcode              = XTX_QUAD_IO_PAGE_PROGRAM,
        .quad_page_program_address        = true,
        .erase_opcode                     = CMD_SECTOR_ERASE,
        .read_erase_progress_opcode       = CMD_READ_STATUS_REGISTER,
        .erase_suspend_opcode             = XTX_ERASE_PROGRAM_SUSPEND,
        .erase_resume_opcode              = XTX_ERASE_PROGRAM_RESUME,
        .erase_in_progress_bit            = FLASH_STATUS_BUSY_BIT,
        .erase_in_progress_bit_high_level = true,
        .send_once                        = 1,
        .extra_byte                       = 0xA0,
        .ucode_wakeup                     = {xt25q08b_ucode_wakeup, sizeof(xt25q08b_ucode_wakeup)},
        .power_down_delay                 = XT25Q08B_POWER_DOWN_DELAY_US,
        .power_up_delay                   = XT25Q08B_POWER_UP_DELAY_US,
        .release_power_down_delay         = XT25Q08B_RELEASE_POWER_DOWN_DELAY_US,
        .qpi_mode                         = false,
        .is_ram                           = false,
        .memory_size                      = MEMORY_SIZE_8Mb,
};



__RETAINED_CODE static void flash_xt25q08b_initialize(HW_QSPIC_ID id)
{
        flash_xtx_enable_quad_mode(id);
}

__RETAINED_CODE static void flash_xt25q08b_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk)
{

}

__RETAINED_CODE static uint8_t flash_xt25q08b_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk)
{
                return 2;
}

#endif /* _QSPI_XXX_H_ */
/**
 * \}
 * \}
 */
