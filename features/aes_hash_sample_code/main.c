/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Crypto AES/HASH Demonstration
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "hw_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

/* Required libraries for the target application */
#include "aes_hash_functionality.h"
#include "vectors.h"


/*
 * Enable/disable HASH functionality. Valid values are:
 *
 * 1 --> Create a freeRTOS task responsible for demonstrating HASH functionality.
 * 0 --> Omit creating a HASH related task.
 **/
#define HASH_CRYPTO_ENABLE     (1)


/*
 * Enable/disable AES functionality. Valid values are:
 *
 * 1 --> Create a freeRTOS task responsible for demonstrating AES functionality.
 * 0 --> Omit creating an AES related task.
 **/
#define AES_CRYPTO_ENABLE      (1)


/*
 * Enable/disable debugging aid. Valid values are:
 *
 * 1 --> Debugging messages are printed on the serial console.
 * 0 --> Disable printing debugging messages on the console.
 *
 **/
#define SERIAL_CONSOLE_ENABLE  (1)


/* Task priorities */
#define mainTEMPLATE_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )


/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware( void );

/*
 * Task functions .
 */
#if (HASH_CRYPTO_ENABLE)
static void prvHASH_CRYPTO( void *pvParameters);
__RETAINED_RW static OS_TASK hash_task_h = NULL;
#endif


#if (AES_CRYPTO_ENABLE)
static void prvAES_CRYPTO( void *pvParameters );
__RETAINED_RW static OS_TASK aes_task_h = NULL;
#endif


static OS_TASK xHandle;

static void system_init( void *pvParameters )
{

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        cm_sys_clk_init(sysclk_XTAL32M);

        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();


        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /*
         * Upon a wakeup cycle, wait for the XTAL32M crystal to settle.
         * BLE, USB and UART blocks require the XTAL32M to be up and
         * running to work properly.
         */
        pm_set_wakeup_mode(true);

        /* Set the desired sleep mode. */
        pm_sleep_mode_set(pm_mode_extended_sleep);

        /* Set the desired wakeup mode. */
        pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);


#if (HASH_CRYPTO_ENABLE)
        /*
         * Task responsible for performing HASH related operations
         */
        OS_TASK_CREATE( "HASH_CRYPTO",                  /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvHASH_CRYPTO,                 /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        1024,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                        hash_task_h );                  /* The task handle */
        OS_ASSERT(hash_task_h);
#endif


#if (AES_CRYPTO_ENABLE)
        /*
         * Task responsible for performing AES related operations
         */
        OS_TASK_CREATE( "AES_CRYPTO",                   /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        prvAES_CRYPTO,                 /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        1024,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        mainTEMPLATE_TASK_PRIORITY,     /* The priority assigned to the task. */
                        aes_task_h );                   /* The task handle */
        OS_ASSERT(aes_task_h);
#endif

        /* the work of the SysInit task is done */
        OS_TASK_DELETE( xHandle );
}

/**
 * @brief Template main creates a SysInit task, which creates a HASH and AES related tasks
 */
int main( void )
{
        OS_BASE_TYPE status;

        /* Start the two tasks as described in the comments at the top of this
        file. */
        status = OS_TASK_CREATE("SysInit",              /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        system_init,                    /* The System Initialization task. */
                        ( void * ) 0,                   /* The parameter passed to the task. */
                        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE,
                                                        /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_HIGHEST,       /* The priority assigned to the task. */
                        xHandle );                      /* The task handle */
        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);



        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
        line will never be reached.  If the following line does execute, then
        there was insufficient FreeRTOS heap memory available for the idle and/or
        timer tasks to be created.  See the memory management section on the
        FreeRTOS web site for more details. */
        for ( ;; );

}

#if (SERIAL_CONSOLE_ENABLE)
/*
 * @brief Function used for printing data on the serial console
 *
 * \param [in] buf  The physical address of the buffer containing the data
 *
 * \param [in] len  The number of bytes to print out
 *
 */
static void _print_crypto_data(uint8 *buf, uint16 len)
{
        for (int i = 0; i < len; i++) {
                /* Change to a new line every 8 bytes */
                if (!(i  % 8)) printf("\n\r");
                printf("0x%02X ", buf[i]);
        }
        printf("\n\n\r");
}
#endif


#if (AES_CRYPTO_ENABLE)
/*
 * The number of bytes the crypto engine should process
 * from the provided input vector.
 */
#define NUM_OF_AES_DATA 64

/**
 * @brief Task responsible for performing AES related operations
 */
static void prvAES_CRYPTO( void *pvParameters )
{
        AES_HASH_STATUS status;

        /* Buffer used for holding the encrypted input vector (after encryption) */
        uint8_t aes_encrypted_vector_buf[NUM_OF_AES_DATA] = { 0 };

        /* Buffer used for holding the decrypted input vector (after decryption) */
        uint8_t aes_decrypted_vector_buf[NUM_OF_AES_DATA] = { 0 };


        /* This structure holds information about the fragmented input vectors */
        const aes_hash_frag_t aes_frag_input_vector_buf[] = {
                {.src = aes_hash_frag_vect_1, .len = sizeof(aes_hash_frag_vect_1)},
                {.src = aes_hash_frag_vect_2, .len = sizeof(aes_hash_frag_vect_2)},
                {.src = aes_hash_frag_vect_3, .len = sizeof(aes_hash_frag_vect_3)},
                {.src = aes_hash_frag_vect_4, .len = sizeof(aes_hash_frag_vect_4)},
        };


        for (;;) {

                /*
                 * Perform an AES-CBC encryption operation using fragmented input data and an
                 * arbitrary symmetric 128-bit key. Key expansion operation is performed by
                 * the crypto engine and thus, only the base key is required. The AES-CBC algorithm
                 * requires an 128-bit Initialization Vector key (IV).
                 */
                status = _crypto_aes_fragmented_input(HW_AES_CBC, HW_AES_ENCRYPT,
                        key_128b, HW_AES_128, HW_AES_PERFORM_KEY_EXPANSION,
                        iv, NULL, aes_frag_input_vector_buf, aes_encrypted_vector_buf,
                        sizeof(aes_frag_input_vector_buf) / sizeof(aes_frag_input_vector_buf[0]),
                        OS_EVENT_FOREVER);

                OS_ASSERT(status == AES_HASH_STATUS_OK);


                /*
                 * Perform an AES-CBC decryption operation using non-fragmented input data and an
                 * arbitrary symmetric 128-bit key. Key expansion operation is performed by the
                 * crypto engine and thus, only the base key is required. The AES-CBC algorithm
                 * requires an 128-bit Initialization Vector key (IV).
                 *
                 * \note The previously computed encrypted data (aes_encrypted_vector_buf) are used
                 *        as the input vector.
                 */
                status = _crypto_aes_non_fragmented_input(HW_AES_CBC, HW_AES_DECRYPT,
                        key_128b, HW_AES_128, HW_AES_PERFORM_KEY_EXPANSION,
                        iv, NULL, aes_encrypted_vector_buf, aes_decrypted_vector_buf,
                        NUM_OF_AES_DATA, OS_EVENT_FOREVER);

                OS_ASSERT(status == AES_HASH_STATUS_OK);


                /* Compare the computed decrypted data with the tested input vector */
                if (!memcmp(aes_decrypted_vector_buf, aes_input_vector_buf, NUM_OF_AES_DATA)) {
                        printf("\n\rSuccessful AES Operation\n\r");
                } else {
                        printf("\n\rUnsuccessful AES Operation\n\r");
                }
                printf("\n\r");


#if (SERIAL_CONSOLE_ENABLE)
                /* Print the results on the serial console */
                printf("\n\rEncrypted data:\n\r");
                _print_crypto_data(aes_encrypted_vector_buf, NUM_OF_AES_DATA);
                printf("\n\n\rDecrypted data:\n\r");
                _print_crypto_data(aes_decrypted_vector_buf, NUM_OF_AES_DATA);
                printf("\n\n\r");
#endif


                /* Clear buffers */
                memset((void *)aes_encrypted_vector_buf, '\0',
                                                sizeof(aes_encrypted_vector_buf));
                memset((void *)aes_decrypted_vector_buf, '\0',
                                                sizeof(aes_decrypted_vector_buf));


                /* Block task execution for 2 seconds (just to let the system to enter sleep) */
                OS_DELAY_MS(2000);
        }
}

#endif


#if (HASH_CRYPTO_ENABLE)
/*
 * The number of bytes the crypto engine should store when finishing the hash operation.
 */
#define NUM_OF_HASH_DATA  (32)

/**
 * @brief Task responsible for performing HASH related operations
 */
static void prvHASH_CRYPTO( void *pvParameters )
{
        AES_HASH_STATUS status;

        /* Buffer used for holding the hashing data (after hash operation) */
        uint8 hash_data_out_buf[NUM_OF_HASH_DATA] = { 0 };


        for (;;) {

                /* Perform a SHA-256 hash operation using non-fragmented data */
                status = _crypto_hash_non_fragmented_input(HW_HASH_SHA_256, NUM_OF_HASH_DATA,
                               hash_input_vector, hash_data_out_buf,
                               (sizeof(hash_input_vector) - 1), OS_EVENT_FOREVER);

                OS_ASSERT(status == AES_HASH_STATUS_OK);


                /* Compare the results with the pre calculated hashing data */
                if (!memcmp(hash_data_out_buf, sha_256_hash, NUM_OF_HASH_DATA)) {
                        printf("\n\rSuccessful HASH Operation\n\r");
                } else {
                        printf("\n\rUnsuccessful HASH Operation\n\r");
                }
                printf("\n\r");


#if (SERIAL_CONSOLE_ENABLE)
                /* Print the results on the serial console */
                printf("\n\rHashing data:\n\r");
                _print_crypto_data(hash_data_out_buf, NUM_OF_HASH_DATA);
                printf("\n\n\r");
#endif

                /* Block task execution for 2 seconds (just to let the system to enter sleep) */
                OS_DELAY_MS(2000);
        }
}
#endif


/**
 * @brief Initialize the peripherals domain after power-up.
 *
 */
static void periph_init(void)
{
}

/**
 * @brief Hardware Initialization
 */
static void prvSetupHardware( void )
{

        /* Init hardware */
        pm_system_init(periph_init);

}

/**
 * @brief Malloc fail hook
 */
void vApplicationMallocFailedHook( void )
{
        /* vApplicationMallocFailedHook() will only be called if
        configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
        function that will get called if a call to OS_MALLOC() fails.
        OS_MALLOC() is called internally by the kernel whenever a task, queue,
        timer or semaphore is created.  It is also called by various parts of the
        demo application.  If heap_1.c or heap_2.c are used, then the size of the
        heap available to OS_MALLOC() is defined by configTOTAL_HEAP_SIZE in
        FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
        to query the size of free heap space that remains (although it does not
        provide information on how the remaining heap might be fragmented). */
        ASSERT_ERROR(0);
}

/**
 * @brief Application idle task hook
 */
void vApplicationIdleHook( void )
{
        /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
        to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
        task.  It is essential that code added to this hook function never attempts
        to block in any way (for example, call OS_QUEUE_GET() with a block time
        specified, or call OS_DELAY()).  If the application makes use of the
        OS_TASK_DELETE() API function (as this demo application does) then it is also
        important that vApplicationIdleHook() is permitted to return to its calling
        function, because it is the responsibility of the idle task to clean up
        memory allocated by the kernel to any task that has since been deleted. */
}

/**
 * @brief Application stack overflow hook
 */
void vApplicationStackOverflowHook( OS_TASK pxTask, char *pcTaskName )
{
        ( void ) pcTaskName;
        ( void ) pxTask;

        /* Run time stack overflow checking is performed if
        configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
        function is called if a stack overflow is detected. */
        ASSERT_ERROR(0);
}

/**
 * @brief Application tick hook
 */
void vApplicationTickHook( void )
{
}
