# Debug Reset Information

The goal of this application is to provide the user with a means to debug fault data in a production environment.  The ADF (Application Debug FreeRTOS) module is intended to be portable to any application. On any fault, it will capture appropriate data and report to the user meaningful information. 

This module relies on uninitialized RAM, a few minor modifications to the SDK and basic unrolling of stacks to communicate to the user pertinent states of each active task.


## HW & SW Configurations

- **Hardware Configurations**
  - This example runs on a DA1469x Bluetooth Smart SoC.
  - A DA1469x Pro Development Kit or USB kit is needed for this example.
- **Software Configurations**
  - Download the latest SDK version for the DA1469x family of devices (10.0.10.x)
  - **SEGGER's J-Link** tools should be downloaded and installed.

## SDK Modifications

  - **FreeRTOS Modifications**
    - Navigate to sdk/FreeRTOS/include/task.h and at the very bottom of the include file, place the following two prototypes:

    ```
    #if dg_configENABLE_ADF
    /*
    * Added for interrupt safe mechanism for determining stack locations
    * without exposing TCB through API
    */
    BaseType_t vTaskIsActive(TaskHandle_t xTask);

    void *vTaskGetEndOfStack(TaskHandle_t xTask);

    #endif //dg_configENABLE_ADF

    ```

    - Next, navigate to sdk/FreeRTOS/src/task.c and add the functions for the prototypes above:


    ```
    #if dg_configENABLE_ADF
    /*-----------------------------------------------------------*/
    BaseType_t vTaskIsActive(TaskHandle_t xTask)
    {

        const TCB_t * const pxTCB = ( TCB_t * ) xTask;

        if(pxTCB == pxCurrentTCB)
        {
                return pdTRUE;
        }

        return pdFALSE;

    }

    void *vTaskGetEndOfStack(TaskHandle_t xTask)
    {

        const TCB_t * const pxTCB = ( TCB_t * ) xTask;

        return pxTCB->pxEndOfStack;

    }

    #endif
    ```
  - **Linker Script - Add Boot Magic Number**
    - Place the following section into section_da1469x.ld.h (sys_init_magic_num), in between hardf_fault_info and retention_mem_uninit:

    
        KEEP(\*(hard_fault_info)) <br>
        `KEEP(*(sys_init_magic_num) )` <br>
        KEEP(*(retention_mem_uninit)) <br>

  - **Hardfault and NMI Handler Modifications**
    - Navigate to sdk/peripherals/src/hw_hard_fault.c and declare Hardfault_HandlerC as __WEAK

    ```
    __WEAK void HardFault_HandlerC(unsigned long *hardfault_args)
    ```

    - Navigate to sdk/peripherals/src/hw_watchdog.c and declare hw_watchdog_handle_int as weak:

    ```
    
    ```
    


