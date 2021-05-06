/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various existing       */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#if (dg_config_USE_FATFS == 1)

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "ad_nvms.h"
#if (!FF_FS_NORTC && !FF_FS_READONLY)
#include "hw_rtc.h"
#endif


/* Partition handler */
__RETAINED static nvms_t fatfs_nvmh;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive number to identify the drive */
)
{
	switch (pdrv) {
	case DEV_FLASH:
	        fatfs_nvmh = ad_nvms_open(FATFS_PARTITION_NAME);

                if (fatfs_nvmh) {
                        return FR_OK;
                } else {
                        return STA_NOINIT;
                }
        case DEV_MMC:
                return STA_NODISK;
	}
	return STA_NODISK;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv		/* Physical drive number to identify the drive */
)
{
	switch (pdrv) {
	case DEV_FLASH:
	        fatfs_nvmh = ad_nvms_open(FATFS_PARTITION_NAME);

                if (fatfs_nvmh) {
                        return FR_OK;
                } else {
                        return STA_NOINIT;
                }
	case DEV_MMC:
	        return STA_NODISK;
	}
	return STA_NODISK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive number to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	        /* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
        /* Get sector size */
        LBA_t ssize;
        ssize = (FF_MIN_SS == FF_MAX_SS) ? FF_MAX_SS : FLASH_SECTOR_SIZE;

	switch (pdrv) {
	case DEV_FLASH:
	        if (ad_nvms_read(fatfs_nvmh, (uint32_t)(sector * ssize), buff, (uint32_t)(count * ssize)) == (uint32_t)(count * ssize)) {
	                return RES_OK;
	        } else {
	                return RES_ERROR;
	        }
        case DEV_MMC:
                return RES_NOTRDY;
	}
	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0
DRESULT disk_write (
	BYTE pdrv,		/* Physical drive number to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count		/* Number of sectors to write */
)
{
        /* Get sector size */
        LBA_t ssize;
        ssize = (FF_MIN_SS == FF_MAX_SS) ? FF_MAX_SS : FLASH_SECTOR_SIZE;

	switch (pdrv) {
	case DEV_FLASH:
                if (ad_nvms_write(fatfs_nvmh, (uint32_t)(sector * ssize), buff, (uint32_t)(count * ssize)) == (uint32_t)(count * ssize)) {
                        return RES_OK;
                } else {
                        return RES_ERROR;
                }
	case DEV_MMC:
	        return RES_NOTRDY;
	}
	return RES_PARERR;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive number (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
        /* Get sector size */
        LBA_t ssize;
        ssize = (FF_MIN_SS == FF_MAX_SS) ? FF_MAX_SS : FLASH_SECTOR_SIZE;

	switch (pdrv) {
	case DEV_FLASH:
	        switch(cmd) {
	        case GET_SECTOR_COUNT:
	                *((LBA_t *)buff) = (LBA_t)(ad_nvms_get_size(fatfs_nvmh) / ssize);
	                return RES_OK;
                case CTRL_SYNC:
                        /*
                         * No actions are required when accessing the flash
                         * memory via the NVMS layer.
                         */
                        return RES_OK;
                case GET_BLOCK_SIZE:
                        *((DWORD *)buff) = (DWORD)1; /* NOTE: The value provided should be power of 2. However, if a '1' or invalid value is passed then a '1' is selected. */
                        return RES_OK;
                case GET_SECTOR_SIZE:
                        *((WORD *)buff) = (WORD)FLASH_SECTOR_SIZE;
                        return RES_OK;
                case CTRL_TRIM:
                        /* Provide actions required to erase unused data blocks */
                        return RES_PARERR;
	        }
	        OS_ASSERT(0); /* Unsupported command - shouldn't be here */
	}
	return RES_PARERR;
}


#if (!FF_FS_NORTC && !FF_FS_READONLY)
/* Get the current time packed as bit-fields into a DWORD value */
DWORD get_fattime(void)
{
        DWORD fs_time;
        rtc_time time;
        rtc_calendar clndr;

        /* Get the current time in decimal format */
        hw_rtc_get_time_clndr(&time, &clndr);

        /* Format data */

        // bit 31:25 year --> Value origin from 1920 (0..127). For instance 37 for 2017
        // bit 24:21 month (1..12)
        // bit 20:16 day (1..31)
        // bit 15:11 hour (0...23)
        // bit 10:5 minute (0...59)
        // bit 4:0 --> Value should be divided by 2 (0...29). For instance 25 for 50.
        fs_time = FS_TIME_PACK(time.sec, time.minute, time.hour, clndr.mday, clndr.month, clndr.year);

        return fs_time;
}
#endif /* (!FF_FS_NORTC && !FF_FS_READONLY) */

#endif /* (dg_config_USE_FATFS == 1) */
