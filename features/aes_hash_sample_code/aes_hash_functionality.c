/**
 ****************************************************************************************
 *
 * @file aes_hash_functionality.c
 *
 * @brief APIs for performing AES/HASH operations
 *
 * Copyright (c) 2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#include "aes_hash_functionality.h"


/* Configure the crypto engine with the selected hash algorithm */
static AES_HASH_STATUS _select_hash_algorithm(hw_aes_hash_mode hash_mode, unsigned int result_size)
{
        AES_HASH_STATUS err = AES_HASH_STATUS_OK;

        switch(hash_mode) {
        case HW_HASH_MD5:
                hw_aes_hash_cfg_md5(result_size);
                break;
        case HW_HASH_SHA_1:
                hw_aes_hash_cfg_sha1(result_size);
                break;
        case HW_HASH_SHA_256_224:
                hw_aes_hash_cfg_sha_224(result_size);
                break;
        case HW_HASH_SHA_256:
                hw_aes_hash_cfg_sha_256(result_size);
                break;
        case HW_HASH_SHA_384:
                hw_aes_hash_cfg_sha_384(result_size);
                break;
        case HW_HASH_SHA_512:
                hw_aes_hash_cfg_sha_512(result_size);
                break;
        case HW_HASH_SHA_512_224:
                hw_aes_hash_cfg_sha_512_224(result_size);
                break;
        case HW_HASH_SHA_512_256:
                hw_aes_hash_cfg_sha_512_256(result_size);
                break;
        default:
                /* The selected mode is not supported */
               err = AES_HASH_STATUS_INVALID_CRYPTO_MODE;
        }

        return err;

}

/* Configure the crypto engine with the selected AES algorithm */
static AES_HASH_STATUS _select_aes_algorithm(hw_aes_hash_mode aes_mode, hw_aes_key_size key_size, const uint8 *iv, const uint8 *ic)
{
        AES_HASH_STATUS err = AES_HASH_STATUS_OK;

        switch(aes_mode) {
        case HW_AES_ECB:
                hw_aes_hash_cfg_aes_ecb(key_size);
                break;
        case HW_AES_CBC:
                hw_aes_hash_cfg_aes_cbc(key_size);

                if (iv) {
                        hw_aes_hash_store_iv(iv) ;
                } else {
                        err = AES_HASH_STATUS_MISSING_REQUIRED_KEYS;
                }
                break;
        case HW_AES_CTR:
                hw_aes_hash_cfg_aes_ctr(key_size);

                if (ic) {
                     hw_aes_hash_store_ic(ic);
                } else {
                      err = AES_HASH_STATUS_MISSING_REQUIRED_KEYS;
                }
                break;
        default:
                /* The selected mode is not supported */
                err = AES_HASH_STATUS_INVALID_CRYPTO_MODE;
        }

        return err;
}


/* Perform AES encryption/decryption operations using fragmented input blocks */
AES_HASH_STATUS _crypto_aes_fragmented_input(hw_aes_hash_mode aes_mode, hw_aes_direction direction,
                        const uint8 *aes_key, hw_aes_key_size key_size,
                        hw_aes_hash_key_exp_t key_exp_flag,
                        const uint8 *iv, const uint8 *ic,
                        const aes_hash_frag_t src[], uint8 *dst, unsigned int src_len,
                        OS_TICK_TIME timeout)
{
        OS_BASE_TYPE status;
        AES_HASH_STATUS err = AES_HASH_STATUS_OK;

        /* Acquire resources for AES/HASH engine */
        status = ad_crypto_acquire_aes_hash(timeout);


        /*
         * If the crypto engine was not acquired just return with the appropriate
         * error code.
         */
        if (status != OS_MUTEX_TAKEN) return AES_HASH_STATUS_TIMEOUT_EXPIRATION;


        /*
         * Enable AES/HASH event signaling as well as the AES/HASH engine clock.
         * This function enables the crypto IRQ which is shared with AES/HASH
         * as well as ECC block.
         */
        ad_crypto_enable_aes_hash_event();


        /* Configure crypto engine with the selected AES mode  */
        err = _select_aes_algorithm(aes_mode, key_size, iv, ic);

        if (err != AES_HASH_STATUS_OK) goto _exit;


        /*
         * Store AES keys in crypto engine memory.
         *
         * \note After an AES operation with a certain key, further operations using
         *       the same key, do not require storing it to the crypto memory again.
         *
         * \note Key expansion bit is automatically cleared by crypto engine, after the completion
         *       of a key expansion process.
         */
        //hw_aes_hash_store_keys(key_size, aes_key, key_exp_flag);
        hw_aes_hash_keys_load(key_size, aes_key, key_exp_flag);

        /*
         * This function configures the dedicated DMA engine with the source
         * and destination buffers.
         *
         * \note \p dst must be NULL when configuring the DMA while the engine
         *       is waiting for more input data
         *
         **/
        hw_aes_hash_cfg_dma(src[0].src, dst, src[0].len);


       /*
        * Signal AES/HASH engine that this is not the last input block
        * to be processed.
        */
        hw_aes_hash_mark_input_block_as_not_last();

        if (direction == HW_AES_ENCRYPT ) {
                /* Start AES encryption operations */
                hw_aes_hash_encrypt();
        } else if (direction == HW_AES_DECRYPT) {
                /* Start AES decryption operations */
                hw_aes_hash_decrypt();
        }


        /* Block until AES operation is finished. */
        status = ad_crypto_wait_aes_hash_event(timeout, NULL);

        if (status != OS_EVENT_SIGNALED) {
                err = AES_HASH_STATUS_TIMEOUT_EXPIRATION;
                goto _exit;
        }

        /* Scan all the fragmented input vectors */
        for (int i = 1; i < src_len; i++) {
                /* Check if this vector is the last to be processed */
                if (i == (src_len - 1))  {
                        /*
                         * Configure the crypto engine so that to consider the next
                         * input vector as the last one.
                         */
                        hw_aes_hash_restart((const uint32)src[i].src, (const uint32)src[i].len, false);

                        /*
                         * Force the AES/HASH engine to output all the encrypted data
                         * in the output buffer.
                         */
                        hw_aes_hash_output_mode_write_all();
                } else {
                        hw_aes_hash_restart((const uint32)src[i].src, (const uint32)src[i].len, true);
                }

                /* Block until AES operation is finished */
                status = ad_crypto_wait_aes_hash_event(timeout, NULL);

                if (status != OS_EVENT_SIGNALED) {
                        err = AES_HASH_STATUS_TIMEOUT_EXPIRATION;
                        goto _exit;
                }
        }

        goto _exit; // This point will be reached in case there are no error


_exit:
        /* Disable AES/HASH event signaling as well as the AES/HASH engine clock */
        ad_crypto_disable_aes_hash_event();


        /*
         *  When crypto in no longer needed, it must be released so that
         *  other tasks can use it.
         */
        ad_crypto_release_aes_hash();

        return err;
}


/* Perform AES encryption/decryption operations using non-fragmented input blocks. */
AES_HASH_STATUS _crypto_aes_non_fragmented_input(hw_aes_hash_mode aes_mode, hw_aes_direction direction,
                          const uint8 *aes_key, hw_aes_key_size key_size,
                          hw_aes_hash_key_exp_t key_exp_flag,
                          const uint8 *iv, const uint8 *ic,
                          const uint8 *src, uint8 *dst, unsigned int src_len,
                          OS_TICK_TIME timeout)
{
        OS_BASE_TYPE status;
        AES_HASH_STATUS err = AES_HASH_STATUS_OK;


        /* Acquire resources for AES/HASH engine */
        status = ad_crypto_acquire_aes_hash(timeout);

        /*
         * If the crypto engine was not acquired just return with the appropriate
         * error code.
         */
        if (status != OS_MUTEX_TAKEN) return AES_HASH_STATUS_TIMEOUT_EXPIRATION;


        /*
         * Enable AES/HASH event signaling as well as the AES/HASH engine clock.
         * This function enables the crypto IRQ which is shared with AES/HASH
         * as well as ECC block.
         */
        ad_crypto_enable_aes_hash_event();


        /* Configure crypto engine with the desired AES algorithm  */
        err = _select_aes_algorithm(aes_mode, key_size, iv, ic);

        if (err != AES_HASH_STATUS_OK) goto _exit;


        /*
         * Store AES keys in crypto engine memory.
         *
         * \note After an AES operation with a certain key, further operations using
         *       the same key do not require storing it to the crypto memory again.
         *
         * \note Key expansion bit is automatically cleared by crypto engine, after the completion
         *       of a key expansion process.
         */
        //hw_aes_hash_store_keys(key_size, aes_key, key_exp_flag);
        hw_aes_hash_keys_load(key_size, aes_key, key_exp_flag);

        /*
         * This function configures the dedicated DMA engine with the source
         * and destination buffers.
         *
         * \note \p dst must be NULL when configuring the DMA while the engine
         *       is waiting for more input data
         *
         **/
        hw_aes_hash_cfg_dma(src, dst, src_len);


        /*
         * Signal the AES/HASH engine that this is the last input block
         * to be processed.
         */
        hw_aes_hash_mark_input_block_as_last();

        if (direction == HW_AES_ENCRYPT ) {
                /* Start AES encryption operations */
                hw_aes_hash_encrypt();
        } else if(direction == HW_AES_DECRYPT) {
                /* Start AES decryption operations */
                hw_aes_hash_decrypt();
        }

        /* Block until AES operation is finished. */
        status = ad_crypto_wait_aes_hash_event(timeout, NULL);

        if (status != OS_EVENT_SIGNALED) {
                err = AES_HASH_STATUS_TIMEOUT_EXPIRATION;
                goto _exit;
        }

        goto _exit; // This point will be reached in case there are no error


_exit:
        /* Disable AES/HASH event signaling as well as the AES/HASH engine clock */
        ad_crypto_disable_aes_hash_event();

        /*
         *  When crypto in no longer needed, it must be released so that
         *  other tasks can use it.
         */
         ad_crypto_release_aes_hash();

         return err;
}


/* Perform hash operations using non-fragmented input blocks */
AES_HASH_STATUS _crypto_hash_non_fragmented_input(hw_aes_hash_mode hash_mode, unsigned int result_size,
                                                   const uint8 *src, uint8 *dst,
                                                   unsigned int src_len, OS_TICK_TIME timeout)
{
        OS_BASE_TYPE status;
        AES_HASH_STATUS err = AES_HASH_STATUS_OK;


        /* Acquire resources for AES/HASH engine */
        status = ad_crypto_acquire_aes_hash(timeout);

        /*
         * If the crypto engine was not acquired just return with the appropriate
         * error code.
         */
        if (status != OS_MUTEX_TAKEN)  return AES_HASH_STATUS_TIMEOUT_EXPIRATION;


        /*
         * Enable AES/HASH event signaling as well as the AES/HASH engine clock.
         * This function enables the crypto IRQ which is shared with AES/HASH
         * as well as ECC block.
         */
        ad_crypto_enable_aes_hash_event();


        /* Select the desired hash algorithm */
        err = _select_hash_algorithm(hash_mode, result_size);

        if (err != AES_HASH_STATUS_OK) goto _exit;


        /*
         * This function configures the dedicated DMA engine with the source
         * and destination buffers.
         *
         * \note \p dst must be NULL when configuring the DMA while the engine
         *       is waiting for more input data
         *
         **/
        hw_aes_hash_cfg_dma(src, dst, src_len);


        /*
         * Signal the AES/HASH engine that this is the last input block
         * to be processed.
         */
        hw_aes_hash_mark_input_block_as_last();


        /* Start AES/HASH operation */
        hw_aes_hash_start();


        /* Block until hashing operation is finished. */
        status = ad_crypto_wait_aes_hash_event(timeout, NULL);

        if (status != OS_EVENT_SIGNALED) {
                err = AES_HASH_STATUS_TIMEOUT_EXPIRATION;
                goto _exit;
        }

        goto _exit; // This point will be reached in case there are no error


_exit:
        /* Disable AES/HASH event signaling as well as the AES/HASH engine clock */
        ad_crypto_disable_aes_hash_event();

        /*
         * When crypto in no longer needed, it must be released so that
         * other tasks can use it.
         */
        ad_crypto_release_aes_hash();

        return err;
}


/* Perform HASH operations using fragmented input blocks */
AES_HASH_STATUS _crypto_hash_fragmented_input(hw_aes_hash_mode hash_mode, unsigned int result_size,
                                               const aes_hash_frag_t src[], uint8 *dst,
                                               unsigned int src_len, OS_TICK_TIME timeout)
{
        OS_BASE_TYPE status;
        AES_HASH_STATUS err = AES_HASH_STATUS_OK;


        /* Acquire resources for AES/HASH engine */
        status = ad_crypto_acquire_aes_hash(timeout);

        /*
         * If the crypto engine was not acquired just return with the appropriate
         * error code.
         */
        if (status != OS_MUTEX_TAKEN) return AES_HASH_STATUS_TIMEOUT_EXPIRATION;


        /*
         * Enable AES/HASH event signaling as well as the AES/HASH engine clock.
         * This function enables crypto IRQ shared with AES/HASH and ECC.
         */
        ad_crypto_enable_aes_hash_event();


        /* Select the desired hash algorithm */
        err = _select_hash_algorithm(hash_mode, result_size);

        if (err != AES_HASH_STATUS_OK) goto _exit;


        /*
         * This function configures the DMA machine with the source and
         * destination buffers.
         */
        hw_aes_hash_cfg_dma(src[0].src, dst, src[0].len);


        /*
         * Signal AES/HASH engine that this is not the last input block
         * to be processed.
         */
         hw_aes_hash_mark_input_block_as_not_last();


        /* Start AES/HASH operation */
        hw_aes_hash_start();


        /* Block until AES operation is finished. */
        status = ad_crypto_wait_aes_hash_event(timeout, NULL);

        if (status != OS_EVENT_SIGNALED) {
                err = AES_HASH_STATUS_TIMEOUT_EXPIRATION;
                goto _exit;
        }

        /* Scan all the fragmented input vectors */
        for (int i = 1; i < src_len; i++) {
                /* Check if this is the last input block to be processed */
                if (i == (src_len - 1))  {
                        /*
                         * Configure the crypto engine so that to consider the next
                         * input vector as the last one.
                         */
                        hw_aes_hash_restart((const uint32)src[i].src, (const uint32)src[i].len, false);

                        /*
                         * Force the AES/HASH engine to output all the encrypted data
                         * in the output buffer.
                         */
                        hw_aes_hash_output_mode_write_all();
                } else {
                        hw_aes_hash_restart((const uint32)src[i].src, (const uint32)src[i].len, true);
                }

                status = ad_crypto_wait_aes_hash_event(timeout, NULL);

                if (status != OS_EVENT_SIGNALED) {
                        err = AES_HASH_STATUS_TIMEOUT_EXPIRATION;
                        goto _exit;
                }
        }

        goto _exit; // This point will be reached in case there are no error


_exit:
        /* Disable AES/HASH event signaling as well as the AES/HASH engine clock */
        ad_crypto_disable_aes_hash_event();


        /*
         * When crypto in no longer needed, it must be released so that
         * other tasks can use it.
         */
        ad_crypto_release_aes_hash();

        return err;
}


