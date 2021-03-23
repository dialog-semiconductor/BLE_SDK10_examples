/**
 ****************************************************************************************
 *
 * @file ble_custom_service_mechanism.c
 *
 * @brief Custom Bluetooth Service mechanism implementation
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


#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "osal.h"
#include "ble_att.h"
#include "ble_bufops.h"
#include "ble_common.h"
#include "ble_gatt.h"
#include "ble_gatts.h"
#include "ble_storage.h"
#include "ble_uuid.h"

#include "ble_custom_service.h"


#define UUID_GATT_CLIENT_CHAR_CONFIGURATION (0x2902)

/* Function prototypes */
mcs_characteristic_structure_t* mcs_characteristic_list_select_element(mcs_characteristic_list_element_t *head,
                                                                        uint8_t list_item);
/*
 * Write handler intended for servicing write requests to characteristic attribute value.
 */
static att_error_t do_char_value_write(ble_service_t *svc, mcs_characteristic_structure_t *attr,
                                                                const ble_evt_gatts_write_req_t *evt)
{

        if (evt->offset) {
                return ATT_ERROR_ATTRIBUTE_NOT_LONG;
        }

        if (evt->length > attr->characteristic_max_size) {
                return ATT_ERROR_INVALID_VALUE_LENGTH;
        }


        /*
         * Check whether developer has defined a function for handling
         * characteristic write request.
         */
        if (!attr->cb || !attr->cb->set_characteristic_value) {
                return ATT_ERROR_WRITE_NOT_PERMITTED;
        }


        /*
         * Switch to application context to update the characteristic value (as requested by the peer device).
         */
        attr->cb->set_characteristic_value(evt->value, evt->length);


        /*
         * Respond to [BLE_EVT_GATTS_WRITE_REQ] event.
         */
        ble_gatts_write_cfm(evt->conn_idx, attr->characteristic_h, ATT_ERROR_OK);


        /*
         * Notify all the connected peers, and given that they have their notifications enabled,
         * that characteristic's value has been changed!
         */
        mcs_auto_notify_all(evt->length, evt->value, attr->characteristic_h, attr->characteristic_ccc_h);

        return ATT_ERROR_OK;

}


/*
 * Write handler intended for servicing write requests to "Client Characteristic Configuration" (CCC) attribute value.
 */
static att_error_t do_char_value_ccc_write(mcs_characteristic_structure_t *attr, const ble_evt_gatts_write_req_t *evt)
{
        uint16_t ccc = GATT_CCC_NONE;

        if (evt->offset) {
                return ATT_ERROR_ATTRIBUTE_NOT_LONG;
        }

        if (evt->length != sizeof(ccc)) {
                return ATT_ERROR_INVALID_VALUE_LENGTH;
        }

        ccc = get_u16(evt->value);

        /*
         * Store the envoy CCC value in Flash memory.
         */
        ble_storage_put_u32(evt->conn_idx, attr->characteristic_ccc_h, (uint32_t)ccc, true);

        ble_gatts_write_cfm(evt->conn_idx, attr->characteristic_ccc_h, ATT_ERROR_OK);

        return ATT_ERROR_OK;
}



/*
 * Read handler intended for servicing read requests to characteristic attribute value.
 */
static void do_char_value_read(ble_service_t *svc, mcs_characteristic_structure_t *attr, const ble_evt_gatts_read_req_t *evt)
{
        /* Data initialized by the user! */
        uint16_t length  = 0;
        uint8_t  *value  = NULL;

        /*
         * Check whether developer has defined a function for handling
         * characteristic read requests.
         */
        if (!attr->cb || !attr->cb->get_characteristic_value) {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
                return;
        }

        /*
         * Switch to application context to get the characteristic value (as requested by the peer device).
         */
        attr->cb->get_characteristic_value(&value, &length);


        /* Response for a [BLE_EVT_GATTS_READ_REQ] BLE event. */
        ble_gatts_read_cfm(evt->conn_idx, attr->characteristic_h, ATT_ERROR_OK, length, (const void *)value);

}

/*
 *  Notify all the connected peers that characteristic's value has been changed.
 */
void mcs_auto_notify_all(uint16_t size, const uint8_t* value, uint16_t attr_h, uint16_t ccc_h)
{

        /*
         * Get the number of connected devices
         */
        uint8_t num_conn;
        uint16_t* conn_idx;

        ble_gap_get_connected(&num_conn, &conn_idx);

        /*
         * For each connected peer device
         */
        while(num_conn--)
        {
                mcs_auto_notify_peer(conn_idx[num_conn], size, value, attr_h, ccc_h);
        }

        if(conn_idx)
        {
                OS_FREE(conn_idx);
        }

}

/*
 * Notify peer at the other end of connection of index conn_idx that a change has occurred on the characteristic.
 */
void mcs_auto_notify_peer(uint16_t conn_idx, uint16_t size, const uint8_t *value, uint16_t attr_h, uint16_t ccc_h)
{
        uint16_t ccc = GATT_CCC_NONE;

        /*
         * Get the Client Configuration Chracteristic (CCC) value stored in flash memory.
         */
        ble_storage_get_u16(conn_idx, ccc_h, &ccc);

        /*
         * Check whether notifications/indications are enabled for this particular peer with conn_idx
         * and send the corresponding event.
         */
        if(ccc & GATT_CCC_NOTIFICATIONS)
        {
                /*
                 * Send a notification to the peer device with the altered value.
                 */
                ble_gatts_send_event(conn_idx, attr_h, GATT_EVENT_NOTIFICATION, size, (const void*)value);
                return;
        }
        else if(ccc & GATT_CCC_INDICATIONS)
        {
                /*
                 * Send a notification to the peer device with the altered value.
                 */
                ble_gatts_send_event(conn_idx, attr_h, GATT_EVENT_INDICATION, size, (const void*)value);
                return;
        }

}


/*
 * Callback function to be called upon [BLE_EVT_GATTS_EVENT_SENT] BLE event.
 */
static void handle_event_sent_evt(ble_service_t *svc, const ble_evt_gatts_event_sent_t *evt)
{
        mcs_service_structure_t *hdr = (mcs_service_structure_t *) svc;

        for (int idx = 0; (idx < hdr->num_of_characteristics); idx++) {

                mcs_characteristic_structure_t *attr = mcs_characteristic_list_select_element(hdr->p, idx);

                if (evt->handle == attr->characteristic_h) {
                        if (attr->cb->event_sent) {
                                attr->cb->event_sent(evt->conn_idx, evt->status, evt->type);
                        }
                        return;
                }
        }
}



/*
 * Callback function to be called upon [BLE_EVT_GATTS_READ_REQ] BLE event.
 */
static void handle_read_req(ble_service_t *svc, const ble_evt_gatts_read_req_t *evt)
{
        mcs_service_structure_t *hdr = (mcs_service_structure_t *) svc;

        /*
         * Start transacting the characteristic list until the requested attribute is reached.
         * Then, exit from callback (there is no need to scan the rest items of the list)
         */
        for (int idx = 0; (idx < hdr->num_of_characteristics); idx++) {

                mcs_characteristic_structure_t *attr = mcs_characteristic_list_select_element(hdr->p, idx);

                // Check if the requested attribute is a valid attribute that can be handled
                if (evt->handle == attr->characteristic_h) {
                        do_char_value_read(svc, attr, evt);
                        return;
                } else if (evt->handle == attr->characteristic_ccc_h) {  // A request to read the descriptor of the Characteristic
                        uint16_t ccc = 0x0000;

                        /* Extract the CCC value from the BLE storage */
                        ble_storage_get_u16(evt->conn_idx, attr->characteristic_ccc_h, &ccc);

                        // We're little-endian - OK to write directly from uint16_t
                        ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_OK, sizeof(ccc), (const void *)&ccc);
                        return;
                }
        }

        // This line will be reached if an invalid attribute has been requested
        ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);

}


/*
 * Callback function to be called upon [BLE_EVT_GATTS_WRITE_REQ] BLE event.
 */
static void handle_write_req(ble_service_t *svc, const ble_evt_gatts_write_req_t *evt)
{
        mcs_service_structure_t *hdr = (mcs_service_structure_t *) svc;  // We know that after service handle dwells the pointer that points to the first element in the list.
        att_error_t status = ATT_ERROR_WRITE_NOT_PERMITTED;

        /*
         * Start traversing the characteristic list until the requested attribute is reached.
         */
        for (int idx = 0; (idx < hdr->num_of_characteristics); idx++) {

                mcs_characteristic_structure_t *attr = mcs_characteristic_list_select_element(hdr->p, idx);

                // Check if the requested attribute is a valid attribute that can be handled
                if (evt->handle == attr->characteristic_h)
                {
                        status = do_char_value_write(svc, attr, evt);
                        goto done;
                } else if (evt->handle == attr->characteristic_ccc_h)
                {
                        status = do_char_value_ccc_write(attr, evt);
                        goto done;
                }

        }


done:



        if (status == ATT_ERROR_OK)
        {
                

                // Write handler executed properly
                return;
        }

        /*
         * This code line will be reached if an invalid attribute has been requested.
         */
        ble_gatts_write_cfm(evt->conn_idx, evt->handle, status);

}


/*
 * Callback function to be called upon [BLE_EVT_GATTS_PREPARE_WRITE_REQ] BLE event.
 */
static void handle_prepare_write_req(ble_service_t *svc, const ble_evt_gatts_prepare_write_req_t *evt)
{
        mcs_service_structure_t *hdr = (mcs_service_structure_t *) svc;

        for (int idx = 0; idx < hdr->num_of_characteristics; idx++) {
                mcs_characteristic_structure_t *attr =
                        mcs_characteristic_list_select_element(hdr->p, idx);

                if (evt->handle == attr->characteristic_h) {
                        /* Response for the prepare write request */
                        ble_gatts_prepare_write_cfm(evt->conn_idx, evt->handle, attr->characteristic_max_size, ATT_ERROR_OK);
                        return;
                }
        }
}


/*
 * Callback function to be called after a Cleanup BLE event.
 */
static void cleanup(ble_service_t *svc)
{
        mcs_service_structure_t *hdr = (mcs_service_structure_t *) svc;

        for (int i = 0; (i < hdr->num_of_characteristics); i++) {

                mcs_characteristic_structure_t *list_item = mcs_characteristic_list_select_element(hdr->p, i);

                /*
                 * Remove all the Characteristic Notification Descriptors stored in flash memory.
                 */
                ble_storage_remove_all(list_item->characteristic_ccc_h);
        }


        /*
         * Remove the previously allocated memory for the Service structure.
         */
        OS_FREE(hdr);
}


/*
 * This function selects a specific item/element from the characteristic list
 */
mcs_characteristic_structure_t* mcs_characteristic_list_select_element(mcs_characteristic_list_element_t *head, uint8_t list_item)
{

        /* Track the current position in the list */
        mcs_characteristic_list_element_t *current_position = head;

        /* Find the requested element */
        if (list_item == 0) {
                return (&current_position->settings);
        } else {
                for (int i = 0; i < list_item; i++) {
                        // Transact the list until you reach the requested element
                        current_position = current_position->next;
                }
                return (&current_position->settings);
        }
}




/*
 * This function initializes the Service handle. It declares callback functions
 * for specific Bluetooth events.
 */
mcs_service_structure_t* mcs_service_handle_init(mcs_characteristic_list_element_t *head, uint8_t num_characteristic)
{
        /* Allocate memory for the service handle */
        mcs_service_structure_t *hdr = (mcs_service_structure_t *) OS_MALLOC(sizeof(mcs_service_structure_t));
        OS_ASSERT(hdr);

        /* Clear the memory */
        memset((char *)hdr, 0x00, sizeof(mcs_service_structure_t));

        hdr->p = head;
        hdr->num_of_characteristics = num_characteristic;

        /* Set callback functions associated with specific BLE events */
        hdr->svc.write_req          = handle_write_req;
        hdr->svc.read_req           = handle_read_req;
        hdr->svc.cleanup            = cleanup;
        hdr->svc.event_sent         = handle_event_sent_evt;
        hdr->svc.prepare_write_req  = handle_prepare_write_req;

        return hdr;
}


/*
 * Create the first element of the linked list that will accommodate all the
 * Characteristic settings of the Bluetooth Service. This would be
 * the head of the list and will be used to traverse the list.
 */
mcs_characteristic_list_element_t* mcs_characteristic_list_init()
{
        /*
         * Allocate memory for the first element of the characteristic list
         */
        mcs_characteristic_list_element_t *head = (mcs_characteristic_list_element_t *)
                                         OS_MALLOC(sizeof(mcs_characteristic_list_element_t));
        OS_ASSERT(head);

        /* Clear the allocated memory */
        memset((char *)(&head->settings), 0x00, sizeof(head->settings));


        /* The last element/item should always be NULL!
          ------------------------------
          |              |             |
          |     DATA     |     NEXT    |-------> NULL
          |              |             |
         ------------------------------
        */
        head->next = NULL;

        return head;
}


/*
 * This function appends a new element in the linked list.
 * The new element is added at the end of the list.
 */
void mcs_characteristic_list_add_element(mcs_characteristic_list_element_t *head)
{

        /* Track the current position in the list */
        mcs_characteristic_list_element_t *current_position = head;

        /* Find the last element of the list (this should be NULL) */
        while (current_position->next != NULL) {
                current_position = current_position->next; // Move forward one position
        }


        /*
          Allocate memory for the new element of the list
         ----------Last Item-----------              -----------New Item-----------
         |              |             |            \ |              |             |
         |     DATA     |     NEXT    |--------------|     DATA     |     NEXT    |--------> NULL
         |              |             |            / |              |             |
         ------------------------------              ------------------------------
        */
        current_position->next = (mcs_characteristic_list_element_t *) OS_MALLOC(sizeof(mcs_characteristic_list_element_t));
        OS_ASSERT(current_position->next);

        current_position->next->next = NULL;   // The last element should always be NULL!


        /* Clear the memory */
        memset((char *)(&current_position->next->settings), 0x00, sizeof(current_position->next->settings));
}


/*
 * Initialize the elements of the linked list with the user-defined values (default values).
 */
void mcs_characteristic_list_set_element_values(mcs_characteristic_list_element_t *head, const mcs_characteristic_config_t val[])
{

        /* Track the current position in the list */
        mcs_characteristic_list_element_t *current_position = head;

        int idx = 0;
        /* Traverse the list and set default values for each element (should be NULL) */
        while (current_position != NULL) {
                /* Apply the user-defined values */
                current_position->settings.cb = &(val[idx].cb);
                current_position->settings.characteristic_max_size = val[idx].characteristic_max_size;

                current_position = current_position->next;  // Move forward
                idx++; // Increment counter by one
        }
}


/*
 * Initialize all the resources required for the Bluetooth Service.
 * First, the function will allocate memory for the linked list that
 * will accommodate all the Characteristic settings.
 */
mcs_characteristic_list_element_t* mcs_service_init(mcs_service_structure_t **svc_hdr, uint8_t num_of_characteristics)
{

        /*
         * Initialize the linked list. The function will return a pointer
         * that point to the first element of the list.
         */
        mcs_characteristic_list_element_t *head = mcs_characteristic_list_init();


        /*
         * Having created the head of the linked list, we can move on
         * creating the rest elements of the list.
         */
        for (int i = 0; i < (num_of_characteristics - 1); i++) {
                mcs_characteristic_list_add_element(head);
        }


        /* Initialize the Bluetooth Service handle */
        *svc_hdr = mcs_service_handle_init(head, num_of_characteristics);


        /* Return the first element of the linked list */
        return head;
}


/*
 * Compute attribute values based on the offset given by the BLE database.
 */
uint16_t mcs_compute_atribute_value(uint16_t *val, uint16_t *hdr)
{
        return *val += *hdr;
}



/*
 * This function checks whether Characteristic User Descriptor has been declared.
 */
bool mcs_is_user_descriptor_enabled(const char *ptr)
{
        // status flag
        int flag;

        /* Check if the assigned value is 'NULL' */
        flag = strncmp(ptr, "NULL", sizeof("NULL"));

        if (flag == 0) {
                return false;
        } else {
                return true;
        }
}


/*
 * This function computes the total number of Descriptor Attributes of the Bluetooth Service.
 */
uint16_t mcs_compute_num_of_descriptors(const mcs_characteristic_config_t settings[], uint8_t num_of_characrteristics)
{
        uint16_t num_of_descriptors = 0;

        for (int i = 0; i < num_of_characrteristics; i++) {
                // Check if notifications/indications are enabled...
                if ((settings[i].notifications == 1) || (settings[i].notifications == 2)) num_of_descriptors++;
                // Check if a Characteristic User Description has been declared...
                if (mcs_is_user_descriptor_enabled(settings[i].characteristic_user_descriptor)) num_of_descriptors++;
        }

        return num_of_descriptors;
}


/*
 * BLE Service initialization.
 */
ble_service_t* mcs_init(const mcs_characteristic_config_t settings[], const char *service_uuid, uint8_t num_of_characrteristics)
{
        uint16_t num_attr;
        att_uuid_t uuid;


        /* Service handle */
        mcs_service_structure_t *service_handle = NULL;

        /* Characteristic settings */
        mcs_characteristic_structure_t *position = NULL;


        /* Bluetooth Service initialization */
        mcs_characteristic_list_element_t *head_list = mcs_service_init(&service_handle, num_of_characrteristics);


        /*  Bluetooth Service configuration with user-defined values */
        mcs_characteristic_list_set_element_values(head_list, settings);


        // Total number of 'Descriptor' declarations
        uint16_t num_of_descriptors = mcs_compute_num_of_descriptors(settings, num_of_characrteristics);


        /*
         * 0* --> Number of 'Included Service' declarations
         * 1* --> Number of 'Characteristic' declarations
         * 2* --> Number of 'Descriptor' declarations
         */
        num_attr = ble_gatts_get_num_attr(0, (uint16_t)num_of_characrteristics, num_of_descriptors);


        /*
         *  'Service' declaration (PRIMARY).
         */
        ble_uuid_from_string(service_uuid, &uuid);
        ble_gatts_add_service(&uuid, GATT_SERVICE_PRIMARY, num_attr);

        /* For all the Characteristic Attributes of the Bluetooth Service */
        for (int i = 0; i < num_of_characrteristics; i++) {

                position = mcs_characteristic_list_select_element(head_list, i);

                /*
                 * 'Characteristic' declarations.
                 */
                ble_uuid_from_string(settings[i].characteristic_128_uuid, &uuid);

                ble_gatts_add_characteristic(&uuid, ((settings[i].characteristic_read_prop) ? GATT_PROP_READ : GATT_PROP_NONE) |
                                ((settings[i].characteristic_write_no_response_prop) ? GATT_PROP_WRITE_NO_RESP    : GATT_PROP_NONE)    |
                                ((settings[i].characteristic_write_prop) ? GATT_PROP_WRITE    : GATT_PROP_NONE)    |
                                ((settings[i].notifications == 1)        ? GATT_PROP_NOTIFY   : GATT_PROP_NONE)    |
                                ((settings[i].notifications == 2)        ? GATT_PROP_INDICATE : GATT_PROP_NONE),
                                                 ATT_PERM_RW, (settings[i].characteristic_max_size),
                                ((settings[i].characteristic_read_prop) ? GATTS_FLAG_CHAR_READ_REQ : 0x00),
                                                                 NULL, &position->characteristic_h);

                /*
                 * 'Characteristic User Descriptor' declarations.
                 */
                if (mcs_is_user_descriptor_enabled(settings[i].characteristic_user_descriptor)) {
                        ble_uuid_create16(UUID_GATT_CHAR_USER_DESCRIPTION, &uuid);

                        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ,
                                             settings[i].characteristic_user_descriptor_size,
                                                           0, &position->characteristic_descriptor_h);

                }


                /*
                 * "Characteristic Notification Descriptor" declarations.
                 */
                if ( (settings[i].notifications == 1) || (settings[i].notifications == 2) ) {
                        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
                        ble_gatts_add_descriptor(&uuid, ATT_PERM_RW, 2, 0, &position->characteristic_ccc_h);   // CHECK FOR HANDLER -
                }
        }


       /*
        * Compute attribute values for the first characteristic.
        */
        position = mcs_characteristic_list_select_element(head_list, 0);
        ble_gatts_register_service(&service_handle->svc.start_h, &position->characteristic_h, &position->characteristic_ccc_h, &position->characteristic_descriptor_h, 0);


        /*
         * Manually, compute attribute values for all the rest characteristics.
         */
        for (int i = 1; i < num_of_characrteristics; i++) {
                position = mcs_characteristic_list_select_element(head_list, i);

                position->characteristic_h              = mcs_compute_atribute_value(&position->characteristic_h, &service_handle->svc.start_h);
                position->characteristic_ccc_h          = mcs_compute_atribute_value(&position->characteristic_ccc_h, &service_handle->svc.start_h);
                position->characteristic_descriptor_h   = mcs_compute_atribute_value(&position->characteristic_descriptor_h, &service_handle->svc.start_h);
        }



        /*
         * Calculate the last attribute handle of the BLE service.
         */
        service_handle->svc.end_h = service_handle->svc.start_h + num_attr;


        /*
         * Declare default values for all the attributes (as needed).
         */
        for (int i = 0; i < num_of_characrteristics; i++) {

                position = mcs_characteristic_list_select_element(head_list, i);

                if (mcs_is_user_descriptor_enabled(settings[i].characteristic_user_descriptor)) {
                        ble_gatts_set_value(position->characteristic_descriptor_h,
                                           settings[i].characteristic_user_descriptor_size,
                                                 (const void *)settings[i].characteristic_user_descriptor);
                }
        }


        /* Add the BLE service to Dialog BLE framework */
        ble_service_add(&service_handle->svc);


        /* Return the BLE service handle! */
        return (&service_handle->svc);

}
