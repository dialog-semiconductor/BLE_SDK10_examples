/**
 ****************************************************************************************
 *
 * @file dsps.h
 *
 * @brief Dialog Serial Port Service sample implementation API
 *
 * Copyright (C) 2022. Dialog Semiconductor Ltd, unpublished work. This computer
 * program includes Confidential, Proprietary Information and is a Trade Secret of
 * Dialog Semiconductor Ltd.  All use, disclosure, and/or reproduction is prohibited
 * unless authorized in writing. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef DSPS_H_
#define DSPS_H_

#include "ble_service.h"

#define UUID_DSPS                "0783b03e-8535-b5a0-7140-a304d2495cb7"
#define UUID_DSPS_SERVER_TX      "0783b03e-8535-b5a0-7140-a304d2495cb8"
#define UUID_DSPS_SERVER_RX      "0783b03e-8535-b5a0-7140-a304d2495cba"
#define UUID_DSPS_FLOW_CTRL      "0783b03e-8535-b5a0-7140-a304d2495cb9"

static const char dsps_tx_desc[] = "Server TX Data";
static const char dsps_rx_desc[] = "Server RX Data";
static const char dsps_flow_control_desc[] = "Flow Control";

/* Size of characteristics: match the MTU size */
static const uint16_t dsps_server_tx_size = 250;
static const uint16_t dsps_server_rx_size = 250;

/**
 * SPS Flow Control flags values
 */
typedef enum {
        DSPS_FLOW_CONTROL_ON = 0x01,
        DSPS_FLOW_CONTROL_OFF = 0x02,
} DSPS_FLOW_CONTROL;

typedef void (* dsps_set_flow_control_cb_t) (ble_service_t *svc, uint16_t conn_idx, DSPS_FLOW_CONTROL value);
typedef void (* dsps_rx_data_cb_t) (ble_service_t *svc, uint16_t conn_idx, const uint8_t *value, uint16_t length);
typedef void (* dsps_tx_done_cb_t) (ble_service_t *svc, uint16_t conn_idx);

/**
 * SPS application callbacks
 */
typedef struct {
        /** Remote client wrote new value of flow control characteristic */
        dsps_set_flow_control_cb_t set_flow_control;
        /** Data received from remote client */
        dsps_rx_data_cb_t          rx_data;
        /** Service finished TX transaction */
        dsps_tx_done_cb_t          tx_done;
} dsps_callbacks_t;

typedef struct {
        ble_service_t svc;

        dsps_callbacks_t *cb;

        uint16_t sps_tx_val_h;
        uint16_t sps_tx_ccc_h;

        uint16_t sps_rx_val_h;

        uint16_t sps_flow_ctrl_val_h;
        uint16_t sps_flow_ctrl_ccc_h;
} dsps_service_t;

/**
 * \brief Register Serial Port Service instance
 *
 * Function registers SPS instance
 *
 * \param [in] cb               application callbacks
 *
 * \return service instance
 *
 */
ble_service_t *dsps_init(dsps_callbacks_t *cb);

/**
 * \brief Set flow control value
 *
 * Function updates flow control value.
 *
 * \param [in] svc              service instance
 * \param [in] conn_idx         connection index
 * \param [in] value            flow control value
 *
 */
void dsps_set_flow_control(dsps_service_t *sps, uint16_t conn_idx, DSPS_FLOW_CONTROL value);

/**
 * \brief Send available TX data
 *
 * Function send available TX data through BLE to client. After sending data, service
 * will call tx_done callback.
 *
 * \param [in] svc              service instance
 * \param [in] conn_idx         connection index
 * \param [in] data             tx data
 * \param [in] length           tx data length
 *
 */
bool dsps_tx_data(dsps_service_t *sps, uint16_t conn_idx, uint8_t *data, uint16_t length);

#endif /* DSPS_H_ */
