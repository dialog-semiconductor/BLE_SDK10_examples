/**
 ****************************************************************************************
 *
 * @file usb_cdc.c
 *
 * @brief USB CDC app implementation
 *
 * Copyright (C) 2016-2022 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <stdarg.h>
#include "sys_charger.h"
#include "sys_power_mgr.h"
#include "sys_usb.h"
#include "hw_usb.h"
#include "sys_watchdog.h"
#include "USB_CDC.h"
#include "USB_HID.h"
#include "osal.h"

/*********************************************************************
 *
 *       Defines, configurable
 *
 **********************************************************************
 */

#define usb_main_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )
__RETAINED static OS_TASK usb_cdc_rx_task_handle;
__RETAINED static OS_TASK usb_cdc_tx_task_handle;
__RETAINED static OS_TASK usb_hid_task_handle;

__RETAINED volatile static uint8 run_usb_cdc_hid_task = false;
__RETAINED static OS_EVENT wait_for_rx_task_to_finish;
__RETAINED static USB_HID_HANDLE usb_hid_h;

__RETAINED USB_CDC_HANDLE hCDC_Inst;


static USB_HOOK UsbpHook;

#define USB_MAX_OUT_BUFS        3

static struct {
        uint8_t read[USB_MAX_PACKET_SIZE];

        uint8_t in[USB_MAX_PACKET_SIZE];

        uint8_t out[USB_MAX_OUT_BUFS + 1][USB_MAX_PACKET_SIZE];
        size_t out_idx;
} usb_hid_bufs;


//
//  Information that is used during enumeration.
//
static const USB_DEVICE_INFO _DeviceInfo = {
        0x2DCF,         // VendorId
        0x6002,         // ProductId
        "Dialog Semiconductor",       // VendorName
        "DA1469x CDC HID",   // ProductName
        "12345678"      // SerialNumber.
};

#ifndef debug_print
#define debug_print(fmt, ...)
#endif

typedef struct {
        int32_t len;
        void*    pdata;
}__q_item;


//Usage Page      Generic Desktop Controls (0x01)
//Usage   Mouse (0x02)
//Collection      Application (0x01)
//  Usage Pointer (0x01)
//  Collection    Physical (0x00)
//    Usage Page  Button (0x09)
//    Usage Minimum       Button 1 (primary/trigger) (0x01)
//    Usage Maximum       Button 3 (tertiary) (0x03)
//    Logical Minimum     0
//    Logical Maximum     1
//    Report Count        8
//    Report Size 1
//    Input       Data (0x02)
//    Usage Page  Generic Desktop Controls (0x01)
//    Usage       X (0x30)
//    Usage       Y (0x31)
//    Usage       Wheel (0x38)
//    Logical Minimum     -127
//    Logical Maximum     127
//    Report Size 8
//    Report Count        3
//    Input       Data (0x06)
//  End Collection
//End Collection

#define HID_BLE_REPORT_MAP \
        0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01, \
        0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x03, \
        0x15, 0x00, 0x25, 0x01, 0x95, 0x08, 0x75, 0x01, \
        0x81, 0x02, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31, \
        0x09, 0x38, 0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, \
        0x95, 0x03, 0x81, 0x06, 0xC0, 0xC0


static const uint8_t report_map[] = {
        HID_BLE_REPORT_MAP
};


__RETAINED static OS_QUEUE usb_queue;

void usb_cdc_TX_task(void *params);
void usb_cdc_RX_task(void *params);
void usb_hid_task(void *params);


/******************************************************************************/
/*                            ###  START  ###                                 */
/*                                                                            */
/* Common code for the USB CDC to be hooked to the SEGGER library and events. */
/*                                                                            */
/******************************************************************************/



/*********************************************************************
 *
 *       _AddCDC
 *
 *  Function description
 *    Add communication device class to USB stack
 */
static USB_CDC_HANDLE _AddCDC(void)
{
        static U8 _abOutBuffer[USB_MAX_PACKET_SIZE];
        USB_CDC_INIT_DATA InitData;
        USB_CDC_HANDLE hInst;

        InitData.EPIn = USBD_AddEP(USB_DIR_IN, USB_TRANSFER_TYPE_BULK, 0, NULL, 0);
        InitData.EPOut = USBD_AddEP(USB_DIR_OUT, USB_TRANSFER_TYPE_BULK, 0, _abOutBuffer,
                USB_MAX_PACKET_SIZE);
        InitData.EPInt = USBD_AddEP(USB_DIR_IN, USB_TRANSFER_TYPE_INT, 8, NULL, 0);
        hInst = USBD_CDC_Add(&InitData);

        return hInst;
}

void usb_cdc_state_cb(void * pContext, U8 NewState)
{
        int OldState = USBD_GetState();

        if (((OldState & USB_STAT_ATTACHED) == 0) && (NewState & USB_STAT_ATTACHED)) {
                //Attached
        }

        if ((OldState & USB_STAT_ATTACHED) && ((NewState & USB_STAT_ATTACHED) == 0)) {
                //Detached
        }

        if (((OldState & USB_STAT_READY) == 0) && (NewState & USB_STAT_READY)) {
                //Ready
        }

        if ((OldState & USB_STAT_READY) && ((NewState & USB_STAT_READY) == 0)) {
                //Un-Ready
        }

        if (((OldState & USB_STAT_ADDRESSED) == 0) && (NewState & USB_STAT_ADDRESSED)) {
                //Addressed
        }

        if ((OldState & USB_STAT_ADDRESSED) && ((NewState & USB_STAT_ADDRESSED) == 0)) {
                //Un-Addressed
        }

        if (((OldState & USB_STAT_CONFIGURED) == 0) && (NewState & USB_STAT_CONFIGURED)) {
                //Configured
        }

        if ((OldState & USB_STAT_CONFIGURED) && ((NewState & USB_STAT_CONFIGURED) == 0)) {
                //Un-Configured
        }

        if (((OldState & USB_STAT_SUSPENDED) == 0) && (NewState & USB_STAT_SUSPENDED)) {
                // USB is going to be Suspended - DO NOT USE THIS POINT TO TRIGGER APP CODE!
                debug_print("USB Node State: Suspend (o:%d, n:%d)!\r\n", OldState, NewState);

        }

        if ((OldState & USB_STAT_SUSPENDED) && ((NewState & USB_STAT_SUSPENDED) == 0)) {
                // USB is going to be Resumed - DO NOT USE THIS POINT TO TRIGGER APP CODE!
                debug_print("USB Node State: Resume (o:%d, n:%d)!\r\n", OldState, NewState);
        }
}


static USB_HID_HANDLE usbd_hid_add(void)
{
        USB_HID_INIT_DATA hid_data = { };
        USB_HID_HANDLE hid_h;

        hid_data.EPIn = USBD_AddEP(USB_DIR_IN, USB_TRANSFER_TYPE_INT, 0, NULL, 0);
        hid_data.EPOut = USBD_AddEP(USB_DIR_OUT, USB_TRANSFER_TYPE_INT, 0,
                usb_hid_bufs.in, sizeof(usb_hid_bufs.in));

        hid_data.pReport = report_map;
        hid_data.NumBytesReport = sizeof(report_map);

        hid_h = USBD_HID_Add(&hid_data);

        return hid_h;
}

static int hid_on_getreport_request_cb(USB_HID_REPORT_TYPE type, unsigned id,
                                                        const uint8_t **data, uint32_t *num_bytes)
{

        return -1;
}

static void hid_on_setreport_request_cb(USB_HID_REPORT_TYPE type, unsigned id, uint32_t num_bytes)
{

}
void usb_cdc_hid_start()
{
        OS_BASE_TYPE status;

        USBD_Init();
        USBD_EnableIAD();
        USBD_HID_Init();
        USBD_CDC_Init();
        USBD_RegisterSCHook(&UsbpHook, usb_cdc_state_cb, NULL);
        hCDC_Inst = _AddCDC();
        USBD_SetDeviceInfo(&_DeviceInfo);

        usb_hid_h = usbd_hid_add();
        USBD_HID_SetOnGetReportRequest(usb_hid_h, hid_on_getreport_request_cb);
        USBD_HID_SetOnSetReportRequest(usb_hid_h, hid_on_setreport_request_cb);
        USBD_SetAllowRemoteWakeUp(1);


#if (dg_configUSE_USB_ENUMERATION == 1)
# if (dg_configUSE_SYS_CHARGER == 1)
        /* Request the minimum guaranteed current budget. */
        USBD_SetMaxPower(hw_charger_i_level_to_miliamp(HW_CHARGER_I_LEVEL_500));
# endif /* dg_configUSE_SYS_CHARGER */
#endif /* dg_configUSE_USB_ENUMERATION */

        USBD_Start();

        /* Create the queue to use for passing the RX to TX for the simple loopback */
        OS_QUEUE_CREATE(usb_queue, sizeof(__q_item), 20);
        OS_ASSERT(usb_queue != NULL);

        OS_EVENT_CREATE(wait_for_rx_task_to_finish);

        /* Start the USB CDC RX application task. */
        status = OS_TASK_CREATE("USB CDC RX Task",   /* The text name assigned to the task, for
                                                      * debug only; not used by the kernel.      */
                        usb_cdc_RX_task,             /* The function that implements the task.   */
                        NULL,                        /* The parameter passed to the task.        */
                        512,                         /* The number of bytes to allocate to the
                                                      * stack of the task.                       */
                        usb_main_TASK_PRIORITY,      /* The priority assigned to the task.       */
                        usb_cdc_rx_task_handle);     /* The task handle.                         */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the USB CDC TX application task. */
        status = OS_TASK_CREATE("USB CDC TX Task",   /* The text name assigned to the task, for
                                                      * debug only; not used by the kernel.      */
                        usb_cdc_TX_task,             /* The function that implements the task.   */
                        NULL,                        /* The parameter passed to the task.        */
                        512,                         /* The number of bytes to allocate to the
                                                      * stack of the task.                       */
                        usb_main_TASK_PRIORITY,      /* The priority assigned to the task.       */
                        usb_cdc_tx_task_handle);     /* The task handle.                         */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the USB CDC TX application task. */
        status = OS_TASK_CREATE("USB HID Task",      /* The text name assigned to the task, for
                                                      * debug only; not used by the kernel.      */
                        usb_hid_task,                /* The function that implements the task.   */
                        NULL,                        /* The parameter passed to the task.        */
                        512,                         /* The number of bytes to allocate to the
                                                      * stack of the task.                       */
                        usb_main_TASK_PRIORITY,      /* The priority assigned to the task.       */
                        usb_hid_task_handle);        /* The task handle.                         */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

}

void usb_cdc_hid_stop()
{
        USBD_UnregisterSCHook(&UsbpHook);

        USBD_DeInit();
        USBD_Stop();
}

/*********************************************************************
 *
 *       sys_usb_ext_hook_begin_enumeration
 *
 *  Function description
 *    Event callback called from the VBUS event to notify
 *    the application that need to proceed with enumeration.
 */
void sys_usb_ext_hook_begin_enumeration(void)
{
        if (!run_usb_cdc_hid_task) {
                run_usb_cdc_hid_task = true;
                usb_cdc_hid_start();
        }
}

/*********************************************************************
 *
 *       sys_usb_ext_hook_detach
 *
 *  Function description
 *    Event callback called from the VBUS event to notify
 *    the application that the USB cable was removed and need to detach.
 */
void sys_usb_ext_hook_detach(void)
{
        if (run_usb_cdc_hid_task) {
                run_usb_cdc_hid_task = false;
                usb_cdc_hid_stop();
        }
}

/******************************************************************************/
/*                                                                            */
/* Common code for the USB CDC to be hooked to the SEGGER library and events. */
/*                                                                            */
/*                            ###   END   ###                                 */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*                             Application Code                               */
/*                                                                            */
/******************************************************************************/

/******************************************************************************
 *
 *       usb_is_suspended
 *
 *  Function description
 *    Callback to indicate that the USB Node is going to be suspended
 */
void usb_is_suspended(void)
{
        debug_print("App: USB Suspend!\r\n", 1);
}

/******************************************************************************
 *
 *       usb_is_resumed
 *
 *  Function description
 *    Callback to indicate that the USB Node is going to be resumed
 */
void usb_is_resumed(void)
{
        debug_print("App: USB Resume!\r\n", 1);
}

/******************************************************************************
 *
 *       usb_hid_task
 *
 */
void usb_hid_task(void *params)
{
        int recv;
        int8_t wdog_id = sys_watchdog_register(false);

        const uint8_t movex[] = { 0x00, 0xf, 0xf, 0x00};

        while (run_usb_cdc_hid_task) {
                /* Notify watchdog on each loop */

                /* Wait for configuration */
                if ((USBD_GetState() & (USB_STAT_CONFIGURED | USB_STAT_SUSPENDED)) != USB_STAT_CONFIGURED) {
                        /* Suspend watchdog while blocking on USBD_HID_Receive */
                        sys_watchdog_suspend(wdog_id);

                        OS_DELAY(50);

                        /* Resume watchdog */
                        sys_watchdog_notify_and_resume(wdog_id);
                        continue;
                }

                /* Suspend watchdog while blocking on USBD_HID_Receive */
                sys_watchdog_suspend(wdog_id);


                /* Resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                // TODO uncomment to make the mouse move
                //USBD_HID_Write(usb_hid_h, movex, sizeof(movex), 0);

                /* Resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);
        }

        sys_watchdog_unregister(wdog_id);

        OS_TASK_DELETE(NULL);


}



/******************************************************************************
 *
 *       usb_cdc_RX_task
 *
 */
void usb_cdc_RX_task(void *params)
{
        unsigned state;
        static __q_item q_item;

#if dg_configUSE_WDOG
        static int8_t usb_cdc_task_wdog_id = -1;

        /* register usb_cdc_task task to be monitored by watchdog */
        usb_cdc_task_wdog_id = sys_watchdog_register(false);
#endif

        while (run_usb_cdc_hid_task) {
#if dg_configUSE_WDOG
                /* notify watchdog on each loop */
                sys_watchdog_notify(usb_cdc_task_wdog_id);
#endif

                /* check for device configuration or suspend state */
                state = USBD_GetState();

                /* USB device is configured and not in suspend state */

                if (((state & USB_STAT_CONFIGURED) == USB_STAT_CONFIGURED) &&
                        ((state & USB_STAT_SUSPENDED) != USB_STAT_SUSPENDED)) {
                        /* Allocate space for one buffer */
                        /* Memory allocation failure is handled by the respective OS hook function */
                        q_item.pdata = OS_MALLOC(USB_MAX_PACKET_SIZE);
#if dg_configUSE_WDOG
                        /* suspend watchdog while blocking on USBD_CDC_Receive */
                        sys_watchdog_suspend(usb_cdc_task_wdog_id);
#endif
                        /* Receive at maximum of USB_MAX_PACKET_SIZE bytes
                         * If less data has been received, should be OK.
                         * If zero or negative is returned then there is a problem */
                        q_item.len = USBD_CDC_Receive(hCDC_Inst, q_item.pdata, USB_MAX_PACKET_SIZE, 0);
#if dg_configUSE_WDOG
                        /* resume watchdog */
                        sys_watchdog_notify_and_resume(usb_cdc_task_wdog_id);
#endif

                        if (q_item.len > 0 && q_item.len <= USB_MAX_PACKET_SIZE) {
                                /* put the received data in the queue
                                 * at this point it can be useful to choose what to do when
                                 * the queue is full.
                                 * Options:
                                 *      - No wait and drop the received data (0)
                                 *        Need to free also the RAM for the q_item.prt in this case if fails to enqueue
                                 *      - Wait for some timeout for the queue to free some space (timeout).
                                 *        Need to free also the RAM for the q_item.prt in this case if fails to enqueue
                                 *      - Block forever until the queue gets some free space */
                                OS_QUEUE_PUT(usb_queue, &q_item, OS_QUEUE_FOREVER);
                        } else {
                                /* if nothing received or an error occurred, then release the RAM allocated
                                 * for the buffer */
                                OS_FREE(q_item.pdata);
                        }

                } else {
                        /* if device is not ready, give it a time and retry */
                        OS_DELAY(50);
                }
        }

        /* signal the TX task that we are done with the queue
         * so it has to exit and clean the queue */
        q_item.len = 0;
        q_item.pdata = NULL;
        OS_QUEUE_PUT(usb_queue, &q_item, OS_QUEUE_FOREVER);

        /* Rx task gives the semaphore to allow the TX task to delete the queue */
        OS_EVENT_SIGNAL(wait_for_rx_task_to_finish);

#if dg_configUSE_WDOG
        /* unregister the watchdog */
        sys_watchdog_unregister(usb_cdc_task_wdog_id);
        /* invalidate the WD ID for the task */
        usb_cdc_task_wdog_id = -1;
#endif

        /* terminate the task */
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());
}



/******************************************************************************
 *
 *       usb_cdc_TX_task
 *
 *       This is the task to receive items from the OS Queue and send them
 *       to the CDC interface
 *
 */
void usb_cdc_TX_task(void *params)
{
        unsigned state;
        static __q_item q_item;

#if dg_configUSE_WDOG
        static int8_t usb_cdc_task_wdog_id = -1;

        /* register usb_cdc_task task to be monitored by watchdog */
        usb_cdc_task_wdog_id = sys_watchdog_register(false);
#endif

        while (run_usb_cdc_hid_task) {
#if dg_configUSE_WDOG
                /* suspend watchdog while blocking on USBD_CDC_Receive */
                sys_watchdog_suspend(usb_cdc_task_wdog_id);
#endif

                /* Receive the next item from the queue */
                if (OS_QUEUE_GET(usb_queue, &q_item, OS_QUEUE_FOREVER) == OS_QUEUE_OK) {
#if dg_configUSE_WDOG
                        /* resume watchdog */
                        sys_watchdog_notify_and_resume(usb_cdc_task_wdog_id);
#endif

                        if (q_item.len > 0) {
                                /* check for device configuration or suspend state */
                                do {
                                        state = USBD_GetState();

                                        if (((state & USB_STAT_CONFIGURED) == USB_STAT_CONFIGURED) &&
                                                ((state & USB_STAT_SUSPENDED) != USB_STAT_SUSPENDED))
                                        {
                                                /* send to CDC interface the received pointer */
                                                USBD_CDC_Write(hCDC_Inst, q_item.pdata, q_item.len, 0);
                                                break;
                                        } else {
                                                /* if device is not ready, give it a time and retry */
                                                OS_DELAY_MS(50);
                                        }
                                } while (run_usb_cdc_hid_task);
                                /* free the used RAM */
                                OS_FREE(q_item.pdata);
                        } else {
                                /* Receiving a queue item with zero length is the signal to end the task
                                 * This is application specific. The developer can choose any exit
                                 * condition is most suitable for their application. */
                                break;
                        }

                }
        }

        /* TX task takes the semaphore to make sure the RX task is terminated and done with the queue */
        OS_EVENT_WAIT(wait_for_rx_task_to_finish, OS_EVENT_FOREVER);

        /* release the space allocated for the queue items q_item.pdata */
        uint32_t q_items = OS_QUEUE_MESSAGES_WAITING(usb_queue);
        for (uint32_t i = 0; i < q_items; i++) {
                if (OS_QUEUE_GET(usb_queue, &q_item, 0) == OS_QUEUE_OK) {
                        /* release any possible allocated space for
                         * the queue entry */
                        OS_FREE(q_item.pdata);
                }
        }

        /* delete the queue */
        OS_QUEUE_DELETE(usb_queue);
        /* invalidate the queue handler */
        usb_queue = NULL;

        /* Delete the semaphore. It will be created again on the next USB-plugin */
        OS_EVENT_DELETE(wait_for_rx_task_to_finish);

#if dg_configUSE_WDOG
        /* unregister the watchdog */
        sys_watchdog_unregister(usb_cdc_task_wdog_id);
        /* invalidate the WD ID for the task */
        usb_cdc_task_wdog_id = -1;
#endif

        /* terminate the task */
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());
}
