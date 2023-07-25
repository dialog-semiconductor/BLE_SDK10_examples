/**
 ****************************************************************************************
 *
 * @file qspi_XXX_template.h
 *
 * @brief Driver for flash XXXX
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

#ifndef _QSPI_PUYA_H_
#define _QSPI_PUYA_H_

#define PUYA_ID         0x85

#include "qspi_common.h"

#define P25Q_WRITE_STATUS_REGISTER2             0x31
#define P25Q_READ_STATUS_REGISTER2              0x35

#define P25Q_ERASE_PROGRAM_SUSPEND              0x75
#define P25Q_ERASE_PROGRAM_RESUME               0x7A

#define P25Q_STATUS2_QE_BIT                     1
#define P25Q_STATUS2_QE_MASK                    (1 << P25Q_STATUS2_QE_BIT)

#define P25Q_STATUS2_SUS1_BIT                   7
#define P25Q_STATUS2_SUS1_MASK                  (1 << P25Q_STATUS2_SUS1_BIT)

#define P25Q_STATUS2_SUS2_BIT                   2
#define P25Q_STATUS2_SUS2_MASK                  (1 << P25Q_STATUS2_SUS2_BIT)

#define P25Q_POWER_DOWN_DELAY_US                3  /* tDP */
#define P25Q_POWER_UP_DELAY_US                  70 /* tVSL */
#define P25Q_RELEASE_POWER_DOWN_DELAY_US        8  /* tRES1 */



#if (dg_configFLASH_POWER_OFF == 1)
/**
 * \brief uCode for handling the QSPI FLASH activation from power off.
 */
        /*
         *
         * Delay 70usec (P25Q_POWER_UP_DELAY_US)
         * 0x01   // CMD_NBYTES = 0, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         *        wait in cycles = 70*1000 /62.5 = 1120 (0x460)
         * 0x60   // CMD_WT_CNT_LS = 60
         * 0x04   // CMD_WT_CNT_MS = 04
         * Exit from Fast Read mode
         * 0x09   // CMD_NBYTES = 1, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         * 0x00   // CMD_WT_CNT_LS = 0
         * 0x00   // CMD_WT_CNT_MS = 0
         * 0xFF   // Enable Reset
         * (up to 16 words)
         */
        const uint32_t p25q_ucode_wakeup[] = {
                0x09000001 | (((uint16_t)(P25Q_POWER_UP_DELAY_US*1000/62.5) & 0xFFFF) << 8),
                0x00FF0000,
        };
#elif (dg_configFLASH_POWER_DOWN == 1)
/**
 * \brief uCode for handling the QSPI FLASH release from power-down.
 */
        /*
         *
         * 0x09   // CMD_NBYTES = 1, CMD_TX_MD = 0 (Single), CMD_VALID = 1
         * 0x80   // CMD_WT_CNT_LS = 8000 / 62.5 = 128 (0x80)// 8usec
         * 0x00   // CMD_WT_CNT_MS = 0
         * 0xAB   // Release Power Down
         * (up to 16 words)
         */
        const uint32_t p25q_ucode_wakeup[] = {
                0xAB000009 | (((uint16_t)(P25Q_RELEASE_POWER_DOWN_DELAY_US*1000/62.5) & 0xFFFF) << 8),
        };
#else
/**
 * \brief uCode for handling the QSPI FLASH exit from the "Continuous Read Mode".
 */
        /*
         *
         * 0x25   // CMD_NBYTES = 4, CMD_TX_MD = 2 (Quad), CMD_VALID = 1
         * 0x00   // CMD_WT_CNT_LS = 0
         * 0x00   // CMD_WT_CNT_MS = 0
         * 0x55   // Clocks 0-1 (A23-16)
         * 0x55   // Clocks 2-3 (A15-8)
         * 0x55   // Clocks 4-5 (A7-0)
         * 0x55   // Clocks 6-7 (M7-0) : M5-4 != '10' ==> Disable "Continuous Read Mode"
         * (up to 16 words)
         */
        const uint32_t p25q_ucode_wakeup[] = {
                0x55000025,
                0x00555555,
        };
#endif


static bool flash_p25q_is_suspended(HW_QSPIC_ID id);
static void flash_p25q_initialize(HW_QSPIC_ID id);


/**
 * \brief Read the Status Register 2 of the Flash
 *
 * \param[in] id QSPI controller id
 *
 * \return The status of the Register 2.
 *
 */
__STATIC_INLINE uint8_t flash_p25q_read_status_register_2(HW_QSPIC_ID id) __attribute__((always_inline));
__STATIC_INLINE uint8_t flash_p25q_read_status_register_2(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { P25Q_READ_STATUS_REGISTER2 };

        flash_transact(id, cmd, 1, &status, 1);

        return status;
}

/**
 * \brief Write the Status Register 2 of the Flash
 *
 * \param[in] id QSPI controller id
 * \param[in] value The value to be written.
 *
 * \note This function blocks until the Flash has processed the command. No verification that the
 *        value has been actually written is done though. It is up to the caller to decide whether
 *        such verification is needed or not and execute it on its own.
 */
__STATIC_INLINE void flash_p25q_write_status_register_2(HW_QSPIC_ID id, uint8_t value) __attribute__((always_inline));
__STATIC_INLINE void flash_p25q_write_status_register_2(HW_QSPIC_ID id, uint8_t value)
{
        uint8_t cmd[] = { P25Q_WRITE_STATUS_REGISTER2, value };

        flash_write(id, cmd, 2);

        /* Wait for the Flash to process the command */
        while (flash_is_busy(id));
}


/**
 * \brief Enable the quad mode of Flash
 *
 * \param[in] id QSPI controller id
 *
 *
 */
__STATIC_INLINE void flash_p25q_enable_quad_mode(HW_QSPIC_ID id) __attribute__((always_inline));
__STATIC_INLINE void flash_p25q_enable_quad_mode(HW_QSPIC_ID id)
{
        uint8_t status;

        status = flash_p25q_read_status_register_2(id);
        if (!(status & P25Q_STATUS2_QE_MASK)) {
                flash_write_enable(id);
                flash_p25q_write_status_register_2(id, status | P25Q_STATUS2_QE_MASK);
        }
}
/**
 * \brief This returns true if the flash is in write/erase suspend mode
 */
__RETAINED_CODE static bool flash_p25q_is_suspended(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;

        status = flash_p25q_read_status_register_2(id);
        return (status & (P25Q_STATUS2_SUS1_MASK | P25Q_STATUS2_SUS2_MASK)) !=0;
}
/**
 * \brief This is called to initialize the flash
 */
__RETAINED_CODE static void flash_p25q_initialize(HW_QSPIC_ID id)
{
        /* Handle 24/32 bit addressing modes, enable QUAD mode, etc */
        flash_p25q_enable_quad_mode(id);
}

#endif /* _QSPI_PUYA_H_ */
/**
 * \}
 * \}
 */
