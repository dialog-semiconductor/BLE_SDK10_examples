/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_core.c
  * @author  MCD Application Team
  * @brief   USBPD dpm core file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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

#include <stdio.h>
#include "osal.h"

#include "usbpd_core.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf_val.h"
#include "usbpd_dpm_user.h"
#include "usbpd_cad_hw_if.h"

#if defined(_LOW_POWER)
#include "usbpd_lowpower.h"
#endif

#if defined(USBPD_TCPM_MODULE_ENABLED)
#include "usbpd_tcpci.h"
#endif /* USBPD_TCPM_MODULE_ENABLED */

#if defined(_FWUPDATE_RESPONDER)
#include "usbpd_pdfu_responder.h"
#endif /* _FWUPDATE_RESPONDER */


#if defined(USBPD_TCPM_MODULE_ENABLED)
void USBPD_ALERT_Task(void *argument);
#else
void USBPD_CAD_Task(void *argument);
#endif /* USBPD_TCPM_MODULE_ENABLED */

#define FREERTOS_PE_PRIORITY                    OS_TASK_PRIORITY_NORMAL
#if defined(_SVDM)
#define FREERTOS_PE_STACK_SIZE                  (350 * OS_STACK_WORD_SIZE)
#elif defined(__AUTHENTICATION__)
#define FREERTOS_PE_STACK_SIZE                  (350 * OS_STACK_WORD_SIZE)
#else
#define FREERTOS_PE_STACK_SIZE                  (200 * OS_STACK_WORD_SIZE)
#endif /*_SVDM*/

#if defined(USBPD_TCPM_MODULE_ENABLED)
#define FREERTOS_ALERT_PRIORITY                 OS_TASK_PRIORITY_HIGHEST
#define FREERTOS_ALERT_STACK_SIZE               (240 * OS_STACK_WORD_SIZE)
#else
#define FREERTOS_CAD_PRIORITY                   OS_TASK_PRIORITY_HIGHEST
#define FREERTOS_CAD_STACK_SIZE                 (300 * OS_STACK_WORD_SIZE)
#endif /*USBPD_TCPM_MODULE_ENABLED*/

#if defined(_DEBUG_TRACE)
#define DPM_CORE_DEBUG_TRACE(_PORTNUM_, __MESSAGE__)  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, _PORTNUM_, 0u, (uint8_t *)(__MESSAGE__), sizeof(__MESSAGE__) - 1u);
#else
#define DPM_CORE_DEBUG_TRACE(_PORTNUM_, __MESSAGE__)
#endif /* _DEBUG_TRACE */

/* Private variables ---------------------------------------------------------*/
static OS_TASK DPM_PEThreadId_Table[USBPD_PORT_COUNT];
#if defined(USBPD_TCPM_MODULE_ENABLED)
OS_QUEUE  AlarmMsgBox;
osThreadId ThreadAlert;
#else
static OS_QUEUE CADQueueId;
#endif /* USBPD_TCPM_MODULE_ENABLED */
static OS_QUEUE PEQueueId[USBPD_PORT_COUNT];

USBPD_ParamsTypeDef   DPM_Params[USBPD_PORT_COUNT];

/* Private function prototypes -----------------------------------------------*/

static void USBPD_PE_TaskWakeUp(uint8_t PortNum);
static void DPM_ManageAttachedState(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc);

void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc);
#if !defined(USBPD_TCPM_MODULE_ENABLED)
static void USBPD_DPM_CADTaskWakeUp(void);
#endif /* !USBPD_TCPM_MODULE_ENABLED */

static const USBPD_PE_Callbacks dpmCallbacks =
{
#if defined(_SRC) || defined(_DRP)
  USBPD_DPM_SetupNewPower,
#else
  NULL,
#endif /*_SRC || DRP */
  USBPD_DPM_HardReset,
  USBPD_DPM_EvaluatePowerRoleSwap,
  USBPD_DPM_Notification,
#ifdef USBPD_REV30_SUPPORT
  USBPD_DPM_ExtendedMessageReceived,
#else
  NULL,
#endif /* USBPD_REV30_SUPPORT */
  USBPD_DPM_GetDataInfo,
  USBPD_DPM_SetDataInfo,
#if defined(_SRC) || defined(_DRP)
  USBPD_DPM_EvaluateRequest,
#else
  NULL,
#endif /*_SRC || DRP */
#if defined(_SNK) || defined(_DRP)
  USBPD_DPM_SNK_EvaluateCapabilities,
#else
  NULL,
#endif /*_SNK || DRP */
#if defined(_DRP)
  USBPD_DPM_PowerRoleSwap,
#else
  NULL,
#endif /*  _DRP  */
  USBPD_PE_TaskWakeUp,
#if defined(_VCONN_SUPPORT)
  USBPD_DPM_EvaluateVconnSwap,
  USBPD_DPM_PE_VconnPwr,
#else
  NULL,
  NULL,
#endif /* _VCONN_SUPPORT */
#if defined(_ERROR_RECOVERY)
  USBPD_DPM_EnterErrorRecovery,
#else
  NULL,
#endif /* _ERROR_RECOVERY */
  USBPD_DPM_EvaluateDataRoleSwap,
  USBPD_DPM_IsPowerReady
};

static const USBPD_CAD_Callbacks CAD_cbs =
{
  USBPD_DPM_CADCallback,
#if defined(USBPD_TCPM_MODULE_ENABLED)
  NULL
#else  /* USBPD_TCPM_MODULE_ENABLED */
  USBPD_DPM_CADTaskWakeUp
#endif
};

/**
  * @brief  Initialize the core stack (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_InitCore(void)
{
  USBPD_StatusTypeDef _retr = USBPD_OK;

#if !defined(_SIMULATOR)
  /* Check the lib selected */
  if (USBPD_TRUE != USBPD_PE_CheckLIB(_LIB_ID))
  {
    return USBPD_ERROR;
  }
#endif

  printf("usbpd core mem: %lu\r\n", USBPD_PE_GetMemoryConsumption());

#if defined(_TRACE) || defined(_GUI_INTERFACE)
  /* Initialise the TRACE */
  USBPD_TRACE_Init();
#endif /* _TRACE || _GUI_INTERFACE */

#if defined(USBPD_TCPM_MODULE_ENABLED)
  USBPD_TCPCI_Init();
#endif

  for (uint8_t _port_index = 0; _port_index < USBPD_PORT_COUNT; ++_port_index)
  {
    /* Variable to be sure that DPM is correctly initialized */
    DPM_Params[_port_index].DPM_Initialized = USBPD_FALSE;

    /* check the stack settings */
    DPM_Params[_port_index].PE_SpecRevision  = DPM_Settings[_port_index].PE_SpecRevision;
    DPM_Params[_port_index].PE_PowerRole     = DPM_Settings[_port_index].PE_DefaultRole;
    DPM_Params[_port_index].PE_SwapOngoing   = USBPD_FALSE;
    DPM_Params[_port_index].ActiveCCIs       = CCNONE;
    DPM_Params[_port_index].VconnCCIs        = CCNONE;
    DPM_Params[_port_index].VconnStatus      = USBPD_FALSE;

#if defined(USBPD_TCPM_MODULE_ENABLED)
    {
      TCPC_DrvTypeDef *tcpc_driver;
      USBPD_TCPCI_GetDevicesDrivers(_port_index, &tcpc_driver);
      USBPD_TCPM_HWInit(_port_index, DPM_Settings[_port_index].CAD_RoleToggle, &DPM_Params[_port_index], &CAD_cbs, tcpc_driver);
    }
#else
    /* CAD SET UP : Port 0 */
    if(USBPD_CAD_OK != USBPD_CAD_Init(_port_index, &CAD_cbs, &DPM_Settings[_port_index], &DPM_Params[_port_index]))
    {
	    return USBPD_ERROR;
    }
#endif

    /* PE SET UP : Port 0 */
    _retr = USBPD_PE_Init(_port_index, &DPM_Settings[_port_index], &DPM_Params[_port_index], &dpmCallbacks);
    if(USBPD_OK != _retr)
    {
	return _retr;
    }

    /* DPM is correctly initialized */
    DPM_Params[_port_index].DPM_Initialized = USBPD_TRUE;

#if defined(USBPD_TCPM_MODULE_ENABLED)
#else
    /* Enable CAD on Port 0 */
    USBPD_CAD_PortEnable(_port_index, USBPD_CAD_ENABLE);
#endif /* USBPD_TCPM_MODULE_ENABLED */
  }

#ifdef _LOW_POWER
  USBPD_LOWPOWER_Init();
#endif /* _LOW_POWER */

  return _retr;
}

/**
  * @brief  Initialize the OS parts (task, queue,... )
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_InitOS(void)
{
#if defined(USBPD_TCPM_MODULE_ENABLED)
	OS_QUEUE_CREATE(AlarmMsgBox, sizeof(uint16_t), TCPM_ALARMBOX_MESSAGES_MAX);
	ASSERT_ERROR(AlarmMsgBox != NULL);
#else
	OS_QUEUE_CREATE(CADQueueId, sizeof(uint16_t), 2);
	ASSERT_ERROR(CADQueueId != NULL);
#endif /* USBPD_TCPM_MODULE_ENABLED */

  /* Create the queue corresponding to PE task */
  OS_QUEUE_CREATE(PEQueueId[0], sizeof(uint16_t), 1);
#if USBPD_PORT_COUNT == 2
  OS_QUEUE_CREATE(PEQueueId[1], sizeof(uint16_t), 1);
#endif /* USBPD_PORT_COUNT == 2 */

  /* PE task to be created on attachment */
  DPM_PEThreadId_Table[USBPD_PORT_0] = NULL;
#if USBPD_PORT_COUNT == 2
  DPM_PEThreadId_Table[USBPD_PORT_1] = NULL;
#endif /* USBPD_PORT_COUNT == 2 */

#if defined(USBPD_TCPM_MODULE_ENABLED)
  USBPD_TCPI_AlertInit();
#endif /* USBPD_TCPM_MODULE_ENABLED */

  return USBPD_OK;
}

/**
  * @brief  Start the OS task
  * @retval USBPD status
  */
USBPD_StatusTypeDef USBPD_DPM_StartOS(void)
{
#if defined(USBPD_TCPM_MODULE_ENABLED)

	OS_TASK task_alert;
	OS_TASK_CREATE( "ALERTTask",
		USBPD_ALERT_Task,
		NULL,
		FREERTOS_ALERT_STACK_SIZE,
		FREERTOS_ALERT_PRIORITY,
		task_alert );
	OS_ASSERT(task_alert);

#else

	OS_TASK task_cad;
	OS_TASK_CREATE( "CAD",
		USBPD_CAD_Task,
		NULL,
		FREERTOS_CAD_STACK_SIZE,
		FREERTOS_CAD_PRIORITY,
		task_cad );
	OS_ASSERT(task_cad);

#endif /* USBPD_TCPM_MODULE_ENABLED */

  return USBPD_OK;
}

/**
  * @brief  Initialize DPM (port power role, PWR_IF, CAD and PE Init procedures)
  * @retval USBPD status
  */
void USBPD_DPM_TimerCounter(void)
{
  /* Call PE/PRL timers functions only if DPM is initialized */
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_0].DPM_Initialized)
  {
#ifdef _USER_TIMERS
    USBPD_DPM_UserTimerCounter(USBPD_PORT_0);
#endif
    USBPD_PE_TimerCounter(USBPD_PORT_0);
    USBPD_PRL_TimerCounter(USBPD_PORT_0);
#if defined(_FWUPDATE_RESPONDER)
    USBPD_PDFU_TimerCounter(USBPD_PORT_0);
#endif /* _FWUPDATE_RESPONDER */
  }
#if USBPD_PORT_COUNT==2
  if (USBPD_TRUE == DPM_Params[USBPD_PORT_1].DPM_Initialized)
  {
#ifdef _USER_TIMERS
    USBPD_DPM_UserTimerCounter(USBPD_PORT_1);
#endif
    USBPD_PE_TimerCounter(USBPD_PORT_1);
    USBPD_PRL_TimerCounter(USBPD_PORT_1);
#if defined(_FWUPDATE_RESPONDER)
    USBPD_PDFU_TimerCounter(USBPD_PORT_1);
#endif /* _FWUPDATE_RESPONDER */
  }
#endif /* USBPD_PORT_COUNT == 2 */
}

/**
  * @brief  WakeUp PE task
  * @param  PortNum port number
  * @retval None
  */
static void USBPD_PE_TaskWakeUp(uint8_t PortNum)
{
  uint16_t event = 0xFFFFU;
  OS_ASSERT(OS_QUEUE_OK == OS_QUEUE_PUT(PEQueueId[PortNum], &event, OS_QUEUE_FOREVER));
}

#if !defined(USBPD_TCPM_MODULE_ENABLED)
/**
  * @brief  WakeUp CAD task
  * @retval None
  */
static void USBPD_DPM_CADTaskWakeUp(void)
{
  uint16_t event = 0xFFFFU;
  // seen hang here when manually calling USBPD_CAD_WakeUp
  //OS_ASSERT(OS_QUEUE_OK == OS_QUEUE_PUT(CADQueueId, &event, OS_QUEUE_FOREVER));
  OS_QUEUE_PUT(CADQueueId, &event, OS_MS_2_TICKS(500));
}
#endif /* !USBPD_TCPM_MODULE_ENABLED */

/**
  * @brief  Main task for PE layer
  * @param  argument Not used
  * @retval None
  */
static void PE_Task(uint32_t PortNum)
{
  uint32_t timing;
  uint16_t event;
#ifdef _LOW_POWER
  LPM_SetOffMode((LPM_Id_t)(LPM_PE_0 + PortNum), LPM_Disable);
#endif

  for (;;)
  {
	//printf("[PE]");

#if USBPD_PORT_COUNT == 2
	if (PortNum == 0)
	{
#endif
		// force SNK for port 0
		#if defined(USBPDCORE_VPD)
		if (USBPD_TRUE == DPM_Settings[PortNum].VPDSupport)
		{
			timing = USBPD_PE_StateMachine_SNKwVPD(PortNum);
		}
		else
		#endif
		{
			timing = USBPD_PE_StateMachine_SNK(PortNum);
		}
#if USBPD_PORT_COUNT == 2
	}
	else
	{
		// force SRC for port 1
		timing = USBPD_PE_StateMachine_SRC(PortNum);
	}
#endif

	// get message
	OS_QUEUE_GET(PEQueueId[PortNum], &event, (timing == portMAX_DELAY) ? portMAX_DELAY : OS_MS_2_TICKS(timing));

#if defined(USBPD_TCPM_MODULE_ENABLED)
	// During SRC tests, VBUS is disabled by the FUSB but the detection is not well done
	if ((DPM_Params[PortNum].PE_SwapOngoing == 0) && (USBPD_ERROR == USBPD_TCPM_VBUS_IsVsafe5V(PortNum)))
	{
		event = (PortNum << 8 | 2);
		OS_ASSERT(OS_QUEUE_OK == OS_QUEUE_PUT(AlarmMsgBox, &event, OS_QUEUE_FOREVER));
	}
#endif
  }
}

/**
  * @brief  Main task for PE layer on Port0
  * @param  argument Not used
  * @retval None
  */
static void USBPD_PE_Task_P0(void *argument)
{
  PE_Task(USBPD_PORT_0);
}

/**
  * @brief  Main task for PE layer on Port1
  * @param  argument Not used
  * @retval None
  */
static void USBPD_PE_Task_P1(void *argument)
{
  PE_Task(USBPD_PORT_1);
}

#if defined(USBPD_TCPM_MODULE_ENABLED)
/**
  * @brief  Main task for ALERT layer
  * @param  argument: Not used
  * @retval None
  */
void USBPD_ALERT_Task(void *argument)
{
  OS_BASE_TYPE res;
  uint16_t event;
  uint8_t port;
  for (;;)
  {
    res = OS_QUEUE_GET(AlarmMsgBox, &event, portMAX_DELAY);
    if (res != OS_QUEUE_EMPTY)
    {
	    port = (event >> 8);

#ifdef _TRACE
	    USBPD_TRACE_Add(USBPD_TRACE_TCPM, port, TCPM_TRACE_CORE_ALERT, (uint8_t *)(&(event.value.v)), 4);
#endif /* _TRACE */
	    USBPD_TCPM_alert(event);
	    HAL_NVIC_EnableIRQ(ALERT_GPIO_IRQHANDLER(port));
    }
  }
}
#else /* !USBPD_TCPM_MODULE_ENABLED */
/**
  * @brief  Main task for CAD layer
  * @param  argument Not used
  * @retval None
  */
void USBPD_CAD_Task(void *argument)
{
  //uint32_t timing;
  uint16_t event;

#ifdef _LOW_POWER
  UTIL_LPM_SetOffMode(LPM_CAD, UTIL_LPM_DISABLE);
#endif
  for (;;)
  {
#if 0
    timing = USBPD_CAD_Process();
    if (timing < 500)
    {
	    timing = 500;
    }
    OS_QUEUE_GET(CADQueueId, &event, (timing == portMAX_DELAY) ? portMAX_DELAY : OS_MS_2_TICKS(timing));
#else
    USBPD_CAD_Process();
    OS_QUEUE_GET(CADQueueId, &event, portMAX_DELAY);
#endif
  }
}
#endif /* USBPD_TCPM_MODULE_ENABLED */

/**
  * @brief  CallBack reporting events on a specified port from CAD layer.
  * @param  PortNum   The handle of the port
  * @param  State     CAD state
  * @param  Cc        The Communication Channel for the USBPD communication
  * @retval None
  */
void USBPD_DPM_CADCallback(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
  //printf("[CAD]");

  switch (State)
  {
#if defined(USBPDCORE_VPD)
    case USPPD_CAD_EVENT_VPD    :
#endif
    case USBPD_CAD_EVENT_ATTEMC :
    {
#ifdef _VCONN_SUPPORT
      DPM_Params[PortNum].VconnStatus = USBPD_TRUE;
#endif /* _VCONN_SUPPORT */
      DPM_ManageAttachedState(PortNum, State, Cc);
#ifdef _VCONN_SUPPORT
      DPM_CORE_DEBUG_TRACE(PortNum, "Note: VconnStatus=TRUE");
#endif /* _VCONN_SUPPORT */
      break;
    }

    case USBPD_CAD_EVENT_ATTACHED :
      DPM_ManageAttachedState(PortNum, State, Cc);
      break;
    case USBPD_CAD_EVENT_DETACHED :
    case USBPD_CAD_EVENT_EMC :
    {

      /* The ufp is detached */
      (void)USBPD_PE_IsCableConnected(PortNum, 0);
      /* Terminate PE task */
      if (DPM_PEThreadId_Table[PortNum] != NULL)
      {
#ifdef _LOW_POWER
        UTIL_LPM_SetStopMode(0 == PortNum ? LPM_PE_0 : LPM_PE_1, UTIL_LPM_ENABLE);
        UTIL_LPM_SetOffMode(0 == PortNum ? LPM_PE_0 : LPM_PE_1, UTIL_LPM_ENABLE);
#endif
       
        /* Kill PE task */
        OS_TASK_DELETE(DPM_PEThreadId_Table[PortNum]);
        DPM_PEThreadId_Table[PortNum] = NULL;
      }

      DPM_Params[PortNum].PE_SwapOngoing = USBPD_FALSE;
      DPM_Params[PortNum].ActiveCCIs = CCNONE;
      DPM_Params[PortNum].PE_Power   = USBPD_POWER_NO;
      USBPD_DPM_UserCableDetection(PortNum, State);
#ifdef _VCONN_SUPPORT
      DPM_Params[PortNum].VconnCCIs = CCNONE;
      DPM_Params[PortNum].VconnStatus = USBPD_FALSE;
      DPM_CORE_DEBUG_TRACE(PortNum, "Note: VconnStatus=FALSE");
#endif /* _VCONN_SUPPORT */

      /* manage the USB stack stop */
      USBPD_PE_Notification(PortNum, USBPD_NOTIFY_USBSTACK_STOP);
      break;
    }
    default :
      /* nothing to do */
      break;
  }
}

static void DPM_ManageAttachedState(uint8_t PortNum, USBPD_CAD_EVENT State, CCxPin_TypeDef Cc)
{
#ifdef _VCONN_SUPPORT
  if (CC1 == Cc)
  {
    DPM_Params[PortNum].VconnCCIs = CC2;
  }
  if (CC2 == Cc)
  {
    DPM_Params[PortNum].VconnCCIs = CC1;
  }
#endif /* _VCONN_SUPPORT */
  DPM_Params[PortNum].ActiveCCIs = Cc;
  (void)USBPD_PE_IsCableConnected(PortNum, 1);

  USBPD_DPM_UserCableDetection(PortNum, State);
  USBPD_PE_Notification(PortNum, USBPD_NOTIFY_USBSTACK_START);

  if (USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole)
  {
    /* Add a delay to postpone the 1st send of SRC capa
    FUS305 seems not react correctly if it sent too quickly */
    USBPD_DPM_WaitForTime(6);
  }

  // Create PE task
  if (DPM_PEThreadId_Table[PortNum] == NULL)
  {
	  // Create PE task
	  if (PortNum == USBPD_PORT_0)
	  {
		OS_TASK_CREATE( "PE_0",
			USBPD_PE_Task_P0,
			NULL,
			FREERTOS_PE_STACK_SIZE,
			FREERTOS_PE_PRIORITY,
			DPM_PEThreadId_Table[USBPD_PORT_0] );
		OS_ASSERT(DPM_PEThreadId_Table[USBPD_PORT_0]);
	  }
	  else
	  {
		OS_TASK_CREATE( "PE_1",
			USBPD_PE_Task_P1,
			NULL,
			FREERTOS_PE_STACK_SIZE,
			FREERTOS_PE_PRIORITY,
			DPM_PEThreadId_Table[USBPD_PORT_1] );
		OS_ASSERT(DPM_PEThreadId_Table[USBPD_PORT_1]);
	  }
  }
  else
  {
	  printf("usbpd (%d): pe task cannot start\r\n", PortNum);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
