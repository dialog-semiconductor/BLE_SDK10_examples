/**
 ****************************************************************************************
 *
 * @file dsps_usbd.c
 *
 * @brief USB CDC device operations
 *
* Copyright (c) 2023 Renesas Electronics Corporation and/or its affiliates
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
#if defined(DSPS_USBD)

#include <string.h>
#include "osal.h"
#include "dsps_usbd.h"
#include "USB.h"
#include "USB_CDC.h"

__RETAINED_RW volatile static uint8 run_usb_cdc_task = false;
__RETAINED_RW static USB_CDC_HANDLE hCDC_Inst = -1;

/* Buffer assigned to the OUT EP */
__RETAINED static U8 _abOutBuffer[USBD_CDC_RX_BUF_SIZE];

static USB_HOOK UsbpHook;

/* Information that is used during enumeration. */
static const USB_DEVICE_INFO _DeviceInfo = {
        0x2DCF,                  // VendorId
        0x6002,                  // ProductId
        "Dialog Semiconductor",  // VendorName
        "DA1469x CDC HID",       // ProductName
        "12345678"               // SerialNumber.
};

/*********************************************************************
 *
 *       _AddCDC
 *
 *  Function description
 *    Add communication device class to USB stack
 */
static USB_CDC_HANDLE _AddCDC(void)
{
        USB_CDC_HANDLE hInst;
        USB_CDC_INIT_DATA InitData;

        InitData.EPIn = USBD_AddEP(USB_DIR_IN, USB_TRANSFER_TYPE_BULK, 0, NULL, 0);
        ASSERT_WARNING(InitData.EPIn > 0);
        InitData.EPOut = USBD_AddEP(USB_DIR_OUT, USB_TRANSFER_TYPE_BULK, 0, _abOutBuffer,
                USB_MAX_PACKET_SIZE);
        ASSERT_WARNING(InitData.EPOut > 0);
        InitData.EPInt = USBD_AddEP(USB_DIR_IN, USB_TRANSFER_TYPE_INT, 8, NULL, 0);
        ASSERT_WARNING(InitData.EPInt > 0);

        /* The handle of the first CDC instance is always zero */
        hInst = USBD_CDC_Add(&InitData);
        ASSERT_WARNING(hInst == 0);

        return hInst;
}

static void usb_cdc_state_cb(void * pContext, U8 NewState)
{
        int OldState = USBD_GetState();

        if (((OldState & USB_STAT_ATTACHED) == 0) && (NewState & USB_STAT_ATTACHED)) {
                // Attached
        }

        if ((OldState & USB_STAT_ATTACHED) && ((NewState & USB_STAT_ATTACHED) == 0)) {
                // Detached
        }

        if (((OldState & USB_STAT_READY) == 0) && (NewState & USB_STAT_READY)) {
                // Ready
        }

        if ((OldState & USB_STAT_READY) && ((NewState & USB_STAT_READY) == 0)) {
                // Un-Ready
        }

        if (((OldState & USB_STAT_ADDRESSED) == 0) && (NewState & USB_STAT_ADDRESSED)) {
                // Addressed
        }

        if ((OldState & USB_STAT_ADDRESSED) && ((NewState & USB_STAT_ADDRESSED) == 0)) {
                // Un-Addressed
        }

        if (((OldState & USB_STAT_CONFIGURED) == 0) && (NewState & USB_STAT_CONFIGURED)) {
                // Configured
        }

        if ((OldState & USB_STAT_CONFIGURED) && ((NewState & USB_STAT_CONFIGURED) == 0)) {
                // Un-Configured
        }

        if (((OldState & USB_STAT_SUSPENDED) == 0) && (NewState & USB_STAT_SUSPENDED)) {
                // USB is going to be Suspended - DO NOT USE THIS POINT TO TRIGGER APP CODE!
        }

        if ((OldState & USB_STAT_SUSPENDED) && ((NewState & USB_STAT_SUSPENDED) == 0)) {
                // USB is going to be Resumed - DO NOT USE THIS POINT TO TRIGGER APP CODE!
        }
}

USB_CDC_HANDLE cdc_usbd_get_handle(void)
{
        return hCDC_Inst;
}

static void cdc_usbsd_close(void)
{
        USBD_UnregisterSCHook(&UsbpHook);

        USBD_DeInit();
        USBD_Stop();
}

static void cdc_usbd_open(void)
{
        USBD_Init();
        USBD_EnableIAD();
        USBD_CDC_Init();
        USBD_RegisterSCHook(&UsbpHook, usb_cdc_state_cb, NULL);
        hCDC_Inst = _AddCDC();
        USBD_SetDeviceInfo(&_DeviceInfo);
        USBD_SetAllowRemoteWakeUp(1);

#if (dg_configUSE_USB_ENUMERATION == 1)
# if (dg_configUSE_SYS_CHARGER == 1)
        /* Request the minimum guaranteed current budget. */
        USBD_SetMaxPower(hw_charger_i_level_to_miliamp(HW_CHARGER_I_LEVEL_500));
# endif /* dg_configUSE_SYS_CHARGER */
#endif /* dg_configUSE_USB_ENUMERATION */

        USBD_Start();
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
        if (!run_usb_cdc_task) {
                run_usb_cdc_task = true;
                cdc_usbd_open();
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
        if (run_usb_cdc_task) {
                run_usb_cdc_task = false;
                cdc_usbsd_close();
        }
}

int read_from_cdc_usbd(USB_CDC_HANDLE handle, void *buf, unsigned len, int timeout)
{
        ASSERT_WARNING(buf != NULL);

        int ret = 0;
        unsigned state;

        state = USBD_GetState();

        /* USB device is configured and not in suspend state */
        if (((state & USB_STAT_CONFIGURED) == USB_STAT_CONFIGURED) &&
                ((state & USB_STAT_SUSPENDED) != USB_STAT_SUSPENDED)) {
                /*
                 * \note The following API will return upon receiving the first packet and should
                 *       not wait for all the requested bytes to be read as UCBD_CDC_Read();
                 *       which in turn should decrease the SPS throughput measured.
                 */
                ret = USBD_CDC_Receive(handle, buf, len, timeout);
        }
        return ret;
}

int write_to_cdc_usbd(USB_CDC_HANDLE handle, const void *buf, unsigned len, int timeout)
{
        ASSERT_WARNING(buf != NULL);

        int ret = 0;
        unsigned state;

        state = USBD_GetState();

        /* USB device is configured and not in suspend state */
        if (((state & USB_STAT_CONFIGURED) == USB_STAT_CONFIGURED) &&
                ((state & USB_STAT_SUSPENDED) != USB_STAT_SUSPENDED)) {
                ret = USBD_CDC_Write(handle, buf, len, timeout);
        }
        return ret;
}

void set_flow_on_cdc_usbd(USB_CDC_HANDLE handle)
{
        (void)handle;

}

void set_flow_off_cdc_usbd(USB_CDC_HANDLE handle)
{
        (void)handle;
}
#endif
