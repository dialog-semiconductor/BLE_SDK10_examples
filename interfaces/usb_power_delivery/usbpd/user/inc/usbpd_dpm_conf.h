/**
  ******************************************************************************
  * @file    usbpd_dpm_conf.h
  * @author  MCD Application Team
  * @brief   Header file for stack/application settings file
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

#ifndef __USBPD_DPM_CONF_H_
#define __USBPD_DPM_CONF_H_

/* Includes ------------------------------------------------------------------*/
#include "usbpd_pdo_defs.h"
#if defined(_SVDM) || defined(_UVDM)
#include "usbpd_vdm_user.h"
#endif
#include "usbpd_dpm_user.h"

/* Define   ------------------------------------------------------------------*/
/* Define VID, PID,... manufacturer parameters */
#define USBPD_VID (0x0483u)     /*!< Vendor ID (assigned by the USB-IF)                     */
#define USBPD_PID (0x0002u)     /*!< Product ID (assigned by the manufacturer)              */
#define USBPD_XID (0x00000000)  /*!< Value provided by the USB-IF assigned to the product. Needs to be 0 before certification   */

#if defined (_STATUS)   
#define POWER_TYPE_DC      2u /* External DC Power */
#define POWER_TYPE_AC      6u /* External AC Power */
#define POWER_TYPE_BATTERY 8u /* Internal power from Battery*/
#define POWER_TYPE_NO_BATT 16u /*Internal Power from Non_Battery*/
#endif

extern USBPD_SettingsTypeDef      DPM_Settings[USBPD_PORT_COUNT];
extern USBPD_USER_SettingsTypeDef DPM_USER_Settings[USBPD_PORT_COUNT];
extern USBPD_IdSettingsTypeDef    DPM_ID_Settings[USBPD_PORT_COUNT]; 

#endif /* __USBPD_DPM_CONF_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
