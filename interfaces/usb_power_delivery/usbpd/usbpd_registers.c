/**
  **************************************************************************************************
  * @file    usbpd_registers.c
  * @author  System Lab - Sensing & Connectivity Application Team
  * @brief   This file provides a set of functions needed to manage the STUSB1602 Driver.
  **************************************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistributions of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  *    may be used to endorse or promote products derived from this software
  *    without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  **************************************************************************************************
  */

#include "usbpd_def.h"
#include "usbpd_registers.h"
#include "usbpd_peripherals.h"

/**
  * @brief  Check registers from 0x0B and 0x0C
  * @param  Addr I2C address of port controller device 
  * @retval return Alert status 
  */ 
STUSB1602_ALERT_STATUS_RegTypeDef STUSB1602_Alert_Raise_Get(uint8_t Addr)
{
    uint8_t Data[2];
    STUSB1602_ALERT_STATUS_RegTypeDef Value;

    STUSB1602_ReadReg(&Data[0], Addr, STUSB1602_ALERT_STATUS_REG, 2); 
    
    Value.d8 = Data[0] & ~Data[1] ;
    return Value;
}


/**
  * @brief  STUSB1602 Sets CC_Detect_Alrt_Int_Mask (Bit6 0x0C -- R/W)
  * @param  Addr I2C address of port controller device
  * @param  st Status to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */ 
STUSB1602_StatusTypeDef STUSB1602_CC_Detect_Alrt_Int_Mask_Set(uint8_t Addr, CC_Detect_Alrt_Int_Mask_Status_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  

    STUSB1602_ALERT_STATUS_MASK_reg_TypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_ALERT_STATUS_MASK_REG, 1); 

    reg.b.CC_DETECTION_STATUS_AL_MASK = st;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_ALERT_STATUS_MASK_REG, 1);

    return status;
}


/**
  * @brief  STUSB1602 Checks Monitoring_Status_Alrt_Int_Mask (Bit5 0x0C -- R/W)
  * @param  Addr I2C address of port controller device
  * @retval Monitor_Alrt_Int_Mask_Status_TypeDef
  */ 
Monitor_Alrt_Int_Mask_Status_TypeDef STUSB1602_Monitoring_Status_Alrt_Int_Mask_Get(uint8_t Addr)
{
    STUSB1602_ALERT_STATUS_MASK_reg_TypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_ALERT_STATUS_MASK_REG, 1); 

    return (Monitor_Alrt_Int_Mask_Status_TypeDef)(reg.b.MONITORING_STATUS_AL_MASK);
}


/**
  * @brief  STUSB1602 Sets Monitoring_Status_Alrt_Int_Mask (Bit5 0x0C -- R/W)
  * @param  Addr I2C address of port controller device
  * @param  st Status to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */ 
STUSB1602_StatusTypeDef STUSB1602_Monitoring_Status_Alrt_Int_Mask_Set(uint8_t Addr, Monitor_Alrt_Int_Mask_Status_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  

    STUSB1602_ALERT_STATUS_MASK_reg_TypeDef reg;    
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_ALERT_STATUS_MASK_REG, 1); 

    reg.b.MONITORING_STATUS_AL_MASK = st;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_ALERT_STATUS_MASK_REG, 1);

    return status;
}


/**
  * @brief  STUSB1602 Checks HW_Fault_Alrt_Int_Mask (Bit4 0x0C -- R/W)
  * @param  Addr I2C address of port controller device
  * @retval HW_Fault_Alrt_Int_Mask_Status_TypeDef
  */ 
HW_Fault_Alrt_Int_Mask_Status_TypeDef STUSB1602_HW_Fault_Alrt_Int_Mask_Get(uint8_t Addr)
{
    STUSB1602_ALERT_STATUS_MASK_reg_TypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_ALERT_STATUS_MASK_REG, 1); 

    return (HW_Fault_Alrt_Int_Mask_Status_TypeDef)(reg.b.HW_FAULT_STATUS_AL_MASK);
}


/**
  * @brief  STUSB1602 Sets HW_Fault_Alrt_Int_Mask (Bit4 0x0C -- R/W)
  * @param  Addr I2C address of port controller device
  * @param  st Status to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */ 
STUSB1602_StatusTypeDef STUSB1602_HW_Fault_Alrt_Int_Mask_Set(uint8_t Addr, HW_Fault_Alrt_Int_Mask_Status_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  

    STUSB1602_ALERT_STATUS_MASK_reg_TypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_ALERT_STATUS_MASK_REG, 1); 

    reg.b.HW_FAULT_STATUS_AL_MASK = st;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_ALERT_STATUS_MASK_REG, 1);

    return status;
}


/* REG_0x0D_CC_DETECTION_STATUS_TRANS *****************************************/

/**
  * @brief  STUSB1602 Checks Attach State Transition Reg (Bit0 0x0D -- RC)
  * @param  Addr I2C address of port controller device
  * @retval Attach_State_Trans_TypeDef 
  */ 
Attach_State_Trans_TypeDef STUSB1602_Attach_State_Trans_Get(uint8_t Addr)
{
    STUSB1602_CC_DETECTION_STATUS_TRANS_RegTypeDef reg;

    /* Check if a Transition occurred on ATTACH_STATE bit */
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_DETECTION_STATUS_TRANS_REG, 1); 

    return (Attach_State_Trans_TypeDef)(reg.b.ATTACH_STATE_TRANS);
}

/**
  * @brief  It enables or disables the notification of the CC Detection interrupt on ALERT pin
  * @param  PortNum The port index
  * @param  status Two allowed values: GPIO_PIN_RESET, GPIO_PIN_SET
  * @retval None
  */ 
void HW_IF_STUSB1602_Interrupt_CC_Detection(uint8_t PortNum, FunctionalState status)
{
  if (status == ENABLE)
  {
    STUSB1602_CC_Detect_Alrt_Int_Mask_Set(PortNum, CC_Detect_Int_UNMASKED);
  }
  else
  {
    STUSB1602_CC_Detect_Alrt_Int_Mask_Set(PortNum, CC_Detect_Int_MASKED);
  }
}

/**
* @brief  It enables or disables the notification of the Monitoringinterrupt on ALERT pin
* @param  PortNum The port index
* @param  status Two allowed values: GPIO_PIN_RESET, GPIO_PIN_SET
* @retval None
*/ 
void HW_IF_STUSB1602_Interrupt_Monitoring(uint8_t PortNum, FunctionalState status)
{
  if (status == ENABLE)
  {
    STUSB1602_Monitoring_Status_Alrt_Int_Mask_Set(PortNum, Monitor_Status_Int_UNMASKED);
  }
  else
  {
    STUSB1602_Monitoring_Status_Alrt_Int_Mask_Set(PortNum, Monitor_Status_Int_MASKED);
  }
}


   

/* REG_0x0E_CC_DETECTION_STATUS REG *******************************************/

 /** @addtogroup REG_0x0E_CC_DETECTION_STATUS
  * @brief  STUSB1602 Checks CC_DETECTION_STATUS REG (0x0E -- RO)
   * @{
   */

   
 /**
  * @brief  STUSB1602 CC Detection Status  (Register 0x0E -- RO)
  * @param  Addr I2C address of port controller device
  * @retval STUSB1602_CC_DETECTION_STATUS_RegTypeDef
  */ 
STUSB1602_CC_DETECTION_STATUS_RegTypeDef STUSB1602_CC_Detection_Status_Get(uint8_t Addr)
{
    STUSB1602_CC_DETECTION_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_DETECTION_STATUS_REG, 1); 

    return (reg);
}
 

/**
  * @brief  STUSB1602 Checks the DATA ROLE (bit2 0x0E -- RO)
  * @param  Addr I2C address of port controller device
  * @retval Data_Role_TypeDef
  */ 
Data_Role_TypeDef STUSB1602_Data_Role_Get(uint8_t Addr)
{
    STUSB1602_CC_DETECTION_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_DETECTION_STATUS_REG, 1); 

    return (Data_Role_TypeDef)(reg.b.CC_DATA_ROLE);
}


/**
  * @brief STUSB1602 checks the entire Monitoring_Status_Trans_Reg (bit0-7 0x0F -- RC)
  * @param Addr I2C address of port controller device 
  * @retval STUSB1602_MONITORING_STATUS_TRANS_RegTypeDef
  */ 
STUSB1602_MONITORING_STATUS_TRANS_RegTypeDef STUSB1602_Monitoring_Status_Trans_Reg_Get(uint8_t Addr)
{
    STUSB1602_MONITORING_STATUS_TRANS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_MONITORING_STATUS_TRANS_REG, 1); 

    return (reg);
}


/**
  * @brief STUSB1602 Read Monitory status reg
  * @param Addr I2C address of port controller device  
  * @retval STUSB1602_MONITORING_STATUS_RegTypeDef
  */ 
STUSB1602_MONITORING_STATUS_RegTypeDef STUSB1602_Monitoring_Status_Reg_Get(uint8_t Addr)
{
    STUSB1602_MONITORING_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_MONITORING_STATUS_REG, 1); 

    return (reg);
}


 /**
  * @brief STUSB1602 checks if CCx_ATTACHED (bit7 0x11 -- RO)
  * @param Addr I2C address of port controller device
  * @retval CCxPin_Attached_TypeDef
  */ 
CCxPin_Attached_TypeDef STUSB1602_CCx_Pin_Attach_Get(uint8_t Addr)
{
    STUSB1602_CC_CONNECTION_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CONNECTION_STATUS_REG, 1); 

    return (CCxPin_Attached_TypeDef)(reg.b.CC_ATTACHED); 
}


/**
  * @brief STUSB1602 checks the Sink_Power_State mode (bit6-5 0x11 -- RO)
  * @param Addr I2C address of port controller device
  * @retval Sink_Power_State_TypeDef
  */ 
Sink_Power_State_TypeDef STUSB1602_Sink_Power_State_Get(uint8_t Addr)
{
    STUSB1602_CC_CONNECTION_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CONNECTION_STATUS_REG, 1); 

    return (Sink_Power_State_TypeDef)(reg.b.SINK_POWER_STATE);
}


/**
  * @brief STUSB1602 checks the TypeC_FSM_State (bit4-0 0x11 -- RO)
  * @param Addr I2C address of port controller device
  * @retval TypeC_FSM_State_TypeDef
  */
TypeC_FSM_State_TypeDef STUSB1602_TypeC_FSM_State_Get(uint8_t Addr)
{
    STUSB1602_CC_CONNECTION_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CONNECTION_STATUS_REG, 1); 

    return (TypeC_FSM_State_TypeDef)(reg.b.TYPEC_FSM_STATE);
}


/**
  * @brief STUSB1602 checks the entire HW_FAULT_STATUS_TRANS reg (bit7 0x12 -- RC)
  * @param Addr Address of the used port
  * @retval     STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef
  */
STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef STUSB1602_Hard_Fault_Trans_Status_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_TRANS_REG, 1); 

    return (reg);
}  

#if 0
/**
  * @brief STUSB1602 checks the THERMAL_FAULT (bit7 0x12 -- RC)
  * @param Addr Address of the used port
  * @retval     Thermal_Fault_TypeDef
  */
Thermal_Fault_TypeDef STUSB1602_Thermal_Fault_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_TRANS_REG, 1);

    return (Thermal_Fault_TypeDef)(reg.b.THERMAL_FAULT);
}


/**
  * @brief STUSB1602 checks the VPU Over Voltage Protection Fault Transition (bit5 0x12 -- RC)
  * @param Addr Address of the used port
  * @retval VPU_OVP_Fault_Trans_TypeDef
  */
VPU_OVP_Fault_Trans_TypeDef STUSB1602_VPU_OVP_Fault_Trans_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_TRANS_REG, 1);

    return (VPU_OVP_Fault_Trans_TypeDef)(reg.b.VPU_OVP_FAULT_TRANS);
}


/**
  * @brief STUSB1602 checks the VPU Presence Transition (bit4 0x12 -- RC)
  * @param Addr Address of the used port
  * @retval     VPU_Presence_Trans_TypeDef
  */
VPU_Presence_Trans_TypeDef STUSB1602_VPU_Presence_Trans_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_TRANS_REG, 1);

    return (VPU_Presence_Trans_TypeDef)(reg.b.VPU_PRESENCE_TRANS);
}


/**
  * @brief STUSB1602 checks the VCONN power switch RVP Fault Transition (bit2 0x12 -- RC)
  * @param Addr Address of the used port
  * @retval     VCONN_SW_RVP_Fault_Trans_TypeDef
  */
VCONN_SW_RVP_Fault_Trans_TypeDef STUSB1602_VCONN_SW_RVP_Fault_Trans_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_TRANS_REG, 1);

    return (VCONN_SW_RVP_Fault_Trans_TypeDef)(reg.b.VCONN_SW_RVP_FAULT_TRANS);
}


/**
  * @brief STUSB1602 checks the VCONN power switch Over Current Protection Fault Transition (bit1 0x12 -- RC)
  * @param Addr Address of the used port
  * @retval     VCONN_SW_OCP_Fault_Trans_TypeDef
  */
VCONN_SW_OCP_Fault_Trans_TypeDef STUSB1602_VCONN_SW_OCP_Fault_Trans_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_TRANS_REG, 1);

    return (VCONN_SW_OCP_Fault_Trans_TypeDef)(reg.b.VCONN_SW_OCP_FAULT_TRANS);
}


/**
  * @brief STUSB1602 checks the VCONN power switch Over Voltage Protection Fault Transition (bit0 0x12 -- RC)
  * @param Addr Address of the used port
  * @retval     VCONN_SW_OVP_Fault_Trans_TypeDef
  */
VCONN_SW_OVP_Fault_Trans_TypeDef STUSB1602_VCONN_SW_OVP_Fault_Trans_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_TRANS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_TRANS_REG, 1);

    return (VCONN_SW_OVP_Fault_Trans_TypeDef)(reg.b.VCONN_SW_OVP_FAULT_TRANS);
}


/**
  * @brief STUSB1602 checks the VPU Over Voltage Protection Fault (bit7 0x13 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VPU_OVP_Fault_TypeDef
  */     
VPU_OVP_Fault_TypeDef STUSB1602_VPU_OVP_Fault_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_REG, 1);

    return (VPU_OVP_Fault_TypeDef)(reg.b.VPU_OVP_FAULT);
}


/**
  * @brief STUSB1602 checks the VPU Presence (bit6 0x13 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VPU_Presence_TypeDef
  */
VPU_Presence_TypeDef STUSB1602_VPU_Presence_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_REG, 1);

    return (VPU_Presence_TypeDef)(reg.b.VPU_PRESENCE);
}


/**
  * @brief STUSB1602 checks the VCONN power switch RVP Fault on CC1 (bit5 0x13 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VCONN_SW_RVP_Fault_CC1_TypeDef
  */
VCONN_SW_RVP_Fault_CC1_TypeDef STUSB1602_VCONN_SW_RVP_Fault_CC1_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_REG, 1);

    return (VCONN_SW_RVP_Fault_CC1_TypeDef)(reg.b.VCONN_SW_RVP_FAULT_CC1);
}


/**
  * @brief STUSB1602 checks the VCONN power switch RVP Fault on CC2 (bit4 0x13 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VCONN_SW_RVP_Fault_CC2_TypeDef
  */
VCONN_SW_RVP_Fault_CC2_TypeDef STUSB1602_VCONN_SW_RVP_Fault_CC2_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_REG, 1);

    return (VCONN_SW_RVP_Fault_CC2_TypeDef)(reg.b.VCONN_SW_RVP_FAULT_CC2);
}


/**
  * @brief STUSB1602 checks the VCONN power switch Over Current Protection Fault on CC1 (bit3 0x13 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VCONN_SW_OCP_Fault_CC1_TypeDef
  */
VCONN_SW_OCP_Fault_CC1_TypeDef STUSB1602_VCONN_SW_OCP_Fault_CC1_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_REG, 1);

    return (VCONN_SW_OCP_Fault_CC1_TypeDef)(reg.b.VCONN_SW_OCP_FAULT_CC1);
}


/**
  * @brief STUSB1602 checks the VCONN power switch Over Current Protection Fault on CC2 (bit2 0x13 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VCONN_SW_OCP_Fault_CC2_TypeDef
  */
VCONN_SW_OCP_Fault_CC2_TypeDef STUSB1602_VCONN_SW_OCP_Fault_CC2_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_REG, 1);

    return (VCONN_SW_OCP_Fault_CC2_TypeDef)(reg.b.VCONN_SW_OCP_FAULT_CC2);
}


/**
  * @brief STUSB1602 checks the VCONN power switch Over Voltage Protection Fault on CC1 (bit1 0x13 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VCONN_SW_OVP_Fault_CC1_TypeDef
  */
VCONN_SW_OVP_Fault_CC1_TypeDef STUSB1602_VCONN_SW_OVP_Fault_CC1_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_REG, 1);

    return (VCONN_SW_OVP_Fault_CC1_TypeDef)(reg.b.VCONN_SW_OVP_FAULT_CC1);
}


/**
  * @brief STUSB1602 checks the VCONN power switch Over Voltage Protection Fault on CC2 (bit0 0x13 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VCONN_SW_OVP_Fault_CC2_TypeDef
  */
VCONN_SW_OVP_Fault_CC2_TypeDef STUSB1602_VCONN_SW_OVP_Fault_CC2_Get(uint8_t Addr)
{
    STUSB1602_HW_FAULT_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_HW_FAULT_STATUS_REG, 1);

    return (VCONN_SW_OVP_Fault_CC2_TypeDef)(reg.b.VCONN_SW_OVP_FAULT_CC2);
}

/**
  * @brief STUSB1602 checks the Bus Idle Status on CC (bit3 0x17 -- RC)
  * @param Addr I2C address of port controller device
  * @retval Bus_Idle_TypeDef
  */
Bus_Idle_TypeDef STUSB1602_Bus_Idle_Status_Get(uint8_t Addr)
{
    PHY_STATUS_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_PHY_STATUS_REG, 1);

    return (Bus_Idle_TypeDef)(reg.b.BUS_IDLE);
}
#endif

/**
  * @brief STUSB1602 checks the CURRENT_ADVERTISED status on CC (bit7-6 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval Current_Capability_Advertised_TypeDef
  */   
Current_Capability_Advertised_TypeDef STUSB1602_Current_Advertised_Get(uint8_t Addr)
{
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 

    return (Current_Capability_Advertised_TypeDef)(reg.b.CC_CURRENT_ADVERTISED);
}


/**
  * @brief STUSB1602 sets the CURRENT_ADVERTISED on CC (bit7-6 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @param curr_cap Current Capability Advertised  of the used port
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_Current_Advertised_Set(uint8_t Addr, Current_Capability_Advertised_TypeDef curr_cap)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
  
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;  
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 
    
    reg.b.CC_CURRENT_ADVERTISED = curr_cap;
    status =  STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);
       
    return status;
}

#if 0
/**
  * @brief STUSB1602 checks the SINK DISCONNECT MODE (bit5 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval SNK_Disconnect_Mode_TypeDef
  */
SNK_Disconnect_Mode_TypeDef STUSB1602_SNK_Disconnect_Mode_Status_Get(uint8_t Addr)
{
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);

    return (SNK_Disconnect_Mode_TypeDef)(reg.b.SNK_DISCONNECT_MODE);
}
#endif

/**
  * @brief STUSB1602 sets the the SINK DISCONNECT MODE (bit5 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st SNK_DISCONNECT_MODE to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_SNK_Disconnect_Mode_Status_Set(uint8_t Addr, SNK_Disconnect_Mode_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
  
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;  
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 

    reg.b.SNK_DISCONNECT_MODE = st;
    status =  STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);

    return status;
}


/**
  * @brief STUSB1602 checks the VCONN_DISCHARGE status on CC (bit4 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VCONN_Discharge_Enable_TypeDef
  */   
VCONN_Discharge_Status_TypeDef STUSB1602_VCONN_Discharge_Status_Get(uint8_t Addr)
{
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 
    
    return (VCONN_Discharge_Status_TypeDef)(reg.b.CC_VCONN_DISCHARGE_EN);
}


/**
  * @brief STUSB1602 sets the VCONN Discharge status on CC (bit4 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st VCONN_Discharge_Status to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_VCONN_Discharge_Status_Set(uint8_t Addr, VCONN_Discharge_Status_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
  
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;  
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 
    
    reg.b.CC_VCONN_DISCHARGE_EN = st;
    status =  STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);
       
    return status;
}


/**
  * @brief STUSB1602 checks the Data Role SWAP status (bit3 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval Data_Role_Swap_TypeDef
  */ 
Data_Role_Swap_TypeDef STUSB1602_Data_Role_Swap_Status_Get(uint8_t Addr)
{
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 
    
    return (Data_Role_Swap_TypeDef)(reg.b.DR_SWAP_EN);
}


/**
  * @brief STUSB1602 sets the Data Role SWAP status (bit3 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st Status to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */ 
STUSB1602_StatusTypeDef STUSB1602_Data_Role_Swap_Status_Set(uint8_t Addr, Data_Role_Swap_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
    
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);     
    
    reg.b.DR_SWAP_EN = st;
    status =  STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);
    
    return status;
}


/**
  * @brief STUSB1602 checks the Power Role SWAP status (bit2 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval Power_Role_Swap_TypeDef
  */ 
Power_Role_Swap_TypeDef STUSB1602_Power_Role_Swap_Status_Get(uint8_t Addr)
{
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 
    
    return (Power_Role_Swap_TypeDef)(reg.b.PR_SWAP_EN);
}


/**
  * @brief STUSB1602 sets the Power Role SWAP status (bit2 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st Status to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */ 
STUSB1602_StatusTypeDef STUSB1602_Power_Role_Swap_Status_Set(uint8_t Addr, Power_Role_Swap_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
    
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);     
    
    reg.b.PR_SWAP_EN = st;
    status =  STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);
    
    return status;
}


/**
  * @brief STUSB1602 checks the VCONN Role SWAP status (bit1 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VCONN_Role_Swap_TypeDef
  */ 
VCONN_Role_Swap_TypeDef STUSB1602_VCONN_Role_Swap_Status_Get(uint8_t Addr)
{
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 
    
    return (VCONN_Role_Swap_TypeDef)(reg.b.VCONN_SWAP_EN);
}


/**
  * @brief STUSB1602 sets the VCONN Role SWAP status (bit1 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st Status to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */ 
STUSB1602_StatusTypeDef STUSB1602_VCONN_Role_Swap_Status_Set(uint8_t Addr, VCONN_Role_Swap_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
    
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);     
    
    reg.b.VCONN_SWAP_EN = st;
    status =  STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);
    
    return status;
}


/**
  * @brief STUSB1602 checks the VCONN_SUPPLY status on CC (bit0 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VCONN_Discharge_Enable_TypeDef
  */   
VCONN_Supply_Status_TypeDef STUSB1602_VCONN_Supply_Status_Get(uint8_t Addr)
{
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 
    
    return (VCONN_Supply_Status_TypeDef)(reg.b.CC_VCONN_SUPPLY_EN);
}


/**
  * @brief STUSB1602 sets the VCONN_SUPPLY status on CC (bit0 0x18 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st VCONN_Supply_Status to be set on the used port
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_VCONN_Supply_Status_Set(uint8_t Addr, VCONN_Supply_Status_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
  
    STUSB1602_CC_CAPABILITY_CTRL_RegTypeDef reg;  
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1); 
    
    reg.b.CC_VCONN_SUPPLY_EN = st;
    status =  STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_CAPABILITY_CTRL_REG, 1);
       
    return status;
}


/**
  * @brief STUSB1602 checks the VCONN switch Current Limitation on CC (bit3-0 0x1E -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VCONN_Switch_Current_Limit_TypeDef
  */   
VCONN_Switch_Current_Limit_TypeDef STUSB1602_VCONN_Switch_Current_Limit_Get(uint8_t Addr)
{
    STUSB1602_CC_VCONN_SWITCH_CTRL_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_VCONN_SWITCH_CTRL_REG, 1); 
    
    return (VCONN_Switch_Current_Limit_TypeDef)(reg.b.CC_VCONN_SWITCH_ILIM);
}


/**
  * @brief STUSB1602 sets the VCONN switch Current Limitation on CC (bit3-0 0x1E -- R/W)
  * @param Addr I2C address of port controller device
  * @param curr_lim Current limit value to be set
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_VCONN_Switch_Current_Limit_Set(uint8_t Addr, VCONN_Switch_Current_Limit_TypeDef curr_lim)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;
    
    STUSB1602_CC_VCONN_SWITCH_CTRL_RegTypeDef reg; 
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_VCONN_SWITCH_CTRL_REG, 1); 
  
    reg.b.CC_VCONN_SWITCH_ILIM = curr_lim;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_VCONN_SWITCH_CTRL_REG, 1);

    return status;
}


/**
  * @brief STUSB1602 checks the TYPEC_CTRL (bit7-4 0x1F -- R/W)
  * @param Addr I2C address of port controller device
  * @retval Type_C_CTRL_TypeDef
  */   
Type_C_CTRL_TypeDef STUSB1602_Type_C_Control_Get(uint8_t Addr)
{
    STUSB1602_CC_MODE_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_MODE_CTRL_REG, 1); 
    
    return (Type_C_CTRL_TypeDef)(reg.b.TYPEC_CTRL);
}

/**
  * @brief STUSB1602 sets the TYPEC_CTRL (bit7-4 0x1F -- R/W)
  * @param Addr I2C address of port controller device
  * @param Ctrl Control to be set
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_Type_C_Control_Set(uint8_t Addr, Type_C_CTRL_TypeDef Ctrl)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;
    
    STUSB1602_CC_MODE_CTRL_RegTypeDef reg; 
//    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_MODE_CTRL_REG, 1); 
  
    reg.b.TYPEC_CTRL = Ctrl;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_MODE_CTRL_REG, 1);

    return status;
}


/**
  * @brief STUSB1602 checks the VCONN_MONITORING Enabling bit (bit7 0x20 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VCONN_Monitoring_TypeDef
  */   
VCONN_Monitoring_TypeDef STUSB1602_VCONN_Monitor_Status_Get(uint8_t Addr)
{
    STUSB1602_VCONN_MONITORING_CTRL_RegTypeDef reg;

    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VCONN_MONITORING_CTRL_REG, 1); 
    
    return (VCONN_Monitoring_TypeDef)(reg.b.VCONN_MONITORING_EN);
}


/**
  * @brief STUSB1602 sets the VCONN_MONITORING Enabling bit (bit7 0x20 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st Power mode to be set
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_VCONN_Monitor_Status_Set(uint8_t Addr, VCONN_Monitoring_TypeDef st)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;
    
    STUSB1602_VCONN_MONITORING_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VCONN_MONITORING_CTRL_REG, 1); 
  
    reg.b.VCONN_MONITORING_EN = st;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VCONN_MONITORING_CTRL_REG, 1);

    return status;
}


/**
  * @brief STUSB1602 checks the VCONN_UVLO_Threshold (bit6 0x20 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VCONN_UVLO_Threshold_TypeDef
  */   
VCONN_UVLO_Threshold_TypeDef STUSB1602_VCONN_UVLO_Thresh_Status_Get(uint8_t Addr)
{
    STUSB1602_VCONN_MONITORING_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VCONN_MONITORING_CTRL_REG, 1); 
    
    return (VCONN_UVLO_Threshold_TypeDef)(reg.b.VCONN_UVLO_THRESHOLD);
}


/**
  * @brief STUSB1602 sets the VCONN_UVLO_Threshold Enabling bit (bit6 0x20 -- R/W)
  * @param Addr I2C address of port controller device
  * @param thr Power mode to be set
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_VCONN_UVLO_Thresh_Status_Set(uint8_t Addr, VCONN_UVLO_Threshold_TypeDef thr)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;
    
    STUSB1602_VCONN_MONITORING_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VCONN_MONITORING_CTRL_REG, 1); 
  
    reg.b.VCONN_UVLO_THRESHOLD = thr;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VCONN_MONITORING_CTRL_REG, 1);

    return status;
}

/**
  * @brief STUSB1602 checks the VBUS_SELECT (bit7:0 0x21 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval uint16_t 
  */   
uint16_t STUSB1602_VBUS_Select_Status_Get(uint8_t Addr)
{
    uint8_t reg; /* first default value = 50 */
    uint16_t DAC_mV=0;
    STUSB1602_ReadReg(&reg, Addr, STUSB1602_VBUS_SELECT_REG, 1);
    DAC_mV=(uint16_t)(reg*100);
    
    return DAC_mV;
}

/**
  * @brief STUSB1602 Sets the VBUS_SELECT DAC reference for VBUS sensing (bit7:0 0x21 -- R/W)
  * @param Addr I2C address of port controller device
  * @param mV The value (expressed in mV) to set the internal reference DAC
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VBUS_Select_Status_Set(uint8_t Addr, uint16_t mV)
{
    
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
    uint8_t reg; /* first default value = 50 */     
    
      
    reg =  mV /100;
    status = STUSB1602_WriteReg(&reg, Addr, STUSB1602_VBUS_SELECT_REG, 1);
             
    return status;
}


/**
  * @brief STUSB1602 checks the VBUS_VShift_High (bit7:4 0x22 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval uint8_t It returns the % shift coefficient used for computing the high threshold value 
  */   
uint8_t STUSB1602_VBUS_VShift_High_Get(uint8_t Addr)
{
    uint8_t set =0;
    STUSB1602_VBUS_RANGE_MONITORING_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_RANGE_MONITORING_CTRL_REG, 1); 
    
    set = (uint8_t)((reg.b.VBUS_VSHIFT_HIGH) + 5);
    return (set);
}


/**
  * @brief STUSB1602 Sets the VBUS_VShift_High (bit7:4 0x22 -- R/W)
  * @param Addr I2C address of port controller device
  * @param Set The VShift_High value >= 5% (expressed in %) to set the high threshold value  
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VBUS_VShift_Set(uint8_t Addr, uint8_t HSet, uint8_t LSet)
{
    
    STUSB1602_StatusTypeDef status = STUSB1602_OK;
    
    STUSB1602_VBUS_RANGE_MONITORING_CTRL_RegTypeDef reg;
//    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_RANGE_MONITORING_CTRL_REG, 1); 
    LSet = (LSet>0) ? -LSet : LSet;
    reg.b.VBUS_VSHIFT_HIGH = (HSet - 5);
    reg.b.VBUS_VSHIFT_LOW = (-LSet - 5);
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_RANGE_MONITORING_CTRL_REG, 1);
        
    return status;
}


/**
  * @brief STUSB1602 Sets the VBUS_VShift_High (bit7:4 0x22 -- R/W)
  * @param Addr I2C address of port controller device
  * @param Set The VShift_High value >= 5% (expressed in %) to set the high threshold value  
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VBUS_VShift_High_Set(uint8_t Addr, uint8_t Set)
{
    
    STUSB1602_StatusTypeDef status = STUSB1602_OK;
    
    STUSB1602_VBUS_RANGE_MONITORING_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_RANGE_MONITORING_CTRL_REG, 1); 
  
    reg.b.VBUS_VSHIFT_HIGH = (Set - 5);
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_RANGE_MONITORING_CTRL_REG, 1);
        
    return status;
}


/**
  * @brief STUSB1602 checks the VBUS_VShift_Low (bit3:0 0x22 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval int8_t It returns the % shift coefficient used for computing the low threshold value 
  */   
int8_t STUSB1602_VBUS_VShift_Low_Get(uint8_t Addr)
{
    int8_t set=0;
    STUSB1602_VBUS_RANGE_MONITORING_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_RANGE_MONITORING_CTRL_REG, 1); 
    
    set = -5 - (uint8_t)(reg.b.VBUS_VSHIFT_LOW);
    return (set);
}


/**
  * @brief STUSB1602 Sets the VBUS_VShift_Low (bit3:0 0x22 -- R/W)
  * @param Addr I2C address of port controller device
  * @param Set The VShift_Low value <=5% (expressed in %) to set the low threshold value
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VBUS_VShift_Low_Set(uint8_t Addr, int8_t Set)
{
    Set = (Set>0) ? -Set : Set;
    STUSB1602_StatusTypeDef status = STUSB1602_OK;
    
    STUSB1602_VBUS_RANGE_MONITORING_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_RANGE_MONITORING_CTRL_REG, 1); 
  
    reg.b.VBUS_VSHIFT_LOW = (-Set - 5);
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_RANGE_MONITORING_CTRL_REG, 1);
        
    return status;
}


/**
  * @brief STUSB1602 checks the SW RESET (bit0 0x23 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval SW_RESET_TypeDef 
  */   
SW_RESET_TypeDef STUSB1602_SW_RESET_Get(uint8_t Addr)
{    
   STUSB1602_RESET_CTRL_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_RESET_CTRL_REG, 1); 
    
   return (SW_RESET_TypeDef)(reg.b.SW_RESET_EN);   
}


/**
  * @brief STUSB1602 Sets the SW RESET (bit0 0x23 -- R/W)
  * @param Addr I2C address of port controller device
  * @param Rst Enable or Disable of the SW RST function
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_SW_RESET_Set(uint8_t Addr, SW_RESET_TypeDef Rst)
{  
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
    
    STUSB1602_RESET_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_RESET_CTRL_REG, 1); 
  
    reg.b.SW_RESET_EN = Rst;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_RESET_CTRL_REG, 1);
             
    return status;
}


/**
  * @brief STUSB1602 checks the Pwr_Acc_Try_SNK (bit1 0x24 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval Pwr_Acc_Try_SNK_TypeDef 
  */   
Pwr_Acc_Try_SNK_TypeDef STUSB1602_Pwr_Acc_Try_SNK_Get(uint8_t Addr)
{    
   STUSB1602_CC_POWERED_ACCESSORY_CTRL_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_POWERED_ACCESSORY_CTRL_REG, 1); 
    
   return (Pwr_Acc_Try_SNK_TypeDef)(reg.b.PWR_ACC_TRY_SNK_EN);   
}


/**
  * @brief STUSB1602 sets the Pwr_Acc_Try_SNK bit (bit1 0x24 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st Status to be set  
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_Pwr_Acc_Try_SNK_Set(uint8_t Addr, Pwr_Acc_Try_SNK_TypeDef st)
{    
    STUSB1602_StatusTypeDef status = STUSB1602_OK;  
    
    STUSB1602_CC_POWERED_ACCESSORY_CTRL_RegTypeDef reg;
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_POWERED_ACCESSORY_CTRL_REG, 1);
  
    reg.b.PWR_ACC_TRY_SNK_EN = st;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_POWERED_ACCESSORY_CTRL_REG, 1);
             
    return status;
}


/**
  * @brief STUSB1602 checks the Pwr_Acc_Detect (bit0 0x24 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval Pwr_Acc_Detect_TypeDef 
  */   
Pwr_Acc_Detect_TypeDef STUSB1602_Pwr_Acc_Detect_Get(uint8_t Addr)
{    
   STUSB1602_CC_POWERED_ACCESSORY_CTRL_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_POWERED_ACCESSORY_CTRL_REG, 1); 
    
   return (Pwr_Acc_Detect_TypeDef)(reg.b.PWR_ACC_DETECT_EN);   
}


/**
  * @brief STUSB1602 sets the Pwr_Acc_Detect bit (bit1 0x24 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st Status to be set 
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_Pwr_Acc_Detect_Set(uint8_t Addr, Pwr_Acc_Detect_TypeDef st)
{    
   STUSB1602_StatusTypeDef status = STUSB1602_OK;  
    
   STUSB1602_CC_POWERED_ACCESSORY_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_CC_POWERED_ACCESSORY_CTRL_REG, 1);
  
   reg.b.PWR_ACC_DETECT_EN = st;    
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_CC_POWERED_ACCESSORY_CTRL_REG, 1);
             
   return status;
}


/**
  * @brief STUSB1602 checks the VBUS_DISCHARGE_TIME_TO_0V (bit7:4 0x25 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval uint16_t VBUS discharge time in msec
  */   
uint16_t STUSB1602_VBUS_Discharge_Time_to_0V_Get(uint8_t Addr)
{
    uint16_t tim=0;
    STUSB1602_VBUS_DISCHARGE_TIME_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_TIME_CTRL_REG, 1);
    
    tim=(uint16_t)((reg.b.VBUS_DISCHARGE_TIME_TO_0V)*84);
    
    return tim;
}


/**
  * @brief STUSB1602 sets the VBUS_DISCHARGE_TIME_TO_0V (bit7:4 0x25 -- R/W)
  * @param Addr I2C address of port controller device
  * @param tim  VBUS discharge time in msec
  * @retval STUSB1602_StatusTypeDef
  */
STUSB1602_StatusTypeDef STUSB1602_VBUS_Discharge_Time_to_0V_Set(uint8_t Addr, uint16_t tim)
{
   STUSB1602_StatusTypeDef status = STUSB1602_OK;
   
   STUSB1602_VBUS_DISCHARGE_TIME_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_TIME_CTRL_REG, 1);
   
   reg.b.VBUS_DISCHARGE_TIME_TO_0V = (uint8_t)(tim/84);
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_TIME_CTRL_REG, 1); 
    
   return status;
}


/**
  * @brief STUSB1602 checks the VBUS_DISCHARGE_TIME_TO_PDO (bit3:0 0x25 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval uint16_t VBUS discharge time in msec 
  */ 
uint16_t STUSB1602_VBUS_Discharge_Time_to_PDO_Get(uint8_t Addr)
{
    uint16_t tim=0;
    STUSB1602_VBUS_DISCHARGE_TIME_CTRL_RegTypeDef reg;
            
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_TIME_CTRL_REG, 1);
    
    tim=(uint16_t)((reg.b.VBUS_DISCHARGE_TIME_TO_PDO)*20);
    
    return tim; 
}


/**
  * @brief STUSB1602 sets the VBUS_DISCHARGE_TIME_TO_PDO (bit3:0 0x25 -- R/W)
  * @param Addr I2C address of port controller device
  * @param tim VBUS discharge time in msec
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_VBUS_Discharge_Time_to_PDO_Set(uint8_t Addr, uint16_t tim)
{
   STUSB1602_StatusTypeDef status = STUSB1602_OK;
   
   STUSB1602_VBUS_DISCHARGE_TIME_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_TIME_CTRL_REG, 1);
   
   reg.b.VBUS_DISCHARGE_TIME_TO_PDO = (uint8_t)(tim/20);
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_TIME_CTRL_REG, 1); 
    
   return status;
}


#if 0
/**
  * @brief STUSB1602 checks the VBUS_DISCHARGE_CTRL State (EN or DIS) (bit7 0x26 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VBUS_Discharge_State_TypeDef 
  */
VBUS_Discharge_State_TypeDef STUSB1602_VBUS_Discharge_State_Get(uint8_t Addr)
{
   STUSB1602_VBUS_DISCHARGE_CTRL_RegTypeDef reg;

   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_CTRL_REG, 1);

   return (VBUS_Discharge_State_TypeDef)(reg.b.VBUS_DISCHARGE_EN);
}
#endif

/**
  * @brief STUSB1602 sets the VBUS_DISCHARGE_CTRL State (EN or DIS) (bit7 0x26 -- R/W)
  * @param Addr I2C address of port controller device
  * @param st Status to be set
  * @retval STUSB1602_StatusTypeDef 
  */
STUSB1602_StatusTypeDef STUSB1602_VBUS_Discharge_State_Set(uint8_t Addr, VBUS_Discharge_State_TypeDef st)
{    
   STUSB1602_StatusTypeDef status = STUSB1602_OK;
   
   STUSB1602_VBUS_DISCHARGE_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_CTRL_REG, 1); 
   
   reg.b.VBUS_DISCHARGE_EN = st;
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_DISCHARGE_CTRL_REG, 1);
   
   return status;   
}


/**
  * @brief STUSB1602 checks the VBUS_SNK_State (EN or DIS) (bit1 0x27 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VBUS_SNK_State_TypeDef 
  */   
VBUS_SNK_State_TypeDef STUSB1602_VBUS_SNK_State_Get(uint8_t Addr)
{    
   STUSB1602_VBUS_ENABLE_STATUS_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_ENABLE_STATUS_REG, 1); 
    
   return (VBUS_SNK_State_TypeDef)(reg.b.VBUS_SINK_EN);   
}


/**
  * @brief STUSB1602 checks the VBUS_SRC_State (EN or DIS) (bit0 0x27 -- RO)
  * @param Addr I2C address of port controller device
  * @retval VBUS_SRC_State_TypeDef 
  */   
VBUS_SRC_State_TypeDef STUSB1602_VBUS_SRC_State_Get(uint8_t Addr)
{    
   STUSB1602_VBUS_ENABLE_STATUS_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_ENABLE_STATUS_REG, 1); 
    
   return (VBUS_SRC_State_TypeDef)(reg.b.VBUS_SOURCE_EN);   
}


/**
  * @brief STUSB1602 checks the Power_Mode (bit2-0 0x28 -- R/W)
  * @param Addr I2C address of port controller device
  * @retval Power_Mode_TypeDef
  */   
Power_Mode_TypeDef STUSB1602_Power_Mode_Get(uint8_t Addr)
{
    STUSB1602_MODE_CTRL_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_MODE_CTRL_REG, 1); 
    
    return (Power_Mode_TypeDef)(reg.b.POWER_MODE);
}


/**
  * @brief STUSB1602 sets the Power_Mode (bit2-0 0x28 -- R/W)
  * @param Addr I2C address of port controller device
  * @param Pwr Power mode to be set
  * @retval STUSB1602_StatusTypeDef
  */   
STUSB1602_StatusTypeDef STUSB1602_Power_Mode_Set(uint8_t Addr, Power_Mode_TypeDef Pwr)
{
    STUSB1602_StatusTypeDef status = STUSB1602_OK;
    
    STUSB1602_MODE_CTRL_RegTypeDef reg; 
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_MODE_CTRL_REG, 1); 
  
    reg.b.POWER_MODE = Pwr;
    status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_MODE_CTRL_REG, 1);
             
    return status;
}


/**
  * @brief STUSB1602 checks the VDD_OVLO_Threshold State (EN or DIS) (bit6 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VDD_OVLO_Threshold_TypeDef 
  */   
VDD_OVLO_Threshold_TypeDef STUSB1602_VDD_OVLO_Threshold_Get(uint8_t Addr)
{    
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
    
   return (VDD_OVLO_Threshold_TypeDef)(reg.b.VDD_OVLO_DISABLE);   
}

/**
  * @brief It checks if NVM is loaded correctly (bit0:1 0x2F -- R)
  * @param Addr I2C address of port controller device
  * @retval NVM_OK_TypeDef 
  */  
NVM_OK_TypeDef STUSB1602_NVM_OK_Get(uint8_t Addr)
{
   STUSB1602_DEVICE_CUT_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_DEVICE_CUT_REG, 1);
      
    return (NVM_OK_TypeDef)(reg.b.Reserved_0_2);
}

/**
  * @brief It gets the cut number (bit2:4 0x2F -- R)
  * @param Addr I2C address of port controller device
  * @retval DEVICE_CUT_TypeDef 
  */ 
DEVICE_CUT_TypeDef STUSB1602_DEVICE_CUT_Get(uint8_t Addr)
{
   STUSB1602_DEVICE_CUT_RegTypeDef reg;
        
    STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_DEVICE_CUT_REG, 1);
      
    return (DEVICE_CUT_TypeDef)(reg.b.DEVICE_CUT);
}

/**
  * @brief STUSB1602 checks the VBUS_RANGE_DISABLE State (EN or DIS) (bit4 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VBUS_Range_State_TypeDef 
  */   
VBUS_Range_State_TypeDef STUSB1602_VBUS_Range_State_Get(uint8_t Addr)
{    
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
    
   return (VBUS_Range_State_TypeDef)(reg.b.VBUS_RANGE_DISABLE);   
}


/**
  * @brief STUSB1602 checks the VBUS_VSAFE0V_Threshold State (bit2:1 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VBUS_VSAFE0V_Threshold_TypeDef 
  */   
VBUS_VSAFE0V_Threshold_TypeDef STUSB1602_VBUS_VSAFE0V_Threshold_Get(uint8_t Addr)
{    
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
    
   return (VBUS_VSAFE0V_Threshold_TypeDef)(reg.b.VBUS_VSAFE0V_THRESHOLD);   
}


/**
  * @brief STUSB1602 checks the VDD_UVLO_Threshold State (EN or DIS) (bit0 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @retval VDD_UVLO_Threshold_TypeDef 
  */   
VDD_UVLO_Threshold_TypeDef STUSB1602_VDD_UVLO_Threshold_Get(uint8_t Addr)
{    
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
    
   return (VDD_UVLO_Threshold_TypeDef)(reg.b.VDD_UVLO_DISABLE);   
}


/**
  * @brief STUSB1602 sets the VDD_OVLO_Threshold State (EN or DIS) (bit6 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @param st status to be set 
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VDD_OVLO_Threshold_Set(uint8_t Addr, VDD_OVLO_Threshold_TypeDef st)
{      
   STUSB1602_StatusTypeDef status = STUSB1602_OK;
   
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
   
   reg.b.VDD_OVLO_DISABLE = st;
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1);
     
   return status;  
}


/**
  * @brief STUSB1602 sets the VBUS_Range_State (EN or DIS) (bit4 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @param st status to be set 
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VBUS_Range_State_Set(uint8_t Addr, VBUS_Range_State_TypeDef st)
{      
   STUSB1602_StatusTypeDef status = STUSB1602_OK;
   
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
   
   reg.b.VBUS_RANGE_DISABLE = st;
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1);
     
   return status;  
}


/**
  * @brief STUSB1602 sets the VBUS_presence_State (EN or DIS) (bit7 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @param st status to be set 
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VBUS_Presence_State_Set(uint8_t Addr, VBUS_Presence_State_TypeDef st)
{      
   STUSB1602_StatusTypeDef status = STUSB1602_OK;
   
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
   
   reg.b.VBUS_PRESENCE_DISABLE = st;
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1);
     
   return status;  
}


/**
* @brief STUSB1602 sets the VBUS_VSAFE0V_Threshold (EN or DIS) (bit2:1 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @param st status to be set 
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VBUS_VSAFE0V_Threshold_Set(uint8_t Addr, VBUS_VSAFE0V_Threshold_TypeDef st)
{      
   STUSB1602_StatusTypeDef status = STUSB1602_OK;
   
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
   
   reg.b.VBUS_VSAFE0V_THRESHOLD = st;
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1);
     
   return status;  
}


/**
  * @brief STUSB1602 sets the VDD_UVLO_Threshold State (EN or DIS) (bit0 0x2E -- R/W)
  * @param Addr I2C address of port controller device
  * @param st status to be set 
  * @retval STUSB1602_StatusTypeDef 
  */   
STUSB1602_StatusTypeDef STUSB1602_VDD_UVLO_Threshold_Set(uint8_t Addr, VDD_UVLO_Threshold_TypeDef st)
{      
   STUSB1602_StatusTypeDef status = STUSB1602_OK;
   
   STUSB1602_VBUS_MONITORING_CTRL_RegTypeDef reg;
   STUSB1602_ReadReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1); 
   
   reg.b.VDD_UVLO_DISABLE = st;
   status = STUSB1602_WriteReg(&reg.d8, Addr, STUSB1602_VBUS_MONITORING_CTRL_REG, 1);
     
   return status;  
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
