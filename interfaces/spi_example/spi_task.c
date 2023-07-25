 /****************************************************************************************
 *
 * @file spi_tack.c
 *
 * @brief SPI master and slave tasks
 *
 * Copyright (C) 2021 Renesas Electronics Corporation and/or its affiliates
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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <ad_spi.h>

#include "osal.h"

#include "peripheral_setup.h"
#include "platform_devices.h"
#include "hw_spi.h"

#define producer_FREQUENCY_MS                   OS_MS_2_TICKS(1000)
#define producer_SPI_TX_DATA_LENGTH             10
#define consumer_FREQUENCY_MS                   OS_MS_2_TICKS(3000)
#define consumer_DATA_ARRAY_BYTES               200
#define consumer_THRESHOLD                      100
#define consumer_SPI_RCV_CB                     (5 * producer_SPI_TX_DATA_LENGTH)       // Should be multiple of the TX side or 1

#define MUTEXED_PRINTF(f, args...)              {                                       \
                                                OS_MUTEX_GET(prt_m, OS_MUTEX_FOREVER);  \
                                                printf(f, ##args);                      \
                                                OS_MUTEX_PUT(prt_m);                    \
                                                }

__RETAINED uint8_t data_received[consumer_DATA_ARRAY_BYTES];
__RETAINED uint32_t data_rcv_cnt;
__RETAINED static OS_MUTEX prt_m;
__RETAINED static OS_EVENT spi_rcv;

void generate_tx_data(uint8_t *data, uint16_t size)
{
        static uint8_t temp;
        for(int i = 0; i < size; i++)
                *data++ = temp++;
}

void spi_producer_task( void *pvParameters )
{
        int err = 0;
        uint8_t tx_data[producer_SPI_TX_DATA_LENGTH];
        uint16_t tx_len = sizeof(tx_data);

        OS_MUTEX_CREATE(prt_m);

        ad_spi_handle_t spi_master_dev = ad_spi_open(SPI_MASTER_DEVICE);

        for ( ;; ) {
                OS_DELAY(producer_FREQUENCY_MS);

                generate_tx_data(tx_data, tx_len);
                ad_spi_activate_cs(spi_master_dev);
                err = ad_spi_write(spi_master_dev, tx_data, tx_len);
                ad_spi_deactivate_cs(spi_master_dev);

                if (err) {
                        MUTEXED_PRINTF("SPI Producer Tx Error (code %d)\n\r", err);
                } else {
                        MUTEXED_PRINTF("SPI Producer Tx OK - Len: %d \n\r", tx_len);
                }
                fflush(stdout);
        }
        ad_spi_close(spi_master_dev, true);

        OS_TASK_DELETE( NULL );
}

static void consumer_rcv_cb(void *user_data, uint16_t transferred)
{
        data_rcv_cnt += transferred;
        OS_EVENT_SIGNAL_FROM_ISR(spi_rcv);
}

void spi_consumer_task( void *pvParameters )
{
        memset(data_received, 0x00, sizeof(data_received));
        OS_EVENT_CREATE(spi_rcv);

        ad_spi_handle_t spi_slave_dev = ad_spi_open(SPI_SLAVE_DEVICE);

        for (;;) {
                ad_spi_read_async(spi_slave_dev, &data_received[data_rcv_cnt], consumer_SPI_RCV_CB, consumer_rcv_cb, NULL);
                OS_EVENT_WAIT(spi_rcv, consumer_FREQUENCY_MS);

                if(data_rcv_cnt >= consumer_THRESHOLD) {
                        MUTEXED_PRINTF("Successfully read SPI by consumer: %lu bytes \n\rData received ", data_rcv_cnt);
                        for(int i = 0; i < data_rcv_cnt; i++)
                                MUTEXED_PRINTF("%d ", data_received[i]);
                        MUTEXED_PRINTF("\n\r");
                        memset(data_received, 0x00, sizeof(data_received));
                        data_rcv_cnt = 0;
                } else {
                        MUTEXED_PRINTF("Consumer Task run, Threshold not reached yet \n\r");
                }

                fflush(stdout);
        }

        ad_spi_close(spi_slave_dev, true);

        OS_TASK_DELETE( NULL );
}

