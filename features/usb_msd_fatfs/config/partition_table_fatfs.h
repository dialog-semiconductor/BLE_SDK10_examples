/**
 ****************************************************************************************
 *
 * @file partition_table_fatfs.h
 *
 * Copyright (C) 2018-2021 Renesas Electronics Corporation and/or its affiliates
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


#define NVMS_PRODUCT_HEADER_PART_START  0x000000        /* Mandatory to be at zero */
#define NVMS_PRODUCT_HEADER_PART_SIZE   0x002000        /* Mandatory to have size 0x2000 */

#define NVMS_PARTITION_TABLE_START      0x002000
#define NVMS_PARTITION_TABLE_SIZE       0x001000        /* Mandatory to have size 0x1000 */

#define NVMS_FIRMWARE_PART_START        0x003000        /* FW partitions Alignment to 512KB step is dictated
                                                         * by the default FLASH_REGION_SIZE with Max Offset <0x3C00.
                                                         * Check Datasheet for details on FLASH_REGION_SIZE options */
#define NVMS_FIRMWARE_PART_SIZE         0x07D000


/*----------- 128Mbits FW partitions limit ----------*/
/* All FW partitions must be fit up to 128Mbps limit */
/* if SUOTA is used also the SUOTA partition must fit*/
/* in the first 128Mbits of the FLASH space          */
/*---------------------------------------------------*/

#define NVMS_GENERIC_PART_START         0x080000        /* used by the BLE to store pairing info and by BLE storage
                                                         * emulates E2PROM and is intended for frequent writes.
                                                         * It ia power safe and it is using wearing of the cells to
                                                         * provide with default settings 8 times longer life */
#define NVMS_GENERIC_PART_SIZE          0x020000        /* the actual useful space will be < (NVMS_GENERIC_PART_SIZE/8) with default SDK
                                                         * configuration */

#define NVMS_FATFS_PART_START           0x0A0000
#define NVMS_FATFS_PART_SIZE            0x35F000

#define NVMS_PARAM_PART_START           0x3FF000
#define NVMS_PARAM_PART_SIZE            0x001000

PARTITION2( NVMS_PRODUCT_HEADER_PART  , 0 )
PARTITION2( NVMS_PARTITION_TABLE      , PARTITION_FLAG_READ_ONLY )
PARTITION2( NVMS_FIRMWARE_PART        , 0 )
PARTITION2( NVMS_PARAM_PART           , 0 )
PARTITION2( NVMS_GENERIC_PART         , PARTITION_FLAG_VES )
PARTITION2( NVMS_FATFS_PART           , 0 )


