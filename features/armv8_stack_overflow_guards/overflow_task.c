#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "osal.h"
#include "hw_wkup.h"


#define TOGGLE_MSP_OVERFLOW                     (0)     //0 Creates an application overflow in FreeRTOS task, 1 creates it on the MSP
#define BUTTON_PRESS_NOTIF                      (1 << 1)

__RETAINED  OS_TASK overFlow_handle;

static void test_overflow_func(void)
{
        test_overflow_func();
}

static void _wkup_key_cb(void)
{

        /* Clear the WKUP interrupt flag!!! */
        hw_wkup_reset_interrupt();

#if TOGGLE_MSP_OVERFLOW > 0
        test_overflow_func();
#endif

        OS_TASK_NOTIFY_FROM_ISR(overFlow_handle, BUTTON_PRESS_NOTIF,
                OS_NOTIFY_SET_BITS);
}

/* Initialize the WKUP controller */
static void _wkup_init(void)
{

        /* Initialize the WKUP controller */
        hw_wkup_init(NULL);

        /*
         * Set debounce time expressed in ms. Maximum allowable value is 63 ms.
         * A value set to 0 disables the debounce functionality.
         */
        hw_wkup_set_debounce_time(10);

        /*
         * Enable interrupts produced by the KEY block of the wakeup controller (debounce
         * circuitry) and register a callback function to hit following a KEY event.
         */
        hw_wkup_register_key_interrupt(_wkup_key_cb, 1);


        /*
         * Set the polarity (rising/falling edge) that triggers the WKUP controller.
         *
         * \note The polarity is applied both to KEY and GPIO blocks of the controller
         *
         */
        hw_wkup_configure_pin(KEY1_PORT, KEY1_PIN, 1, HW_WKUP_PIN_STATE_LOW);



        /* Enable interrupts of WKUP controller */
        hw_wkup_enable_irq();
}


/**
 * @brief OverFlowTask, for creating PSP and MSP overflows to test faults
 */
void prvTestOverFlowTask( void *pvParameters )
{
        _wkup_init();

        overFlow_handle = OS_GET_CURRENT_TASK();

        for ( ;; ) {
               OS_BASE_TYPE ret __UNUSED;
               uint32_t notif;
               /*
                * Wait on any of the notification bits, then clear them all
                */
               ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
               /* Blocks forever waiting for the task notification. Therefore, the return value must
                * always be OS_OK
                */
               OS_ASSERT(ret == OS_OK);

               /* Notified from BLE manager? */
               if (notif & BUTTON_PRESS_NOTIF) {
                       test_overflow_func();
               }
        }
}

#if dg_configARMV8_USE_STACK_GUARDS > 0


__RETAINED_CODE void UsageFault_HandlerC(uint8_t stack_pointer_mask)
{

        volatile uint16_t usage_fault_status_reg __UNUSED;

        usage_fault_status_reg = (SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) >> SCB_CFSR_USGFAULTSR_Pos;

        if(usage_fault_status_reg & (SCB_CFSR_STKOF_Msk >> SCB_CFSR_USGFAULTSR_Pos))
        {
                /*Hardware detect for MSP or PSP stack overflow */
                if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE) {
                        hw_watchdog_freeze();                           // Stop WDOG
                        while(1){}
                }
        }

#if dg_configENABLE_MTB
        /* Disable MTB */
        *MTB_MASTER_REG = MTB_MASTER_DISABLE_MSK;
#endif

        if (dg_configIMAGE_SETUP == DEVELOPMENT_MODE) {
                hw_watchdog_freeze();                           // Stop WDOG
        }

        while (1) {}
}

#endif
