/**
 ****************************************************************************************
 *
 * @file ble_custom_service_mechanism.h
 *
 * @brief Custom Bluetooth Service mechanism APIs
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


#ifndef SDK_BLE_CUSTOM_SERVICE_MECHANISM_H_
#define SDK_BLE_CUSTOM_SERVICE_MECHANISM_H_

#include <stdint.h>
#include <ble_service.h>


/*
 * @brief: Characteristic Attribute declaration
 *
 *
 * This macro should be used in application context to declare Characteristic Attributes
 * associated with a Bluetooth Service.
 *
 *
 * \param [in] _characteristic_uuid:      An 128-bit UUID associated with the Characteristic Attribute.
 *
 * \param [in] _characteristic_max_size: The maximum permitted size of the Characteristic Attribute value.
 *
 *
 * \param [in] _write_prop:    The write properties of the Characteristic Attribute value (at GATT level).
 *                             Select a value from the CHAR_WRITE_PROP enumeration.
 *
 * \param [in] _read_prop:     The read properties of the Characteristic Attribute value (at GATT level).
 *                             Select a value from the CHAR_READ_PROP enumeration.
 *
 * \param [in] _notifications: Enable/disable Notification/Indication Attribute.
 *                             Select a value from the CHAR_NOTIF enumeration.
 *
 *
 * \param [in] _characteristic_user_descriptor: A value for the Characteristic User Description Attribute value.
 *                                              To discard it set a NULL value.
 *
 *
 * \param [in] _read_cb:  The user can define a callback function which will hit upon a read request from the peer device.
 *                        To discard it, set a NULL value.
 *
 * \param [in] _write_cb: The user can define a callback function which will hit upon a write request from the peer device.
 *                        To discard it, set a NULL value.
 *
 * \param [in] _event_cb: The user can define a callback function which will hit when a Characteristic Attribute value
 *                        is updated. The underlying mechanism will automatically notify all the connected peer devices
 *                        about the updated value. For instance, if three peer devices are connected to the target BLE
 *                        peripheral, and have their notifications enabled, you should expect this callback to hit
 *                        three times.
 *                        To discard it, set a NULL value.
 *

 * \warning: The UUID should comply with the following format: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX,
 *           otherwise an assertion will hit!!!
 *
 * \warning: If the peer device attempts to write more than the maximum allowable Characteristic Attribute value size,
 *           the application may crash and the Bluetooth connection between the master - slave will drop!!!
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
 * SERVICE_DECLARATION(my_service_1, XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX)
 *
 * {code}
 *
 */
#define CHARACTERISTIC_DECLARATION(_characteristic_uuid, _characteristic_max_size, _write_no_resp_prop,     \
                                                                  _write_prop, _read_prop, _notifications,  \
                                            _characteristic_user_descriptor, _read_cb, _write_cb, _event_cb)\
                                                                                                            \
        {                                                                                                   \
                  .characteristic_128_uuid             = #_characteristic_uuid,                             \
                  .characteristic_max_size             = _characteristic_max_size,                          \
                  .characteristic_write_no_response_prop=_write_no_resp_prop,                               \
                  .characteristic_write_prop           = _write_prop,                                       \
                  .characteristic_read_prop            = _read_prop,                                        \
                  .notifications                       = _notifications,                                    \
                  .characteristic_user_descriptor      = #_characteristic_user_descriptor,                  \
                  .characteristic_user_descriptor_size = sizeof(#_characteristic_user_descriptor),          \
                  {                                                                                         \
                          .get_characteristic_value = _read_cb,                                             \
                          .set_characteristic_value = _write_cb,                                            \
                          .event_sent               = _event_cb,                                            \
                  }                                                                                         \
        }



/*
 * @brief: Bluetooth Service declaration
 *
 * This macro should be used in application context to create/register a Bluetooth Service in BLE attribute database
 *
 * \param [in] _service_settings: An array with all the Characteristic Attribute declarations
 *                               (as defined with CHARACTERISTIC_DECLARATION macro).
 *
 * \param [in] _service_uuid: An 128-bit UUID associated with the Bluetooth Service.
 *
 *
 * \warning: The UUID should comply with the following format: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX,
 *           otherwise an assertion will hit!!!
 */
#define SERVICE_DECLARATION(_service_settings, _service_uuid)     \
                        mcs_init( _service_settings, #_service_uuid, (sizeof(_service_settings) / sizeof(_service_settings[0])) );


typedef enum {
         CHAR_WRITE_NO_RESP_PROP_DIS = 0,
         CHAR_WRITE_NO_RESP_PROP_EN  = 1,
} CHAR_WRITE_NO_RESP_PROP;

/*
 * @brief: Characteristic Attribute value write permissions (at GATT level).
 */
typedef enum {
        CHAR_WRITE_PROP_DIS = 0,
        CHAR_WRITE_PROP_EN  = 1,
} CHAR_WRITE_PROP;


/*
 * &brief: Characteristic Attribute value read permissions (at GATT level).
 */
typedef enum {
       CHAR_READ_PROP_DIS = 0,
       CHAR_READ_PROP_EN  = 1,
} CHAR_READ_PROP;


/*
 * @brief: Client Characteristic Configuration.
 *
 * In case of Notifications, the target BLE peripheral device should not expect an ACK (response) from the peer device
 * when a Notification event is sent from the peripheral to the master device.
 *
 * In case of Indications, the target BLE peripheral device should expect an ACK (response) from the peer device
 * when a Notification event is sent from the peripheral to the master device.
 *
 */
typedef enum {
        CHAR_NOTIF_NONE = 0,
        CHAR_NOTIF_NOTIF_EN  = 1,
        CHAR_NOTIF_INDIC_EN  = 2,
} CHAR_NOTIF;



typedef void (* mcs_get_characteristic_value_cb_t) (uint8_t **value, uint16_t *length);

typedef void (* mcs_set_characteristic_value_cb_t) (const uint8_t *value, uint16_t length);

typedef void (*mcs_notification_sent_cb_t) (uint16_t conn_idx, bool status, gatt_event_t type);



/* Callback functions associated with specific BLE events. */
typedef struct mcs_characteristic_cb {

        /*
         * Callback function triggered upon a read request from a peer device.
         * The developer should provide the requested data to the peer device
         * using this callback
         */
        mcs_get_characteristic_value_cb_t get_characteristic_value;

        /*
         * Callback function triggered upon a write request from a peer device.
         * The developer should use this callback to get the value sent by
         * the peer device.
         */
        mcs_set_characteristic_value_cb_t set_characteristic_value;

        /*
         * Callback function which is called when a notification/indication
         * is sent by the peripheral to the peer device, to signify that a
         * Characteristic Attribute value is updated!
         */
        mcs_notification_sent_cb_t event_sent;

} mcs_characteristic_cb_t;



/*
 * Characteristic Attribute configurations
 */
typedef struct mcs_characteristic_config {

        const char     *characteristic_128_uuid;
        uint16_t       characteristic_max_size;

        /* Characteristic permissions (at GATT level) */
        CHAR_WRITE_NO_RESP_PROP         characteristic_write_no_response_prop;
        CHAR_WRITE_PROP                 characteristic_write_prop;
        CHAR_READ_PROP                  characteristic_read_prop;
        CHAR_NOTIF                      notifications;

        /* Characteristic User Descriptor attribute */
        const char     *characteristic_user_descriptor;
        uint16_t       characteristic_user_descriptor_size;

        /* Callback functions */
        const mcs_characteristic_cb_t cb;

} mcs_characteristic_config_t;


                                                                                                                                             \
/*
 * Characteristic Attribute settings
 */
typedef struct mcs_characteristic_structure {

        /* Callback functions */
        const mcs_characteristic_cb_t *cb;

        /*
         * Bluetooth attribute handles
         */
        uint16_t characteristic_h;             // Handle of Characteristic Attribute
        uint16_t characteristic_ccc_h;         // Handle of CCC Attribute
        uint16_t characteristic_descriptor_h;  // Handle of User Descriptor Attribute.

        /*
         * Maximum permitted length of Characteristic attribute value.
         */
        uint16_t characteristic_max_size;

} mcs_characteristic_structure_t;




/*
 * Structure of an element/item in the linked list
 */
typedef struct mcs_characteristic_list_element {

        /* Values unique for each Characteristic attribute */
        mcs_characteristic_structure_t settings;

        /* Pointer for the next element in the list */
        struct mcs_characteristic_list_element *next;

} mcs_characteristic_list_element_t;



/*
 * Structure of a BLE Service handle
 */
typedef struct mcs_service_structure {

        /* Service handle */
        ble_service_t svc;

        /* Pointer to the first element (head) of the linked list */
        mcs_characteristic_list_element_t *p;

        /* The total number of Characteristic attributes  */
        uint8_t num_of_characteristics;

} mcs_service_structure_t;




/*
 * @brief Bluetooth Service creation/registration.
 *
 * This function initializes as well as registers a custom Bluetooth Service in BLE attribute database
 *
 * \param[in] settings                     An array with all the Characteristic Attribute declarations.
 * \param[in] service_uuid                 An 128-bit UUID associated with the Bluetooth Service
 * \param[in] num_of_characrteristics      The total number of Characteristic Attribute declarations, associated
 *                                         with the Bluetooth Service
 *
 * \return service handle
 *
 * \note It is recommend that, the developer should use the SERVICE_DECLARATION() macro instead of this one.
 *
 */
ble_service_t* mcs_init(const mcs_characteristic_config_t settings[], const char *service_uuid, uint8_t num_of_characrteristics);

/*
 * Notify all peers of automatic_change of specific characteristic.
 */
void mcs_auto_notify_all(uint16_t size, const uint8_t* value, uint16_t attr_h, uint16_t ccc_h);

void mcs_auto_notify_peer(uint16_t conn_idx, uint16_t size, const uint8_t *value, uint16_t attr_h, uint16_t ccc_h);

#endif /* SDK_CUSTOM_SERVICE_DEMO_H_ */
