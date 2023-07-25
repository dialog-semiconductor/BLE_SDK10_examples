/**
 ****************************************************************************************
 *
 * @file peripheral_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2017-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef PERIPHERAL_SETUP_H_
#define PERIPHERAL_SETUP_H_


/* Pattern Generator (PG) signals are mapped on specific GPIO pins on DA14699 SoC */

/* PG #0 signals definition */
#define PG0_SIG0  HW_GPIO_PIN_26
#define PG0_SIG1  HW_GPIO_PIN_27
#define PG0_SIG2  HW_GPIO_PIN_28
#define PG0_SIG3  HW_GPIO_PIN_29

/* PG #1 signals definition */
#define PG1_SIG0  HW_GPIO_PIN_30
#define PG1_SIG1  HW_GPIO_PIN_31
#define PG1_SIG2  HW_GPIO_PIN_21
#define PG1_SIG3  HW_GPIO_PIN_24

/* PG #2 signals definition */
#define PG2_SIG0  HW_GPIO_PIN_17
#define PG2_SIG1  HW_GPIO_PIN_18
#define PG2_SIG2  HW_GPIO_PIN_19
#define PG2_SIG3  HW_GPIO_PIN_20

/* PG #3 signals definition */
#define PG3_SIG0  HW_GPIO_PIN_4
#define PG3_SIG1  HW_GPIO_PIN_3
#define PG3_SIG2  HW_GPIO_PIN_5
#define PG3_SIG3  HW_GPIO_PIN_0

/* PG #4 signals definition */
#define PG4_SIG0  HW_GPIO_PIN_2
#define PG4_SIG1  HW_GPIO_PIN_1
#define PG4_SIG2  HW_GPIO_PIN_8
#define PG4_SIG3  HW_GPIO_PIN_9


#endif /* PERIPHERAL_SETUP_H_ */
