/**
  ******************************************************************************
  * @file    usbpd_dpm_user.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_dpm_user.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef __USBPD_DPM_USER_VAL_H_
#define __USBPD_DPM_USER_VAL_H_

#include "usbpd_dpm_user.h"

USBPD_HandleTypeDef DPM_Ports[USBPD_PORT_COUNT] =
{
  {
    .DPM_Reserved = 0,
#if defined(USBPD_REV30_SUPPORT)
    .FlagSendGetSrcCapaExtended = 0,
    .DPM_TimerSRCExtendedCapa = 0,                  /*!< timer to request the extended capa                                   */
    .DPM_RcvSRCExtendedCapa = {0},                  /*!< SRC Extended Capability received by port partner                     */
#if _MANU_INFO
    .DPM_GetManufacturerInfo = {0},                 /*!< Get Manufacturer Info                                                */
#endif /* _MANU_INFO */
    .DPM_GetBatteryStatus = {0},                    /*!< Get Battery status                                                   */
    .DPM_GetBatteryCapability = {0},                /*!< Get Battery Capability                                               */
    .DPM_BatteryStatus = {0},                       /*!< Battery status                                                       */
#if _ALERT
    .DPM_TimerAlert = 0,                            /*!< Timer used to monitor current and trig an ALERT                      */
    .DPM_SendAlert = {0},                           /*!< Save the Alert sent to port partner                                  */
    .DPM_RcvAlert = {0},                            /*!< Save the Alert received by port partner                              */
#endif /* _ALERT */
#ifdef _SVDM
    .DPM_TimerDisco = 0,
#endif
    .DPM_TimerRetry_DRswap =0,
    .DPM_TimerRetry_PRswap =0,
#if _ADC_MONITORING
    .DPM_TimerADC = 0,
#endif
#endif /* USBPD_REV30_SUPPORT */
    .DPM_CurrentPDOInfo ={0},                       /*!< Store Current PDO info  from Source                                  */

#if USBPD_PORT_COUNT >= 2
  },
  {
    .DPM_Reserved = 0,
#if defined(USBPD_REV30_SUPPORT)
    .FlagSendGetSrcCapaExtended = 0,
    .DPM_TimerSRCExtendedCapa = 0,                  /*!< timer to request the extended capa                                   */
    .DPM_RcvSRCExtendedCapa = {0},                  /*!< SRC Extended Capability received by port partner                     */
#if _MANU_INFO
    .DPM_GetManufacturerInfo = {0},                 /*!< Get Manufacturer Info                                                */
#endif /* _MANU_INFO */
#if _BATTERY
    .DPM_GetBatteryStatus = {0},                    /*!< Get Battery status                                                   */
    .DPM_GetBatteryCapability = {0},                /*!< Get Battery Capability                                               */
    .DPM_BatteryStatus = {0},                       /*!< Battery status                                                       */
#endif /* _BATTERY */
#if _ALERT
    .DPM_TimerAlert = 0,                            /*!< Timer used to monitor current and trig an ALERT                      */
    .DPM_SendAlert = {0},                           /*!< Save the Alert sent to port partner                                  */
    .DPM_RcvAlert = {0},                            /*!< Save the Alert received by port partner                              */
#endif /* _ALERT */
#ifdef _SVDM
    .DPM_TimerDisco = 0,
#endif
    .DPM_TimerRetry_PRswap =0,
    .DPM_TimerRetry_DRswap =0,
#if _ADC_MONITORING
    .DPM_TimerADC = 0,
#endif
#endif /* USBPD_REV30_SUPPORT */
    .DPM_CurrentPDOInfo ={0},                       /*!< Store Current PDO info  from Source                                  */
  }
#else
  }
#endif /*USBPD_PORT_COUNT >= 2*/
};

#endif /* __USBPD_DPM_USER_VAL_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
