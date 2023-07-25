/**
 ****************************************************************************************
 *
 * @file usb_cdc_smsd.c
 *
 * @brief USB CDC and SmartMSD app implementation
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
#include "USB.h"
#include "USB_MSD.h"

#include "ff.h"

#if ( dg_configUSE_SYS_CHARGER == 1 )
#include "custom_charging_profile.h"
#endif

/*********************************************************************
 *
 *       Defines, configurable
 *
 **********************************************************************
 */
#define usb_main_TASK_PRIORITY          ( OS_TASK_PRIORITY_NORMAL )

__RETAINED static OS_TASK usb_msd_task_handle;
__RETAINED_RW volatile static uint8 run_usb_task = 0;

/* No need to declare them __RETAINED because when USB is plugged-in the
 * sleep/wake is suspended and the device is always active
 */
static U8 _abOutBuffer[USB_MAX_PACKET_SIZE];
USB_MSD_INIT_DATA InitializationData;
USB_MSD_INST_DATA InstanceData;
uint32_t temp_sector_buffer[FLASH_SECTOR_SIZE/4];

//  Information that is used during enumeration.
static const USB_DEVICE_INFO _DeviceInfo = {
        0x2DCF,                         // VendorId
        0x6004,                         // ProductId
        "Dialog Semiconductor",         // VendorName
        "DA1469x MSD",                  // ProductName
        "0123456789AB"                  // SerialNumber. Should be 12 character or more for compliance with Mass Storage Device Bootability spec.
        };

// String information used when inquiring the volume 0.
static const USB_MSD_LUN_INFO _Lun0Info = {
        "Dialog Semiconductor",         // MSD VendorName
        "DA1469x MSD",                  // MSD ProductName
        "1.00",                         // MSD ProductVer
        "134657890"                     // MSD SerialNo
        };

#define MEDIUM_REFRESH_NOTIFY                   (1<<0)
#define EXIT_MEDIUM_REFRESH_NOTIFY              (1<<1)
#define MEDIUM_REFRESH_TIMEOUT                  (100) //in ms

/*********************************************************************
 *
 *  msd_storage_Init
 *
 *  Function description
 *    Initializes the storage medium.
 *    This is a callback required by the SEGGER MSD class implementation
 */
static void msd_storage_Init( U8 Lun, const USB_MSD_INST_DATA_DRIVER * pDriverData )
{
        ASSERT_WARNING(Lun==0);
}

/*********************************************************************
 *
 *  msd_storage_DeInit
 *
 *  Function description
 *    Initializes the storage medium.
 *    This is a callback required by the SEGGER MSD class implementation
 */
static void msd_storage_DeInit( U8 Lun )
{
        ASSERT_WARNING(Lun==0);
}

/*********************************************************************
 *
 *  msd_storage_GetInfo
 *
 *  Function description
 *    Retrieves storage medium information such as sector size
 *    and number of sectors available..
 *    This is a callback required by the SEGGER MSD class implementation
 */
static void msd_storage_GetInfo(U8 Lun, USB_MSD_INFO * pInfo)
{
        ASSERT_WARNING(Lun==0);

        /* Locate and read the info of the FATFS_PARTITION */
        partition_entry_t part_info;
        uint32_t partitions_count = ad_nvms_get_partition_count();

        for (int idx=0; idx<partitions_count; idx++) {
                ad_nvms_get_partition_info(idx, &part_info);
                if (part_info.type == NVMS_FATFS_PART)  break;
        }

        pInfo->NumSectors = part_info.sector_count;
        pInfo->SectorSize = FLASH_SECTOR_SIZE;
}

/*********************************************************************
 *
 *  msd_storage_GetReadBuffer
 *
 *  Function description
 *    Prepares read function and returns a pointer to a buffer that
 *    is used by the storage driver.
 *    This is a callback required by the SEGGER MSD class implementation
 */
static U32 msd_storage_GetReadBuffer(U8 Lun, U32 SectorIndex, void ** ppData, U32 NumSectors)
{
        ASSERT_WARNING(Lun==0);
        /* Since we are using the automode for read, we can return the actual
         * address of the FLASH where it wants to read and avoid all the
         * memory allocation.
         * We need to calculate the pointer and return the number of sectors requested.
         */

        ad_nvms_get_pointer(ad_nvms_open(FATFS_PARTITION_NAME),
                            SectorIndex * FLASH_SECTOR_SIZE,
                            NumSectors * FLASH_SECTOR_SIZE,
                            (const void **)ppData);

        return NumSectors;
}

/*********************************************************************
 *
 *  msd_storage_Read
 *
 *  Function description
 *    Reads one or multiple sectors from the storage medium.
 *    This is a callback required by the SEGGER MSD class implementation
 */
static int8_t msd_storage_Read(U8 Lun, U32 SectorIndex, void * pData, U32 NumSectors)
{
        ASSERT_WARNING(Lun==0);
        const void* flash_pData;

        /* Since we are using the automode for read, we can use the actual
         * address of the FLASH where it wants to read.
         */
        ad_nvms_get_pointer(ad_nvms_open(FATFS_PARTITION_NAME), SectorIndex * FLASH_SECTOR_SIZE, NumSectors * FLASH_SECTOR_SIZE, &flash_pData);

        if (pData != flash_pData) {
                /* then we can copy to provided buffer
                 * Use the tool-chain optimized copy for
                 * better performance */
                __aeabi_memcpy(pData, flash_pData, NumSectors*FLASH_SECTOR_SIZE);
        }

        return 0;
}

/*********************************************************************
 *
 *  msd_storage_GetWriteBuffer
 *
 *  Function description
 *    Prepares write function and returns a pointer to a buffer that
 *    is used by the storage driver.
 *    This is a callback required by the SEGGER MSD class implementation
 */
static U32 msd_storage_GetWriteBuffer(U8 Lun, U32 SectorIndex, void ** ppData, U32 NumSectors)
{
        ASSERT_WARNING(Lun==0);
        printf("Request write buffer for %ld sectors\n",NumSectors);
        fflush(stdout);
        /* We can reuse the buffer declared in the MSD driver
         * It can be a separate buffer but this saves RAM space */
        *ppData = (void*)OS_MALLOC(NumSectors * FLASH_SECTOR_SIZE);
        return 1;
}

/*********************************************************************
 *
 *  msd_storage_Write
 *
 *  Function description
 *    Writes one or more sectors to the storage medium.
 *    This is a callback required by the SEGGER MSD class implementation
 */
static int8_t msd_storage_Write(U8 Lun, U32 SectorIndex, const void * pData, U32 NumSectors)
{
        ASSERT_WARNING(Lun==0);
        nvms_t nvms;

        nvms = ad_nvms_open(FATFS_PARTITION_NAME);

        if (nvms) {
                /* Write to designated partition the raw data from the Host */
                ad_nvms_write(nvms, SectorIndex * FLASH_SECTOR_SIZE, (uint8_t*)pData, NumSectors * FLASH_SECTOR_SIZE);
                OS_FREE((void *)pData);

                return 0;
        }

        OS_FREE((void *)pData);

        /* There is no designated partition for the MSD Storage */
        return -1;
}

/*********************************************************************
 *
 *  msd_storage_Write
 *
 *  Function description
 *    Checks if medium is present.
 *    This is a callback required by the SEGGER MSD class implementation
 */
static int8_t msd_storage_MediumIsPresent(U8 Lun)
{
        ASSERT_WARNING(Lun==0);
        nvms_t nvms;

        nvms = ad_nvms_open(FATFS_PARTITION_NAME);

        if (nvms && InstanceData.IsPresent == 1) {
                return 1; /* medium is present */
        }

        return 0; /* medium is not present */
}

/*********************************************************************/
/* USB-MSD initialization structure that is required                 */
/* when adding an MSD interface.                                     */
/*********************************************************************/
const static USB_MSD_STORAGE_API USB_MSD_StorageNVMS = {
        .pfInit = msd_storage_Init,                             // Initializes the storage medium.
        .pfGetInfo = msd_storage_GetInfo,                       // Retrieves storage medium information such as sector size and number of sectors available.
        .pfGetReadBuffer = msd_storage_GetReadBuffer,           // Prepares read function and returns a pointer to a buffer that is used by the storage driver.
        .pfRead = msd_storage_Read,                             // Reads one or multiple sectors from the storage medium.
        .pfGetWriteBuffer = msd_storage_GetWriteBuffer,         // Prepares write function and returns a pointer to a buffer that is used by the storage driver.
        .pfWrite = msd_storage_Write,                           // Writes one or more sectors to the storage medium.
        .pfMediumIsPresent = msd_storage_MediumIsPresent,       // Checks if medium is present.
        .pfDeInit = msd_storage_DeInit,                         // De-initializes the storage medium.
};


/*********************************************************************
 *
 * _AddMSD
 *      See SEGGER emUSB documentation
 */
static void _AddMSD(void)
{
        partition_entry_t part_info;
        uint32_t partitions_count;

        /* Add USB endpoints */
        InitializationData.EPIn = USBD_AddEP(USB_DIR_IN,
                                             USB_TRANSFER_TYPE_BULK,
                                             USB_MAX_PACKET_SIZE,
                                             NULL,
                                             0);

        InitializationData.EPOut = USBD_AddEP(USB_DIR_OUT,
                                              USB_TRANSFER_TYPE_BULK,
                                              USB_MAX_PACKET_SIZE,
                                              _abOutBuffer,
                                              sizeof(_abOutBuffer));
        USBD_MSD_Add(&InitializationData);

        // Add logical unit 0
        memset(&InstanceData, 0, sizeof(InstanceData));

        InstanceData.pAPI = &USB_MSD_StorageNVMS;

        /* Get the partition info */
        partitions_count = ad_nvms_get_partition_count();

        for (int idx=0; idx<partitions_count; idx++) {
                ad_nvms_get_partition_info(idx, &part_info);
                if (part_info.type == NVMS_FATFS_PART)  break;
        }

        InstanceData.DriverData.pStart = (void*)0;
        InstanceData.DriverData.NumSectors = part_info.sector_count;
        InstanceData.DriverData.SectorSize = FLASH_SECTOR_SIZE;
        InstanceData.DriverData.StartSector = 0;                                // The start sector that is used for the driver.
        InstanceData.DriverData.pSectorBuffer = temp_sector_buffer;             // Pointer to an application provided buffer to be used as temporary buffer for storing the sector data.
        InstanceData.DriverData.NumBytes4Buffer = sizeof(temp_sector_buffer);   // Size of the application provided buffer.
        InstanceData.DriverData.NumBuffers = 1;                                 // Number of buffer that are available. This is only used when using the MT storage layer.

        InstanceData.IsPresent = 1;
        InstanceData.DeviceType = 0; /* See SEGGER Documentation */
        InstanceData.pfHandleCmd = NULL;
        InstanceData.IsWriteProtected = false;
        InstanceData.pLunInfo = &_Lun0Info;

        USBD_MSD_AddUnit(&InstanceData);
}

/*********************************************************************
 * usb_msd_task
 *
 *  Function description
 *    This is the main task for implementing the application level
 *    of the MSD
 */
void usb_msd_task(void *params)
{

        run_usb_task = 1;

        USBD_Init();
        _AddMSD();
        USBD_SetDeviceInfo(&_DeviceInfo);
        USBD_Start();

#if ( dg_configUSE_SYS_CHARGER == 1 )
        /* Request the minimum guaranteed current budget. */
        USBD_SetMaxPower(hw_charger_i_level_to_miliamp(HW_CHARGER_I_LEVEL_500));
#endif

        while (run_usb_task == 1) {

                while ((run_usb_task == 1) &&
                       ((USBD_GetState() & (USB_STAT_CONFIGURED | USB_STAT_SUSPENDED)) != USB_STAT_CONFIGURED)) {
                        USB_OS_Delay(50);
                }

                if (run_usb_task == 1) {
                        /* Run the MSD class */
                        USBD_MSD_Task();
                }
        }

        run_usb_task = 0;


        OS_TASK_DELETE(NULL);
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
        OS_BASE_TYPE status;

        if (run_usb_task == 0) {
                /* Start the USB MSD application task. */
                status = OS_TASK_CREATE("msdTask",      /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                                usb_msd_task,           /* The function that implements the task. */
                                NULL,                   /* The parameter passed to the task. */
                                1024,                   /* The number of bytes to allocate to the
                                                             stack of the task. */
                                usb_main_TASK_PRIORITY, /* The priority assigned to the task. */
                                usb_msd_task_handle);   /* The task handle. */

                OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);
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
        USBD_Stop();
        USBD_DeInit();
        run_usb_task = 0;
}



