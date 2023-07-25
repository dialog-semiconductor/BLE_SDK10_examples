/**
 ****************************************************************************************
 *
 * @file partition_def.h
 *
 * @brief Partition table entry definition
 *
 * Copyright (C) 2016-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef PARTITION_DEF_H_
#define PARTITION_DEF_H_

/**
 * \brief NVMS Partition IDs
 */
typedef enum {
        NVMS_FIRMWARE_PART              = 1,
        NVMS_PARAM_PART                 = 2,
        NVMS_BIN_PART                   = 3,
        NVMS_LOG_PART                   = 4,
        NVMS_GENERIC_PART               = 5,
        NVMS_PLATFORM_PARAMS_PART       = 15,
        NVMS_PARTITION_TABLE            = 16,
        NVMS_FW_EXEC_PART               = 17,
        NVMS_FW_UPDATE_PART             = 18,
        NVMS_PRODUCT_HEADER_PART        = 19,
        NVMS_IMAGE_HEADER_PART          = 20,
        NVMS_FATFS_PART                 = 0xF0,
} nvms_partition_id_t;

/**
 * \brief Partition entry.
 */
typedef struct partition_entry_t {
        uint8_t magic;          /**< Partition magic number 0xEA */
        uint8_t type;           /**< Partition ID */
        uint8_t valid;          /**< Valid marker 0xFF */
        uint8_t flags;          /**< */
        uint16_t start_sector;  /**< Partition start sector */
        uint16_t sector_count;  /**< Number of sectors in partition */
        uint8_t reserved2[8];   /**< Reserved for future use */
} partition_entry_t;

#define PARTITION_FLAG_READ_ONLY        (1 << 0)
#define PARTITION_FLAG_VES              (1 << 1)

#endif /* PARTITION_DEF_H_ */
