/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief FreeRTOS template application with retarget
 *
 * Copyright (C) 2020 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "osal.h"
#include "resmgmt.h"
#include "hw_cpm.h"
#include "hw_gpio.h"
#include "sys_watchdog.h"
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"
#include "hw_otpc.h"

/* Required header files for using crypto engine */
#include "ad_crypto.h"
#include "hw_aes.h"
#include "hw_hash.h"
#include "vectors_keys.h"

/* Task priorities */
#define mainAES_TASK_PRIORITY              	( OS_TASK_PRIORITY_NORMAL )

/* MACROS for enabling/disabling tasks, ONE AT A TIME!*/
#define AES_CBC_128_NON_FRAG_DATA               (0)
#define AES_CTR_192_FRAG_DATA                   (0)
#define AES_CBC_256_NON_FRAG_DATA               (1)
#define HASH_SHA_256                            (0)
#define OTP_KEY					(0x30080A00)	// User Data Encryption Keys – Payload

#if (dg_configAES_USE_OTP_KEYS == 1)
uint8_t * const key_otp = (uint8_t *) OTP_KEY;		  // Key in OTP
void static aes_construct_words(uint32_t *out_data, const uint8_t *in_data, uint8_t length);
void aes_check_otp_keys(void);
#else
uint8_t * const key_otp = (uint8_t *) key_256b;			// Key in RAM
#endif /* dg_configAES_USE_OTP_KEYS */

/* Task priorities */

#define mainTEMPLATE_TASK_PRIORITY		( OS_TASK_PRIORITY_NORMAL )

/* The configCHECK_FOR_STACK_OVERFLOW setting in FreeRTOSConifg can be used to
 check task stacks for overflows.  It does not however check the stack used by
 interrupts.  This demo has a simple addition that will also check the stack used
 by interrupts if mainCHECK_INTERRUPT_STACK is set to 1.  Note that this check is
 only performed from the tick hook function (which runs in an interrupt context).
 It is a good debugging aid - but won't catch interrupt stack problems until the
 tick interrupt next executes. */

//#define mainCHECK_INTERRUPT_STACK			1
#if mainCHECK_INTERRUPT_STACK == 1
const unsigned char ucExpectedInterruptStackValues[] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};
#endif

/*
 * Perform any application specific hardware configuration.  The clocks,
 * memory, etc. are configured before main() is called.
 */
static void prvSetupHardware(void);

/*
 * Task functions .
 */
#if (AES_CTR_192_FRAG_DATA)
static void prvAES_CTR_192_FRAG_DATA( void *pvParameters );
#endif
#if (AES_CBC_128_NON_FRAG_DATA)
static void prvAES_CBC_128_NON_FRAG_DATA( void *pvParameters );
#endif
#if (AES_CBC_256_NON_FRAG_DATA)
static void prvAES_CBC_256_NON_FRAG_DATA(void *pvParameters);
#endif
#if (HASH_SHA_256)
static void prvHASH_SHA_256( void *pvParameters );
#endif

static OS_TASK xHandle;

static void system_init(void *pvParameters)
{

#if defined CONFIG_RETARGET
        extern void retarget_init(void);
#endif

        /* Prepare clocks. Note: cm_cpu_clk_set() and cm_sys_clk_set() can be called only from a
         * task since they will suspend the task until the XTAL16M has settled and, maybe, the PLL
         * is locked.
         */
        cm_sys_clk_init(sysclk_XTAL32M);
        cm_apb_set_clock_divider(apb_div1);
        cm_ahb_set_clock_divider(ahb_div1);
        cm_lp_clk_init();

        /* Initialize platform watchdog */
        sys_watchdog_init();

#if dg_configUSE_WDOG
        /* Register the Idle task first */
        idle_task_wdog_id = sys_watchdog_register(false);
        ASSERT_WARNING(idle_task_wdog_id != -1);
        sys_watchdog_configure_idle_id(idle_task_wdog_id);
#endif

        /* Prepare the hardware to run this demo. */
        prvSetupHardware();

        /* init resources */
        resource_init();

#if defined CONFIG_RETARGET
        retarget_init();
#endif

        /* Set the desired sleep mode. */
        pm_set_wakeup_mode(true);
        pm_sleep_mode_set(pm_mode_active);

#if (dg_configAES_USE_OTP_KEYS == 1)
        hw_otpc_init();
        aes_check_otp_keys();
#endif

#if (dg_configAES_USE_SECURE_DMA_CHANNEL == 1)

        /*
         * If the encryption key(s) need to be secured than this bit needs to be set at startup in the CS (Configuration Script)
         * For test purposes it is set here.
         * For test without secured key(s), just set this flag to (0)
         *
         * Note: This bit is write-only (sticky bit) and can only be reset by a HW- or a POReset.
         *
         * */

        REG_SETF(CRG_TOP, SECURE_BOOT_REG, PROT_AES_KEY_READ, 1);
#endif
        /* Create a task responsible for AES encryption/decryption operations */
#if (AES_CBC_128_NON_FRAG_DATA)
        OS_TASK ctrypto_non_frag = NULL;

        OS_TASK_CREATE( "CRYPTO_NON_FRAG",
                prvAES_CBC_128_NON_FRAG_DATA,
                NULL,
                200 * OS_STACK_WORD_SIZE,
                mainAES_TASK_PRIORITY,
                ctrypto_non_frag );
        OS_ASSERT(ctrypto_non_frag);
#endif

        /* Create a task responsible for AES encryption/decryption operations */
#if (AES_CBC_256_NON_FRAG_DATA)
        OS_TASK ctrypto_non_frag = NULL;

        OS_TASK_CREATE("CRYPTO_NON_FRAG",
                prvAES_CBC_256_NON_FRAG_DATA,
                NULL,
                400 * OS_STACK_WORD_SIZE,
                mainAES_TASK_PRIORITY,
                ctrypto_non_frag);
        OS_ASSERT(ctrypto_non_frag);
#endif

        /* Create a task responsible for AES encryption/decryption operations */
#if (AES_CTR_192_FRAG_DATA)
        OS_TASK ctrypto_frag = NULL;

        OS_TASK_CREATE( "CRYPTO_FRAG",
                prvAES_CTR_192_FRAG_DATA,
                NULL,
                200 * OS_STACK_WORD_SIZE,
                mainAES_TASK_PRIORITY,
                ctrypto_frag );
        OS_ASSERT(ctrypto_frag);
#endif

        /* Create a task responsible for SHA-512 operation */
#if (HASH_SHA_256)
        OS_TASK hash_sha_256 = NULL;

        OS_TASK_CREATE( "CRYPTO_FRAG",
                prvHASH_SHA_256,
                NULL,
                200 * OS_STACK_WORD_SIZE,
                mainAES_TASK_PRIORITY,
                hash_sha_256 );
        OS_ASSERT(hash_sha_256);
#endif

        /* the work of the SysInit task is done */
        OS_TASK_DELETE(xHandle);
}

#if (dg_configAES_USE_OTP_KEYS == 1)
void static aes_construct_words(uint32_t *out_data, const uint8_t *in_data, uint8_t length)
{
        uint8_t i;
        for (i = 0; i < length; i += 4) {
                out_data[i / 4] = ((in_data[i + 3] & 0xff) | (in_data[i + 2] << 8 & 0xff00) |
                        (in_data[i + 1] << 16 & 0xff0000) | (in_data[i] << 24 & 0xff000000));
        }
}

void aes_check_otp_keys(void)
{
        uint32_t otp_data;

        otp_data = hw_otpc_word_read(MEMORY_OTP_USER_DATA_KEYS_PAYLOAD_START / 4);

        /*
         * Check if OTP Key area is empty. If it is empty (0xffffffff), the 256-bit key (key_256b[32]) is programmed into
         * the OTP
         *
         *  */

        if (otp_data == 0xFFFFFFFF)
                {
                uint32_t aes_key[8];
                /* write keys into otp */
                aes_construct_words(aes_key, key_256b, 32);
                hw_otpc_prog_and_verify(aes_key, (MEMORY_OTP_USER_DATA_KEYS_PAYLOAD_START / 4), 8);
        }
}
#endif

int main(void)
{
        OS_BASE_TYPE status;

        /* Start the initialization task. */
        status = OS_TASK_CREATE("SysInit", 					/* The text name assigned to the task, for debug only; not used by the kernel. */
        system_init, 										/* The System Initialization task. */
        ( void * ) 0, 										/* The parameter passed to the task. */
        configMINIMAL_STACK_SIZE * OS_STACK_WORD_SIZE, 		/* The number of bytes to allocate to the stack of the task. */
        OS_TASK_PRIORITY_HIGHEST,							/* The priority assigned to the task. */
        xHandle); 											/* The task handle */

        OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);

        /* Start the tasks and timer running. */
        vTaskStartScheduler();

        /* If all is well, the scheduler will now be running, and the following
         line will never be reached.  If the following line does execute, then
         there was insufficient FreeRTOS heap memory available for the idle and/or
         timer tasks	to be created.  See the memory management section on the
         FreeRTOS web site for more details. */
        for (;;)
                ;

}

#if  (HASH_SHA_256 == 1)

/*
 * Number of bytes of the hash result which will be saved at the memory by the DMA. In relation with the selected hash algorithm the accepted values are:
 * MD5:          0..15 --> from 1 to 16 bytes
 * SHA-1:        0..19 --> from 1 to 20 bytes
 * SHA-256:      0..31 --> from 1 to 32 bytes
 * SHA-256/224:  0..27 --> from 1 to 28 bytes
 * SHA-384:      0..47 --> from 1 to 48 bytes
 * SHA-512:      0..63 --> from 1 to 64 bytes
 * SHA-512/224:  0..27 --> from 1 to 28 bytes
 * SHA-512/256:  0..31 --> from 1 to 32 bytes
 */
#define NUM_OF_HASH_DATA  (32)

/* This task performs a SHA-256 hashing operation */
static void prvHASH_SHA_256( void *pvParameters )
{
        OS_BASE_TYPE status;
        unsigned int aes_hash_status;

        uint8_t hash_data_rd[NUM_OF_HASH_DATA] = {0}; 		/*  Destination buffer - Output result */
        uint8_t hash_data_wd[] = "This is confidential!";	/* Data to be processed - Source buffer */

        printf("256-bit SHA task started.\r\n");

        for (;;) {

                /* The task is blocked for 2 seconds */
                OS_DELAY(OS_MS_2_TICKS(2000));

                /* Wait until the CRYPTO engine is acquired */
                status = ad_crypto_acquire_aes_hash(OS_EVENT_FOREVER);
                OS_ASSERT(status == OS_MUTEX_TAKEN);

                /* For debugging purposes */
                hw_gpio_set_active(HW_GPIO_PORT_1, HW_GPIO_PIN_5);

                /*
                 * Enable event signaling and start AES/HASH engine clock, as well. This function enables crypto IRQ. Please note
                 * that crypto IRQ is shared with AES/HASH and ECC engines.
                 */
                ad_crypto_enable_aes_hash_event();

                /*
                 * Configure the engine to perform a SHA-512 hashing. Define the number in bytes of the result that the engine will write
                 * to the output memory. This value depends on the selected HASH algorithm. Here developer shall select another hashing function.
                 */
                hw_hash_set_type(HW_HASH_TYPE_SHA_256);
                hw_hash_set_output_data_len(HW_HASH_TYPE_SHA_256, HW_HASH_OUTPUT_LEN_MAX_SHA_256);

                /* This function configures the DMA machine with the data to be processed and the destination buffer where results are stored */
                hw_aes_hash_set_input_data_addr((uint32_t)hash_data_wd);
                hw_aes_hash_set_output_data_addr((uint32_t)hash_data_rd);
                hw_aes_hash_set_input_data_len(sizeof(hash_data_wd) - 1);

                /* Start the AES/HASH engine */
                hw_aes_hash_start();

                /*
                 * Wait until operation is finished. A callback is triggered, indicating that the AES/HASH engine has finished
                 * with the correct operation.
                 */
                status = ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, &aes_hash_status);
                OS_ASSERT(status == OS_OK);

                /* Compare results with the already precalculated ones -They should match */
                if ( !memcmp(hash_data_rd, sha_256_hash, NUM_OF_HASH_DATA) ) {

                        printf("\n\rHashing SHA-256 operation has been executed successfully!\n\r");
                } else {
                        printf("Unsuccessful Hashing SHA-256 operation!\n\r");
                }

                /* Disable event signaling and the AES/HASH engine clock, as well */
                ad_crypto_disable_aes_hash_event();

                /* It must ALWAYS be called after the resource has been acquired and as soon as it is no longer needed by the task */
                status = ad_crypto_release_aes_hash();
                OS_ASSERT(status == OS_OK);

                /* For debugging purposes */
                hw_gpio_set_inactive(HW_GPIO_PORT_1, HW_GPIO_PIN_5);
        }
}
#endif

/* This macro defines the total number of bytes of the test vector. */

#define NUMBER_OF_DATA   (64)

/* This macro defines the chunked number of bytes */
#define NUMBER_OF_BYTE_PER_CHUNK  (16)

#if  (AES_CBC_128_NON_FRAG_DATA == 1)
/*
 * This task is executed every 2.5 seconds and performs a AES encryption/decryption operation using non fragmented data, that is,
 * the AES/HASH engine process data from one memory location. After processing a data block the engine is set in inactive state.
 */
static void prvAES_CBC_128_NON_FRAG_DATA( void *pvParameters )
{
        OS_BASE_TYPE status;
        unsigned int aes_hash_status = 0;

        /* This buffer holds the encrypted input vector after the encryption */
        static uint8_t non_frag_encr_data[NUMBER_OF_DATA];

        /* This buffer holds the decrypted input vector after the decryption */
        static uint8_t non_frag_decr_data[NUMBER_OF_DATA];

        printf("128 bit AES encryption & decryption task started.\r\n");

        for (;;) {

                /* The task is blocked for 2.5 seconds */
                OS_DELAY(OS_MS_2_TICKS(2500));

                /* Wait until the CRYPTO engine is acquired */
                status = ad_crypto_acquire_aes_hash(OS_EVENT_FOREVER);
                OS_ASSERT(status == OS_MUTEX_TAKEN);

                /* For debugging purposes */
                hw_gpio_set_active(HW_GPIO_PORT_1, HW_GPIO_PIN_2);

                /* Enable event signaling and start the AES/HASH engine clock as well */
                ad_crypto_enable_aes_hash_event();

                /* --------From this point onwards, Low Level Drivers are used for controlling and performing AES/HASH operations-------------- */

                /* Configure engine to perform an AES CBC encryption/decryption operation. Here developer shall select another AES mode. */
                hw_aes_set_mode(HW_AES_MODE_CBC);
                hw_aes_set_key_size(HW_AES_KEY_SIZE_128);

#if (dg_configAES_USE_OTP_KEYS == 1)
                hw_aes_otp_keys_load(HW_AES_128, key_otp);
#else
                hw_aes_load_keys((uint32_t)key_otp, HW_AES_KEY_SIZE_128, HW_AES_KEY_EXPAND_BY_HW);
#endif //#if (dg_configAES_USE_OTP_KEYS == 1)

                /* ------------------------------------------------------ Encryption process ------------------------------------------------- */

                /*
                 * This function is used to configure the engine so as to consider the next input block as the last of the operation (since this task
                 * handles non fragmented data)
                 */
                hw_aes_hash_set_input_data_mode(false);

                /* This function stores the initialization vector (IV) that is necessary for AES CBC mode */
                hw_aes_set_init_vector(iv);

                /* This function configures the DMA machine with the data to be processed and the destination buffer where results are stored */
                hw_aes_hash_set_input_data_addr((uint32_t)vector);
                hw_aes_hash_set_output_data_addr((uint32_t)non_frag_encr_data);
                hw_aes_hash_set_input_data_len(NUMBER_OF_DATA);

                /* Start a AES encryption operation */
                hw_aes_start_operation(HW_AES_OPERATION_ENCRYPT);

                /* Wait for the crypto IRQ  indicating that the task has been executed */
                status = ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, &aes_hash_status);
                OS_ASSERT(status == OS_EVENT_SIGNALED);

                /*  If str1 == str2 then it returns zero */
                if ( !memcmp(non_frag_encr_data, cbc_128b_cipher, NUMBER_OF_DATA) ) {

                        printf("\n\rSuccessful 128-bit AES CBC encryption! AES STATUS: %d\n\r", aes_hash_status);
                } else {
                        printf("\n\rUnsuccessful 128-bit AES CBC encryption! AES STATUS: %d\n\r", aes_hash_status);
                }
                /* ------------------------------------------------- Encryption process ---------------------------------------------------- */

                /* ------------------------------------------------- Decryption process ---------------------------------------------------- */
                hw_aes_hash_set_input_data_mode(false);
                hw_aes_set_init_vector(iv);

                /* Configure DMA with new data source/destination */
                hw_aes_hash_set_input_data_addr((uint32_t)non_frag_encr_data);
                hw_aes_hash_set_output_data_addr((uint32_t)non_frag_decr_data);
                hw_aes_hash_set_input_data_len(NUMBER_OF_DATA);

                /* Start a AES decryption operation */
                hw_aes_start_operation(HW_AES_OPERATION_DECRYPT);

                /* Wait for the crypto IRQ  indicating that the task has been executed */
                status = ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, &aes_hash_status);
                OS_ASSERT(status == OS_EVENT_SIGNALED);

                /* Compare results with the already precalculated ones - They should match */
                if ( !memcmp(non_frag_decr_data, vector, NUMBER_OF_DATA) ) {

                        printf("Successful 128-bit AES CBC decryption! AES STATUS: %d\n\r", aes_hash_status);
                } else {
                        printf("Unsuccessful 128-bit AES CBC decryption! AES STATUS: %d\n\r", aes_hash_status);
                }
                /* -------------------------------------------------------- Decryption process ------------------------------------------------ */

                /* Disable event signaling and the AES/HASH engine clock as well */
                ad_crypto_disable_aes_hash_event();

                /* It must ALWAYS be called after the resource has been acquired and as soon as it is no longer needed by the task */
                status = ad_crypto_release_aes_hash();
                OS_ASSERT(status == OS_OK);

                /* For debugging purposes */
                hw_gpio_set_inactive(HW_GPIO_PORT_1, HW_GPIO_PIN_2);

        }

}
#endif

#if  (AES_CBC_256_NON_FRAG_DATA == 1)
/*
 * This task is executed every 2.5 seconds and performs a AES encryption/decryption operation using non fragmented data, that is,
 * the AES/HASH engine process data from one memory location. After processing a data block the engine is set in inactive state.
 */
static void prvAES_CBC_256_NON_FRAG_DATA(void *pvParameters)
{
        OS_BASE_TYPE status;
        unsigned int aes_hash_status = 0;

        /* This buffer holds the encrypted input vector after the encryption */
        static uint8_t non_frag_encr_data[NUMBER_OF_DATA];

        /* This buffer holds the decrypted input vector after the decryption */
        static uint8_t non_frag_decr_data[NUMBER_OF_DATA];

        printf("256 bit AES encryption & decryption task started.\r\n");

        for (;;) {

                /* The task is blocked for 2.5 seconds */
                OS_DELAY(OS_MS_2_TICKS(2500));

                /* Wait until the CRYPTO engine is acquired */
                status = ad_crypto_acquire_aes_hash(OS_EVENT_FOREVER);
                OS_ASSERT(status == OS_MUTEX_TAKEN);

                /* Enable event signaling and start the AES/HASH engine clock as well */
                ad_crypto_enable_aes_hash_event();

                /* --------From this point onwards, Low Level Drivers are used for controlling and performing AES/HASH operations-------------- */

                /* Configure engine to perform an AES CBC encryption/decryption operation. Here developer shall select another AES mode. */
                hw_aes_set_mode(HW_AES_MODE_CBC);
                hw_aes_set_key_size(HW_AES_KEY_SIZE_256);

                /* Store the base key in AES/HASH engine memory and configure engine to perform key expansion, as well */
#if (dg_configAES_USE_OTP_KEYS == 1)
                hw_aes_otp_keys_load(HW_AES_256, key_otp);
#else
                hw_aes_load_keys((uint32_t)key_otp, HW_AES_KEY_SIZE_256, HW_AES_KEY_EXPAND_BY_HW);
#endif //#if (dg_configAES_USE_OTP_KEYS == 1)
                /* ------------------------------------------------------ Encryption process ------------------------------------------------- */

                /*
                 * This function is used to configure the engine so as to consider the next input block as the last of the operation (since this task
                 * handles non fragmented data)
                 */
                hw_aes_hash_set_input_data_mode(false);

                /* This function stores the initialization vector (IV) that is necessary for AES CBC mode */
                hw_aes_set_init_vector(iv);

                /* This function configures the DMA machine with the data to be processed and the destination buffer where results are stored */
                hw_aes_hash_set_input_data_addr((uint32_t)vector);
                hw_aes_hash_set_output_data_addr((uint32_t)non_frag_encr_data);
                hw_aes_hash_set_input_data_len(NUMBER_OF_DATA);

                /* Start a AES encryption operation */
                hw_aes_start_operation(HW_AES_OPERATION_ENCRYPT);

                /* Wait for the crypto IRQ  indicating that the task has been executed */
                status = ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, &aes_hash_status);
                OS_ASSERT(status == OS_EVENT_SIGNALED);

                /*  If str1 == str2 then it returns zero */
                if (!memcmp(non_frag_encr_data, cbc_256b_cipher, NUMBER_OF_DATA)) {

                        printf("\n\rSuccessful 256-bit AES CBC encryption! AES STATUS: %d\n\r",
                                aes_hash_status);
                } else {
                        printf("\n\rUnsuccessful 256-bit AES CBC encryption! AES STATUS: %d\n\r",
                                aes_hash_status);
                }

                /* ------------------------------------------------- Decryption process ---------------------------------------------------- */

                hw_aes_hash_set_input_data_mode(false);
                hw_aes_set_init_vector(iv);

                /* Configure DMA with new data source/destination */
                hw_aes_hash_set_input_data_addr((uint32_t)non_frag_encr_data);
                hw_aes_hash_set_output_data_addr((uint32_t)non_frag_decr_data);
                hw_aes_hash_set_input_data_len(NUMBER_OF_DATA);

                /* Start a AES decryption operation */
                hw_aes_start_operation(HW_AES_OPERATION_DECRYPT);

                /* Wait for the crypto IRQ  indicating that the task has been executed */
                status = ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, &aes_hash_status);
                OS_ASSERT(status == OS_EVENT_SIGNALED);

                /* Compare results with the already precalculated ones - They should match */
                if (!memcmp(non_frag_decr_data, vector, NUMBER_OF_DATA)) {

                        printf("Successful 256-bit AES CBC decryption! AES STATUS: %d\n\r",
                                aes_hash_status);
                } else {
                        printf("Unsuccessful 256-bit AES CBC decryption! AES STATUS: %d\n\r",
                                aes_hash_status);
                }
                /* -------------------------------------------------------- Decryption process ------------------------------------------------ */

                /* Disable event signaling and the AES/HASH engine clock as well */
                ad_crypto_disable_aes_hash_event();

                /* It must ALWAYS be called after the resource has been acquired and as soon as it is no longer needed by the task */
                status = ad_crypto_release_aes_hash();
                OS_ASSERT(status == OS_OK);
        }
}
#endif

#if   (AES_CTR_192_FRAG_DATA == 1)
/*
 * This task is executed every 2.5 seconds and performs a AES encryption/decryption operation using fragmented data,
 * that is, the AES/HASH engine can process data from different locations in the memory. In specific, after processing a first data block
 * then the engine waits until new data block is declared. In this example, we just slice input vector into smaller data chunks.
 */
static void prvAES_CTR_192_FRAG_DATA( void *pvParameters )
{
        OS_BASE_TYPE status;
        unsigned int aes_hash_status = 0;

        /* This buffer holds the encrypted input vector after the encryption */
        uint8_t frag_encr_data[NUMBER_OF_DATA];

        /* This buffer holds the decrypted input vector after the decryption */
        uint8_t frag_decr_data[NUMBER_OF_DATA];

        printf("192 bit AES encryption & decryption task started.\r\n");

        for (;;) {

                /* The task is blocked for 2.5 seconds */
                OS_DELAY(OS_MS_2_TICKS(2500));

                /* Wait until the CRYPTO engine is acquired */
                status = ad_crypto_acquire_aes_hash(OS_EVENT_FOREVER);
                OS_ASSERT(status == OS_MUTEX_TAKEN);

                hw_gpio_set_active(HW_GPIO_PORT_1, HW_GPIO_PIN_4);

                /*
                 * Enable event signaling and start the AES/HASH engine clock as well. This function enables crypto IRQ. Please take
                 * care that tis IRQ is shared with AES/HASH and ECC engines.
                 */
                ad_crypto_enable_aes_hash_event();

                /* --------From this point onwards, Low Level Drivers are used for controlling and performing AES/HASH operations-------------- */

                /* Configure engine for AES CTR encryption/decryption. Here developer shall select another AES mode. */
                hw_aes_set_mode(HW_AES_MODE_CTR);
                hw_aes_set_key_size(HW_AES_KEY_SIZE_192);

                /*
                 * This function stores the keys used for AES operations. If key expansion is performed by the engine then the provided key
                 * must be the base key only. Key expansion bit is cleared after each AES/HASH operation, thus it must be redifined at each operation.
                 */

               #if (dg_configAES_USE_OTP_KEYS == 1)
                hw_aes_otp_keys_load(HW_AES_192, key_otp);
#else
                hw_aes_load_keys((uint32_t)key_otp, HW_AES_KEY_SIZE_192, HW_AES_KEY_EXPAND_BY_HW);
#endif //#if (dg_configAES_USE_OTP_KEYS == 1)

                /* --------------------------------------------- Encryption process ---------------------------------------------------- */

                /*
                 * Mark the AES/HASH engine that this is not the last input block to be processed. This must ALWAYS be called to tell AES/HASH
                 * engine that more data will follow after the first encryption/decryption operation.
                 */
                hw_aes_hash_set_input_data_mode(true);


                /* This function stores the initialization vector (IV) that is necessary for AES CBC mode */
                hw_aes_set_init_vector(ic);

                /* This function configures the DMA machine with the data to be processed and the destination buffer where results are stored */
                hw_aes_hash_set_input_data_addr((uint32_t)&vector[0]);
                hw_aes_hash_set_output_data_addr((uint32_t)frag_encr_data);
                hw_aes_hash_set_input_data_len(NUMBER_OF_BYTE_PER_CHUNK);

                /* Start a AES encryption operation */
                hw_aes_start_operation(HW_AES_OPERATION_ENCRYPT);

                /*
                 * Wait until operation is finished. A callback is triggered, indicating that the AES/HASH engine has finished
                 * with the correct operation.
                 */
                status = ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, &aes_hash_status);
                OS_ASSERT(status == OS_OK);

                /* Continue the processing with the rest data. A chunk of NUMBER_OF_CHUNK bytes is processed each time */
                for (int i = 1; i < (NUMBER_OF_DATA/NUMBER_OF_BYTE_PER_CHUNK); i++) {

                        /*
                         * Configure DMA to transfer the next chunk of data to the AES/HASH engine for processing.
                         * Please note that destination address must be NULL when configuring the DMA while the engine is waiting for
                         * more input data.
                         */
                        hw_aes_hash_set_input_data_addr((uint32_t)&vector[i*NUMBER_OF_BYTE_PER_CHUNK]);
                        hw_aes_hash_set_input_data_len(NUMBER_OF_BYTE_PER_CHUNK);


                        /* Time for processing the last input block has elapsed. */
                        if (i == ((NUMBER_OF_DATA/NUMBER_OF_BYTE_PER_CHUNK) - 1)) {
                                /* This is the last block of data so signal the AES/HASH engine that this is the last data block to be processed */
                                hw_aes_hash_set_input_data_mode(false);

                                /* Force the AES/HASH engine to output all the encrypted data in the output buffer */

                                hw_aes_set_output_data_mode(HW_AES_OUTPUT_DATA_MODE_ALL);
                        }

                        /* Continue with the next operation. */
                        hw_aes_start_operation(HW_AES_OPERATION_ENCRYPT);

                        /* Wait until operation is finished */
                        status = ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, &aes_hash_status);
                        OS_ASSERT(status == OS_OK);
                }

                /*  If str1 == str2 then it returns zero */
                if ( !memcmp(frag_encr_data, ctr_192b_cipher, NUMBER_OF_DATA) ) {

                        printf("\n\rSuccessful 192-bit AES CTR encryption! AES STATUS: %d\n\r", aes_hash_status);
                } else {
                        printf("\n\rUnsuccessful 192-bit AES CTR encryption! AES STATUS: %d\n\r", aes_hash_status);
                }
                /* ------------------------------------------------ Encryption process ------------------------------------------------------- */

                /* ------------------------------------------------ Decryption process ------------------------------------------------------- */

                /* Mark the AES/HASH engine that this is the last input block to be processed. Here, decryption is performed in non-fragmented data. */
                hw_aes_hash_set_input_data_mode(false);
                hw_aes_set_init_vector(iv);

                /* Configure DMA with new data source/destination */
                hw_aes_hash_set_input_data_addr((uint32_t)frag_encr_data);
                hw_aes_hash_set_output_data_addr((uint32_t)frag_decr_data);
                hw_aes_hash_set_input_data_len(NUMBER_OF_DATA);

                /* Start a AES decryption operation */
                hw_aes_start_operation(HW_AES_OPERATION_DECRYPT);

                /* Wait until operation is finished */
                status = ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, &aes_hash_status);
                OS_ASSERT(status == OS_OK);

                /* Check whether the decryption has been executed successfully */
                if (!memcmp(frag_decr_data, vector, NUMBER_OF_DATA)) {

                        printf("Successful 192-bit AES CTR decryption! AES STATUS: %d\n\r", aes_hash_status);
                } else {
                        printf("Unsuccessful 192-bit AES CTR decryption! AES STATUS: %d\n\r", aes_hash_status);
                }
                /* ---------------------------------------------- Decryption process ---------------------------------------------------- */

                /* Disable event signaling and the AES/HASH engine clock as well */
                ad_crypto_disable_aes_hash_event();

                /* It must ALWAYS be called after the resource has been acquired and as soon as it is no longer needed by the task */
                status = ad_crypto_release_aes_hash();
                OS_ASSERT(status == OS_OK);

                hw_gpio_set_inactive(HW_GPIO_PORT_1, HW_GPIO_PIN_4);
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
static void prvSetupHardware(void)
{

        /* Init hardware */
        pm_system_init(periph_init);

#if mainCHECK_INTERRUPT_STACK == 1
        extern unsigned long _vStackTop[], _pvHeapStart[];
        unsigned long ulInterruptStackSize;

        /* The size of the stack used by main and interrupts is not defined in
         the linker, but just uses whatever RAM is left.  Calculate the amount of
         RAM available for the main/interrupt/system stack, and check it against
         a reasonable number.  If this assert is hit then it is likely you don't
         have enough stack to start the kernel, or to allow interrupts to nest.
         Note - this is separate to the stacks that are used by tasks.  The stacks
         that are used by tasks are automatically checked if
         configCHECK_FOR_STACK_OVERFLOW is not 0 in FreeRTOSConfig.h - but the stack
         used by interrupts is not.  Reducing the conifgTOTAL_HEAP_SIZE setting will
         increase the stack available to main() and interrupts. */
        ulInterruptStackSize = ( ( unsigned long ) _vStackTop ) - ( ( unsigned long ) _pvHeapStart );
        OS_ASSERT( ulInterruptStackSize > 350UL );

        /* Fill the stack used by main() and interrupts to a known value, so its
         use can be manually checked. */
        memcpy( ( void * ) _pvHeapStart, ucExpectedInterruptStackValues, sizeof( ucExpectedInterruptStackValues ) );
#endif
}

/**
 * @brief Malloc fail hook
 */
void vApplicationMallocFailedHook(void)
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
        taskDISABLE_INTERRUPTS()
        ;
        for (;;)
                ;
}

/**
 * @brief Application idle task hook
 */
void vApplicationIdleHook(void)
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
void vApplicationStackOverflowHook( OS_TASK pxTask, char *pcTaskName)
{
        (void)pcTaskName;
        (void)pxTask;

        /* Run time stack overflow checking is performed if
         configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
         function is called if a stack overflow is detected. */
        taskDISABLE_INTERRUPTS()
        ;
        for (;;)
                ;
}

/**
 * @brief Application tick hook
 */
void vApplicationTickHook(void)
{
#if mainCHECK_INTERRUPT_STACK == 1
        extern unsigned long _pvHeapStart[];

        /* This function will be called by each tick interrupt if
         configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
         added here, but the tick hook is called from an interrupt context, so
         code must not attempt to block, and only the interrupt safe FreeRTOS API
         functions can be used (those that end in FromISR()). */

        /* Manually check the last few bytes of the interrupt stack to check they
         have not been overwritten.  Note - the task stacks are automatically
         checked for overflow if configCHECK_FOR_STACK_OVERFLOW is set to 1 or 2
         in FreeRTOSConifg.h, but the interrupt stack is not. */
        OS_ASSERT( memcmp( ( void * ) _pvHeapStart, ucExpectedInterruptStackValues, sizeof( ucExpectedInterruptStackValues ) ) == 0U );
#endif /* mainCHECK_INTERRUPT_STACK */
}

