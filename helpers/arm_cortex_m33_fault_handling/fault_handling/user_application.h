/**
 ****************************************************************************************
 *
 * @file user_application.h
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

#ifndef FAULT_HANDLING_USER_APPLICATION_H_
#define FAULT_HANDLING_USER_APPLICATION_H_


/*
 * If set to '1', debugging messages are displayed on the serial console while
 * in the fault handler.
 * This macro is useful since the state of the system is unknown after a fault
 * event and any attempt to access a peripheral interface (e.g. UART) might
 * lead to failure or even system collapse.
 *
 * \note This macro has a meaning only if _FAULT_HANDLING_MODE is set to '2' and
 *       the UART block is enabled.
 **/
#define _FAULT_VERBOSE                 (0)

/*
 * There are two main flavors for handling fault exceptions:
 *
 * 1 --> CPU execution either is trapped in an infinite loop or halted via a breakpoint,
 *       waiting for the user to attach a debugger and perform a debugging session.
 *
 * 2 --> System's state is stored in a dedicated and retained memory region in SRAM and
 *       a HW reset is triggered in order for the system to recover. After reboot, user
 *       can examine the logged information and identify the cause of the fault.
 **/
#define _FAULT_HANDLING_MODE           (2)


/*
 * Function pointer prototype. Programmer can define their own
 * callback functions using that specific function prototype.
 */
typedef void (*FaultHandler_t)(void);


/* Trigger a BusFault exception */
void _trigger_BusFault(void);

/* Trigger an escalated fault exception (HardFault). */
void _trigger_escalated_BusFault(void);

/* Trigger a HardFault exception. */
void _trigger_HardFault(void);

/* Trigger a UsageFault exception. */
void _trigger_UsageFault(void);

/* Trigger a MemManage fault exception. */
void _trigger_MemFault(void);


/*
 * Function used for checking whether the device recovered from a fault or booted
 * normally. It should be called at the beginning of the application (e.g. inside
 * application tasks).
 */
void _check_if_system_booted_normally(void);


#endif /* FAULT_HANDLING_USER_APPLICATION_H_ */
