/**
 ****************************************************************************************
 *
 * @file fault_exception_analysis.h
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

#ifndef FAULT_EXCEPTION_ANALYSIS_H_
#define FAULT_EXCEPTION_ANALYSIS_H_

#include <stdbool.h>
#include "osal.h"


/*
 * This is a magic value written to a special memory location when a fault exception
 * takes place.
 *
 * \note Not intended to be changed by the user.
 *
 **/
#define _FAULT_MAGIC_VALUE             (0xAABBCCDD)


#define _STAKED_FRAME_FP_START_IDX     (8 )
#define _STAKED_FRAME_FP_STOP_IDX      (23)
#define _STAKED_FRAME_FP_MAX_ENTRIES   (16)


/* Configurable Fault Status Register */
typedef union {
        /* Bit fields can be accessed individually */
        struct {
           uint32_t _MMFSR : 8;
           uint32_t _BFSR  : 8;
           uint32_t _UFSR  : 16;
        } _xFSR;

        /* CFSR can be accessed as a unified register */
        uint32_t _CFSR;
} CFSR_t;


/* Structure that holds the CPU status following a fault exception */
typedef struct {
     /* Magic value */
     uint32_t magic_val;

     /* IRQ number */
     uint32_t irq_num;

     /* FP content validity */
     bool is_fp_valid;

     /* CPU registers stacked on exception entrance. */
     uint32_t _r0;
     uint32_t _r1;
     uint32_t _r2;
     uint32_t _r3;
     uint32_t _r12;
     uint32_t _lr;
     uint32_t _pc;
     uint32_t _xPSR;

     /* Floating Point (FP) stacked registers */
     uint32_t _s0;
     uint32_t _s1;
     uint32_t _s2;
     uint32_t _s3;
     uint32_t _s4;
     uint32_t _s5;
     uint32_t _s6;
     uint32_t _s7;
     uint32_t _s8;
     uint32_t _s9;
     uint32_t _s10;
     uint32_t _s11;
     uint32_t _s12;
     uint32_t _s13;
     uint32_t _s14;
     uint32_t _s15;
     uint32_t _FPSCR;

     /* CPU status registers (related to fault exceptions). */
     CFSR_t   _CFSR;
     uint32_t _HFSR;
     uint32_t _DFSR;
     uint32_t _AFSR;

     /* System address registers (related to fault exceptions).  */
     uint32_t _MMFAR;
     uint32_t _BFAR;
} Fault_t;



/* Bit fields definitions and descriptions */
#define   _UFSR_BIT_FIELD_DESCRIPTION                                                                                                                                      \
        "UFSR[0]          UNDEFINSTR        An undefined instruction error has occurred."                                                                                  , \
        "UFSR[1]          INVSTATE          An invalid core state took place. EPSR.T or EPSR.IT validity error has occurred (e.g. switching from Thumb to ARM state)."     , \
        "UFSR[2]          INVPC             Invalid PC flag - Attempt to do an exception with a bad EXC_RETURN value."                                                     , \
        "UFSR[3]          NOCP              Attempt to execute a co-processor instruction while not supported or enabled."                                                 , \
        "UFSR[4]          STKOF             A stack overflow error has occurred."                                                                                          , \
        "UFSR[5]          RESERVED"                                                                                                                                        , \
        "UFSR[6]          RESERVED"                                                                                                                                        , \
        "UFSR[7]          RESERVED"                                                                                                                                        , \
        "UFSR[8]          UNALIGNED         An unaligned access took place."                                                                                               , \
        "UFSR[9]          DIVBYZERO         An integer division by zero error has occurred."                                                                               , \
        "UFSR[10]         RESERVED"                                                                                                                                        , \
        "UFSR[11]         RESERVED"                                                                                                                                        , \
        "UFSR[12]         RESERVED"                                                                                                                                        , \
        "UFSR[13]         RESERVED"                                                                                                                                        , \
        "UFSR[14]         RESERVED"                                                                                                                                        , \
        "UFSR[15]         RESERVED"                                                                                                                                          \

#define _BFSR_BIT_FIELD_DESCRIPTION                                                                                                                                                                                    \
        "BFSR[0]          IACCVIOL          Instruction bus error - The fault is triggered only when the faulting instruction is issued."                                                                              , \
        "BFSR[1]          DACCVIOL          Precise data bus error - The stacked PC value points to the instruction that caused the fault."                                                                            , \
        "BFSR[2]          RESERVED"                                                                                                                                                                                    , \
        "BFSR[3]          UNSTKERR          Unstacking operation (for an exception return) has caused one or more access violations - This fault is chained to the handler and the original return is still present."  , \
        "BFSR[4]          STKERR            Stacking operation (for an exception entry) has caused one or more BusFults - Stacked frame might be incorrect."                                                           , \
        "BFSR[5]          LSPERR            A bus fault occurred during FP lazy state preservation."                                                                                                                   , \
        "BFSR[6]          RESERVED"                                                                                                                                                                                    , \
        "BFSR[7]          BFARVALID         BFAR holds a valid fault address - A precise exception took place."


#define _MMFSR_BIT_FIELD_DESCRITION                                                                                                                                                                                    \
        "MMFSR[0]         IACCVIOL          Instruction access violation - The processor attempted an instruction fetch from a region that does not permit execution."                                                 , \
        "MMFSR[1]         DACCVIOL          Data access violation - The stacked PC value points to the faulting instruction."                                                                                          , \
        "MMFSR[2]         RESERVED"                                                                                                                                                                                    , \
        "MMFSR[3]         MUNSTKERR         MemManage fault on unstacking operation has caused one or more access violations - This fault is chained to the handler and the original return is still present."         , \
        "MMFSR[4]         MSTKERR           Stacking operation (for an exception entry) has caused one or more BusFults - Stacked frame might be incorrect."                                                           , \
        "MMFSR[5]         MLSPERR           MemManage fault occurred during FP lazy state preservation."                                                                                                               , \
        "MMFSR[6]         RESERVED"                                                                                                                                                                                    , \
        "MMFSR[7]         MMARVALID         MMFAR holds a valid fault address - A precise exception took place."


#define _CORTEX_M33_IRQn_DEFINITION              \
        /* ARM Cortex-M33 exception numbering */ \
        "Thread Mode" ,                          \
        "Reset"       ,                          \
        "NMI"         ,                          \
        "HardFault"   ,                          \
        "MemManage"   ,                          \
        "BusFault"    ,                          \
        "UsageFault"  ,                          \
        "SecureFault" ,                          \
        "Reserved"    ,                          \
        "Reserved"    ,                          \
        "Reserved"    ,                          \
        "SVCall"      ,                          \
        "DebugMonitor",                          \
        "Reserved"    ,                          \
        "PendSV"      ,                          \
        "SysTick"

        /* Device-specific interrupts */

#endif /* FAULT_EXCEPTION_ANALYSIS_H_ */
