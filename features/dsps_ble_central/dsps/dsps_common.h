/**
 ****************************************************************************************
 *
 * @file dsps_common.h
 *
 * @brief DSPS common
 *
 * Copyright (C) 2023. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef DSPS_COMMON_H_
#define DSPS_COMMON_H_

typedef enum {
        SPS_DIRECTION_IN  = 0x00,
        SPS_DIRECTION_OUT       ,
        SPS_DIRECTION_MAX
} SPS_DIRECTION;

/**
 * TX and RX queues used to buffer data between UART and BLE.
 * Queue size, high water mark, low water mark are defined.
 */
#ifndef TX_SPS_QUEUE_SIZE
   #define TX_SPS_QUEUE_SIZE (60)
#endif

#ifndef RX_SPS_QUEUE_SIZE
   #define RX_SPS_QUEUE_SIZE (60)
#endif

#ifndef DATA_THRESHOLD_TO_CAL_THROUGHPUT
   #define DATA_THRESHOLD_TO_CAL_THROUGHPUT  (20000)
#endif

#ifndef THROUGHPUT_CALCULATION_ENABLE
   #define THROUGHPUT_CALCULATION_ENABLE  (1)
#endif

#ifndef MTU_SIZE
   #define MTU_SIZE         (dg_configBLE_DATA_LENGTH_TX_MAX - 4) // 4-byte L2CAP header
#endif

#ifndef DSPS_RX_SIZE
   #define DSPS_RX_SIZE     (MTU_SIZE - 3) // Match the used MTU size, excluding the 3-byte ATT header
#endif

#define TX_QUEUE_HWM      ((TX_SPS_QUEUE_SIZE)*0.80)
#define TX_QUEUE_LWM      ((TX_SPS_QUEUE_SIZE)*0.10)
/* Because of packets on-the-air, be careful when increase the RX HWM */
#define RX_QUEUE_HWM      ((RX_SPS_QUEUE_SIZE)*0.60)
#define RX_QUEUE_LWM      ((RX_SPS_QUEUE_SIZE)*0.10)

#endif /* DSPS_DSPS_COMMON_H_ */
