/**
  ******************************************************************************
  * @file    usbpd_dpm_core.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_dpm_core.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __USBPD_DPM_CORE_H_
#define __USBPD_DPM_CORE_H_

#if defined(USBPD_TCPM_MODULE_ENABLED)
#define TCPM_ALARMBOX_MESSAGES_MAX      (3U * USBPD_PORT_COUNT)
#endif /* USBPD_TCPM_MODULE_ENABLED */

USBPD_StatusTypeDef USBPD_DPM_InitCore(void);
USBPD_StatusTypeDef USBPD_DPM_InitOS(void);
USBPD_StatusTypeDef USBPD_DPM_StartOS(void);
void                USBPD_DPM_TimerCounter(void);

#endif /* __USBPD_DPM_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
