/**
  ******************************************************************************
  * @file    usbpd_pdo_defs_val.h
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

#ifndef __USBPD_PDO_DEF_VAL_H_
#define __USBPD_PDO_DEF_VAL_H_

#include "usbpd_pdo_defs.h"

uint8_t USBPD_NbPDO[4] = {(PORT0_NB_SINKPDO + PORT0_NB_SINKAPDO), 
                          ((PORT0_NB_SOURCEPDO + PORT0_NB_SOURCEAPDO)),
                          ((PORT1_NB_SINKPDO + PORT1_NB_SINKAPDO)),
			  ((PORT1_NB_SOURCEPDO + PORT1_NB_SOURCEAPDO))};

/* Definition of Sink PDO for Port 0 */
uint32_t PORT0_PDO_ListSNK[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */
        (((PWR_A_10MA(USBPD_SINK_REQ_CUR_PDO1_MA/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos) |
         ((PWR_V_50MV(USBPD_SINK_REQ_VOL_PDO1_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
          USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED |
          USBPD_PDO_TYPE_FIXED
        ),
 /* PDO 2 */
        (((PWR_A_10MA(USBPD_SINK_REQ_CUR_PDO2_MA/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos) |
         ((PWR_V_50MV(USBPD_SINK_REQ_VOL_PDO2_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
          USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED |
          USBPD_PDO_TYPE_FIXED
	),
  /* PDO 3 */
        (((PWR_A_10MA(USBPD_SINK_REQ_CUR_PDO3_MA/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos) |
         ((PWR_V_50MV(USBPD_SINK_REQ_VOL_PDO3_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
          USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED |
          USBPD_PDO_TYPE_FIXED
	),
  /* PDO 4 */
        (((PWR_A_10MA(USBPD_SINK_REQ_CUR_PDO4_MA/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos) |
         ((PWR_V_50MV(USBPD_SINK_REQ_VOL_PDO4_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
          USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED |
          USBPD_PDO_TYPE_FIXED
	),
  /* PDO 5 */
        (((PWR_A_10MA(USBPD_SINK_REQ_CUR_PDO5_MA/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos) |
         ((PWR_V_50MV(USBPD_SINK_REQ_VOL_PDO5_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
          USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED |
          USBPD_PDO_TYPE_FIXED
	),
  /* PDO 6 */
        (((PWR_A_10MA(USBPD_SINK_REQ_CUR_PDO6_MA/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos) |
         ((PWR_V_50MV(USBPD_SINK_REQ_VOL_PDO6_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
          USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED |
          USBPD_PDO_TYPE_FIXED
	),
  /* PDO 7 */
        (((PWR_A_10MA(USBPD_SINK_REQ_CUR_PDO7_MA/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos) |
         ((PWR_V_50MV(USBPD_SINK_REQ_VOL_PDO7_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
          USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED |
          USBPD_PDO_TYPE_FIXED
	),
};

#if  (USBPD_PORT_COUNT == 2)
/* Definition of Source PDO for Port 1 */
uint32_t PORT1_PDO_ListSRC[USBPD_MAX_NB_PDO] = 
{
  /* PDO 1 */ (0x00000000U),
  /* PDO 2 */ (0x00000000U),
  /* PDO 3 */ (0x00000000U),
  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};
#endif /*USBPD_PORT_COUNT == 2*/

#endif /* __USBPD_PDO_DEF_VAL_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
