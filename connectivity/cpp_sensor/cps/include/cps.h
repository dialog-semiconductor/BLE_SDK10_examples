/**
 ****************************************************************************************
 *
 * @file cps.h
 *
 * @brief Cycling Power Service implementation API
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
#ifndef CPS_H_
#define CPS_H_

#include <stdint.h>
#include "ble_service.h"
#include "svc_types.h"

/**
 * Cycling Power Feature characteristic bit values
 */
typedef enum {
        /* Pedal Power Balance */
        CPS_FEATURE_PEDAL_POWER_BALANCE_SUPPORT             = 0x00001, /** (1 << 0)  */
        /* Accumulated Torque */
        CPS_FEATURE_ACCUMULATED_TORQUE_SUPPORRT             = 0x00002, /** (1 << 1)  */
        /* Wheel Revolution Data */
        CPS_FEATURE_WHEEL_REVOLUTION_DATA_SUPPORT           = 0x00004, /** (1 << 2)  */
        /* Crank Revolution Data */
        CPS_FEATURE_CRANK_REVOLUTION_DATA_SUPPORT           = 0x00008, /** (1 << 3)  */
        /* Extreme Magnitudes */
        CPS_FEATURE_EXTREME_MAGNITUDE_SUPPORT               = 0x00010, /** (1 << 4)  */
        /* Extreme Angle */
        CPS_FEATURE_EXTREME_ANGLE_SUPPORT                   = 0x00020, /** (1 << 5)  */
        /* Top and Bottom Dead Spot Angles */
        CPS_FEATURE_TOP_AND_BOTTOM_DEAD_SPOT_ANGLES_SUPPORT = 0x00040, /** (1 << 6)  */
        /* Accumulated Energy */
        CPS_FEATURE_ACCUMULATED_ENERGY_SUPPORT              = 0x00080, /** (1 << 7)  */
        /* Offset Compensation Indicator */
        CPS_FEATURE_OFFSET_COMPENSATION_INDICATOR_SUPPORT   = 0x00100, /** (1 << 8)  */
        /* Offset Compensation */
        CPS_FEATURE_OFFSET_COMPENSATION_SUPPORT             = 0x00200, /** (1 << 9)  */
        /* Cyclic Power Measurement Characteristic Content Masking */
        CPS_FEATURE_CHARACTERISTIC_CONTENT_MASKING_SUPPORT  = 0x00400, /** (1 << 10) */
        /* Multiple Sensor Locations */
        CPS_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORT       = 0x00800, /** (1 << 11) */
        /* Crank Length Adjustment */
        CPS_FEATURE_CRANK_LENGTH_ADJUSTMENT_SUPPORT         = 0x01000, /** (1 << 12) */
        /* Chain Length Adjustment */
        CPS_FEATURE_CHAIN_LENGTH_ADJUSTMENT_SUPPORT         = 0x02000, /** (1 << 13) */
        /* Chain Weight Adjustment */
        CPS_FEATURE_CHAIN_WEIGHT_ADJUSTMENT_SUPPORT         = 0x04000, /** (1 << 14) */
        /* Span Length Adjustment */
        CPS_FEATURE_SPAN_LENGTH_ADJUSTMENT_SUPPORT          = 0x08000, /** (1 << 15) */
        /* Sensor Measurement Context */
        CPS_FEATURE_SENSOR_MEASUREMENT_CONTEXT_SUPPORT      = 0x10000, /** (1 << 16) */
        /* Instantaneous Measurement Direction */
        CPS_FEATURE_INSTANTANEOUS_MEASUREMENT_DIR_SUPPORT   = 0x20000, /** (1 << 17) */
        /* Factory Calibration Date */
        CPS_FEATURE_FACTORY_CALIBRATION_DATE_SUPPORT        = 0x40000, /** (1 << 18) */
} cps_feature_t;

/**
 * Cycling Power Feature values for determining sensor context
 * to force based or torque based measurement type
 */
typedef enum {
        /* Sensor Measurement Context Force Based */
        CPS_SENSOR_MEASUREMENT_CONTEXT_FORCE_BASED  = 0x00,
        /* Sensor Measurement Context Torque Based */
        CPS_SENSOR_MEASUREMENT_CONTEXT_TORQUE_BASED = 0x01,
} cps_sensor_measurement_context_t;

/**
 * Cycling Power Vector measurements direction
 */
typedef enum {
        /* Instantaneous Measurement Direction Tangential Component */
        CPS_VECTOR_INSTANT_MEASUREMENT_DIRECTION_TANGENTIAL  = 0x10,
        /* Instantaneous Measurement Direction Radial Component */
        CPS_VECTOR_INSTANT_MEASUREMENT_DIRECTION_RADIAL      = 0x20,
        /* Instantaneous Measurement Direction Lateral Component */
        CPS_VECTOR_INSTANT_MEASUREMENT_DIRECTION_LATERAL     = 0x30,
} cps_vector_direction_t;

/**
 * Cycling Power Measurement pedal power balance reference
 */
typedef enum {
        /* Pedal power balance unknown */
        CPS_PEDAL_POWER_BALANCE_REFERENCE_UNKNOW   = 0x00,
        /* Pedal power balance left */
        CPS_PEDAL_POWER_BALANCE_REFERENCE_LEFT     = 0x01,
} cps_pedal_power_balance_reference_t;

/**
 * Cycling Power Measurement accumulated torque source
 */
typedef enum {
        /* Accumulated torque source wheel based */
        CPS_ACCUMULATED_TORQUE_SOURCE_WHEEL_BASED   = 0x00,
        /* Accumulated torque source crank based */
        CPS_ACCUMULATED_TORQUE_SOURCE_CRANK_BASED   = 0x01,
} cps_accumulated_torque_source_t;

/**
 * Sensor locations
 */
typedef enum {
        /* Other */
        CPS_SENSOR_LOCATION_OTHER        = 0x00,
        /* Top of shoe */
        CPS_SENSOR_LOCATION_TOP_OF_SHOE  = 0x01,
        /* In shoe */
        CPS_SENSOR_LOCATION_IN_SHOE      = 0x02,
        /* Hip */
        CPS_SENSOR_LOCATION_HIP          = 0x03,
        /* Front Wheel */
        CPS_SENSOR_LOCATION_FRONT_WHEEL  = 0x04,
        /* Left Crank */
        CPS_SENSOR_LOCATION_LEFT_CRANK   = 0x05,
        /* Right Crank */
        CPS_SENSOR_LOCATION_RIGHT_CRANK  = 0x06,
        /* Left Pedal */
        CPS_SENSOR_LOCATION_LEFT_PEDAL   = 0x07,
        /* Right Pedal */
        CPS_SENSOR_LOCATION_RIGHT_PEDAL  = 0x08,
        /* Front Hub */
        CPS_SENSOR_LOCATION_FRONT_HUB    = 0x09,
        /* Rear Dropout */
        CPS_SENSOR_LOCATION_REAR_DROPOUT = 0x0a,
        /* Chainstay */
        CPS_SENSOR_LOCATION_CHAINSTAY    = 0x0b,
        /* Rear Wheel */
        CPS_SENSOR_LOCATION_REAR_WHEEL   = 0x0c,
        /* Rear Hub */
        CPS_SENSOR_LOCATION_REAR_HUB     = 0x0d,
        /* Chest */
        CPS_SENSOR_LOCATION_CHEST        = 0x0e,
} cps_sensor_location_t;

/**
 * Status
 */
typedef enum {
        /* Status success */
        CPS_STATUS_SUCCESS              = 0x01,
        /* Status opcode not supported */
        CPS_STATUS_OPCODE_NOT_SUPPORTED = 0x02,
        /*  Status invalid parameters */
        CPS_STATUS_INVALID_PARAM        = 0x03,
        /* Status operation failed */
        CPS_STATUS_OPERATION_FAILED     = 0x04,
} cps_status_t;

/**
 * \brief Measurement notification changed callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] enabled          measurement notification enabled
 *
 */
typedef void (* cps_measure_notif_changed_cb_t)  (ble_service_t *svc, uint16_t conn_idx,
                                                                                     bool enabled);

/**
 * \brief Power Vector notification changed callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] enabled          vector notification enabled
 *
 */
typedef void (* cps_vector_notif_changed_cb_t)   (ble_service_t *svc, uint16_t conn_idx,
                                                                                     bool enabled);

/**
 * \brief Update sensor location callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] location         sensor location
 *
 */
typedef void (* cps_update_sensor_location_cb_t) (ble_service_t *svc, uint16_t conn_idx,
                                                                   cps_sensor_location_t location);

/**
 * \brief Set cumulative value callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] value            cumulative value
 *
 */
typedef void (* cps_set_cumulative_value_cb_t)   (ble_service_t *svc, uint16_t conn_idx,
                                                                                   uint32_t value);

/**
 * \brief Set crank length callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] length           crank length
 *
 */
typedef void (* cps_set_crank_length_cb_t)       (ble_service_t *svc, uint16_t conn_idx,
                                                                                  uint16_t length);

/**
 * \brief Set chain length callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] length           chain length
 *
 */
typedef void (* cps_set_chain_length_cb_t)       (ble_service_t *svc, uint16_t conn_idx,
                                                                                  uint16_t length);

/**
 * \brief Set chain weight callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] weight           chain weight
 *
 */
typedef void (* cps_set_chain_weight_cb_t)       (ble_service_t *svc, uint16_t conn_idx,
                                                                                  uint16_t weight);

/**
 * \brief Set span length callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] length           chain length
 *
 */
typedef void (* cps_set_span_length_cb_t)        (ble_service_t *svc, uint16_t conn_idx,
                                                                                  uint16_t length);

/**
 * \brief Start offset compensation callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 *
 */
typedef void (* cps_start_offset_compensation_cb_t)      (ble_service_t *svc, uint16_t conn_idx);

/**
 * \brief Mask control point measurement characteristic content callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] mask             features to be masked
 *
 */
typedef void (* cps_mask_cp_measurent_char_content_cb_t) (ble_service_t *svc, uint16_t conn_idx,
                                                                                    uint16_t mask);

/**
 * \brief Power vector notification enable callback
 *
 * \param [in] svc              ble_service instance
 * \param [in] conn_idx         connection index
 * \param [in] enabled          true if notifications for Power Vector are being enabled,
 *                              false otherwise
 *
 */
typedef void (* cps_vector_notification_enable_cb_t) (ble_service_t *svc, uint16_t conn_idx,
                                                                                     bool enabled);
/**
 * CPS application callbacks
 */
typedef struct {
        /* Notification status for Cycling Power Measurement changed by client */
        cps_measure_notif_changed_cb_t measur_notif_changed;
        /* Notification status for Cycling Power Vector changed by client */
        cps_vector_notif_changed_cb_t power_vector_notif_changed;
        /* Client update sensor location callback */
        cps_update_sensor_location_cb_t update_sensor_location;
        /* Client set cumulative value callback */
        cps_set_cumulative_value_cb_t set_cumulative_value;
        /* Client set crank length value callback */
        cps_set_crank_length_cb_t set_crank_length;
        /* Client set chain length value callback */
        cps_set_chain_length_cb_t set_chain_length;
        /* Client set chain weight value callback */
        cps_set_chain_weight_cb_t set_chain_weight;
        /* Client set span length value callback */
        cps_set_span_length_cb_t set_span_length;
        /* Client start offset compensation procedure callback */
        cps_start_offset_compensation_cb_t start_offset_compensation;
        /* Client mask cycling power measurement characteristic content callback*/
        cps_mask_cp_measurent_char_content_cb_t mask_cp_measurement_char_content;
        /* Client power vector notification enable callback */
        cps_vector_notification_enable_cb_t vector_notification_enable;
} cps_callbacks_t;

/**
 * CPS Measurements
 *
 * It corresponds to CPS measurement characteristic value
 */
typedef struct {
        /* Pedal Power Balance Present */
        bool pedal_power_balance_present        : 1;
        /* Accumulated Torque Present */
        bool accumulated_torque_present         : 1;
        /* Wheel Revolution Data Present */
        bool wheel_revolution_data_present      : 1;
        /* Crank Revolution Data Present */
        bool crank_revolution_data_present      : 1;
        /* Extreme Force Magnitudes Present */
        bool extreme_force_magnitude_present    : 1;
        /* Extreme Torque Magnitudes Present */
        bool extreme_torque_magnitude_present   : 1;
        /* Extreme Angles Present */
        bool extreme_angle_present              : 1;
        /* Top Dead Spot Angle Present */
        bool top_dead_spot_angle_present        : 1;
        /* Bottom Dead Spot Angle Present */
        bool bottom_dead_spot_angle_present     : 1;
        /* Accumulated Energy Present */
        bool accumulated_energy_present         : 1;
        /* Offset Compensation Indicator */
        bool offset_compensation_indicator      : 1;
        /* Instantaneous Power */
        int16_t instant_power;
        /* Pedal Power balance */
        uint8_t pedal_power_balance;
        /* Accumulated Torque */
        uint16_t accumulated_torque;
        /* Wheel Revolution Data - Cumulative Wheel Revolutions */
        uint32_t wrd_cumulative_revol;
        /* Wheel Revolution Data - Last Wheel Event Time */
        uint16_t wrd_last_wheel_evt_time;
        /* Crank Revolution Data - Cumulative Crank Revolutions */
        uint16_t crd_cumulative_revol;
        /* Crank Revolution Data - Last Crank Event Time */
        uint16_t crd_last_crank_evt_time;
        /* Extreme Force Magnitude - Maximum Force Magnitude */
        int16_t efm_max_force_magnitude;
        /* Extreme Force Magnitude - Minimum Force Magnitude */
        int16_t efm_min_force_magnitude;
        /* Extreme Torque Magnitude - Maximum Torque Magnitude */
        int16_t etm_max_torq_magnitude;
        /* Extreme Torque Magnitude - Minimum Torque Magnitude */
        int16_t etm_min_torq_magnitude;
        /* Extreme Angle - Maximum Angle */
        uint16_t ea_maximum_angle;
        /* Extreme Angle - Minimum Angle */
        uint16_t ea_minimum_angle;
        /* Top Dead Spot Angle */
        uint16_t top_dead_spot_angle;
        /* Bottom Dead Spot Angle */
        uint16_t bottom_dead_spot_angle;
        /* Accumulated Energy */
        uint16_t accumulated_energy;
} cps_measurement_t;

/**
 * CPS Power Vector
 *
 * It corresponds to CPS Power Vector characteristic raw data (either force or torque)
 */
typedef struct {
        /* Crank Revolution Data Present */
        bool crank_revolution_data_present          : 1;
        /* First Crank Measurement Angle Present */
        bool crank_measurement_angle_present        : 1;
        /* Cycling Power Vector measurement direction values */
        cps_vector_direction_t vector_direction;
        /* Crank Revolution Data - Cumulative Crank Revolution */
        uint16_t crs_cumul_crank_revol;
        /* Crank Revolution Data - Last Crank Event Time */
        uint16_t crs_last_crank_evt_time;
        /* First Crank Measurement Angle */
        uint16_t first_crank_measur_angle;
        /* Instantaneous Magnitude Length.
         * If length is zero then force or torque measurements aren't present
         */
        uint8_t instantaneous_magnitude_len;
        /* Instantaneous Magnitude Array for force or torque measurements
         * depending on measurements context setup
         */
        int16_t instantaneous_magnitude_arr[];
} cps_power_vector_t;

/**
 * CPS config used during initialization
 */
typedef struct {
        /* Initial context for force based or torque based measurements */
        cps_sensor_measurement_context_t cfg_measurement_context_type;
        /* Initial instantaneous measurement direction */
        cps_vector_direction_t cfg_vector_direction;
        /* Supported sensor locations */
        const uint8_t *supported_sensor_locations;
        /* Supported sensor locations count */
        uint8_t supported_sensor_locations_count;
        /* Features supported by application */
        cps_feature_t supported_features;
        /* Initial sensor location */
        cps_sensor_location_t init_location;
        /* Initial sampling rate */
        uint8_t sampling_rate;
        /* Initial factory calibration date */
        svc_date_time_t calibration_date;

        /* Initial Cycling Power Control Point request values */
        uint16_t init_crank_length;
        uint16_t init_chain_length;
        uint16_t init_chain_weight;
        uint16_t init_span_length;

        /* Power vector characteristic support */
        bool power_vector_support : 1;
} cps_config_t;

/**
 * \brief Register Cyclic Power Service instance
 *
 * Function registers Cyclic Power Service with give set of features.
 *
 * \param [in] config     service configuration
 * \param [in] cps_config configuration of supported features and sensor location information
 * \param [in] cb         application callbacks
 *
 * \return service instance
 *
 */
ble_service_t *cps_init(const ble_service_config_t *config, const cps_config_t *cps_config,
                                                                        const cps_callbacks_t *cb);
/**
 * \brief Set cumulative value response
 *
 * Function sends confirmation for set_cumulative_value callback.
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          operation status
 *
 * \return true if response sent successfully, otherwise false
 */
bool cps_set_cumulative_value_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status);

/**
 * \brief Update sensor location response
 *
 * Function sends confirmation for update_sensor_location callback.
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          operation status
 *
 * \return true if response sent successfully, otherwise false
 */
bool cps_update_sensor_location_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status);

/**
 * \brief Set crank length value response
 *
 * Function sends confirmation for set_crank_length callback.
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          operation status
 *
 * \return true if response sent successfully, otherwise false
 */
bool cps_set_crank_length_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status);

/**
 * \brief Set span length value response
 *
 * Function sends confirmation for set_span_length callback.
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          operation status
 *
 * \return true if response sent successfully, otherwise false
 */
bool cps_set_span_length_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status);

/**
 * \brief Set chain length value response
 *
 * Function sends confirmation for set_chain_length callback.
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          operation status
 *
 * \return true if response sent successfully, otherwise false
 */
bool cps_set_chain_length_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status);

/**
 * \brief Set chain weight value response
 *
 * Function sends confirmation for set_chain_weight callback.
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          operation status
 *
 * \return true if response sent successfully, otherwise false
 */
bool cps_set_chain_weight_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status);

/**
 * \brief Start offset compensation response
 *
 * Function sends confirmation with offset value for start_offset_compensation callback.
 *
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          operation status
 * \param [in]          offset          offset compensation value
 *
 * \return true if response sent successfully, otherwise false
 */
bool cps_start_offset_compensation_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status,
                                                                                   int16_t offset);
/**
 * \brief Mask cycling power measurement characteristic response
 *
 * Function sends confirmation for mask_cp_measurement_char_content callback.
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          operation status
 *
 * \return true if response sent successfully, otherwise false
 */
bool cps_mask_cp_measurement_char_cfm(ble_service_t *svc, uint16_t conn_idx, cps_status_t status);

/**
 * \brief Send CP measurement
 *
 * Function sends cp measurement to specified client
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          measurement     CP measurement
 *
 * \return true if measurement sent successfully, otherwise false
 */
bool cps_send_cp_measurement(ble_service_t *svc, uint16_t conn_idx,
                                                             const cps_measurement_t *measurement);

/**
 * \brief Send CP vector
 *
 * Function sends cp vector to specified client
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          vector          CP vector
 *
 * \return true if measurement sent successfully, otherwise false
 */
bool cps_send_cp_vector(ble_service_t *svc, uint16_t conn_idx, const cps_power_vector_t *vector);

/**
 * \brief Send CP Power Vector notification enable confirmation
 *
 * Function enables power vector notification if needed and sends write confirmation
 * regarding to connection parameter update status
 *
 * \param [in]          svc             CP service instance
 * \param [in]          conn_idx        connection index
 * \param [in]          status          CP vector write status
 *
 * \return true if response was sent successfully, otherwise false
 */
bool cps_vector_notification_enable_cfm(ble_service_t *svc, uint16_t conn_idx, att_error_t status);

#endif /* CPS_H_ */
