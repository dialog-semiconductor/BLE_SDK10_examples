/**
  ******************************************************************************
  * @file    usbpd_pdo_defs.h
  * @author  MCD Application Team
  * @brief   Header file for definition of PDO/APDO values for 1 ports(DRP) configuration
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

#ifndef __USBPD_PDO_DEF_H_
#define __USBPD_PDO_DEF_H_

#include "usbpd_def.h"

/**
  * @brief  USBPD Port PDO Structure definition
  *
  */
typedef struct
{
  uint32_t *ListOfPDO;                          /*!< Pointer on Power Data Objects list, defining
                                                     port capabilities */
  uint8_t  *NumberOfPDO;                         /*!< Number of Power Data Objects defined in ListOfPDO
                                                     This parameter must be set at max to @ref USBPD_MAX_NB_PDO value */
} USBPD_PortPDO_TypeDef;

/**
  * @brief  USBPD Port PDO Storage Structure definition
  */
typedef struct
{
  USBPD_PortPDO_TypeDef    SourcePDO;        /*!< SRC Power Data Objects */
  USBPD_PortPDO_TypeDef    SinkPDO;          /*!< SNK Power Data Objects */
}USBPD_PWR_Port_PDO_Storage_TypeDef;

/* Board operating power and max power */
#define USBPD_BOARD_MIN_VOLTAGE_MV		5000
#define USBPD_BOARD_MAX_VOLTAGE_MV		20000
#define USBPD_BOARD_MAX_CURRENT_MA		5000
#define USBPD_BOARD_MAX_POWER_MW		100000

/* Core config */
#define USBPD_CORE_CABLE_3A			3000
#define USBPD_CORE_CABLE_5A			5000

/* Sink config */
#define USBPD_SINK_REQ_VOL_PDO1_MV		5000
#define USBPD_SINK_REQ_CUR_PDO1_MA		2000
#define USBPD_SINK_REQ_VOL_PDO2_MV		9000
#define USBPD_SINK_REQ_CUR_PDO2_MA		2000
#define USBPD_SINK_REQ_VOL_PDO3_MV		12000
#define USBPD_SINK_REQ_CUR_PDO3_MA		1666
#define USBPD_SINK_REQ_VOL_PDO4_MV		15000
#define USBPD_SINK_REQ_CUR_PDO4_MA		3000
#define USBPD_SINK_REQ_VOL_PDO5_MV		20000
#define USBPD_SINK_REQ_CUR_PDO5_MA		2250
#define USBPD_SINK_REQ_VOL_PDO6_MV		20000
#define USBPD_SINK_REQ_CUR_PDO6_MA		3250
#define USBPD_SINK_REQ_VOL_PDO7_MV		20000
#define USBPD_SINK_REQ_CUR_PDO7_MA		5000

/* Source config */
#define USBPD_SOURCE_MAX_POWER_W		100

/* Definitions of nb of PDO and APDO for each port */
#define PORT0_NB_SOURCEPDO         0   /* Number of Source PDOs (applicable for port 0)   */
#define PORT0_NB_SINKPDO           7   /* Number of Sink PDOs (applicable for port 0)     */
#if  (USBPD_PORT_COUNT == 2)
#define PORT1_NB_SOURCEPDO         0   /* Number of Source PDOs (applicable for port 1)   */
#define PORT1_NB_SINKPDO           0   /* Number of Sink PDOs (applicable for port 1)     */
#else
#define PORT1_NB_SOURCEPDO         0   /* Number of Source PDOs (applicable for port 1)   */
#define PORT1_NB_SINKPDO           0   /* Number of Sink PDOs (applicable for port 1)     */
#endif /*USBPD_PORT_COUNT == 2*/


#define PORT0_NB_SOURCEAPDO        0   /* Number of Source APDOs (applicable for port 0)  */
#define PORT0_NB_SINKAPDO          0   /* Number of Sink APDOs (applicable for port 0)    */
#define PORT1_NB_SOURCEAPDO        0   /* Number of Source APDOs (applicable for port 1)  */
#define PORT1_NB_SINKAPDO          0   /* Number of Sink APDOs (applicable for port 1)    */

extern uint8_t USBPD_NbPDO[4];

extern uint32_t PORT0_PDO_ListSRC[USBPD_MAX_NB_PDO];
extern uint32_t PORT0_PDO_ListSNK[USBPD_MAX_NB_PDO];
#if  (USBPD_PORT_COUNT == 2)
extern uint32_t PORT1_PDO_ListSRC[USBPD_MAX_NB_PDO];
extern uint32_t PORT1_PDO_ListSNK[USBPD_MAX_NB_PDO];
#endif /*USBPD_PORT_COUNT == 2*/

#endif /* __USBPD_PDO_DEF_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
