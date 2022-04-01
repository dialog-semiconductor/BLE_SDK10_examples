#ifndef __ADF_CONFIG_H__
#define __ADF_CONFIG_H__


#define ADF_MAX_TASK_NAME_LEN                   (8)     //Max char [] length for freeRTOS task name reporting
#define ADF_CALL_DEPTH                          (10)    //Total depth of words reported in the call routine
#define ADF_MAX_TRACKED_APP_TASKS               (7)     //Total number of tasks tracked by ADF (freertos_retarget is 4 for a baseline)

#define CMAC_STACK_DEPTH                        (10)    //Total depth in words reported in call depth

#define __SECTION_ADF_MAGIC                     __attribute__((section("sys_init_magic_num")));

void adf_trace_task_create(void *pxTCB);
void adf_trace_task_delete(void *pxTCB);

#define traceTASK_CREATE(tcb)                   adf_trace_task_create(tcb)
#define traceTASK_DELETE(tcb)                   adf_trace_task_delete(tcb)


#include "stdio.h"
/*
 * User definable print function to be called with adf_print_verbose.
 * undefine this function if printing is not needed, as verbose printing has significant overhead.
 */
#define ADF_PRINTF(...)                         printf(__VA_ARGS__)



#endif //__ADF_CONFIG_H__


