/**
 * \addtogroup BSP
 * \{
 * \addtogroup INTERFACES
 * \{
 * \addtogroup SECURITY_TOOLBOX
 * \{
 * \addtogroup HASH
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file crypto_hash.c
 *
 * @brief HASH API
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
 *
 ****************************************************************************************
 */
#if dg_configUSE_HW_AES_HASH

#include <crypto_hash_op.h>
#include <hw_aes_hash.h>
#include <ad_crypto.h>
#include <string.h>

#if !dg_configCRYPTO_ADAPTER
#error dg_configCRYPTO_ADAPTER macro must be set to (1) in order to use HMAC.
#endif


CRYPTO_HASH_RET crypto_hash_init_context(crypto_hash_context_t *ctx, CRYPTO_HASH_MODE hash_mode, uint8_t* plaintext_adress,
        uint8_t* ciphertext_adress, size_t plaintext_size_in_bytes){

        /* TODO: Add input sanitization check and return proper error codes. */
        ctx->mode = hash_mode;
        ctx->plaintext_size = plaintext_size_in_bytes;
        ctx->bytes_left = plaintext_size_in_bytes;
        ctx->hash_plaintext_adress= plaintext_adress;
        ctx->hash_ciphertext_adress= ciphertext_adress;

        return CRYPTO_HASH_RET_OK;
}

CRYPTO_HASH_RET crypto_hash_compute(crypto_hash_context_t *ctx, size_t packet_size, OS_TICK_TIME timeout){

                // grab adapter mutex
                if (ad_crypto_acquire_aes_hash(timeout) != OS_MUTEX_TAKEN) {
                        return CRYPTO_HASH_RET_TO;
                }
                ad_crypto_enable_aes_hash_event();

                //configure and start aes_hash module
                switch (ctx->mode) {
                case CRYPTO_HASH_MD5:
                        hw_aes_hash_cfg_md5(packet_size);
                        break;
                case CRYPTO_HASH_SHA_1:
                        hw_aes_hash_cfg_sha1(packet_size);
                        break;
                case CRYPTO_HASH_SHA_224:
                        hw_aes_hash_cfg_sha_224(packet_size);
                        break;
                case CRYPTO_HASH_SHA_256_224:
                        hw_aes_hash_cfg_sha_224(packet_size);
                        break;
                case CRYPTO_HASH_SHA_256:
                        hw_aes_hash_cfg_sha_256(packet_size);
                        break;
                case CRYPTO_HASH_SHA_384:
                        hw_aes_hash_cfg_sha_384(packet_size);
                        break;
                case CRYPTO_HASH_SHA_512:
                        hw_aes_hash_cfg_sha_512(packet_size);
                        break;
                case CRYPTO_HASH_SHA_512_224:
                        hw_aes_hash_cfg_sha_512_224(packet_size);
                        break;
                case CRYPTO_HASH_SHA_512_256:
                        hw_aes_hash_cfg_sha_512_256(packet_size);
                        break;
//                default:
                }

        hw_aes_hash_mark_input_block_as_last();



        //hash input
        hw_aes_hash_cfg_dma(ctx->hash_plaintext_adress, ctx->hash_ciphertext_adress, ctx->plaintext_size);
        hw_aes_hash_start();
        ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, NULL);

        //hashig complete for the whole plaintext
        //release adapter
        /* Disable engine clock and adapter even handling */
        //hw_aes_hash_output_mode_write_all();
        ad_crypto_disable_aes_hash_event();
        if (ad_crypto_release_aes_hash() != OS_OK) {
                /* This means that the resource was acquired by a different task or under ISR context.
                 The code should not reach here normally. */
                OS_ASSERT(0);
        }

        return CRYPTO_HASH_RET_OK;
}

#endif /* dg_configUSE_HW_AES_HASH */

/**
 * \}
 * \}
 * \}
 * \}
 */
