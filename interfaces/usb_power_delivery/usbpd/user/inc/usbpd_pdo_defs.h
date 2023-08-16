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

#ifndef __USBPD_PDO_DEF_DRP_1PORT_H_
#define __USBPD_PDO_DEF_DRP_1PORT_H_

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/

/* Define   ------------------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
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

/* Exported define -----------------------------------------------------------*/
#if defined _GPIO_FOR_SRC
#define USBPD_PDP_SRC_IN_WATTS          27  /* SRC PD Power in Watts */
#else
#define USBPD_PDP_SRC_IN_WATTS          15  /* SRC PD Power in Watts */
#endif
#define USBPD_PDP_SNK_IN_WATTS          14 /* SINK PD Power in Watts */

/* Define board operating power and max power */
/* Request 5V */
#define USBPD_BOARD_REQUESTED_VOLTAGE_MV       5000
#define USBPD_BOARD_MIN_VOLTAGE_MV             5000
#define USBPD_BOARD_MAX_VOLTAGE_MV             20000
#define USBPD_BOARD_REQUESTED_9000_MV          9000
#define USBPD_BOARD_REQUESTED_20000_MV          20000

/* Max current */
#define USBPD_CORE_CABLE_3A 3000
#define USBPD_CORE_CABLE_5A 5000
#define USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT_5A 5000
#define USBPD_CORE_PDO_SRC_FIXED_2000   2000
/* 3000mA in source mode */
#define USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT   3000
/* 1500mA in sink mode */
#define USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT   1500
#define USBPD_CORE_PDO_SNK_FIXED_1500 1500
#define USBPD_CORE_PDO_SNK_FIXED_1200 1200
#define USBPD_CORE_PDO_SNK_APDO_1500 1500


/* Definitions of nb of PDO and APDO for each port */
#if _GPIO_FOR_SRC
#define PORT0_NB_SOURCEPDO         3   /* Number of Source PDOs (applicable for port 0)   */
#else
#define PORT0_NB_SOURCEPDO         1   /* Number of Source PDOs (applicable for port 0)   */
#endif
#define PORT0_NB_SINKPDO           2   /* Number of Sink PDOs (applicable for port 0)     */
#if  (USBPD_PORT_COUNT == 2)
#define PORT1_NB_SOURCEPDO         1   /* Number of Source PDOs (applicable for port 1)   */
#define PORT1_NB_SINKPDO           2   /* Number of Sink PDOs (applicable for port 1)     */
#else
#define PORT1_NB_SOURCEPDO         0   /* Number of Source PDOs (applicable for port 1)   */
#define PORT1_NB_SINKPDO           0   /* Number of Sink PDOs (applicable for port 1)     */
#endif /*USBPD_PORT_COUNT == 2*/


#define PORT0_NB_SOURCEAPDO        0   /* Number of Source APDOs (applicable for port 0)  */
#define PORT0_NB_SINKAPDO          0   /* Number of Sink APDOs (applicable for port 0)    */
#define PORT1_NB_SOURCEAPDO        0   /* Number of Source APDOs (applicable for port 1)  */
#define PORT1_NB_SINKAPDO          0   /* Number of Sink APDOs (applicable for port 1)    */

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
#ifndef __USBPD_PWR_IF_C

extern uint8_t USBPD_NbPDO[4];

extern uint32_t PORT0_PDO_ListSRC[USBPD_MAX_NB_PDO];
extern uint32_t PORT0_PDO_ListSNK[USBPD_MAX_NB_PDO];
#if  (USBPD_PORT_COUNT == 2)
extern uint32_t PORT1_PDO_ListSRC[USBPD_MAX_NB_PDO];
extern uint32_t PORT1_PDO_ListSNK[USBPD_MAX_NB_PDO];
#endif /*USBPD_PORT_COUNT == 2*/
#else
uint8_t USBPD_NbPDO[4] = {(PORT0_NB_SINKPDO + PORT0_NB_SINKAPDO), 
                          ((PORT0_NB_SOURCEPDO + PORT0_NB_SOURCEAPDO)),
                          ((PORT1_NB_SINKPDO + PORT1_NB_SINKAPDO)),
                          ((PORT1_NB_SOURCEPDO + PORT1_NB_SOURCEAPDO))};
/* Definition of Source PDO for Port 0 */
uint32_t PORT0_PDO_ListSRC[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */  
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT/1000.0)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(5)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                        |
          USBPD_PDO_SRC_FIXED_PEAKCURRENT_EQUAL                                                       |
#if defined(USBPD_REV30_SUPPORT)
#if defined(_UNCHUNKED_SUPPORT)
          USBPD_PDO_SRC_FIXED_UNCHUNK_SUPPORTED                                                      |
#else
          USBPD_PDO_SRC_FIXED_UNCHUNK_NOT_SUPPORTED                                                   |
#endif /* _UNCHUNKED_SUPPORT */
#endif /*USBPD_REV30_SUPPORT*/
#if defined(USBPD_USBDATA)
          USBPD_PDO_SRC_FIXED_DRD_SUPPORTED                                                           |
#else
          USBPD_PDO_SRC_FIXED_DRD_NOT_SUPPORTED                                                           |
#endif
#if defined(USBPD_USBDATA)
          USBPD_PDO_SRC_FIXED_USBCOMM_SUPPORTED                                                   |
#else
           USBPD_PDO_SRC_FIXED_USBCOMM_NOT_SUPPORTED                                                 |
#endif
          USBPD_PDO_SRC_FIXED_EXT_POWER_AVAILABLE                                                     |
          USBPD_PDO_SRC_FIXED_USBSUSPEND_NOT_SUPPORTED                                                |
          USBPD_PDO_SRC_FIXED_DRP_SUPPORTED                                                           |
          USBPD_PDO_TYPE_FIXED
        ),
#if  defined _GPIO_FOR_SRC
/* PDO 2*/
( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT/1000.0)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(9)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                        |
           USBPD_PDO_TYPE_FIXED
        ),
#else
  /* PDO 2 */ (0x00000000U),
#endif 
#if defined _GPIO_FOR_SRC
  /* PDO 3 */ ( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT/1000.0)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(15)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                        |
           USBPD_PDO_TYPE_FIXED
        ),
#else
  /* PDO 3 */ (0x00000000U),
#endif
  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};

/* Definition of Sink PDO for Port 0 */
uint32_t PORT0_PDO_ListSNK[USBPD_MAX_NB_PDO] =
{
  /* PDO 1 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos)    |
          ((PWR_V_50MV(USBPD_BOARD_REQUESTED_VOLTAGE_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
#if defined(USBPD_REV30_SUPPORT)
           USBPD_PDO_SNK_FIXED_FRS_NOT_SUPPORTED                                                     |
#endif /*USBPD_REV30_SUPPORT*/
#if defined(USBPD_USBDATA)
           USBPD_PDO_SNK_FIXED_DRD_SUPPORTED                                                         |
#else
           USBPD_PDO_SNK_FIXED_DRD_NOT_SUPPORTED                                                         |
#endif
#if defined(USBPD_USBDATA)
           USBPD_PDO_SNK_FIXED_USBCOMM_SUPPORTED                                                 |
#else
           USBPD_PDO_SNK_FIXED_USBCOMM_NOT_SUPPORTED                                                 |
#endif
           USBPD_PDO_SNK_FIXED_EXT_POWER_AVAILABLE                                                   |
           USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED                                             |
           USBPD_PDO_SNK_FIXED_DRP_SUPPORTED                                                         |
           USBPD_PDO_TYPE_FIXED
        ),

 /* PDO 2 : SNK */
		(((PWR_A_10MA(USBPD_CORE_PDO_SNK_FIXED_1500/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos)    |
          ((PWR_V_50MV(USBPD_BOARD_REQUESTED_9000_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos)|
		  USBPD_PDO_TYPE_FIXED
		  ),
  /* PDO 3 */ (0x00000000U),

  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};

#if  (USBPD_PORT_COUNT == 2)
/* Definition of Source PDO for Port 1 */
uint32_t PORT1_PDO_ListSRC[USBPD_MAX_NB_PDO] = 
{
  /* PDO 1 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SRC_FIXED_MAX_CURRENT/1000.0)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(5)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos)                                        |
          USBPD_PDO_SRC_FIXED_PEAKCURRENT_EQUAL                                                       |
#if defined(USBPD_REV30_SUPPORT)
#if defined(_UNCHUNKED_SUPPORT)
          USBPD_PDO_SRC_FIXED_UNCHUNK_SUPPORTED                                                       |
#else
          USBPD_PDO_SRC_FIXED_UNCHUNK_NOT_SUPPORTED                                                   |
#endif /* _UNCHUNKED_SUPPORT */
#endif /*USBPD_REV30_SUPPORT*/

          USBPD_PDO_SRC_FIXED_DRD_NOT_SUPPORTED                                                           |
          USBPD_PDO_SRC_FIXED_USBCOMM_NOT_SUPPORTED                                                   |
          USBPD_PDO_SRC_FIXED_EXT_POWER_AVAILABLE                                                     |
          USBPD_PDO_SRC_FIXED_USBSUSPEND_NOT_SUPPORTED                                                |
          USBPD_PDO_SRC_FIXED_DRP_SUPPORTED                                                           |
          USBPD_PDO_TYPE_FIXED
        ),
  /* PDO 2 */ (0x00000000U),
  /* PDO 3 */ (0x00000000U),
  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};

/* Definition of Sink PDO for Port 1 */
uint32_t PORT1_PDO_ListSNK[USBPD_MAX_NB_PDO] = 
{
  /* PDO 1 */
        ( ((PWR_A_10MA(USBPD_CORE_PDO_SNK_FIXED_MAX_CURRENT/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos)    |
          ((PWR_V_50MV(USBPD_BOARD_REQUESTED_VOLTAGE_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos) |
#if defined(USBPD_REV30_SUPPORT)
           USBPD_PDO_SNK_FIXED_FRS_NOT_SUPPORTED                                                     |
#endif /*USBPD_REV30_SUPPORT*/
		   USBPD_PDO_SNK_FIXED_DRD_NOT_SUPPORTED                                                     |
           USBPD_PDO_SNK_FIXED_USBCOMM_NOT_SUPPORTED                                                 |
           USBPD_PDO_SNK_FIXED_EXT_POWER_AVAILABLE                                                   |
           USBPD_PDO_SNK_FIXED_HIGHERCAPAB_SUPPORTED                                             |
           USBPD_PDO_SNK_FIXED_DRP_SUPPORTED                                                         |
           USBPD_PDO_TYPE_FIXED
        ),
 /* PDO 2 : SNK */
		(((PWR_A_10MA(USBPD_CORE_PDO_SNK_FIXED_1500/1000.0)) << USBPD_PDO_SNK_FIXED_OP_CURRENT_Pos)    |
          ((PWR_V_50MV(USBPD_BOARD_REQUESTED_9000_MV/1000.0)) << USBPD_PDO_SNK_FIXED_VOLTAGE_Pos)|
		  USBPD_PDO_TYPE_FIXED
		  ),
  /* PDO 3 */ (0x00000000U),
  /* PDO 4 */ (0x00000000U),
  /* PDO 5 */ (0x00000000U),
  /* PDO 6 */ (0x00000000U),
  /* PDO 7 */ (0x00000000U)
};
#endif /*USBPD_PORT_COUNT == 2*/

#endif /* __USBPD_PWR_IF_C */

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_PDO_DEF_DRP_1PORT_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
