/**
 ****************************************************************************************
 *
 * @file fault_exception_analysis.c
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "osal.h"
#include "hw_mpu.h"

#include "fault_handling.h"
#include "user_application.h"

/* Macro definitions */
#define PRINT_NULL                     printf("\n\r")
#define PRINT_REG_HEADER               printf("\n\rBit Position |   Bit-Field Name |  Description\n\n\r")


/*  This is an invalid memory address - Outside system's memory boundaries */
#define _FAULT_INVALID_MEM_ADDRESS     (0x99999999UL)

/* Unaligned memory address */
#define _FAULT_UNALIGNED_MEM_ADDRESS   (0x50030E01)

/*
 * Structure used for storing the CPU status following a fault event. User can define
 * their own retained memory location in SRAM by changing the linker configurations
 * accordingly.
 */
volatile Fault_t _fault_exception_info __attribute__((section("hard_fault_info")));


/* For more info refer to Table 2-10 IPSR bit assignments - The Cortex-M33 Processor Programmer's model */
__RETAINED_RW static const char * _CORTEX_M33_IRQn_def[] = { _CORTEX_M33_IRQn_DEFINITION };

/* For more info refer to "The Cortex-M33 Processor Programmer's model" */
__RETAINED_RW static const char * _MMFSR_bit_field_def[] = { _MMFSR_BIT_FIELD_DESCRITION };

__RETAINED_RW static const char * _BFSR_bit_field_def[]  = { _BFSR_BIT_FIELD_DESCRIPTION };

__RETAINED_RW static const char * _UFSR_bit_field_def[]  = { _UFSR_BIT_FIELD_DESCRIPTION };


/* Flag used for triggering escalated fault exceptions */
__RETAINED_RW volatile bool fault_escalation_flag = false;


/* Enable/disable escalated fault exceptions */
static void _set_escalation_flag(bool flag)
{
        if (flag) {
              fault_escalation_flag = true;
        }
        else {
              fault_escalation_flag = false;
        }
}


static void _analyze_xFSR(uint32_t reg_val, uint8_t reg_size_in_bits, const char * report[])
{
        OS_ASSERT(reg_size_in_bits <= 31);

        bool flag = false;
        /*
         * Check each bit of the status register individually starting from LSB.
         * If set, then print the corresponding description.
         */
        for (size_t i=0; i < reg_size_in_bits; i++) {
               if (reg_val & (uint32_t)(1 << i)) {
                       printf("%s\n\r", report[i]);
                       flag = true;
               }
        }

        if (flag) {
                PRINT_NULL;
                PRINT_REG_HEADER;
        }
}

/* Analyze the contents of the Configurable Fault Status Register */
static void _analyze_CFSR(void)
{
        uint8_t memmanage_fault_status, bus_fault_status;
        uint16_t usage_fault_status;

        /* Analyze the CFSR */
        memmanage_fault_status = _FLD2VAL(SCB_CFSR_MEMFAULTSR, _fault_exception_info._CFSR._CFSR);
        bus_fault_status       = _FLD2VAL(SCB_CFSR_BUSFAULTSR, _fault_exception_info._CFSR._CFSR);
        usage_fault_status     = _FLD2VAL(SCB_CFSR_USGFAULTSR, _fault_exception_info._CFSR._CFSR);

        PRINT_NULL;
        PRINT_REG_HEADER;
        _analyze_xFSR((uint32_t)memmanage_fault_status,
                      sizeof(memmanage_fault_status) == 1 ? 8 : sizeof(memmanage_fault_status) == 2 ? 16 : 32,
                      _MMFSR_bit_field_def);
        _analyze_xFSR((uint32_t)bus_fault_status,
                      sizeof(bus_fault_status) == 1 ? 8 : sizeof(bus_fault_status) == 2 ? 16 : 32,
                      _BFSR_bit_field_def );
        _analyze_xFSR((uint32_t)usage_fault_status,
                      sizeof(usage_fault_status) == 1 ? 8 : sizeof(usage_fault_status) == 2 ? 16 : 32,
                      _UFSR_bit_field_def);
}


/*
 * Given the acquired CPU status after a fault triggering, this routine performs a
 * basic analysis of the causes of a fault exception.
 */
static void _analyze_fault_exception(void)
{

        printf("\n\n\r---- Extensive Fault Exception Analysis ----\n\r");

        printf("\n\r---- Stacked Core Registers ----\n\n\r");
        printf("  - R0    = 0x%08lx\r\n", _fault_exception_info._r0  );
        printf("  - R1    = 0x%08lx\r\n", _fault_exception_info._r1  );
        printf("  - R2    = 0x%08lx\r\n", _fault_exception_info._r2  );
        printf("  - R3    = 0x%08lx\r\n", _fault_exception_info._r3  );

        printf("  - R12   = 0x%08lx\r\n", _fault_exception_info._r12 );

        printf("  - LR    = 0x%08lx\r\n", _fault_exception_info._lr  );
        printf("  - PC    = 0x%08lx\r\n", _fault_exception_info._pc  );
        printf("  - xPSR  = 0x%08lx\r\n", _fault_exception_info._xPSR);

        /* Check if FP context was stacked during on exception entrance */
        if (_fault_exception_info.is_fp_valid) {
                for (int i = 0; i < _STAKED_FRAME_FP_MAX_ENTRIES; i++) {
                        printf("  - S[%d]  = 0x%08lx\n\r", i, *((uint32_t *)(&_fault_exception_info._s0 + (i*4))));
                }
                printf("  - FPSCR = 0x%08lx\r\n", _fault_exception_info._FPSCR);
        }
        else {
                printf("\n\n\r**** Floating Point (FP) context is not present in the Stack Frame ****\n\r");
        }

        printf("\n\n\r---- Fault Status Related Registers ----\n\n\r");
        printf("  - CFSR  = 0x%08lx\n\r", _fault_exception_info._CFSR._CFSR );
        printf("  - HFSR  = 0x%08lx\n\r", _fault_exception_info._HFSR       );
        printf("  - DFSR  = 0x%08lx\n\r", _fault_exception_info._DFSR       );
        printf("  - AFSR  = 0x%08lx\n\r", _fault_exception_info._AFSR       );

        printf("\n\n\r---- Fault Address Related Registers ----\n\n\r");
        printf("  - MMFAR = 0x%08lx\n\r", _fault_exception_info._MMFAR      );
        printf("  - BFAR  = 0x%08lx\n\r", _fault_exception_info._BFAR       );

        if (_FLD2VAL(SCB_HFSR_FORCED, _fault_exception_info._HFSR)) {
                printf("\n\n\r***** A forced HardFault took place - Escalated by a configurable fault ******\r\n");
        }
        if (_FLD2VAL(SCB_HFSR_VECTTBL, _fault_exception_info._HFSR)) {
                printf("\n\n\r***** HardFault on a vector table read during exception processing - The stacked PC value points to the instruction that was preempted by the exception ******\r\n");
        }

        printf("\n\n\r******* Exception Type: %s *******\n\n\r", _CORTEX_M33_IRQn_def[_fault_exception_info.irq_num]);

        /* Analyze the contents of the shadowed Configurable Fault Status Register  */
        _analyze_CFSR();
}


/* Check whether the system booted normally or due to a fault exception */
void _check_if_system_booted_normally(void)
{
        if (_fault_exception_info.magic_val == _FAULT_MAGIC_VALUE) {
                printf("\n\n\r******** System Rebooted Due to Fault Exception *******\n\r");
                /* Perform a basic analysis of the fault exception */
                _analyze_fault_exception();

                /* After fault analysis, invalidate the magic value */
                _fault_exception_info.magic_val = 0x00000000;
        }
        else {
              printf("\n\n\r********** System Booted Normally! ***********\n\r");
        }
}



/* Trigger a BusFault exception */
void _trigger_BusFault(void)
{
      /* Attempt to access a memory location outside memory boundaries */
      volatile uint8_t *p = (volatile uint8_t *) _FAULT_INVALID_MEM_ADDRESS;
      *p = 0xFF;
}


/* Trigger an escalated fault (HardFault) */
void _trigger_escalated_BusFault(void)
{
        _set_escalation_flag(true);
        _trigger_BusFault();
}


/* Trigger a HardFault exception */
void _trigger_HardFault(void)
{
        /*
         * Execution of a breakpoint instruction and while the CPU
         * not in debug mode, will trigger a HardFault.
         */
        __BKPT(0);
}


/* Trigger a Usage Fault deliberately */
void _trigger_UsageFault(void)
{
        /* Perform an unaligned memory access */
        volatile uint32_t *ptr = (volatile uint32_t *) _FAULT_UNALIGNED_MEM_ADDRESS;
        *ptr = 0xFFFFFFFF;
}


void _MPU_configuration(mpu_region_config * cfg)
{
        hw_mpu_disable();
        hw_mpu_config_region(HW_MPU_REGION_0, cfg);
        hw_mpu_enable(true);
}


/* Trigger a Memory Management exception */
void _trigger_MemFault(void)
{
    /* MPU Settings */
    mpu_region_config region_cfg = {
            .access_permissions = HW_MPU_AP_RO,                            // Mark the target region as Read-only. This area has full access, meaning that both privileged and unprivileged tasks can access this region.
            .attributes         = HW_MPU_ATTR_NORMAL,                      // Normal memory (internal memory)
            .execute_never      = HW_MPU_XN_FALSE,                         // Mark the target region as Executable region
            .start_addr         = MEMORY_SYSRAM_BASE & 0xFFFFFFE0,         // The appropriate mask should be applied when writing to
            .end_addr           = (MEMORY_SYSRAM_BASE + 0x200 - 1) | 0x1F  // The appropriate mask should be applied when writing to
     };
    _MPU_configuration(&region_cfg);


     /* Attempt to perform a write operation in a Read-only SRAM region */
     volatile uint32_t *ptr = (volatile uint32_t *) MEMORY_SYSRAM_BASE;
     *ptr = 0xFFFFFFFF;
}

