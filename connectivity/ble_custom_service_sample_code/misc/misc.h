/**
 ****************************************************************************************
 *
 * @file misc.h
 *
 * @brief Miscellaneous functionality header file
 *
 * Copyright (C) 2018-2021 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
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

