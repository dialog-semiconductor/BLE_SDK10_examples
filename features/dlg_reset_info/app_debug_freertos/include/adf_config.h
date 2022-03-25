#ifndef __ADF_CONFIG_H__
#define __ADF_CONFIG_H__





#define ADF_MAX_TASK_NAME_LEN               (8)
#define ADF_CALL_DEPTH                      (10)
#define ADF_MAX_TRACKED_APP_TASKS           (7)

#define CMAC_STACK_DEPTH                    (10)
#define __SECTION_ADF_MAGIC                 __attribute__((section("sys_init_magic_num")));


void adf_trace_task_create(void *pxTCB);
void adf_trace_task_delete(void *pxTCB);


#define traceTASK_CREATE(tcb)           adf_trace_task_create(tcb)
#define traceTASK_DELETE(tcb)           adf_trace_task_delete(tcb)

#include "stdio.h"

#define ADF_PRINTF(...)                     printf(__VA_ARGS__)

#include "FreeRTOS.h"




#endif //__ADF_CONFIG_H__


