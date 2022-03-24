#ifndef __ADF_CONFIG_H__
#define __ADF_CONFIG_H__


#define ADF_MAX_TASK_NAME_LEN               (8)
#define ADF_CALL_DEPTH                      (10)
#define ADF_MAX_TRACKED_APP_TASKS           (7)


void adf_trace_task_create(void *pxTCB);
void adf_trace_task_delete(void *pxTCB);

#ifdef traceTASK_CREATE
#undef traceTASK_CREATE
#undef traceTASK_DELETE
#endif

#define traceTASK_CREATE(tcb)           adf_trace_task_create(tcb)
#define traceTASK_DELETE(tcb)           adf_trace_task_delete(tcb)

#include "FreeRTOS.h"



#endif //__ADF_CONFIG_H__


