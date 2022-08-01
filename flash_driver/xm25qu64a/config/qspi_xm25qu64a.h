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
 * @file qspi_xm25qu64a.h
 *
 * @brief Driver for flash XMC XM25QU64A
 *
 * Copyright (C) 2016-2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**************************************** flash_configurations.xml ENTRY *****************************************

  <configuration name="XMC XM25QU64A">
    <flash_size>0x800000</flash_size>
    <flash_burstcmda_reg_value>0xa8a500eb</flash_burstcmda_reg_value>
    <flash_burstcmdb_reg_value>0x00000066</flash_burstcmdb_reg_value>
    <flash_write_config_command>0x07</flash_write_config_command>
  </configuration>

 *****************************************************************************************************************/


#ifndef _QSPI_XMC_XM25QU64A_H_
#define _QSPI_XMC_XM25QU64A_H_

#include "qspi_common.h"
#include "stdio.h"

/************************************************* JEDEC ID INFO *************************************************/

/**
 * \brief The Flash type JEDEC ID
 *
 * This is the first byte returned by the 0x9F command
 */
#define XMC_ID                                          0x20

/**
 * \brief The Flash type JEDEC ID
 *
 * This is the second byte returned by the 0x9F command
 */
#define XM25QU_TYPE                                     0x38

/**
 * \brief The Flash density JEDEC ID
 *
 * This is the third byte returned by the 0x9F command
 */
#define XM25QU64A_SIZE                                  0x17

/**************************************************** TIMINGS ****************************************************/

/* Flash power up/down timings in usec */
#define XM25QU64A_POWER_DOWN_DELAY_US                   3       /* cannot have less than 3us delay */
#define XM25QU64A_RELEASE_POWER_DOWN_DELAY_US           3       /* cannot have less than 3us delay */
#define XM25QU64A_POWER_UP_DELAY_US                     10000

/************************************************ Opcodes SECTION ************************************************/

/* Status Register Opcodes */
#define XM25QU64A_READ_STATUS_REGISTER_2_OPCODE         0x09

/* Write Disable Opcode */
#define XM25QU64A_WRITE_DISABLE_OPCODE                  0x04

/* Erase/Program Suspend/Resume Opcodes */
#define XM25QU64A_ERASE_PROGRAM_SUSPEND_OPCODE          0xB0
#define XM25QU64A_ERASE_PROGRAM_RESUME_OPCODE           0x30

/* Quad IO Fast Read Opcode */
#define XM25QU64A_FAST_READ_QUAD_OPCODE                 0xEB

/* Enter OTP Mode Opcode */
#define XM25QU64A_ENTER_OTP_MODE_OPCODE                 0x3A

/* Quad Page Program */
#define XM25QU64A_QUAD_PAGE_PROGRAM_OPCODE              0x32

/********************************************** DRIVER GENERIC INFO **********************************************/

/* WXDIS Bit of OTP Mode Status Register */
#define XM25QU64A_WXDIS_BIT_POS                         (6)
#define XM25QU64A_WXDIS_MASK                            (1 << XM25QU64A_WXDIS_BIT_POS)

/* Erase/Program Suspend Bit Position - Status Register 2 */
#define XM25QU64A_SR_2_WSE_BIT_POS                      (2)
#define XM25QU64A_SR_2_WSP_BIT_POS                      (3)
#define XM25QU64A_SR_2_EPSUS_MASK                       ((1 << XM25QU64A_SR_2_WSE_BIT_POS) | (1 << XM25QU64A_SR_2_WSP_BIT_POS))

/* Extra/Mode Byte used for entering continuous read mode */
#define XM25QU64A_EXTRA_BYTE_M7_M0                      0xA5

/************************************************* uCode SECTION *************************************************/

/*
 * This section is OBSOLETE in DA1469x Family products however it is maintained here for reference reasons as
 * this part is mandatory for DA14683 products.
 */

#if (dg_configFLASH_POWER_OFF == 1)
/**
 * \brief uCode for handling the QSPI FLASH activation from power off.
 */

const uint32_t xm25qu64a_ucode_wakeup[] = {
        0x11000001 | (((uint16_t)(XM25QU64A_POWER_UP_DELAY_US*1000/62.5) & 0xFFFF) << 8),
        0xFFFF0000,
};
#elif (dg_configFLASH_POWER_DOWN == 1)
/**
 * \brief uCode for handling the QSPI FLASH release from power-down.
 */
const uint32_t xm25qu64a_ucode_wakeup[] = {
        0xAB000009 | (((uint16_t)(XM25QU64A_RELEASE_POWER_DOWN_DELAY_US*1000/62.5) & 0xFFFF) << 8),
};
#else
/**
 * \brief uCode for handling the QSPI FLASH exit from the "Continuous Read Mode".
 */
const uint32_t xm25qu64a_ucode_wakeup[] = {
        0xFF000025,
        0x00FFFFFF,
};

const uint32_t xm25qu64a_ucode_wakeup_32bit_addressing[] = {
        0xFF000045,
        0xFFFFFFFF,
        0x00FFFFFF,
};

#endif

/********************************************** FUNCTION PROTOTYPES **********************************************/
/**
 * \brief Set up the device to be able to accept Quad Page Program requests.
 *
 * \param[in] id        QSPI controller id
 *
 * \note This function is responsible for first entering OTP operational mode and then setting up the WXDIS bit of the
 *       Status Register. Keep in mind that WXDIS bit is only accessible as such in OTP mode and will not be perceived
 *       as such while in normal mode.
 *
 * See also: flash_cm25qu64a_enter_OTP_mode(HW_QSPIC_ID id);
 */
__RETAINED_CODE __UNUSED static void flash_xm25qu64a_enable_quad_mode(HW_QSPIC_ID id);

/**
 * \brief Enter OTP operational mode.
 *
 * \param[in] id        QSPI controller id
 *
 * \note This function is responsible for entering OTP operational mode granting access to the OTP sector. During this mode
 *       Only Erase Sector instruction can be used to erase while all other erase instructions are ignored. Additionally, the
 *       Status Register bits[2:7] have alternative functionality as shown below.
 *
 *       +-----+-----+-----+-----+-----+-----+-----+-----+             +-----+-----+-----+-----+-----+-----+-----+-----+
 *       | SRP | EBL | BP3 | BP2 | BP1 | BP0 | WEL | WIP |     -->     |OTP_L|WXDIS| HRSW| 4KBL|  TB | RES | WEL | WIP |
 *       +-----+-----+-----+-----+-----+-----+-----+-----+             +-----+-----+-----+-----+-----+-----+-----+-----+
 *
 *       Consult XMC XM25QU64A Datasheet for more information about the status register.
 *
 * See also: flash_cm25qu64a_enable_mode(HW_QSPIC_ID id);
 */
__RETAINED_CODE __UNUSED static void flash_xm25qu64a_enter_OTP_mode(HW_QSPIC_ID id);

/**
 * \brief Return the dummy bytes required when utilizing Fast Read Quad.
 *
 * \param[in] id        QSPI controller id
 *
 * \note This function will return the number of dummy bytes required while Fast Read Quad is activated. In some QSPI Flash devices,
 *       it is possible that the number of dummy bytes required can vary depending on the operating speed.
 */
__RETAINED_CODE static uint8_t flash_xm25qu64a_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk);

/**
 * \brief Initialize XM25QU64A QSPI Flash
 *
 * \param[in] id        QSPI controller id
 *
 * \note This function will perform all the required actions in order for XM25QU64A QSPI Flash to
 *       be properly initialized.
 */
__RETAINED_CODE static void flash_xm25qu64a_initialize(HW_QSPIC_ID id);

/**
 * \brief Return true if an erase or program operation is already suspended.
 *
 * \param[in] id        QSPI controller id
 *
 * \note This function reads the status register and checks the erase/program suspend bit. If it is set then it return true since
 *       a program or erase operation is currently suspended.
 */
__RETAINED_CODE static bool flash_xm25qu64a_is_suspended(HW_QSPIC_ID id);

/**
 * \brief Read the Status Register 2 of the Flash
 *
 * \param[in] id        QSPI controller id
 *
 * \return uint8_t The value of the Status Register 2 of the Flash.
 */
__RETAINED_CODE static uint8_t flash_xm25qu64a_read_status_register_2(HW_QSPIC_ID id);

/**
 * \brief Hook function called when the clock is changed
 *
 * \param[in] id        QSPI controller id
 * \param[in] sys_clk   The clock source to transition to.
 *
 * \note This function will be called prior to switching from one clock source to another. This function allows doing all
 *       the required tasks before the clock source is switched. Typical use could be setting up the proper QSPI divider
 *       to ensure that the device will continue to operate even after the clock source will be switched. E.g. Flash that supports
 *       up to 80 MHz operating speed will need to have a divider of 2 if PLL96 is picked as the source.
 */
__RETAINED_CODE static void flash_xm25qu64a_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk);

/**
 * \brief Issues Write Disable.
 *
 * \param[in] id QSPI controller id
 *
 * \note The secondary role of this function is to make the system exit OTP_mode.
 */
__RETAINED_CODE static void flash_xm25qu64a_write_disable(HW_QSPIC_ID id);

/************************************************* CONFIG OBJECT *************************************************/

/**
 * \brief This struct configures the system for the specific flash
 *
 * \note This struct MUST be const for this to work. Therefore, assignments must
 *       not change (must be read-only)
 */
static const qspi_flash_config_t flash_xm25qu64a_config = {
        /* JEDEC Bytes 9Fh */
        .manufacturer_id                  = XMC_ID,
        .device_type                      = XM25QU_TYPE,
        .device_density                   = XM25QU64A_SIZE,

        /* Flash Info */
        .memory_size                      = MEMORY_SIZE_64Mb,
        .address_size                     = HW_QSPI_ADDR_SIZE_24,
        .is_ram                           = false,
        .qpi_mode                         = false,

        /* Callbacks */
        .is_suspended                     = flash_xm25qu64a_is_suspended,
        .initialize                       = flash_xm25qu64a_initialize,
        .sys_clk_cfg                      = flash_xm25qu64a_sys_clock_cfg,
        .get_dummy_bytes                  = flash_xm25qu64a_get_dummy_bytes,

        /* Read */
        .fast_read_opcode                 = XM25QU64A_FAST_READ_QUAD_OPCODE,
        .send_once                        = 1,
        .extra_byte                       = XM25QU64A_EXTRA_BYTE_M7_M0,
        .break_seq_size                   = HW_QSPI_BREAK_SEQ_SIZE_1B,

        /* Page Program */
        .page_program_opcode              = XM25QU64A_QUAD_PAGE_PROGRAM_OPCODE,
        .quad_page_program_address        = false,

        /* Sector Erase */
        .erase_opcode                     = CMD_SECTOR_ERASE,
        .read_erase_progress_opcode       = CMD_READ_STATUS_REGISTER,
        .erase_in_progress_bit            = FLASH_STATUS_BUSY_BIT,
        .erase_in_progress_bit_high_level = true,

        /* Program/Erase Suspend/Resume */
        .erase_suspend_opcode             = XM25QU64A_ERASE_PROGRAM_SUSPEND_OPCODE,
        .erase_resume_opcode              = XM25QU64A_ERASE_PROGRAM_RESUME_OPCODE,

        /* Timings */
        .power_down_delay                 = XM25QU64A_POWER_DOWN_DELAY_US,
        .release_power_down_delay         = XM25QU64A_RELEASE_POWER_DOWN_DELAY_US,
        .power_up_delay                   = XM25QU64A_POWER_UP_DELAY_US,
};

/********************************************* FUNCTION  DEFINITIONS *********************************************/

__RETAINED_CODE static void flash_xm25qu64a_enable_quad_mode(HW_QSPIC_ID id)
{
        uint8_t status;

        /* Enter OTP Mode so the Status Register Bits will have alternate functionality */
        flash_xm25qu64a_enter_OTP_mode(id);

        /* Read the Altered Functionality Status Register */
        status = flash_read_status_register(id);

        /* If WXDIS bit is not set, set it */
        if (!(status & XM25QU64A_WXDIS_MASK))
        {
                flash_write_enable(id);
                flash_write_status_register(id, status | XM25QU64A_WXDIS_MASK);
        }

        /* Exit OTP Mode */
        flash_xm25qu64a_write_disable(id);
}

__RETAINED_CODE static void flash_xm25qu64a_enter_OTP_mode(HW_QSPIC_ID id)
{
        uint8_t cmd[1] = { XM25QU64A_ENTER_OTP_MODE_OPCODE };

        do
        {
                flash_write(id, cmd, 1);
        }
        while (flash_is_busy(id));
}

__RETAINED_CODE static uint8_t flash_xm25qu64a_get_dummy_bytes(HW_QSPIC_ID id, sys_clk_t sys_clk)
{
        /* 2 Dummy Bytes are required for non QPI mode of operation regardless of Operating Frequency for XMC XM25QU64A. */
        return 2;
}

__RETAINED_CODE static void flash_xm25qu64a_initialize(HW_QSPIC_ID id)
{
        flash_xm25qu64a_enable_quad_mode(id);
}

__RETAINED_CODE static bool flash_xm25qu64a_is_suspended(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;

        status = flash_xm25qu64a_read_status_register_2(id);
        return ((status & XM25QU64A_SR_2_EPSUS_MASK) != 0);
}

__RETAINED_CODE static uint8_t flash_xm25qu64a_read_status_register_2(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t cmd[] = { XM25QU64A_READ_STATUS_REGISTER_2_OPCODE };

        flash_transact(id, cmd, 1, &status, 1);

        return status;
}

__RETAINED_CODE static void flash_xm25qu64a_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk)
{
        /* Operating Frequency larger than 96 MHz achieved through PLL so there is no need to use a divider. Empty hook function */
}

__RETAINED_CODE static void flash_xm25qu64a_write_disable(HW_QSPIC_ID id)
{
        uint8_t cmd[1] = { XM25QU64A_WRITE_DISABLE_OPCODE };

        do
        {
                flash_write(id, cmd, 1);
        }
        while (flash_is_busy(id));
}

#endif /* _QSPI_XMC_XM25QU64A_H_ */
/**
 * \}
 * \}
 */
