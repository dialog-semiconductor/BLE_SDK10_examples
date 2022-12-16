# ARMV8 STACK OVERFLOW GUARDS
This example illustrates how to use the armv8 MSP and PSP limit registers to use hardware to protect against stack overflows, [ARMv8 Register Summary](https://developer.arm.com/documentation/100230/0004/functional-description/programmers-model/processor-core-registers-summary)

---


Historically, stack overflows have plagued the embedded development process and have wasted engineering hours trying to root causing the stack overflow itself. Software solutions have been implemented to detect these conditions, but still fall short of detecting all stack overflows, under all conditions.

FreeRTOS utilizes the 'magic number' approach.  They write a signature to each tasks stack location, and then check the last word in the stack, to see if it is been over-written. They also implement a high watermark reading, where they scan the stack to see the max depth at any given time.   There are several downfalls of this method:

1.  FreeRTOS only checks the watermark on a context switch.  This means, if a thread overflows the stack, and isn't yielding, i.e. causing a context switch, it can corrupt memory, access null pointers, etc.  These errors will manifest in many different fashions.  

2.  FreeRTOS does not do any checking on the MSP.  Meaning, that you can overflow the stack when in an exception or interrupt context, without any detection. 

3. FreeRTOS does not recommend using this feature in production environments because of the context switch overhead: [FreeRTOS Stack and Overflow Checking](https://www.freertos.org/Stacks-and-stack-overflow-checking.html)

With the changes and patches below, we can use hardware available in the M33F to protect both the MSP and PSP.

To learn more about the MSP, PSP and FreeRTOS context switcher, please reference Memfault's Interrupt blog:  https://interrupt.memfault.com/blog/cortex-m-rtos-context-switching


## HW & SW Configurations

- **Hardware Configurations**
  - This example runs on a DA1469x Bluetooth Smart SoC.
  - A DA1469x Pro Development Kit or USB kit is needed for this example (KEY1_PORT and KEY1_PIN should be modified for USB Kit functionality).
- **Software Configurations**
  - Download the latest SDK version for the DA1469x family of devices (10.0.12.x)
  - **SEGGER's J-Link** tools should be downloaded and installed.

## SDK Modifications

You can apply the modification to the SDK in order to support the MSP/PSP Overflow feature by using the following command:

```console
  > cd <SDKROOT>
  > patch -p1 < <SDK_EXAMPLE_ROOT>/features/features/armv8_stack_overflowguards/armv8_stack_overflow_guards.patch 
```

Alternatively you can apply the modification manually

- **Add MSP Protection in the assembly startup**

  Navigate to startup/DA1469x/GCC/exception_handlers.s and make the following modifications:

  First, add the following lines to the start of the Wakeup_Reset_Handler:

  ```c
  Wakeup_Reset_Handler:

  /*
  The MSPLIM register is not retained in sleep, we must restore it on wakeup or from 
  reset
  */
  #if dg_configARMV8_USE_STACK_GUARDS == 1
      ldr		r0, =__StackLimit
      add		r0, r0, #dg_configMSP_PADDING
      msr		MSPLIM, r0
  #endif

  ```

  Next, add the initialization of the MSPLIM register in the Reset_Handler (After the str r1, [r0]):

  ```c

  Reset_Handler:
                  ldr     r0, =0x20000004
                  ldr     r1, =Wakeup_Reset_Handler
                  str     r1, [r0]
  #if dg_configARMV8_USE_STACK_GUARDS == 1
  /*
  * Intialize the MSPLIM register.  Must be placed here as the msr instruction
  * requires the device to be in privileged mode. Add padding to the register
  * so we can leave room for the UsageFault_Handler to place items on the stack
  */
          ldr		r0, =__StackLimit
          add		r0, r0, #dg_configMSP_PADDING
          msr		MSPLIM, r0
  #endif
  ```

  Last, we need to add the UsageFault_Handler in assembly, place this as a seperate handler in the file:
 ```c

  #if dg_configARMV8_USE_STACK_GUARDS == 1
#if (dg_configCODE_LOCATION == NON_VOLATILE_IS_FLASH)
            .section text_retained
#endif
             .align  2
             .thumb
             .thumb_func
             .globl  UsageFault_Handler
             .type   UsageFault_Handler, %function
 UsageFault_Handler:
 /*
	 * We need special handling for the UsageFault_Handler with the use of the MSPLIM register set.  First we check to
	 * see if the MSPLIM or the PSPLIM registers equal the msp or psp and set the stack pointer mask respectively.
	 * If the MSPLIM register triggered the usagefault, then we must move the MSPLIM back below the padding to allow for the
	 * UsageFault_HandlerC to push to the stack if needed.  If the MSPLIM is not moved, then a push to the stack can cause
	 * nested UsageFaults and circular exceptions
	 */
		       ldr		r2,=UsageFault_HandlerC
		       mrs		r1, msp
		       mrs		r0, MSPLIM
		       cmp		r0, r1
		       beq		UsageFault_with_MSP_Overflow
		       mrs		r1, psp
		       mrs		r0, PSPLIM
		       cmp		r0, r1
		       beq 		UsageFault_with_PSP_Overflow
		       mov		r0, #0
		       bx		r2

 UsageFault_with_PSP_Overflow:
		       mov 		r0, #2
		       bx		r2

 UsageFault_with_MSP_Overflow:
		       ldr 		r1, =__StackLimit
		       msr		MSPLIM, r1
		       mov		r0, #1
		       bx		r2
 #endif

  ```

- **FreeRTOS Modifications - for PSP protection**
We need to make two small changes to FreeRTOS to allow for switching out the PSP.  Navigate to sdk/FreeRTOS/tasks.c

Add the following line of code above *vTaskSwitchContext*:  


```c
#if dg_configARMV8_USE_STACK_GUARDS == 1

void vTaskSwitchStackGuard(void)
{
        volatile uint32_t end_of_stack_val = (uint32_t)pxCurrentTCB->pxStack;
     __set_PSPLIM( end_of_stack_val);
}

#endif //#if dg_configARMV8_USE_STACK_GUARDS == 1

```
Next, modify the xPortPendSVHandler to switch in the PSPLIM guards, in sdk/FreeRTOS/protable/GCC/DA1469x/port.c:

```diff

	"	bl vTaskSwitchContext				\n"
+#if dg_configARMV8_USE_STACK_GUARDS == 1
+ "     bl vTaskSwitchStackGuard                        \n"
+ #endif
	"	mov r0, #0

```

- **Fault Handler Modification**
Last, modify the UsageFault_Handler in peripherals/hw_hard_fault.c to have a weak attribute:

```c
__WEAK void UsageFault_Handler(void)
```




## Running the example

### Initial Setup

- Clone the Source code from Github.
- Import the project into your workspace.
- Connect the target device to your host PC.
- Compile the code (either in Release or Debug Mode)  and load it onto the chip.
- Place a breakpoint in the UsageFault_Handler in overflow_task.c
- Short Press the K1 Button on the Pro Kit to trigger an MSP or PSP overflow.   


### Modifying the example to generate other issues.

The example uses a recursive function call to illustrate the limitations of FreeRTOS and software solutions.  We can modify the example with switches:

  - in overflow_task.c, there is a define - TOGGLE_MSP_OVERFLOW. (0) Will create a PSP overflow, and (1) will create an MSP overflow.
  - In custom_config_xxx.h, you can #undef dg_configARMV8_USE_STACK_GUARDS, and this will illustrate what happens when we don't use the hardware guards.
  - If you modify the UsageFault_HandlerC, and you need more stack than 2 spots on the stack, you can modify  dg_configMSP_PADDING.
  



## Porting to Your Own Application

  1. Patch or Modify your SDK  
  2. Add the two configurations to your custom_config_qspi/ram.h
```c
    #define dg_configARMV8_USE_STACK_GUARDS         (1)
    #define dg_configMSP_PADDING                    (16)
```
  3. The assembly code checks the MSP and PSP against the MSPLIM and PSPLIM, and gives a mask as an argument into UsageFault_HandlerC. 0 indicates that an overflow did not occur, 1 MSP overflow, and 2, PSP Overflow.