/**
 * \addtogroup BSP
 * \{
 * \addtogroup INTERFACES
 * \{
 * \addtogroup SECURITY_TOOLBOX
 * \{
 * \addtogroup HASH
 *
 * \brief Hash algorithms implementation.
 *
 * \{
 */

/**
 ****************************************************************************************
 *
 * @file crypto_hash_op.h
 *
 * @brief Hash API
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

#ifndef SDK_CRYPTO_INCLUDE_CRYPTO_HASH_OP_H_
#define SDK_CRYPTO_INCLUDE_CRYPTO_HASH_H_

#include <osal.h>
#include <stdint.h>
#include <stddef.h>

/**
 * \brief Hash modes. Possible values: CRYPTO_HASH_MD5,
 * CRYPTO_AES_SHA_1, CRYPTO_AES_SHA_256_224, CRYPTO_AES_SHA_256, CRYPTO_AES_SHA_384, CRYPTO_AES_SHA_512,
 * CRYPTO_AES_SHA_512_224, CRYPTO_AES_SHA_512_256.
 */
typedef enum {
        CRYPTO_HASH_MD5,
        CRYPTO_HASH_SHA_1,
        CRYPTO_HASH_SHA_224,
        CRYPTO_HASH_SHA_256_224,
        CRYPTO_HASH_SHA_256,
        CRYPTO_HASH_SHA_384,
        CRYPTO_HASH_SHA_512,
        CRYPTO_HASH_SHA_512_224,
        CRYPTO_HASH_SHA_512_256
} CRYPTO_HASH_MODE;

/**
 * \brief HASH API return codes.
 */
typedef enum {
        CRYPTO_HASH_RET_OK = 0,       /**< No error. */
        CRYPTO_HASH_RET_TO = 1,       /**< Operation timed out. */
        CRYPTO_HASH_RET_EE = 2,       /**< HASH operation error. */
        CRYPTO_HASH_RET_ER            /**< Other error. */
} CRYPTO_HASH_RET;

/**
 * \brief HASH context.
 *
 * The context data are stored.
 */
typedef struct {
        CRYPTO_HASH_MODE mode;           /**< HASH mode. */
        size_t plaintext_size;           /**< Total size in bytes of plaintext. */
        size_t bytes_left;               /**< Size in bytes left for aes operation. */
        uint8_t* hash_plaintext_adress;  /**< Hash operation plaintext adress. */
        uint8_t* hash_ciphertext_adress; /**< Hash operation ciphertext address. */
} crypto_hash_context_t;

/**
 * \brief Compute hash message.
 *
 * This function is used to calculate hash of a plaintext, using the algorithm selected
 * during the hash context initialization that is used for this operation. Packet size is the
 * Since the function needs to acquire a hardware resource for aes/hash hw module,
 * it also takes as input a timeout value used when waiting to acquire the resource.
 * Example usage:
 *
 * \code{c}
 * const uint8_t plaintext[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
 * uint8_t hash_output[64];
 *
 * crypto_hash_context_t hash_ctx;
 * hash_status = crypto_hash_init_context(&hash_ctx, CRYPTO_HASH_SHA_256, plaintext, hash_output, sizeof(plaintext) - 1);
 * hash_status = crypto_hash_compute(&hash_ctx, 32,100);
 * \endcode
 *
 * \param [in] ctx         The hash context where the result will be stored.
 * \param [in] packet_size Length in bytes of data to operate in this step.
 * \param [in] timeout     Timeout in msec of aes operations.
 *
 * \returns The function returns one of the return codes defined in ::CRYPTO_HASH_RET.
 */
CRYPTO_HASH_RET crypto_hash_compute(crypto_hash_context_t *ctx, size_t packet_size, OS_TICK_TIME timeout);

/**
 * \brief Initialize context for use with crypto_hash_compute.
 *
 * \param [in,out] ctx                     The hash context where the result will be stored.
 * \param [in]     hash_mode               Hash mode to be used with the created hash context.
 * \param [in]     plaintext_adress        Hash operation plaintext adress.
 * \param [in]     ciphertext_adress       Hash operation ciphertext adress.
 * \param [in]     plaintext_size_in_bytes Total size in bytes of hash plaintext.
 *
 * \returns The function returns one of the return codes defined in ::CRYPTO_HASH_RET.
 */
CRYPTO_HASH_RET crypto_hash_init_context(crypto_hash_context_t *ctx, CRYPTO_HASH_MODE hash_mode, uint8_t* plaintext_adress,
        uint8_t* ciphertext_adress, size_t plaintext_size_in_bytes);


#endif /* SDK_CRYPTO_INCLUDE_CRYPTO_HASH_OP_H_ */
