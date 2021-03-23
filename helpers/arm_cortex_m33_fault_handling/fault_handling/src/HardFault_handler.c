/**
 ****************************************************************************************
 *
 * @file HardFault_handler.c
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
#include <stdbool.h>

#include "hw_cpm.h"
#include "hw_watchdog.h"

#include "fault_handling.h"
#include "user_application.h"

/* External defined symbols */
extern volatile Fault_t _fault_exception_info __UNUSED;

/*
 * User-defined HardFault handler. This is the 2nd part of the handler written in
 * C language and used for extracting the contents of the stacked frame. The 1st
 * part of the handler (assembly code) in declared in
 * startup\DA1469x\GCC\startup_da1469x.S
 *
 * \note For the same reasons with the assembly implementation the code
 *       is placed in SRAM.
 *
 */
#if (dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH)
__attribute__((section("text_retained")))
#endif
void HardFault_HandlerC(unsigned long *fault_args)
{
        uint32_t fp_validity_flag;

        /* First check if FP registers have been stacked in the stack frame */
        __asm volatile
        (
                "tst lr, #16                            \n" /* Check if exception entrance process allocated space on the stack for FP context. */
                "ite eq                                 \n"
                "moveq r4, #1                           \n" /* If bit 4 of LR (EXC_RETURN) is 0, FP context is present on the stacked frame. */
                "movne r4, #2                           \n" /* If bit 4 of LR (EXC_RETURN) is 1, FP context is not present on the stackED frame. */
                "mov %0, r4                             \n"

                : "=r" (fp_validity_flag) : : "r4"
        );


#if (_FAULT_HANDLING_MODE == 1)
                hw_watchdog_freeze(); // Freeze WDOG

                ENABLE_DEBUGGER;

                /* If enabled, print out the stacked frame while in handler context */
                if (_FAULT_VERBOSE) {

                        if (_FLD2VAL(SCB_HFSR_FORCED, SCB->HFSR)) {
                                printf("***** A forced HardFault generated due to escalation of a configurable fault ******\r\n\n");
                        }
                        if (_FLD2VAL(SCB_HFSR_VECTTBL, SCB->HFSR)) {
                                printf("***** HardFault on a Vector Table read during exception processing - The stacked PC value points to the instruction that was preempted by the exception ******\r\n\n");
                        }

                        printf("\n\rHardFault Handler\n\n\r---- Stacked Core Registers ----\n\n\r");
                        printf("  - R0   = 0x%08lx\r\n", fault_args[0]);
                        printf("  - R1   = 0x%08lx\r\n", fault_args[1]);
                        printf("  - R2   = 0x%08lx\r\n", fault_args[2]);
                        printf("  - R3   = 0x%08lx\r\n", fault_args[3]);
                        printf("  - R12  = 0x%08lx\r\n", fault_args[4]);
                        printf("  - LR   = 0x%08lx\r\n", fault_args[5]);
                        printf("  - PC   = 0x%08lx\r\n", fault_args[6]);
                        printf("  - xPSR = 0x%08lx\r\n", fault_args[7]);

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
                        printf("  - CFSR = 0x%08lx\n\r", (*(volatile uint32_t *)(0xE000ED28)));
                        printf("  - HFSR = 0x%08lx\n\r", (*(volatile uint32_t *)(0xE000ED2C)));
                        printf("  - DFSR = 0x%08lx\n\r", (*(volatile uint32_t *)(0xE000ED30)));
                        printf("  - AFSR = 0x%08lx\n\r", (*(volatile uint32_t *)(0xE000ED3C)));
                        printf("  - MMAR = 0x%08lx\n\r", (*(volatile uint32_t *)(0xE000ED34)));
                        printf("  - BFAR = 0x%08lx\n\r", (*(volatile uint32_t *)(0xE000ED38)));
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


