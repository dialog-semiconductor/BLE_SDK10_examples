/**
 ****************************************************************************************
 *
 * @file ExternalResources.h
 *
 * @brief External resources support header file
 *
 * Copyright (C) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef UI_EXTERNALRESOURCES_H_
#define UI_EXTERNALRESOURCES_H_

#include "ad_nvms.h"

/**********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define PARTITION_ID_XBF_FONT                        NVMS_LOG_PART        //! Flash partition ID where XBF data are stored to
#define XBF_FONT_TYPE                                GUI_XBF_TYPE_PROP    //! Font type stored in the flash memory
#define PARTITION_OFFSET_XBF_FONT                    0x00                 //! Offset required to access XBF data
#define PARTITION_ID_BITMAP_DATASTREAM               NVMS_LOG_PART        //! Flash partition ID where bitmap stream data are stored to
#define PARTITION_OFFSET_BITMAP_DATASTREAM           0x1AE0               //! Offset required to access bitmap stream data

/**********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef enum {
        XBF_STATUS_SUCCESS           = 0,
        XBF_STATUS_FAIL,
        XBF_STATUS_PARTITION_FAIL,
        XBF_STATUS_MALLOC_FAIL
} XBF_STATUS;

typedef nvms_t hPartition_t;

typedef struct {
        hPartition_t hPartition;    //! Partition handle (calculated internally based on \ref PartitionID)
        int PartitionID;            //! Flash partition ID (provided by the user)
        uint32_t Offset;            //! Flash partition offset (provided by the user)
} USERDATA_EXTERNAL_RESOURCES;

/*
 * \brief User-defined callback function used to retrieve bitmap stream data stored in the flash memory.
 *
 * \warning  \p p should be valid for as long as bitmap stream data are being retrieved
 */
int _cbGetBitmapStreamData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off);

/**
 * \brief Initialize a font stored in the flash memory in XBF format.
 *
 * \param[out]   pFont      The returned pointer points to the XBF font
 *
 * \param[out]   pXBF       The returned pointer points to XBF data
 *
 * \param[in]    pData      Pointer to user-defined data
 *
 * \return    Status of the XBF font creation. \sa XBF_STATUS can be used to interpret the status
 *
 * \note      The memory area allocated for both \p pFont and \p pXBF should be freed via \sa _XBFDeleteFont() once the XBF font is no longer needed
 *
 * \note      \p pFont should be used to select the font via \sa GUI_SetFont()
 *
 * \warning   \p pData should stay valid for as long as the XBF font is used
 */
int _XBFCreateFont(GUI_FONT ** pFont, GUI_XBF_DATA ** pXBF, const GUI_XBF_TYPE * pFontType, void * pData);

/**
 * \brief Delete an XBF font and release all its resources allocated at creation
 *
 * \param[in]  pFont       Pointer to the underlying font
 *
 * \param[in]  pXFB        Pointer to XBF data
 *
 * \note Both \p pFont and \p pXBF should be initiated via \sa _XBFCreateFont()
 *
 */
void _XBFDeleteFont(GUI_FONT * pFont, GUI_XBF_DATA * pXBF);

/**
 * \brief Open a specific flash partition
 *
 * \param[in]  PartitionID     Flash partition ID
 *
 * \return   Handle of the requested flash partition ID
 *
 */
hPartition_t _OpenPartition(int PartitionID);

#endif /* UI_EXTERNALRESOURCES_H_ */
