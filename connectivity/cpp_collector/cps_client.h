/**
 ****************************************************************************************
 *
 * @file cps_client.h
 *
 * @brief Cycling Power Service Client API
 *
 * Copyright (c) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef CPS_CLIENT_H_
#define CPS_CLIENT_H_

#include <inttypes.h>
#include "ble_att.h"
#include "ble_client.h"
#include "ble_bufops.h"

/**
 * Cycling Power Feature characteristic bit values
 */
typedef enum {
        /** Pedal Power Balance */
        CPS_CLIENT_FEATURE_PEDAL_POWER_BALANCE_SUPPORT = 0x00001,
        /** Accumulated Torque */
        CPS_CLIENT_FEATURE_ACCUMULATED_TORQUE_SUPPORRT = 0x00002,
        /** Wheel Revolution Data */
        CPS_CLIENT_FEATURE_WHEEL_REVOLUTION_DATA_SUPPORT = 0x00004,
        /** Crank Revolution Data */
        CPS_CLIENT_FEATURE_CRANK_REVOLUTION_DATA_SUPPORT = 0x00008,
        /** Extreme Magnitudes */
        CPS_CLIENT_FEATURE_EXTREME_MAGNITUDE_SUPPORT = 0x00010,
        /** Extreme Angle */
        CPS_CLIENT_FEATURE_EXTREME_ANGLE_SUPPORT = 0x00020,
        /** Top and Bottom Dead Spot Angles */
        CPS_CLIENT_FEATURE_TOP_AND_BOTTOM_DEAD_SPOT_ANGLES_SUPPORT = 0x00040,
        /** Accumulated Energy */
        CPS_CLIENT_FEATURE_ACCUMULATED_ENERGY_SUPPORT = 0x00080,
        /** Offset Compensation Indicator */
        CPS_CLIENT_FEATURE_OFFSET_COMPENSATION_INDICATOR_SUPPORT = 0x00100,
        /** Offset Compensation */
        CPS_CLIENT_FEATURE_OFFSET_COMPENSATION_SUPPORT = 0x00200,
        /** Cyclic Power Measurement Characteristic Content Masking */
        CPS_CLIENT_FEATURE_CHARACTERISTIC_CONTENT_MASKING_SUPPORT = 0x00400,
        /** Multiple Sensor Locations */
        CPS_CLIENT_FEATURE_MULTIPLE_SENSOR_LOCATIONS_SUPPORT = 0x00800,
        /** Crank Length Adjustment */
        CPS_CLIENT_FEATURE_CRANK_LENGTH_ADJUSTMENT_SUPPORT = 0x01000,
        /** Chain Length Adjustment */
        CPS_CLIENT_FEATURE_CHAIN_LENGTH_ADJUSTMENT_SUPPORT = 0x02000,
        /** Chain Weight Adjustment */
        CPS_CLIENT_FEATURE_CHAIN_WEIGHT_ADJUSTMENT_SUPPORT = 0x04000,
        /** Span Length Adjustment */
        CPS_CLIENT_FEATURE_SPAN_LENGTH_ADJUSTMENT_SUPPORT = 0x08000,
        /** Sensor Measurement Context */
        CPS_CLIENT_FEATURE_SENSOR_MEASUREMENT_CONTEXT = 0x10000,
        /** Instantaneous Measurement Direction */
        CPS_CLIENT_FEATURE_INSTANTANEOUS_MEASUREMENT_DIR_SUPPORT = 0x20000,
        /** Factory Calibration Date */
        CPS_CLIENT_FEATURE_FACTORY_CALIBRATION_DATE_SUPPORT = 0x40000,
        /** Enhanced Offset Compensation Supported */
        CPS_CLIENT_FEATURE_ENHANCED_OFFSET_COMPENSATION_SUPPORT = 0x80000,
        /** Distribute System Support */
        CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT = 0x300000,
} cps_client_feature_t;

/**
 * Sensor Measurement Context values
 */
typedef enum {
        /** Unspecified (legacy sensor) */
        CPS_CLIENT_FEATURE_SENSOR_MEASUREMNET_CONTEXT_FORCE_BASED = 0x00000000,
        /** Not for use in a distributed system */
        CPS_CLIENT_FEATURE_SENSOR_MEASUREMNET_CONTEXT_TORQUE_BASED = 0x00010000,
} cps_client_feature_sensor_measurement_context_t;

/**
 * Distribute System Support Feature values
 */
typedef enum {
        /** Unspecified (legacy sensor) */
        CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT_UNSPECIFIED = 0x00000000,
        /** Not for use in a distributed system */
        CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT_NOT_FOR_USE = 0x00100000,
        /** Can be used in a distributed system */
        CPS_CLIENT_FEATURE_DISTRIBUTE_SYSTEM_SUPPORT_CAN_BE_USED = 0x00200000,
} cps_client_feature_distribute_system_support_t;

/**
 * Cycling Power Measurement characteristic flags
 */
typedef enum {
        /** Pedal Power Balance Present */
        CPS_CLIENT_PM_FLAGS_PEDAL_POWER_BALANCE_PRESENT = 0x0001,
        /** Pedal Power Balance Reference */
        CPS_CLIENT_PM_FLAGS_PEDAL_POWER_BALANCE_REFERENCE = 0x0002,
        /** Accumulated Torque Present */
        CPS_CLIENT_PM_FLAGS_ACCUMULATED_TORQUE_PRESENT = 0x0004,
        /** Accumulated Torque Source */
        CPS_CLIENT_PM_FLAGS_ACCUMULATED_TORQUE_SOURCE = 0x0008,
        /** Wheel Revolution Data Present */
        CPS_CLIENT_PM_FLAGS_WHEEL_REVOLUTION_DATA_PRESENT = 0x0010,
        /** Crank Revolution Data Present */
        CPS_CLIENT_PM_FLAGS_CRANK_REVOLUTION_DATA_PRESENT = 0x0020,
        /** Extreme Force Magnitudes Present */
        CPS_CLIENT_PM_FLAGS_EXTREME_FORCE_MAGNITUDES_PRESENT = 0x0040,
        /** Extreme Torque Magnitudes Present */
        CPS_CLIENT_PM_FLAGS_EXTREME_TORQUE_MAGNITUDES_PRESENT = 0x0080,
        /** Extreme Angles Present */
        CPS_CLIENT_PM_FLAGS_EXTREME_ANGLES_PRESENT = 0x0100,
        /** Top Dead Spot Angle Present */
        CPS_CLIENT_PM_FLAGS_TOP_DEAD_SPOT_ANGLE_PRESENT = 0x0200,
        /** Bottom Dead Spot Angle Present */
        CPS_CLIENT_PM_FLAGS_BOTTOM_DEAD_SPOT_ANGLE_PRESENT = 0x0400,
        /** Accumulated Energy Present */
        CPS_CLIENT_PM_FLAGS_ACCUMULATED_ENERGY_PRESENT = 0x0800,
        /** Offset Compensation Indicator */
        CPS_CLIENT_PM_FLAGS_OFFSET_COMPENSATION_INDICATOR = 0x1000,
} cps_measurement_flags_t;

/**
 * Cycling Power Measurement pedal power balance reference
 */
typedef enum {
        /** Pedal power balance unknown */
        CPS_CLIENT_PEDAL_POWER_BALANCE_REFERENCE_UNKNOWN = 0x00,
        /** Pedal power balance left */
        CPS_CLIENT_PEDAL_POWER_BALANCE_REFERENCE_LEFT = 0x01,
} cps_client_pedal_power_balance_reference_t;

/**
 * Cycling Power Measurement accumulated torque source
 */
typedef enum {
        /** Accumulated torque source wheel based */
        CPS_CLIENT_ACCUMULATED_TORQUE_SOURCE_WHEEL_BASED = 0x00,
        /** Accumulated torque source crank based */
        CPS_CLIENT_ACCUMULATED_TORQUE_SOURCE_CRANK_BASED = 0x01,
} cps_client_accumulated_torque_source_t;

/**
 * Sensor locations
 */
typedef enum {
        /** Other */
        CPS_CLIENT_SENSOR_LOCATION_OTHER = 0x00,
        /** Top of shoe */
        CPS_CLIENT_SENSOR_LOCATION_TOP_OF_SHOE = 0x01,
        /** In shoe */
        CPS_CLIENT_SENSOR_LOCATION_IN_SHOE = 0x02,
        /** Hip */
        CPS_CLIENT_SENSOR_LOCATION_HIP = 0x03,
        /** Front Wheel */
        CPS_CLIENT_SENSOR_LOCATION_FRONT_WHEEL = 0x04,
        /** Left Crank */
        CPS_CLIENT_SENSOR_LOCATION_LEFT_CRANK = 0x05,
        /** Right Crank */
        CPS_CLIENT_SENSOR_LOCATION_RIGHT_CRANK = 0x06,
        /** Left Pedal */
        CPS_CLIENT_SENSOR_LOCATION_LEFT_PEDAL = 0x07,
        /** Right Pedal */
        CPS_CLIENT_SENSOR_LOCATION_RIGHT_PEDAL = 0x08,
        /** Front Hub */
        CPS_CLIENT_SENSOR_LOCATION_FRONT_HUB = 0x09,
        /** Rear Dropout */
        CPS_CLIENT_SENSOR_LOCATION_REAR_DROPOUT = 0x0a,
        /** Chainstay */
        CPS_CLIENT_SENSOR_LOCATION_CHAINSTAY = 0x0b,
        /** Rear Wheel */
        CPS_CLIENT_SENSOR_LOCATION_REAR_WHEEL = 0x0c,
        /** Rear Hub */
        CPS_CLIENT_SENSOR_LOCATION_REAR_HUB = 0x0d,
        /** Chest */
        CPS_CLIENT_SENSOR_LOCATION_CHEST = 0x0e,
} cps_client_sensor_location_t;

/**
 * Status
 */
typedef enum {
        /** Status success */
        CPS_CLIENT_STATUS_SUCCESS = 0x01,
        /** Status opcode not supported */
        CPS_CLIENT_STATUS_OPCODE_NOT_SUPPORTED = 0x02,
        /** Status invalid parameters */
        CPS_CLIENT_STATUS_INVALID_PARAM = 0x03,
        /** Status operation failed */
        CPS_CLIENT_STATUS_OPERATION_FAILED = 0x04,
        /** Procedure Already in Progress */
        CPS_CLIENT_STATUS_PROC_ALREADY_IN_PROGRESS = 0x80,
        /** CCC descriptor improperly configured */
        CPS_CLIENT_STATUS_CCC_IMPROPERLY_CONFIGURED = 0x81,
        /** Timeout - response from CP Control Point was not sent in proper time */
        CPS_CLIENT_STATUS_TIMEOUT = 0xFE,
        /** Write to CP Control Point failed */
        CPS_CLIENT_STATUS_WRITE_FAILED = 0xFF
} cps_client_status_t;

/**
 * CP Client event
 */
typedef enum {
        /** CP Client measurements notification */
        CPS_CLIENT_EVENT_MEASUREMENT_NOTIF = 0x01,
        /** CP Client measurements broadcast */
        CPS_CLIENT_EVENT_MEASUREMENT_BROADCAST = 0x02,
        /** CP Client power vector notification */
        CPS_CLIENT_EVENT_CP_VECTOR_NOTIF = 0x03,
} cps_client_event_t;

/**
 * CP Measurements
 *
 * It corresponds to Cycling Power Measurement characteristic value
 */
typedef struct {
        /** Pedal Power Balance Present */
        bool pedal_power_balance_present        : 1;
        /** Accumulated Torque Present */
        bool accumulated_torque_present         : 1;
        /** Wheel Revolution Data Present */
        bool wheel_revolution_data_present      : 1;
        /** Crank Revolution Data Present */
        bool crank_revolution_data_present      : 1;
        /** Extreme Force Magnitudes Present */
        bool extreme_force_magnitude_present    : 1;
        /** Extreme Torque Magnitudes Present */
        bool extreme_torque_magnitude_present   : 1;
        /** Extreme Angles Present */
        bool extreme_angle_present              : 1;
        /** Top Dead Spot Angle Present */
        bool top_dead_spot_angle_present        : 1;
        /** Bottom Dead Spot Angle Present */
        bool bottom_dead_spot_angle_present     : 1;
        /** Accumulated Energy Present */
        bool accumulated_energy_present         : 1;
        /** Offset Compensation Indicator */
        bool offset_compensation_indicator      : 1;
        /** Instantaneous Power */
        int16_t instant_power;
        /** Pedal Power balance */
        uint8_t pedal_power_balance;
        /** Pedal Power balance reference */
        cps_client_pedal_power_balance_reference_t pedal_power_balance_reference;
        /** Accumulated Torque */
        uint16_t accumulated_torque;
        /** Accumulated Torque source */
        cps_client_accumulated_torque_source_t accumulated_torque_source;
        /** Wheel Revolution Data - Cumulative Wheel Revolutions */
        uint32_t wrd_cumulative_revol;
        /** Wheel Revolution Data - Last Wheel Event Time */
        uint16_t wrd_last_wheel_evt_time;
        /** Crank Revolution Data - Cumulative Crank Revolutions */
        uint16_t crd_cumulative_revol;
        /** Crank Revolution Data - Last Crank Event Time */
        uint16_t crd_last_crank_evt_time;
        /** Extreme Force Magnitude - Maximum Force Magnitude */
        int16_t efm_max_force_magnitude;
        /** Extreme Force Magnitude - Minimum Force Magnitude */
        int16_t efm_min_force_magnitude;
        /** Extreme Torque Magnitude - Maximum Torque Magnitude */
        int16_t etm_max_torq_magnitude;
        /** Extreme Torque Magnitude - Minimum Torque Magnitude */
        int16_t etm_min_torq_magnitude;
        /** Extreme Angle - Maximum Angle */
        uint16_t ea_maximum_angle;
        /** Extreme Angle - Minimum Angle */
        uint16_t ea_minimum_angle;
        /** Top Dead Spot Angle */
        uint16_t top_dead_spot_angle;
        /** Bottom Dead Spot Angle */
        uint16_t bottom_dead_spot_angle;
        /** Accumulated Energy */
        uint16_t accumulated_energy;
} cps_client_measurement_t;

/**
 * Cycling Power Vector Direction
 */
typedef enum {
        /** Direction Unknown */
        CPS_CLIENT_CYCLING_POWER_VECTOR_DIRECTION_UNKNOWN = 0x00,
        /** Direction Tangential Component */
        CPS_CLIENT_CYCLING_POWER_VECTOR_DIRECTION_TANGENTIAL_COMPONENT = 0x01,
        /** Direction Radial Component */
        CPS_CLIENT_CYCLING_POWER_VECTOR_DIRECTION_RADIAL_COMPONENT = 0x02,
        /** Direction Lateral Component */
        CPS_CLIENT_CYCLING_POWER_VECTOR_DIRECTION_LATERAL_COMPONENT = 0x03,
} cps_client_cycling_power_vector_direction_t;

/**
 * Cycling Power Vector
 *
 * It corresponds to CP Power Vector characteristic raw data (either force or torque)
 */
typedef struct {
        /** Crank Revolution Data Present */
        uint8_t crank_revolution_data_present                   : 1;
        /** First Crank Measurement Angle Present */
        uint8_t first_crank_measurement_angle_present           : 1;
        /* Instantaneous Force Magnitude Array Present */
        uint8_t instantaneous_force_magnitude_array_present     : 1;
        /* Instantaneous Torque Magnitude Array Present */
        uint8_t instantaneous_torque_magnitude_array_present    : 1;
        /* Cycling Power Vector Direction */
        uint8_t instantaneous_measurement_direction             : 2;
        /** Crank Revolution Data - Cumulative Crank Revolution */
        uint16_t cumulative_crank_revolutions;
        /** Crank Revolution Data - Last Crank Event Time */
        uint16_t last_crank_event_time;
        /** First Crank Measurement Angle */
        uint16_t first_crank_measurement_angle;
        /**
         * Instantaneous Magnitude Length.
         * If length is zero then force or torque measurements aren't present
         */
        uint8_t instantaneous_magnitude_len;
        /**
         * Instantaneous Magnitude Array for force or torque measurements
         * depending on measurements context setup
         */
        int16_t instantaneous_magnitude_arr[0];
} cps_client_power_vector_t;

/**
 * Date time
 */
typedef struct {
        uint16_t year;          /**< 1582 .. 9999, 0 - unknown */
        uint8_t  month;         /**< 1..12, 0 - unknown */
        uint8_t  day;           /**< 1..31, 0 - unknown */
        uint8_t  hours;         /**< 0..23 */
        uint8_t  minutes;       /**< 0..59 */
        uint8_t  seconds;       /**< 0..59 */
} date_time_t;

/**
 * Supported Capabilities bit mask
 */
typedef enum {
        /** Cycling Power Vector Characteristic*/
        CPS_CLIENT_CAP_POWER_VECTOR = 0x01,
        /** Cycling Power Control Point Characteristic */
        CPS_CLIENT_CAP_CYCLING_POWER_CONTROL_POINT = 0x02,
        /** Cycling Power Measurement Broadcast Capability */
        CPS_CLIENT_CAP_CYCLING_POWER_MEASUREMENT_BROADCAST = 0x04,
} cps_client_cap_t;

/**
 * CPS Client application callbacks
 */
typedef struct {
        /**
        * \brief read features completed callback
        *
        * \param [in] cps_client        client instance
        * \param [in] status            operation status
        * \param [in] features          support features
        *
        */
        void (* read_feature_completed) (ble_client_t *cps_client, att_error_t status,
                                                                cps_client_feature_t features);
        /**
        * \brief Set event state completed callback
        *
        * Called after successful operation of cps_client_set_event_state callback
        *
        * \param [in] cps_client        client instance
        * \param [in] status            operation status
        * \param [in] event             client event
        *
        */
        void (* set_event_state_completed) (ble_client_t *client, att_error_t status,
                                                                cps_client_event_t event);
        /**
        * \brief Get event state completed callback
        *
        * Called after successful operation of cps_client_get_event_state callback
        *
        * \param [in] cps_client        client instance
        * \param [in] status            operation status
        * \param [in] event             client event
        * \param [in] enabled           event state
        *
        */
        void (* get_event_state_completed) (ble_client_t *client, att_error_t status,
                                                cps_client_event_t event, bool enabled);

        /**
        * \brief Set cycling power control point state completed callback
        *
        * Called after successful operation of cps_client_set_event_state callback
        *
        * \param [in] cps_client        client instance
        * \param [in] status            operation status
        *
        */
        void (* set_cycling_power_cp_state_completed) (ble_client_t *client,
                                                                        att_error_t status);

        /**
        * \brief Get cycling power control point state completed callback
        *
        * Called after successful operation of cps_client_set_event_state callback
        *
        * \param [in] cps_client        client instance
        * \param [in] status            operation status
        * \param [in] enabled           control point state
        *
        */
        void (* get_cycling_power_cp_state_completed_cb_t) (ble_client_t *client,
                                                                        att_error_t status,
                                                                        bool enabled);

        /**
         * \brief Measurement notification callback
         *
         * Called when client received measurement notification from server.
         *
         * \param [in] client           ble_client instance
         * \param [in] measurement      CP measurement instance
         *
         */
        void (* measurement_notif) (ble_client_t *client,
                                                const cps_client_measurement_t *measurement);

        /**
         * \brief Power Vector notification callback
         *
         * Called when client received power vector notification from server.
         *
         * \param [in] client           ble_client instance
         * \param [in] vector           CP power vector instance
         *
         */
        void (* vector_notif) (ble_client_t *client, const cps_client_power_vector_t *vector);

        /**
         * \brief Update sensor location procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         *
         */
        void (* update_sensor_location_completed) (ble_client_t *client,
                                                                cps_client_status_t status);

        /**
         * \brief Request supported sensor location procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         * \param [in] locations_count  supported sensor locations array's length
         * \param [in] locations        supported sensor locations array
         *
         */
        void (* request_supported_sensor_locations_completed) (ble_client_t *client,
                                                                cps_client_status_t status,
                                                                uint8_t locations_count,
                                                                const cps_client_sensor_location_t *locations);

        /**
         * \brief Read sensor location completed callback
         *
         * Called when client received read completed event from server's Sensor location characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           read operation status
         * \param [in] location         current sensor location
         *
         */
        void (* read_sensor_location_completed) (ble_client_t *client, att_error_t status,
                                                        cps_client_sensor_location_t location);

        /**
         * \brief Set cumulative value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         *
         */
        void (* set_cumulative_value_completed) (ble_client_t *client, cps_client_status_t status);

        /**
         * \brief Set crank length value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         *
         */
        void (* set_crank_length_completed) (ble_client_t *client,
                                                                cps_client_status_t status);
        /**
         * \brief Request crank length value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         * \param [in] crank_length     crank length value
         *
         */
        void (* request_crank_length_completed) (ble_client_t *client, cps_client_status_t status,
                                                                        uint16_t crank_length);

        /**
         * \brief Set chain length value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         *
         */
        void (* set_chain_length_completed) (ble_client_t *client, cps_client_status_t status);

        /**
         * \brief Request chain length value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         * \param [in] chain_length     chain length value
         *
         */
        void (* request_chain_length_completed) (ble_client_t *client, cps_client_status_t status,
                                                                        uint16_t chain_length);

        /**
         * \brief Set chain weight value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         *
         */
        void (* set_chain_weight_completed) (ble_client_t *client, cps_client_status_t status);

        /**
         * \brief Request chain weight value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         * \param [in] chain_weight     chain weight value
         *
         */
        void (* request_chain_weight_completed) (ble_client_t *client, cps_client_status_t status,
                                                                        uint16_t chain_weight);

        /**
         * \brief Request span length value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         * \param [in] span_length      span length value
         *
         */
        void (* request_span_length_completed) (ble_client_t *client, cps_client_status_t status,
                                                                         uint16_t span_length);

        /**
         * \brief Set span length value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         *
         */
        void (* set_span_length_completed) (ble_client_t *client, cps_client_status_t status);

        /**
         * \brief Start offset compensation value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         * \param [in] offset           offset compensation value
         *
         */
        void (* start_offset_compensation_completed) (ble_client_t *client,
                                                                       cps_client_status_t status,
                                                                       int16_t offset);

        /**
         * \brief Request sampling rate procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         * \param [in] sampling_rate    sapmling rate value
         *
         */
        void (* request_sampling_rate_completed) (ble_client_t *client, cps_client_status_t status,
                                                                        uint8_t sampling_rate);

        /**
         * \brief Request factory calibration date procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         * \param [in] calib_date       calibration date value
         *
         */
        void (* request_calibration_date_completed_cb) (ble_client_t *client,
                                                               cps_client_status_t status,
                                                               const date_time_t *calib_date);

        /**
         * \brief Set control point measurement mask value procedure completed callback
         *
         * Called when client received indication from server's CP Control Point characteristic.
         *
         * \param [in] client           ble_client instance
         * \param [in] status           operation status
         *
         */
        void (* set_mask_cp_measurement_completed) (ble_client_t *client,
                                                               cps_client_status_t status);

        /**
         * \brief Start timer callback
         *
         * Called when application should start timer.
         *
         * \param [in] client           ble_client instance
         * \param [in] ms               time in milliseconds
         *
         */
        void (* start_timer) (ble_client_t *client, uint32_t ms);

        /**
         * \brief Cancel timer callback
         *
         * Called when application should stop timer.
         *
         * \param [in] client           ble_client instance
         *
         */
        void (* cancel_timer) (ble_client_t *client);
} cps_client_callbacks_t;

/**
 * \brief Register Cycling Power Service Client instance
 *
 * Function initializes Cycling Power Service Client.
 *
 * \param [in] cb               application callbacks
 * \param [in] evt              browse svc event with CP svc details
 *
 * \return client instance
 */
ble_client_t *cps_client_init(const cps_client_callbacks_t *cb,
                                                            const ble_evt_gattc_browse_svc_t *evt);

/**
 * \brief Get CP features
 *
 * Function reads mandatory CP Feature characteristic.
 * After successful operation get_feature callback will be called.
 *
 * \param [in] client           client instance
 *
 * \return true if read request has been sent successfully, otherwise false
 */
bool cps_client_read_cycling_power_feature(ble_client_t *client);

/**
 * \brief Read sensor location
 *
 * Function sends current sensor location read request.
 *
 * \param [in] client          ble_client instance
 *
 * \return true if read request has been sent successfully, otherwise false
 */
bool cps_client_read_sensor_location(ble_client_t *client);

/**
 * \brief Get capabilities
 *
 * Function returns supported optional capabilities enum.
 *
 * \param [in] client           client instance
 *
 * \return Bit mask with supported capabilities
 */
cps_client_cap_t cps_client_get_capabilities(ble_client_t *client);

/**
 * \brief CPS client set event characteristic state
 *
 * Function writes CCC descriptor of desired event. After successful operation
 * cps_client_set_event_state_completed callback will be called
 *
 * \param [in] client           client instance
 * \param [in] event            client event
 * \param [in] enable           state flag
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_set_event_state(ble_client_t *client, cps_client_event_t event, bool enable);

/**
 * \brief CPS client get event characteristic state
 *
 * Function reads CCC and SCC descriptors of desired event. After successful operation
 * cps_client_get_event_state_completed callback will be called
 *
 * \param [in] client           client instance
 * \param [in] event            client event
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_get_event_state(ble_client_t *client, cps_client_event_t event);

/**
 * \brief Set control point state
 *
 * Function writes CCC descriptor of Cycling Power Control Point characteristic. After successful
 * operation set_cycling_power_cp_state_completed callback will be called.
 *
 * \param [in] client           client instance
 * \param [in] enable           state flag
 *
 * \return true if write request has been sent successfully, false otherwise.
 *
 */
bool cps_client_set_cycling_power_cp_state(ble_client_t *client, bool enable);

/**
 * \brief Get control point state
 *
 * Function reads CCC descriptor of Cycling Power Control Point characteristic. After successful
 * operation get_cycling_power_cp_state_completed callback will be called.
 *
 * \param [in] client           client instance
 *
 * \return true if read request has been sent successfully, false otherwise
 *
 */
bool cps_client_get_cycling_power_cp_state(ble_client_t *client);

/**
 * \brief Set cumulative value request
 *
 * Function sends request for set cumulative value operation.
 *
 * \param [in] client          ble_client instance
 * \param [in] value           new cumulative value
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_set_cumulative_value(ble_client_t *client, uint32_t value);

/**
 * \brief Update sensor location request
 *
 * Function sends request for update sensor location operation.
 *
 * \param [in] client          ble_client instance
 * \param [in] location        new sensor location
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_update_sensor_location(ble_client_t *client, cps_client_sensor_location_t location);

/**
 * \brief Request supported sensor locations request
 *
 * Function sends request for get supported sensor location operation.
 *
 * \param [in] client          ble_client instance
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_request_supported_sensor_locations(ble_client_t *client);


/**
 * \brief Set crank length value request
 *
 * Function sends request for set crank length value operation.
 *
 * \param [in] client           ble_client instance
 * \param [in] value            new crank length value
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_set_crank_length(ble_client_t *client, uint16_t value);

/** \brief Request crank length value request
 *
 * Function sends request for get crank length value operation.
 *
 * \param [in] client           ble_client instance
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_request_crank_length(ble_client_t *client);

/**
 * \brief Set chain length value request
 *
 * Function sends request for set chain length value operation.
 *
 * \param [in] client           ble_client instance
 * \param [in] value            new chain length value
 *
 * \return true if request sent successfully, otherwise false
 */
bool cps_client_set_chain_length(ble_client_t *client, uint16_t value);

/** \brief Request chain length value request
 *
 * Function sends request for get chain length value operation.
 *
 * \param [in] client           ble_client instance
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_request_chain_length(ble_client_t *client);

/**
 * \brief Set chain weight value request
 *
 * Function sends request for set chain weight value operation.
 *
 * \param [in] client           ble_client instance
 * \param [in] value            new chain weight value
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_set_chain_weight(ble_client_t *client, uint16_t value);

/** \brief Request chain weight value request
 *
 * Function sends request for get chain weight value operation.
 *
 * \param [in] client           ble_client instance
 *
 * \return true if request sent successfully, otherwise false
 */
bool cps_client_request_chain_weight(ble_client_t *client);

/**
 * \brief Set span length value request
 *
 * Function sends request for set span length value operation.
 *
 * \param [in] client           ble_client instance
 * \param [in] value            new chain weight value
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_set_span_length(ble_client_t *client, uint16_t value);

/** \brief Request span length value request
 *
 * Function sends request for get span length value operation.
 *
 * \param [in] client           ble_client instance
 *
 * \return true if request sent successfully, otherwise false
 */
bool cps_client_request_span_length(ble_client_t *client);

/**
 * \brief Start offset compensation value request
 *
 * Function sends request for start offset compensation value operation.
 *
 * \param [in] client           ble_client instanc
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_start_offset_compensation(ble_client_t *client);

/**
 * \brief Set measurements mask value request
 *
 * Function sends request for set measurement mask value operation.
 *
 * \param [in] client           ble_client instance
 * \param [in] value            new mask value
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_mask_cycling_power_measurement_characteristic_content(ble_client_t *client,
                                                                                uint16_t value);


/** \brief Request sampling rate value request
 *
 * Function sends request for get sampling rate value operation.
 *
 * \param [in] client           ble_client instance
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_request_sampling_rate(ble_client_t *client);

/** \brief Request factory calibration date value request
 *
 * Function sends request for get factory calibration date value operation.
 *
 * \param [in] client           ble_client instance
 *
 * \return true if request has been sent successfully, otherwise false
 */
bool cps_client_request_factory_calibration_date(ble_client_t *client);

/**
 * \brief Application's timer notification
 *
 * Function should be used when Control Point response timer in application ended countdown.
 *
 * \param [in] client          ble_client instance
 *
 */
void cps_client_timer_notif(ble_client_t *client);

/**
 * \brief Initialize CPP Client instance from buffered (cached) data and register application
 * callbacks
 *
 * Function semantics is very similar to cpp_client_init() but internal data is initialized by
 * buffered context and the client is automatically added to active clients collection.
 *
 * \param [in] conn_idx         connection index
 * \param [in] cb               application callbacks
 * \param [in] data             buffered data
 * \param [in] length           buffer's length
 *
 * \return client instance when initialized properly, NULL otherwise
 */
ble_client_t *cps_client_init_from_data(uint16_t conn_idx, const cps_client_callbacks_t *cb,
                                                                   const void *data, size_t length);

#endif /* CPS_CLIENT_H_ */
