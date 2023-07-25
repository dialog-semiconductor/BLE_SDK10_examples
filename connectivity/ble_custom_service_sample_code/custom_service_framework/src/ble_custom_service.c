
/**
 ****************************************************************************************
 *
 * @file ble_custom_service.c
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

/********************************* Macro definitions ****************************************/
#define UUID_CUSTOM_DEFINITION_MAX_LENGTH       128

/**
 * Max. number of head lists stored for debugging aid.
 *
 * \note This macro has a meaning only if \p MCS_DBG_SERVICES_EN
 *       is set.
 */
#ifndef MCS_DBG_SERVICES_MAX_NUM
#define MCS_DBG_SERVICES_MAX_NUM               ( 2 )
#endif

/**
 * Macro to provide debugging aid. If set, all head lists generated
 * (one linked list for each custom service defined) are stored in
 * an array of size \p MCS_DBG_SERVICES_MAX_NUM
 */
#ifndef MCS_DBG_SERVICES_EN
#define MCS_DBG_SERVICES_EN                    ( 0 )
#endif

/********************************* Retained symbols *****************************************/

/* Notifications head linked list */
__RETAINED_RW static mcs_notif_list_element_t *notif_list_head = NULL;

#if MCS_DBG_SERVICES_EN
__RETAINED mcs_characteristic_list_element_t *database_list_head[MCS_DBG_SERVICES_MAX_NUM];
__RETAINED_RW int cnt_list = 0;
#endif

/********************************* Function prototypes **************************************/
static mcs_attributes_config_t* mcs_select_list_item_by_index(mcs_characteristic_list_element_t *head,
                                                                                          uint8_t item_idx);

static void mcs_free_list(mcs_characteristic_list_element_t *head);

static void notify_peer_devices(ble_service_t *svc, uint16_t size, const uint8_t *value,
                                                                      mcs_attributes_config_t *attr);

static void helper_notify_peer_devices(ble_service_t *svc, uint16_t conn_idx, uint16_t size,
                                                const uint8_t *value, mcs_attributes_config_t *attr);

static mcs_attributes_config_t* mcs_notif_select_list_item_by_uuid(const char *uuid);

static void mcs_notif_remove_list_item_by_addr(uint32_t addr);

/********************************* Static routines *****************************************/

/* Helper function to service ATT write requests. */
static att_error_t helper_att_write_handler(ble_service_t *svc, mcs_attributes_config_t *attr,
                                                                const ble_evt_gatts_write_req_t *evt)
{
        ASSERT_WARNING(svc != NULL);
        ASSERT_WARNING(attr != NULL);
        ASSERT_WARNING(evt != NULL);

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
        if ((attr->cb == NULL) || (attr->cb->set_value == NULL)) {
                return ATT_ERROR_WRITE_NOT_PERMITTED;
        }

        /**
         *  Switch to application context to update the characteristic value
         *  (as requested by the peer device).
         */
        attr->cb->set_value(evt->value, evt->length);

        /* Respond to the BLE_EVT_GATTS_WRITE_REQ event. */
        ble_gatts_write_cfm(evt->conn_idx, attr->attr_h, ATT_ERROR_OK);

        /*
         * Notify all the connected peers, and given that they have their notifications enabled,
         * that characteristic's value has been changed!
         */
        notify_peer_devices(svc, evt->length, evt->value, attr);

        return ATT_ERROR_OK;
}

/* Helper function to service write "Client Characteristic Configuration" (CC requests. */
static att_error_t helper_ccc_write_handler(mcs_attributes_config_t *attr, const ble_evt_gatts_write_req_t *evt)
{
        ASSERT_WARNING(attr != NULL);
        ASSERT_WARNING(evt != NULL);

        uint16_t ccc = GATT_CCC_NONE;

        if (evt->offset) {
                return ATT_ERROR_ATTRIBUTE_NOT_LONG;
        }

        if (evt->length != sizeof(ccc)) {
                return ATT_ERROR_INVALID_VALUE_LENGTH;
        }

        ccc = get_u16(evt->value);

        /* Store the envoy CCC value in Flash memory. */
        ble_storage_put_u32(evt->conn_idx, attr->attr_ccc_h, (uint32_t)ccc, true);

        return ATT_ERROR_OK;
}

/* Helper function to service read ATT requests. */
static void helper_att_read_handler(ble_service_t *svc, mcs_attributes_config_t *attr, const ble_evt_gatts_read_req_t *evt)
{
        ASSERT_WARNING(svc != NULL);
        ASSERT_WARNING(attr != NULL);
        ASSERT_WARNING(evt != NULL);

        uint16_t length = 0;
        uint8_t  *value;

        /*
         * Check whether developer has defined a function for handling
         * characteristic read requests.
         */
        if ((attr->cb == NULL) || (attr->cb->get_value == NULL)) {
                ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);
                return;
        }

        /* Switch to application context to get the characteristic value (as requested by the peer device). */
        attr->cb->get_value(&value, &length);

        /* Make sure the user has defined a valid value */
        ASSERT_WARNING(value != NULL);

        /* Respond to the BLE_EVT_GATTS_READ_REQ BLE event. */
        ble_gatts_read_cfm(evt->conn_idx, attr->attr_h, ATT_ERROR_OK, length, (const void *)value);

}

/*
 * Helper function to send notifications to the peer devices.
 *
 * \note This routine has the same functionality as \sa helper_notify_peer_devices() but requires
 *       different input parameters.
 */
static void helper_send_notif(uint16_t conn_idx, uint16_t size, const uint8_t *value, uint16_t attr_h, uint16_t ccc_h)
{
        uint16_t ccc = GATT_CCC_NONE;

        /*
         * Get the Client Configuration Characteristic (CCC) value stored in flash memory.
         */
         ble_storage_get_u16(conn_idx, ccc_h, &ccc);

         /* Check whether notifications are enabled by the peer device. */
         if ( ((!(ccc & GATT_CCC_NOTIFICATIONS)) && (!(ccc & GATT_CCC_INDICATIONS))) ) {
                 return;
         }

         if (ccc & GATT_CCC_NOTIFICATIONS) {
                 ble_gatts_send_event(conn_idx, attr_h, GATT_EVENT_NOTIFICATION, size, (const void *)value);
         } else if (ccc & GATT_CCC_INDICATIONS) {
                 ble_gatts_send_event(conn_idx, attr_h, GATT_EVENT_INDICATION, size, (const void *)value);
         }
}

/*
 * Notify peer devices that an ATT value has been changed.
 *
 * \note This routine has the same functionality as \sa mcs_send_notifications() but requires
 *       different input parameters.
 */
static void notify_peer_devices(ble_service_t *svc, uint16_t size, const uint8_t *value, mcs_attributes_config_t *attr)
{
        ASSERT_WARNING(svc != NULL);
        ASSERT_WARNING(attr != NULL);
        ASSERT_WARNING(value != NULL);

        uint8_t num_conn;
        uint16_t *conn_idx;

        ble_gap_get_connected(&num_conn, &conn_idx);

        /* For all the connected peer devices. s*/
        while (num_conn--) {
                helper_notify_peer_devices(svc, conn_idx[num_conn], size, value, attr);
        }

        if (conn_idx) {
                OS_FREE(conn_idx);
        }
}

/*
 * Helper function to notify peer devices.
 *
 * \note This routine has the same functionality as \sa helper_send_notif() but requires
 *       different input parameters.
 *
 */
static void helper_notify_peer_devices(ble_service_t *svc, uint16_t conn_idx, uint16_t size,
                                                const uint8_t *value, mcs_attributes_config_t *attr)
{
        ASSERT_WARNING(svc != NULL);
        ASSERT_WARNING(attr != NULL);
        ASSERT_WARNING(value != NULL);

        uint16_t ccc = GATT_CCC_NONE;

       /* Get the Client Configuration Characteristic (CCC) value stored in flash memory. */
        ble_storage_get_u16(conn_idx, attr->attr_ccc_h, &ccc);

        /* Check whether notifications are explicitly enabled by the peer device. */
        if ( ((!(ccc & GATT_CCC_NOTIFICATIONS)) && (!(ccc & GATT_CCC_INDICATIONS))) ) {
                return;
        }

        if (ccc & GATT_CCC_NOTIFICATIONS) {
                ble_gatts_send_event(conn_idx, attr->attr_h, GATT_EVENT_NOTIFICATION,
                                                                           size, (const void *)value);
        } else if (ccc & GATT_CCC_INDICATIONS) {
                ble_gatts_send_event(conn_idx, attr->attr_h, GATT_EVENT_INDICATION,
                                                                            size, (const void *)value);
        }
}

/* Handler to service \sa BLE_EVT_GATTS_EVENT_SENT BLE events */
static void handle_event_sent(ble_service_t *svc, const ble_evt_gatts_event_sent_t *evt)
{
        ASSERT_WARNING(svc != NULL);
        ASSERT_WARNING(evt != NULL);

        mcs_service_config_t *hdr = (mcs_service_config_t *) svc;

        for (int idx = 0; (idx < hdr->num_of_characteristics); idx++) {

                mcs_attributes_config_t *attr = mcs_select_list_item_by_index(hdr->head, idx);

                if (evt->handle == attr->attr_h) {
                        if (attr->cb->event_sent) {
                                attr->cb->event_sent(evt->conn_idx, evt->status, evt->type);
                        }
                        return;
                }
        }
}

/* Handler to service \sa BLE_EVT_GATTS_READ_REQ BLE events. */
static void handle_read_req(ble_service_t *svc, const ble_evt_gatts_read_req_t *evt)
{
        ASSERT_WARNING(svc != NULL);
        ASSERT_WARNING(evt != NULL);

        mcs_service_config_t *hdr = (mcs_service_config_t *) svc;

        /*
         * Start transacting the characteristic list until the requested attribute is reached.
         * Then, exit from callback (there is no need to scan the rest items of the list)
         */
        for (int idx = 0; (idx < hdr->num_of_characteristics); idx++) {

                mcs_attributes_config_t *attr = mcs_select_list_item_by_index(hdr->head, idx);

                /* Check if the requested attribute is a valid attribute that can be handled. */
                if (evt->handle == attr->attr_h) {
                        helper_att_read_handler(svc, attr, evt);
                        return;
                } else if (evt->handle == attr->attr_ccc_h) {  // A request to read the descriptor of the Characteristic
                        uint16_t ccc = 0x0000;

                        /* Extract the CCC value from the BLE storage */
                        ble_storage_get_u16(evt->conn_idx, attr->attr_ccc_h, &ccc);

                        /* We're little-endian - OK to write directly from uint16_t */
                        ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_OK, sizeof(ccc), (const void *)&ccc);
                        return;
                }
        }

        /* This line will be reached if an invalid attribute has been requested. */
        ble_gatts_read_cfm(evt->conn_idx, evt->handle, ATT_ERROR_READ_NOT_PERMITTED, 0, NULL);

}

/* Handler to service \sa BLE_EVT_GATTS_WRITE_REQ BLE events. */
static void handle_write_req(ble_service_t *svc, const ble_evt_gatts_write_req_t *evt)
{
        ASSERT_WARNING(svc != NULL);
        ASSERT_WARNING(evt != NULL);

        mcs_service_config_t *hdr = (mcs_service_config_t *) svc;
        att_error_t status = ATT_ERROR_WRITE_NOT_PERMITTED;

        /* Start traversing the characteristic list until the requested attribute is reached. */
        for (int idx = 0; (idx < hdr->num_of_characteristics); idx++) {

                mcs_attributes_config_t *attr = mcs_select_list_item_by_index(hdr->head, idx);

                /* Check if the requested attribute is a valid attribute that can be handled */
                if (evt->handle == attr->attr_h) {
                        status = helper_att_write_handler(svc, attr, evt);
                        goto done;
                } else if (evt->handle == attr->attr_ccc_h) {
                        status = helper_ccc_write_handler(attr, evt);
                        goto done;
                }

        }

done:
                if (status == ((att_error_t) - 1)) {
                        return; // Write handler executed properly
                }

        /* This code line will be reached if an invalid attribute has been requested. */
        ble_gatts_write_cfm(evt->conn_idx, evt->handle, status);
}

/* Handler to service \sa BLE_EVT_GATTS_PREPARE_WRITE_REQ BLE events. */
static void handle_prepare_write_req(ble_service_t *svc, const ble_evt_gatts_prepare_write_req_t *evt)
{
        ASSERT_WARNING(svc != NULL);
        ASSERT_WARNING(evt != NULL);

        mcs_service_config_t *hdr = (mcs_service_config_t *) svc;

        for (int idx = 0; idx < hdr->num_of_characteristics; idx++) {
                mcs_attributes_config_t *attr = mcs_select_list_item_by_index(hdr->head, idx);

                if (evt->handle == attr->attr_h) {
                        /* Response for the prepare write request */
                        ble_gatts_prepare_write_cfm(evt->conn_idx, evt->handle, attr->characteristic_max_size, ATT_ERROR_OK);
                        return;
                }
        }
}



/* Handler to service BLE database cleanup requests */
static void handle_cleanup_req(ble_service_t *svc)
{
        ASSERT_WARNING(svc != NULL);

        mcs_service_config_t *hdr = (mcs_service_config_t *) svc;

        for (int i = 0; i < hdr->num_of_characteristics; i++) {

                mcs_attributes_config_t *list_item = mcs_select_list_item_by_index(hdr->head, i);

                /* Remove all the Characteristic Notification Descriptors stored in flash memory. */
                ble_storage_remove_all(list_item->attr_ccc_h);
                mcs_notif_remove_list_item_by_addr((uint32_t)list_item);
        }

        /* Remove previously allocated memory spaces. */
        mcs_free_list(hdr->head);
        OS_FREE(hdr);
}

/*
 * This function initializes the Service handle. It declares callback functions
 * for specific Bluetooth events.
 */
static mcs_service_config_t* mcs_service_database_init(mcs_characteristic_list_element_t *head,
                                                                        uint8_t num_of_characteristics)
{
        ASSERT_WARNING(head != NULL);

        /* Allocate memory for the service handle */
        mcs_service_config_t *hdr = (mcs_service_config_t *) OS_MALLOC(sizeof(mcs_service_config_t));
        OS_ASSERT(hdr != NULL);

        /* Clear the memory */
        memset((char *)hdr, 0x00, sizeof(mcs_service_config_t));

        hdr->head = head;
        hdr->num_of_characteristics = num_of_characteristics;

        /* Set callback functions associated with specific BLE events */
        hdr->svc.write_req          = handle_write_req;
        hdr->svc.read_req           = handle_read_req;
        hdr->svc.cleanup            = handle_cleanup_req;
        hdr->svc.event_sent         = handle_event_sent;
        hdr->svc.prepare_write_req  = handle_prepare_write_req;

        return hdr;
}

/*
 * Create the first element of the linked list that will accommodate all the
 * Characteristic settings of the Bluetooth Service. This would be
 * the head of the list and will be used to traverse the list.
 */
static mcs_characteristic_list_element_t* mcs_list_init(void)
{
        /* Allocate memory for the first element of the characteristic list */
        mcs_characteristic_list_element_t *head = (mcs_characteristic_list_element_t *)
                                         OS_MALLOC(sizeof(mcs_characteristic_list_element_t));
        OS_ASSERT(head != NULL);

        /* Clear the allocated memory */
        memset((void *)(&head->config), 0x0, sizeof(head->config));

        /* The last element/item should always be NULL!
          ------------------------------
          |              |             |
          |     DATA     |     NEXT    |-------> NULL
          |              |             |
         ------------------------------
        */
        head->next = NULL;

        if (notif_list_head == NULL) {
                /* Allocate memory for the first element of the characteristic notification list */
                notif_list_head = (mcs_notif_list_element_t *)
                                                OS_MALLOC(sizeof(mcs_notif_list_element_t));
                OS_ASSERT(notif_list_head != NULL);

                /* Clear the space allocated */
                memset((void *)(&notif_list_head->config), 0x0, sizeof(notif_list_head->config));
                notif_list_head->next = NULL;
        }
        return head;
}

/* Function to clear an attribute linked list */
static void mcs_free_list(mcs_characteristic_list_element_t *head)
{
        ASSERT_WARNING(head != NULL);

        mcs_characteristic_list_element_t *current_node = head;

        while (current_node != NULL) {
                /* First get the next element */
                mcs_characteristic_list_element_t *next_node = current_node->next;

                OS_FREE(current_node);
                current_node = next_node;
        }
}

/*
 * Function to allocate memory for an ATT configuration structure appended at the end
 * of the linked list.
 *
 * \note The function resets all memory allocated to zero. To initialize the memory
 *       allocated use \sa mcs_set_list_items()
 */
static void mcs_add_list_item(mcs_characteristic_list_element_t *head)
{
        ASSERT_WARNING(head != NULL);

        mcs_characteristic_list_element_t *current_node;

        /* Find the last element of the list (this should be NULL) */
        current_node = head;
        while (current_node->next != NULL) {
                current_node = current_node->next; // Move forward one position
        }

        /*
          Allocate memory for the new element of the list
         ----------Last Item-----------              -----------New Item-----------
         |              |             |            \ |              |             |
         |     DATA     |     NEXT    |--------------|     DATA     |     NEXT    |--------> NULL
         |              |             |            / |              |             |
         ------------------------------              ------------------------------
        */
        current_node->next = (mcs_characteristic_list_element_t *)
                                        OS_MALLOC(sizeof(mcs_characteristic_list_element_t));
        OS_ASSERT(current_node->next != NULL);

        /* Clear the memory */
        current_node->next->next = NULL;
        memset((void *)(&current_node->next->config), 0x0, sizeof(current_node->next->config));

}

/*
 * Initialize one or more ATT configuration structures (linked list).
 *
 * \note This routine is called after calling \sa mcs_add_list_item().
 *
 */
static void mcs_set_list_items(mcs_characteristic_list_element_t *head,
                        const mcs_characteristic_config_t cfg[], size_t num_of_items)
{
        ASSERT_WARNING(head != NULL);

        mcs_characteristic_list_element_t *current_node = head;

        int i = 0;
        for (; i < num_of_items && current_node != NULL; i++) {
                current_node->config.cb = &cfg[i].cb;
                current_node->config.characteristic_max_size = cfg[i].max_size;

                current_node = current_node->next;
        }
        ASSERT_WARNING(i == num_of_items); // Make sure all requested items are written
}

/* Function to select an ATT configuration structure (linked list)  */
static mcs_attributes_config_t* mcs_select_list_item_by_index(mcs_characteristic_list_element_t *head,
                                                                                            uint8_t item_idx)
{
        ASSERT_WARNING(head != NULL);

        mcs_characteristic_list_element_t *current_node = head;

        /* Find the requested element */
        if (item_idx == 0) {
                return (&current_node->config);
        } else {
                int i = 0;
                for (; i < item_idx && current_node != NULL; i++) {
                        /* Traverse the list until you reach the requested element */
                        current_node = current_node->next;
                }
                ASSERT_WARNING(i == item_idx); // Make sure all requested items are traversed

                return (&current_node->config);
        }
}

/* Function to remove item by address (address of the ATT configuration structures) */
static void mcs_notif_remove_list_item_by_addr(uint32_t addr)
{
        mcs_notif_list_element_t *current_node = notif_list_head;
        mcs_notif_list_element_t *previous_node = NULL;

        while (current_node != NULL) {
                if ((uint32_t)current_node->config.cfg == (uint32_t)addr) {
                        break;
                }
                previous_node = current_node;
                current_node = current_node->next;
        }
        /**
         * This expression will be valid if either the list (head) is not empty
         * or the list was not traversed till its last item.
         */
        if (current_node != NULL) {
                if (previous_node != NULL) {
                        previous_node->next = current_node->next;
                } else {
                        notif_list_head = current_node->next;
                }
                OS_FREE(current_node);
        }
}

/* Function to get the location/index of the next empty location in the notification linked list */
static int mcs_notif_get_empty_list_item(void)
{
        mcs_notif_list_element_t *current_node = notif_list_head;
        int idx = 0;

        while (current_node != NULL) {
                current_node = current_node->next;
                idx++;
        }
        return idx;
}

/* Function to select notification configuration structure by index */
static mcs_notif_list_element_t* mcs_notif_select_list_item_by_index(int item_idx)
{
        mcs_notif_list_element_t *current_node = notif_list_head;

        if (item_idx == 0) {
                return current_node;
        } else {
                int i = 0;
                for (; i < item_idx && current_node != NULL; i++) {
                        current_node = current_node->next;
                }
                ASSERT_WARNING(i == item_idx); // Make sure all requested items are traversed

                return current_node;
        }
}

/* Function to select notification configuration structure by UUID */
static mcs_attributes_config_t* mcs_notif_select_list_item_by_uuid(const char *uuid)
{
        ASSERT_WARNING(uuid != NULL);

        mcs_notif_list_element_t *current_node = notif_list_head;

        while (current_node != NULL) {
                if (strncmp(uuid, current_node->config.uuid, UUID_CUSTOM_DEFINITION_MAX_LENGTH) == 0) {
                        return current_node->config.cfg;
                }
                current_node = current_node->next;
        }
        return NULL;
}

/*
 * Function to allocate memory for a notification configuration structure appended at the end
 * of the linked list.
 *
 * \note The function resets all memory allocated to zero. To initialize the memory
 *       allocated use \sa mcs_set_list_items()
 */
static void mcs_notif_add_list_item(void)
{
        mcs_notif_list_element_t *current_node = notif_list_head;

        while (current_node->next != NULL) {
                current_node = current_node->next;
        }

        current_node->next = (mcs_notif_list_element_t *)
                                        OS_MALLOC(sizeof(mcs_notif_list_element_t));
        OS_ASSERT(current_node->next != NULL);

        /* Clear the memory */
        current_node->next->next = NULL;
        memset((void *)(&current_node->next->config), 0x0, sizeof(current_node->next->config));
}

/*
 * Initialize one or more notifications configuration structures (linked list).
 *
 * \note This routine is called after calling \sa mcs_notif_add_list_item() and
 *       \sa mcs_list_init().
 *
 */
static void mcs_notif_set_list_items(mcs_characteristic_list_element_t *head,
                const mcs_characteristic_config_t cfg[], size_t num_of_items, int notif_idx)
{
        ASSERT_WARNING(head != NULL);
        ASSERT_WARNING(cfg != NULL);

        mcs_characteristic_list_element_t *current_node;
        mcs_notif_list_element_t *current_notif_node;

        current_node = head;
        current_notif_node = mcs_notif_select_list_item_by_index(notif_idx);

        int i = 0;
        for (; i < num_of_items && current_node != NULL && current_notif_node != NULL; i++) {
                current_notif_node->config.uuid = cfg[i].uuid;
                current_notif_node->config.cfg = &current_node->config;

                current_node = current_node->next;
                current_notif_node = current_notif_node->next;
        }
        ASSERT_WARNING(i == num_of_items); // Make sure all requested items are written
}

/* Function to initialize all resources required for the Bluetooth custom service. */
static mcs_characteristic_list_element_t* helper_service_init(mcs_service_config_t **svc_hdr, uint8_t num_of_characteristics)
{
        bool notif_list_null = notif_list_head == NULL ? true : false;

        /*
         * Initialize the linked list. The function will return a pointer
         * that point to the first element of the list.
         */
        mcs_characteristic_list_element_t *head = mcs_list_init();

        /*
         * Having created the head of the linked list, we can move on
         * creating the rest elements of the list.
         */
        for (int i = 0; i < (num_of_characteristics - 1); i++) {
                mcs_add_list_item(head);
        }
        for (int i = 0; i < (notif_list_null ? (num_of_characteristics - 1) : num_of_characteristics); i++) {
                mcs_notif_add_list_item();
        }

        /* Initialize the Bluetooth Service handle */
        *svc_hdr = mcs_service_database_init(head, num_of_characteristics);

        /* Return the first element of the linked list */
        return head;
}

/* Helper function to compute the ATT handle offsets/values in the BLE database created. */
static uint16_t helper_compute_att_handle_offset(uint16_t *attr, uint16_t *hdr)
{
        ASSERT_WARNING(attr != NULL);
        ASSERT_WARNING(hdr != NULL);

        return *attr += *hdr;
}

/*
 * Helper function to define whether or not a Characteristic User Descriptor is declared/valid. A
 * string set to "NULL" indicates an invalid descriptor declaration.
 */
static bool helper_is_user_descriptor_defined(const char *str)
{
        ASSERT_WARNING(str != NULL);

        int status;

        /* Check if the assigned value is 'NULL' */
        status = strncmp(str, "NULL", sizeof("NULL"));

        if (status == 0) {
                return false;
        } else {
                return true;
        }
}

/* Helper function to compute the total number of descriptor ATTs defined in the BLE custom service. */
static uint16_t helper_compute_total_num_of_descriptors(const mcs_characteristic_config_t cfg[], uint8_t num_of_characrteristics)
{
        ASSERT_WARNING(cfg != NULL);

        uint16_t num_of_descriptors = 0;

        for (int i = 0; i < num_of_characrteristics; i++) {
                /* Check if notifications/indications are enabled */
                if ((cfg[i].gatt_prop & GATT_PROP_NOTIFY) || (cfg[i].gatt_prop & GATT_PROP_INDICATE)) {
                                num_of_descriptors++;
                }
                /* Check if a Characteristic User Description has been declared */
                if (helper_is_user_descriptor_defined(cfg[i].user_descriptor)) {
                        num_of_descriptors++;
                }
        }
        return num_of_descriptors;
}

/********************************* User APIs routines *****************************************/

/*
 * Function used to send notifications to the peer devices.
 *
 * \note This routine has the same functionality as \sa notify_peer_devices() but requires
 *       different input parameters.
 */
bool mcs_send_notifications(const char *uuid, const uint8_t *value, uint16_t size)
{
        ASSERT_WARNING(uuid != NULL);
        ASSERT_WARNING(value != NULL);

        uint8_t num_conn;
        uint16_t *conn_idx;
        mcs_attributes_config_t *cfg;

        cfg = mcs_notif_select_list_item_by_uuid(uuid);
        if (cfg == NULL) {
                return false;
        }

        ble_gap_get_connected(&num_conn, &conn_idx);

        /* For all the connected peer devices. */
        while (num_conn--) {
                helper_send_notif(conn_idx[num_conn], size, value, cfg->attr_h, cfg->attr_ccc_h);
        }

        if (conn_idx) {
                OS_FREE(conn_idx);
        }
        return true;
}

/* Routine to declare a Bluetooth custom service */
ble_service_t* mcs_service_init(const mcs_characteristic_config_t cfg[], const char *service_uuid, uint8_t num_of_characrteristics)
{
        ASSERT_WARNING(cfg != NULL);
        ASSERT_WARNING(service_uuid != NULL);

        uint16_t num_of_attributes, num_of_descriptors;
        int notif_empty_idx;
        att_uuid_t uuid;
        mcs_service_config_t *service_handle;
        mcs_attributes_config_t *current_position;
        mcs_characteristic_list_element_t *head_list;

        /*
         * Get the position of the last allocated item (next free position) before registering
         * new service.
         */
        notif_empty_idx = mcs_notif_get_empty_list_item();

        /* Bluetooth customer service initialization */
        head_list = helper_service_init(&service_handle, num_of_characrteristics);

#if MCS_DBG_SERVICES_EN
        if (cnt_list < MCS_DBG_SERVICES_MAX_NUM) {
                database_list_head[cnt_list++] = head_list;
        }
#endif

        /* Set linked lists items */
        mcs_set_list_items(head_list, cfg, num_of_characrteristics);
        mcs_notif_set_list_items(head_list, cfg, num_of_characrteristics, notif_empty_idx);

        num_of_descriptors = helper_compute_total_num_of_descriptors(cfg, num_of_characrteristics);

        /**
         * \note 1st input parameters defines number of included services that are not currently supported!!!
         */
        num_of_attributes = ble_gatts_get_num_attr(0, (uint16_t)num_of_characrteristics, num_of_descriptors);

        /* ATT Primary Service declaration. */
        ble_uuid_from_string(service_uuid, &uuid);
        ble_gatts_add_service(&uuid, GATT_SERVICE_PRIMARY, num_of_attributes);

        /* For all the Characteristic Attributes of the Bluetooth Service */
        for (int i = 0; i < num_of_characrteristics; i++) {

                current_position = mcs_select_list_item_by_index(head_list, i);

                /* ATT Characteristic declarations. */
                ble_uuid_from_string(cfg[i].uuid, &uuid);

                ble_gatts_add_characteristic(&uuid, cfg[i].gatt_prop, cfg[i].att_perm, cfg[i].max_size,
                         ((cfg[i].gatt_prop & GATT_PROP_READ) ? GATTS_FLAG_CHAR_READ_REQ : 0x00), NULL,
                                                                                 &current_position->attr_h);

                /* ATT Characteristic User Descriptor declarations. */
                if (helper_is_user_descriptor_defined(cfg[i].user_descriptor)) {
                        ble_uuid_create16(UUID_GATT_CHAR_USER_DESCRIPTION, &uuid);

                        ble_gatts_add_descriptor(&uuid, ATT_PERM_READ, cfg[i].user_descriptor_size,
                                                          0, &current_position->attr_descriptor_h);

                }

                /* ATT Characteristic Notification Descriptor declarations. */
                if ( (cfg[i].gatt_prop & GATT_PROP_NOTIFY ) || (cfg[i].gatt_prop & GATT_PROP_INDICATE) ) {
                        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
                        ble_gatts_add_descriptor(&uuid, ATT_PERM_RW, 2, 0, &current_position->attr_ccc_h);   // CHECK FOR HANDLER -
                }
        }

        /* Compute ATT handle values for the first attribute. */
        current_position = mcs_select_list_item_by_index(head_list, 0);
        ble_gatts_register_service(&service_handle->svc.start_h, &current_position->attr_h,
                                        &current_position->attr_ccc_h, &current_position->attr_descriptor_h, 0);

        /* Manually, compute ATT handle values for the rest of attributes. */
        for (int i = 1; i < num_of_characrteristics; i++) {
                current_position = mcs_select_list_item_by_index(head_list, i);

                current_position->attr_h              = helper_compute_att_handle_offset(&current_position->attr_h, &service_handle->svc.start_h);
                current_position->attr_ccc_h          = helper_compute_att_handle_offset(&current_position->attr_ccc_h, &service_handle->svc.start_h);
                current_position->attr_descriptor_h   = helper_compute_att_handle_offset(&current_position->attr_descriptor_h, &service_handle->svc.start_h);
        }

        /* Calculate the last attribute handle value for the target BLE custom service database. */
        service_handle->svc.end_h = service_handle->svc.start_h + num_of_attributes;

        /* Declare default values for all the attributes (per needs). */
        for (int i = 0; i < num_of_characrteristics; i++) {

                current_position = mcs_select_list_item_by_index(head_list, i);

                if (helper_is_user_descriptor_defined(cfg[i].user_descriptor)) {
                        ble_gatts_set_value(current_position->attr_descriptor_h,
                                cfg[i].user_descriptor_size, (const void *)cfg[i].user_descriptor);
                }
        }

        /* Register the BLE custom service database to Dialog BLE database */
        ble_service_add(&service_handle->svc);

        /* Return the BLE service handle (used for debugging purposes) */
        return (&service_handle->svc);
}
