/**
 ****************************************************************************************
 *
 * @file cps_client.c
 *
 * @brief Cycling Power Service Client implementation
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include "osal.h"
#include "ble_client.h"
#include "ble_gatt.h"
#include "ble_gattc.h"
#include "ble_gattc_util.h"
#include "ble_uuid.h"
#include "cps_client.h"

#define UUID_CYCLING_POWER_MEASUREMENT     (0x2A63)
#define UUID_CYCLING_POWER_FEATURE         (0x2A65)
#define UUID_CYCLING_POWER_SENSOR_LOCATION (0x2A5D)
#define UUID_CYCLING_POWER_VECTOR          (0x2A64)
#define UUID_CYCLING_POWER_CONTROL_POINT   (0x2A66)

#define PROCEDURE_TIMEOUT_MS    (30000)

/** GATT Server Characteristic Configuration bitmask values */
typedef enum {
        GATT_SCC_NONE           = 0x0000,
        GATT_SCC_BROADCAST      = 0x0001,
} gatt_scc_t;

#define CPS_CLIENT_CB(CLIENT, CB, ...)          \
        if (CB) {                               \
                CB(CLIENT, __VA_ARGS__);        \
        }

/**
 * Cycling Power Control Point Operation Opcodes
 */
typedef enum {
        /* No request send - only for saving last request */
        CPS_CLIENT_CP_OPCODE_NO_REQUEST = 0x00,
        /* Set Cumulative Value */
        CPS_CLIENT_CP_OPCODE_SET_CUMULATIVE_VALUE = 0x01,
        /* Update Sensor Location */
        CPS_CLIENT_CP_OPCODE_UPDATE_SENSOR_LOCATION = 0x02,
        /* Request Supported Sensor Locations */
        CPS_CLIENT_CP_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS = 0x03,
        /* Set Crank Length */
        CPS_CLIENT_CP_OPCODE_SET_CRANK_LENGTH = 0x04,
        /* Request Crank Length */
        CPS_CLIENT_CP_OPCODE_REQUEST_CRANK_LENGTH = 0x05,
        /* Set Chain Length */
        CPS_CLIENT_CP_OPCODE_SET_CHAIN_LENGTH = 0x06,
        /* Request Chain Length */
        CPS_CLIENT_CP_OPCODE_REQUEST_CHAIN_LENGTH = 0x07,
        /* Set Chain Weight */
        CPS_CLIENT_CP_OPCODE_SET_CHAIN_WEIGHT = 0x08,
        /* Request Chain Weight */
        CPS_CLIENT_CP_OPCODE_REQUEST_CHAIN_WEIGHT = 0x09,
        /* Set Span Length */
        CPS_CLIENT_CP_OPCODE_SET_SPAN_LENGTH = 0x0a,
        /* Request Span Length */
        CPS_CLIENT_CP_OPCODE_REQUEST_SPAN_LENGTH = 0x0b,
        /* Start Offset Compensation */
        CPS_CLIENT_CP_OPCODE_START_OFFSET_COMPENSATION = 0x0c,
        /* Mask Measurement Characteristic Content */
        CPS_CLIENT_CP_OPCODE_MASK_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT = 0x0d,
        /* Request Sampling Rate */
        CPS_CLIENT_CP_OPCODE_REQUEST_SAMPLING_RATE = 0x0e,
        /* Request Factory Calibration Date */
        CPS_CLIENT_CP_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE = 0x0f,
        /* Response Code */
        CPS_CLIENT_CP_OPCODE_RESPONSE_CODE = 0x20,
} cps_client_cp_opcode_t;

/**
 * Cycling Power Vector characteristic flags
 */
typedef enum {
        /* Crank Revolution Data - Cumulative Crank Revolutions */
        CPS_CLIENT_VECTOR_FLAG_CRANK_REVOLUTION_DATA_PRESENT_MASK = 0x01,
        /* First Crank Measurement Angle */
        CPS_CLIENT_VECTOR_FLAG_FIRST_CRANK_MEASUREMENT_ANGLE_PRESENT_MASK = 0x02,
        /* Instantaneous Force Magnitude Array */
        CPS_CLIENT_VECTOR_FLAG_INSTANT_FORCE_MAGNITUDE_ARRAY_PRESENT_MASK = 0x04,
        /* Instantaneous Torque Magnitude Array */
        CPS_CLIENT_VECTOR_FLAG_INSTANT_TORQUE_MAGNITUDE_ARRAY_PRESENT_MASK = 0x08,
        /* Instantaneous Measurement Direction */
        CPS_CLIENT_VECTOR_FLAG_INSTANT_MEASUREMENT_DIRECTION_MASK = 0x30,
} cps_vector_flag_t;

typedef struct {
        ble_client_t client;

        const cps_client_callbacks_t *cb; /* Callbacks */

        uint16_t measurement_h;        /* Cycling Power Measurement Handle */
        uint16_t measurement_ccc_h;    /* Cycling Power Measurement CCC */
        uint16_t measurement_scc_h;    /* Cycling Power Measurement SCC */

        uint16_t feature_h;            /* Cycling Power Feature Handle */

        uint16_t sensor_location_h;    /* Cycling Power Sensor Location Handle */

        uint16_t vector_h;             /* Cycling Power Vector Handle */
        uint16_t vector_ccc_h;         /* Cycling Power Vector CCC */

        uint16_t ctrl_point_h;         /* Cycling Power Control Point Handle */
        uint16_t ctrl_point_ccc_h;     /* Cycling Power Control Point CCC */

        uint8_t pending_opcode;
} cps_client_t;

/* Type used for client serialization */
typedef struct {
        uint16_t measurement_h;
        uint16_t measurement_ccc_h;
        uint16_t measurement_scc_h;

        uint16_t feature_h;

        uint16_t sensor_location_h;

        uint16_t vector_h;
        uint16_t vector_ccc_h;

        uint16_t ctrl_point_h;
        uint16_t ctrl_point_ccc_h;

        uint16_t start_h;
        uint16_t end_h;
} __attribute__((packed)) cps_client_serialized_t;

static void serialize(ble_client_t *client, void *data, size_t *length)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        cps_client_serialized_t *s_data = data;
        *length = sizeof(cps_client_serialized_t);

        if (!data) {
                return;
        }

        s_data->measurement_h = cps_client->measurement_h;
        s_data->measurement_ccc_h = cps_client->measurement_ccc_h;
        s_data->measurement_scc_h = cps_client->measurement_scc_h;
        s_data->feature_h = cps_client->feature_h;
        s_data->sensor_location_h = cps_client->sensor_location_h;
        s_data->vector_h = cps_client->vector_h;
        s_data->vector_ccc_h = cps_client->vector_ccc_h;
        s_data->ctrl_point_h = cps_client->ctrl_point_h;
        s_data->ctrl_point_ccc_h = cps_client->ctrl_point_ccc_h;
        s_data->start_h = cps_client->client.start_h;
        s_data->end_h = cps_client->client.end_h;
}

static void cleanup(ble_client_t *client)
{
        OS_FREE((cps_client_t *) client);
}

static void handle_cp_feature_value(cps_client_t *cps_client, att_error_t status,
                                                        uint16_t length, const uint8_t *value)
{
        uint32_t feat = 0;

        if (!cps_client->cb->read_feature_completed) {
                return;
        }

        if (status != ATT_ERROR_OK) {
                goto done;
        }

        if (length != sizeof(uint32_t)) {
                status = ATT_ERROR_UNLIKELY;
                goto done;
        }

        feat = get_u32(value);

done:
        cps_client->cb->read_feature_completed(&cps_client->client, status, feat);
}

static void handle_sensor_location_value(cps_client_t *cps_client, att_error_t status,
                                                        uint16_t length, const uint8_t *value)
{
        cps_client_sensor_location_t location = 0;

        if (!cps_client->cb->read_sensor_location_completed) {
                return;
        }

        if (status != ATT_ERROR_OK) {
                goto done;
        }

        if (length != sizeof(uint8_t)) {
                status = ATT_ERROR_UNLIKELY;
                goto done;
        }

        location = value[0];

done:
        cps_client->cb->read_sensor_location_completed(&cps_client->client, status, location);
}

static void handle_cp_measurement_ccc_value(cps_client_t *cps_client, att_error_t status,
                                                        uint16_t length, const uint8_t *value)
{
        gatt_ccc_t ccc_val = GATT_CCC_NONE;

        if (!cps_client->cb->get_event_state_completed) {
                return;
        }

        if (status == ATT_ERROR_OK && length < sizeof(uint16_t)) {
                status = ATT_ERROR_UNLIKELY;
                goto done;
        }

        ccc_val = (gatt_ccc_t) get_u16(value);
done:
        cps_client->cb->get_event_state_completed(&cps_client->client, status,
                                                                CPS_CLIENT_EVENT_MEASUREMENT_NOTIF,
                                                                ccc_val & GATT_CCC_NOTIFICATIONS);
}

static void handle_cp_measurement_scc_value(cps_client_t *cps_client, att_error_t status,
                                                        uint16_t length, const uint8_t *value)
{
        gatt_scc_t scc_val = GATT_SCC_NONE;

        if (!cps_client->cb->get_event_state_completed) {
                return;
        }

        if (status == ATT_ERROR_OK && length < sizeof(uint16_t)) {
                status = ATT_ERROR_UNLIKELY;
                goto done;
        }

        scc_val = (gatt_scc_t) get_u16(value);
done:
        cps_client->cb->get_event_state_completed(&cps_client->client, status,
                                                CPS_CLIENT_EVENT_MEASUREMENT_BROADCAST,
                                                scc_val & GATT_SCC_BROADCAST);
}

static void handle_cp_vector_ccc_value(cps_client_t *cps_client, att_error_t status,
                                                        uint16_t length, const uint8_t *value)
{
        gatt_ccc_t ccc_val = GATT_CCC_NONE;

        if (!cps_client->cb->get_event_state_completed) {
                return;
        }

        if (status == ATT_ERROR_OK && length < sizeof(uint16_t)) {
                status = ATT_ERROR_UNLIKELY;
                goto done;
        }

        ccc_val = (gatt_ccc_t) get_u16(value);
done:
        cps_client->cb->get_event_state_completed(&cps_client->client, status,
                                                        CPS_CLIENT_EVENT_CP_VECTOR_NOTIF,
                                                        ccc_val & GATT_CCC_NOTIFICATIONS);
}

static void handle_ctrl_point_ccc_value(cps_client_t *cps_client, att_error_t status,
                                                        uint16_t length, const uint8_t *value)
{
        gatt_ccc_t ccc_val = GATT_CCC_NONE;

        if (!cps_client->cb->get_cycling_power_cp_state_completed_cb_t) {
                return;
        }

        if (status == ATT_ERROR_OK && length < sizeof(uint16_t)) {
                status = ATT_ERROR_UNLIKELY;
                goto done;
        }

        ccc_val = (gatt_ccc_t) get_u16(value);
done:
        cps_client->cb->get_cycling_power_cp_state_completed_cb_t(&cps_client->client, status,
                                                                ccc_val & GATT_CCC_INDICATIONS);
}

static void handle_read_completed_evt(ble_client_t *client,
                                                        const ble_evt_gattc_read_completed_t *evt)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        uint16_t handle = evt->handle;

        if (handle == cps_client->feature_h) {
                handle_cp_feature_value(cps_client, evt->status, evt->length, evt->value);
        } else if (handle == cps_client->sensor_location_h) {
                handle_sensor_location_value(cps_client, evt->status, evt->length, evt->value);
        } else if (handle == cps_client->measurement_ccc_h) {
                handle_cp_measurement_ccc_value(cps_client, evt->status, evt->length, evt->value);
        } else if (handle == cps_client->measurement_scc_h) {
                handle_cp_measurement_scc_value(cps_client, evt->status, evt->length, evt->value);
        } else if (handle == cps_client->vector_ccc_h) {
                handle_cp_vector_ccc_value(cps_client, evt->status, evt->length, evt->value);
        } else if (handle == cps_client->ctrl_point_ccc_h) {
                handle_ctrl_point_ccc_value(cps_client, evt->status, evt->length, evt->value);
        }
}

static void handle_ctrl_point_result(cps_client_t *cps_client, uint8_t opcode,
                                                cps_client_status_t status, uint16_t length,
                                                const uint8_t *data)
{

        cps_client->pending_opcode = 0x00;

        switch (opcode) {
        case CPS_CLIENT_CP_OPCODE_SET_CUMULATIVE_VALUE:
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->set_cumulative_value_completed,
                                                                                        status);
                break;
        case CPS_CLIENT_CP_OPCODE_UPDATE_SENSOR_LOCATION:
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->update_sensor_location_completed,
                                                                                        status);
                break;
        case CPS_CLIENT_CP_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS:
                CPS_CLIENT_CB(&cps_client->client,
                                        cps_client->cb->request_supported_sensor_locations_completed,
                                        status, length, data);
                break;
        case CPS_CLIENT_CP_OPCODE_SET_CRANK_LENGTH:
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->set_crank_length_completed,
                                                                                        status);
                break;
        case CPS_CLIENT_CP_OPCODE_REQUEST_CRANK_LENGTH:
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->set_crank_length_completed,
                                                                                        status);
                break;
        case CPS_CLIENT_CP_OPCODE_SET_CHAIN_LENGTH:
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->set_chain_length_completed,
                                                                                        status);
                break;
        case CPS_CLIENT_CP_OPCODE_REQUEST_CHAIN_LENGTH:
        {
                uint16_t chain_length = 0;

                if (status == CPS_CLIENT_STATUS_SUCCESS) {
                        if (length != sizeof(chain_length)) {
                                status = CPS_CLIENT_STATUS_OPERATION_FAILED;
                        } else {
                                chain_length = get_u16(data);
                        }
                }

                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->request_chain_length_completed,
                                                                        status, chain_length);
                break;
        }
        case CPS_CLIENT_CP_OPCODE_SET_CHAIN_WEIGHT:
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->set_chain_weight_completed,
                                                                                        status);
                break;
        case CPS_CLIENT_CP_OPCODE_REQUEST_CHAIN_WEIGHT:
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->set_chain_weight_completed,
                                                                                        status);
                break;
        case CPS_CLIENT_CP_OPCODE_SET_SPAN_LENGTH:
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->set_span_length_completed,
                                                                                        status);
                break;
        case CPS_CLIENT_CP_OPCODE_REQUEST_SPAN_LENGTH:
        {
                uint16_t span_length = 0;

                if (status == CPS_CLIENT_STATUS_SUCCESS) {
                        if (length != sizeof(span_length)) {
                                status = CPS_CLIENT_STATUS_OPERATION_FAILED;
                        } else {
                                span_length = get_u16(data);
                        }
                }

                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->request_span_length_completed,
                                                                        status, span_length);
                break;
        }
        case CPS_CLIENT_CP_OPCODE_START_OFFSET_COMPENSATION:
        {
                int16_t offset = 0;

                if (status == CPS_CLIENT_STATUS_SUCCESS) {
                        if (length != sizeof(offset)) {
                                status = CPS_CLIENT_STATUS_OPERATION_FAILED;
                        } else {
                                offset = (int16_t) get_u16(data);
                        }
                }

                CPS_CLIENT_CB(&cps_client->client,
                                cps_client->cb->start_offset_compensation_completed, status,
                                offset);
                break;
        }
        case CPS_CLIENT_CP_OPCODE_MASK_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT:
                CPS_CLIENT_CB(&cps_client->client,
                                        cps_client->cb->set_mask_cp_measurement_completed, status);
                break;
        case CPS_CLIENT_CP_OPCODE_REQUEST_SAMPLING_RATE:
        {
                uint8_t sampling_rate = 0;

                if (status == CPS_CLIENT_STATUS_SUCCESS) {
                        if (length != sizeof(sampling_rate)) {
                                status = CPS_CLIENT_STATUS_OPERATION_FAILED;
                        } else {
                                sampling_rate = get_u8(data);
                        }
                }

                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->request_sampling_rate_completed,
                                                                        status, sampling_rate);
                break;
        }
        case CPS_CLIENT_CP_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE:
        {
                date_time_t date_time;
                const uint8_t *ptr = data;

                if (status == CPS_CLIENT_STATUS_SUCCESS) {
                        if (length != 7) {
                                status = CPS_CLIENT_STATUS_OPERATION_FAILED;
                        } else {
                                date_time.year = get_u16_inc(&ptr);
                                date_time.month = get_u8_inc(&ptr);
                                date_time.day = get_u8_inc(&ptr);
                                date_time.hours = get_u8_inc(&ptr);
                                date_time.minutes = get_u8_inc(&ptr);
                                date_time.seconds = get_u8_inc(&ptr);
                        }
                }

                CPS_CLIENT_CB(&cps_client->client,
                                cps_client->cb->request_calibration_date_completed_cb,
                                status, status == CPS_CLIENT_STATUS_SUCCESS ? &date_time : NULL);
                break;
        }
        default:
                break;
        }
}

static void handle_ctrl_point_write(cps_client_t *cps_client, att_error_t status)
{
        cps_client_status_t cps_status = (cps_client_status_t) status;

        if (cps_client->pending_opcode == CPS_CLIENT_CP_OPCODE_NO_REQUEST) {
                return;
        }

        if (status == ATT_ERROR_OK) {
                // Wait for indication for 30s
                CPS_CLIENT_CB(&cps_client->client, cps_client->cb->start_timer,
                                                                        PROCEDURE_TIMEOUT_MS);
                return;
        }

        switch (cps_status) {
        case CPS_CLIENT_STATUS_PROC_ALREADY_IN_PROGRESS:
        case CPS_CLIENT_STATUS_CCC_IMPROPERLY_CONFIGURED:
                break;
        default:
                cps_status = CPS_CLIENT_STATUS_OPERATION_FAILED;
                break;
        }

        handle_ctrl_point_result(cps_client, cps_client->pending_opcode, cps_status, 0, NULL);
}

static void handle_write_completed_evt(ble_client_t *client,
                                                        const ble_evt_gattc_write_completed_t *evt)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        uint16_t handle = evt->handle;

        if (handle == cps_client->measurement_ccc_h) {
                CPS_CLIENT_CB(client, cps_client->cb->set_event_state_completed,
                                                evt->status, CPS_CLIENT_EVENT_MEASUREMENT_NOTIF);
        } else if (handle == cps_client->measurement_scc_h) {
                CPS_CLIENT_CB(client, cps_client->cb->set_event_state_completed,
                                        evt->status, CPS_CLIENT_EVENT_MEASUREMENT_BROADCAST);
        } else if (handle == cps_client->vector_ccc_h) {
                CPS_CLIENT_CB(client, cps_client->cb->set_event_state_completed,
                                                evt->status, CPS_CLIENT_EVENT_CP_VECTOR_NOTIF);
        } else if (handle == cps_client->ctrl_point_ccc_h) {
                CPS_CLIENT_CB(client, cps_client->cb->set_cycling_power_cp_state_completed,
                                                                                evt->status);
        } else if (handle == cps_client->ctrl_point_h) {
                handle_ctrl_point_write(cps_client, evt->status);
        }
}

static void handle_ctrl_point_indication(cps_client_t *cps_client, uint16_t length,
                                                                        const uint8_t *data)
{
        uint8_t opcode;
        cps_client_status_t status;
        const uint8_t *ptr = NULL;
        uint16_t data_length = 0;

        if (cps_client->pending_opcode == CPS_CLIENT_CP_OPCODE_NO_REQUEST) {
                return;
        }

        if (cps_client->cb->cancel_timer) {
                cps_client->cb->cancel_timer(&cps_client->client);
        }

        /*
         * Indication should contain at least:
         * 1 byte response opcode
         * 1 byte operation opcode
         * 1 byte status field
         */
        if (length < 3) {
                status = CPS_CLIENT_STATUS_INVALID_PARAM;
                goto done;
        }

        if (data[0] != CPS_CLIENT_CP_OPCODE_RESPONSE_CODE) {
                status = CPS_CLIENT_STATUS_OPERATION_FAILED;
                goto done;
        }

        opcode = data[1];
        status = data[2];

        if (opcode != cps_client->pending_opcode) {
                status = CPS_CLIENT_STATUS_INVALID_PARAM;
                goto done;
        }

        data_length = length - 3;
        if (data_length > 0) {
                ptr = data + 3;
        }

done:
        handle_ctrl_point_result(cps_client, cps_client->pending_opcode, status, data_length, ptr);
}

static void handle_indication_evt(ble_client_t *client, const ble_evt_gattc_indication_t *evt)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        uint16_t handle = evt->handle;

        if (handle == cps_client->ctrl_point_h) {
                handle_ctrl_point_indication(cps_client, evt->length, evt->value);
        }
}

static void handle_measurement_notification(cps_client_t *cps_client, uint16_t length,
                                                                        const uint8_t *data)
{
        uint16_t flags;
        cps_client_measurement_t measurement;
        const uint8_t *ptr = data;

        /*
         * Measurement notification must contain at least:
         * Flags (2 byte), Instantaneous Power (2 bytes)
         */
        if (length < 4) {
                return;
        }

        /* Callback not set */
        if (!cps_client->cb->measurement_notif) {
                return;
        }

        memset(&measurement, 0, sizeof(measurement));

        flags = get_u16_inc(&ptr);
        measurement.instant_power = get_u16_inc(&ptr);
        length -= 4;

        if (flags & CPS_CLIENT_PM_FLAGS_PEDAL_POWER_BALANCE_PRESENT) {

                if (length < sizeof(uint8_t)) {
                        goto measure_frame_incomplete;
                }

                measurement.pedal_power_balance_reference =
                              (flags & CPS_CLIENT_PM_FLAGS_PEDAL_POWER_BALANCE_REFERENCE) ?
                                            CPS_CLIENT_PEDAL_POWER_BALANCE_REFERENCE_LEFT :
                                          CPS_CLIENT_PEDAL_POWER_BALANCE_REFERENCE_UNKNOWN;
                measurement.pedal_power_balance_present = true;
                measurement.pedal_power_balance = get_u8_inc(&ptr);
                length -= sizeof(uint8_t);
        }

        if (flags & CPS_CLIENT_PM_FLAGS_ACCUMULATED_TORQUE_PRESENT) {
                if(length < sizeof(uint16_t)) {
                        goto measure_frame_incomplete;
                }

                measurement.accumulated_torque_source =
                                  (flags & CPS_CLIENT_PM_FLAGS_ACCUMULATED_TORQUE_SOURCE) ?
                                         CPS_CLIENT_ACCUMULATED_TORQUE_SOURCE_CRANK_BASED :
                                          CPS_CLIENT_ACCUMULATED_TORQUE_SOURCE_WHEEL_BASED;
                measurement.accumulated_torque_present = true;
                measurement.accumulated_torque = get_u16_inc(&ptr);
                length -= sizeof(uint16_t);
        }

        if (flags & CPS_CLIENT_PM_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT) {
                if (length < (sizeof(uint32_t) + sizeof(uint16_t))) {
                        goto measure_frame_incomplete;
                }

                measurement.wheel_revolution_data_present = true;
                measurement.wrd_cumulative_revol = get_u32_inc(&ptr);
                measurement.wrd_last_wheel_evt_time = get_u16_inc(&ptr);
                length -= (sizeof(uint32_t) + sizeof(uint16_t));
        }

        if (flags & CPS_CLIENT_PM_FLAGS_CRANK_REVOLUTION_DATA_PRESENT) {
                if (length < sizeof(uint32_t)) {
                        goto measure_frame_incomplete;
                }

                measurement.crank_revolution_data_present = true;
                measurement.crd_cumulative_revol = get_u16_inc(&ptr);
                measurement.crd_last_crank_evt_time = get_u16_inc(&ptr);
                length -= sizeof(uint32_t);
        }


        if (flags & CPS_CLIENT_PM_FLAGS_EXTREME_FORCE_MAGNITUDES_PRESENT) {
                if (length < sizeof(uint32_t)) {
                        goto measure_frame_incomplete;
                }

                measurement.extreme_force_magnitude_present = true;
                measurement.efm_max_force_magnitude = get_u16_inc(&ptr);
                measurement.efm_min_force_magnitude = get_u16_inc(&ptr);

                length -= sizeof(uint32_t);
        }

        if (flags & CPS_CLIENT_PM_FLAGS_EXTREME_TORQUE_MAGNITUDES_PRESENT) {
                if (length < sizeof(uint32_t)) {
                        goto measure_frame_incomplete;
                }

                measurement.extreme_torque_magnitude_present = true;
                measurement.etm_max_torq_magnitude = get_u16_inc(&ptr);
                measurement.etm_min_torq_magnitude = get_u16_inc(&ptr);

                length -= sizeof(uint32_t);
        }

        if (flags & CPS_CLIENT_PM_FLAGS_EXTREME_ANGLES_PRESENT) {

                /* Extreme angles is 24 bit (3 octets) long */
                if (length < 3) {
                        goto measure_frame_incomplete;
                }

                measurement.extreme_angle_present = true;
                measurement.ea_maximum_angle = ptr[0] | (ptr[1] & 0x0F) << 8;
                measurement.ea_minimum_angle = (ptr[1] & 0xF0) >> 4 | ptr[2] << 4;
                length -= 3;
        }

        if (flags & CPS_CLIENT_PM_FLAGS_TOP_DEAD_SPOT_ANGLE_PRESENT) {
                if (length < sizeof(uint16_t)) {
                        goto measure_frame_incomplete;
                }

                measurement.top_dead_spot_angle_present = true;
                measurement.top_dead_spot_angle = get_u16_inc(&ptr);

                length -= sizeof(uint16_t);
        }

        if (flags & CPS_CLIENT_PM_FLAGS_BOTTOM_DEAD_SPOT_ANGLE_PRESENT) {
                if (length < sizeof(uint16_t)) {
                        goto measure_frame_incomplete;
                }

                measurement.bottom_dead_spot_angle_present = true;
                measurement.bottom_dead_spot_angle = get_u16_inc(&ptr);
                length -= sizeof(uint16_t);
        }

        if (flags & CPS_CLIENT_PM_FLAGS_ACCUMULATED_ENERGY_PRESENT) {
                if (length < sizeof(uint16_t)) {
                        goto measure_frame_incomplete;
                }

                measurement.accumulated_energy_present = true;
                measurement.accumulated_energy = get_u16_inc(&ptr);
                length -= sizeof(uint16_t);
        }

        if (flags & CPS_CLIENT_PM_FLAGS_OFFSET_COMPENSATION_INDICATOR) {
                measurement.offset_compensation_indicator  = true;
        }

measure_frame_incomplete:
        cps_client->cb->measurement_notif(&cps_client->client, &measurement);

}

static void handle_vector_notification(cps_client_t *cps_client, uint16_t length,
                                                                        const uint8_t *data)
{
        uint16_t inst_array_length, expected_length = 0;
        cps_client_power_vector_t *vector;
        const uint8_t *ptr = data;
        uint8_t flags;

        /* Vector notification must contain at least flags (1 byte) */
        if (length < 1) {
                return;
        }

        /* Callback not set */
        if (!cps_client->cb->vector_notif) {
                return;
        }

        flags = get_u8_inc(&ptr);
        length -= 1;

        if (flags & CPS_CLIENT_VECTOR_FLAG_CRANK_REVOLUTION_DATA_PRESENT_MASK) {
                /* Cumulative crank rev and last crank event time */
                expected_length += sizeof(uint16_t) * 2;
        }

        if (flags & CPS_CLIENT_VECTOR_FLAG_FIRST_CRANK_MEASUREMENT_ANGLE_PRESENT_MASK) {
                /* First Crank Measurement Angle */
                expected_length += sizeof(uint16_t);
        }

        /* Notification is incomplete */
        if (expected_length < length || (expected_length - length) % sizeof(int16_t)) {
                return;
        }

        if (flags & (CPS_CLIENT_VECTOR_FLAG_INSTANT_FORCE_MAGNITUDE_ARRAY_PRESENT_MASK |
                        CPS_CLIENT_VECTOR_FLAG_INSTANT_TORQUE_MAGNITUDE_ARRAY_PRESENT_MASK)) {
                inst_array_length = (expected_length - length) / sizeof(int16_t);
        } else {
                inst_array_length = 0;
        }

        vector = OS_MALLOC(sizeof(*vector) + (inst_array_length * sizeof(int16_t)));
        memset(vector, 0, sizeof(*vector) + (inst_array_length * sizeof(int16_t)));

        vector->crank_revolution_data_present = flags &
                                CPS_CLIENT_VECTOR_FLAG_CRANK_REVOLUTION_DATA_PRESENT_MASK ? 1 : 0;
        vector->first_crank_measurement_angle_present = flags &
                        CPS_CLIENT_VECTOR_FLAG_FIRST_CRANK_MEASUREMENT_ANGLE_PRESENT_MASK ? 1 : 0;
        vector->instantaneous_force_magnitude_array_present = flags &
                        CPS_CLIENT_VECTOR_FLAG_INSTANT_FORCE_MAGNITUDE_ARRAY_PRESENT_MASK ? 1 : 0;
        vector->instantaneous_torque_magnitude_array_present = flags &
                        CPS_CLIENT_VECTOR_FLAG_INSTANT_TORQUE_MAGNITUDE_ARRAY_PRESENT_MASK ? 1 : 0;
        vector->instantaneous_measurement_direction =
                        (flags & CPS_CLIENT_VECTOR_FLAG_INSTANT_MEASUREMENT_DIRECTION_MASK) >> 4;

        if (vector->crank_revolution_data_present) {
                vector->cumulative_crank_revolutions = get_u16_inc(&ptr);
                vector->last_crank_event_time = get_u16_inc(&ptr);
        }

        if (vector->first_crank_measurement_angle_present) {
                vector->first_crank_measurement_angle = get_u16_inc(&ptr);
        }

        if (inst_array_length) {
                vector->instantaneous_magnitude_len = inst_array_length;
                memcpy(vector->instantaneous_magnitude_arr, ptr,
                                                        inst_array_length * sizeof(int16_t));
        }

        cps_client->cb->vector_notif(&cps_client->client, vector);

        OS_FREE(vector);
}

static void handle_notification_evt(ble_client_t *client, const ble_evt_gattc_notification_t *evt)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        uint16_t handle = evt->handle;

        if (handle == cps_client->measurement_h) {
                handle_measurement_notification(cps_client, evt->length, evt->value);
        } else if (handle == cps_client->vector_h) {
                handle_vector_notification(cps_client, evt->length, evt->value);
        }
}

static void handle_disconnected_evt(ble_client_t *client, const ble_evt_gap_disconnected_t *evt)
{
        client->conn_idx = BLE_CONN_IDX_INVALID;

        ble_client_remove(client);
}

static cps_client_t *init(uint16_t conn_idx, const cps_client_callbacks_t *cb)
{
        cps_client_t *cps_client;

        cps_client = OS_MALLOC(sizeof(*cps_client));
        memset(cps_client, 0, sizeof(*cps_client));

        cps_client->cb = cb;
        cps_client->client.cleanup = cleanup;
        cps_client->client.serialize = serialize;
        cps_client->client.read_completed_evt = handle_read_completed_evt;
        cps_client->client.write_completed_evt = handle_write_completed_evt;
        cps_client->client.indication_evt = handle_indication_evt;
        cps_client->client.notification_evt = handle_notification_evt;
        cps_client->client.disconnected_evt = handle_disconnected_evt;
        cps_client->client.conn_idx = conn_idx;

        return cps_client;
}

ble_client_t *cps_client_init(const cps_client_callbacks_t *cb,
                                                             const ble_evt_gattc_browse_svc_t *evt)
{
        cps_client_t *cps_client;
        const gattc_item_t *item;
        att_uuid_t uuid;

        if (!cb) {
                return NULL;
        }

        ble_uuid_create16(UUID_SERVICE_CPS, &uuid);
        if (!ble_uuid_equal(&uuid, &evt->uuid)) {
                return NULL;
        }

        cps_client = init(evt->conn_idx, cb);
        cps_client->client.start_h = evt->start_h;
        cps_client->client.end_h = evt->end_h;

        /* Initialize browse event structure */
        ble_gattc_util_find_init(evt);

        /* Find CP Services characteristic and descriptor */
        /* CP Measurement */
        ble_uuid_create16(UUID_CYCLING_POWER_MEASUREMENT, &uuid);
        item = ble_gattc_util_find_characteristic(&uuid);

        if (!item || !(item->c.properties & GATT_PROP_NOTIFY)) {
                /* Characteristic doesn't exist or doesn't have mandatory property */
                goto failed;
        }

        cps_client->measurement_h = item->c.value_handle;

        /* Check if CP Measurement characteristic supports optional broadcast */
        if (item->c.properties & GATT_PROP_BROADCAST) {
                ble_uuid_create16(UUID_GATT_SERVER_CHAR_CONFIGURATION, &uuid);
                item = ble_gattc_util_find_descriptor(&uuid);
                if (!item) {
                        goto failed;
                }

                cps_client->measurement_scc_h = item->handle;
        }

        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
        item = ble_gattc_util_find_descriptor(&uuid);
        if (!item) {
                goto failed;
        }

        cps_client->measurement_ccc_h = item->handle;

        /* CP feature */
        ble_uuid_create16(UUID_CYCLING_POWER_FEATURE, &uuid);
        item = ble_gattc_util_find_characteristic(&uuid);

        if (!item || !(item->c.properties & GATT_PROP_READ)) {
                /* Characteristic doesn't exist or doesn't have mandatory property */
                goto failed;
        }

        cps_client->feature_h = item->c.value_handle;

        /* CP Sensor Location */
        ble_uuid_create16(UUID_CYCLING_POWER_SENSOR_LOCATION, &uuid);
        item = ble_gattc_util_find_characteristic(&uuid);

        if (!item || !(item->c.properties & GATT_PROP_READ)) {
                /* Characteristic doesn't exist or doesn't have mandatory property */
                goto failed;
        }

        cps_client->sensor_location_h = item->c.value_handle;

        /* CP Vector */
        ble_uuid_create16(UUID_CYCLING_POWER_VECTOR, &uuid);
        item = ble_gattc_util_find_characteristic(&uuid);

        /* Vector is optional characteristic */
        if (item) {
                /* This property is mandatory for the characteristic */
                if (!(item->c.properties & GATT_PROP_NOTIFY)) {
                        goto failed;
                }

                cps_client->vector_h = item->c.value_handle;

                ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
                item = ble_gattc_util_find_descriptor(&uuid);

                /*
                 * If Vector characteristic don't have CCC descriptor
                 * then make it unsupported
                 */
                if (!item) {
                        goto failed;
                }

                cps_client->vector_ccc_h = item->handle;
        }

        /* CP Control Point */
        ble_uuid_create16(UUID_CYCLING_POWER_CONTROL_POINT, &uuid);
        item = ble_gattc_util_find_characteristic(&uuid);

        /* Control Point characteristic is optional */
        if (item) {
                if (!(item->c.properties & GATT_PROP_WRITE) || !(item->c.properties &
                                                                        GATT_PROP_INDICATE)) {
                        goto failed;
                }

                cps_client->ctrl_point_h = item->c.value_handle;

                ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
                item = ble_gattc_util_find_descriptor(&uuid);

                /*
                 * If Control Point characteristic don't have CCC descriptor
                 * then make it unsupported
                 */
                if (!item) {
                        goto failed;
                }

                cps_client->ctrl_point_ccc_h = item->handle;
        }

        /* All necessary check are made above */
        return &cps_client->client;

failed:
        cleanup(&cps_client->client);
        return NULL;
}

ble_client_t *cps_client_init_from_data(uint16_t conn_idx, const cps_client_callbacks_t *cb,
                                                                   const void *data, size_t length)
{
        cps_client_t *cps_client;
        const cps_client_serialized_t *d = data;

        if (!data || (length < sizeof(cps_client_serialized_t))) {
                return NULL;
        }

        cps_client = init(conn_idx, cb);

        cps_client->measurement_h = d->measurement_h;
        cps_client->measurement_ccc_h = d->measurement_ccc_h;
        cps_client->measurement_scc_h = d->measurement_scc_h;
        cps_client->feature_h = d->feature_h;
        cps_client->sensor_location_h = d->sensor_location_h;
        cps_client->vector_h = d->vector_h;
        cps_client->vector_ccc_h = d->vector_ccc_h;
        cps_client->ctrl_point_h = d->ctrl_point_h;
        cps_client->ctrl_point_ccc_h = d->ctrl_point_ccc_h;
        cps_client->client.start_h = d->start_h;
        cps_client->client.end_h = d->end_h;

        ble_client_attach(&cps_client->client, conn_idx);

        return &cps_client->client;
}

bool cps_client_read_cycling_power_feature(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        ble_error_t status;

        /* Feature handle must be non-zero - it is mandatory characteristic */
        status = ble_gattc_read(cps_client->client.conn_idx, cps_client->feature_h, 0);

        return status == BLE_STATUS_OK;
}

bool cps_client_read_sensor_location(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        ble_error_t status;

        /* Sensor location handle must be non-zero - it is mandatory characteristic */
        status = ble_gattc_read(cps_client->client.conn_idx, cps_client->sensor_location_h, 0);

        return status == BLE_STATUS_OK;
}

cps_client_cap_t cps_client_get_capabilities(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        cps_client_cap_t cap = 0;

        if (cps_client->vector_h) {
                cap |= CPS_CLIENT_CAP_POWER_VECTOR;
        }

        if (cps_client->ctrl_point_h) {
                cap |= CPS_CLIENT_CAP_CYCLING_POWER_CONTROL_POINT;
        }

        if (cps_client->measurement_scc_h) {
                cap |= CPS_CLIENT_CAP_CYCLING_POWER_MEASUREMENT_BROADCAST;
        }

        return cap;
}

bool cps_client_set_event_state(ble_client_t *client, cps_client_event_t event, bool enable)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        uint16_t value, handle = 0x0000;
        ble_error_t status;

        switch (event) {
        case CPS_CLIENT_EVENT_CP_VECTOR_NOTIF:
                value = enable ? GATT_CCC_NOTIFICATIONS : GATT_CCC_NONE;
                handle = cps_client->vector_ccc_h;
                break;
        case CPS_CLIENT_EVENT_MEASUREMENT_NOTIF:
                value = enable ? GATT_CCC_NOTIFICATIONS : GATT_CCC_NONE;
                handle = cps_client->measurement_ccc_h;
                break;
        case CPS_CLIENT_EVENT_MEASUREMENT_BROADCAST:
                value = enable ? GATT_SCC_BROADCAST : GATT_SCC_NONE;
                handle = cps_client->measurement_scc_h;
                break;
        default:
                return false;
        }

        if (!handle) {
                return false;
        }

        status = ble_gattc_write(client->conn_idx, handle, 0, sizeof(value), (uint8_t *) &value);

        return (status == BLE_STATUS_OK);
}

bool cps_client_get_event_state(ble_client_t *client, cps_client_event_t event)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        uint16_t handle = 0x0000;
        ble_error_t status;

        switch (event) {
        case CPS_CLIENT_EVENT_CP_VECTOR_NOTIF:
                handle = cps_client->vector_ccc_h;
                break;
        case CPS_CLIENT_EVENT_MEASUREMENT_NOTIF:
                handle = cps_client->measurement_ccc_h;
                break;
        case CPS_CLIENT_EVENT_MEASUREMENT_BROADCAST:
                handle = cps_client->measurement_scc_h;
                break;
        default:
                return false;
        }

        if (!handle) {
                return false;
        }

        status = ble_gattc_read(client->conn_idx, handle, 0);

        return (status == BLE_STATUS_OK);
}

bool cps_client_set_cycling_power_cp_state(ble_client_t *client, bool enable)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        ble_error_t status;
        uint16_t value;

        if (!cps_client->ctrl_point_ccc_h) {
                return false;
        }

        value = enable ? GATT_CCC_INDICATIONS : GATT_CCC_NONE;

        status = ble_gattc_write(client->conn_idx, cps_client->ctrl_point_ccc_h, 0, sizeof(value),
                                                                        (uint8_t *) &value);

        return (status == BLE_STATUS_OK);
}

bool cps_client_get_cycling_power_cp_state(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        ble_error_t status;

        if (!cps_client->ctrl_point_ccc_h) {
                return false;
        }

        status = ble_gattc_read(client->conn_idx, cps_client->ctrl_point_ccc_h, 0);

        return (status == BLE_STATUS_OK);
}

static bool ctrl_point_write(cps_client_t *cps_client, cps_client_cp_opcode_t opcode,
                                                        uint16_t length, const uint8_t *payload)
{
        uint8_t data[length + 1];
        uint8_t *ptr = data;
        ble_error_t status;

        if (!cps_client->ctrl_point_h) {
                return false;
        }

        if (cps_client->pending_opcode != CPS_CLIENT_CP_OPCODE_NO_REQUEST) {
                return false;
        }

        put_u8_inc(&ptr, opcode);
        if (length > 0) {
                put_data_inc(&ptr, length, payload);
        }

        status = ble_gattc_write(cps_client->client.conn_idx, cps_client->ctrl_point_h, 0,
                                                                                ptr - data, data);

        if (status != BLE_STATUS_OK) {
                return false;
        }

        cps_client->pending_opcode = opcode;

        return true;
}

bool cps_client_set_cumulative_value(ble_client_t *client, uint32_t value)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_SET_CUMULATIVE_VALUE,
                                                                sizeof(value), (uint8_t *) &value);
}

bool cps_client_update_sensor_location(ble_client_t *client, cps_client_sensor_location_t location)
{
        cps_client_t *cps_client = (cps_client_t *) client;
        uint8_t loc = location;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_UPDATE_SENSOR_LOCATION,
                                                                                sizeof(loc), &loc);
}

bool cps_client_request_supported_sensor_locations(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client,
                                CPS_CLIENT_CP_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS, 0, NULL);
}

bool cps_client_set_crank_length(ble_client_t *client, uint16_t value)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_SET_CRANK_LENGTH, sizeof(value),
                                                                        (uint8_t *) &value);
}

bool cps_client_request_crank_length(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_REQUEST_CRANK_LENGTH, 0, NULL);
}

bool cps_client_set_chain_length(ble_client_t *client, uint16_t value)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_SET_CHAIN_LENGTH, sizeof(value),
                                                                        (uint8_t *) &value);
}

bool cps_client_request_chain_length(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_REQUEST_CHAIN_LENGTH, 0, NULL);
}

bool cps_client_request_chain_weight(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_REQUEST_CHAIN_LENGTH, 0, NULL);
}

bool cps_client_set_chain_weight(ble_client_t *client, uint16_t value)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_SET_CHAIN_WEIGHT, sizeof(value),
                                                                        (uint8_t *) &value);
}

bool cps_client_set_span_length(ble_client_t *client, uint16_t value)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_SET_SPAN_LENGTH, sizeof(value),
                                                                        (uint8_t *) &value);
}

bool cps_client_request_span_length(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_REQUEST_SPAN_LENGTH, 0, NULL);
}

bool cps_client_start_offset_compensation(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_START_OFFSET_COMPENSATION, 0,
                                                                                        NULL);
}

bool cps_client_mask_cycling_power_measurement_characteristic_content(ble_client_t *client,
                                                                                uint16_t value)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client,
                        CPS_CLIENT_CP_OPCODE_MASK_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT,
                        sizeof(value), (uint8_t *) &value);
}

bool cps_client_request_sampling_rate(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_REQUEST_SAMPLING_RATE, 0, NULL);
}

bool cps_client_request_factory_calibration_date(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        return ctrl_point_write(cps_client, CPS_CLIENT_CP_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE,
                                                                                        0, NULL);
}

void cps_client_timer_notif(ble_client_t *client)
{
        cps_client_t *cps_client = (cps_client_t *) client;

        if (cps_client->pending_opcode == CPS_CLIENT_CP_OPCODE_NO_REQUEST) {
                return;
        }

        handle_ctrl_point_result(cps_client, cps_client->pending_opcode, CPS_CLIENT_STATUS_TIMEOUT,
                                                                                        0, NULL);
}
