/**
 ****************************************************************************************
 *
 * @file aes_hash_functionality.h
 *
 * @brief Declaration of APIs used for performing AES/HASH operations
 *
 * Copyright (c) 2019-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#ifndef AES_HASH_FUNCTIONALITY_H_
#define AES_HASH_FUNCTIONALITY_H_

#include "osal.h"
#include "ad_crypto.h"
#include "hw_aes_hash.h"


/*
 * @brief Structure used for holding the fragmented Input Vector/Data
 */
typedef struct {
       const uint8 *src; // Input Vector
       unsigned int len; // The length of the Input Vector expressed in bytes
} aes_hash_frag_t;


/*
 * @brief Enum used for declaring the status of an AES/HASH operation.
 */
typedef enum {
        /* AES/Hash operation completed successfully  */
        AES_HASH_STATUS_OK = 0,
        /* When the crypto engine is configured incorrectly */
        AES_HASH_STATUS_INVALID_CRYPTO_MODE,
        /*
         * Keys required for the selected crypto mode are missing (e.g.
         * when an AES-CBC is selected and the IV key is not provided).
         */
        AES_HASH_STATUS_MISSING_REQUIRED_KEYS,
        /*
         * The total time required either to access the crypto engine or perform
         * an AES/Hash operation exceeded the declared \p timeout.
         */
        AES_HASH_STATUS_TIMEOUT_EXPIRATION,
} AES_HASH_STATUS;



/*
 * @brief Perform AES encryption/decryption operations using fragmented input blocks
 *
 * param [in] aes_mode       Select the desired AES mode. Valid values are HW_AES_ECB, HW_AES_CBC and HW_AES_CTR.
 *
 * param [in] direction      Select whether an encryption or decryption operation should be executed.
 *
 * param [in] aes_key        The physical address of the buffer containing the crypto keys. If key expansion is performed by the crypto engine,
 *                           developer should provide only the base crypto key. Otherwise, the buffer should contain all the expanded keys.
 *
 * param [in] key_size       The size of the crypto keys used during encryption/decryption operations.
 *
 * param [in] key_exp_flag   Select whether key expansion is performed by the crypto engine or application. If key expansion is performed by the engine,
 *                           then \p aes_key should contain only the base key.
 *
 * param [in] iv             The physical address of the buffer containing the Initialization Vector. This parameter
 *                           has a meaning only when the selected AES mode is the AES-CBC. If not used, this value
 *                           should be set to NULL.
 *
 * param [in] ic             The physical address of the buffer containing the Initialization Counter. This parameters
 *                           has a meaning only when the selected AES mode is the AES-CTR. If not used, this value
 *                           should be set to NULL.
 *
 * param [in] src            An array of type aes_hash_frag_t that holds all the info related to the fragmented data.
 *
 * param [in] dst            The physical address of the buffer where the output of the AES operation will be stored.
 *
 * param [in] src_len        The number of elements the array \p src consists of.
 *
 * param [in] timeout        The maximum number of ticks to wait either to acquire the crypto engine or to perform
 *                           an AES encryption/decryption operations.
 *
 *
 * \return  The status of the performed AES operation. The returned values should be interpreted using the AES_HASH_STATUS enum.
 *
 * \warning The number of bytes of the input vector that the DMA engine should process should comply with the following restrictions.
 *          Otherwise, application is responsible for performing zero padding.
 *
 *
 * mode                | moreDataToCome = true  | moreDataToCome = false
 * ------------------- | ---------------------- | ----------------------
 * HW_AES_ECB          | multiple of 16 (bytes) | multiple of 16 (bytes)
 * HW_AES_CBC          | multiple of 16         | no restrictions
 * HW_AES_CTR          | multiple of 16         | no restrictions
 * HW_HASH_MD5         | multiple of 8          | no restrictions
 * HW_HASH_SHA_1       | multiple of 8          | no restrictions
 * HW_HASH_SHA_256_224 | multiple of 8          | no restrictions
 * HW_HASH_SHA_256     | multiple of 8          | no restrictions
 * HW_HASH_SHA_384     | multiple of 8          | no restrictions
 * HW_HASH_SHA_512     | multiple of 8          | no restrictions
 * HW_HASH_SHA_512_224 | multiple of 8          | no restrictions
 * HW_HASH_SHA_512_256 | multiple of 8          | no restrictions
 *
 */
AES_HASH_STATUS _crypto_aes_fragmented_input(hw_aes_hash_mode aes_mode, hw_aes_direction direction,
                           const uint8 *aes_key, hw_aes_key_size key_size,
                           hw_aes_hash_key_exp_t key_exp_flag,
                           const uint8 *iv, const uint8 *ic,
                           const aes_hash_frag_t src[], uint8 *dst, unsigned int src_len,
                           OS_TICK_TIME timeout);


/*
 * @brief Perform AES encryption/decryption operations using non-fragmented input blocks
 *
 * param [in] aes_mode       The desired AES mode. Valid values are HW_AES_ECB, HW_AES_CBC, HW_AES_CTR.
 *
 * param [in] direction      Select whether an encryption or decryption operation will be executed
 *
 * param [in] aes_key        The physical address of the buffer containing the crypto keys. If key expansion
 *                           is performed by the crypto engine, developer should provide only the base crypto
 *                           key. Otherwise, the buffer should contain all the expanded keys.
 *
 * param [in] key_size       The size of the crypto keys used during encryption/decryption operations.
 *
 * param [in] key_exp_flag   Defines whether key expansion will be performed by the crypto engine or application.
 *                           If key expansion is performed by the engine, then \p aes_key should contain only the
 *                           base key.
 *
 * param [in] iv             The physical address of the buffer containing the Initialization Vector. This parameter
 *                           has a meaning only when the selected AES mode is the AES-CBC. If not used, this value
 *                           should be set to NULL.
 *
 * param [in] ic             The physical address of the buffer containing the Initialization Counter. This parameters
 *                           has a meaning only when the selected AES mode is the AES-CTR. If not used, this value
 *                           should be set to NULL.
 *
 * param [in] src            The physical address of the buffer containing the Input Vector.
 *
 * param [in] dst            The physical address of the buffer where the output of the AES operation will be stored.
 *
 * param [in] src_len        The number of bytes of the input vector that the DMA engine should process.
 *
 * param [in] timeout        The maximum number of ticks to wait either to acquire the crypto engine or to perform
 *                           an AES encryption/decryption operation.
 *
 *
 * \return  The status of the performed AES operation. The returned values should be
 *          interpreted using the AES_HASH_STATUS enum.
 *
 * \warning The \p src_len should comply with the following restrictions. Otherwise,
 *          application is responsible for performing zero padding.
 *
 *
 * mode                | moreDataToCome = true  | moreDataToCome = false
 * ------------------- | ---------------------- | ----------------------
 * HW_AES_ECB          | multiple of 16 (bytes) | multiple of 16 (bytes)
 * HW_AES_CBC          | multiple of 16         | no restrictions
 * HW_AES_CTR          | multiple of 16         | no restrictions
 * HW_HASH_MD5         | multiple of 8          | no restrictions
 * HW_HASH_SHA_1       | multiple of 8          | no restrictions
 * HW_HASH_SHA_256_224 | multiple of 8          | no restrictions
 * HW_HASH_SHA_256     | multiple of 8          | no restrictions
 * HW_HASH_SHA_384     | multiple of 8          | no restrictions
 * HW_HASH_SHA_512     | multiple of 8          | no restrictions
 * HW_HASH_SHA_512_224 | multiple of 8          | no restrictions
 * HW_HASH_SHA_512_256 | multiple of 8          | no restrictions
 *
 */
AES_HASH_STATUS _crypto_aes_non_fragmented_input(hw_aes_hash_mode aes_mode, hw_aes_direction direction,
                           const uint8 *aes_key, hw_aes_key_size key_size,
                           hw_aes_hash_key_exp_t key_exp_flag,
                           const uint8 *iv, const uint8 *ic,
                           const uint8 *src, uint8 *dst, unsigned int src_len,
                           OS_TICK_TIME timeout);


/*
 * @brief Perform hash operations using non-fragmented input vectors
 *
 * \param [in] hash_mode   The desired hash mode.
 *
 * \param [in] result_size The number of bytes that the DMA should write to the destination buffer after the completion of a hash operation.
 *                         Out of range values are adjusted to the closest limit.
 *
 * \param [in] timeout     The maximum number of ticks to wait either to acquire the crypto engine or to perform a hash operation.
 *
 * \param [in] src         The physical address of the buffer containing the input vector.
 *
 * \param [in] dst         The physical address of the buffer where the output of the operation will be stored.
 *
 * \param [in] src_len     The number of bytes of the input vector that the DMA engine should process.
 *
 * \return  The status of the performed hash operation. The returned values should be interpreted using the AES_HASH_STATUS enum.
 *
 * \note \p result_size should be selected based on the hash algorithm used:
 *
 *       mode          | maximum output len
 *       ------------- | ------------------
 *       MD5           | 1 - 16 bytes
 *       SHA-1         | 1 - 20 bytes
 *       SHA-256       | 1 - 32 bytes
 *       SHA-256/224   | 1 - 28 bytes
 *       SHA-384       | 1 - 48 bytes
 *       SHA-512       | 1 - 64 bytes
 *       SHA-512/224   | 1 - 28 bytes
 *       SHA-512/256   | 1 - 32 bytes
 *
 *
 * \note The \p src_len should comply with the following restrictions. Otherwise, data
 *       are automatically expanded with zeros.
 *
 *       mode                | moreDataToCome = true  | moreDataToCome = false
 *       ------------------- | ---------------------- | ----------------------
 *       HW_AES_ECB          | multiple of 16 (bytes) | multiple of 16 (bytes)
 *       HW_AES_CBC          | multiple of 16         | no restrictions
 *       HW_AES_CTR          | multiple of 16         | no restrictions
 *       HW_HASH_MD5         | multiple of 8          | no restrictions
 *       HW_HASH_SHA_1       | multiple of 8          | no restrictions
 *       HW_HASH_SHA_256_224 | multiple of 8          | no restrictions
 *       HW_HASH_SHA_256     | multiple of 8          | no restrictions
 *       HW_HASH_SHA_384     | multiple of 8          | no restrictions
 *       HW_HASH_SHA_512     | multiple of 8          | no restrictions
 *       HW_HASH_SHA_512_224 | multiple of 8          | no restrictions
 *       HW_HASH_SHA_512_256 | multiple of 8          | no restrictions
 */
AES_HASH_STATUS _crypto_hash_non_fragmented_input(hw_aes_hash_mode hash_mode, unsigned int result_size,
                                                   const uint8 *src, uint8 *dst,
                                                   unsigned int src_len, OS_TICK_TIME timeout);


/*
 * @brief Perform hash operations using fragmented input vectors
 *
 * \param [in] hash_mode   The desired hash mode.
 *
 * \param [in] result_size The number of bytes that the DMA should write to the destination buffer after the
 *                         completion of a hash operation. Out of range values are adjusted to the closest limit.
 *
 * \param [in] timeout     The maximum number of ticks to wait either to acquire the crypto engine or to perform a hash operation.
 *
 * \param [in] src         An array of type aes_hash_frag_t which holds all the info related to the fragmented input vectors
 *
 * \param [in] dst         The physical address of the buffer where the output of the operation will be stored.
 *
 * \param [in] src_len     The number of elements the array \p src consists of.
 *
 * \return  The status of the performed hash operation. The returned values should be interpreted using the AES_HASH_STATUS enum.
 *
 * \note \p result_size should be selected based on the hash algorithm used:
 *
 *       mode          | maximum output len
 *       ------------- | ------------------
 *       MD5           | 1 - 16 bytes
 *       SHA-1         | 1 - 20 bytes
 *       SHA-256       | 1 - 32 bytes
 *       SHA-256/224   | 1 - 28 bytes
 *       SHA-384       | 1 - 48 bytes
 *       SHA-512       | 1 - 64 bytes
 *       SHA-512/224   | 1 - 28 bytes
 *       SHA-512/256   | 1 - 32 bytes
 *
 *
 * \note The number of bytes of the input vector that the DMA engine should process should comply with
 *       the following restrictions. Otherwise, data are automatically expanded with zeros.
 *
 *
 *       mode                | moreDataToCome = true  | moreDataToCome = false
 *       ------------------- | ---------------------- | ----------------------
 *       HW_AES_ECB          | multiple of 16 (bytes) | multiple of 16 (bytes)
 *       HW_AES_CBC          | multiple of 16         | no restrictions
 *       HW_AES_CTR          | multiple of 16         | no restrictions
 *       HW_HASH_MD5         | multiple of 8          | no restrictions
 *       HW_HASH_SHA_1       | multiple of 8          | no restrictions
 *       HW_HASH_SHA_256_224 | multiple of 8          | no restrictions
 *       HW_HASH_SHA_256     | multiple of 8          | no restrictions
 *       HW_HASH_SHA_384     | multiple of 8          | no restrictions
 *       HW_HASH_SHA_512     | multiple of 8          | no restrictions
 *       HW_HASH_SHA_512_224 | multiple of 8          | no restrictions
 *       HW_HASH_SHA_512_256 | multiple of 8          | no restrictions
 */
AES_HASH_STATUS _crypto_hash_fragmented_input(hw_aes_hash_mode hash_mode, unsigned int result_size,
                                               const aes_hash_frag_t src[], uint8 *dst,
                                               unsigned int src_len, OS_TICK_TIME timeout);

#endif /* AES_HASH_FUNCTIONALITY_H_ */
