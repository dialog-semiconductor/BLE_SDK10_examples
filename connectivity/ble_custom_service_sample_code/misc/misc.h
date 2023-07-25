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

#endif /* MISC_H_ */

