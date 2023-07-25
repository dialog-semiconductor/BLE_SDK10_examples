/**
 ****************************************************************************************
 *
 * @file partition_table.h
 *
 * @brief Partition table selection. Image partition's size definition.
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

/*
 * When partition_table is not overridden by adding a custom partition_table.h file to a project
 * then this file is used to select partition table by macro definition.
 *
 * To use layout other than SDK one, add include path into the project settings that will point
 * to a folder with the custom partition_table file.
 */


#if defined(USE_PARTITION_TABLE_1MB_WITH_SUOTA)
#include <1M/suota/partition_table.h>
#elif defined(USE_PARTITION_TABLE_4MB_WITH_SUOTA)
#include <4M/suota/partition_table.h>
#elif defined(USE_FATFS_PARTITION_TABLE_4M)
#include <partition_table_fatfs.h>
#else
#include <4M/partition_table.h>
#endif


/*
 * Define a maximal size of the image which could be written to QSPI - based on the partition sizes.
 */
#if defined(NVMS_FW_EXEC_PART_SIZE) && defined(NVMS_FW_UPDATE_PART_SIZE)
#if NVMS_FW_EXEC_PART_SIZE < NVMS_FW_UPDATE_PART_SIZE
#define IMAGE_PARTITION_SIZE    NVMS_FW_EXEC_PART_SIZE
#else
#define IMAGE_PARTITION_SIZE    NVMS_FW_UPDATE_PART_SIZE
#endif /* NVMS_FW_EXEC_PART_SIZE < NVMS_FW_UPDATE_PART_SIZE */
#elif defined(NVMS_FIRMWARE_PART_SIZE)
#define IMAGE_PARTITION_SIZE    NVMS_FIRMWARE_PART_SIZE
#else
#error "At least one partition where application could be placed should be defined!"
#endif /* defined(NVMS_FW_EXEC_PART_SIZE) && defined(NVMS_FW_UPDATE_PART_SIZE) */


