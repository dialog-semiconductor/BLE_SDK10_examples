#ifndef __APP_DEBUG_FREERTOS_TCB_H__
#define __APP_DEBUG_FREERTOS_TCB_H__


#include <stdbool.h>
#include <stdint.h>



uint16_t adf_get_data_length_avail(void);

void adf_get_task_data(uint8_t *data);

void adf_clear_data(void);

void adf_save_task_data(unsigned long *exception_args, uint32_t fault_lr);

void adf_tracking_boot(bool ret_needs_init);



#endif //__APP_DEBUG_FREERTOS_TCB_H__


