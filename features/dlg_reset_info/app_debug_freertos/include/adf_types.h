#ifndef __ADF_TYPES_H__
#define __ADF_TYPES_H__

#include "adf_config.h"
#include "cmsis_gcc.h"


/*
 * Debug Info Type
 */
typedef enum
{
        ADF_TYPE_HEADER,                //Main ADF Header
        ADF_TYPE_LAST_FRAME,            //Last stack frame, including type
        ADF_TYPE_TCB_TRACE,             //Task information from FreeRTOS TCB
        ADF_TYPE_CMAC_TRACE,            //CMAC stack trace

}adf_serialize_type_t;

typedef __PACKED_STRUCT
{
        adf_serialize_type_t    type;
        uint16_t                length;
}adf_serialize_tlv_t;


typedef enum
{
        RESET_POR,
        RESET_HW,
        RESET_SW,
        RESET_WDOG,
        RESET_SWD,
        RESET_CMAC,
        RESET_LAST,
}reset_reason_t;

/*
 * Basic stack frame
 */

typedef struct
{

        uint32_t        r0;
        uint32_t        r1;
        uint32_t        r2;
        uint32_t        r3;
        uint32_t        r12;
        uint32_t        LR;
        uint32_t        ReturnAddress;
        uint32_t        xPSR;

}cortex_m_stack_frame_t;


typedef enum
{
        LF_HARDFAULT,
        LF_NMI,
        LF_CMAC_NMI,
        LF_CMAC_HF,

}adf_last_frame_type;

typedef __PACKED_STRUCT
{
        adf_last_frame_type    type;
        cortex_m_stack_frame_t last_frame;
}adf_last_frame_info_t;


typedef __PACKED_STRUCT
{
        uint8_t                         data_avail;
        char                            pcTaskName[ ADF_MAX_TASK_NAME_LEN ];
        uint32_t                        lr;
        uint32_t                        pc;
        bool                            taskIsActive;
        uint8_t                         stack_collected;                //in words
        uint32_t                        stack_vals[ ADF_CALL_DEPTH ];
}tcb_info_t;


typedef __PACKED_STRUCT
{
        uint8_t                         stack_depth;
        uint32_t                        stack_vals[CMAC_STACK_DEPTH];
}cmac_event_info_t;



#endif //__ADF_TYPES_H__

