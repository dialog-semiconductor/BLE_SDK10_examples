/**
 ****************************************************************************************
 *
 * @file ble_custom_service.h
 *
 * @brief Bluetooth custom service framework
 *
 * Copyright (c) 2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef BLE_CUSTOM_SERVICE_H_
#define BLE_CUSTOM_SERVICE_H_

#include <stdint.h>
#include <ble_service.h>
#include <string.h>

/************************************ Macro declaration ***************************************/

/* Macro to convert input to text */
#define NUM_TO_STRING(_val)             (#_val)

/*
 * @brief: Characteristic Attribute declaration
 *
 *
 * This macro should be used in application context to declare Characteristic Attributes
 * associated with a Bluetooth Service.
 *
 *
 * \param [in] _uuid:            An 128-bit UUID associated with the target characteristic attribute.
 *                               Use NUM_TO_STRING() to stringify the UUID provided.
 *
 * \param [in] _max_size:        The maximum permitted size for the characteristic attribute.
 *
 * \param [in] _properties:      The GATT properties of the characteristic attribute \p CHAR_GATT_PORP.
 *                               Multiple values should be OR'ed together.
 *
 * \param [in] _permission:      The ATT permissions of the characteristic attribute \p CHAR_ATT_PERM.
 *                               Multiple values should be OR'ed together.
 *
 * \param [in] _user_descriptor: The Characteristic User Description Attribute value.
 *                               Set to 'NULL' to exclude it from the BLE database.
 *                               Use NUM_TO_STRING() to stringify the text provided.
 *
 * \param [in] _read_cb:  User callback function to be called following ATT read requests (for the target characteristic).
 *                        Set it to 'NULL' if no callback is registered.
 *
 * \param [in] _write_cb: User callback function to be called following ATT write requests (for the target characteristic).
 *                        Set it to 'NULL' if no callback is registered.
 *
 * \param [in] _event_cb: User callback function to be called following notification/indication events.
 *                        The underlying mechanism will automatically notify all the connected peer devices
 *                        for the updated value.
 *                        Set it to 'NULL' if no callback is registered.
 *

 * \warning: The UUID should comply with the following format: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX,
 *           otherwise an assertion will hit!!!
 *
 * \warning: If the peer device attempts to write more than the \p _max_size
 *           the application may crash and the Bluetooth connection will be dropped!!!
 *
 *
 * Below is a code snippet that demonstrates the correct usage of this macro:
 *
 * {code}
 *
 *  // Declare Characteristic Attributes
 * const mcs_characteristic_config_t my_service_1[] = {
 *
 *
 *      CHARACTERISTIC_DECLARATION(...)
 *
 *      CHARACTERISTIC_DECLARATION(...)
 *
 *      ....
 *
 *
 * }
 *
 *  // Register Bluetooth Service in Attribute Database
 * SERVICE_DECLARATION(my_service_1, NUM_TO_STRING(XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX))
 *
 * {code}
 *
 */
#define CHARACTERISTIC_DECLARATION(_uuid, _max_size, _properties, _permission, _user_descriptor,            \
                                                                           _read_cb, _write_cb, _event_cb)  \
                                                                                                            \
        {                                                                                                   \
                  .uuid                 = _uuid,                                             \
                  .max_size             = _max_size,                                         \
                  .gatt_prop            = _properties,                                       \
                  .att_perm             = _permission,                                       \
                  .user_descriptor      = _user_descriptor,                                  \
                  .user_descriptor_size = strlen(_user_descriptor),                          \
                  {                                                                          \
                          .get_value    = _read_cb,                                          \
                          .set_value    = _write_cb,                                         \
                          .event_sent   = _event_cb,                                         \
                  }                                                                          \
        }

/*
 * @brief: Bluetooth Service declaration
 *
 * This macro should be used in application context to create/register a Bluetooth Service in BLE attribute database
 *
 * \param [in] _cfg:  An array containing the characteristic ATT declarations \p mcs_characteristic_config_t
 *
 * \param [in] _uuid: An 128-bit UUID associated with the target Bluetooth Service.
 *                    Use NUM_TO_STRING() to stringify the text provided.
 *
 * \warning: The UUID should comply with the following format: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX,
 *           otherwise an assertion will hit!!!
 */
#define SERVICE_DECLARATION(_cfg, _uuid)     \
                        mcs_service_init(_cfg, _uuid, ARRAY_LENGTH(_cfg));

/************************************ Type definitions ***************************************/
typedef att_perm_t    CHAR_ATT_PERM;
typedef gatt_prop_t   CHAR_GATT_PORP;

/* Callback functions */
typedef void (* mcs_get_value_cb_t) (uint8_t **value, uint16_t *length);

typedef void (* mcs_set_value_cb_t) (const uint8_t *value, uint16_t length);

typedef void (*mcs_notification_sent_cb_t) (uint16_t conn_idx, bool status, gatt_event_t type);

/**
 *  Callback functions associated with a characteristic attribute
 */
typedef struct mcs_characteristic_callbacks {
        /*
         * Callback function triggered upon a read request. The developer should provide
         * the requested data to the peer device.
         */
        mcs_get_value_cb_t get_value;

        /*
         * Callback function triggered upon a write request. The developer should get the
         * value sent by the peer device.
         */
        mcs_set_value_cb_t set_value;

        /*
         * Callback function called when a notification/indication is sent to the peer device,
         * when a characteristic attribute value is changed.
         */
        mcs_notification_sent_cb_t event_sent;
} mcs_characteristic_callbacks_t;

/**
 * ATT characteristic configuration structure
 */
typedef struct mcs_characteristic_config {
        const char     *uuid;
        uint16_t       max_size;

        CHAR_GATT_PORP gatt_prop;
        CHAR_ATT_PERM  att_perm;

        /* Characteristic User Descriptor attribute */
        const char     *user_descriptor;
        uint16_t       user_descriptor_size;

        /* Callback functions */
        const mcs_characteristic_callbacks_t cb;
} mcs_characteristic_config_t;
                                                                                                                                             \
/**
 * ATT configuration structure
 */
typedef struct mcs_attributes_config {
        /* Callback functions associated with a characteristic attribute */
        const mcs_characteristic_callbacks_t *cb;

        /* Attribute handles associated with a characteristic attribute */
        uint16_t attr_h;             // Characteristic attribute handle
        uint16_t attr_ccc_h;         // CCC attribute handle
        uint16_t attr_descriptor_h;  // User descriptor attribute handle

        /*
         * Maximum permitted length of Characteristic attribute value.
         */
        uint16_t characteristic_max_size;
} mcs_attributes_config_t;

/**
 * Characteristic notification configuration structure
 */
typedef struct mcs_notif_config {
        /* 128-bit UUID associated with a defined characteristic */
        const char *uuid;

        /* Attribute configuration structure associated with the UUID provided */
        mcs_attributes_config_t *cfg;
} mcs_notif_config_t;

/**
 * Characteristic notification linked list item
 */
typedef struct mcs_notif_list_element {
        /* Notifications settings (list item value) */
        mcs_notif_config_t config;

        /* Next item in the linked list */
        struct mcs_notif_list_element *next;
} mcs_notif_list_element_t;

/**
 * Characteristic attribute linked list item
 */
typedef struct mcs_characteristic_list_element {
        /* Attribute settings (list item value) */
        mcs_attributes_config_t config;

        /* Next item in the linked list */
        struct mcs_characteristic_list_element *next;
} mcs_characteristic_list_element_t;

/**
 * BLE custom service configuration structure
 */
typedef struct mcs_service_config {
        /* Service handle */
        ble_service_t svc;

        /* Pointer to the attributes list head */
        mcs_characteristic_list_element_t *head;

        /* Total number of characteristic attributes  */
        uint8_t num_of_characteristics;
} mcs_service_config_t;

/************************************ API definitions ***************************************/

/*
 * @brief Bluetooth custom service creation.
 *
 * This function initializes and then registers a BLE custom service into the BLE Dialog database
 *
 * \param[in] settings                     An array with all the Characteristic Attribute declarations.
 * \param[in] service_uuid                 An 128-bit UUID associated with the Bluetooth Service
 * \param[in] num_of_characrteristics      The total number of Characteristic Attribute declarations, associated
 *                                         with the Bluetooth Service
 *
 * \return service handle
 *
 * \note It is recommended that user uses \sa SERVICE_DECLARATION() instead of this API
 *
 */
ble_service_t* mcs_service_init(const mcs_characteristic_config_t settings[], const char *service_uuid, uint8_t num_of_characrteristics);

/*
 * @brief Send notification/indication to the peer devices.
 *
 * This function should be used when the value of a characteristic has been changed so all the peer
 * devices are notified.
 *
 * \param[in] uuid                          The 128-bit UUID associated with the target characteristic.
 *                                          Use NUM_TO_STRING() to stringify the UUID provided.
 *
 * \param[in] value                         Pointer to the updated value
 * \param[in] num_of_characrteristics       Number of bytes to be read from \p value
 *
 */
bool mcs_send_notifications(const char *uuid, const uint8_t *value, uint16_t size);

#endif /* BLE_CUSTOM_SERVICE_H_ */
