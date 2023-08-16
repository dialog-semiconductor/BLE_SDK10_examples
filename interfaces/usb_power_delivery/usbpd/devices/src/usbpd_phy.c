/**
******************************************************************************
* @file    usbpd_phy.c
* @author  AMG Application Team
* @brief   This file contains PHY layer functions.
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

#include "stdio.h"
#include "string.h"
#include "usbpd_def.h"
#include "usbpd_porthandle.h"
#include "usbpd_stusb_dpm_if.h"
#include "usbpd_pwr_if.h"
#include "usbpd_dpm_user.h"
#include "usbpd_phy.h"

extern STUSB16xx_PORT_HandleTypeDef Ports[];
extern const uint32_t crc32_tab[] ;

/**
* @brief Private struct to store buffers and Tx variables
*/
#ifdef __STAT
typedef struct
{
  uint32_t TxCount;
  uint32_t TxBusy;
  uint32_t TxSent;
  uint32_t TxError;
  uint32_t ResetCount;
  uint32_t ResetError;
  
  /* Rx stats */
  uint32_t RxSopCorrected;
  uint32_t RX2Count;
  uint32_t RX2Special;
  uint32_t RX2Ok;
  uint32_t RX2Error;
  uint32_t RX2ErrorEOP;
  uint32_t RX2ErrorCRC;
  uint32_t RX2ErrorUnsupportedSOP;
  uint32_t RX2ErrorInvalidSOP;
  uint32_t RX2ErrorInvalidSymbol;
  
} PHY_StatsTypeDef;
#endif

/**
* @brief Status available for the PHY layer
*/
typedef enum
{
  PHY_StateNone         = 0,  /*!< PHY State Not available              */
  PHY_StateWaitingIdle  = 1,  /*!< PHY State Idle                       */
  PHY_StateBusy         = 2,  /*!< PHY State Busy                       */
  PHY_StateBusyTxStart  = 3,  /*!< PHY State Busy, transmission process */
  PHY_StateBusyRx       = 4,  /*!< PHY State Busy, reception process    */
  PHY_StateBusyBIST_Tx  = 5,  /*!< PHY State Busy, TX BIST              */
  PHY_StateBusyBIST_Rx  = 6,  /*!< PHY State Busy, RX BIST              */
} PHY_State;

/**
* @brief structure to support the decoding on fly during the reception
*/
typedef struct
{
  USBPD_PHY_RX_Status_TypeDef Status;       /*!< Status of the reception process   */
  uint32_t OrderSet;                        /*!< Last orderset received            */
  USBPD_SOPType_TypeDef MsgType;            /*!< Type of the incoming message      */
  uint32_t DataCount;                       /*!< Counting of received data         */
  uint32_t   Rx_CRC;                         /* CRC storage */  
  uint32_t   Calc_CRC;                       /* CRC storage for on going calculation */
} PHY_RxDecodingTypeDef;

/**
* @brief Handle to support the data of the layer
*/
typedef struct
{
  const USBPD_PHY_Callbacks  *cbs;                /*!< callbacks of the USBPD_CORE_PRL */
  uint8_t*   pRxBuffer;                     /*!< buffer provided by the USBPD_CORE_PRL */
  PHY_State  State;                         /*!< Current state of the PHY layer @ref PHY_State */
  uint8_t*   pTxBuffer;                     /*!< Temporary TX buffer in 5B coding (32 bit size), no BMC coding, ready for USBPD_HW_IF */
  uint32_t   TxDatabitLen;                  /*!< TX buffer len (bits) */
  PHY_RxDecodingTypeDef RxDec;              /*!< var to support the RX decoding */
  uint32_t   SupportedSOP;                  /*!< bit field SOP"Debug SOP'Debug SOP" SOP' SOP */
  uint8_t    RxEnable;                      /*!< Flag to enable reception */
#ifdef __STAT
  PHY_StatsTypeDef Stats;                     /*!< Statistics (Only for debug scope) */
#endif
} PHY_HandleTypeDef;

/**
* @brief prototype definition shared in several callbacks
*/
typedef void (*PHY_CB_t)(uint8_t PortNum, USBPD_SOPType_TypeDef Type); 

/* Private define and macro --------------------------------------------------*/
/** @defgroup USBPD_DEVICE_PHY_Private_macros USBPD DEVICE PHY Private macros
* @brief defines and macros to support the coding/decoding through lookup table and bit operations
* @{
*/
#define __SIZE     ((uint8_t)4)               /*!< Size in bytes of the coding/decoding variables */
#define __SIZEBIT   ((uint16_t)((__SIZE)*8))  /*!< Size in bits of the coding/decoding variables */
#define __BITMASK(__VAL__) ((1<<(__VAL__))-1) /*!< Create a n bit mask i.e. n=5 => 0x1F */


/** @defgroup USBPD_DEVICE_PHY_Key_Code K-Codes 
* @brief  K-Codes available in the Symbol Encoding Table
* @details  K-Codes available in the Symbol Encoding Table, according to the USBPD specifications
* @{
*/
#define KC_S_SYNC1 0x18 /*!< Startsynch #1 */
#define KC_S_SYNC2 0x11 /*!< Startsynch #2 */
#define KC_S_SYNC3 0x06 /*!< Startsynch #3 */
#define KC_S_RST1  0x07 /*!< Hard Reset #1 */
#define KC_S_RST2  0x19 /*!< Hard Reset #2 */
#define KC_S_EOP   0x0D /*!< EOP End Of Packet */
#define KC_BITSIZE 0x05 /*!< Size of in bits of the K-Code */

#define SYM_FOR_WORD        6  /*!< Number of symbols inside a 32bit word for coding/decoding */

/** @defgroup USBPD_DEVICE_PHY_Order_Set Order Set definition and tools
* @brief  Tools for ordersets and defined items
* @details  Tools to  create the ordersets by the K-Codes and definition, refer to the USBPD specifications
* @{
*/

/**
* @brief Orderset creation through the 4 K-Codes
*/
#define OS_MAKE(_KC1_, _KC2_, _KC3_, _KC4_)   ((_KC1_) | ((_KC2_)<<5) | ((_KC3_)<<10) | ((_KC4_)<<15))
#define OS_KC_NUM   4                                                               /*!< Num of K-Code in a Orderset */
#define OS_BITSIZE ((KC_BITSIZE) * (OS_KC_NUM))                                     /*!< Orderset size in bits */
#define OS_MASK (__BITMASK(OS_BITSIZE))                                             /*!< Orderset mask, 20 bits => 0xFFFFF */
#define OS_NUM              7                                                       /*!< Number of predefined Orderedsets */
#define OS_SYM_SOP         OS_MAKE(KC_S_SYNC1, KC_S_SYNC1, KC_S_SYNC1, KC_S_SYNC2)  /*!< SOP : Start of Packet Sequence */
#define OS_SYM_SOP_1       OS_MAKE(KC_S_SYNC1, KC_S_SYNC1, KC_S_SYNC3, KC_S_SYNC3)  /*!< Prime SOP : Start of Packet Sequence Prime */
#define OS_SYM_SOP_2       OS_MAKE(KC_S_SYNC1, KC_S_SYNC3, KC_S_SYNC1, KC_S_SYNC3)  /*!< Double SOP : Start of Packet Sequence Double */
#define OS_SYM_SOP_DBG_1   OS_MAKE(KC_S_SYNC1, KC_S_RST2,  KC_S_RST2,  KC_S_SYNC3)  /*!< Prime Debug SOP : Start of Packet Sequence Prime Debug */
#define OS_SYM_SOP_DBG_2   OS_MAKE(KC_S_SYNC1, KC_S_RST2,  KC_S_SYNC3, KC_S_SYNC2)  /*!< Double Debug SOP : Start of Packet Sequence Double Debug */
#define OS_SYM_HARD_RESET  OS_MAKE(KC_S_RST1,  KC_S_RST1,  KC_S_RST1,  KC_S_RST2 )  /*!< Hard Reset */
#define OS_SYM_CABLE_RESET OS_MAKE(KC_S_RST1,  KC_S_SYNC1, KC_S_RST1,  KC_S_SYNC3)  /*!< Cable Reset */

#define CODE_5B_INVALID 0xFF                                                    /*!< Invalid symbol */
#define CODE_5B_20BIT_INVALID 0xFFFFFFFF                                        /*!< Invalid group symbols */
#define CODE_5B_20BIT_IS_INVALID(__VAL__) ((__VAL__) == CODE_5B_20BIT_INVALID)  /*!< Group symbols validation  */

/* Bit Sizes */
#define CODE_5B_ITEM1_BITSIZE KC_BITSIZE             /*!< Symbol size in bits, the same of @ref KC_BITSIZE */
#define CODE_5B_ITEM1_MASK (__BITMASK(CODE_5B_ITEM1_BITSIZE))   /*!< Symbol mask, 5 bits => 0x1F */
#define CODE_5B_ITEM2_BITSIZE (CODE_5B_ITEM1_BITSIZE * 2)     /*!< Two-Symbol size in bits */
#define CODE_5B_ITEM4_BITSIZE (CODE_5B_ITEM1_BITSIZE * 4)     /*!< Four-Symbol size in bits */


/** @defgroup USBPD_DEVICE_PHY_Private_variables USBPD DEVICE PHY Private variables
* @brief PHY variable and const, 
* @details In this section there are the lookup tables: @ref coding4b5b and @ref decoding5b4b
*          useful to improve the performance of the coding/decoding processes
* @{
*/

/**
* @brief List of valid Order sets
*/
const uint32_t OrderSets[OS_NUM] =
{
  OS_SYM_SOP          , /*!<  SOP*  MESSAGES       */
  OS_SYM_SOP_1        , /*!<  SOP'  MESSAGES       */
  OS_SYM_SOP_2        , /*!<  SOP'' MESSAGES       */
  OS_SYM_SOP_DBG_1    , /*!<  SOP'  DEBUG_MESSAGES */
  OS_SYM_SOP_DBG_2    , /*!<  SOP'' DEBUG_MESSAGES */
  OS_SYM_HARD_RESET   , /*!<  HARD RESET MESSAGE   */
  OS_SYM_CABLE_RESET  , /*!<  CABLE RESET MESSAGE  */
};

/**
* @brief Lookup Table 4b/5b coding
*/
const uint8_t coding4b5b[] =
{
  0x1E, /*!< 0 = 0000 => 11110 */
  0x09, /*!< 1 = 0001 => 01001 */
  0x14, /*!< 2 = 0010 => 10100 */
  0x15, /*!< 3 = 0011 => 10101 */
  0x0A, /*!< 4 = 0100 => 01010 */
  0x0B, /*!< 5 = 0101 => 01011 */
  0x0E, /*!< 6 = 0110 => 01110 */
  0x0F, /*!< 7 = 0111 => 01111 */
  0x12, /*!< 8 = 1000 => 10010 */
  0x13, /*!< 9 = 1001 => 10011 */
  0x16, /*!< A = 1010 => 10110 */
  0x17, /*!< B = 1011 => 10111 */
  0x1A, /*!< C = 1100 => 11010 */
  0x1B, /*!< D = 1101 => 11011 */
  0x1C, /*!< E = 1110 => 11100 */
  0x1D, /*!< F = 1111 => 11101 */
};

/**
* @brief Lookup Table 5b/4b decoding
* @details Lookup Table 5b/4b decoding to quick decoding of the symbols
*/
const uint8_t decoding5b4b[] =
{
  CODE_5B_INVALID, /* 00 = 00000 => E--- */
  CODE_5B_INVALID, /* 01 = 00001 => E--- */
  CODE_5B_INVALID, /* 02 = 00010 => E--- */
  CODE_5B_INVALID, /* 03 = 00011 => E--- */
  CODE_5B_INVALID, /* 04 = 00100 => E--- */
  CODE_5B_INVALID, /* 05 = 00101 => E--- */
  CODE_5B_INVALID, /* 06 = 00110 => E--- */
  CODE_5B_INVALID, /* 07 = 00111 => E--- */
  CODE_5B_INVALID, /* 08 = 01000 => E--- */
  0x01,            /* 09 = 01001 => 0001 */
  0x04,            /* 0A = 01010 => 0100 */
  0x05,            /* 0B = 01011 => 0101 */
  CODE_5B_INVALID, /* 0C = 01100 => E--- */
  CODE_5B_INVALID, /* 0D = 01101 => E--- */
  0x06,            /* 0E = 01110 => 0110 */
  0x07,            /* 0F = 01111 => 0111 */
  CODE_5B_INVALID, /* 10 = 10000 => E--- */
  CODE_5B_INVALID, /* 11 = 10001 => E--- */
  0x08,            /* 12 = 10010 => 1000 */
  0x09,            /* 13 = 10011 => 1001 */
  0x02,            /* 14 = 10100 => 0010 */
  0x03,            /* 15 = 10101 => 0011 */
  0x0A,            /* 16 = 10110 => 1010 */
  0x0B,            /* 17 = 10111 => 1011 */
  CODE_5B_INVALID, /* 18 = 11000 => E--- */
  CODE_5B_INVALID, /* 19 = 11001 => E--- */
  0x0C,            /* 1A = 11010 => 1100 */
  0x0D,            /* 1B = 11011 => 1101 */
  0x0E,            /* 1C = 11100 => 1110 */
  0x0F,            /* 1D = 11101 => 1111 */
  0x00,            /* 1E = 11110 => 0000 */
  CODE_5B_INVALID, /* 1F = 11111 => E--- */
};

/** Internal struct for RXTX ref to @ref PHY_HandleTypeDef */
static PHY_HandleTypeDef PHY_Ports[USBPD_PORT_COUNT];


/* Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_DEVICE_PHY_Private_functions USBPD DEVICE PHY Private functions
* @{
*/
USBPD_StatusTypeDef         PHY_PortInit(uint8_t PortNum, const USBPD_PHY_Callbacks *pCallback, uint8_t *pRxBuffer, uint32_t SupportedSOP);
USBPD_StatusTypeDef         PHY_PreparePacket(uint8_t PortNum, USBPD_SOPType_TypeDef Type, uint8_t* pPacketBuffer, uint16_t Size);
void                        PHY_TxBuffer_Reset(uint8_t PortNum);
void                        PHY_BistCompleted(uint8_t PortNum, USBPD_BISTMsg_TypeDef bistmode);
void                        PHY_TxCompleted(uint8_t portnum, uint32_t Status);
USBPD_SOPType_TypeDef       PHY_SopDetect(uint8_t PortNum, uint32_t OrderSet);
void                        PHY_TxBuffer_Append(uint8_t PortNum, uint32_t val, uint16_t nbit);
void                        USBPD_PHY_ResetCompleted(uint8_t PortNum, USBPD_SOPType_TypeDef Type);
USBPD_PHY_RX_Status_TypeDef PHY_Rx_Reset(uint8_t PortNum);
USBPD_PHY_RX_Status_TypeDef PHY_Rx_Accumulate(uint8_t PortNum, uint32_t data); /* 10 bits => 1 byte */
USBPD_PHY_RX_Status_TypeDef PHY_Rx_Completed(uint8_t PortNum);
#ifdef __STAT
void PHY_Stat_Reset(uint8_t PortNum);
#endif /* __STAT */

#ifdef _MSG_TRACE
const char *usbpd_msg_ctr[] = { "rsv", "goodcrc", "gotomin", "accept", "reject", "ping", "psrdy", "srccap", "sinkcap", "drswap", "prswap", "vconnswap", "wait", "softrst", "datarst", "datarstc", "notsup", "srcext", "status", "frswap", "pps", "country", "snkext", "srcinfo", "revision" };
const char *usbpd_msg_dat[] = { "rsv", "srccap", "request", "bist", "sinkcap", "battery", "alert", "country", "usb", "eprreq", "eprmode", "srcinfo", "revision", "rsv1", "rsv2", "vendor" };

static USBPD_StatusTypeDef PHY_TraceMsg(uint8_t PortNum, uint8_t *buf, uint16_t len, bool tx)
{
  uint8_t hd_ext = buf[1] & 0x80;
  uint8_t hd_dat = (buf[1] & 0x70) >> 4;
  uint8_t hd_id = (buf[1] & 0x0e) >> 1;
  //uint8_t hd_role = buf[1] & 0x01;
  uint8_t hd_type = buf[0] & 0x1f;
  char *id = tx ? "tx" : "rx";

  if (hd_ext)
  {
    printf("usbpd (%d): %s ext type %d\r\n", PortNum, id, hd_type);
    return USBPD_NOTSUPPORTED;
  }
  else if (hd_dat)
  {
    printf("usbpd (%d): %s data %s (%d)\r\n", PortNum, id, (hd_type > 15) ? "?" : usbpd_msg_dat[hd_type], hd_id);
  }
  else
  {
    printf("usbpd (%d): %s %s (%d)\r\n", PortNum, id, (hd_type > 24) ? "?" : usbpd_msg_ctr[hd_type], hd_id);
  }
  return USBPD_OK;
}
#endif

/**
* @brief  Initialize the PHY of a specified port.
* @param  PortNum       Number of the port.
* @param  pCallbacks    PHY callbacks
* @param  pRxBuffer     Buffer to storage received message.
* @param  PowerRole     Power Role of the board.
* @param  SupportedSOP  bit field of the supported SOP
* @retval status        @ref USBPD_OK
*/
USBPD_StatusTypeDef USBPD_PHY_Init(uint8_t PortNum, const USBPD_PHY_Callbacks *pCallbacks, uint8_t *pRxBuffer, USBPD_PortPowerRole_TypeDef PowerRole, uint32_t SupportedSOP)
{
  /* set all callbacks */
  USBPD_HW_IF_Callbacks hwif_cbs;
  hwif_cbs.USBPD_HW_IF_TxCompleted    = PHY_TxCompleted;
  hwif_cbs.USBPD_HW_IF_BistCompleted  = PHY_BistCompleted;
  hwif_cbs.USBPD_HW_IF_RX_Reset       = PHY_Rx_Reset;
  hwif_cbs.USBPD_HW_IF_RX_Accumulate  = PHY_Rx_Accumulate;
  hwif_cbs.USBPD_HW_IF_RX_Completed   = PHY_Rx_Completed;
  
  /* Initialize the hardware for the port */
  USBPD_HW_IF_PortHwInit(PortNum, hwif_cbs, PowerRole);
  
  /* Initialize port related functionalities inside this layer */
  if (PHY_PortInit(PortNum, pCallbacks, pRxBuffer, SupportedSOP))
  {
    return USBPD_ERROR;
  }
  
#ifdef __STAT
  PHY_Stat_Reset(PortNum);
#endif
  
  return USBPD_OK;
}

/**
* @brief  this function return the retry counter value in us.
* @note   time used to determine when the protocol layer must re-send a message not aknowledge by a goodCRC
* @param  PortNum    Number of the port.
* @retval retry counter value in us.
*/
uint32_t USBPD_PHY_GetRetryTimerValue(uint8_t PortNum)
{
  return 920u;
}

/**
* @brief  this function return the min time to wait before sending a goodCRC to ack a message (in us).
* @note   time used to guarantee the min time of 26us between two PD message.
* @param  PortNum    Number of the port.
* @retval value in us.
*/
uint16_t USBPD_PHY_GetMinGOODCRCTimerValue(uint8_t PortNum)
{
  return 0u; /* PE launch timer TIM_PORT0_CRC or TIM_PORT1_CRC to avoid sending CRC too early thoses 2 timer are stubify in STUSB1602 */
}

/**
* @brief  Reset the PHY of a specified port.
* @param  PortNum    Number of the port.
* @retval None
*/
void USBPD_PHY_Reset(uint8_t PortNum)
{
  /* reset PHY layer */
  memset( Ports[PortNum].pRxBuffPtr, 0x00, TXRX_BUFFER_SIZE);
  memset( Ports[PortNum].pTxBuffPtr, 0x00, TXRX_BUFFER_SIZE);
  PHY_TxBuffer_Reset(PortNum);
}

/**
* @brief  Request to send a reset on a port.
* @param  PortNum      Number of the port
* @param  Type      Type of reset (hard or cable reset) @ref USBPD_SOPType_TypeDef
* @retval USBPD_StatusTypeDef status
*/
USBPD_StatusTypeDef USBPD_PHY_ResetRequest(uint8_t PortNum, USBPD_SOPType_TypeDef Type)
{
  /* Send the requested reset */
#ifdef __STAT
  PHY_Ports[PortNum].Stats.ResetCount++;
#endif
  if (USBPD_PHY_SendMessage(PortNum, Type, NULL, 0) != USBPD_OK)
  {
#ifdef __STAT
    PHY_Ports[PortNum].Stats.ResetError++;
#endif
    return USBPD_ERROR;
  }
  
  PHY_Ports[PortNum].State = PHY_StateBusy;
  
  /* Send reset information to PRL layer */
  if (PHY_Ports[PortNum].cbs->USBPD_PHY_ResetCompleted != NULL)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_ResetCompleted(PortNum, Type);
  }
  
  PHY_Ports[PortNum].State = PHY_StateNone;
  
  return USBPD_OK;
}

/**
* @brief  Send a Message.
* @param  PortNum     Number of the port
* @param  Type      Type of the message
* @param  pBuffer      Pointer to the buffer to be transmitted
* @param  Size      Size of the buffer (bytes)
* @retval USBPD_StatusTypeDef status
*/
USBPD_StatusTypeDef USBPD_PHY_SendMessage(uint8_t PortNum, USBPD_SOPType_TypeDef Type, uint8_t *pBuffer, uint16_t Size)
{
  USBPD_StatusTypeDef res = USBPD_FAIL;
#ifdef __STAT
  PHY_Ports[PortNum].Stats.TxCount++;
#endif
  if (PHY_Ports[PortNum].State != PHY_StateNone)
  {
    
#ifdef __STAT
    PHY_Ports[PortNum].Stats.TxBusy++;
#endif
    printf("usbpd (%d): tx while phy busy in state %d\r\n", PortNum, PHY_Ports[PortNum].State);
    return USBPD_BUSY;
  }
  /* Prepare the packet to be sent, structure: <SOP><DATA><CRC><EOP> */
  res = PHY_PreparePacket(PortNum, Type, pBuffer, Size);
  
  /* if ok pass the packet to the HW_IF to be sent */
  if (res == USBPD_OK)
  {
    
    if (Size == 20 )
      __NOP();
    /* Setup the State of the port */
    PHY_Ports[PortNum].State = PHY_StateBusyTxStart;
    res = USBPD_HW_IF_SendBuffer(PortNum, (uint8_t *)Ports[PortNum].pTxBuffPtr,  PHY_Ports[PortNum].TxDatabitLen);
  }
  else if (res == USBPD_DISCARDRX)
  {
    /* ignore filtered messages */
    return USBPD_OK;
  }
  else
  {
    /* in any case return a generic error */
    printf("usbpd (%d): tx prep error %d\r\n", PortNum, res);
    return USBPD_ERROR;
  }
  
#ifdef __STAT
  if (USBPD_OK == res)
  {
    PHY_Ports[PortNum].Stats.TxSent++;
  }
  else
  {
    PHY_Ports[PortNum].Stats.TxError++;
    res = USBPD_FAIL;
  }
#endif
  /* reset the status of the port */
  PHY_Ports[PortNum].State = PHY_StateNone;
  
  return res;
}

/**
* @brief  Send BIST pattern.
* @param  PortNum    Number of the port
* @retval status    @ref USBPD_OK
*/
USBPD_StatusTypeDef USBPD_PHY_Send_BIST_Pattern(uint8_t PortNum)
{
  USBPD_StatusTypeDef res = USBPD_OK;
  
  /* Setup the State of the port */
  PHY_Ports[PortNum].State = PHY_StateBusyBIST_Tx;
  
  /* Call the low-level function (HW_IF) to accomplish the BIST Carrier Mode Transmission */
  USBPD_HW_IF_Send_BIST_Pattern(PortNum);
  
  /* reset the status of the port */
  PHY_Ports[PortNum].State = PHY_StateNone;
  return res;
}

/**
* @brief  Request PHY to exit of BIST mode 2
* @param  PortNum   port number value
* @param  mode      SOP BIST MODE 2
* @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_PHY_ExitTransmit(uint8_t PortNum, USBPD_SOPType_TypeDef mode)
{
  /* Empty function */
  //UNUSED(PortNum); /* To avoid warning */
  //UNUSED(mode); /* To avoid warning */
  return USBPD_OK;
}

/**
* @brief  Reset completed notification.
* @param  PortNum   Number of the port
* @param  Type      PD Type
* @retval None
*/
void USBPD_PHY_ResetCompleted(uint8_t PortNum, USBPD_SOPType_TypeDef Type)
{
  PHY_Ports[PortNum].State = PHY_StateBusy;
  
  /* perform a PHY layer reset */
  USBPD_PHY_Reset(PortNum);
  
  /* notify to upper level (PRL) */
  if (PHY_Ports[PortNum].cbs->USBPD_PHY_ResetCompleted != NULL)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_ResetCompleted(PortNum, Type);
  }
  
  /* reset the status of the port */
  PHY_Ports[PortNum].State = PHY_StateNone;
}

/**
* @brief  Set the SinkTxNg value of the resistor,
* @note   used to manage the collision avoidance
* @param  PortNum  Number of the port
* @retval None
*/
void USBPD_PHY_SetResistor_SinkTxNG(uint8_t PortNum)
{
  /* Call the low level function to change the exposed resistence */
  STUSB1602_Current_Advertised_Set(PortNum, USB_C_Current_1_5_A);
}

/**
* @brief  function to set the SinkTxOK
* @note   used to manage the collision avoidance
* @param  PortNum  Number of the port.
* @retval none.
*/
void USBPD_PHY_SetResistor_SinkTxOK(uint8_t PortNum)
{
  /* Call the low level function to change the exposed resistence */
  STUSB1602_Current_Advertised_Set(PortNum, USB_C_Current_3_0_A);
}

/**
* @brief  function to set the supported SOP
* @param  PortNum       Number of the port.
* @param  SOPSupported  List of the supported SOP
* @retval None.
*/
void USBPD_PHY_SOPSupported(uint8_t PortNum, uint32_t SOPSupported)
{
  PHY_Ports[PortNum].SupportedSOP = SOPSupported;
#ifdef _DEBUG_TRACE
  printf("usbpd (%d): sop supported = %lu\r\n", PortNum, SOPSupported);
#endif
}

/**
* @brief  Check if SinkTxOK is set or not
* @note   used to manage the collision avoidance
* @param  PortNum  Number of the port.
* @retval USBPD_TRUE or USBPD_FALSE
*/
uint8_t USBPD_PHY_IsResistor_SinkTxOk(uint8_t PortNum)
{
  /* Call the low level function to check the exposed resistence */
  return USBPD_16xx_IsResistor_SinkTxOk(PortNum);
}

/**
* @brief  function to generate an FRS signalling
* @param  PortNum  Number of the port.
* @retval None.
*/
void USBPD_PHY_FastRoleSwapSignalling(uint8_t PortNum)
{
}

/**
* @brief  function used to enable RX
* @param  PortNum    Number of the port.
* @retval None
*/
void USBPD_PHY_EnableRX(uint8_t PortNum)
{
  /* In case user application needs to implement a mechanism for enabling/disabling
  Rx chain with specific sequence, this function could be used to enable the Rx path.
  Implementation left on user side.
  */
  PHY_Ports[PortNum].RxEnable = 1;
  //printf("usbpd (%d): rx enabled\r\n", PortNum);
}

/**
* @brief  function used to disable RX
* @param  PortNum    Number of the port.
* @retval None
*/
void USBPD_PHY_DisableRX(uint8_t PortNum)
{
  /* In case user application needs to implement a mechanism for enabling/disabling
  Rx chain with specific sequence, this function could be used to disable the Rx path.
  Implementation left on user side.
  */
  PHY_Ports[PortNum].RxEnable = 0;
  //printf("usbpd (%d): rx disabled\r\n", PortNum);
}

/**
* @brief  Port initiatlization
* @param  PortNum       Number of the port
* @param  pCallback     PHY callbacks 
* @param  pRxBuffer     Pointer on the reception buffer
* @param  SupportedSOP  Supported SOP
* @retval USBPD Status
*/
USBPD_StatusTypeDef PHY_PortInit(uint8_t PortNum, const USBPD_PHY_Callbacks *pCallback, uint8_t *pRxBuffer, uint32_t SupportedSOP)
{
  /* Associate the RXBUF */
  PHY_Ports[PortNum].pRxBuffer    = pRxBuffer;
  PHY_Ports[PortNum].SupportedSOP = SupportedSOP;
  
  /* Associate the callbacks */
  PHY_Ports[PortNum].cbs = pCallback;
  memset( Ports[PortNum].pRxBuffPtr, 0x00, TXRX_BUFFER_SIZE);
  PHY_TxBuffer_Reset(PortNum);
  PHY_Ports[PortNum].State = PHY_StateNone;
  PHY_Ports[PortNum].RxEnable = 0;
  
  return USBPD_OK;
}

/**
* @brief  PRL Hard Reset indication from PHY
* @param  PortNum Port Number of the port
* @retval None
*/
void PHY_TxBuffer_Reset(uint8_t PortNum)
{
  //memset( Ports[PortNum].pTxRxBuffPtr, 0x00, TXRX_BUFFER_SIZE);
  memset( Ports[PortNum].pTxBuffPtr, TX_PREAMBLE, TX_PREAMBLE_SIZE);

  PHY_Ports[PortNum].pTxBuffer = Ports[PortNum].pTxBuffPtr + TX_PREAMBLE_SIZE;
  PHY_Ports[PortNum].TxDatabitLen = 0;
  PHY_Ports[PortNum].State = PHY_StateNone;
}

/**
* @brief  PRL Hard Reset indication from PHY
* @param  PortNum  Number of the port
* @param  val      Appending value to the TX buffer
* @param  nbit     Number of valid bits
* @retval None
*/
void PHY_TxBuffer_Append(uint8_t PortNum, uint32_t val, uint16_t nbit)
{
  uint32_t *pBuffer = (uint32_t *)PHY_Ports[PortNum].pTxBuffer;
  uint32_t *pOffset = (uint32_t *)&PHY_Ports[PortNum].TxDatabitLen;
  uint32_t map = __BITMASK(nbit);
  uint32_t value = val & map;
  
  uint32_t pos = *pOffset / __SIZEBIT;
  uint32_t bit = *pOffset % __SIZEBIT;
  
  /* append the bits */
  pBuffer[pos] |= (value << bit);
  /* checking if there are some bits for next byte */
  if (bit > (__SIZEBIT - MIN(nbit, __SIZEBIT)))
  {
    pBuffer[pos + 1] |= (value >> (__SIZEBIT - bit));
  }
  
  /* increment the offset according to the appended value */
  *pOffset += nbit;
}

/**
* @brief  Encode a byte (8bits) to 5B representation (output 10 bit)
* @param  val    Convert two nibble 8bits in two symbols (10bits)
* @retval None
*/
__STATIC_INLINE uint32_t PHY_Encode5b_Byte(uint8_t val)
{
  /* using the lookup table, achieve quickly the corresponding 5bit value */
  return coding4b5b[val & 0x0F] | (coding4b5b[(val >> 4) & 0x0F] << 5);
}

/**
* @brief  Encode a short (16bits) to its 5B representation (output 20 bit)
* @param  val    Convert four nibble 16bits in four symbols (20bits)
* @retval None
*/
__STATIC_INLINE uint32_t PHY_Encode5b_Short(uint16_t val)
{
  /* managed calling 2 times the byte conversion */
  return PHY_Encode5b_Byte(val & 0x00FF) | (PHY_Encode5b_Byte((val >> 8) & 0x00FF) << 10);
}

/**
* @brief  Prepare data packet
* @param  PortNum      Number of the port
* @param  Type        PD type
* @param  pBuffer     Pointer on the PD buffer
* @param  Size        Size of the PD buffer
* @retval USBPD Status
*/
USBPD_StatusTypeDef PHY_PreparePacket(uint8_t PortNum, USBPD_SOPType_TypeDef Type, uint8_t *pBuffer, uint16_t Size)
{
  uint32_t i, value, crc_value = 0xFFFFFFFF;
  uint8_t val8;
  /* get if it is a reset (hard or cable) */
  uint8_t resetRequired = ((Type == USBPD_SOPTYPE_HARD_RESET || Type == USBPD_SOPTYPE_CABLE_RESET )? 1 : 0);
  /* check the size, according to the following criteria: Exist a n : 2+(n+1)*4 = size, where n is the number of Objects */
  
  /* in case of chunked message,  header size needs to be removed */
  if (!resetRequired  && ((Size < 2) || (Size > MAX_DATA_LEN)))
  {
    /* the size is not a USBPD PRL message */
    return USBPD_ERROR;
  }
  
  if (!USBPD_PORT_IsValid(PortNum) || (!resetRequired && pBuffer == NULL))
  {
    return USBPD_ERROR;
  }

#ifdef USBPD_IGNORE_RESET_WHEN_READY
  // prevent library from messing up after successful handshake
  if (resetRequired && (DPM_Ports[PortNum].DPM_RequestedVoltage != 5000))
  {
    return USBPD_DISCARDRX;
  }
#endif

#ifdef _MSG_TRACE
  if (pBuffer)
  {
    USBPD_StatusTypeDef res = PHY_TraceMsg(PortNum, pBuffer, Size, true);
    if (res != USBPD_OK)
    {
      return res;
    }
  }
  else
  {
    printf("usbpd (%d): tx reset %d\r\n", PortNum, Type);
  }
#endif
  
  /* Clean the Tx buffer */
#ifdef USBPDCORE_UNCHUNCKED_MODE
  memset( Ports[PortNum].pTxBuffPtr, 0x00, 100);
#else
  memset( Ports[PortNum].pTxBuffPtr, 0x00, TXRX_BUFFER_SIZE);
#endif
  PHY_TxBuffer_Reset(PortNum);
  
  /* Start Of Packet SOP */
  PHY_TxBuffer_Append(PortNum, OrderSets[(uint8_t)Type], OS_BITSIZE);
  
  if (!resetRequired)
  {
    /* encoding data */
    for (i = 0; i < Size; i ++)
    {
      val8 = pBuffer[i] ;
      value = PHY_Encode5b_Byte(val8);
      PHY_TxBuffer_Append(PortNum, value, CODE_5B_ITEM2_BITSIZE);
      /* calculate crc in loop */
      crc_value ^= pBuffer[i] ;
      crc_value = crc32_tab[crc_value & 0xFF] ^ (crc_value >> 8) ;
    }
    crc_value ^= 0xFFFFFFFF;
    /* appending CRC (32 bits) */
    value = PHY_Encode5b_Short(crc_value & 0x0000FFFF);
    PHY_TxBuffer_Append(PortNum, value, CODE_5B_ITEM4_BITSIZE);
    value = PHY_Encode5b_Short((crc_value >> 16) & 0x0000FFFF);
    PHY_TxBuffer_Append(PortNum, value, CODE_5B_ITEM4_BITSIZE);
    
    /* appending EOP */
    PHY_TxBuffer_Append(PortNum, KC_S_EOP, CODE_5B_ITEM1_BITSIZE);
  }
  
  return USBPD_OK;
}

/**
* @brief  SOP Detect, according to the spec with not correct bits
* @param  PortNum  Number of the port
* @param  OrderSet Ordered Sets
* @retval SOP Type based on @ref USBPD_SOPType_TypeDef
*/
USBPD_SOPType_TypeDef PHY_SopDetect(uint8_t PortNum, uint32_t OrderSet)
{
  USBPD_SOPType_TypeDef type = USBPD_SOPTYPE_INVALID;
  
  uint32_t temp;
  uint32_t count, index;
  
  for (index = 0; index < OS_NUM; index++)
  {
    /* If the orderset mismatchs with one of the available(supported)
    orderset, the xor result is zero */
    temp = OrderSets[index] ^ OrderSet;
    /* counting number of wrong symbols */
    /* if we find more than 1 wrong symbol pass to next orderset */
    count = 0;
    /* if the orderset is correct temp is zero */
    if (temp)
    {
      /* check if the orderset contains less than 2 error */
      /* check first keycode */
      if (temp & 0x000F8000)
      {
        count++;
      }
      
      /* check second and if wrong continue */
      if (temp & 0x00007C00)
      {
        count++;
      }
      if (count > 1)
      {
        continue;
      }
      
      /* check third and if wrong continue */
      if (temp & 0x000003E0)
      {
        count++;
      }
      if (count > 1)
      {
        continue;
      }
      
      /* check last and if wrong continue */
      if (temp & 0x0000001F)
      {
        count++;
      }
      if (count > 1)
      {
        continue;
      }
    }
#ifdef __STAT
    if (count > 0)
    {
      PHY_Ports[PortNum].Stats.RxSopCorrected++;
    }
#endif
    /* found a valid orderset */
    type = (USBPD_SOPType_TypeDef)index;
    break;
  }
  
  return type;
}

/**
* @brief  Decoding 10 bits and return the byte (Decoding 5B/4B)
* @param  value    Masked to 10 bits
* @retval return   the decoded value or -1 
*/
__STATIC_INLINE int16_t Decode10Bit(uint32_t value)
{
  uint8_t v0,v1;
  
  v0 = decoding5b4b[(value >>  0) & 0x1F]; /* decoding first nibble */
  v1 = decoding5b4b[(value >>  5) & 0x1F]; /* decoding second nibble */
  
  /* check if there is an error */
  if (v0 == CODE_5B_INVALID || v1 == CODE_5B_INVALID) 
  {
    return -1;
  }
  
  /* create the decoded value */
  return v0 | (v1<<4);
}

/** @addtogroup USBPD_DEVICE_PHY_Private_functions_generic_callbacks generic callbacks functions
* @brief   PHY internally function used as callback from HW_IF layer
* @details 
* @{
*/
#ifdef __STAT
void PHY_Stat_Reset(uint8_t PortNum)
{
  memset(&PHY_Ports[PortNum].Stats, 0, sizeof(PHY_StatsTypeDef));
}
#endif

/**
* @brief  Callback to notify the BIST is completed
* @param  PortNum   Number of the port
* @param  bistmode  Modality of the BIST
* @retval none.
*/
void PHY_BistCompleted(uint8_t PortNum, USBPD_BISTMsg_TypeDef bistmode)
{
  if (PHY_Ports[PortNum].cbs->USBPD_PHY_BistCompleted)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_BistCompleted(PortNum,bistmode);
  }
}

/**
* @brief  Callback to notify the a transmission is completed
* @param  PortNum  Number of the port.
* @retval none.
*/
void PHY_TxCompleted(uint8_t PortNum, uint32_t Status)
{
  PHY_Ports[PortNum].State = PHY_StateNone;
  if (PHY_Ports[PortNum].cbs->USBPD_PHY_TxCompleted)
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_TxCompleted(PortNum, Status);
  }
}
/** 
* @}
*/

/** @addtogroup USBPD_DEVICE_PHY_Private_functions_reception_callbacks reception callbacks functions (called by the decoding on fly process)
* @brief PHY internally function used as callback from HW_IF layer for the decoding phase
* @details 
* @{
*/

/**
* @brief  Callback to notify the start of reception
* @param  PortNum  Number of the port.
* @retval Status of current reception.
*/
USBPD_PHY_RX_Status_TypeDef PHY_Rx_Reset(uint8_t PortNum)
{
  /* Get the pointer to the decoding structure */
  PHY_RxDecodingTypeDef *pRxData = (PHY_RxDecodingTypeDef *)&PHY_Ports[PortNum].RxDec;
  /* reset the status of the RX process */
  pRxData->Status = USBPD_PHY_RX_STATUS_INIT;
  
  /* reset the variable of count and memory */
  pRxData->DataCount = 0;
  //memset(PHY_Ports[PortNum].pRxBuffer, 0, __RX_DATA_LEN);  
  
  /* reset the type of SOP */
  pRxData->MsgType = USBPD_SOPTYPE_INVALID;
  
  /* reset the type of OrderSet received */
  pRxData->OrderSet = 0x00;
  
  return pRxData->Status;
}

/**
* @brief   Callback to notify the new data available for the current reception 
* @details A state machine to receive data and decode them on fly (during the reception the other bits)
* @param   PortNum  Number of the port.
* @param   data     New received data
* @retval  Status of current reception.
*/
USBPD_PHY_RX_Status_TypeDef PHY_Rx_Accumulate(uint8_t PortNum, uint32_t data) /* 10 bits => 1 byte */
{
  
  PHY_RxDecodingTypeDef *pRxData = (PHY_RxDecodingTypeDef *)&PHY_Ports[PortNum].RxDec;
  uint8_t *pRxbuf ;
  int16_t data4b_temp;
  
  
  switch (pRxData->Status)
  {
    /* at start-up the status is init, the preamble is stripped by the low level */
  case USBPD_PHY_RX_STATUS_INIT:
    /* received first part of the SOP, store it for next check */
    pRxData->OrderSet = data;
    
    /* move the status in SOP dection condition */
    pRxData->Status = USBPD_PHY_RX_STATUS_SOP_DETECTING;
    break;
    /* first information in the message is the SOP (after the preamble)*/
  case USBPD_PHY_RX_STATUS_MESSAGE_READY:
    break;
  case USBPD_PHY_RX_STATUS_SOP_DETECTING:
    /* received second part of the SOP */
    pRxData->OrderSet |= data<<10;
    
    /* SOP detection */
    pRxData->MsgType = PHY_SopDetect(PortNum, pRxData->OrderSet);
    
    /* move to the next state according to the received SOP */
    switch(pRxData->MsgType)
    {
    case USBPD_SOPTYPE_HARD_RESET :
      pRxData->Status = USBPD_PHY_RX_STATUS_MESSAGE_READY;
      break;
    case USBPD_SOPTYPE_CABLE_RESET :
      if(PHY_Ports[PortNum].SupportedSOP & 0x1E)
      {
        pRxData->Status = USBPD_PHY_RX_STATUS_MESSAGE_READY;
      }
      else
      {
        pRxData->Status = USBPD_PHY_RX_STATUS_ERROR_UNSUPPORTED_SOP;
      }
      break;
    case USBPD_SOPTYPE_SOP :
    case USBPD_SOPTYPE_SOP1 :
    case USBPD_SOPTYPE_SOP2 :
    case USBPD_SOPTYPE_SOP1_DEBUG :
    case USBPD_SOPTYPE_SOP2_DEBUG :
      if(PHY_Ports[PortNum].SupportedSOP & (0x1 << pRxData->MsgType))
      {
        /* move to state to receive data */
        pRxData->Status = USBPD_PHY_RX_STATUS_DATA;
        pRxData->Calc_CRC = 0xFFFFFFFF;
      }
      else
      {
        pRxData->Status = USBPD_PHY_RX_STATUS_ERROR_UNSUPPORTED_SOP;
      }
      break;
    default :
      pRxData->Status = USBPD_PHY_RX_STATUS_ERROR_INVALID_SOP;
      break;
    }
    
    break;
    
    /* state to receive data and decode it on fly */
  case USBPD_PHY_RX_STATUS_DATA:
    if ((data & CODE_5B_ITEM1_MASK) == KC_S_EOP)
    {
      /* Found a EOP, the message is ready for the check (format, size and crc) */
      pRxData->Status = USBPD_PHY_RX_STATUS_MESSAGE_READY;
    }
    else
    {
      /* decoding 10 bit */
      data4b_temp = Decode10Bit(data);
      
      if (data4b_temp == -1)
      {
        /* if there is an invalid symbol abort the process */
        pRxData->Status = USBPD_PHY_RX_STATUS_ERROR_INVALID_SYMBOL;
      }
      else
      { 
        /* add the decoded data (4B) to the buffer and increment the counter */
        if (pRxData->DataCount < __RX_DATA_LEN )
        {
          pRxbuf = PHY_Ports[PortNum].pRxBuffer + pRxData->DataCount++;
        }
        else
        {
          uint8_t index_byte = (pRxData->DataCount++ - __RX_DATA_LEN) % 4 ; /*overflow buff protection : if bist data are longuer that CRC is scrapted  __RX_DATA_LEN*/
          pRxbuf = (uint8_t *)&pRxData->Rx_CRC + index_byte; 
        }
        *pRxbuf =  (uint8_t )(data4b_temp & 0x00FF);

        /* Rx crc calculation */
        pRxData->Calc_CRC ^= *pRxbuf ;
        pRxData->Calc_CRC = crc32_tab[pRxData->Calc_CRC & 0xFF] ^ (pRxData->Calc_CRC >> 8) ;
      }
    }
    /* note: the status is the same, no move/change */
    break;
  default:
    /* reset the variable of count and memory */
    pRxData->DataCount = 0;
    pRxData->Rx_CRC = 0 ;
    break;
  }
  
  return pRxData->Status;
}

/**
* @brief  Callback to notify the end of the current reception
* @param  PortNum  Number of the port
* @retval Status of current reception
*/
USBPD_PHY_RX_Status_TypeDef PHY_Rx_Completed(uint8_t PortNum)
{
  PHY_RxDecodingTypeDef * pRxData = (PHY_RxDecodingTypeDef *)&PHY_Ports[PortNum].RxDec;

  if (PHY_Ports[PortNum].RxEnable == 0)
  {
    printf("usbpd (%d): rx while disabled\r\n", PortNum);
    return USBPD_PHY_RX_STATUS_INVALID;
  }

  if (pRxData->Status != USBPD_PHY_RX_STATUS_MESSAGE_READY)
  {
#ifdef __STAT
      PHY_Ports[PortNum].Stats.RX2Error++;
      
      switch (pRxData->Status)
      {
      case USBPD_PHY_RX_STATUS_ERROR_UNSUPPORTED_SOP:
        PHY_Ports[PortNum].Stats.RX2ErrorUnsupportedSOP++;
        break;
      case USBPD_PHY_RX_STATUS_ERROR_INVALID_SOP:
        PHY_Ports[PortNum].Stats.RX2ErrorInvalidSOP++;
        break;
      case USBPD_PHY_RX_STATUS_ERROR_INVALID_SYMBOL:
        PHY_Ports[PortNum].Stats.RX2ErrorInvalidSymbol++;
        break;
      case USBPD_PHY_RX_STATUS_ERROR_EOP_NOT_FOUND:
        PHY_Ports[PortNum].Stats.RX2ErrorEOP++;
        break;
      default :
        break;
      }
#endif
      return pRxData->Status;
  }
  
  if (pRxData->MsgType == USBPD_SOPTYPE_HARD_RESET || pRxData->MsgType == USBPD_SOPTYPE_CABLE_RESET)
  {
    /* received a hard reset, call the callback, no other checks */
    printf("usbpd (%d): rx reset %d\r\n", PortNum, pRxData->MsgType);
#ifdef __STAT
    PHY_Ports[PortNum].Stats.RX2Special++;
#endif
    if (PHY_Ports[PortNum].cbs->USBPD_PHY_ResetIndication != NULL)
    {
      PHY_Ports[PortNum].cbs->USBPD_PHY_ResetIndication(PortNum, pRxData->MsgType);
    }
    pRxData->Status = USBPD_PHY_RX_STATUS_OK;
    return USBPD_PHY_RX_STATUS_INVALID;
  }
  
  /* incoming message is a valid SOP with an EOP */
  if (pRxData->DataCount >=4)
  {
    if (pRxData->DataCount > (__RX_DATA_LEN + 4))
    {
      return pRxData->Status = USBPD_PHY_RX_STATUS_ERROR_CRC_FAILED;
    }
    /* calculate the crc for the incoming message */
    pRxData->Calc_CRC ^= 0xFFFFFFFF;
  }
  else
  {
    return pRxData->Status = USBPD_PHY_RX_STATUS_ERROR_CRC_FAILED;
  }

  /* if it isn't ok discard the incoming message */
  if (pRxData->Calc_CRC != 0x2144DF1C)
  {
#ifdef __STAT
    PHY_Ports[PortNum].Stats.RX2Error++;
    PHY_Ports[PortNum].Stats.RX2ErrorCRC++;
#endif
    return pRxData->Status = USBPD_PHY_RX_STATUS_ERROR_CRC_FAILED;
  }
  
#ifdef _MSG_TRACE
  if (PHY_TraceMsg(PortNum, PHY_Ports[PortNum].pRxBuffer, pRxData->DataCount, false) != USBPD_OK)
  {
    return USBPD_PHY_RX_STATUS_INVALID;
  }
#endif

  // FIXME why this filter?
  // message type 1 -> control message goodcrc / data message source capabilities
#if 0
  uint8_t FirstDataInBuf = *PHY_Ports[PortNum].pRxBuffer;
  if ((( FirstDataInBuf & 0x1F) != 0x01) &&  (Ports[PortNum].unwrapdata.exed_flag == 2))
  {
    return pRxData->Status = USBPD_PHY_RX_STATUS_ERROR_INVALID_SYMBOL;
  }
#endif

  /* call the callback */
#ifdef __STAT
  PHY_Ports[PortNum].Stats.RX2Ok++;
#endif

  if ((1 == PHY_Ports[PortNum].RxEnable) && (PHY_Ports[PortNum].cbs->USBPD_PHY_MessageReceived != NULL))
  {
    PHY_Ports[PortNum].cbs->USBPD_PHY_MessageReceived(PortNum, pRxData->MsgType);
  }
  
  return pRxData->Status = USBPD_PHY_RX_STATUS_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
