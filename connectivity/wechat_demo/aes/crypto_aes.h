/**
 * \addtogroup BSP
 * \{
 * \addtogroup INTERFACES
 * \{
 * \addtogroup SECURITY_TOOLBOX
 * \{
 * \addtogroup AES
 *
 * \brief AES algorithms implementation.
 *
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file crypto_aes.h
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

#ifndef SDK_CRYPTO_INCLUDE_CRYPTO_AES_H_
#define SDK_CRYPTO_INCLUDE_CRYPTO_AES_H_

#include <osal.h>
#include <stdint.h>
#include <stddef.h>

#include "hw_aes_hash.h"

/**
 * \brief AES modes. Possible values: CRYPTO_AES_ECB, CRYPTO_AES_CBC, CRYPTO_AES_CTR, CRYPTO_AES_LAST_VALUE
 */
typedef enum {
        CRYPTO_AES_ECB,
        CRYPTO_AES_CBC,
        CRYPTO_AES_CTR,
        CRYPTO_AES_LAST_VALUE
} CRYPTO_AES_MODE;

/**
 * \brief AES API return codes.
 */
typedef enum {
        CRYPTO_AES_RET_OK = 0,       /**< No error. */
        CRYPTO_AES_RET_TO = 1,       /**< Operation timed out. */
        CRYPTO_AES_RET_EE = 2,       /**< AES operation error. */
        CRYPTO_AES_RET_ER            /**< Other error. */
} CRYPTO_AES_RET;

/**
 * \brief AES context.
 *
 * The context data are stored.
 */
typedef struct {
        CRYPTO_AES_MODE mode;           /**< AES mode. */
        size_t plaintext_size;          /**< Total size in bytes of plaintext. */
        size_t bytes_left;              /**< Size in bytes left for aes operation. */
        uint8_t* aes_key_adress;        /**< AES private key. */
        uint8_t* aes_iv_adress;         /**< AES initialization vector (CBC mode only). */
        uint8_t* aes_ic_adress;         /**< AES initialization counter (CTR mode only). */
        uint8_t* aes_plaintext_adress;  /**< AES operation plaintext address. */
        uint8_t* aes_ciphertext_adress; /**< AES operation ciphertext address. */
        hw_aes_key_size key_size;        /**< AES key size to use. */
} crypto_aes_context_t;

/**
 * \brief Encrypt message with AES.
 *
 * This function is used in order to encrypt a specified palintext using AES
 * algorithm. Mode (ECB, CBC, CTR) and key length (128, 192, 256) are specified
 * during operation context initialization. Depending on selected mode, an
 * initialization vector(CBC mode) or an initialization counter(CTR mode)
 * should be provided. If a certain operation needs no initialization vector or
 * initialization counter, then the function should be called with NULL arguments.
 * Since the function needs to acquire a hardware resource for aes/hash hw module,
 * it also takes as input a timeout value used when waiting to acquire the resource.
 * Example usage:
 *
 * \code{c}
 *
 * uint8_t plaintext[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do";
 * uint8_t ciphertext[64];
 *
 * const uint8 key_128b[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7,
 *                              0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
 *
 * const uint8_t init_counter[16] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
 *                                    0xfc, 0xfd, 0xfe, 0xff };
 *
 * crypto_aes_context_t aes_ctx;
 * CRYPTO_AES_RET aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_CTR, HW_AES_128, key_128b, NULL, init_counter, plaintext, encrypted_data, 16);
 *
 * aes_status =  crypto_aes_encrypt(&aes_ctx, 16, 500);
 * \endcode
 *
 * It is possible to call this function even if the complete message is partially available. packet size
 * is then the length of plaintext that is currently available, and input plaintext chunk should either be
 * copied to the original context plaintext address, or the pointer of the context plaintext address should
 * be overwritten.
 * \code{c}
 * uint8_t plaintext[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do"
 * const uint8 key_128b[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7,
 *                              0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
 * uint8_t ciphertext[64];
 *
 * crypto_aes_context_t aes_ctx;
 * CRYPTO_AES_RET aes_status = crypto_aes_init_context(&aes_ctx, CRYPTO_AES_ECB, HW_AES_128, key_128b, NULL, NULL, plaintext, encrypted_data, 16);
 *
 * aes_status =  crypto_aes_encrypt(&aes_ctx, 8, 500);
 * aes_ctx.plaintext_adress+= 8;
 * aes_status =  crypto_aes_encrypt(&aes_ctx, 8, 500);
 * \endcode
 *
 * AES decryption is performed in the same way, by calling crypto_aes_decrypt for a specified aes context.
 *
 * \param [in] ctx         The aes context where the result will be stored.
 * \param [in] packet_size Length in bytes of data to operate in thhis step.
 * \param [in] timeout     Timeout in msec of aes operations.
 *
 * \returns The function returns one of the return codes defined in ::CRYPTO_AES_RET.
 */

CRYPTO_AES_RET crypto_aes_encrypt(crypto_aes_context_t *ctx, size_t packet_size, OS_TICK_TIME timeout);
/**
 * \brief Initialize context for use with crypto_aes_encrypt/crypto_aes_decrypt.
 *
 * \param [in,out] ctx                     Pointer to the aes context used for aes operations.
 * \param [in]     aes_mode                AES mode to be used with the created AES context { ECB, CBC, CTR}.
 * \param [in]     key_size                Key size of the aes mode { 128, 192, 256}.
 * \param [in]     aes_key                 AES operations key.
 * \param [in]     aes_iv                  AES initialization vector (CBC mode only).
 * \param [in]     aes_ic                  AES initialization counter (CTR mode only).
 * \param [in]     plaintext_adress        AES operation plaintext address.
 * \param [in]     ciphertext_adress       AES operation ciphertext address.
 * \param [in]     plaintext_size_in_bytes Total size in bytes of aes plaintext.
 *
 * \returns The function returns one of the return codes defined in ::CRYPTO_AES_RET.
 */
CRYPTO_AES_RET crypto_aes_init_context(crypto_aes_context_t *ctx, CRYPTO_AES_MODE aes_mode, hw_aes_key_size key_size,
                                       uint8_t* aes_key, uint8_t* aes_iv, uint8_t* aes_ic, uint8_t* plaintext_adress,
                                       uint8_t* ciphertext_adress, size_t plaintext_size_in_bytes);

/**
 * \brief Decrypt message.
 *
 * \param [in] ctx         The aes context where the result will be stored.
 * \param [in] packet_size Length in bytes of data to operate in thhis step.
 * \param [in] timeout     Timeout in msec of aes operations.
 *
 * \returns The function returns one of the return codes defined in ::CRYPTO_AES_RET.
 */
CRYPTO_AES_RET crypto_aes_decrypt(crypto_aes_context_t *ctx, size_t packet_size, OS_TICK_TIME timeout);

#endif /* SDK_CRYPTO_INCLUDE_CRYPTO_AES_H_ */
