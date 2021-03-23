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
 * @file qspi_at25e321.h
 *
 * @brief QSPI flash driver for the Adesto at25xe321
 *
 * Copyright (C) 2016-2018 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef _QSPI_AT25XE321_H_
#define _QSPI_AT25XE321_H_

#include "qspi_automode.h"


/**
 * \brief The Flash vendor JEDEC ID
 *
 * This is the first byte returned by the 0x9F command
 */
#ifndef ADESTO_ID
#define ADESTO_ID                       0x1F
#endif

/**
 * \brief The Flash type JEDEC ID
 *
 * This is the second byte returned by the 0x9F command
 */

#ifndef ADESTO_AT25X_SERIES
#define ADESTO_AT25X_SERIES             0x47
#endif

/**
 * \brief The Flash density JEDEC ID
 *
 * This is the third byte returned by the 0x9F command
 */
#ifndef AT25XE321_SIZE
#define AT25XE321_SIZE                  0x0C
#endif


#include "qspi_common.h"

// Custom command opcodes
#define AT_READ_STATUS_REG2                     0x35
#define AT_READ_STATUS_REG3                     0x15
#define AT_WRITE_STATUS_REG2                    0x31
#define AT_WRITE_STATUS_REG3                    0x11

#define AD_QUAD_IO_PAGE_PROGRAM                 0x32

#define AD_ERASE_PROGRAM_SUSPEND                0x75
#define AD_ERASE_PROGRAM_RESUME                 0x7A



#define AT_STATUS2_SS_BIT                       (7)
#define AT_STATUS2_SS_MASK                      (1 << AT_STATUS2_SS_BIT)

#define AT_STATUS2_QE_BIT                       (1)
#define AT_STATUS2_QE_MASK                      (1 << AT_STATUS2_QE_BIT)

#define AT_STATUS3_DRV_LEVEL_MASK               (0x3 << 5)

#define AT_STATUS4_XIP_EN_BIT                   (3)
#define AT_STATUS4_XIP_EN_MASK                  (1 << AT_STATUS4_XIP_EN_BIT)

#define AT_STATUS4_POWER_DOWN_MODE_BIT          (7)
#define AT_STATUS4_POWER_DOWN_MODE_MASK         (1 << AT_STATUS4_POWER_DOWN_MODE_BIT)

#define AT_INDIRECT_STATUS_ADDR_READ            (0x65)
#define AT_INDIRECT_STATUS_ADDR_WRITE           (0x71)

#define AT25xe321_POWER_DOWN_DELAY_US           (3)
#define AT25xe321_RELEASE_POWER_DOWN_DELAY_US   (24) //16us Typical // 24uS max
#define AT25xe321_POWER_UP_DELAY_US             (260)


/**
 * \brief The Flash type JEDEC ID
 *
 * This is the second byte returned by the 0x9F command
 */
#define AD_USE_XIP_MODE                         (1)




typedef enum{
        TWO_CLOCKS =    0x00,
        FOUR_CLOCKS =   0x10,
        SIX_CLOCKS =    0x20,
        EIGHT_CLOCKS =  0x30,
        TEN_CLOCKS  =   0x40
}flash_at25ex321_dummy_clocks_t;


enum
{
        STATUS_REG1_IND_ADDR = 0x01,
        STATUS_REG2_IND_ADDR,
        STATUS_REG3_IND_ADDR,
        STATUS_REG4_IND_ADDR,
        STATUS_REG5_IND_ADDR

};

typedef enum
{
        ULTRA_DEEP_POWER_DOWN_MODE,
        DEEP_POWER_DOWN_MODE
}flash_at25ex321_power_down_mode_t;

typedef enum
{
        DRV_STRENGTH_30PF = 0x00,
        DRV_STRENGTH_22PF,
        DRV_STRENGTH_15PF,
        DRV_STRENGTH_AUTO
}flash_at25ex321_drive_strength_t;


//uCode only used in 68x variants, not 69x
const uint32_t at25xe321_ucode_wakeup[] = {
        0x55000025,
        0x00555555,
};

__RETAINED static volatile uint8_t flash_at25xe321_dummy_bytes = 4;

__RETAINED_CODE static void flash_at25xe321_initialize(HW_QSPIC_ID id);
__RETAINED_CODE static void flash_at25xe321_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk);
__RETAINED_CODE static bool flash_at25xe321_is_suspended(HW_QSPIC_ID id);
__RETAINED_CODE static void flash_at25xe321_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk);
__RETAINED_CODE static uint8_t flash_at25xe321_get_dummy_bytes(HW_QSPIC_ID id);
__RETAINED_CODE static uint8_t flash_at25xe321_set_dummy_clocks(HW_QSPIC_ID id, flash_at25ex321_dummy_clocks_t clocks);
__RETAINED_CODE static uint8_t flash_at25e321_read_status_reg_4(HW_QSPIC_ID id);
__RETAINED_CODE static void flash_at25e321_write_status_reg_4(HW_QSPIC_ID id, uint8_t value);

#if dg_configFLASH_POWER_DOWN
__RETAINED_CODE void flash_at25xe321_set_power_down_mode(HW_QSPIC_ID id, flash_at25ex321_power_down_mode_t pwr_down);
#endif

static qspi_flash_config_t flash_at25xe321_config = {
        .manufacturer_id                  = ADESTO_ID,
        .device_type                      = ADESTO_AT25X_SERIES,
        .device_density                   = AT25XE321_SIZE,
        .is_suspended                     = flash_at25xe321_is_suspended,
        .initialize                       = flash_at25xe321_initialize,
        .sys_clk_cfg                      = flash_at25xe321_sys_clock_cfg,
        .get_dummy_bytes                  = flash_at25xe321_get_dummy_bytes,
        .break_seq_size                   = HW_QSPI_BREAK_SEQ_SIZE_2B,
        .address_size                     = HW_QSPI_ADDR_SIZE_24,
        .page_program_opcode              = CMD_QUAD_PAGE_PROGRAM,
        .page_qpi_program_opcode          = CMD_QPI_PAGE_PROGRAM,
		.fast_read_opcode 				  = CMD_FAST_READ_QUAD,
        .quad_page_program_address        = false,
        .erase_opcode                     = CMD_SECTOR_ERASE,
        .erase_suspend_opcode             = AD_ERASE_PROGRAM_SUSPEND,
        .erase_resume_opcode              = AD_ERASE_PROGRAM_RESUME,
        .read_erase_progress_opcode       = CMD_READ_STATUS_REGISTER,
        .erase_in_progress_bit            = FLASH_STATUS_BUSY_BIT,
        .erase_in_progress_bit_high_level = true,
#if AD_USE_XIP_MODE
        .send_once                        = true,
        .extra_byte                       = 0xA0,

#else
        .send_once                        = false,
        .extra_byte                       = 0x00,
#endif
        .ucode_wakeup                     = {at25xe321_ucode_wakeup, sizeof(at25xe321_ucode_wakeup)},
        .power_down_delay                 = AT25xe321_POWER_DOWN_DELAY_US,
        .release_power_down_delay         = AT25xe321_RELEASE_POWER_DOWN_DELAY_US,
        .power_up_delay                   = AT25xe321_POWER_UP_DELAY_US,
        .is_ram                           = false,
        .qpi_mode                         = false,
        .enter_qpi_opcode                 = 0x00,
        .memory_size                      = MEMORY_SIZE_32Mb, /* 32M-bit Serial Flash 1024K -byte */
};


__RETAINED_CODE static void flash_at25e321_write_status_reg_2(HW_QSPIC_ID id, uint8_t value)
{
        uint8_t volatile cmd[2];

        cmd[0] = AT_WRITE_STATUS_REG2;
        cmd[1] = value;

        flash_write(id, (uint8_t *)cmd, 2);

        /* Wait for the Flash to process the command */
        while (flash_is_busy(id));
}




#if dg_configFLASH_POWER_DOWN

__RETAINED_CODE void flash_at25xe321_set_power_down_mode(HW_QSPIC_ID id, flash_at25ex321_power_down_mode_t pwr_down)
{
        uint8_t status = 0xFF;
        bool deactivate_command_mode = false;

        //Changing power down mode dynamically, will need to exit auto -mode


        if(HW_QSPIC_REG_GETF(id, CTRLMODE, AUTO_MD))
        {
                qspi_int_activate_command_entry_mode(id);
                deactivate_command_mode = true;

        }

        status = flash_at25e321_read_status_reg_4(id);

        //Check if bit 7 is set or cleared accordign to pwr_down, if not toggle the bit
        if(((status & AT_STATUS4_POWER_DOWN_MODE_MASK) >> AT_STATUS4_POWER_DOWN_MODE_BIT)  ^ pwr_down )
        {
                flash_write_enable(id);
                flash_at25e321_write_status_reg_4(id, status | (pwr_down << AT_STATUS4_POWER_DOWN_MODE_BIT) );
        }


        //Need to change the release power down value to match the power down mode
        if(pwr_down == DEEP_POWER_DOWN_MODE)
        {
                flash_at25xe321_config.release_power_down_delay = AT25xe321_RELEASE_POWER_DOWN_DELAY_US;
        }
        else if(pwr_down == ULTRA_DEEP_POWER_DOWN_MODE)
        {
				/*ULTRA DEEP POWER DOWN requires a JEDEC reset which the SDK does not provide.  Currently not supported.*/
                ASSERT_ERROR(0);
        }

        if(deactivate_command_mode)
                qspi_int_deactivate_command_entry_mode(id);




}
#endif

__RETAINED_CODE static uint8_t flash_at25xe321_set_dummy_clocks(HW_QSPIC_ID id, uint8_t clocks)
{

        uint8_t status;
        uint8_t volatile read_cmd[3];
        uint8_t volatile write_cmd[3];

        //Need to do these commands programatically here, when in command mode, need the constants in RAM not in flash for access
        write_cmd[0] = AT_INDIRECT_STATUS_ADDR_WRITE;
        write_cmd[1] = STATUS_REG5_IND_ADDR;
        write_cmd[2] = clocks;

        read_cmd[0] = AT_INDIRECT_STATUS_ADDR_READ;
        read_cmd[1] = STATUS_REG5_IND_ADDR;
        read_cmd[2] = 0x00; //needed to clock in the read byte


        flash_write_enable(id);
        flash_write(id, (uint8_t *)write_cmd, 3);
        while (flash_is_busy(id));
        flash_transact(id, (uint8_t *)read_cmd, 3, &status, 1);

        if(!(status == clocks))
        {
                ASSERT_ERROR(0);
        }

        /*Clocks are equal to (dummy_clocks>>4)*2 + 2. E.g. (EIGHT_CLOCKS>>4)*2 +2 = 8
         dummy clocks include 2 SCK for Mode bits M[7:0] so subtract two.  Then each clock is .5 bytes so divide by two
         FOUR_CLOCKS = 2 dummy bytes
         TWO_CLOCKS = 0, FOUR_CLOCKS = 1, SIX_CLOCKS = 2, EIGHT_CLOCKS = 3, TEN_CLOCKS = 4
          */
        flash_at25xe321_dummy_bytes = (clocks>>4) ;
        hw_qspi_set_dummy_bytes_count(id, flash_at25xe321_dummy_bytes);



        return status;
}



__RETAINED_CODE static uint8_t flash_at25e321_read_status_reg_2(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t volatile cmd[1];

        cmd[0] = AT_READ_STATUS_REG2;

        flash_transact(id, (uint8_t *)cmd, 2, &status, 1);

        return status;
}

__RETAINED_CODE uint8_t flash_at25e321_read_status_reg_4(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t volatile read_cmd[3];

        read_cmd[0] = AT_INDIRECT_STATUS_ADDR_READ;
        read_cmd[1] = STATUS_REG4_IND_ADDR;
        read_cmd[2] = 0x00;

        flash_transact(id, (uint8_t *)read_cmd, 3, &status, 1);

        return status;
}

__RETAINED_CODE  void flash_at25e321_write_status_reg_4(HW_QSPIC_ID id, uint8_t value)
{

        uint8_t volatile write_cmd[3];
        write_cmd[0] = AT_INDIRECT_STATUS_ADDR_WRITE;
        write_cmd[1] = STATUS_REG4_IND_ADDR;
        write_cmd[2] = value;

        flash_write(id, (uint8_t *)write_cmd, 3);

        /* Wait for the Flash to process the command */
        while (flash_is_busy(id));
}

__RETAINED_CODE static void flash_at25xe321_initialize(HW_QSPIC_ID id)
{
        uint8_t status;
        flash_at25xe321_dummy_bytes = 4;



        status = flash_at25e321_read_status_reg_2(id);
        if (!(status & AT_STATUS2_QE_MASK)) {
                flash_write_enable(id);
                flash_at25e321_write_status_reg_2(id, status | AT_STATUS2_QE_MASK);
        }

#if AD_USE_XIP_MODE
        status = flash_at25e321_read_status_reg_4(id);
        if(!(status & AT_STATUS4_XIP_EN_MASK))
        {
                flash_write_enable(id);
                flash_at25e321_write_status_reg_4(id, status | AT_STATUS4_XIP_EN_MASK);
        }
#else
        status = flash_at25e321_read_status_reg_4(id);
        if((status & AT_STATUS4_XIP_EN_MASK))
        {
                flash_write_enable(id);
                flash_at25e321_write_status_reg_4(id, status & (~AT_STATUS4_XIP_EN_MASK));
        }

#endif
        //status1 = status;
#if dg_configFLASH_POWER_DOWN
       flash_at25xe321_set_power_down_mode(id, DEEP_POWER_DOWN_MODE);
#endif


        flash_at25xe321_set_dummy_clocks(id, TEN_CLOCKS);


}
__RETAINED_CODE static bool flash_at25xe321_is_suspended(HW_QSPIC_ID id)
{
        __DBG_QSPI_VOLATILE__ uint8_t status;
        uint8_t volatile cmd[1];

        cmd[0] = AT_READ_STATUS_REG2;

        flash_transact(id, (uint8_t *)cmd, 2, &status, 1);
        return ((status & AT_STATUS2_SS_MASK) != 0);
}

__RETAINED_CODE static void flash_at25xe321_sys_clock_cfg(HW_QSPIC_ID id, sys_clk_t sys_clk)
{



}

__RETAINED_CODE static uint8_t flash_at25xe321_get_dummy_bytes(HW_QSPIC_ID id)
{
        return flash_at25xe321_dummy_bytes;
}

#endif /* _QSPI_AT25XE321_H_ */
/**
 * \}
 * \}
 */
