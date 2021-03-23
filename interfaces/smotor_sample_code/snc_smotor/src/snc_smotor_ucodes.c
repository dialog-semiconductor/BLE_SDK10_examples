/**
 ****************************************************************************************
 *
 * @file snc_smotor_ucodes.c
 *
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
