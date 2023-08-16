/**
  ******************************************************************************
  * @file    usbpd_timersserver.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of usbpd_timerserver.h.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#ifndef __USBPD_TIMERSSERVER_H_
#define __USBPD_TIMERSSERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_DEVICE
  * @{
  */

/** @addtogroup USBPD_DEVICE_TIMESERVER
  * @{
  */

/* Exported constants --------------------------------------------------------*/
typedef enum {
  TIM_PORT0_CRC,                /*!< TIMERSERVER CRC check on Port0 not used in STUSB1602 */
  TIM_PORT0_RETRY,              /*!< TIMERSERVER Retry on Port0 */
  TIM_PORT1_CRC,                /*!< TIMERSERVER CRC check on Port1 not used in STUSB1602 */
  TIM_PORT1_RETRY,              /*!< TIMERSERVER Retry on Port1 */
  TIM_PORT0_CA,                 /*!< TIMERSERVER CA check on Port0 used in STUSB1602 to controle Received byte duration for colision avoidance */
  TIM_PORT1_CA,                 /*!< TIMERSERVER CA check on Port1 used in STUSB1602 to controle Received byte duration for colision avoidance */
  TIM_MAX                       /*!< TIMERSERVER reachs TIM Max */
} TIM_identifier;

#define TIM_MAX_TIME 10000u /*time in us, means 10 ms */
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void    USBPD_TIM_Init(void);
void    USBPD_TIM_Start(TIM_identifier id, uint16_t us_time);
uint8_t USBPD_TIM_IsExpired(TIM_identifier id);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_TIMERSSERVER_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

