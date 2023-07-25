/**
 ****************************************************************************************
 *
 * @file misc.h
 *
 * @brief Miscellaneous functionality header file
 *
 * Copyright (C) 2018-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef MISC_H_
#define MISC_H_

#include <stdio.h>
#include "hw_gpio.h"

#ifndef DBG_PRINT_ENABLE
#define DBG_PRINT_ENABLE  ( 0 )
#endif

#ifndef DBG_IO_ENABLE
#define DBG_IO_ENABLE     ( 0 )
#endif

#ifndef DBG_CONST_NONE
#define DBG_CONST_NONE    ( 0 )
#endif

#if (DBG_CONST_NONE == 1)
# define __CONST
#else
# define __CONST    const
#endif /* DBG_CONST_NONE */

#if (DBG_PRINT_ENABLE == 1)
# define DBG_PRINTF(_f, args...)   printf((_f), ## args)
#else
# define DBG_PRINTF(_f, args...)
#endif /* DBG_PRINT_ENABLE == 1 */

#if (DBG_IO_ENABLE == 1)
# define DBG_IO_ON(_port, _pin) \
        { \
                hw_gpio_set_pin_function((_port), (_pin), HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO); \
                hw_gpio_pad_latch_enable((_port), (_pin)); \
                hw_gpio_set_active((_port), (_pin)); \
                hw_gpio_pad_latch_disable((_port), (_pin)); \
        }

# define DBG_IO_OFF(_port, _pin) \
        { \
                hw_gpio_set_pin_function((_port), (_pin), HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_GPIO); \
                hw_gpio_pad_latch_enable((_port), (_pin)); \
                hw_gpio_set_inactive((_port), (_pin)); \
                hw_gpio_pad_latch_disable((_port), (_pin)); \
        }

#else
# define DBG_IO_ON(_port, _pin)
# define DBG_IO_OFF(_port, _pin)
#endif /* DBG_IO_ENABLE == 1 */

/*
 * MCP4822 DAC Module Configuration Macros
 *********************************************************************************************************
 */

/* MCP4822 control register bits-masks */
#define MCP4822_AB_Msk           ( 0x8000 )
#define MCP4822_GA_Msk           ( 0x2000 )
#define MCP4822_SHDN_Msk         ( 0x1000 )
#define MCP4822_DATA_Msk         ( 0x0FFF )
#define MCP4822_CTRL_Msk         ( 0xF000 )

#define MCP4822_GET_MSK(x)       MCP4822_ ## x ## _Msk

/* MCP4822 control register bit-fields */
typedef enum {
        MCP4822_AB_CONTROL_BIT_RESET   = 0, /* Select DACA channel */
        MCP4822_GA_CONTROL_BIT_RESET   = 0, /* Output gain 1x */
        MCP4822_SHDN_CONTROL_BIT_RESET = 0, /* Shutdown the selected DAC channel  */
        MCP4822_AB_CONTROL_BIT_SET     = MCP4822_GET_MSK(AB),  /* Select DACB channel */
        MCP4822_GA_CONTROL_BIT_SET     = MCP4822_GET_MSK(GA),  /* Output gain 2x */
        MCP4822_SHDN_CONTROL_BIT_SET   = MCP4822_GET_MSK(SHDN) /* Activate the selected DAC channel */
} MCP4822_CONTROL_BITS;

/*
 * Set the MCP4822 2-byte register:
 *
 *          +------+-----+------+------+------+------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *  Bit:    |  15  |  14 |  13  |  12  |  11  |  10  |  9  |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 *          +------+-----+------+------+------+------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *  Func:   |  AB  |  -  |  GA  | SHDN |  D11 |  D10 |  D9 |  D8 |  D7 |  D6 |  D5 |  D4 |  D3 |  D2 |  D1 |  D0 |
 *          +------+-----+------+------+------+------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 **/
#define MCP4822_SET_REG(_val, _reg)     ( ((_val) & MCP4822_GET_MSK(DATA)) | ((_reg) & MCP4822_GET_MSK(CTRL)) )

#endif /* MISC_H_ */

