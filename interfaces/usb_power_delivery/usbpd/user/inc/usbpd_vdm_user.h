/**
  ******************************************************************************
  * @file    usbpd_vdm_user.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_vdm_user.c file
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

#ifndef __USBPD_VDM_USER_H_
#define __USBPD_VDM_USER_H_

#ifdef __cplusplus
 extern "C" {
#endif 

typedef struct
{
  uint32_t VDM_XID_SOP                      :32; /*!< A decimal number assigned by USB-IF before certification */
  uint32_t VDM_USB_VID_SOP                  :16; /*!< A unique 16-bit number, assigned to the Vendor by USB-IF. */
  uint32_t VDM_PID_SOP                      :16; /*!< A unique number assigned by the Vendor ID holder identifying the product. */
  uint32_t VDM_bcdDevice_SOP                :16; /*!< A unique number assigned by the Vendor ID holder containing identity information relevant to the release version of the product. */
  USBPD_ModalOp_TypeDef VDM_ModalOperation  : 1; /*!< Product support Modes based on @ref USBPD_ModalOp_TypeDef */
  USBPD_USBCapa_TypeDef VDM_USBHostSupport  : 1; /*!< Indicates whether the UUT is capable of enumerating USB Host */
  USBPD_USBCapa_TypeDef VDM_USBDeviceSupport: 1; /*!< Indicates whether the UUT is capable of enumerating USB Devices */
  USBPD_ProductType_TypeDef VDM_ProductTypeUFPorCP : 3; /*!< Product type UFP or CablePlug of the UUT based on @ref USBPD_ProductType_TypeDef */
#if defined(USBPD_REV30_SUPPORT)
  USBPD_ProductType_TypeDef VDM_ProductTypeDFP : 3; /*!< Product type DFP of the UUT based on @ref USBPD_ProductType_TypeDef */
  uint32_t Reserved3                        : 7; /*!< Reserved bits */
#else
  uint32_t Reserved3                        :10; /*!< Reserved bits */
#endif /* USBPD_REV30_SUPPORT */
} USBPD_VDM_SettingsTypeDef;

#ifdef _SVDM
/*
 * DisplayPort Status VDO
 */
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t   ConnectStatus  : 2;    /*!< Connect status : 00b ==  no (DFP|UFP)_D is connected or disabled.
                                           01b == DFP_D connected, 10b == UFP_D connected, 11b == both        */
    uint32_t   PowerLow       : 1;    /*!< Power low : 0 == normal or LPM disabled, 1 == DP disabled for LPM  */
    uint32_t   Enable         : 1;    /*!< Enabled : is DPout on/off.                                         */
    uint32_t   MultiFunction  : 1;    /*!< Multi-function preference : 0 == no pref, 1 == MF preferred        */
    uint32_t   USBConfig      : 1;    /*!< USB config : 0 == maintain current, 1 == switch to USB from DP     */
    uint32_t   ExitDPMode     : 1;    /*!< Exit DP Alt mode: 0 == maintain, 1 == exit                         */
    uint32_t   HPDState       : 1;    /*!< HPD state : 0 = HPD_LOW, 1 == HPD_HIGH                             */
    uint32_t   IRQ_HPD        : 1;    /*!< IRQ_HPD : 1 == irq arrived since last message otherwise 0.         */
    uint32_t   Reserved       : 22;   /*!< Reserved                                                           */
  }d;
}USBPD_DPStatus_TypeDef;

/*
 * DisplayPort Config capabilities
 */

typedef enum {
  MODE_DP_CONFIG_SELECT_USB,            /*!< Set configuration for USB.           */
  MODE_DP_CONFIG_SELECT_UFP_U_AS_DFP_D, /*!< Set configuration for UFP_U_AS_DFP_. */
  MODE_DP_CONFIG_SELECT_UFP_U_AS_UFP_D, /*!< Set configuration for UFP_U_AS_UFP_D.*/
  MODE_DP_CONFIG_SELECT_RESERVED,       /*!< Reserved                             */
} USBPD_MODE_DP_CONFIG_SELECT;


typedef union
{
  uint32_t d32;
  struct
  {
    USBPD_MODE_DP_CONFIG_SELECT   SelectConfiguration  : 2;    /*!< Selection configuration */
    uint32_t   Signaling                : 4;    /*!< Signaling for transport of DP protocol */
    uint32_t   Reserved1                : 2;    /*!< Reserved                               */
    uint32_t   UFP_U_Pin                : 8;    /*!< Configure UFP_U pin Assignement        */
    uint32_t   Reserved2                : 16;   /*!< Reserved                               */
  }d;
}USBPD_DPConfig_TypeDef;

/*
 * Structure to SVID received by the port partner
 */
typedef struct {
  uint32_t  NumSVIDs;             /*!< Number of received SVIDs                           */
  uint16_t  SVIDs[24];            /*!< List of received SVIDs                             */
  uint8_t   FlagAllSVIDReceived;  /*!< Flag to indicate that all SVIDs have been received */
}USBPD_SVIDPortPartnerInfo_TypeDef;

#endif

#if defined(_SVDM) || defined(_UVDM) || defined(_VCONN_SUPPORT)
USBPD_StatusTypeDef USBPD_VDM_UserInit(uint8_t PortNum);
#endif

#if defined(_SVDM )
void                USBPD_VDM_UserReset(uint8_t PortNum);
void                USBPD_VDM_FillDPStatus(uint8_t PortNum, USBPD_DPStatus_TypeDef *dp_status);
void                USBPD_VDM_FillDPConfig(uint8_t PortNum, USBPD_DPConfig_TypeDef *pDP_Config);
#endif

#endif /* __USBPD_VDM_USER_H_ */
