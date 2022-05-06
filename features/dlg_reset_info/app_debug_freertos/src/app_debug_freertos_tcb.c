#include "../../app_debug_freertos/include/app_debug_freertos_tcb.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cmsis_gcc.h"
#include "sdk_defs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "hw_hard_fault.h"
#include "hw_cpm.h"

#include "../../app_debug_freertos/include/adf_types.h"
#include "../../app_debug_freertos/include/adf_config.h"

#if dg_configENABLE_ADF

#define CONFIG_CMAC_SUPPORT    defined(CONFIG_USE_BLE)  && defined(CMAC_CTX_PTR_SUPPORT)


#define SYS_RAM_MAGIC_NUM               (0x01C28122)
#define RET_RAM_NEEDS_INITIALIZED(mn)   (mn != SYS_RAM_MAGIC_NUM)

#define SLOT_EMPTY                      (0x0000)
#define LAST_FRAME_MN                   (0xBADCOFEE)
#define ADF_MN                          (0xBAADBEBE)
#define FPU_CONTEXT_MASK                (0x10)
#define FOUR_BYTE_PADDING_MASK          (0x100)

#define IVT_SIZE                        (0x200)
#define CMAC_IVT_SIZE                   (0xb4)
#define MAX_IMAGE_ADDRESS               (dg_configQSPI_MAX_IMAGE_SIZE)


#define IS_IN_RAM(x)                    (x > MEMORY_SYSRAM_BASE && x < MEMORY_SYSRAM_END)
#define IS_IN_FLASH(x)                  (x>IVT_SIZE && x<MAX_IMAGE_ADDRESS)
#define IS_VALID_ADDRESS(x)             (x & 0x01) && ( IS_IN_RAM(x) || IS_IN_FLASH(x) )
#define IN_FPU_CONTEXT(f)               (!(f & FPU_CONTEXT_MASK))
#define GET_STACK_ALIGN_BYTES(xPSR)     (xPSR & FOUR_BYTE_PADDING_MASK ? 4 : 0)


#define CMAC_STACK_VAL_IN_RANGE(x, cmi_end)     (x > CMAC_IVT_SIZE && x < cmi_end)
#define CMAC_ADDR_LINKED(x, cmi_end)            (CMAC_STACK_VAL_IN_RANGE(x, cmi_end)) && (x & 0x01)


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
        // The SP when NMI_HandlerC() was called.
        //    (stacked_r8, .. , hf_stacked_psr)
        uint32_t stack_ptr;             // 0x0C

        // {r4 - r7} at time of crash
        uint32_t stacked_r8;            // 0x10
        uint32_t stacked_r9;            // 0x14
        uint32_t stacked_r10;           // 0x18
        uint32_t stacked_r11;           // 0x1C

        // {r8 - r11} at time of crash
        uint32_t stacked_r4;            // 0x20
        uint32_t stacked_r5;            // 0x24
        uint32_t stacked_r6;            // 0x28
        uint32_t stacked_r7;            // 0x2C

}cmac_additional_stacked_regs;


typedef __PACKED_STRUCT {
        uint32_t                        magic;                 // 0x00
        uint32_t                        magic_0;               // 0x04
        uint32_t                        magic_1;               // 0x08
        cmac_additional_stacked_regs    add_regs;

        cortex_m_stack_frame_t          nmi_stack_frame;
        cortex_m_stack_frame_t          hardfault_stack_frame;

} cmac_exception_t;


typedef __PACKED_STRUCT
{
        uint32_t mn;
        adf_last_frame_info_t last_stack_frame;
        tcb_info_t tcb_data[ADF_MAX_TRACKED_APP_TASKS];
}adf_info_t;


volatile        uint32_t        boot_ram_mn_val                      __SECTION_ADF_MAGIC

__RETAINED      bool                    s_start_tracking_tcbs;
__RETAINED      uint8_t                 s_num_tracked_tasks;
__RETAINED      void                    *s_task_tcbs[ADF_MAX_TRACKED_APP_TASKS];

__RETAINED_UNINIT adf_info_t            s_adf_info;
__RETAINED_UNINIT reset_reason_t        s_last_reset_reason;
__RETAINED_UNINIT cmac_event_info_t     s_cmac_error_event_data;


/**
\brief Get Link Register
\details Returns the current value of the Link Register (LR).
\return LR Register value
*/
__STATIC_FORCEINLINE uint32_t __get_LR(void)
{
  uint32_t result;

  __ASM volatile ("MOV %0, LR\n" : "=r" (result) );
  return(result);
}



/*
 * Find tcb pointer in static tcb buffer
 */
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


/*
 * Get the reset reason as indicated by the RESET_STAT_REG
 */

static reset_reason_t get_reset_reason(void)
{
        reset_reason_t reason = RESET_LAST;
        uint32_t reset_stat_reg = CRG_TOP->RESET_STAT_REG;
        uint8_t mask;

        for(uint8_t i = 0; i < RESET_LAST; i++)
        {
                mask = 1 << i;
                if(reset_stat_reg & mask)
                {
                        reason = (reset_reason_t)i;
                        break;
                }

        }

        //Set RESET_STAT_REG back to 0 to track for next reset
        hw_sys_track_reset_type();

        return reason;

}

/*
 * Get relative stack location offset, relative to the stack pointer
 */

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


__RETAINED_CODE static void adf_save_task_data(unsigned long *exception_args)
{

        uint8_t tcb_info_slot = 0;
        uint32_t fault_lr = __get_LR();

        for(uint8_t i = 0; i < ADF_MAX_TRACKED_APP_TASKS; i++)
        {

                if(s_task_tcbs[i] != SLOT_EMPTY)
                {
                        s_adf_info.tcb_data[tcb_info_slot].data_avail = 1;
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


                        /*
                         * Get the relative stack location, relative to the stack pointer, to find
                         * call stack
                         */
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

        if(!tcb_info_slot)
        {
                //User defined adf_tracking_boot too late and zero tasks are being tracked
                s_adf_info.mn = 0;
        }

}

/*
 * Start tracking tcbs.
 */

void adf_start_tracking_init(void)
{
        s_start_tracking_tcbs = true;
        memset(s_task_tcbs, 0, sizeof(s_task_tcbs));
        s_last_reset_reason = get_reset_reason();

        /*
         * If we have a POR or the sys_ram_magic_num is corrupted, then reinitialize.
         * The magic number is placed in a linker location, that if you rebuild new firmware, this will move
         * the number, and not trigger reset data for a new firwmare load (New firmware load is a HW_RESET, not POR).
         */
        if(s_last_reset_reason == RESET_POR ||
                RET_RAM_NEEDS_INITIALIZED(boot_ram_mn_val))
        {
                memset(&s_adf_info, 0, sizeof(s_adf_info));
        }

        boot_ram_mn_val = SYS_RAM_MAGIC_NUM;
}


#ifdef ADF_PRINTF

char reset_reason_map[][10] = {
        "RESET_POR",
        "RESET_HW",
        "RESET_SW",
        "RESET_WDOG",
        "RESET_SWD",
        "RESET_CMAC",

};

char last_frame_type_map [][15] =
{
        "LF_HARDFAULT",
        "LF_NMI",
        "LF_CMAC_NMI",
        "LF_CMAC_HF",
};

void adf_print_verbose(uint8_t *data, uint16_t len)
{
        uint16_t ptr = 0;
        uint8_t *p_data = data;
        ADF_PRINTF("\r\n*****************ADF Data*****************\r\n\r\n");

        while(ptr < len)
        {
                adf_serialize_type_t type = (adf_serialize_type_t)*p_data;
                p_data += sizeof(adf_serialize_type_t);
                uint16_t adf_len = *((uint16_t *)p_data);
                p_data += sizeof(uint16_t);

                switch (type)
                {
                        case ADF_TYPE_HEADER:
                        {
                                reset_reason_t reason = *p_data;
                                p_data += sizeof(reset_reason_t);

                                ADF_PRINTF("ADF Header:\r\n");
                                ADF_PRINTF("\tADF Total Length: %d bytes\r\n", adf_len);


                                ADF_PRINTF("\tLast Reset Reason: %s\r\n\r\n", reset_reason_map[reason]);

                        }break;
                        case ADF_TYPE_LAST_FRAME:
                        {
                                adf_last_frame_info_t * lf = (adf_last_frame_info_t *)p_data;

                                ADF_PRINTF("ADF Last Frame Information:\r\n");
                                ADF_PRINTF("\t Last Frame Type: %s\r\n", last_frame_type_map[lf->type]);
                                ADF_PRINTF("\t Last Stack Frame:\r\n");

                                ADF_PRINTF("\t\t r0: 0x%08x\r\n", (unsigned int)lf->last_frame.r0);
                                ADF_PRINTF("\t\t r1: 0x%08x\r\n", (unsigned int)lf->last_frame.r1);
                                ADF_PRINTF("\t\t r2: 0x%08x\r\n", (unsigned int)lf->last_frame.r2);
                                ADF_PRINTF("\t\t r3: 0x%08x\r\n", (unsigned int)lf->last_frame.r3);
                                ADF_PRINTF("\t\t r12: 0x%08x\r\n", (unsigned int)lf->last_frame.r12);
                                ADF_PRINTF("\t\t LR: 0x%08x\r\n", (unsigned int)lf->last_frame.LR);
                                ADF_PRINTF("\t\t ReturnAddress: 0x%08x\r\n", (unsigned int)lf->last_frame.ReturnAddress);
                                ADF_PRINTF("\t\t xPSR: 0x%08x\r\n", (unsigned int)lf->last_frame.xPSR);



                                p_data += sizeof(adf_last_frame_info_t);

                        }break;
                        case ADF_TYPE_TCB_TRACE:
                        {
                                //We don't serialize data_avail bit
                                tcb_info_t *info = (tcb_info_t *)p_data;
                                char taskname[ADF_MAX_TASK_NAME_LEN + 1];
                                memcpy(taskname, info->pcTaskName, ADF_MAX_TASK_NAME_LEN);
                                taskname[ADF_MAX_TASK_NAME_LEN] = '\0';

                                ADF_PRINTF("ADF TCB Trace:\r\n");
                                ADF_PRINTF("\t Data Valid: %s\r\n", info->data_avail != 0 ? "True" : "False");
                                ADF_PRINTF("\t Task Name: %s\r\n", taskname);
                                ADF_PRINTF("\t Link Register: 0x%08x\r\n", (unsigned int)info->lr);
                                ADF_PRINTF("\t PC: 0x%08x\r\n", (unsigned int)info->pc);
                                ADF_PRINTF("\t Task is Active: %s\r\n", info->taskIsActive ? "True" : "False");
                                ADF_PRINTF("\t Call Stack Depth: %d words\r\n", info->stack_collected);

                                if(info->stack_collected > 0)
                                {
                                        ADF_PRINTF("\t Call Stack:\r\n");

                                        for(uintptr_t i = 0;
                                                i < info->stack_collected; i ++)
                                        {
                                                ADF_PRINTF("\t\tCall #%d:0x%08x\r\n", i+1, (unsigned int)info->stack_vals[i]);
                                        }

                                }

                                p_data += sizeof(tcb_info_t) - (ADF_CALL_DEPTH - info->stack_collected);;

                        }break;
                        case ADF_TYPE_CMAC_TRACE:
                        {
                        }break;

                        default:
                        break;
                }

                ptr = p_data - data;

        }

        ADF_PRINTF("\r\n******************************************\r\n\r\n");

}
#endif

uint16_t adf_get_serialized_size(void)
{
        uint16_t len = sizeof(reset_reason_t);
        len += sizeof(adf_serialize_tlv_t);

        if(s_adf_info.mn != ADF_MN)
        {

              return len;
        }

        /*
         * Add TL plus length of stack frame type
         */
        len += sizeof(adf_serialize_tlv_t);
        len += sizeof(s_adf_info.last_stack_frame);

        /*
         * If the last stack frame is there then the TCB data will be there as well
         */

        for(uint8_t i = 0; i < ADF_MAX_TRACKED_APP_TASKS; i++)
        {
                if(s_adf_info.tcb_data[i].data_avail == 1)
                {
                        len += sizeof(adf_serialize_tlv_t);
                        len += sizeof(tcb_info_t) - (ADF_CALL_DEPTH - s_adf_info.tcb_data[i].stack_collected);

                }

        }
#if CONFIG_CMAC_SUPPORT
        if(s_adf_info.last_stack_frame.type == LF_CMAC_HF ||
                s_adf_info.last_stack_frame.type == LF_CMAC_NMI)
        {
                len += sizeof(adf_serialize_tlv_t);
                len += sizeof(cmac_event_info_t) - (CMAC_STACK_DEPTH - s_cmac_error_event_data.stack_depth);
        }
#endif

        return len;

}


void adf_get_serialized_reset_data(uint8_t *data, uint16_t *len, uint16_t serial_len)
{

        uint16_t current_ptr = 0;
        uint16_t adf_header_len = serial_len - sizeof(adf_serialize_tlv_t);

         /*
          * Copy over header information
          */
         data[current_ptr++] = ADF_TYPE_HEADER;
         data[current_ptr++] = adf_header_len & 0xFF;
         data[current_ptr++] = adf_header_len >> 8;
         data[current_ptr++] = s_last_reset_reason;

        if(s_adf_info.mn != ADF_MN)
        {
              *len = current_ptr;
              return;
        }


         /*
          * Copy over last frame information
          */

         uint8_t last_stack_frame_size = sizeof(adf_last_frame_info_t);

         data[current_ptr++] = ADF_TYPE_LAST_FRAME;  //Type
         data[current_ptr++] = last_stack_frame_size & 0xFF; //Length
         data[current_ptr++] = last_stack_frame_size >> 8;
         memcpy(data+current_ptr, &s_adf_info.last_stack_frame, last_stack_frame_size);
         current_ptr += last_stack_frame_size;


         /*
          * If the last stack frame is there then the TCB data will be there as well
          */

         for(uint8_t i = 0; i < ADF_MAX_TRACKED_APP_TASKS; i++)
         {
                 if(s_adf_info.tcb_data[i].data_avail == 1)
                 {
                         data[current_ptr++] = ADF_TYPE_TCB_TRACE;
                         uint8_t tcb_len  = sizeof(tcb_info_t) - (ADF_CALL_DEPTH - s_adf_info.tcb_data[i].stack_collected);
                         data[current_ptr++] = tcb_len & 0xFF;
                         data[current_ptr++] = tcb_len >> 8;

                         memcpy(data + current_ptr, &s_adf_info.tcb_data[i], tcb_len);
                         current_ptr += tcb_len;

                 }

         }

#if CONFIG_CMAC_SUPPORT

         if(s_adf_info.last_stack_frame.type == LF_CMAC_HF ||
                         s_adf_info.last_stack_frame.type == LF_CMAC_NMI)
         {
                 data[current_ptr++] = ADF_TYPE_TCB_TRACE;
                 uint8_t cmac_len = sizeof(cmac_event_info_t) - (CMAC_STACK_DEPTH - s_cmac_error_event_data.stack_depth);

                 data[current_ptr++] = cmac_len & 0xFF;
                 data[current_ptr++] = cmac_len >> 8;

                 memcpy(data + current_ptr, &s_cmac_error_event_data, cmac_len);
                 current_ptr += cmac_len;
         }

         *len = current_ptr;
#endif

         memset(&s_adf_info, 0, sizeof(s_adf_info));



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



__STATIC_INLINE void set_last_stack_frame(void *frame)
{
        s_adf_info.mn = ADF_MN;
        memcpy(&s_adf_info.last_stack_frame.last_frame, frame, sizeof(cortex_m_stack_frame_t));
}

__RETAINED_CODE void adf_hardfault_event_handler(void *exception_args)
{
        s_adf_info.last_stack_frame.type = LF_HARDFAULT;
        set_last_stack_frame(exception_args);

        adf_save_task_data(exception_args);
}

__RETAINED_CODE void adf_nmi_event_handler(unsigned long *exception_args)
{

        s_adf_info.last_stack_frame.type = LF_NMI;
        set_last_stack_frame(exception_args);

        adf_save_task_data(exception_args);

}


#if CONFIG_CMAC_SUPPORT
__RETAINED_CODE void ble_controller_error(void)
{
        s_adf_info.mn = ADF_MN;
        cmac_exception_t *reg_info =  (cmac_exception_t *)cmac_exception_ctx_ptr;

        s_adf_info.last_stack_frame.type =  reg_info->hardfault_stack_frame.r0 == 0xFFFFFFFF ? LF_CMAC_NMI : LF_CMAC_HF;

        if(s_adf_info.last_stack_frame.type == LF_CMAC_HF)
        {
                memcpy(&s_adf_info.last_stack_frame.last_frame, &reg_info->hardfault_stack_frame, sizeof(cortex_m_stack_frame_t));
        }else
        {
                memcpy(&s_adf_info.last_stack_frame.last_frame, &reg_info->nmi_stack_frame, sizeof(cortex_m_stack_frame_t));
        }

        extern uint32_t cmi_fw_dst_addr;
        uint8_t stack_val_num = 0;

        /*
         * Get the max code size to get the max range for Linked Addresses, need to convert from M33 memory
         * space to M33 memory space
         */
        uint32_t cmac_code_max = (uintptr_t)MEMCTRL->CMI_DATA_BASE_REG- (uintptr_t)MEMCTRL->CMI_CODE_BASE_REG;

        uintptr_t cmac_stack_ptr = reg_info->add_regs.stack_ptr;
        uintptr_t cmac_stack_ptr_m33  = MEMCTRL->CMI_DATA_BASE_REG + cmac_stack_ptr;
        uintptr_t cmac_end_of_stack_m33 =  (uintptr_t)cmi_fw_dst_addr  +
                                        (uintptr_t)MEMCTRL->CMI_DATA_BASE_REG;

        for(uintptr_t i = (uintptr_t)cmac_stack_ptr_m33; i < (uintptr_t)cmac_end_of_stack_m33; i +=sizeof(uintptr_t))
        {
                uint32_t stack_val = *(uint32_t *)i;
                if( CMAC_ADDR_LINKED(stack_val, cmac_code_max) ) //0x01 to check it it's linked, 0xb4 to make sure after the IVT
                {
                        s_cmac_error_event_data.stack_vals[stack_val_num] = stack_val;
                        stack_val_num++;
                }

                /*Copy don't copy a value out of bounds
                 */

                if(stack_val_num >= CMAC_STACK_DEPTH)
                {
                        break;
                }

        }

        s_cmac_error_event_data.stack_depth = stack_val_num;




        hw_cpm_reboot_system();                         // Force reset

}
#endif //CONFIG_CMAC_SUPPORT


__RETAINED_CODE void HardFault_HandlerC(unsigned long *hardfault_args)
{
#if dg_configENABLE_MTB
        /* Disable MTB */
        *MTB_MASTER_REG = MTB_MASTER_DISABLE_MSK;
#endif /* dg_configENABLE_MTB */

        if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE) {
                hw_watchdog_freeze();                           // Stop WDOG

                ENABLE_DEBUGGER;


                if (EXCEPTION_DEBUG == 1) {
                        hw_sys_assert_trigger_gpio();
                }

                adf_hardfault_event_handler(hardfault_args);

                while (1);
        }
        else {

                adf_hardfault_event_handler(hardfault_args);
                hw_cpm_reboot_system();                         // Force reset

                while (1);
        }
}


__RETAINED_CODE void hw_watchdog_handle_int(unsigned long *exception_args)
{
        // Reached this point due to a WDOG timeout
        uint16_t pmu_ctrl_reg = CRG_TOP->PMU_CTRL_REG;
        pmu_ctrl_reg |= ((1 << CRG_TOP_PMU_CTRL_REG_COM_SLEEP_Pos)     |        /* turn off communication PD */
                         (1 << CRG_TOP_PMU_CTRL_REG_RADIO_SLEEP_Pos)   |        /* turn off radio PD */
                         (1 << CRG_TOP_PMU_CTRL_REG_PERIPH_SLEEP_Pos));         /* turn off peripheral PD */
        CRG_TOP->PMU_CTRL_REG = pmu_ctrl_reg;

#if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE)
        hw_watchdog_freeze();                           // Stop WDOG

        ENABLE_DEBUGGER;

        adf_nmi_event_handler(exception_args);

        if (EXCEPTION_DEBUG == 1) {
                hw_sys_assert_trigger_gpio();
        }

        if (REG_GETF(CRG_TOP, SYS_STAT_REG, DBG_IS_ACTIVE)) {
                __BKPT(0);
        }
        else {
                while (1);
        }

#else // dg_configIMAGE_SETUP == DEVELOPMENT_MODE

        adf_nmi_event_handler(exception_args);
        // Wait for the reset to occur
        while (1);
#endif // dg_configIMAGE_SETUP == DEVELOPMENT_MODE
}


#endif //dg_configENABLE_ADF
