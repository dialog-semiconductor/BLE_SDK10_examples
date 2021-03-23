/**
 ****************************************************************************************
 *
 * @file usb_cdc_smsd.c
 *
 * @brief USB CDC and SmartMSD app implementation
 *
 * Copyright (C) 2016-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include "ad_nvms.h"
#include "sys_charger.h"
#include "sys_power_mgr.h"
#include "sys_watchdog.h"
#include "hw_usb.h"
#include "USB_CDC.h"
#include "USB_MSD.h"
#include "USB_SmartMSD.h"
#include "readme_html.h"


/*********************************************************************
 *
 *       Defines, configurable
 *
 **********************************************************************
 */
#define SMSD_USE_NVMS //Use NVMS or RAM for data
#define SMSD_DATA_SIZE 2*1024 //file size
#define _SMARTMSD_NUM_SECTORS (32+512*2) //8kbyte
#define MAX_CONST_FILE 1
#define APP_FILE_HEADER "FWBIN"
#define usb_main_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )

__RETAINED static OS_TASK usb_cdc_task_handle;
__RETAINED static OS_TASK usb_smsd_task_handle;
__RETAINED static uint8 run_usb_task/* = 0*/;
static uint8 _ReattchRequest;
static USB_HOOK UsbpHook;
static char usb_cdc_buf[USB_MAX_PACKET_SIZE];
static bool isFwFile;

//
//  Information that is used during enumeration.
//
static const USB_DEVICE_INFO _DeviceInfo = {
        0x2DCF,         // VendorId
        0x6003,         // ProductId
        "Dialog Semiconductor",       // VendorName
        "DA1469x CDC/SmartMSD device",  // ProductName
        "0123456789AB" // SerialNumber. Should be 12 character or more for compliance with Mass Storage Device Bootability spec.
        };
//
// String information used when inquiring the volume 0.
//
static const USB_MSD_LUN_INFO _Lun0Info = {
        "Vendor",     // MSD VendorName
        "MSD Volume", // MSD ProductName
        "1.00",       // MSD ProductVer
        "134657890"   // MSD SerialNo
        };

//
// Remaining bytes of sector are filled with 0s on read, if a file does not occupy complete sectors
//
#ifndef SMSD_USE_NVMS
static U8 smsd_log_dat[SMSD_DATA_SIZE] = {
        0x4C,0x4F,0x47,0x2E,0x44,0x41,0x54,
};

static U8 smsd_data_dat[SMSD_DATA_SIZE] = {
        0x44,0x41,0x54,0x41,0x2E,0x44,0x41,0x54,
};

static U8 smsd_app_dat[SMSD_DATA_SIZE] = {
        0x41,0x50,0x50,0x2E,0x44,0x41,0x54,
};
#endif

//
// Constant files which should be displayed on the virtual volume.
//
static USB_SMSD_CONST_FILE _aConstFiles[MAX_CONST_FILE] = {
        //     sName                     pData                       FileSize                      Flags
        { "LOG.DAT", NULL, 0, 0, },
};

#ifdef SMSD_USE_NVMS
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
        const USB_SMSD_FILE_INFO* pFile)
{
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
        const USB_SMSD_FILE_INFO* pFile)
{
        (void)Lun;
        (void)Off;
        (void)NumBytes;
        (void)pFile;

        if (NumBytes == 0 || pData == 0)
                return 0;

        if ((strncmp("APP", (char *)pFile->pDirEntry->acFilename, 3) == 0)
                && (strncmp("DAT", (char *)pFile->pDirEntry->acExt, 3) == 0)) {
#ifdef SMSD_USE_NVMS
                usb_read_from_nvms(NVMS_GENERIC_PART, (uint8 *)pData, Off, NumBytes);
#else
                memcpy((char *)pData, smsd_app_dat+Off, NumBytes);
#endif
        }
        else if ((strncmp("LOG", (char *)pFile->pDirEntry->acFilename, 3) == 0)
                && (strncmp("DAT", (char *)pFile->pDirEntry->acExt, 3) == 0)) {
#ifdef SMSD_USE_NVMS
                usb_read_from_nvms(NVMS_LOG_PART, (uint8 *)pData, Off, NumBytes);
#else
                memcpy((char *)pData, smsd_log_dat+Off, NumBytes);
#endif
        }
        else if ((strncmp("DATA", (char *)pFile->pDirEntry->acFilename, 4) == 0)
                && (strncmp("DAT", (char *)pFile->pDirEntry->acExt, 3) == 0)) {
#ifdef SMSD_USE_NVMS
                usb_read_from_nvms(NVMS_PARAM_PART, (uint8 *)pData, Off, NumBytes);
#else
                memcpy((char *)pData, smsd_data_dat+Off, NumBytes);
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

static const USB_SMSD_USER_FUNC_API _UserFuncAPI = {
        _cbOnRead,     // pfOnRead    -> Is called when a sector of a given file is read.
        _cbOnWrite,    // pfOnWrite   -> Is called when a sector of a given file is written.
        _memAlloc, // pfMemAlloc  -> Optional, can be set in order to allow the SmartMSD to share the mem alloc function of a system.
        _memFree // pfMemFree   -> Optional, can be set in order to allow the SmartMSD to share the mem free function of a system.
        };

/*********************************************************************
 *
 *       USB_SMSD_X_Config
 *
 *  Function description
 *    This function is called by the USB MSD Module during USB_SmartMSD_Init() and initializes the SmartMSD volume.
 */
void USB_SMSD_X_Config(void)
{
        //
        // Global configuration
        //
        USBD_SMSD_SetUserAPI(&_UserFuncAPI);

        //
        // Setup LUN0
        //
        USBD_SMSD_SetNumSectors(0, _SMARTMSD_NUM_SECTORS);
        USBD_SMSD_SetSectorsPerCluster(0, 4); // Anywhere from 1 ... 128, but needs to be a Power of 2
        USBD_SMSD_SetNumRootDirSectors(0, 2);
        USBD_SMSD_SetVolumeInfo(0, "DA1469x.Kit", &_Lun0Info);   // Add volume ID

        _aConstFiles[0].FileSize = sizeof(html_file) -1;
        _aConstFiles[0].sName = "Readme.html";
        _aConstFiles[0].Flags = 0;
        _aConstFiles[0].pData = (U8 *)html_file;

        USBD_SMSD_AddConstFiles(0, &_aConstFiles[0], SEGGER_COUNTOF(_aConstFiles)); // Push const file to the volume
}


void usb_cdc_smsd_state_cb(void * pContext, U8 NewState)
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

void usb_smsd_task(void *params)
{
//        USB_CDC_HANDLE hInst;
//        OS_BASE_TYPE status;

        USBD_Init();
//        USBD_CDC_Init();
        USBD_SMSD_Init();
        USBD_RegisterSCHook(&UsbpHook, usb_cdc_smsd_state_cb, NULL);
        USBD_EnableIAD();
//        hInst = _AddCDC();
        USBD_SMSD_Add();
        USBD_SetDeviceInfo(&_DeviceInfo);
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
                        USB_SMSD_ReInit();
                        _ReattchRequest = 0;
                }
        }
}

void usb_cdc_smsd_start()
{
        OS_BASE_TYPE status;

        /* Start the USB SmartMSD application task. */
        status = OS_TASK_CREATE("UsbSmsdTask",  /* The text name assigned to the task, for
                                                   debug only; not used by the kernel. */
                        usb_smsd_task,          /* The function that implements the task. */
                        NULL,                   /* The parameter passed to the task. */
                        2048, /* The number of bytes to allocate to the
                                                     stack of the task. */
                        usb_main_TASK_PRIORITY, /* The priority assigned to the task. */
                        usb_smsd_task_handle);  /* The task handle. */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);
}

void usb_cdc_smsd_stop()
{
        USBD_UnregisterSCHook(&UsbpHook);
        USBD_DeInit();
        OS_TASK_DELETE(usb_cdc_task_handle);
        OS_TASK_DELETE(usb_smsd_task_handle);
}

/*********************************************************************
 *
 *       usb_start_enumeration_cb
 *
 *  Function description
 *    Event callback called from the usbcharger task to notify
 *    the application about to allow enumeration.
 *    Note: The USB charger task is started before the application task. Thus, these
 *          call-backs may be called before the application task is started.
 *          The application code should handle this case, if need be.
 */
void sys_usb_ext_hook_begin_enumeration(void)
{
        if (run_usb_task == 0) {
                _ReattchRequest = 0;
                run_usb_task = 1;
                isFwFile = false;

                usb_cdc_smsd_start();

        }

}
/*********************************************************************
 *
 *       usb_detach_cb
 *
 *  Function description
 *    Event callback called from the usbcharger task to notify
 *    the application that a detach of the USB cable was detected.
 *
 *    Note: The USB charger task is started before the application task. Thus, these
 *          call-backs may be called before the application task is started.
 *          The application code should handle this case, if need be.
 */
void  sys_usb_ext_hook_detach(void)
{
        if (run_usb_task == 1) {
                usb_cdc_smsd_stop();
                run_usb_task = 0;
        }
}
