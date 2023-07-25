/**
 ****************************************************************************************
 *
 * @file dsps_common.h
 *
 * @brief DSPS common
 *
 * Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
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
