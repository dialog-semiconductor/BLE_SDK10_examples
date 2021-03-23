 /****************************************************************************************
 *
 * @file i2c_tack.c
 *
 * @brief I2C master and slave tasks
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <ad_i2c.h>

#include "peripheral_setup.h"
#include "platform_devices.h"
#include "osal.h"
#include "i2c_task.h"

extern OS_TASK i2c_task_s;
extern OS_TASK i2c_task_m;

/* Retained symbols */

//__RETAINED static OS_EVENT signal_i2c_async_done;

/*
 * Error code returned after an I2C operation. It can be used
 * to identify the reason of a failure.
 */
__RETAINED static HW_I2C_ABORT_SOURCE I2C_error_code;


/**
 * global response string
 */
static unsigned char _response[I2C_RESPONSE_SIZE];

/**
 * Callback counters
 */
static int _count_read_req = 0;
static int _count_data_rdy = 0;
static int _count_data_rcvd = 0;
static int _count_data_sent = 0;

/**
 * Callbacks
 */

/*
 * Callback function for the I2C master asynchronous transactions:
 *
 * \param  error      Error code returned at the end of an I2C transaction.
 * \param  user_data  User data that can be passed and used within the function.
 */
void _i2c_master_async_read_done(void *user_data, HW_I2C_ABORT_SOURCE error)
{
        /* Read the error status code */
        I2C_error_code = error;

        /* Signal the master task that time for resuming has elapsed. */
        OS_TASK_NOTIFY_FROM_ISR(i2c_task_m, I2C_DATA_SENT_NOTIFY, eSetBits); //notify slave that data was sent
}

/*
 * Callback function on data sent from slave finished:
 *
 * \param  p            I2C handle.
 * \param  len          Transaction length.
 * \param  success      Error code returned at the end of an I2C transaction.
 * \param  user_data    User data that can be passed and used within the function.
 */
static void _i2c_slave_data_sent(ad_i2c_handle_t p, uint16_t len, bool success, void *user_data)
{
        _count_data_sent++;

        OS_TASK_NOTIFY_FROM_ISR(i2c_task_s, I2C_DATA_SENT_NOTIFY, eSetBits); //notify slave that data was sent
}

/*
 * Callback function on data received from master finished:
 *
 * \param  p            I2C handle.
 * \param  len          Transaction length.
 * \param  success      Error code returned at the end of an I2C transaction.
 * \param  user_data    User data that can be passed and used within the function.
 */
static void _i2c_slave_data_received(ad_i2c_handle_t p, uint16_t len, bool success, void *user_data)
{
        _count_data_rcvd++;

        memset(_response,0,I2C_RESPONSE_SIZE);
        sprintf((char*)_response, "REQ: %02d RDY: %02d RCVD: %02d SENT: %02d",
                _count_read_req,
                _count_data_rdy,
                _count_data_rcvd,
                _count_data_sent);

        OS_TASK_NOTIFY_FROM_ISR(i2c_task_s, I2C_DATA_RCVD_NOTIFY, eSetBits); //notify slave that data was received
}

/*
 * Callback function on master data ready for slave:
 *
 * \param  p          I2C handle.
 * \param  user_data  User data that can be passed and used within the function.
 */
static void _i2c_slave_data_ready(ad_i2c_handle_t p, void *user_data)
{
        _count_data_rdy++;
        OS_TASK_NOTIFY_FROM_ISR(i2c_task_s, I2C_DATA_RDY_NOTIFY, eSetBits); //notify slave that data is ready from master
}

/*
 * Callback function on data requested from slave by master :
 *
 * \param  p          I2C handle.
 * \param  user_data  User data that can be passed and used within the function.
 */
static void _i2c_slave_read_request(ad_i2c_handle_t p, void *user_data)
{
        _count_read_req++;
        OS_TASK_NOTIFY_FROM_ISR(i2c_task_s, I2C_READ_REQ_NOTIFY, eSetBits); //notify slave that data was requested
}

i2c_dev_slave_event_callbacks_t slave_callbacks = {
        .data_sent = _i2c_slave_data_sent,
        .data_received = _i2c_slave_data_received,
        .data_ready = _i2c_slave_data_ready,
        .read_request = _i2c_slave_read_request,
};

void i2c_master_task( void *pvParameters )
{

        unsigned char resp[I2C_RESPONSE_SIZE];
        ad_i2c_handle_t _master_handle = ad_i2c_open(I2C_MASTER_DEVICE);

        OS_TICK_TIME xNextWakeTime;
        static uint32_t loop_counter=0;
        static uint32_t transaction_counter=0;

        /* Initialise xNextWakeTime - this only needs to be done once. */
        xNextWakeTime = OS_GET_TICK_COUNT();

        for ( ;; ) {

                vTaskDelayUntil( &xNextWakeTime, COUNTER_FREQUENCY_MS );
                loop_counter++;
                if (loop_counter % (1000 / OS_TICKS_2_MS(COUNTER_FREQUENCY_MS)) == 0) {
                        transaction_counter++;
                        unsigned char * _req = (unsigned char *)"callbacks?";
                        printf("Write I2C [%s] : \n", _req);

#if (I2C_ASYNC_EN)
                        uint32_t notif = 0;
                        I2C_error_code = ad_i2c_write_read_async(_master_handle,
                                                 _req,
                                                 I2C_REQUEST_SIZE,
                                                 resp,
                                                 I2C_RESPONSE_SIZE,
                                                 _i2c_master_async_read_done,
                                                 NULL,
                                                 HW_I2C_F_ADD_STOP);

        /*
         * In the meantime and while I2C transactions are performed in the background,
         * application task can proceed to other operations/calculation.
         * It is essential that, the new operations do not involve I2C transactions
         * on the already occupied bus!!!
         */

         /* Wait here until the current asynchronous I2C operation is done.
                        OS_EVENT_WAIT(signal_i2c_async_done, OS_EVENT_FOREVER);*/
                        OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);

#else
                 I2C_error_code = ad_i2c_write_read(_master_handle,
                                                   _req,
                                                   I2C_REQUEST_SIZE,//reduce this to simulate incomplete send
                                                   resp,
                                                   I2C_RESPONSE_SIZE,
                                                   HW_I2C_F_ADD_STOP);
#endif
                        if(0 == I2C_error_code){
                                printf("Successfully read I2C:  [%s] \n", resp);

                        } else {
                                printf("i2c read error(%d)\n", I2C_error_code);
                        }
                        fflush(stdout);
                }
        }
        ad_i2c_close(_master_handle, true);
        OS_TASK_DELETE( NULL ); //should never get here
}

void i2c_slave_task( void *pvParameters )
{
        memset(_response,0,I2C_RESPONSE_SIZE);

        ad_i2c_handle_t _slave_handle = ad_i2c_open(I2C_SLAVE_DEVICE);
        uint32_t notif = 0;
        unsigned char req[I2C_REQUEST_SIZE];
        for ( ;; ) {

                //wait for master to send request, send response
                ad_i2c_start_slave(_slave_handle, _response, I2C_RESPONSE_SIZE,
                                                 req, I2C_REQUEST_SIZE, &slave_callbacks, NULL);
                // wait until data is received
                OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                //wait until data is sent
                OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
        }
        ad_i2c_close(_slave_handle, true);
        OS_TASK_DELETE( NULL ); //should never get here
}

