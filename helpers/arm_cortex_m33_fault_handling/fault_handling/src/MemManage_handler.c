/**
 ****************************************************************************************
 *
 * @file MemManage_fault_handler.c
 *
 * Copyright (c) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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

#include <stdio.h>
#include <stdbool.h>

#include "hw_cpm.h"
#include "hw_watchdog.h"

#include "fault_handling.h"
#include "user_application.h"

/* External defined symbols */
extern volatile Fault_t _fault_exception_info __UNUSED;

/*
 * User-defined MemoryManagement handler. Programmer can define their own handlers
 * and override the default ones, provided by the SDK, on the premise that default
 * handlers are marked as "weak".
 * This is the 1st part of the handler written in pure assembly to trace the stacked
 * frame saved on exception entrance as well as other information.
 *
 * \note Since the state of the system is unknown after a fault, an attempt to
 *       access a peripheral device (e.g. FLASH) could lead to failure or even
 *       to system collapse. Hence, it's a good practice to place the handlers
 *       in SysRAM. Moreover, accessing code from SRAM is much faster compared
 *       to a non volatile device (e.g. FLASH or EEPROM) and quite often is
 *       critical for the system to react in a fault event as fast as possible.
 *
 */
#if (dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH)
__attribute__((section("text_retained")))
#endif
__attribute__((naked)) void MemManage_Handler(void)
{
        /* This is a naked assembly function (no function prologue and epilogue is generated) */
        __asm volatile
        (
                "tst lr, #4                                        \n" /* Determine which stack pointer (SP) contains the stacked frame. */
                "ite eq                                            \n"
                "mrseq r0, msp                                     \n" /* If bit 2 of LR (EXC_RETURN) is 0, MSP is the one that contains the stacked frame. (condition flag Z = 0) */
                "mrsne r0, psp                                     \n" /* If bit 2 of LR (EXC_RETURN) is 1, PSP is the one that contains the stackED frame. (condition flag Z = 1) */

                "tst lr, #16                                       \n" /* Check if the exception entrance process allocated space for FP context on the stacked frame. */
                "ite eq                                            \n"
                "moveq r1, #1                                      \n" /* If bit 4 of LR (EXC_RETURN) is 0, FP context is present on the stacked frame. */
                "movne r1, #2                                      \n" /* If bit 4 of LR (EXC_RETURN) is 1, FP context is not present on the stackED frame. */

                "ldr r2, _memfault_stack_trace                     \n"
                "bx r2                                             \n" /* Branch to C fault handler implementation */
                "_memfault_stack_trace: .word MemManage_HandlerC   \n"
        );
}



/*
 * User-defined Memory Management handler. This is the 2nd part of the
 * handler written in C language and used for extracting the contents
 * of the stacked frame.
 *
 * \note For the same reasons with the assembly implementation the code
 *       is placed in SRAM.
 *
 */
#if (dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH)
__attribute__((section("text_retained")))
#endif
void MemManage_HandlerC(uint32_t *fault_args, uint32_t fp_validity_flag)
{
        uint32_t mem_fault_addr __UNUSED;

        /* Get the value of Memory Management Fault Status Register (MMFSR) */
        __UNUSED uint8_t mem_fault_status_reg = _FLD2VAL(SCB_CFSR_MEMFAULTSR, SCB->CFSR);

#if (_FAULT_HANDLING_MODE == 1)
        hw_watchdog_freeze(); // Freeze WDOG

        ENABLE_DEBUGGER; // Enable debugging functionality in case it's disabled

        /*
         * Check if MemManage Fault Address Register (MMFAR) contains a valid address
         *
         * Bits  Name         Value
         * [7]   MMARVALID    Indicates if MMFAR contains a valid fault address
         *
         */
        if (mem_fault_status_reg & 0x80) {
                mem_fault_addr = SCB->MMFAR;
        }
        /*
         * This is the case where:
         *  - An imprecise fault exception tool place, that is, the exception was not triggered immediately after the fault.
         *  - The fault took place during stacking/ustacking process.
         */
        else {
                mem_fault_addr = 0x0;
        }

        /* If enabled, print out the stacked frame while in handler context */
        if (_FAULT_VERBOSE) {
                printf("\n\rMemManage Fault Handler\n\n\r---- Stacked Core Registers ----\n\n\r");
                printf("  - R0    = 0x%08lx\r\n", fault_args[0]);
                printf("  - R1    = 0x%08lx\r\n", fault_args[1]);
                printf("  - R2    = 0x%08lx\r\n", fault_args[2]);
                printf("  - R3    = 0x%08lx\r\n", fault_args[3]);
                printf("  - R12   = 0x%08lx\r\n", fault_args[4]);
                printf("  - LR    = 0x%08lx\r\n", fault_args[5]);
                printf("  - PC    = 0x%08lx\r\n", fault_args[6]);
                printf("  - xPSR  = 0x%08lx\r\n", fault_args[7]);


                /* FP context is not present in the stacked frame */
                if (fp_validity_flag == 2) {
                        printf("\n\r Floating Point (FP) context is not present in the Stack Frame -- \n\r");
                /* FP context is present in the stacked frame */
                } else if (fp_validity_flag == 1) {
                        for (int i = _STAKED_FRAME_FP_START_IDX; i <= _STAKED_FRAME_FP_STOP_IDX; i++) {
                                printf("  - S[%d]  = 0x%08lx\r\n", (i - _STAKED_FRAME_FP_START_IDX), fault_args[i]);
                        }
                        printf("  - FPSCR = 0x%08lx\r\n", fault_args[_STAKED_FRAME_FP_STOP_IDX+1]);
                }

                printf("\n\n\r******* Exception Number: %d *******\n\n\r", (uint8_t)__get_IPSR());

                printf("\n\r---- Fault Related Registers ----\n\n\r");
                printf("  - MMFSR = 0x%02x\r\n" , mem_fault_status_reg);
                printf("  - MMFAR = 0x%08lx\r\n", mem_fault_addr);
        }

        /* Check if debugger is already attached and halt CPU operation */
        if (REG_GETF(CRG_TOP, SYS_STAT_REG, DBG_IS_ACTIVE)) {
                __BKPT(0);
        }
        else {
                /* Otherwise, trap CPU execution in an infinite loop */
                while (1) {}
        }

#elif (_FAULT_HANDLING_MODE == 2)
        hw_watchdog_freeze(); // Freeze WDOG

        _fault_exception_info.magic_val = _FAULT_MAGIC_VALUE;
        _fault_exception_info.irq_num   = __get_IPSR();

        _fault_exception_info._r0   = fault_args[0];
        _fault_exception_info._r1   = fault_args[1];
        _fault_exception_info._r2   = fault_args[2];
        _fault_exception_info._r3   = fault_args[3];
        _fault_exception_info._r12  = fault_args[4];
        _fault_exception_info._lr   = fault_args[5];
        _fault_exception_info._pc   = fault_args[6];
        _fault_exception_info._xPSR = fault_args[7];

        if (fp_validity_flag == 2) {
                _fault_exception_info.is_fp_valid = false; // Update flag
                for (int i = 0; i < _STAKED_FRAME_FP_MAX_ENTRIES; i++) {
                        *((uint32_t *)&_fault_exception_info._s0 + (i*4)) = 0UL;
                }

        }
        else if (fp_validity_flag == 1) {
                _fault_exception_info.is_fp_valid = true; // Update flag
                for (int i = 0; i < _STAKED_FRAME_FP_MAX_ENTRIES; i++) {
                        *((uint32_t *)&_fault_exception_info._s0 + (i*4)) = fault_args[i+_STAKED_FRAME_FP_START_IDX];
                }
        }

        _fault_exception_info._CFSR._CFSR = SCB->CFSR;

        _fault_exception_info._HFSR  = SCB->HFSR;
        _fault_exception_info._DFSR  = SCB->DFSR;
        _fault_exception_info._AFSR  = SCB->AFSR;
        _fault_exception_info._MMFAR = SCB->MMFAR;
        _fault_exception_info._BFAR  = SCB->BFAR;

        hw_cpm_reboot_system(); // Configure WDOG to reboot the device when it reaches 0.

        /* Stick here waiting for WDOG to expire and trigger HW reset */
        while (1) {}

#endif
}


