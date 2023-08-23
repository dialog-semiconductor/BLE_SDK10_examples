/**
******************************************************************************
* @file    usbpd_pwr_if.c
* @author  MCD Application Team
* @brief   This file contains power interface control functions.
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

#include "usbpd_pwr_if.h"
#include "usbpd_dpm_user.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_pdo_defs_val.h"
#include "usbpd_registers.h"
#include "usbpd_core.h"
#include "usbpd_stusb_dpm_if.h"
#include "string.h"

extern USBPD_ParamsTypeDef DPM_Params[USBPD_PORT_COUNT];

#if ((PORT0_NB_SOURCEPDO) > USBPD_MAX_NB_PDO)
#error "Nb of Source PDO/APDO is exceeding stack capabilities"
#endif
#if ((PORT0_NB_SINKPDO) > USBPD_MAX_NB_PDO)
#error "Nb of Sink PDO/APDO is exceeding stack capabilities"
#endif
#if USBPD_PORT_COUNT == 2
#if ((PORT1_NB_SOURCEPDO) > USBPD_MAX_NB_PDO)
#error "Nb of Source PDO/APDO is exceeding stack capabilities"
#endif
#if ((PORT1_NB_SINKPDO) > USBPD_MAX_NB_PDO)
#error "Nb of Sink PDO/APDO is exceeding stack capabilities"
#endif
#endif /* USBPD_PORT_COUNT == 2 */

#define _PWR_UPDATE_VOLTAGE_MIN(_PDO_VOLT_, _SNK_VOLT_) \
if ((_PDO_VOLT_) < (_SNK_VOLT_)) \
  { \
    /* Update min voltage */ \
      (_SNK_VOLT_) = (_PDO_VOLT_); \
  }
#define _PWR_UPDATE_VOLTAGE_MAX(_PDO_VOLT_, _SNK_VOLT_) \
if ((_PDO_VOLT_) > (_SNK_VOLT_)) \
  { \
    /* Update min voltage */ \
      (_SNK_VOLT_) = (_PDO_VOLT_); \
  }
#define _PWR_UPDATE_CURRENT_MAX(_PDO_CURRENT_, _SNK_CURRENT_) \
if ((_PDO_CURRENT_) > (_SNK_CURRENT_)) \
  { \
    /* Update min current */ \
      (_SNK_CURRENT_) = (_PDO_CURRENT_); \
  }
#define _PWR_UPDATE_POWER_MAX(_PDO_POWER_, _SNK_POWER_) \
if ((_PDO_POWER_) > (_SNK_POWER_)) \
  { \
    /* Update min POWER */ \
      (_SNK_POWER_) = (_PDO_POWER_); \
  }
#define _PWR_CHECK_VOLTAGE_MIN(_PDO_VOLT_, _SNK_VOLT_) \
if ((_PDO_VOLT_) < (_SNK_VOLT_)) \
{ \
  /* Disalignment between PDO and DPM_SNKRequestedPower structure */ \
    _status = USBPD_ERROR; \
}
#define _PWR_CHECK_VOLTAGE_MAX(_PDO_VOLT_, _SNK_VOLT_) \
if ((_PDO_VOLT_) > (_SNK_VOLT_)) \
{ \
  /* Disalignment between PDO and DPM_SNKRequestedPower structure */ \
    _status = USBPD_ERROR; \
}
#define _PWR_CHECK_CURRENT_MAX(_PDO_CURRENT_, _SNK_CURRENT_) \
if ((_PDO_CURRENT_) > (_SNK_CURRENT_)) \
{ \
  /* Disalignment between PDO and DPM_SNKRequestedPower structure */ \
    _status = USBPD_ERROR; \
}
#define _PWR_CHECK_POWER_MAX(_PDO_POWER_, _SNK_POWER_) \
if ((_PDO_POWER_) > (_SNK_POWER_)) \
{ \
  /* Disalignment between PDO and DPM_SNKRequestedPower structure */ \
    _status = USBPD_ERROR; \
}

/**** PDO ****/
USBPD_PWR_Port_PDO_Storage_TypeDef PWR_Port_PDO_Storage[USBPD_PORT_COUNT];

/* Functions to initialize Source PDOs */
uint32_t _PWR_SRCFixedPDO(float  _C_, float _V_,
			USBPD_CORE_PDO_PeakCurr_TypeDef _PK_,
			USBPD_CORE_PDO_DRDataSupport_TypeDef DRDSupport,
			USBPD_CORE_PDO_USBCommCapable_TypeDef UsbCommCapable,
			USBPD_CORE_PDO_ExtPowered_TypeDef ExtPower,
			USBPD_CORE_PDO_USBSuspendSupport_TypeDef UsbSuspendSupport,
			USBPD_CORE_PDO_DRPowerSupport_TypeDef DRPSupport);

uint32_t _PWR_SRCVariablePDO(float _MAXV_, float _MINV_, float _C_);

uint32_t _PWR_SRCBatteryPDO(float _MAXV_,float _MINV_,float _PWR_);
/* Functions to initialize Sink PDOs */

uint32_t _PWR_SNKFixedPDO(float  _C_, float _V_,
			USBPD_CORE_PDO_DRDataSupport_TypeDef DRDSupport,
			USBPD_CORE_PDO_USBCommCapable_TypeDef UsbCommCapable,
			USBPD_CORE_PDO_ExtPowered_TypeDef ExtPower,
			USBPD_CORE_PDO_HigherCapability_TypeDef HigherCapab,
			USBPD_CORE_PDO_DRPowerSupport_TypeDef DRPSupport);

uint32_t _PWR_SNKVariablePDO(float  _MAXV_,float _MINV_,float _C_);

uint32_t _PWR_SNKBatteryPDO(float _MAXV_,float _MINV_,float _PWR_);

/**
* @brief  Initialize structures and variables related to power board profiles
*         used by Sink and Source, for all available ports.
* @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_Init(void)
{
	USBPD_StatusTypeDef _status = USBPD_OK;

	/* Set links to PDO values and number for Port 0 (defined in PDO arrays in H file) */
	PWR_Port_PDO_Storage[USBPD_PORT_0].SinkPDO.ListOfPDO = (uint32_t *)PORT0_PDO_ListSNK;
	PWR_Port_PDO_Storage[USBPD_PORT_0].SinkPDO.NumberOfPDO = &USBPD_NbPDO[0];
	PWR_Port_PDO_Storage[USBPD_PORT_0].SourcePDO.ListOfPDO = NULL;
	PWR_Port_PDO_Storage[USBPD_PORT_0].SourcePDO.NumberOfPDO = &USBPD_NbPDO[1];
	_status |= USBPD_PWR_IF_CheckUpdateSNKPower(USBPD_PORT_0);

	#if USBPD_PORT_COUNT == 2
	/* Set links to PDO values and number for Port 1 (defined in PDO arrays in H file) */
	PWR_Port_PDO_Storage[USBPD_PORT_1].SinkPDO.ListOfPDO = NULL;
	PWR_Port_PDO_Storage[USBPD_PORT_1].SinkPDO.NumberOfPDO = &USBPD_NbPDO[2];
	PWR_Port_PDO_Storage[USBPD_PORT_1].SourcePDO.ListOfPDO = (uint32_t *) PORT1_PDO_ListSRC;
	PWR_Port_PDO_Storage[USBPD_PORT_1].SourcePDO.NumberOfPDO = &USBPD_NbPDO[3];
	#endif /* USBPD_PORT_COUNT == 2 */

	return _status;
}

/**
* @brief  Update PDO values
* @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_Update_PDO_Values(uint8_t PortNum, uint8_t NrOfPDO, uint32_t PDO_Mv, uint32_t PDO_Ma)
{
        uint32_t pdo =
	( ((PWR_A_10MA(PDO_Ma/1000.0)) << USBPD_PDO_SRC_FIXED_MAX_CURRENT_Pos) |
          ((PWR_V_50MV(PDO_Mv/1000.0)) << USBPD_PDO_SRC_FIXED_VOLTAGE_Pos) |
          //USBPD_PDO_SRC_FIXED_EXT_POWER_AVAILABLE |
          USBPD_PDO_TYPE_FIXED
        );

        if (NrOfPDO >= USBPD_MAX_NB_PDO)
        {
        	return USBPD_ERROR;
        }
	if (PortNum == USBPD_PORT_0)
	{
		// port 0 fixed sink
		PORT0_PDO_ListSNK[NrOfPDO] = pdo;
	}
	#if USBPD_PORT_COUNT == 2
	else
	{
		// port 1 fixed source
		PORT1_PDO_ListSRC[NrOfPDO] = pdo;
	}
	#endif /* USBPD_PORT_COUNT == 2 */
	return USBPD_OK;
}

/**
* @brief  Update PDO count
* @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_Update_PDO_Count(uint8_t PortNum, uint8_t NumberOfPDO)
{
        if (NumberOfPDO > USBPD_MAX_NB_PDO)
        {
        	return USBPD_ERROR;
        }
        if (PortNum == USBPD_PORT_0)
	{
        	USBPD_NbPDO[0] = NumberOfPDO;
	}
	else
	{
		USBPD_NbPDO[3] = NumberOfPDO;
	}
	return USBPD_OK;
}

/**
* @brief  Sets the required power profile, now it works only with Fixed ones
* @param  PortNum               Port number
* @param  Profile          Number of the required Power Data Objects
* @param  PreviousPowerProfile  Number of the previous required Power Data Objects
* @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_SetProfile(uint8_t PortNum, uint8_t Profile, uint8_t PreviousPowerProfile)
{
        uint32_t vbusTargetInmv;
        USBPD_PDO_TypeDef pdo;
        USBPD_StatusTypeDef ret = USBPD_OK;
        
        /* Check if valid port */
        if ( !USBPD_PORT_IsValid(PortNum) )
        {
          return USBPD_ERROR;
        }
        
        /* SRC Mode */
        if (USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole)
        {
          /* Check if profile nb is valid for this port */
          if (Profile >= *PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO)
          {
            return USBPD_ERROR;
          }
          if (0 == *PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO)
          {
            return USBPD_ERROR;
          }
          pdo.d32 = PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO[Profile];
          
          /* Case PDO is a fixed type */
          if (pdo.GenericPDO.PowerObject == USBPD_PDO_TYPE_FIXED)
          {
            /* Get the voltage in MV and set it */
            vbusTargetInmv = (pdo.SRCFixedPDO.VoltageIn50mVunits * 50);
            APPLI_SetVoltage(PortNum, vbusTargetInmv);
          }
        }
        return ret;
}

/**
* @brief  Reads the voltage and the current on a specified port
* @param  PortNum Port number
* @param  pVoltage: The Voltage in mV
* @param  pCurrent: The Current in mA
* @retval USBPD_ERROR or USBPD_OK
*/
USBPD_StatusTypeDef USBPD_PWR_IF_ReadVA(uint8_t PortNum, uint16_t *pVoltage, uint16_t *pCurrent)
{
	/* check for valid port */
	if (!USBPD_PORT_IsValid(PortNum))
	{
		return USBPD_ERROR;
	}

	/* USBPD_OK if at least one pointer is not null, otherwise USBPD_ERROR */
	USBPD_StatusTypeDef ret = USBPD_ERROR;

	/* Get values from HW_IF */
	if (pVoltage != NULL)
	{
		*pVoltage = APPLI_GetVBUS(PortNum);
		ret = USBPD_OK;
	}
	if (pCurrent != NULL)
	{
		*pCurrent = APPLI_GetIvbus(PortNum);
		ret = USBPD_OK;
	}
	return ret;
}
      
#if defined(_VCONN_SUPPORT)
/**
* @brief  Enables the VConn on the port.
* @param  PortNum Port number
* @param  CC      Specifies the CCx to be selected based on @ref CCxPin_TypeDef structure
* @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_Enable_VConn(uint8_t PortNum, CCxPin_TypeDef CC)
{
	USBPD_StatusTypeDef _status = USBPD_OK;
	#if defined(_TRACE)
	USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 1,(uint8_t *)"VCONN ON", 8);
	#endif /* _TRACE */

	DPM_Params[PortNum].VconnStatus = USBPD_TRUE;
	_status = (USBPD_StatusTypeDef)STUSB1602_Type_C_Control_Set(PortNum, PD_VCONN_SWAP_TURN_ON_VCONN_REQ);
	_status = USBPD_HW_IF_CheckVconnPresence(PortNum, 1) ;
	return _status;
}
      
/**
* @brief  Disable the VConn on the port.
* @param  PortNum Port number
* @param  CC      Specifies the CCx to be selected based on @ref CCxPin_TypeDef structure
* @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_Disable_VConn(uint8_t PortNum, CCxPin_TypeDef CC)
{
	USBPD_StatusTypeDef _status = USBPD_OK;
	#if defined(_TRACE)
	USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 1,(uint8_t *)"VCONN OFF", 9);
	#endif /* _TRACE */
	DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
	_status = (USBPD_StatusTypeDef)STUSB1602_Type_C_Control_Set(PortNum, PD_VCONN_SWAP_TURN_OFF_VCONN_REQ);
	( USBPD_HW_IF_CheckVconnPresence(PortNum, 1) ==  USBPD_ERROR ? USBPD_OK : USBPD_ERROR );
	return _status;
}
#endif /* _VCONN_SUPPORT */
      
/**
* @brief  Allow PDO data reading from PWR_IF storage.
* @param  PortNum Port number
* @param  DataId Type of data to be read from PWR_IF
*         This parameter can be one of the following values:
*           @arg @ref USBPD_CORE_DATATYPE_SRC_PDO Source PDO reading requested
*           @arg @ref USBPD_CORE_DATATYPE_SNK_PDO Sink PDO reading requested
* @param  Ptr Pointer on address where PDO values should be written (u8 pointer)
* @param  Size Pointer on nb of u32 written by PWR_IF (nb of PDOs)
* @retval None
*/
void USBPD_PWR_IF_GetPortPDOs(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t *Size)
{
        uint32_t   nbpdo, index = 0, nb_valid_pdo = 0;
        uint32_t   *ptpdoarray = NULL;
        USBPD_PDO_TypeDef pdo_first;
        
        // sanity check
        if (((PortNum == USBPD_PORT_0) && (DataId == USBPD_CORE_DATATYPE_SRC_PDO)) ||
            ((PortNum == USBPD_PORT_1) && (DataId == USBPD_CORE_DATATYPE_SNK_PDO)))
        {
        	printf("usbpd (%d): getting NULL pdo\r\n", PortNum);
        }

        /* Check if valid port */
        if (USBPD_PORT_IsValid(PortNum))
        {
          /* According to type of PDO to be read, set pointer on values and nb of elements */
          switch (DataId)
          {
          case USBPD_CORE_DATATYPE_SRC_PDO:
            nbpdo       = *PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO;
            ptpdoarray  = PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO;

            /* Save the 1st PDO */
            pdo_first.d32 = *ptpdoarray;
#if defined(USBPD_REV30_SUPPORT)
            /* Reset unchunked bit if current revision is PD2.0*/
            if (USBPD_SPECIFICATION_REV2 == DPM_Params[PortNum].PE_SpecRevision)
            {
              pdo_first.SRCFixedPDO.UnchunkedExtendedMessage  = USBPD_PDO_SRC_FIXED_UNCHUNK_NOT_SUPPORTED;
            }
#endif /* USBPD_REV30_SUPPORT */
            break;
          case USBPD_CORE_DATATYPE_SNK_PDO:
            nbpdo       = *PWR_Port_PDO_Storage[PortNum].SinkPDO.NumberOfPDO;
            ptpdoarray  = PWR_Port_PDO_Storage[PortNum].SinkPDO.ListOfPDO;

            /* Save the 1st PDO */
            pdo_first.d32 = *ptpdoarray;
#if defined(USBPD_REV30_SUPPORT)
            /* Reset FRS bit if current revision is PD2.0*/
            if (USBPD_SPECIFICATION_REV2 == DPM_Params[PortNum].PE_SpecRevision)
            {
              pdo_first.SNKFixedPDO.FastRoleSwapRequiredCurrent = USBPD_PDO_SNK_FIXED_FRS_NOT_SUPPORTED;
            }
#endif /* USBPD_REV30_SUPPORT */
            break;
          default:
            nbpdo = 0;
            break;
          }

          uint16_t cable_amps = USBPD_CORE_CABLE_3A;

          /* Copy PDO data in output buffer */
          for (index = 0; index < nbpdo; index++)
          {
            /* Copy 1st PDO as potentially FRS or UNCHUNKED bits have been reset */
            if (0 == index)
            {
              (void)memcpy(Ptr, (uint8_t*)&pdo_first.d32, 4u);
              nb_valid_pdo++;
            }
            else
            {
              USBPD_PDO_TypeDef localpdo = {0};
              /* Copy the next PDO */
              localpdo.d32 = *ptpdoarray;
#if  defined(_VCONN_SUPPORT)
                if ((0 == DPM_Ports[PortNum].VDM_DiscoCableIdentify.CableVDO_Presence)
                    || (VBUS_3A == DPM_Ports[PortNum].VDM_DiscoCableIdentify.CableVDO.b.VBUS_CurrentHandCap))
                {
                  cable_amps = USBPD_CORE_CABLE_3A;
                }
                else
                {
                  if (USBPD_PORTPOWERROLE_SRC ==DPM_Params[PortNum].PE_PowerRole)
                  {
                    if (VBUS_5A == DPM_Ports[PortNum].VDM_DiscoCableIdentify.CableVDO.b.VBUS_CurrentHandCap)
                    {
                      cable_amps = USBPD_CORE_CABLE_5A;
                    }
                  }
                }
#endif /* VCONN_SUPPORT */

                localpdo.SRCFixedPDO.MaxCurrentIn10mAunits = USBPD_MIN(localpdo.SRCFixedPDO.MaxCurrentIn10mAunits , cable_amps / 10);
                (void)memcpy((Ptr + (nb_valid_pdo * 4u)), (uint8_t*)&localpdo.d32, 4u);
                nb_valid_pdo++;
              }
            ptpdoarray++;
          }
        }
        /* Set nb of read PDO (nb of u32 elements); */
        *Size = nb_valid_pdo;
}

/**
* @brief  Find out SRC PDO pointed out by a position provided in a Request DO (from Sink).
* @param  PortNum Port number
* @param  RdoPosition RDO Position in list of provided PDO
* @param  Pdo Pointer on PDO value pointed out by RDO position (u32 pointer)
* @retval Status of search
*         USBPD_OK : Src PDO found for requested DO position (output Pdo parameter is set)
*         USBPD_FAIL : Position is not compliant with current Src PDO for this port (no corresponding PDO value)
*/
USBPD_StatusTypeDef USBPD_PWR_IF_SearchRequestedPDO(uint8_t PortNum, uint32_t RdoPosition, uint32_t *Pdo)
{
	if((RdoPosition == 0) || (RdoPosition > *PWR_Port_PDO_Storage[PortNum].SourcePDO.NumberOfPDO))
	{
		/* Invalid PDO index */
		return USBPD_FAIL;
	}
	*Pdo = PWR_Port_PDO_Storage[PortNum].SourcePDO.ListOfPDO[RdoPosition - 1];
	return USBPD_OK;
}

/**
* @brief  Function to check validity between SNK PDO and power user settings
* @param  PortNum Port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_PWR_IF_CheckUpdateSNKPower(uint8_t PortNum)
{
	USBPD_StatusTypeDef _status = USBPD_OK;
	USBPD_PDO_TypeDef pdo;
	uint32_t _max_power = 0;
	uint16_t _voltage = 0, _current = 0, _power = 0;
	uint16_t _min_voltage = 0xFFFF, _max_voltage = 0, _max_current = 0;

	for (uint32_t _index = 0; _index < *PWR_Port_PDO_Storage[PortNum].SinkPDO.NumberOfPDO; _index++)
	{
	  pdo.d32 = PWR_Port_PDO_Storage[PortNum].SinkPDO.ListOfPDO[_index];
	  switch (pdo.GenericPDO.PowerObject)
	  {
	  case USBPD_CORE_PDO_TYPE_FIXED:    /*!< Fixed Supply PDO                             */
	    _voltage = PWR_DECODE_50MV(pdo.SNKFixedPDO.VoltageIn50mVunits);
	    _PWR_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
	    _PWR_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
	    _current = PWR_DECODE_10MA(pdo.SNKFixedPDO.OperationalCurrentIn10mAunits);
	    _PWR_UPDATE_CURRENT_MAX(_current, _max_current);
	    break;
	  case USBPD_CORE_PDO_TYPE_BATTERY:  /*!< Battery Supply PDO                           */
	    _voltage = PWR_DECODE_50MV(pdo.SNKBatteryPDO.MinVoltageIn50mVunits);
	    _PWR_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
	    _voltage = PWR_DECODE_50MV(pdo.SNKBatteryPDO.MaxVoltageIn50mVunits);
	    _PWR_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
	    _power = PWR_DECODE_MW(pdo.SNKBatteryPDO.OperationalPowerIn250mWunits);
	    _PWR_UPDATE_POWER_MAX(_power, _max_power);
	    break;
	  case USBPD_CORE_PDO_TYPE_VARIABLE: /*!< Variable Supply (non-battery) PDO            */
	    _voltage = PWR_DECODE_50MV(pdo.SNKVariablePDO.MinVoltageIn50mVunits);
	    _PWR_UPDATE_VOLTAGE_MIN(_voltage, _min_voltage);
	    _voltage = PWR_DECODE_50MV(pdo.SNKVariablePDO.MaxVoltageIn50mVunits);
	    _PWR_UPDATE_VOLTAGE_MAX(_voltage, _max_voltage);
	    _current = PWR_DECODE_10MA(pdo.SNKVariablePDO.OperationalCurrentIn10mAunits);
	    _PWR_UPDATE_CURRENT_MAX(_current, _max_current);
	    break;
	  default:
	    break;
	  }
	}

	_PWR_CHECK_VOLTAGE_MIN(_min_voltage, DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.MinOperatingVoltageInmVunits);
	_PWR_CHECK_VOLTAGE_MAX(_max_voltage, DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.MaxOperatingVoltageInmVunits);
	_PWR_CHECK_CURRENT_MAX(_max_current, DPM_USER_Settings[PortNum].DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits);
	return _status;
}

/**
* @brief  Create SRC Fixed PDO object
* @param  _C_: Current in A
* @param  _V_: voltage in V
* @param  _PK_: The peak of current
* @param  DRDSupport: Data Role Swap support indication
* @param  UsbCommCapable: USB communications capability indication
* @param  ExtPower: Port externally powered indication
* @param  UsbSuspendSupport: USB Suspend support indication
* @param  DRPSupport: Dual Role Power support indication
* @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SRCFixedPDO(float  _C_, float _V_,
			USBPD_CORE_PDO_PeakCurr_TypeDef _PK_,
			USBPD_CORE_PDO_DRDataSupport_TypeDef DRDSupport,
			USBPD_CORE_PDO_USBCommCapable_TypeDef UsbCommCapable,
			USBPD_CORE_PDO_ExtPowered_TypeDef ExtPower,
			USBPD_CORE_PDO_USBSuspendSupport_TypeDef UsbSuspendSupport,
			USBPD_CORE_PDO_DRPowerSupport_TypeDef DRPSupport)
{
	USBPD_PDO_TypeDef pdo;

	pdo.d32 = 0;
	pdo.SRCFixedPDO.MaxCurrentIn10mAunits       = PWR_A_10MA(_C_);
	pdo.SRCFixedPDO.VoltageIn50mVunits          = PWR_V_50MV(_V_);
	pdo.SRCFixedPDO.PeakCurrent                 = _PK_;
	pdo.SRCFixedPDO.DataRoleSwap                = DRDSupport;
	pdo.SRCFixedPDO.USBCommunicationsCapable    = UsbCommCapable;
	pdo.SRCFixedPDO.ExternallyPowered           = ExtPower;
	pdo.SRCFixedPDO.USBSuspendSupported         = UsbSuspendSupport;
	pdo.SRCFixedPDO.DualRolePower               = DRPSupport;
	pdo.SRCFixedPDO.FixedSupply                 = USBPD_CORE_PDO_TYPE_FIXED;
	return pdo.d32;
}

/**
* @brief  Create SRC Variable PDO object
* @param  _MAXV_ Max voltage in V
* @param  _MINV_ Min voltage in V
* @param  _C_: Max current in A
* @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SRCVariablePDO(float _MAXV_, float _MINV_, float _C_)
{
	USBPD_PDO_TypeDef pdo;

	pdo.d32 = 0;
	pdo.SRCVariablePDO.MaxCurrentIn10mAunits = PWR_A_10MA(_C_);
	pdo.SRCVariablePDO.MaxVoltageIn50mVunits = PWR_V_50MV(_MAXV_);
	pdo.SRCVariablePDO.MinVoltageIn50mVunits = PWR_V_50MV(_MINV_);
	pdo.SRCVariablePDO.VariableSupply        = USBPD_CORE_PDO_TYPE_VARIABLE;
	return pdo.d32;
}

/**
* @brief  Create SRC Battery PDO object
* @param  _MAXV_ Max voltage in V
* @param  _MINV_ Min voltage in V
* @param  _PWR_ Max allowable power in W
* @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SRCBatteryPDO(float _MAXV_,float _MINV_,float _PWR_)
{
	USBPD_PDO_TypeDef pdo;

	pdo.d32 = 0;
	pdo.SRCBatteryPDO.MaxAllowablePowerIn250mWunits = PWR_W(_PWR_);
	pdo.SRCBatteryPDO.MinVoltageIn50mVunits         = PWR_V_50MV(_MINV_);
	pdo.SRCBatteryPDO.MaxVoltageIn50mVunits         = PWR_V_50MV(_MAXV_);
	pdo.SRCBatteryPDO.Battery                       = USBPD_CORE_PDO_TYPE_BATTERY;
	return pdo.d32;
}

/**
* @brief  Create SNK Fixed PDO object
* @param  _C_ Current in A
* @param  _V_ voltage in V
* @param  DRDSupport: Data Role Swap support indication
* @param  UsbCommCapable: USB communications capability indication
* @param  ExtPower: Port externally powered indication
* @param  HigherCapab: Sink needs more than vSafe5V to provide full functionality indication
* @param  DRPSupport: Dual Role Power support indication
* @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SNKFixedPDO(float  _C_, float _V_,
			USBPD_CORE_PDO_DRDataSupport_TypeDef DRDSupport,
			USBPD_CORE_PDO_USBCommCapable_TypeDef UsbCommCapable,
			USBPD_CORE_PDO_ExtPowered_TypeDef ExtPower,
			USBPD_CORE_PDO_HigherCapability_TypeDef HigherCapab,
			USBPD_CORE_PDO_DRPowerSupport_TypeDef DRPSupport)
{
	USBPD_PDO_TypeDef pdo;

	pdo.d32 = 0;
	pdo.SNKFixedPDO.OperationalCurrentIn10mAunits = PWR_A_10MA(_C_);
	pdo.SNKFixedPDO.VoltageIn50mVunits            = PWR_V_50MV(_V_);
	pdo.SNKFixedPDO.DataRoleSwap                  = DRDSupport;
	pdo.SNKFixedPDO.USBCommunicationsCapable      = UsbCommCapable;
	pdo.SNKFixedPDO.ExternallyPowered             = ExtPower;
	pdo.SNKFixedPDO.HigherCapability              = HigherCapab;
	pdo.SNKFixedPDO.DualRolePower                 = DRPSupport;
	pdo.SNKFixedPDO.FixedSupply                   = USBPD_CORE_PDO_TYPE_FIXED;

	return pdo.d32;
}

/**
* @brief  Create SNK Variable PDO object
* @param  _MAXV_ Max voltage in V
* @param  _MINV_ Min voltage in V
* @param  _C_: Max current in A
* @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SNKVariablePDO(float  _MAXV_,float _MINV_,float _C_)
{
	USBPD_PDO_TypeDef pdo;

	pdo.d32 = 0;
	pdo.SRCVariablePDO.MaxCurrentIn10mAunits = PWR_A_10MA(_C_);
	pdo.SRCVariablePDO.MaxVoltageIn50mVunits = PWR_V_50MV(_MAXV_);
	pdo.SRCVariablePDO.MinVoltageIn50mVunits = PWR_V_50MV(_MINV_);
	pdo.SRCVariablePDO.VariableSupply        = USBPD_CORE_PDO_TYPE_VARIABLE;
	return pdo.d32;
}

/**
* @brief  Create SNK Battery PDO object
* @param  _MAXV_ Max voltage in V
* @param  _MINV_ Min voltage in V
* @param  _PWR_ Max allowable power in W
* @retval PDO object value (expressed on u32)
*/
uint32_t _PWR_SNKBatteryPDO(float _MAXV_,float _MINV_,float _PWR_)
{
	USBPD_PDO_TypeDef pdo;

	pdo.d32 = 0;
	pdo.SRCBatteryPDO.MaxAllowablePowerIn250mWunits = PWR_W(_PWR_);
	pdo.SRCBatteryPDO.MinVoltageIn50mVunits         = PWR_V_50MV(_MINV_);
	pdo.SRCBatteryPDO.MaxVoltageIn50mVunits         = PWR_V_50MV(_MAXV_);
	pdo.SRCBatteryPDO.Battery                       = USBPD_CORE_PDO_TYPE_BATTERY;
	return pdo.d32;
}
