/**
 * \addtogroup BSP
 * \{
 * \addtogroup INTERFACES
 * \{
 * \addtogroup SECURITY_TOOLBOX
 * \{
 * \addtogroup AES
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file crypto_aes.c
 *
 * @brief AES API
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

#include <crypto_aes.h>
#include <hw_aes_hash.h>
#include <ad_crypto.h>
#include <string.h>

#if !dg_configCRYPTO_ADAPTER
#error dg_configCRYPTO_ADAPTER macro must be set to (1) in order to use HMAC.
#endif

CRYPTO_AES_RET crypto_aes_init_context(crypto_aes_context_t *ctx, CRYPTO_AES_MODE aes_mode, hw_aes_key_size key_size,
                                       uint8_t* aes_key, uint8_t* aes_iv, uint8_t* aes_ic, uint8_t* plaintext_adress,
                                       uint8_t* ciphertext_adress, size_t plaintext_size_in_bytes){


        /* TODO: Add input sanitization check and return proper error codes. */
        ctx->mode = aes_mode;
        ctx->plaintext_size = plaintext_size_in_bytes;
        ctx->bytes_left = plaintext_size_in_bytes;
        ctx->aes_key_adress = aes_key;
        ctx->aes_iv_adress= aes_iv;
        ctx->aes_plaintext_adress= plaintext_adress;
        ctx->aes_ciphertext_adress= ciphertext_adress;
        ctx->key_size = key_size;
        if (aes_iv != NULL) {
                ctx->aes_iv_adress = aes_iv;
        }
        if (aes_ic != NULL) {
                ctx->aes_ic_adress = aes_ic;
        }

        return CRYPTO_AES_RET_OK;

}


CRYPTO_AES_RET crypto_aes_encrypt(crypto_aes_context_t *ctx, size_t packet_size, OS_TICK_TIME timeout){

        if(ctx->bytes_left == ctx->plaintext_size){
                //first time called for this context...
                // grab adapter mutex
                if (ad_crypto_acquire_aes_hash(timeout) != OS_MUTEX_TAKEN) {
                        return CRYPTO_AES_RET_TO;
                }
                ad_crypto_enable_aes_hash_event();

                //configure and start aes_hash module
                switch (ctx->mode){
                case CRYPTO_AES_ECB:
                        hw_aes_hash_cfg_aes_ecb(ctx->key_size);
                        break;
                case CRYPTO_AES_CTR:
                        hw_aes_hash_cfg_aes_ctr(ctx->key_size);
                        break;
                case CRYPTO_AES_CBC:
                        hw_aes_hash_cfg_aes_cbc(ctx->key_size);
                        break;
                default:
                        break;
                }


                //if cbc mode store initialization vector.
                if (ctx->mode == CRYPTO_AES_CBC) {
                        if (ctx->aes_iv_adress == NULL) {
                                //cbc context should be initialized with a proper iv
                                //release adapter and return proper error code.
                        }
                        else {
                                hw_aes_hash_store_iv(ctx->aes_iv_adress);
                        }
                }

                //if ctr mode store initialization counter.
                if (ctx->mode == CRYPTO_AES_CTR) {
                        if (ctx->aes_ic_adress == NULL) {
                                //cbc context should be initialized with a proper iv
                                //release adapter and return proper error code.
                        }
                        else {
                                hw_aes_hash_store_ic(ctx->aes_ic_adress);
                        }
                }
                //store aes key
                hw_aes_hash_keys_load(ctx->key_size, ctx->aes_key_adress, HW_AES_PERFORM_KEY_EXPANSION);
        }

        if(packet_size < ctx->bytes_left){
                //this is not the last packet.
                hw_aes_hash_mark_input_block_as_not_last();
        }
        else{
                //this is the last packet.
                hw_aes_hash_mark_input_block_as_last();
        }


        //encrypt this packet...
        hw_aes_hash_cfg_dma(ctx->aes_plaintext_adress, ctx->aes_ciphertext_adress, packet_size);
        hw_aes_hash_encrypt();
        ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, NULL);

        //update bytes left
        ctx->bytes_left = ctx->bytes_left - packet_size;
        if (ctx->bytes_left<=0){
                //encryption complete for the whole packet
                //release adapter
                /* Disable engine clock and adapter even handling */
                //hw_aes_hash_output_mode_write_all();
                ad_crypto_disable_aes_hash_event();
                if (ad_crypto_release_aes_hash() != OS_OK) {
                        /* This means that the resource was acquired by a different task or under ISR context.
                           The code should not reach here normally. */
                        OS_ASSERT(0);
                }
                //reset bytes left in context
                ctx->bytes_left = ctx->plaintext_size;
        }
        else{
                //update source and dest address???
                ctx->aes_ciphertext_adress = ctx->aes_ciphertext_adress + packet_size;
                ctx->aes_plaintext_adress = ctx->aes_plaintext_adress + packet_size;
        }

        return CRYPTO_AES_RET_OK;
}

CRYPTO_AES_RET crypto_aes_decrypt(crypto_aes_context_t *ctx, size_t packet_size, OS_TICK_TIME timeout){


        if(ctx->bytes_left == ctx->plaintext_size){
                //first time called...
                // grab adapter mutex
                if (ad_crypto_acquire_aes_hash(timeout) != OS_MUTEX_TAKEN) {
                        return CRYPTO_AES_RET_TO;
                }
                ad_crypto_enable_aes_hash_event();

                //configure and start aes_hash module
                switch (ctx->mode){
                case CRYPTO_AES_ECB:
                        hw_aes_hash_cfg_aes_ecb(ctx->key_size);
                        break;
                case CRYPTO_AES_CTR:
                        hw_aes_hash_cfg_aes_ctr(ctx->key_size);
                        break;
                case CRYPTO_AES_CBC:
                        hw_aes_hash_cfg_aes_cbc(ctx->key_size);
                        break;
                default:
                        break;
                }

                //if cbc mode store initialization vector.
                if (ctx->mode == CRYPTO_AES_CBC) {
                        if (ctx->aes_iv_adress == NULL) {
                                //cbc context should be initialized with a proper iv
                                //release adapter and return proper error code.
                        }
                        else {
                                hw_aes_hash_store_iv(ctx->aes_iv_adress);
                        }
                }
                //if ctr mode store initialization counter.
                if (ctx->mode == CRYPTO_AES_CTR) {
                        if (ctx->aes_ic_adress == NULL) {
                                //cbc context should be initialized with a proper iv
                                //release adapter and return proper error code.
                        }
                        else {
                                hw_aes_hash_store_ic(ctx->aes_ic_adress);
                        }
                }
                //store aes key
                hw_aes_hash_keys_load(ctx->key_size, ctx->aes_key_adress, HW_AES_PERFORM_KEY_EXPANSION);
        }

        if(packet_size < ctx->bytes_left){
                //this is not the last packet.
                hw_aes_hash_mark_input_block_as_not_last();
        }
        else{
                //this is the last packet.
                hw_aes_hash_mark_input_block_as_last();
        }

        //encrypt this packet...
        hw_aes_hash_cfg_dma(ctx->aes_ciphertext_adress, ctx->aes_plaintext_adress, packet_size);
        hw_aes_hash_decrypt();
        ad_crypto_wait_aes_hash_event(OS_EVENT_FOREVER, NULL);

        //update bytes left
        ctx->bytes_left = ctx->bytes_left - packet_size;
        if (ctx->bytes_left<=0){
                //encryption complete for the whole packet
                //release adapter
                /* Disable engine clock and adapter even handling */
                //hw_aes_hash_output_mode_write_all();
                ad_crypto_disable_aes_hash_event();
                if (ad_crypto_release_aes_hash() != OS_OK) {
                        /* This means that the resource was acquired by a different task or under ISR context.
                           The code should not reach here normally. */
                        OS_ASSERT(0);
                }
                //reset bytes left in context
                ctx->bytes_left = ctx->plaintext_size;
        }
        else{
                //update source and dest address???
                ctx->aes_ciphertext_adress = ctx->aes_ciphertext_adress + packet_size;
                ctx->aes_plaintext_adress = ctx->aes_plaintext_adress + packet_size;
        }

        return CRYPTO_AES_RET_OK;
}


#endif /* dg_configUSE_HW_AES_HASH */

/**
 * \}
 * \}
 * \}
 * \}
 */
