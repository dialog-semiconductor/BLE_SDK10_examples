/**
 ****************************************************************************************
 *
 * @file cps.c
 *
 * @brief Cycling Power Service sample implementation
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
#if defined(CONFIG_USE_BLE_SERVICES)
#include <stdbool.h>
#include <stddef.h>
#include "osal.h"
#include "ble_att.h"
#include "ble_bufops.h"
#include "ble_common.h"
#include "ble_gatts.h"
#include "ble_uuid.h"
#include "ble_storage.h"
#include "cps.h"

#define UUID_CYCLING_POWER_MEASUREMENT     (0x2A63)
#define UUID_CYCLING_POWER_FEATURE         (0x2A65)
#define UUID_CYCLING_POWER_SENSOR_LOCATION (0x2A5D)
#define UUID_CYCLING_POWER_VECTOR          (0x2A64)
#define UUID_CYCLING_POWER_CONTROL_POINT   (0x2A66)

#define CP_VECTOR_CCC_PENDING  (0x10000)
#define CCC_VAL_SIZE           (2)
#define SCC_VAL_SIZE           (2)
#define CPS_FEATURE_SIZE       (4)
#define CPS_CONTROL_POINT_SIZE (5)
#define CPS_POWER_VECTOR_SIZE  (19)
#define SENSOR_LOCATION_SIZE   (1)
#define POWER_MEASUREMENT_SIZE (34)

#define CPS_ALL_SUPPORTED_FEATURES_MASK (CPS_FEATURE_PEDAL_POWER_BALANCE_SUPPORT    | \
                                CPS_FEATURE_ACCUMULATED_TORQUE_SUPPORRT             | \
                                CPS_FEATURE_WHEEL_REVOLUTION_DATA_SUPPORT           | \
                                CPS_FEATURE_CRANK_REVOLUTION_DATA_SUPPORT           | \
                                CPS_FEATURE_EXTREME_MAGNITUDE_SUPPORT               | \
                                CPS_FEATURE_EXTREME_ANGLE_SUPPORT                   | \
                                CPS_FEATURE_TOP_AND_BOTTOM_DEAD_SPOT_ANGLES_SUPPORT | \
                                CPS_FEATURE_ACCUMULATED_ENERGY_SUPPORT              | \
                                CPS_FEATURE_OFFSET_COMPENSATION_INDICATOR_SUPPORT   | \
                                CPS_FEATURE_OFFSET_COMPENSATION_SUPPORT             | \
                                CPS_FEATURE_CHARACTERISTIC_CONTENT_MASKING_SUPPORT  | \
                                CPS_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORT       | \
                                CPS_FEATURE_CRANK_LENGTH_ADJUSTMENT_SUPPORT         | \
                                CPS_FEATURE_CHAIN_WEIGHT_ADJUSTMENT_SUPPORT         | \
                                CPS_FEATURE_CHAIN_LENGTH_ADJUSTMENT_SUPPORT         | \
                                CPS_FEATURE_SPAN_LENGTH_ADJUSTMENT_SUPPORT          | \
                                CPS_FEATURE_SENSOR_MEASUREMENT_CONTEXT_SUPPORT      | \
                                CPS_FEATURE_INSTANTANEOUS_MEASUREMENT_DIR_SUPPORT   | \
                                CPS_FEATURE_FACTORY_CALIBRATION_DATE_SUPPORT)

#define CPS_VECTOR_SUPPORTED_DIRECTIONS_MASK \
                               (CPS_VECTOR_INSTANT_MEASUREMENT_DIRECTION_TANGENTIAL | \
                                CPS_VECTOR_INSTANT_MEASUREMENT_DIRECTION_RADIAL     | \
                                CPS_VECTOR_INSTANT_MEASUREMENT_DIRECTION_LATERAL)

/**
 * Cycling Power Control Point Operation Opcodes
 */
typedef enum {
        CPS_CP_OPCODE_SET_CUMULATIVE_VALUE                  = 0x01,
        CPS_CP_OPCODE_UPDATE_SENSOR_LOCATION                = 0x02,
        CPS_CP_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS    = 0x03,
        CPS_CP_OPCODE_SET_CRANK_LENGTH                      = 0x04,
        CPS_CP_OPCODE_REQUEST_CRANK_LENGTH                  = 0x05,
        CPS_CP_OPCODE_SET_CHAIN_LENGTH                      = 0x06,
        CPS_CP_OPCODE_REQUEST_CHAIN_LENGTH                  = 0x07,
        CPS_CP_OPCODE_SET_CHAIN_WEIGHT                      = 0x08,
        CPS_CP_OPCODE_REQUEST_CHAIN_WEIGHT                  = 0x09,
        CPS_CP_OPCODE_SET_SPAN_LENGTH                       = 0x0a,
        CPS_CP_OPCODE_REQUEST_SPAN_LENGTH                   = 0x0b,
        CPS_CP_OPCODE_START_OFFSET_COMPENSATION             = 0x0c,
        CPS_CP_OPCODE_MASK_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT     = 0x0d,
        CPS_CP_OPCODE_REQUEST_SAMPLING_RATE                 = 0x0e,
        CPS_CP_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE      = 0x0f,
        CPS_CP_OPCODE_RESPONSE_CODE                         = 0x20,
} cps_cp_opcode_t;

/**
 * Cycling Power Measurement characteristic flags
 */
typedef enum {
        /* Pedal Power Balance Present */
        CPS_PM_FLAGS_PEDAL_POWER_BALANCE_PRESENT         = 0x0001,
        /* Pedal Power Balance Reference */
        CPS_PM_FLAGS_PEDAL_POWER_BALANCE_REFERENCE       = 0x0002,
        /* Accumulated Torque Present */
        CPS_PM_FLAGS_ACCUMULATED_TORQUE_PRESENT          = 0x0004,
        /* Accumulated Torque Source */
        CPS_PM_FLAGS_ACCUMULATED_TORQUE_SOURCE           = 0x0008,
        /* Wheel Revolution Data Present */
        CPS_PM_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT       = 0x0010,
        /* Crank Revolution Data Present */
        CPS_PM_FLAGS_CRANK_REVOLUTION_DATA_PRESENT       = 0x0020,
        /* Extreme Force Magnitudes Present */
        CPS_PM_FLAGS_EXTREME_FORCE_MAGNITUDES_PRESENT    = 0x0040,
        /* Extreme Torque Magnitudes Present */
        CPS_PM_FLAGS_EXTREME_TORQUE_MAGNITUDES_PRESENT   = 0x0080,
        /* Extreme Angles Present */
        CPS_PM_FLAGS_EXTREME_ANGLES_PRESENT              = 0x0100,
        /* Top Dead Spot Angle Present */
        CPS_PM_FLAGS_TOP_DEAD_SPOT_ANGLE_PRESENT         = 0x0200,
        /* Bottom Dead Spot Angle Present */
        CPS_PM_FLAGS_BOTTOM_DEAD_SPOT_ANGLE_PRESENT      = 0x0400,
        /* Accumulated Energy Present */
        CPS_PM_FLAGS_ACCUMULATED_ENERGY_PRESENT          = 0x0800,
        /* Offset Compensation Indicator */
        CPS_PM_FLAGS_OFFSET_COMPENSATION_INDICATOR       = 0x1000,
} cps_measurement_flags_t;

/**
 * Cycling Power Vector characteristic flags
 */
typedef enum {
        /* Crank Revolution Data - Cumulative Crank Revolutions */
        CPS_VECTOR_CRANK_REVOLUTION_DATA_PRESENT    = 0x01,
        /* First Crank Measurement Angle */
        CPS_VECTOR_CRANK_MEASUREMENT_ANGLE_PRESENT  = 0x02,
        /* Instantaneous Force Magnitude Array */
        CPS_VECTOR_INSTANT_FORCE_MAGNITUDE_PRESENT  = 0x04,
        /* Instantaneous Torque Magnitude Array */
        CPS_VECTOR_INSTANT_TORQUE_MAGNITUDE_PRESENT = 0x08,
} cps_vector_flags_t;

typedef struct {
        ble_service_t svc;

        const cps_callbacks_t *cb;        /* Callbacks */

        uint16_t cp_measurement_h;        /* Cycling Power Measurement Handle */
        uint16_t cp_measurement_ccc_h;    /* Cycling Power Measurement CCC */

        uint16_t cp_features_h;           /* Cycling Power Feature Handle */

        uint16_t cp_sens_location_h;      /* Sensor Location Handle */

        uint16_t cp_vector_h;             /* Cycling Power Vector Handle */
        uint16_t cp_cpv_ccc_h;            /* Cycling Power Vector CCC */

        uint16_t cp_ctrl_point_h;         /* Cycling Power Control Point Handle */
        uint16_t cp_cpcp_ccc_h;           /* Cycling Power Control Point CCC */

        cps_feature_t supported_features; /* Cycling Power Feature Flags */
        uint8_t *sensor_locations;
        uint8_t  sensor_locations_count;

        /* Cycling Power Control Point request values */
        uint16_t crank_length;
        uint16_t chain_length;
        uint16_t chain_weight;
        uint16_t span_length;
        uint16_t measurement_flag_mask;
        uint8_t  sampling_rate;
        svc_date_time_t factory_calibration_date;

        /* Sensor context for determining force based (0) or torque based (1) measurements */
        bool measurements_context_type;

} cp_service_t;

/*
 * Cycling Power Control Point request value and opcode storage structure
 */
typedef struct {
        uint16_t opcode;
        uint16_t requested_value;
} cps_request_values_t;

static void set_cp_feature_value(cp_service_t *cps, cps_feature_t feature)
{
        uint32_t cp_feature = feature;

        ble_gatts_set_value(cps->cp_features_h, sizeof(cp_feature), &cp_feature);
}

static void set_sensor_location_value(cp_service_t *cps, cps_sensor_location_t location)
{
        uint8_t sensor_location = location;
        ble_gatts_set_value(cps->cp_sens_location_h, sizeof(sensor_location), &sensor_location);
}

static void add_ccc(uint16_t *ccc_h)
{
        att_uuid_t uuid;

        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
        ble_gatts_add_descriptor(&uuid, ATT_PERM_RW, CCC_VAL_SIZE, 0, ccc_h);
}

static uint16_t cps_get_num_att(const ble_service_config_t *config, const cps_config_t *cps_config)
{
        /* CP Measurement, CP Feature, Sensor Location */
        uint16_t num_chars = 3;
        /* CP Measurement CCC descriptor */
        uint16_t num_desc = 1;

        if ((cps_config->supported_features & CPS_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORT) ||
                    (cps_config->supported_features & CPS_FEATURE_WHEEL_REVOLUTION_DATA_SUPPORT)) {
                /* CP Control Point */
                num_chars++;
                /* CP Control Point CCC descriptor */
                num_desc++;
        }

        if (cps_config->power_vector_support) {
                /* CP  Vector */
                num_chars++;
                /* CP Vector CCC descriptor */
                num_desc++;
        }

        return ble_service_get_num_attr(config, num_chars, num_desc);
}

static void handle_ccc_read_req(uint16_t conn_idx, uint16_t handle)
{
        uint16_t ccc = 0x0000;

        ble_storage_get_u16(conn_idx, handle, &ccc);

        ble_gatts_read_cfm(conn_idx, handle, ATT_ERROR_OK, sizeof(ccc), &ccc);
}

static void handle_read_req(ble_service_t *svc, const ble_evt_gatts_read_req_t *evt)
{
        cp_service_t *cps = (cp_service_t *) svc;

        if (evt->handle == cps->cp_measurement_ccc_h || evt->handle == cps->cp_cpv_ccc_h
                                                     || evt->handle == cps->cp_cpcp_ccc_h) {
                handle_ccc_read_req(evt->conn_idx, evt->handle);
        } else {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0,
                                                                                           NULL);
        }
}

static void measurement_notification_changed(cp_service_t *cps, uint16_t conn_idx, bool enabled)
{
        if (cps->cb && cps->cb->measur_notif_changed) {
                cps->cb->measur_notif_changed(&cps->svc, conn_idx, enabled);
        }
}

static void vector_notification_changed(cp_service_t *cps, uint16_t conn_idx, bool enabled)
{
        if ((cps->cp_vector_h != 0) && (cps->cb && cps->cb->power_vector_notif_changed)) {
                cps->cb->power_vector_notif_changed(&cps->svc, conn_idx, enabled);
        }
}

static void handle_disconnect_evt(ble_service_t *svc, const ble_evt_gap_disconnected_t *evt)
{
        cp_service_t *cps = (cp_service_t *) svc;

        measurement_notification_changed(cps, evt->conn_idx, false);
        vector_notification_changed(cps, evt->conn_idx, false);
}

static void handle_event_sent_evt(ble_service_t *svc, const ble_evt_gatts_event_sent_t *evt)
{
        cp_service_t *cps = (cp_service_t *) svc;

        if (evt->handle == cps->cp_ctrl_point_h) {
                ble_storage_remove(evt->conn_idx, cps->cp_ctrl_point_h);
        }
}

static void handle_connect_evt(ble_service_t *svc, const ble_evt_gap_connected_t *evt)
{
        cp_service_t *cps = (cp_service_t *) svc;
        uint16_t ccc = 0x0000;

        ble_storage_get_u16(evt->conn_idx, cps->cp_measurement_ccc_h, &ccc);
        if (ccc & GATT_CCC_NOTIFICATIONS) {
                measurement_notification_changed(cps, evt->conn_idx, true);
        }

        ccc = 0x0000; /* set default value to prevent above storage get to take action */
        ble_storage_get_u16(evt->conn_idx, cps->cp_cpv_ccc_h, &ccc);
        if (ccc & GATT_CCC_NOTIFICATIONS) {
                vector_notification_changed(cps, evt->conn_idx, true);
        }
}

static cps_status_t set_cumulative_value(cp_service_t *cps, uint16_t conn_idx, uint16_t length,
                                                                              const uint8_t *value)
{
        uint32_t cumulative_value;
        cps_request_values_t store_buff = { 0 };

        if (!(cps->supported_features & CPS_FEATURE_WHEEL_REVOLUTION_DATA_SUPPORT)) {
            return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        /* Value must contain opcode and 32-bit unsigned integer */
        if (length < 5) {
                return CPS_STATUS_INVALID_PARAM;
        }

        store_buff.opcode = CPS_CP_OPCODE_SET_CUMULATIVE_VALUE;
        ble_storage_put_buffer_copy(conn_idx, cps->cp_ctrl_point_h, sizeof(store_buff),
                                                                         &store_buff, NULL, false);

        cumulative_value = get_u32(&value[1]);
        cps->cb->set_cumulative_value(&cps->svc, conn_idx, cumulative_value);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t update_sensor_location(cp_service_t *cps, uint16_t conn_idx, uint16_t length,
                                                                              const uint8_t *value)
{
        cps_sensor_location_t location;
        bool location_valid = false;
        uint8_t i;

        cps_request_values_t store_buff = { 0 };

        if (!(cps->supported_features & CPS_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        /* Value must contain sensor location and 8-bit unsigned integer */
        if (length < 2) {
                return CPS_STATUS_INVALID_PARAM;
        }

        if (!cps->cb || !cps->cb->update_sensor_location) {
                return CPS_STATUS_INVALID_PARAM;
        }

        location = get_u8(&value[1]);
        for (i = 0; i < cps->sensor_locations_count; i++) {
                if (cps->sensor_locations[i] == location) {
                        location_valid = true;
                        break;
                }
        }

        if (!location_valid) {
                return CPS_STATUS_INVALID_PARAM;
        }

        store_buff.opcode = CPS_CP_OPCODE_UPDATE_SENSOR_LOCATION;
        store_buff.requested_value = location;
        ble_storage_put_buffer_copy(conn_idx, cps->cp_ctrl_point_h, sizeof(store_buff),
                                                                         &store_buff, NULL, false);

        cps->cb->update_sensor_location(&cps->svc, conn_idx, location);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t request_supported_sensor_location(cp_service_t *cps, uint16_t conn_idx,
                               uint16_t buff_length, uint8_t *buffer, uint16_t *response_param_len)
{
        if (!(cps->supported_features & CPS_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        *response_param_len = cps->sensor_locations_count > buff_length ? buff_length :
                                                                       cps->sensor_locations_count;

        memcpy(buffer, cps->sensor_locations, *response_param_len);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t set_crank_length(cp_service_t *cps, uint16_t conn_idx, uint16_t length,
                                                                              const uint8_t *value)
{
        uint16_t crank_length;
        cps_request_values_t store_buff = { 0 };

        if (!(cps->supported_features & CPS_FEATURE_CRANK_LENGTH_ADJUSTMENT_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        /* Value must contain opcode and 16-bit unsigned integer */
        if (length < 3) {
                return CPS_STATUS_INVALID_PARAM;
        }

        if (!cps->cb || !cps->cb->set_crank_length) {
                return CPS_STATUS_OPERATION_FAILED;
        }

        crank_length = get_u16(&value[1]);

        store_buff.opcode = CPS_CP_OPCODE_SET_CRANK_LENGTH;
        store_buff.requested_value = crank_length;
        ble_storage_put_buffer_copy(conn_idx, cps->cp_ctrl_point_h, sizeof(store_buff),
                                                                         &store_buff, NULL, false);

        cps->cb->set_crank_length(&cps->svc, conn_idx, crank_length);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t request_crank_length(cp_service_t *cps, uint16_t conn_idx, uint8_t *buffer,
                                                                     uint16_t *response_param_len)
{
        uint16_t crank_length = cps->crank_length;

        if (!(cps->supported_features & CPS_FEATURE_CRANK_LENGTH_ADJUSTMENT_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        *response_param_len = sizeof(crank_length);
        memcpy(buffer, &crank_length, *response_param_len);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t set_chain_length(cp_service_t *cps, uint16_t conn_idx, uint16_t length,
                                                                              const uint8_t *value)
{
        uint16_t chain_length;
        cps_request_values_t store_buff = { 0 };

        if (!(cps->supported_features & CPS_FEATURE_CHAIN_LENGTH_ADJUSTMENT_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        /* Value must contain opcode and 16-bit unsigned integer */
        if (length < 3) {
                return CPS_STATUS_INVALID_PARAM;
        }

        if (!cps->cb || !cps->cb->set_chain_length) {
                return CPS_STATUS_OPERATION_FAILED;
        }

        chain_length = get_u16(&value[1]);

        store_buff.opcode = CPS_CP_OPCODE_SET_CHAIN_LENGTH;
        store_buff.requested_value = chain_length;
        ble_storage_put_buffer_copy(conn_idx, cps->cp_ctrl_point_h, sizeof(store_buff),
                                                                         &store_buff, NULL, false);

        cps->cb->set_chain_length(&cps->svc, conn_idx, chain_length);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t request_chain_length(cp_service_t *cps, uint16_t conn_idx, uint8_t *buffer,
                                                                      uint16_t *response_param_len)
{
        uint16_t chain_length = cps->chain_length;

        if (!(cps->supported_features & CPS_FEATURE_CHAIN_LENGTH_ADJUSTMENT_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        *response_param_len = sizeof(chain_length);
        memcpy(buffer, &chain_length, *response_param_len);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t set_chain_weight(cp_service_t *cps, uint16_t conn_idx, uint16_t length,
                                                                              const uint8_t *value)
{
        uint16_t chain_weight;
        cps_request_values_t store_buff = { 0 };

        if (!(cps->supported_features & CPS_FEATURE_CHAIN_WEIGHT_ADJUSTMENT_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        /* Value must contain opcode and 16-bit unsigned integer */
        if (length < 3) {
                return CPS_STATUS_INVALID_PARAM;
        }

        if (!cps->cb || !cps->cb->set_chain_length) {
                return CPS_STATUS_OPERATION_FAILED;
        }

        chain_weight = get_u16(&value[1]);

        store_buff.opcode = CPS_CP_OPCODE_SET_CHAIN_WEIGHT;
        store_buff.requested_value = chain_weight;
        ble_storage_put_buffer_copy(conn_idx, cps->cp_ctrl_point_h, sizeof(store_buff),
                                                                         &store_buff, NULL, false);

        cps->cb->set_chain_weight(&cps->svc, conn_idx, chain_weight);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t request_chain_weight(cp_service_t *cps, uint16_t conn_idx, uint8_t *buffer,
                                                                      uint16_t *response_param_len)
{
        uint16_t chain_weight = cps->chain_weight;

        if (!(cps->supported_features & CPS_FEATURE_CHAIN_WEIGHT_ADJUSTMENT_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        *response_param_len = sizeof(chain_weight);
        memcpy(buffer, &chain_weight, *response_param_len);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t set_span_length(cp_service_t *cps, uint16_t conn_idx, uint16_t length,
                                                                             const uint8_t *value)
{
        uint16_t span_length;
        cps_request_values_t store_buff = { 0 };

        if (!(cps->supported_features & CPS_FEATURE_SPAN_LENGTH_ADJUSTMENT_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        /* Value must contain opcode and 16-bit unsigned integer */
        if (length < 3) {
                return CPS_STATUS_INVALID_PARAM;
        }

        if (!cps->cb || !cps->cb->set_span_length) {
                return CPS_STATUS_OPERATION_FAILED;
        }

        span_length = get_u16(&value[1]);

        store_buff.opcode = CPS_CP_OPCODE_SET_SPAN_LENGTH;
        store_buff.requested_value = span_length;
        ble_storage_put_buffer_copy(conn_idx, cps->cp_ctrl_point_h, sizeof(store_buff),
                                                                         &store_buff, NULL, false);

        cps->cb->set_span_length(&cps->svc, conn_idx, span_length);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t request_span_length(cp_service_t *cps, uint16_t conn_idx, uint8_t *buffer,
                                                                      uint16_t *response_param_len)
{
        uint16_t span_length = cps->span_length;

        if (!(cps->supported_features & CPS_FEATURE_SPAN_LENGTH_ADJUSTMENT_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        *response_param_len = sizeof(span_length);
        memcpy(buffer, &span_length, *response_param_len);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t start_offset_compensation(cp_service_t *cps, uint16_t conn_idx,
                                                             uint16_t length, const uint8_t *value)
{
        cps_request_values_t store_buff = { 0 };

        if (!(cps->supported_features & CPS_FEATURE_OFFSET_COMPENSATION_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        /* Value must contain opcode only */
        if (length != 1) {
                return CPS_STATUS_INVALID_PARAM;
        }

        if (!cps->cb || !cps->cb->start_offset_compensation) {
                return CPS_STATUS_OPERATION_FAILED;
        }

        store_buff.opcode = CPS_CP_OPCODE_START_OFFSET_COMPENSATION;
        ble_storage_put_buffer_copy(conn_idx, cps->cp_ctrl_point_h, sizeof(store_buff),
                                                                         &store_buff, NULL, false);

        cps->cb->start_offset_compensation(&cps->svc, conn_idx);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t mask_cyclic_power_meas_char(cp_service_t *cps, uint16_t conn_idx,
                                                             uint16_t length, const uint8_t *value)
{

        uint16_t mask_power_measurement;
        cps_request_values_t store_buff = { 0 };
        /*
         * Declare max value available for Mask Cycling Power Measurement Characteristic Content
         * PTS set 0xFFFF value for testing unavailable parameters.
         */
        const uint16_t max_mask_value = 0x1FF;

        if (!(cps->supported_features & CPS_FEATURE_CHARACTERISTIC_CONTENT_MASKING_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        /* Value must contain opcode and 16-bit unsigned integer */
        if (length < 3) {
                return CPS_STATUS_INVALID_PARAM;
        }

        if (!cps->cb || !cps->cb->mask_cp_measurement_char_content) {
                return CPS_STATUS_OPERATION_FAILED;
        }

        mask_power_measurement = get_u16(&value[1]);
        if (mask_power_measurement > max_mask_value) {
                return CPS_STATUS_INVALID_PARAM;
        }

        store_buff.opcode = CPS_CP_OPCODE_MASK_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT;
        store_buff.requested_value = mask_power_measurement;

        ble_storage_put_buffer_copy(conn_idx, cps->cp_ctrl_point_h, sizeof(store_buff),
                                                                         &store_buff, NULL, false);

        cps->cb->mask_cp_measurement_char_content(&cps->svc, conn_idx, mask_power_measurement);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t request_sampling_rate(cp_service_t *cps, uint16_t conn_idx, uint8_t *buffer,
                                                                      uint16_t *response_param_len)
{
        uint8_t sampling_rate = cps->sampling_rate;

        if (cps->cp_vector_h == 0) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        if (cps->sampling_rate == 0) {
                return CPS_STATUS_OPERATION_FAILED;
        }

        *response_param_len = sizeof(sampling_rate);
        memcpy(buffer, &sampling_rate, *response_param_len);

        return CPS_STATUS_SUCCESS;
}

static cps_status_t request_factory_calibration_date(cp_service_t *cps, uint16_t conn_idx,
                                                     uint8_t *buffer, uint16_t *response_param_len)
{
        uint8_t *ptr = buffer;

        if (!(cps->supported_features & CPS_FEATURE_FACTORY_CALIBRATION_DATE_SUPPORT)) {
                return CPS_STATUS_OPCODE_NOT_SUPPORTED;
        }

        if ((cps->factory_calibration_date.year == 0) ||
          (cps->factory_calibration_date.month == 0) || (cps->factory_calibration_date.day == 0)) {
                return CPS_STATUS_OPERATION_FAILED;
        }

        put_u16_inc(&ptr, cps->factory_calibration_date.year);
        put_u8_inc(&ptr, cps->factory_calibration_date.month);
        put_u8_inc(&ptr, cps->factory_calibration_date.day);
        put_u8_inc(&ptr, cps->factory_calibration_date.hours);
        put_u8_inc(&ptr, cps->factory_calibration_date.minutes);
        put_u8_inc(&ptr, cps->factory_calibration_date.seconds);

        *response_param_len = ptr - buffer;

        return CPS_STATUS_SUCCESS;
}

static void handle_cp_control_point_write(cp_service_t *cps, const ble_evt_gatts_write_req_t *evt)
{
        att_error_t status = ATT_ERROR_OK;
        uint16_t response_parameter_len = 0;
        uint8_t response[20] = { 0 };
        cps_status_t cp_status = CPS_STATUS_OPCODE_NOT_SUPPORTED;
        uint16_t ccc;
        cps_request_values_t *store_buff = NULL;
        /* This variable is used only to indicate that procedure is already in progress */
        cps_request_values_t busy_store_buff = { 0 };
        /* Flag used for indication that value is requested so response with it will be send */
        bool value_request = false;

        if (evt->length == 0) {
                status = ATT_ERROR_INVALID_VALUE_LENGTH;
                goto write_rsp;
        }

        if (evt->offset != 0) {
                status = ATT_ERROR_ATTRIBUTE_NOT_LONG;
                goto write_rsp;
        }

        ble_storage_get_u16(evt->conn_idx, cps->cp_cpcp_ccc_h, &ccc);
        if (!(ccc & GATT_CCC_INDICATIONS)) {
                status = ATT_ERROR_CCC_DESCRIPTOR_IMPROPERLY_CONFIGURED;
                goto write_rsp;
        }

        ble_storage_get_buffer(evt->conn_idx, cps->cp_ctrl_point_h, NULL, (void **) &store_buff);
        /* If storage if empty then nothing is in progress */
        if (store_buff != NULL) {
                status = ATT_ERROR_PROCEDURE_ALREADY_IN_PROGRESS;
        }

write_rsp:
        ble_gatts_write_cfm(evt->conn_idx, cps->cp_ctrl_point_h, status);
        if (status != ATT_ERROR_OK) {
                return;
        }

        switch (evt->value[0]) {
        case CPS_CP_OPCODE_SET_CUMULATIVE_VALUE:
                cp_status = set_cumulative_value(cps, evt->conn_idx, evt->length, evt->value);
                break;
        case CPS_CP_OPCODE_UPDATE_SENSOR_LOCATION:
                cp_status = update_sensor_location(cps, evt->conn_idx, evt->length, evt->value);
                break;
        case CPS_CP_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS:
                cp_status = request_supported_sensor_location(cps, evt->conn_idx,
                                    (sizeof(response) - 3), &response[3], &response_parameter_len);
                value_request = true;
                break;
        case CPS_CP_OPCODE_SET_CRANK_LENGTH:
                cp_status = set_crank_length(cps, evt->conn_idx, evt->length, evt->value);
                break;
        case CPS_CP_OPCODE_REQUEST_CRANK_LENGTH:
                cp_status = request_crank_length(cps, evt->conn_idx, &response[3],
                                                                          &response_parameter_len);
                value_request = true;
                break;
        case CPS_CP_OPCODE_SET_CHAIN_LENGTH:
                cp_status = set_chain_length(cps, evt->conn_idx, evt->length, evt->value);
                break;
        case CPS_CP_OPCODE_REQUEST_CHAIN_LENGTH:
                cp_status = request_chain_length(cps, evt->conn_idx,&response[3],
                                                                          &response_parameter_len);
                value_request = true;
                break;
        case CPS_CP_OPCODE_SET_CHAIN_WEIGHT:
                cp_status = set_chain_weight(cps, evt->conn_idx, evt->length, evt->value);
                break;
        case CPS_CP_OPCODE_REQUEST_CHAIN_WEIGHT:
                cp_status = request_chain_weight(cps, evt->conn_idx, &response[3],
                                                                          &response_parameter_len);
                value_request = true;
                break;
        case CPS_CP_OPCODE_SET_SPAN_LENGTH:
                cp_status = set_span_length(cps, evt->conn_idx, evt->length, evt->value);
                break;
        case CPS_CP_OPCODE_REQUEST_SPAN_LENGTH:
                cp_status = request_span_length(cps, evt->conn_idx, &response[3],
                                                                          &response_parameter_len);
                value_request = true;
                break;
        case CPS_CP_OPCODE_START_OFFSET_COMPENSATION:
                cp_status = start_offset_compensation(cps, evt->conn_idx, evt->length, evt->value);
                break;
        case CPS_CP_OPCODE_MASK_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT:
                cp_status = mask_cyclic_power_meas_char(cps, evt->conn_idx, evt->length,
                                                                                       evt->value);
                break;
        case CPS_CP_OPCODE_REQUEST_SAMPLING_RATE:
                cp_status = request_sampling_rate(cps, evt->conn_idx, &response[3],
                                                                          &response_parameter_len);
                value_request = true;
                break;
        case CPS_CP_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE:
                cp_status = request_factory_calibration_date(cps, evt->conn_idx, &response[3],
                                                                          &response_parameter_len);
                value_request = true;
                break;
        default:
                break;
        }

        /* If request for value not occurred then value is about to be set */
        if ((cp_status == CPS_STATUS_SUCCESS) && !value_request) {
                /* Indication will be send once application confirms operation */
                return;
        }

        /* Send local variable to buffer to avoid sending confirmations from application meaning
         * that if storage is not empty, even if it's zero value, then it's busy.
         */
        ble_storage_put_buffer_copy(evt->conn_idx, cps->cp_ctrl_point_h, sizeof(busy_store_buff),
                                                                   &busy_store_buff, NULL, false);

        /*
         * Response header format:
         * 1 byte: Response opcode
         * 2 byte: Request opcode
         * 3 byte: Operation status
         * 4 ... bytes : optional response parameter valid for
         *      CPS_CP_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS
         *      CPS_CP_OPCODE_REQUEST_CRANK_LENGTH
         *      CPS_CP_OPCODE_REQUEST_SPAN_LENGTH
         *      CPS_CP_OPCODE_REQUEST_CHAIN_LENGTH
         *      CPS_CP_OPCODE_REQUEST_CHAIN_WEIGHT
         *      CPS_CP_OPCODE_REQUEST_SAMPLING_RATE
         *      CPS_CP_OPCODE_REQUEST_FACTORY_CALIBRATION_DATE
         */
        response[0] = CPS_CP_OPCODE_RESPONSE_CODE;
        response[1] = evt->value[0];
        response[2] = cp_status;

        ble_gatts_send_event(evt->conn_idx, cps->cp_ctrl_point_h, GATT_EVENT_INDICATION,
                                                        response_parameter_len + 3, &response[0]);
}

static void handle_cp_vector_ccc_write(cp_service_t *cps, uint16_t conn_idx, uint16_t ccc)
{
        bool enabled = ccc & GATT_CCC_NOTIFICATIONS;

        if (cps->cb && cps->cb->vector_notification_enable) {
                /* Inform that vector notification enabling is on going and send 'enabled' value */
                ble_storage_put_u32(conn_idx, cps->cp_vector_h, CP_VECTOR_CCC_PENDING | ccc, false);

                /* Enable power vector notification regarding to client characteristic
                 * configuration and determine if connection parameters should be updated
                 * to new value (true) or previous connection parameters values (false)
                 * should be set.
                 */
                cps->cb->vector_notification_enable(&cps->svc, conn_idx, enabled);

        } else {
                ble_storage_put_u32(conn_idx, cps->cp_cpv_ccc_h, ccc, true);
                vector_notification_changed(cps, conn_idx, enabled);
                ble_gatts_write_cfm(conn_idx, cps->cp_cpv_ccc_h, ATT_ERROR_OK);
        }

}

static void handle_write_req(ble_service_t *svc, const ble_evt_gatts_write_req_t *evt)
{
        cp_service_t *cps = (cp_service_t*) svc;
        att_error_t status = ATT_ERROR_WRITE_NOT_PERMITTED;

        uint16_t ccc;

        if (evt->handle == cps->cp_ctrl_point_h) {
                handle_cp_control_point_write(cps, evt);
                return;
        } else if (evt->handle == cps->cp_measurement_ccc_h || evt->handle == cps->cp_cpv_ccc_h
                                                            || evt->handle == cps->cp_cpcp_ccc_h) {

                if (evt->offset) {
                        status = ATT_ERROR_ATTRIBUTE_NOT_LONG;
                        goto write_cfm;
                }

                if (evt->length != sizeof(ccc)) {
                        status  = ATT_ERROR_INVALID_VALUE_LENGTH;
                        goto write_cfm;
                }

                ccc = get_u16(evt->value);

                if (evt->handle == cps->cp_measurement_ccc_h) {
                        ble_storage_put_u32(evt->conn_idx, evt->handle, ccc, true);
                        measurement_notification_changed(cps, evt->conn_idx,
                                                                     ccc & GATT_CCC_NOTIFICATIONS);
                        status = ATT_ERROR_OK;
                } else if (evt->handle == cps->cp_cpv_ccc_h) {
                        handle_cp_vector_ccc_write(cps, evt->conn_idx, ccc);
                        return;
                } else if (evt->handle == cps->cp_cpcp_ccc_h) {
                        ble_storage_put_u32(evt->conn_idx, evt->handle, ccc, true);
                        status = ATT_ERROR_OK;;
                }
        }

write_cfm:
        ble_gatts_write_cfm(evt->conn_idx, evt->handle, status);
}

ble_service_t *cps_init(const ble_service_config_t *config, const cps_config_t *cps_config,
                                                                     const cps_callbacks_t *cb)
{
        cp_service_t *cps;
        uint16_t num_attr;
        att_uuid_t uuid;
        att_perm_t read_perm, write_perm;
        bool sensor_locations_valid = false;
        bool calibration_date_valid = false;

        /* Check if multiple locations are supported and location count isn't zero */
        if ((cps_config->supported_features & CPS_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORT) &&
                                              (cps_config->supported_sensor_locations_count > 0)) {
                /** Check if locations are provided*/
                if (!cps_config->supported_sensor_locations) {
                        return NULL;
                }

                sensor_locations_valid = true;
        }

        /* Check if calibration date is supported */
        if (cps_config->supported_features & CPS_FEATURE_FACTORY_CALIBRATION_DATE_SUPPORT) {

                /** Check if date values are valid */
                if ((cps_config->calibration_date.year  == 0) ||
                        (cps_config->calibration_date.month == 0) ||
                        (cps_config->calibration_date.day   == 0)) {
                        return NULL;
                }

                calibration_date_valid = true;
        }

        cps = OS_MALLOC(sizeof(*cps));
        memset(cps, 0, sizeof(*cps));

        cps->cb = cb;
        cps->supported_features = cps_config->supported_features;
        cps->svc.read_req = handle_read_req;
        cps->svc.write_req = handle_write_req;
        cps->svc.connected_evt = handle_connect_evt;
        cps->svc.disconnected_evt = handle_disconnect_evt;
        cps->svc.event_sent = handle_event_sent_evt;

        /* Set measurement context feature bit according to initial configuration */
        if (cps_config->cfg_measurement_context_type ==
                                                     CPS_SENSOR_MEASUREMENT_CONTEXT_TORQUE_BASED) {
                cps->supported_features |= CPS_FEATURE_SENSOR_MEASUREMENT_CONTEXT_SUPPORT;
        } else {
                cps->supported_features &= ~CPS_FEATURE_SENSOR_MEASUREMENT_CONTEXT_SUPPORT;
        }

        num_attr = cps_get_num_att(config, cps_config);

        read_perm = ble_service_config_elevate_perm(ATT_PERM_READ, config);
        write_perm = ble_service_config_elevate_perm(ATT_PERM_WRITE, config);

        /* Cycling Power Service */
        ble_uuid_create16(UUID_SERVICE_CPS, &uuid);
        ble_gatts_add_service(&uuid, GATT_SERVICE_PRIMARY, num_attr);

        /* Include services */
        ble_service_config_add_includes(config);

        /* Add Power Measurement */
        ble_uuid_create16(UUID_CYCLING_POWER_MEASUREMENT, &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_NOTIFY, ATT_PERM_NONE, 0, 0, NULL,
                                                                       &cps->cp_measurement_h);
        /* Add Power Measurement CCC descriptor */
        add_ccc(&cps->cp_measurement_ccc_h);

        /* Add Power Feature */
        ble_uuid_create16(UUID_CYCLING_POWER_FEATURE, &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_READ, read_perm, CPS_FEATURE_SIZE, 0,
                                                                        NULL, &cps->cp_features_h);

        /* Add Sensor Location */
        ble_uuid_create16(UUID_CYCLING_POWER_SENSOR_LOCATION, &uuid);
        ble_gatts_add_characteristic(&uuid, GATT_PROP_READ, read_perm, SENSOR_LOCATION_SIZE, 0,
                                                                   NULL, &cps->cp_sens_location_h);

        /* Add Power Vector if supported*/
        if (cps_config->power_vector_support) {
                ble_uuid_create16(UUID_CYCLING_POWER_VECTOR, &uuid);
                ble_gatts_add_characteristic(&uuid, GATT_PROP_NOTIFY, ATT_PERM_NONE,
                                                CPS_POWER_VECTOR_SIZE, 0, NULL, &cps->cp_vector_h);
                /* Add Power Vector CCC descriptor */
                add_ccc(&cps->cp_cpv_ccc_h);
        }

        /* Add Power Control Point and CCC */
        if (cps_config->supported_features & CPS_ALL_SUPPORTED_FEATURES_MASK) {

                ble_uuid_create16(UUID_CYCLING_POWER_CONTROL_POINT, &uuid);
                ble_gatts_add_characteristic(&uuid, GATT_PROP_WRITE | GATT_PROP_INDICATE, write_perm,
                                           CPS_CONTROL_POINT_SIZE, 0, NULL, &cps->cp_ctrl_point_h);

                /* Add Power Control Point CCC descriptor */
                add_ccc(&cps->cp_cpcp_ccc_h);
        }

        /* Register all characteristics */
        ble_gatts_register_service(&cps->svc.start_h, &cps->cp_measurement_h,
                     &cps->cp_measurement_ccc_h, &cps->cp_features_h, &cps->cp_sens_location_h, 0);

        cps->svc.end_h = cps->svc.start_h + num_attr;

        /* Select type of measurements (force or torque based) */
        if (cps_config->cfg_measurement_context_type ==
                                                      CPS_SENSOR_MEASUREMENT_CONTEXT_FORCE_BASED) {
            cps->measurements_context_type = CPS_SENSOR_MEASUREMENT_CONTEXT_FORCE_BASED;
        } else {
            cps->measurements_context_type = CPS_SENSOR_MEASUREMENT_CONTEXT_TORQUE_BASED;
        }

        if (sensor_locations_valid) {
                cps->sensor_locations_count = cps_config->supported_sensor_locations_count;
                cps->sensor_locations = OS_MALLOC(cps->sensor_locations_count);
                memcpy(cps->sensor_locations, cps_config->supported_sensor_locations,
                                                                      cps->sensor_locations_count);
        }

        if (calibration_date_valid) {
                memcpy(&cps->factory_calibration_date, &cps_config->calibration_date,
                                                            sizeof(cps->factory_calibration_date));
        }

        if ((cps->cp_ctrl_point_h) && (cps_config->sampling_rate > 0)) {
                cps->sampling_rate = cps_config->sampling_rate;
        }

        if (cps->cp_ctrl_point_h && (cps_config->init_crank_length > 0)) {
                cps->crank_length = cps_config->init_crank_length;
        }

        if (cps->cp_ctrl_point_h && (cps_config->init_chain_length > 0)) {
                cps->chain_length = cps_config->init_chain_length;
        }

        if (cps->cp_ctrl_point_h && (cps_config->init_chain_weight > 0)) {
                cps->chain_weight = cps_config->init_chain_weight;
        }

        if (cps->cp_ctrl_point_h && (cps_config->init_span_length > 0)) {
                cps->span_length = cps_config->init_span_length;
        }

        if (cps->cp_ctrl_point_h) {
                cps->cp_ctrl_point_h += cps->svc.start_h;
                cps->cp_cpcp_ccc_h += cps->svc.start_h;
        }

        if (cps->cp_vector_h) {
                cps->cp_vector_h += cps->svc.start_h;
                cps->cp_cpv_ccc_h += cps->svc.start_h;
        }

        set_cp_feature_value(cps, cps->supported_features);
        set_sensor_location_value(cps, cps_config->init_location);

        return &cps->svc;
}

bool cps_vector_notification_enable_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status)
{
        cp_service_t *cps = (cp_service_t *) svc;
        ble_error_t ret;
        uint32_t vector_value = 0;
        uint16_t ccc;

        if (cps->cp_cpv_ccc_h == 0) {
                return false;
        }

        /* Get value that was stored while handling write to vector ccc */
        ble_storage_get_u32(conn_idx, cps->cp_vector_h, &vector_value);
        if (!vector_value) {
                return false;
        }

        ret = ble_gatts_write_cfm(conn_idx, cps->cp_cpv_ccc_h, status);

        /* Get ccc value from vector_value before storing it in vector ccc handle */
        ccc = (~CP_VECTOR_CCC_PENDING & vector_value);

        if (status == ATT_ERROR_OK) {
                vector_notification_changed(cps, conn_idx, (ccc & GATT_CCC_NOTIFICATIONS));
                ble_storage_put_u32(conn_idx, cps->cp_cpv_ccc_h, ccc, true);
        }

        /* Remove storage to indicate that notification enabling procedure has ended */
        ble_storage_remove(conn_idx, cps->cp_vector_h);

        return ret == BLE_STATUS_OK;
}

bool cps_send_cp_measurement(ble_service_t *svc, uint16_t conn_idx,
                                                              const cps_measurement_t *measurement)
{
        cp_service_t *cps = (cp_service_t *) svc;
        uint8_t notification[POWER_MEASUREMENT_SIZE];
        uint8_t *ptr = &notification[2];
        uint16_t ccc = 0x0000;
        ble_error_t ret;
        cps_measurement_flags_t flags = 0;
        cps_measurement_t measure = *measurement;

        ble_storage_get_u16(conn_idx, cps->cp_measurement_ccc_h, &ccc);
        if (!(ccc & GATT_CCC_NOTIFICATIONS)) {
                return false;
        }

        memset(notification, 0, sizeof(notification));

        /* Set which measurement to disable if masking occurred */
        if (cps->measurement_flag_mask > 0) {
                /* Mask Pedal Power Balance */
                measure.pedal_power_balance_present = ~cps->measurement_flag_mask & (1 << 0);
                /* Mask Accumulated Torque */
                measure.accumulated_torque_present = ~cps->measurement_flag_mask & (1 << 1);
                /* Mask Wheel Revolution Data */
                measure.wheel_revolution_data_present = ~cps->measurement_flag_mask & (1 << 2);
                /* Mask Crank Revolution Data */
                measure.crank_revolution_data_present = ~cps->measurement_flag_mask & (1 << 3);
                /* Mask Extreme Magnitudes - Force Magnitude */
                measure.extreme_force_magnitude_present = ~cps->measurement_flag_mask & (1 << 4);
                /* Mask Extreme Magnitudes - Torque Magnitude */
                measure.extreme_torque_magnitude_present = ~cps->measurement_flag_mask & (1 << 4);
                /* Mask Extreme Angles */
                measure.extreme_angle_present = ~cps->measurement_flag_mask & (1 << 5);
                /* Mask Top Dead Spot Angle */
                measure.top_dead_spot_angle_present = ~cps->measurement_flag_mask & (1 << 6);
                /* Mask Bottom Dead Spot Angle */
                measure.bottom_dead_spot_angle_present = ~cps->measurement_flag_mask & (1 << 7);
                /* Mask Accumulated Energy */
                measure.accumulated_energy_present = ~cps->measurement_flag_mask & (1 << 8);
                /* Reset mask to prevent caching */
                cps->measurement_flag_mask = 0;
        }

        put_u16_inc(&ptr, measurement->instant_power);

        if (measure.pedal_power_balance_present) {
                flags |= CPS_PM_FLAGS_PEDAL_POWER_BALANCE_PRESENT;
                put_u8_inc(&ptr, measurement->pedal_power_balance);
        }

        if (measure.accumulated_torque_present) {
                flags |= CPS_PM_FLAGS_ACCUMULATED_TORQUE_PRESENT;
                put_u16_inc(&ptr, measurement->accumulated_torque);
        }

        if (measure.wheel_revolution_data_present) {
                flags |= CPS_PM_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT;
                put_u32_inc(&ptr, measurement->wrd_cumulative_revol);
                put_u16_inc(&ptr, measurement->wrd_last_wheel_evt_time);
        }

        if (measure.crank_revolution_data_present) {
                flags |= CPS_PM_FLAGS_CRANK_REVOLUTION_DATA_PRESENT;
                put_u16_inc(&ptr, measurement->crd_cumulative_revol);
                put_u16_inc(&ptr, measurement->crd_last_crank_evt_time);
        }

        if (measure.extreme_force_magnitude_present &&
                 (cps->measurements_context_type ==  CPS_SENSOR_MEASUREMENT_CONTEXT_FORCE_BASED)) {
                flags |= CPS_PM_FLAGS_EXTREME_FORCE_MAGNITUDES_PRESENT;
                put_u16_inc(&ptr, measurement->efm_max_force_magnitude);
                put_u16_inc(&ptr, measurement->efm_min_force_magnitude);
        }

        if (measure.extreme_torque_magnitude_present &&
                 (cps->measurements_context_type == CPS_SENSOR_MEASUREMENT_CONTEXT_TORQUE_BASED)) {
                flags |= CPS_PM_FLAGS_EXTREME_TORQUE_MAGNITUDES_PRESENT;
                put_u16_inc(&ptr, measurement->etm_max_torq_magnitude);
                put_u16_inc(&ptr, measurement->etm_min_torq_magnitude);
        }

        if (measure.extreme_angle_present) {
                uint32_t angles;

                angles = (measurement->ea_minimum_angle << 12) |
                                                          (measurement->ea_maximum_angle & 0x0FFF);
                flags |= CPS_PM_FLAGS_EXTREME_ANGLES_PRESENT;
                /* We need only 24-bit of little-endian value */
                put_data_inc(&ptr, 3, &angles);
        }

        if (measure.top_dead_spot_angle_present) {
                flags |= CPS_PM_FLAGS_TOP_DEAD_SPOT_ANGLE_PRESENT;
                put_u16_inc(&ptr, measurement->top_dead_spot_angle);
        }

        if (measure.bottom_dead_spot_angle_present) {
                flags |= CPS_PM_FLAGS_BOTTOM_DEAD_SPOT_ANGLE_PRESENT;
                put_u16_inc(&ptr, measurement->bottom_dead_spot_angle);
        }

        if (measure.accumulated_energy_present) {
                flags |= CPS_PM_FLAGS_ACCUMULATED_ENERGY_PRESENT;
                put_u16_inc(&ptr, measurement->accumulated_torque);
        }

        if (measure.offset_compensation_indicator) {
                flags |= CPS_PM_FLAGS_OFFSET_COMPENSATION_INDICATOR;
        }

        put_u16(&notification[0], flags);
        ret = ble_gatts_send_event(conn_idx, cps->cp_measurement_h, GATT_EVENT_NOTIFICATION,
                                                                 ptr - notification, notification);

        return ret == BLE_STATUS_OK;
}

bool cps_send_cp_vector(ble_service_t *svc, uint16_t conn_idx, const cps_power_vector_t *vector)
{
        cp_service_t *cps = (cp_service_t *) svc;
        uint8_t notification[CPS_POWER_VECTOR_SIZE];
        uint8_t *ptr= &notification[1];;
        uint16_t ccc = 0x0000;
        ble_error_t ret;
        cps_vector_flags_t flags = 0;

        ble_storage_get_u16(conn_idx, cps->cp_cpv_ccc_h, &ccc);
        if (!(ccc & GATT_CCC_NOTIFICATIONS)) {
                return false;
        }

        memset(notification, 0, sizeof(*notification));

        if (vector->crank_revolution_data_present) {
                flags |= CPS_VECTOR_CRANK_REVOLUTION_DATA_PRESENT;
                put_u16_inc(&ptr, vector->crs_cumul_crank_revol);
                put_u16_inc(&ptr, vector->crs_last_crank_evt_time);
        }

        if (vector->crank_measurement_angle_present) {
                flags |= CPS_VECTOR_CRANK_MEASUREMENT_ANGLE_PRESENT;
                put_u16_inc(&ptr, vector->first_crank_measur_angle);
        }

        /* If magnitude length isn't zero then there is measurement present */
        if (vector->instantaneous_magnitude_len > 0) {
                uint8_t i;
                /* Maximum magnitude length. Determine how many bytes are left for magnitude */
                uint8_t max_magnitude_len = (sizeof(notification) - (ptr - notification)) /
                                                                                  sizeof(uint16_t);

                if (max_magnitude_len > vector->instantaneous_magnitude_len) {
                        max_magnitude_len = vector->instantaneous_magnitude_len;
                }

                cps->supported_features & CPS_FEATURE_SENSOR_MEASUREMENT_CONTEXT_SUPPORT ?
                        (flags |= CPS_VECTOR_INSTANT_TORQUE_MAGNITUDE_PRESENT) :
                        (flags |= CPS_VECTOR_INSTANT_FORCE_MAGNITUDE_PRESENT);

                for (i = 0; i < max_magnitude_len; i++) {
                        put_u16_inc(&ptr, vector->instantaneous_magnitude_arr[i]);
                }
        }

        /* Set only bits allowed by vector_direction */
        flags |= (CPS_VECTOR_SUPPORTED_DIRECTIONS_MASK & vector->vector_direction);
        put_u8(&notification[0], flags);
        ret = ble_gatts_send_event(conn_idx, cps->cp_vector_h, GATT_EVENT_NOTIFICATION,
                                                                 ptr - notification, notification);

        return ret == BLE_STATUS_OK;
}

static ble_error_t confirmation_helper(ble_service_t *svc, uint16_t conn_idx,
                                                       cps_cp_opcode_t opcode, cps_status_t status)
{
        cp_service_t *cps = (cp_service_t *) svc;
        ble_error_t ret;
        uint8_t response[3];
        cps_request_values_t *get_buff;

        if (cps->cp_ctrl_point_h == 0) {
                return BLE_ERROR_FAILED;
        }

        ble_storage_get_buffer(conn_idx, cps->cp_ctrl_point_h, NULL, (void **) &get_buff);
        if ((get_buff->opcode) != opcode) {
                return BLE_ERROR_FAILED;
        }

        response[0] = CPS_CP_OPCODE_RESPONSE_CODE;
        response[1] = opcode;
        response[2] = status;

        ret = ble_gatts_send_event(conn_idx, cps->cp_ctrl_point_h, GATT_EVENT_INDICATION,
                                                                       sizeof(response), response);

        /* Remove data from storage if sending indication failed to allow new CP requests */
        if (ret != BLE_STATUS_OK) {
                ble_storage_remove(conn_idx, cps->cp_ctrl_point_h);
                return ret;
        }

        if (status != CPS_STATUS_SUCCESS) {
                return ret;
        }

        switch (opcode) {
        case CPS_CP_OPCODE_UPDATE_SENSOR_LOCATION :
                set_sensor_location_value(cps,
                                         (cps_sensor_location_t)get_buff->requested_value);
                break;
        case CPS_CP_OPCODE_SET_CRANK_LENGTH:
                cps->crank_length = get_buff->requested_value;
                break;
        case CPS_CP_OPCODE_SET_CHAIN_LENGTH:
                cps->chain_length = get_buff->requested_value;
                break;
        case CPS_CP_OPCODE_SET_CHAIN_WEIGHT:
                cps->chain_weight = get_buff->requested_value;
                break;
        case CPS_CP_OPCODE_SET_SPAN_LENGTH:
                cps->span_length = get_buff->requested_value;
                break;
        case CPS_CP_OPCODE_MASK_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT:
                cps->measurement_flag_mask = get_buff->requested_value;
                break;
        default:
                break;
        }

        return ret;
}

bool cps_set_cumulative_value_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status)
{
        ble_error_t ret;

        ret = confirmation_helper(svc, conn_idx, CPS_CP_OPCODE_SET_CUMULATIVE_VALUE, status);

        return ret == BLE_STATUS_OK;
}

bool cps_update_sensor_location_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status)
{
        ble_error_t ret;

        ret= confirmation_helper(svc, conn_idx, CPS_CP_OPCODE_UPDATE_SENSOR_LOCATION, status);

        return ret == BLE_STATUS_OK;
}

bool cps_set_crank_length_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status)
{
        ble_error_t ret;

        ret = confirmation_helper(svc, conn_idx, CPS_CP_OPCODE_SET_CRANK_LENGTH, status);

        return ret == BLE_STATUS_OK;
}

bool cps_set_span_length_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status)
{
        ble_error_t ret;

        ret = confirmation_helper(svc, conn_idx, CPS_CP_OPCODE_SET_SPAN_LENGTH, status);

        return ret == BLE_STATUS_OK;
}

bool cps_set_chain_length_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status)
{
        ble_error_t ret;

        ret = confirmation_helper(svc, conn_idx, CPS_CP_OPCODE_SET_CHAIN_LENGTH, status);

        return ret == BLE_STATUS_OK;
}

bool cps_set_chain_weight_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status)
{
        ble_error_t ret;

        ret = confirmation_helper(svc, conn_idx, CPS_CP_OPCODE_SET_CHAIN_WEIGHT, status);

        return ret == BLE_STATUS_OK;
}

bool cps_start_offset_compensation_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status,
                                                                                    int16_t offset)
{
        cp_service_t *cps = (cp_service_t *) svc;
        ble_error_t ret;
        uint8_t response[5];
        cps_request_values_t *get_buff;

        if (cps->cp_ctrl_point_h == 0) {
                return false;
        }

        ble_storage_get_buffer(conn_idx, cps->cp_ctrl_point_h, NULL, (void **) &get_buff);
        if ((get_buff->opcode) != CPS_CP_OPCODE_START_OFFSET_COMPENSATION) {
                return false;
        }

        response[0] = CPS_CP_OPCODE_RESPONSE_CODE;
        response[1] = CPS_CP_OPCODE_START_OFFSET_COMPENSATION;
        response[2] = status;
        put_u16(&response[3], offset);

        ret = ble_gatts_send_event(conn_idx, cps->cp_ctrl_point_h, GATT_EVENT_INDICATION,
                                                                       sizeof(response), response);

        /* Remove data from storage if sending indication failed to allow new CP requests */
        if (ret != BLE_STATUS_OK) {
                ble_storage_remove(conn_idx, cps->cp_ctrl_point_h);
                return false;
        }

        if (status != CPS_STATUS_SUCCESS) {
                return false;
        }

        return ret == BLE_STATUS_OK;
}

bool cps_mask_cp_measurement_char_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status)
{
        ble_error_t ret;

        ret = confirmation_helper(svc, conn_idx,
                      CPS_CP_OPCODE_MASK_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT, status);

        return ret == BLE_STATUS_OK;
}
#endif /* defined(CONFIG_USE_BLE_SERVICES) */
