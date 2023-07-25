/**
 ****************************************************************************************
 *
 * @file snc_i2c_demo.h
 *
 * Copyright (c) 2020-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef SNC_SPI_DEMO_INCLUDE_H_
#define SNC_SPI_DEMO_INCLUDE_H_

#include "ad_snc.h"

/*
 * \brief SNC initialization and uCode registration
 *
 * \param [in] _snc_ucode_cb   A callback function that is called as soon as
 *                             SNC notifies the CM33 core.
 *
 * \return uint32_t           The uCode ID of the registered uCode-Block
 **/
uint32_t _snc_reader_init(ad_snc_interrupt_cb _snc_ucode_cb);


#endif /* SNC_SPI_DEMO_INCLUDE_H_ */
