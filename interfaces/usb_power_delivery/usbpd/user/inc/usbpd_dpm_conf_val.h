/**
  ******************************************************************************
  * @file    usbpd_dpm_conf_val.h
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

#ifndef __USBPD_DPM_CONF_VAL_H_
#define __USBPD_DPM_CONF_VAL_H_

/* Includes ------------------------------------------------------------------*/
#include "usbpd_dpm_conf.h"

/* __USBPD_DPM_CORE_C */
USBPD_SettingsTypeDef DPM_Settings[USBPD_PORT_COUNT] =
{
  {
#if  defined(_VCONN_SUPPORT)
    .PE_SupportedSOP = USBPD_SUPPORTED_SOP_SOP | USBPD_SUPPORTED_SOP_SOP1, /* Supported SOP : SOP, SOP' */
#else
    .PE_SupportedSOP = USBPD_SUPPORTED_SOP_SOP, /* Supported SOP : SOP, SOP' SOP" SOP'Debug SOP"Debug      */
#endif  /* GENERATOR__AUTHENTICATION__ */
#if defined(USBPD_REV30_SUPPORT)
    .PE_SpecRevision = USBPD_SPECIFICATION_REV3,/* spec revision value                                     */
#else
    .PE_SpecRevision = USBPD_SPECIFICATION_REV2,/* spec revision value                                     */
#endif /* USBPD_REV30_SUPPORT */
    .PE_DefaultRole = USBPD_PORTPOWERROLE_SNK,  /* Default port role                                       */
    .PE_RoleSwap = USBPD_FALSE,                 /* support port role swap                                  */
#if defined(_SVDM) || defined(_UVDM)
    .PE_VDMSupport = USBPD_TRUE,
#else
    .PE_VDMSupport = USBPD_FALSE,
#endif
#ifdef _SVDM
    .PE_RespondsToDiscovSOP = USBPD_TRUE,       /*!< Can respond successfully to a Discover Identity */
    .PE_AttemptsDiscovSOP = USBPD_TRUE,         /*!< Can send a Discover Identity */
#else
    .PE_RespondsToDiscovSOP = USBPD_FALSE,      /*!< Can respond successfully to a Discover Identity */
    .PE_AttemptsDiscovSOP = USBPD_FALSE,        /*!< Can send a Discover Identity */
#endif
    .PE_PingSupport = USBPD_FALSE,              /* support Ping (only for PD3.0)                           */
    .PE_CapscounterSupport = USBPD_TRUE,       /* support caps counter                                    */
    .CAD_RoleToggle = USBPD_FALSE,               /* cad role toggle                                         */
    .CAD_TryFeature = USBPD_FALSE,              /* cad try feature                                         */
    .CAD_AccesorySupport = USBPD_FALSE,         /* cas accessory support                                   */
#if defined(USBPD_REV30_SUPPORT)
    .PE_PD3_Support =                           /*!< PD3 SUPPORT FEATURE                                   */
    {
#if defined(_UNCHUNKED_SUPPORT)
      .d.PE_UnchunkSupport                = USBPD_TRUE,       /*!< Unchunked mode Support not supported   */
#else
      .d.PE_UnchunkSupport                = USBPD_FALSE,       /*!< Unchunked mode Support                */
#endif /* _UNCHUNKED_SUPPORT */
      .d.PE_FastRoleSwapSupport           = USBPD_FALSE,       /*!< Support fast role swap only spec revsion 3.0            */
      .d.Is_GetPPSStatus_Supported        = USBPD_FALSE,              /*!< PPS message supported or not by DPM */
      .d.Is_SrcCapaExt_Supported          = USBPD_FALSE,     /*!< Source_Capabilities_Extended message supported or not by DPM */
#if _ALERT
      .d.Is_Alert_Supported               = USBPD_TRUE,            /*!< Alert message supported or not by DPM */
#else
      .d.Is_Alert_Supported               = USBPD_FALSE,            /*!< Alert message supported or not by DPM */
#endif /* _ALERT */
#if _STATUS
      .d.Is_GetStatus_Supported           = USBPD_TRUE,           /*!< Status message supported or not by DPM (Is_Alert_Supported should be enabled) */
#else
      .d.Is_GetStatus_Supported           = USBPD_FALSE,           /*!< Status message supported or not by DPM (Is_Alert_Supported should be enabled) */
#endif /* _STATUS */
#if _MANU_INFO
      .d.Is_GetManufacturerInfo_Supported = USBPD_TRUE,        /*!< Manufacturer_Info message supported or not by DPM */
#else
      .d.Is_GetManufacturerInfo_Supported = USBPD_FALSE,        /*!< Manufacturer_Info message supported or not by DPM */
#endif /* _MANU_INFO */
      .d.Is_GetCountryCodes_Supported     = USBPD_FALSE,        /*!< Country_Codes message supported or not by DPM */
      .d.Is_GetCountryInfo_Supported      = USBPD_FALSE,        /*!< Country_Info message supported or not by DPM */
      .d.Is_SecurityRequest_Supported     = USBPD_FALSE,       /*!< Security_Response message supported or not by DPM */
      .d.Is_FirmUpdateRequest_Supported   = USBPD_FALSE,       /*!< Firmware update response message supported by PE */

    },
#else
    .reserved = 0,                              /* uint32_t reserved:16;                                   */
#endif /* USBPD_REV30_SUPPORT */

    .CAD_SRCToggleTime          = 40,                    /* uint8_t CAD_SRCToggleTime; */
    .CAD_SNKToggleTime          = 40,                    /* uint8_t CAD_SNKToggleTime; */
#if USBPD_PORT_COUNT >= 2
  },
  {
#if  defined(_VCONN_SUPPORT)
    .PE_SupportedSOP = USBPD_SUPPORTED_SOP_SOP | USBPD_SUPPORTED_SOP_SOP1, /* Supported SOP : SOP, SOP' */
#else
    .PE_SupportedSOP = USBPD_SUPPORTED_SOP_SOP, /* Supported SOP : SOP, SOP' SOP" SOP'Debug SOP"Debug      */
#endif  /* GENERATOR__AUTHENTICATION__ */
#if defined(USBPD_REV30_SUPPORT)
    .PE_SpecRevision = USBPD_SPECIFICATION_REV3,/* spec revision value                                     */
#else
    .PE_SpecRevision = USBPD_SPECIFICATION_REV2,/* spec revision value                                     */
#endif /* USBPD_REV30_SUPPORT */
    .PE_DefaultRole = USBPD_PORTPOWERROLE_SRC,  /* Default port role                                       */
    .PE_RoleSwap = USBPD_FALSE,                 /* support port role swap                                  */
#if defined(_SVDM) || defined(_UVDM)
    .PE_VDMSupport = USBPD_TRUE,
#else
    .PE_VDMSupport = USBPD_FALSE,
#endif
#ifdef _SVDM
    .PE_RespondsToDiscovSOP = USBPD_TRUE,       /*!< Can respond successfully to a Discover Identity */
    .PE_AttemptsDiscovSOP = USBPD_TRUE,         /*!< Can send a Discover Identity */
#else
    .PE_RespondsToDiscovSOP = USBPD_FALSE,      /*!< Can respond successfully to a Discover Identity */
    .PE_AttemptsDiscovSOP = USBPD_FALSE,        /*!< Can send a Discover Identity */
#endif
    .PE_PingSupport = USBPD_FALSE,              /* support Ping (only for PD3.0)                           */
    .PE_CapscounterSupport = USBPD_TRUE,       /* support caps counter                                    */
    .CAD_RoleToggle = USBPD_FALSE,               /* cad role toggle                                         */
    .CAD_TryFeature = USBPD_FALSE,              /* cad try feature                                         */
    .CAD_AccesorySupport = USBPD_FALSE,         /* cas accessory support                                   */
#if defined(USBPD_REV30_SUPPORT)
    .PE_PD3_Support =                           /*!< PD3 SUPPORT FEATURE                                   */
    {
#if defined(_UNCHUNKED_SUPPORT)
      .d.PE_UnchunkSupport                = USBPD_TRUE,       /*!< Unchunked mode Support not supported   */
#else
      .d.PE_UnchunkSupport                = USBPD_FALSE,       /*!< Unchunked mode Support                */
#endif /* _UNCHUNKED_SUPPORT */
      .d.PE_FastRoleSwapSupport           = USBPD_FALSE,       /*!< Support fast role swap only spec revsion 3.0            */
      .d.Is_GetPPSStatus_Supported        = USBPD_FALSE,              /*!< PPS message supported or not by DPM */
      .d.Is_SrcCapaExt_Supported          = USBPD_FALSE,     /*!< Source_Capabilities_Extended message supported or not by DPM */
#if _ALERT
      .d.Is_Alert_Supported               = USBPD_TRUE,            /*!< Alert message supported or not by DPM */
#else
      .d.Is_Alert_Supported               = USBPD_FALSE,            /*!< Alert message supported or not by DPM */
#endif /* _ALERT */
#if _STATUS
      .d.Is_GetStatus_Supported           = USBPD_TRUE,           /*!< Status message supported or not by DPM (Is_Alert_Supported should be enabled) */
#else
      .d.Is_GetStatus_Supported           = USBPD_FALSE,           /*!< Status message supported or not by DPM (Is_Alert_Supported should be enabled) */
#endif /* _STATUS */
#if _MANU_INFO
      .d.Is_GetManufacturerInfo_Supported = USBPD_TRUE,        /*!< Manufacturer_Info message supported or not by DPM */
#else
      .d.Is_GetManufacturerInfo_Supported = USBPD_FALSE,        /*!< Manufacturer_Info message supported or not by DPM */
#endif /* _MANU_INFO */
      .d.Is_GetCountryCodes_Supported     = USBPD_FALSE,        /*!< Country_Codes message supported or not by DPM */
      .d.Is_GetCountryInfo_Supported      = USBPD_FALSE,        /*!< Country_Info message supported or not by DPM */
      .d.Is_SecurityRequest_Supported     = USBPD_FALSE,       /*!< Security_Response message supported or not by DPM */
      .d.Is_FirmUpdateRequest_Supported   = USBPD_FALSE,       /*!< Firmware update response message supported by PE */
    },
#else
    .reserved = 0,                              /* uint32_t reserved:16;                                   */
#endif /* USBPD_REV30_SUPPORT */

    .CAD_SRCToggleTime          = 40,                    /* uint8_t CAD_SRCToggleTime; */
    .CAD_SNKToggleTime          = 40,                    /* uint8_t CAD_SNKToggleTime; */
#endif /* USBPD_PORT_COUNT >= 2 */
  }
};

USBPD_IdSettingsTypeDef          DPM_ID_Settings[USBPD_PORT_COUNT] =
{
  {
    .XID = USBPD_XID,     /*!< Value provided by the USB-IF assigned to the product   */
    .VID = USBPD_VID,     /*!< Vendor ID (assigned by the USB-IF)                     */
    .PID = USBPD_PID,     /*!< Product ID (assigned by the manufacturer)              */
  },
#if USBPD_PORT_COUNT >= 2
  {
    .XID = USBPD_XID,     /*!< Value provided by the USB-IF assigned to the product   */
    .VID = USBPD_VID,     /*!< Vendor ID (assigned by the USB-IF)                     */
    .PID = USBPD_PID,     /*!< Product ID (assigned by the manufacturer)              */
  }
#endif /* USBPD_PORT_COUNT >= 2 */
};
USBPD_USER_SettingsTypeDef DPM_USER_Settings[USBPD_PORT_COUNT] =
{
  {
    .DPM_SNKRequestedPower =                                             /*!< Requested Power by the sink board                                    */
    {
      .MaxOperatingCurrentInmAunits = USBPD_BOARD_MAX_CURRENT_MA,
      .OperatingVoltageInmVunits    = 0,
      .MaxOperatingVoltageInmVunits = USBPD_BOARD_MAX_VOLTAGE_MV,
      .MinOperatingVoltageInmVunits = USBPD_BOARD_MIN_VOLTAGE_MV,
      .OperatingPowerInmWunits       = 0,
      .MaxOperatingPowerInmWunits   = (USBPD_BOARD_MAX_POWER_MW)
    },
    .PE_DR_Swap_To_UFP = USBPD_FALSE,                  /* support data swap    */
    .PE_DR_Swap_To_DFP = USBPD_FALSE,                  /* support data swap    */
#ifdef _VCONN_SUPPORT
    .PE_VconnSwap = USBPD_TRUE,                 /* support VCONN swap   */
#else
    .PE_VconnSwap = USBPD_FALSE,                /* support VCONN swap   */
#endif /* _VCONN_SUPPORT */
#if defined(USBPD_REV30_SUPPORT)
#if _MANU_INFO
    .DPM_ManuInfoPort =                      /*!< Manufacturer information used for the port            */
    {
      .VID = USBPD_VID,                      /*!< Vendor ID (assigned by the USB-IF)        */
      .PID = USBPD_PID,                      /*!< Product ID (assigned by the manufacturer) */
      .ManuString = "STMicroelectronics",    /*!< Vendor defined byte array                 */
    },
#endif /* _MANU_INFO */
#endif /* USBPD_REV30_SUPPORT */
  },
#if USBPD_PORT_COUNT >= 2
  {
    .PE_DR_Swap_To_UFP = USBPD_FALSE,                  /* support data swap  */
    .PE_DR_Swap_To_DFP = USBPD_FALSE,
#ifdef _VCONN_SUPPORT
      .PE_VconnSwap = USBPD_TRUE,                 /* support VCONN swap                                  */
#else
      .PE_VconnSwap = USBPD_FALSE,                /* support VCONN swap                                  */
#endif /* _VCONN_SUPPORT */
#if defined(USBPD_REV30_SUPPORT)
    .DPM_SRCExtendedCapa =                        /*!< SRC Extended Capability           */
    {
      .VID = USBPD_VID,      /*!< Vendor ID (assigned by the USB-IF)                   */
      .PID = USBPD_PID,      /*!< Product ID (assigned by the manufacturer)            */
      .XID = USBPD_XID,      /*!< Value provided by the USB-IF assigned to the product */
      .FW_revision = 2,      /*!< Firmware version number                              */
      .HW_revision = 2,      /*!< Hardware version number                              */
      .Voltage_regulation= 0,/*!< Voltage Regulation                                   */
      .Holdup_time = 0,      /*!< Holdup Time                                          */
      .Compliance = 0,       /*!< Compliance                                           */
      .TouchCurrent = 0,     /*!< Touch Current                                        */
      .PeakCurrent1 = 0,     /*!< Peak Current1                                        */
      .PeakCurrent2 = 0,     /*!< Peak Current2                                        */
      .PeakCurrent3 = 0,     /*!< Peak Current3                                        */
      .Touchtemp = 0,        /*!< Touch Temp                                           */
      .Source_inputs = 0,    /*!< Source Inputs                                        */
      .NbBatteries = 0,      /*!< Number of Batteries/Battery Slots                    */
      .SourcePDP = (uint8_t)USBPD_SOURCE_MAX_POWER_W,       /*!< Source PDP  5V*3A     */
    },
#if _MANU_INFO
    .DPM_ManuInfoPort =                      /*!< Manufacturer information used for the port            */
    {
      .VID = USBPD_VID,                      /*!< Vendor ID (assigned by the USB-IF)        */
      .PID = USBPD_PID,                      /*!< Product ID (assigned by the manufacturer) */
      .ManuString = "STMicroelectronics",    /*!< Vendor defined byte array                 */
    },
#endif /* _MANU_INFO */
#endif /* USBPD_REV30_SUPPORT */
  }
#endif /* USBPD_PORT_COUNT >= 2 */
};

#endif /* __USBPD_DPM_CONF_VAL_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
