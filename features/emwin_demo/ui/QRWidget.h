/**
 ****************************************************************************************
 *
 * @file QRWidget.h
 *
 * @brief Custom widget support header file
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#ifndef QR_WIDGET_H_
#define QR_WIDGET_H_

#include "WM.h"
#include "osal.h"

/**********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef WM_HMEM QR_WIDGET_Handle;

typedef struct {
        U8 Pressed;             //! Widget state (pressed/unpressed)
        int NumExtraBytes;      //! Extra bytes that should be allocated for the widget
        const char *pQRData;    //! Pointer to QR data
        GUI_HMEM hQR;           //! QR object handle
        int Id;                 //! Widget Id
} QR_WIDGET_Obj;

/**********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define QR_WIDGET_PIXEL_SIZE               ( 2 )
#define QR_WIDGET_WHITE_FRAME_SIZE         ( 6 )
#define QR_WIDGET_NOTIFICATION_CLICKED     ( WM_NOTIFICATION_USER + 0 )
#define QR_WIDGET_NOTIFICATION_UNPRESSED   ( WM_NOTIFICATION_USER + 1 )
#define QR_WIDGET_NOTIFICATION_MOVED_OUT   ( WM_NOTIFICATION_USER + 2 )

/* Default QR Code callback function */
void CustomWidgetQR_Callback(WM_MESSAGE * pMsg);

/**
 * \brief Widget creation routine
 *
 * \param[in] x0             Leftmost pixel of the widget in parent coordinates
 *
 * \param[in] y0             Topmost pixel of the widget in parent coordinates
 *
 * \param[in] hWinParent     Handle of the parent window
 *
 * \param[in] Style          Child window (widget) configurations
 *
 * \param[in] pfCallback     Callback function of the child window (widget)
 *
 * \param[in] pQRData        Data to be displayed once the QR code is scanned. IF not set, the default value is displayed.
 *
 * \param[in] NumExtraBytes  Extra bytes allocated that should be allocated for the child window (widget). It should be set to zero if not required.
 *
 * \param[in] Id             Widget Id. A user-defined Id can be defined so that widget is distinguishable.
 *
 */
QR_WIDGET_Handle _CustomWidgetQRCreate(int x0, int y0, WM_HWIN hWinParent, U32 Style, WM_CALLBACK * pfCallback, const char * pQRData, int NumExtraBytes, int Id);

/**
 * \brief Get the handle of the generated QR object.
 *
 * \param[in] hWin   Widget handle
 *
 * \return Handle of the QR object
 */
GUI_HMEM _CustomWidgetGetQRHandle(QR_WIDGET_Handle hWin);

/**
 * \brief Get user-defined data set for the custom widget
 *
 * \param[in] hWin           Widget handle
 *
 * \param[in] pDest          Pointer to the destination buffer
 *
 * \param[in] SizeOfBuffer   Size of data to read
 *
 * \return Number of read bytes
 */
int _CustomWidgetGetUserData(QR_WIDGET_Handle hWin, void * pDest, int SizeOfBuffer);

/**
 * \brief Set user-defined data for the custom widget.
 *
 * \param[in] hWin           Handle of Widget
 *
 * \param[in] pDest          Pointer to the source buffer
 *
 * \param[in] SizeOfBuffer   Size of data to write
 *
 * \return Number of written bytes
 */
int _CustomWidgetSetUserData(QR_WIDGET_Handle hWin, void * pSrc, int SizeOfBuffer);

#endif /* QR_WIDGET_H_ */
