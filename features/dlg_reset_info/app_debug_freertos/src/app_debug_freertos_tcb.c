#include "../../app_debug_freertos/include/app_debug_freertos_tcb.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cmsis_gcc.h"
#include "sdk_defs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

#include "../../app_debug_freertos/include/adf_config.h"

#if dg_configENABLE_ADF

#define SLOT_EMPTY                      (0x0000)
#define ADF_MN                          (0xBAADBEBE)
#define FPU_CONTEXT_MASK                (0x10)
#define FOUR_BYTE_PADDING_MASK          (0x100)


#define IS_IN_RAM(x)                    (x > MEMORY_SYSRAM_BASE && x < MEMORY_SYSRAM_END)
#define IS_IN_FLASH(x)                  (x>0x200 && x <512*1024)
#define IS_VALID_ADDRESS(x)             (x & 0x01) && ( IS_IN_RAM(x) || IS_IN_FLASH(x) )
#define IN_FPU_CONTEXT(f)               (!(f & FPU_CONTEXT_MASK))
#define GET_STACK_ALIGN_BYTES(xPSR)     (xPSR & FOUR_BYTE_PADDING_MASK ? 4 : 0)



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

typedef struct
{

        uint32_t                s16;
        uint32_t                s17;
        uint32_t                s18;
        uint32_t                s19;
        uint32_t                s20;
        uint32_t                s21;
        uint32_t                s22;
        uint32_t                s23;
        uint32_t                s24;
        uint32_t                s25;
        uint32_t                s26;
        uint32_t                s27;
        uint32_t                s28;
        uint32_t                s29;
        uint32_t                s30;
        uint32_t                s31;

}freertos_extended_frame_t;

typedef struct
{

        uint32_t                s0;
        uint32_t                s1;
        uint32_t                s2;
        uint32_t                s3;
        uint32_t                s4;
        uint32_t                s5;
        uint32_t                s6;
        uint32_t                s7;
        uint32_t                s8;
        uint32_t                s9;
        uint32_t                s10;
        uint32_t                s11;
        uint32_t                s12;
        uint32_t                s13;
        uint32_t                s14;
        uint32_t                s15;
        uint32_t                FPSCR;
        uint32_t                reserved;

}fpu_extended_frame_t;

typedef struct
{
        uint32_t                                r4;
        uint32_t                                r5;
        uint32_t                                r6;
        uint32_t                                r7;
        uint32_t                                r8;
        uint32_t                                r9;
        uint32_t                                r10;
        uint32_t                                r11;
        uint32_t                                exec_return;
        freertos_extended_frame_t               f_ext_frame;
        cortex_m_stack_frame_t                  frame;
        fpu_extended_frame_t                    ext_frame;


}m33_freetos_with_fpu_context_frame_t;

typedef struct
{
        uint32_t                        r4;
        uint32_t                        r5;
        uint32_t                        r6;
        uint32_t                        r7;
        uint32_t                        r8;
        uint32_t                        r9;
        uint32_t                        r10;
        uint32_t                        r11;
        uint32_t                        exec_return;
        cortex_m_stack_frame_t          frame;


}m33_freetos_no_fpu_context_frame_t;




typedef __PACKED_STRUCT
{
        char                            pcTaskName[ ADF_MAX_TASK_NAME_LEN ];
        uint32_t                        lr;
        uint32_t                        pc;
        bool                            taskIsActive;
        uint8_t                         stack_collected;                //in words
        uint32_t                        stack_vals[ ADF_CALL_DEPTH ];
}tcb_info_t;



typedef __PACKED_STRUCT
{
        uint32_t mn;
        tcb_info_t tcb_data[ADF_MAX_TRACKED_APP_TASKS];
}adf_info_t;



__RETAINED      bool            s_start_tracking_tcbs;
__RETAINED      uint8_t         s_num_tracked_tasks;
__RETAINED      void            *s_task_tcbs[ADF_MAX_TRACKED_APP_TASKS];

__RETAINED_UNINIT adf_info_t    s_adf_info;

static bool find_slot(size_t *idx, void *desired_tcb) {

        for (size_t i = 0; i < ADF_MAX_TRACKED_APP_TASKS; i++)
        {
                if (s_task_tcbs[i] == desired_tcb)
                {
                        *idx = i;
                        return true;
                }
        }

        return false;
}

__RETAINED_CODE uint8_t get_relative_stack_location(void *stack_pointer, bool task_active, uint8_t slot, uint32_t fault_lr)
{

        uint8_t offset = 0;

        if(task_active)
        {
                /*
                 * Task was last active this means that the stack pointer will have a standard or an
                 * extend frame the stack.  Extended Frame is the addition of the floating point registers
                 * on top of a standard frame
                 */
                cortex_m_stack_frame_t *frame =  (cortex_m_stack_frame_t *)stack_pointer;
                offset += sizeof(cortex_m_stack_frame_t);

                /*
                 *  $lr in Handler mode (bit 5 gives the FPU context).  If we were in FPU context
                 *  jump past the extend frame registers.
                 */
                if(IN_FPU_CONTEXT(fault_lr))
                {
                        offset += sizeof(fpu_extended_frame_t);
                }

                offset += GET_STACK_ALIGN_BYTES(frame->xPSR);

                s_adf_info.tcb_data[slot].lr = frame->LR;
                s_adf_info.tcb_data[slot].pc = frame->ReturnAddress;

        }else
        {


                m33_freetos_no_fpu_context_frame_t *ctx_frame = (m33_freetos_no_fpu_context_frame_t *)stack_pointer;

                /*
                 *  Both context frames have the exec_return value at the same relative location.
                 *  First cast to no fpu frame and change pointer only if we are in context frame.
                 */
                if(IN_FPU_CONTEXT(ctx_frame->exec_return))
                {
                        m33_freetos_with_fpu_context_frame_t *ctx_ext_frame =
                                                (m33_freetos_with_fpu_context_frame_t *)stack_pointer;

                        offset += sizeof(m33_freetos_with_fpu_context_frame_t);
                        offset += GET_STACK_ALIGN_BYTES(ctx_ext_frame->frame.xPSR);

                        s_adf_info.tcb_data[slot].lr = ctx_ext_frame->frame.LR;
                        s_adf_info.tcb_data[slot].pc = ctx_ext_frame->frame.ReturnAddress;

                }else
                {
                        offset += sizeof(m33_freetos_no_fpu_context_frame_t);

                        s_adf_info.tcb_data[slot].lr = ctx_frame->frame.LR;
                        s_adf_info.tcb_data[slot].pc = ctx_frame->frame.ReturnAddress;

                        offset += GET_STACK_ALIGN_BYTES(ctx_frame->frame.xPSR);
                }

        }

        s_adf_info.tcb_data[slot].taskIsActive = task_active;


        return offset;

}


__RETAINED_CODE void adf_save_task_data(unsigned long *exception_args, uint32_t fault_lr)
{

        uint8_t tcb_info_slot = 0;

        for(uint8_t i = 0; i < ADF_MAX_TRACKED_APP_TASKS; i++)
        {

                if(s_task_tcbs[i] != SLOT_EMPTY)
                {
                        char *pcTaskName = pcTaskGetName(s_task_tcbs[i]);
                        memcpy(s_adf_info.tcb_data[tcb_info_slot].pcTaskName,
                                                        pcTaskName, ADF_MAX_TASK_NAME_LEN);

                       /*
                        * Top of stack is always the first element on the TCB pointer just copy
                        * from here
                        */
                        void *top_of_stack = (void *)(*(uintptr_t *)s_task_tcbs[i]);
                        void *task_stack_vals_loc;

                        /*
                         * Depending on the state of a task, the contents of the stack frame will differ.
                         * If a task is not presently active, then a context stack frame will be located at the top
                         * of stack that is used by the context switching by FreeRTOS.  If the task is active,
                         * then this function plus the m33_frame will be pushed to the stop of the task.
                         * Finding the last active stack requires understanding where we are and moving the pointer
                         * accordingly.
                         */

                        BaseType_t task_active = vTaskIsActive(s_task_tcbs[i]);
                        void *stack_pointer = task_active ? exception_args : top_of_stack;


                        task_stack_vals_loc = (void *)stack_pointer + get_relative_stack_location(stack_pointer,
                                                                task_active,
                                                                tcb_info_slot,
                                                                fault_lr);

                        /*
                         * Make sure that we aren't copying outside of the stack boundary
                         */
                        void    *btm_of_stack = vTaskGetEndOfStack(s_task_tcbs[i]);

                        uint8_t stack_val_num = 0;
                        for(uintptr_t i = (uintptr_t)task_stack_vals_loc; i < (uintptr_t)btm_of_stack; i +=sizeof(uintptr_t))
                        {
                                uint32_t stack_val = *(uint32_t *)i;
                                if( IS_VALID_ADDRESS(stack_val) )
                                {
                                        s_adf_info.tcb_data[tcb_info_slot].stack_vals[stack_val_num] = stack_val;
                                        stack_val_num++;
                                }

                                /*Copy don't copy a value out of bounds
                                 */

                                if(stack_val_num >= ADF_CALL_DEPTH)
                                {
                                        break;
                                }

                        }


                        s_adf_info.tcb_data[tcb_info_slot].stack_collected = stack_val_num;

                        tcb_info_slot++;



                }

        }

        if(tcb_info_slot)
        {
                s_adf_info.mn = ADF_MN;
        }

}

void adf_tracking_boot(bool ret_needs_init)
{
        s_start_tracking_tcbs = true;
        memset(s_task_tcbs, 0, sizeof(s_task_tcbs));

        if(ret_needs_init)
        {
                memset(&s_adf_info, 0, sizeof(s_adf_info));
        }
}

void adf_trace_task_create(void *pxTCB)
{

        if(s_start_tracking_tcbs)
        {
                size_t slot_idx;
                bool slot_found = find_slot(&slot_idx, SLOT_EMPTY);

                if(slot_found)
                {
                        s_task_tcbs[slot_idx] = pxTCB;
                        s_num_tracked_tasks++;

                }else
                {
                        //INCREASE MAX_TRACKED_APP_TASKS or decrease the number of tasks tracked
                        ASSERT_ERROR(0);
                }
        }

}

void adf_trace_task_delete(void *pxTCB)
{
        if(s_start_tracking_tcbs)
        {

                size_t slot_idx;
                bool slot_found = find_slot(&slot_idx, pxTCB);

                if(slot_found)
                {
                        s_task_tcbs[slot_idx] = SLOT_EMPTY;
                        s_num_tracked_tasks--;

                }else
                {
                        //If start stracking happens in system init, the TCB wont be tracked so ignore
                }
        }
}


uint16_t adf_get_data_length_avail(void)
{
        if(s_adf_info.mn == ADF_MN)
        {
                return s_num_tracked_tasks*sizeof(tcb_info_t);
        }

        return 0;
}

void adf_get_task_data(uint8_t *data)
{
        uint16_t size = s_num_tracked_tasks*sizeof(tcb_info_t);

        memcpy(data, s_adf_info.tcb_data, size);

}

void adf_clear_data(void)
{
        /*
         * No reason to waste time with a memset.  s_adf_info is statically allocated so the size will
         * be static as well.  If the Magic Number is set to zero then
         */
        s_adf_info.mn = 0;
}



#endif
