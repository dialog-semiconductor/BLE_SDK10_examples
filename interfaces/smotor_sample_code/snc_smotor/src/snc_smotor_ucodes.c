/**
 ****************************************************************************************
 *
 * @file snc_smotor_ucodes.c
 *
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

#include "SeNIS.h"
#include "snc_hw_sys.h"
#include "snc_hw_gpio.h"

#include "snc_smotor_ucodes.h"
#include "peripheral_setup.h"
#include "../motor_sample_code.h"



/* Variable used for storing the status of the motor engine */
_SNC_RETAINED static uint32_t smotor_status = 0;


/* uCode-Block used for trigger the motor controller */
SNC_UCODE_BLOCK_DEF(_ucode_smotor_set_and_trigger)
{
        /* Clear the RTC PDC event */
        SNC_hw_sys_clear_rtc_pdc_event();


        /* Configure PG signals */
        SNC_hw_gpio_set_pin_function(HW_GPIO_PORT_0, PG0_SIG0, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_PG);
        SNC_hw_gpio_pad_latch_enable(HW_GPIO_PORT_0, PG0_SIG0);

        SNC_hw_gpio_set_pin_function(HW_GPIO_PORT_0, PG0_SIG1, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_PG);
        SNC_hw_gpio_pad_latch_enable(HW_GPIO_PORT_0, PG0_SIG1);

#if (SMOTOR_MIRRORING_MODE_ENABLE)
        SNC_hw_gpio_set_pin_function(HW_GPIO_PORT_0, PG0_SIG2, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_PG);
        SNC_hw_gpio_pad_latch_enable(HW_GPIO_PORT_0, PG0_SIG2);

        SNC_hw_gpio_set_pin_function(HW_GPIO_PORT_0, PG0_SIG3, HW_GPIO_MODE_OUTPUT, HW_GPIO_FUNC_PG);
        SNC_hw_gpio_pad_latch_enable(HW_GPIO_PORT_0, PG0_SIG3);
#endif



        SNC_STEP_BY_STEP_BEGIN(); /* Debugging */

        /* Trigger the motor controller manually. One or multiple commands will be executed. */
        SENIS_xor(da(&SMOTOR->SMOTOR_TRIGGER_REG), SMOTOR_SMOTOR_TRIGGER_REG_POP_CMD_Msk);


        /* Perform a delay expressed in low power clock */
        SENIS_del(1);

        /* Wait until the motor controller is not busy */
        SENIS_while(1)
        {
                /* Get the status of the motor controller */
                SENIS_assign(da(&smotor_status), da(&SMOTOR->SMOTOR_STATUS_REG));

                /* Poll PG #0 for as long as it outputs waves. The 5th bit of the status register should be zero. */
                SENIS_if(da(&smotor_status), NBIT, SMOTOR_SMOTOR_STATUS_REG_PG0_BUSY_Pos) {
                        /* Poll PG #1 for as long as it outputs waves. The 6th bit of the status register should be zero. */
                        SENIS_if(da(&smotor_status), NBIT, SMOTOR_SMOTOR_STATUS_REG_PG1_BUSY_Pos) {
                                SENIS_break; /* Exit from infinite loop */
                        }
                }
        }

        SNC_STEP_BY_STEP_END(); /* Debugging */


        /* Disable pad latches before leaving the uCode */
        SNC_hw_gpio_pad_latch_disable(HW_GPIO_PORT_0, PG0_SIG0);
        SNC_hw_gpio_pad_latch_disable(HW_GPIO_PORT_0, PG0_SIG1);

#if (SMOTOR_MIRRORING_MODE_ENABLE)
        SNC_hw_gpio_pad_latch_disable(HW_GPIO_PORT_0, PG0_SIG2);
        SNC_hw_gpio_pad_latch_disable(HW_GPIO_PORT_0, PG0_SIG3);
#endif

}
