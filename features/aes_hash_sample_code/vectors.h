/**
 ****************************************************************************************
 *
 * @file vectors.h
 *
 * @brief Arbitrary Input Vectors used for demonstrating HAES/AES operations.
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

#ifndef VECTORS_H_
#define VECTORS_H_

/*
 * An arbitrary input vector used for AES operations.
 *
 * \warning The size of the input vector, expressed in bytes, should comply with the following
 *          restrictions. Otherwise, application must expand the input vector with zeros.
 *
 * mode                | moreDataToCome = true | moreDataToCome = false
 * ------------------- | --------------------- | ----------------------
 * HW_AES_ECB          | multiple of 16        | multiple of 16
 * HW_AES_CBC          | multiple of 16        | no restrictions
 * HW_AES_CTR          | multiple of 16        | no restrictions
 * HW_HASH_MD5         | multiple of 8         | no restrictions
 * HW_HASH_SHA_1       | multiple of 8         | no restrictions
 * HW_HASH_SHA_256_224 | multiple of 8         | no restrictions
 * HW_HASH_SHA_256     | multiple of 8         | no restrictions
 * HW_HASH_SHA_384     | multiple of 8         | no restrictions
 * HW_HASH_SHA_512     | multiple of 8         | no restrictions
 * HW_HASH_SHA_512_224 | multiple of 8         | no restrictions
 * HW_HASH_SHA_512_256 | multiple of 8         | no restrictions
 */
const uint8 aes_input_vector_buf[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                                       0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                                       0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                                       0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };

/*
 * Here we split the aes_input_vector_buf array into smaller chunks.
 * Thus, creating fragmented input data.
 */

/*
 * This is not the last input vector to be processed (moreDataToCome = true) and
 * thus its size should be multiple of 16.
 */
const uint8 aes_hash_frag_vect_1[] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};

/*
 * This is not the last input vector to be processed (moreDataToCome = true) and
 * thus its size should be multiple of 16.
 */
const uint8 aes_hash_frag_vect_2[] = {0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};

/*
 * This is not the last input vector to be processed (moreDataToCome = true) and
 * thus its size should be multiple of 16.
 */
const uint8 aes_hash_frag_vect_3[] = {0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef};

/*
 * This is the last input vector to be processed (moreDataToCome = false)
 * and thus its size must comply with the above table.
 */
const uint8 aes_hash_frag_vect_4[] = {0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};



/* An arbitrary 128-bit Initialization Vector (IV) required for AES-CBC operations */
const uint8 iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };


/* An arbitrary 128-bit Initialization Counter (IC) required for AES-CTR operations */
const uint8 ic[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };


/* An arbitrary 128-bit key (base) used for AES operations  */
const uint8 key_128b[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                             0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };


/* An arbitrary 192-bit key (base) used for AES operations  */
const uint8 key_192b[24] = { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
                             0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                             0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };



/*
 * An arbitrary input vector used for non fragmented hash operations.
 *
 * \note The size of the input vector, expressed in bytes, should comply with the
 *       following restrictions. Otherwise, data are automatically expanded with
 *       zeros.
 *
 * mode                | moreDataToCome = true | moreDataToCome = false
 * ------------------- | --------------------- | ----------------------
 * HW_AES_ECB          | multiple of 16        | multiple of 16
 * HW_AES_CBC          | multiple of 16        | no restrictions
 * HW_AES_CTR          | multiple of 16        | no restrictions
 * HW_HASH_MD5         | multiple of 8         | no restrictions
 * HW_HASH_SHA_1       | multiple of 8         | no restrictions
 * HW_HASH_SHA_256_224 | multiple of 8         | no restrictions
 * HW_HASH_SHA_256     | multiple of 8         | no restrictions
 * HW_HASH_SHA_384     | multiple of 8         | no restrictions
 * HW_HASH_SHA_512     | multiple of 8         | no restrictions
 * HW_HASH_SHA_512_224 | multiple of 8         | no restrictions
 * HW_HASH_SHA_512_256 | multiple of 8         | no restrictions
 */
const uint8 hash_input_vector[] = "This is confidential!";


/*
 * Here we split the hash_input_vector array into smaller chunks.
 * Thus, creating fragmented input data.
 */

/*
 * Since this is not the last input vector to be processed (moreDataToCome = true),
 * its size should be multiple of 8.
 */
const uint8 hash_frag_intut_vector_1[] = "This is ";

/*
 * Since this is not the last input vector to be processed (moreDataToCome = true),
 * its size should be multiple of 8.
 */
const uint8 hash_frag_input_vector_2[] = "confiden";

/*
 * Since this is the last input vector to be processed (moreDataToCome = false),
 * there are no restrictions for its size.
 */
const uint8 hash_frag_input_vector_3[] = "tial!";


/*
 * The SHA-256 hash of hash_input_vector input vector. We have
 * pre-calculated it and can be used for verification.
 */
const uint8 sha_256_hash[32] = { 0x45, 0x3b, 0x2c, 0xf5, 0x95, 0x64, 0x26, 0x7e,
                                 0xf6, 0x04, 0x84, 0x85, 0x5b, 0x00, 0x66, 0x3b,
                                 0xaa, 0xfe, 0xe5, 0x96, 0xc6, 0x4d, 0x78, 0xd3,
                                 0x9b, 0xae, 0x45, 0x5a, 0x69, 0x41, 0x03, 0xa3 };


#endif /* VECTORS_H_ */
