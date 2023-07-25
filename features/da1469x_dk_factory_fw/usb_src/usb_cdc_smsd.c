/**
 ****************************************************************************************
 *
 * @file usb_cdc_vmsd.c
 *
 * @brief USB CDC and VirtualMSD app implementation
 *
 * Copyright (C) 2016-2021 Renesas Electronics Corporation and/or its affiliates
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
#include "ad_nvms.h"
#include "sys_charger.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"
#include "hw_usb.h"
#include "USB_CDC.h"
#include "USB_MSD.h"
#include "USB_VirtualMSD.h"
#include "sys_tcs.h"
#include "readme_html.h"

/*********************************************************************
 *
 *       Defines, configurable
 *
 **********************************************************************
 */
#define VMSD_USE_NVMS                           //Use NVMS or RAM for data
#define VMSD_DATA_SIZE 2*1024                   //file size
#define _VIRTUALMSD_NUM_SECTORS (32+512*2)        //8kbyte
#define MAX_CONST_FILE 1
#define APP_FILE_HEADER "FWBIN"
#define usb_main_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

__RETAINED static OS_TASK usb_cdc_task_handle;
__RETAINED static OS_TASK usb_vmsd_task_handle;
__RETAINED static uint8 run_usb_task;
static uint8 _ReattchRequest;
static USB_HOOK UsbpHook;
static char usb_cdc_buf[USB_MAX_PACKET_SIZE];
static bool isFwFile;

//
//  Information that is used during enumeration.
//
static const USB_DEVICE_INFO _DeviceInfo = {
        0x2DCF,                                   // VendorId
        0x6003,                                   // ProductId
        "Dialog Semiconductor",                   // VendorName
        "DA1469x VirtualMSD device",              // ProductName
        "0123456789AB"                            // SerialNumber. Should be 12 character or more for compliance with Mass Storage Device Bootability spec.
};
//
// String information used when inquiring the volume 0.
//
static const USB_MSD_LUN_INFO _Lun0Info = {
        "Vendor",                                // MSD VendorName
        "MSD Volume",                            // MSD ProductName
        "1.00",                                  // MSD ProductVer
        "134657890"                              // MSD SerialNo
};

//
// Remaining bytes of sector are filled with 0s on read, if a file does not occupy complete sectors
//
#ifndef VMSD_USE_NVMS
static U8 vmsd_log_dat[VMSD_DATA_SIZE] = {
        0x4C,0x4F,0x47,0x2E,0x44,0x41,0x54,
};

static U8 vmsd_data_dat[VMSD_DATA_SIZE] = {
        0x44,0x41,0x54,0x41,0x2E,0x44,0x41,0x54,
};

static U8 vmsd_app_dat[VMSD_DATA_SIZE] = {
        0x41,0x50,0x50,0x2E,0x44,0x41,0x54,
};
#endif

//
// Constant files which should be displayed on the virtual volume.
//
static USB_VMSD_CONST_FILE _aConstFiles[MAX_CONST_FILE] = {
        //     sName                     pData                       FileSize                      Flags
        { "LOG.DAT", NULL, 0, 0, },
};

#ifdef VMSD_USE_NVMS
static int usb_read_from_nvms(nvms_partition_id_t nv_id, uint8* pData, uint32 Off, uint32 Numbytes)
{
        nvms_t nvms;

        nvms = ad_nvms_open(nv_id);
        if (nvms) {
                ad_nvms_read(nvms, Off, pData, Numbytes);
                return 0;
        }
        return -1;
}

static int usb_write_to_nvms(nvms_partition_id_t nv_id, uint8* pData, uint32 Off, uint32 Numbytes)
{
        nvms_t nvms;

        nvms = ad_nvms_open(nv_id);
        if (nvms) {
                ad_nvms_write(nvms, Off, pData, Numbytes);
                ad_nvms_flush(nvms, true);
                return 0;
        }
        return -1;
}
#endif
/*********************************************************************
 *
 *       _cbOnWrite
 *
 *  Parameters
 *    Lun       LUN ID
 *    pData     Data to be written
 *    Off       Offset into current file to be written
 *    NumBytes  Number of bytes to write into the file
 *    pFile     *Optional* Additional information about the file being written (RootDir entry etc.)
 */
static int _cbOnWrite(unsigned Lun, const U8* pData, U32 Off, U32 NumBytes,
        const USB_VMSD_FILE_INFO* pFile)
{
        int write_bytes;

        if (NumBytes == 0) {
                return 0;
        }

        if ((Off == 0 && memcmp(APP_FILE_HEADER, (const void *)pData, sizeof(APP_FILE_HEADER) - 1) != 0) ||
                (Off > 0 && !isFwFile)) {
                isFwFile = false;
                _ReattchRequest = 1;

                //Make the device read-only
                USBD_MSD_UpdateWriteProtect(0,1);

                USBD_MSD_RequestRefresh(0, USB_MSD_RE_ATTACH | USB_MSD_TRY_DISCONNECT);
                return -1;
        }

        if (Off == 0) {
                isFwFile = true;
        }

        if ((Off + NumBytes) > VMSD_DATA_SIZE) {
                write_bytes = VMSD_DATA_SIZE - Off;
        } else {
                write_bytes = NumBytes;
        }

        if ((Off + write_bytes) <= VMSD_DATA_SIZE) {
#ifdef VMSD_USE_NVMS
                usb_write_to_nvms(NVMS_GENERIC_PART, (uint8 *)pData, Off, write_bytes);
#else
                memcpy(vmsd_app_dat + Off, pData, write_bytes);
#endif
        }
        else {
                _ReattchRequest = 1;
                USBD_MSD_RequestRefresh(0, USB_MSD_RE_ATTACH | USB_MSD_TRY_DISCONNECT);
                return -1;
        }

        return 0;
}

/*********************************************************************
 *
 *       _cbOnRead
 *
 *  Parameters
 *    Lun       LUN ID
 *    pData     Data which will be sent to the host
 *    Off       Offset of the current file requested by the host
 *    NumBytes  Number of bytes to read
 *    pFile     *Optional* Additional information about the file being written (RootDir entry etc.)
 */
static int _cbOnRead(unsigned Lun, U8* pData, U32 Off, U32 NumBytes,
        const USB_VMSD_FILE_INFO* pFile)
{
        (void)Lun;
        (void)Off;
        (void)NumBytes;
        (void)pFile;

        if (NumBytes == 0 || pData == 0)
                return 0;

        if ((strncmp("APP", (char *)pFile->pDirEntry->acFilename, 3) == 0)
                && (strncmp("DAT", (char *)pFile->pDirEntry->acExt, 3) == 0)) {
#ifdef VMSD_USE_NVMS
                usb_read_from_nvms(NVMS_GENERIC_PART, (uint8 *)pData, Off, NumBytes);
#else
                memcpy((char *)pData, vmsd_app_dat+Off, NumBytes);
#endif
        }
        else if ((strncmp("LOG", (char *)pFile->pDirEntry->acFilename, 3) == 0)
                && (strncmp("DAT", (char *)pFile->pDirEntry->acExt, 3) == 0)) {
#ifdef VMSD_USE_NVMS
                usb_read_from_nvms(NVMS_LOG_PART, (uint8 *)pData, Off, NumBytes);
#else
                memcpy((char *)pData, vmsd_log_dat+Off, NumBytes);
#endif
        }
        else if ((strncmp("DATA", (char *)pFile->pDirEntry->acFilename, 4) == 0)
                && (strncmp("DAT", (char *)pFile->pDirEntry->acExt, 3) == 0)) {
#ifdef VMSD_USE_NVMS
                usb_read_from_nvms(NVMS_PARAM_PART, (uint8 *)pData, Off, NumBytes);
#else
                memcpy((char *)pData, vmsd_data_dat+Off, NumBytes);
#endif
        }

        return 0;
}

static void *_memAlloc(U32 size)
{
        return OS_MALLOC(size);
}

static void _memFree(void *p)
{
        OS_FREE(p);
}

static const USB_VMSD_USER_FUNC_API _UserFuncAPI = {
        _cbOnRead,     // pfOnRead    -> Is called when a sector of a given file is read.
        _cbOnWrite,    // pfOnWrite   -> Is called when a sector of a given file is written.
        _memAlloc,     // pfMemAlloc  -> Optional, can be set in order to allow the VirtualMSD to share the mem alloc function of a system.
        _memFree       // pfMemFree   -> Optional, can be set in order to allow the VirtualMSD to share the mem free function of a system.
};

/*********************************************************************
 *
 *       USB_VMSD_X_Config
 *
 *  Function description
 *    This function is called by the USB MSD Module during USB_VirtualMSD_Init() and initializes the VirtualMSD volume.
 */
void USB_VMSD_X_Config(void)
{
        //
        // Global configuration
        //
        USBD_VMSD_SetUserAPI(&_UserFuncAPI);

        //
        // Setup LUN0
        //
        USBD_VMSD_SetNumSectors(0, _VIRTUALMSD_NUM_SECTORS);
        USBD_VMSD_SetSectorsPerCluster(0, 4); // Anywhere from 1 ... 128, but needs to be a Power of 2
        USBD_VMSD_SetNumRootDirSectors(0, 2);
        USBD_VMSD_SetVolumeInfo(0, "DA1469x.Kit", &_Lun0Info);   // Add volume ID

        _aConstFiles[0].FileSize = sizeof(html_file) -1;
        _aConstFiles[0].sName = "Readme.html";
        _aConstFiles[0].Flags = 0;
        _aConstFiles[0].pData = (U8 *)html_file;

        USBD_VMSD_AddConstFiles(0, &_aConstFiles[0], SEGGER_COUNTOF(_aConstFiles)); // Push const file to the volume
}

uint32_t USB_VMSD_FS_BootSector_VolID_Config(void)
{
        /* Use product info as volume id of the VMSD device */
        uint32_t *variant = NULL;
        uint8_t size = 0;

        sys_tcs_get_custom_values(SYS_TCS_GROUP_PROD_INFO, &variant, &size);

        // If the Device Variant entry has been successfully retrieved
        if ((size == 3) && (variant != NULL)) {
                return *variant;
        }

        return 0;
}

void usb_cdc_vmsd_state_cb(void * pContext, U8 NewState)
{
        if (NewState & USB_STAT_ATTACHED) {
                //Attached
        }

        if (NewState & USB_STAT_READY) {
                //Ready
        }

        if (NewState & USB_STAT_ADDRESSED) {
                //Addressed
        }

        if (NewState & USB_STAT_CONFIGURED) {
                //Configured
        }

        if (NewState & USB_STAT_SUSPENDED) {
                //Suspended
        }
}

/*********************************************************************
 *
 *       Usb_cdc_eco_task
 *
 */
void usb_cdc_eco_task(void *params)
{
        USB_CDC_HANDLE hInst;
        int NumBytesReceived;
#if dg_configUSE_WDOG
        int8_t wdog_id;

        wdog_id = sys_watchdog_register(false);
#endif
        hInst = *(USB_CDC_HANDLE *)params;

        while (1) {
#if dg_configUSE_WDOG
                /* notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);
#endif

                //
                // Wait for configuration
                //
                while ((USBD_GetState() & (USB_STAT_CONFIGURED | USB_STAT_SUSPENDED))
                        != USB_STAT_CONFIGURED) {
                        OS_DELAY(50);
                }

#if dg_configUSE_WDOG
                /* suspend watchdog while blocking on USBD_CDC_Receive */
                sys_watchdog_suspend(wdog_id);
#endif
                //
                // Receive at maximum of sizeof(usb_cdc_buf) Bytes
                // If less data has been received,
                // this should be OK.
                //
                NumBytesReceived = USBD_CDC_Receive(hInst, usb_cdc_buf, sizeof(usb_cdc_buf), 0);
#if dg_configUSE_WDOG
                /* resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);
#endif

                if (NumBytesReceived > 0) {
                        //USBD_CDC_Write(hInst, usb_cdc_buf, NumBytesReceived, 0);
                }
        }
}

void usb_vmsd_task(void *params)
{

        USBD_Init();
        USBD_EnableIAD();
        USBD_VMSD_Init();
        USBD_RegisterSCHook(&UsbpHook, usb_cdc_vmsd_state_cb, NULL);
        USBD_VMSD_Add();
        USBD_SetDeviceInfo(&_DeviceInfo);
        USBD_MSD_UpdateWriteProtect(0,1);
#if ( dg_configUSE_SYS_CHARGER == 1 )
        /* Request the minimum guaranteed current budget. */
        USBD_SetMaxPower(hw_charger_i_level_to_miliamp(HW_CHARGER_I_LEVEL_500));
#endif
        USBD_Start();


        while (1) {
                //
                // Wait for configuration
                //
                while ((USBD_GetState() & (USB_STAT_CONFIGURED | USB_STAT_SUSPENDED))
                        != USB_STAT_CONFIGURED) {
                        USB_OS_Delay(50);
                }

                USBD_MSD_Task();
                if (_ReattchRequest) {
                        USB_VMSD_ReInit();
                        _ReattchRequest = 0;
                }
        }
        OS_TASK_DELETE(OS_GET_CURRENT_TASK());
}

void usb_cdc_vmsd_start()
{
        OS_BASE_TYPE status;

        /* Start the USB VirtualMSD application task. */
        status = OS_TASK_CREATE("UsbVmsdTask",  /* The text name assigned to the task, for
                                                   debug only; not used by the kernel. */
                        usb_vmsd_task,          /* The function that implements the task. */
                        NULL,                   /* The parameter passed to the task. */
                        4096,                   /* The number of bytes to allocate to the
                                                                     stack of the task. */
                        usb_main_TASK_PRIORITY, /* The priority assigned to the task. */
                        usb_vmsd_task_handle);  /* The task handle. */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);
}

void usb_cdc_vmsd_stop()
{
        USBD_UnregisterSCHook(&UsbpHook);
        USBD_DeInit();
        USBD_Stop();
        OS_TASK_DELETE(usb_cdc_task_handle);
        OS_TASK_DELETE(usb_vmsd_task_handle);
        run_usb_task = 0;
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
        if (run_usb_task == 0) {
                _ReattchRequest = 0;
                run_usb_task = 1;
                isFwFile = false;

                usb_cdc_vmsd_start();

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
void  sys_usb_ext_hook_detach(void)
{
        if (run_usb_task == 1) {
                usb_cdc_vmsd_stop();
        }
}
