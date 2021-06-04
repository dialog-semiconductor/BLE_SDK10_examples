/*------------------------------------------------------------------------*/
/* Sample Code of OS Dependent Functions for FatFs                        */
/* (C)ChaN, 2018                                                          */
/*------------------------------------------------------------------------*/
#if (dg_config_USE_FATFS == 1)
#include "ff.h"

#if FF_USE_LFN == 3             /* Dynamic memory allocation */
/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/

void* ff_memalloc (	        /* Returns pointer to the allocated memory block (null if not enough core) */
	UINT msize		/* Number of bytes to allocate */
)
{
#if defined(OS_FREERTOS)
        return OS_MALLOC(msize);        /* Allocate a new memory block with POSIX API */
#else
        return malloc(msize);
#endif
}

/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree (
	void* mblock	        /* Pointer to the memory block to free (nothing to do if null) */
)
{
#if defined(OS_FREERTOS)
	OS_FREE(mblock);	/* Free the memory block with POSIX API */
#else
	free(mblock);
#endif
}
#endif /* FF_USE_LFN */

#if FF_FS_REENTRANT	/* Mutual exclusion */
/*------------------------------------------------------------------------*/
/* Create a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object for the volume, such as semaphore and mutex.
/  When a 0 is returned, the f_mount() function fails with FR_INT_ERR.
*/

//const osMutexDef_t Mutex[FF_VOLUMES];	/* Table of CMSIS-RTOS mutex */

int ff_cre_syncobj (	        /* 1:Function succeeded, 0:Could not create the sync object */
	BYTE vol,		/* Corresponding volume (logical drive number) */
	FF_SYNC_t* sobj		/* Pointer to return the created sync object */
)
{
        /* FreeRTOS */
#if defined(OS_FREERTOS)
        OS_MUTEX_CREATE(*sobj);
	return (int)(*sobj != NULL);
#endif
}

/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/

int ff_del_syncobj (	        /* 1:Function succeeded, 0:Could not delete due to an error */
	FF_SYNC_t sobj		/* Sync object tied to the logical drive to be deleted */
)
{
	/* FreeRTOS */
#if defined(OS_FREERTOS)
        OS_MUTEX_DELETE(sobj);
	return 1;
#endif
}

/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant (	        /* 1:Got a grant to access the volume, 0:Could not get a grant */
	FF_SYNC_t sobj	        /* Sync object to wait */
)
{
	/* FreeRTOS */
#if defined(OS_FREERTOS)
        return (int)(OS_MUTEX_GET(sobj, FF_FS_TIMEOUT) == OS_MUTEX_TAKEN);
#endif
}

/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
	FF_SYNC_t sobj	        /* Sync object to be signaled */
)
{
	/* FreeRTOS */
#if defined(OS_FREERTOS)
        OS_MUTEX_PUT(sobj);
#endif
}
#endif /* FF_FS_REENTRANT */

#endif /* (dg_config_USE_FATFS == 1) */
