#ifndef __APP_DEBUG_FREERTOS_TCB_H__
#define __APP_DEBUG_FREERTOS_TCB_H__


#include <stdbool.h>
#include <stdint.h>


void adf_tracking_boot(void);

void adf_get_serialized_reset_data(uint8_t **data, uint16_t *len);

void adf_print_verbose(uint8_t *data, uint16_t len);



#endif //__APP_DEBUG_FREERTOS_TCB_H__


