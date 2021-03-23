/**
 ****************************************************************************************
 *
 * @file QRWidget.c
 *
 * @brief Custom widget support source code
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "GUI.h"
#include "WM.h"
#include "osal.h"
#include "QRWidget.h"

/*
 * Define default values for all widget properties, if any.
 */
static const QR_WIDGET_Obj WidgetDefaultData = {
        0,                                                 //! Widget state (pressed/unpressed)
        0,                                                 //! Number of extra bytes (allocated by user)
        "www.dialog-semiconductor.com",                    //! Pointer to QR data
        0,                                                 //! QR object handle
        0,                                                 //! Widget Id
};

/*
 * Routine to react on PID events
 */
static void _OnTouch(QR_WIDGET_Handle hWin, WM_MESSAGE * pMsg, QR_WIDGET_Obj * pData)
{
        int Notification;
        QR_WIDGET_Obj * pWidgetData;
        GUI_PID_STATE * pPIDState;

        pWidgetData = pData;
        pPIDState = (GUI_PID_STATE *)pMsg->Data.p;

        if (pPIDState) {
                // Check if pressed state is updated
                if (pWidgetData->Pressed != pPIDState->Pressed) {
                        pWidgetData->Pressed = pPIDState->Pressed;
                        WM_SetUserData(hWin, pWidgetData, sizeof(QR_WIDGET_Obj));
                        if (pPIDState->Pressed) {
                                Notification = QR_WIDGET_NOTIFICATION_CLICKED;
                                WM_SetFocus(hWin); // WM_SET_FOCUS
                        }
                        else {
                                Notification = QR_WIDGET_NOTIFICATION_UNPRESSED;
                        }
                        // Notify the parent window so, it can react on the changes
                        WM_NotifyParent(hWin, Notification);
                }
        // Touch events are no longer valid
        } else {
                Notification = QR_WIDGET_NOTIFICATION_MOVED_OUT;
                // Notify the parent window so, it can react on the changes
                WM_NotifyParent(hWin, Notification);
        }
        // Invalidate window so, it can be redrawn immediately with the new properties
        WM_InvalidateWindow(hWin);
}

/*
 * Widget-specific callback function
 */
void CustomWidgetQR_Callback(WM_MESSAGE * pMsg)
{
        QR_WIDGET_Obj WidgetData;
        GUI_RECT WinRect;
        WM_HWIN hWin;

        hWin = pMsg->hWin;

        WM_GetWindowRectEx(hWin, &WinRect);
        GUI_MoveRect(&WinRect, -WinRect.x0, -WinRect.y0); // Required!!!

        // Get user-defined data
        WM_GetUserData(hWin, &WidgetData, sizeof(WidgetData));

        switch(pMsg->MsgId) {
        case WM_TOUCH:
                // React on touch events
                _OnTouch(hWin, pMsg, &WidgetData);
                break;
        case WM_PAINT:
                if (WM_HasFocus(hWin)) {
                        // Add your own functionality
                }

                // Draw the QR object
                GUI_SetColor(GUI_WHITE);
                GUI_FillRect(WinRect.x0, WinRect.y0, WinRect.x1, WinRect.y1);
                GUI_QR_Draw(WidgetData.hQR, WinRect.x0 + (QR_WIDGET_WHITE_FRAME_SIZE >> 1), WinRect.y0 + (QR_WIDGET_WHITE_FRAME_SIZE >> 1));
                break;
        case WM_SET_FOCUS:
                if (pMsg->Data.v) {
                        pMsg->Data.v = 0; // Accept the focus by setting the message value to zero.
                }
                WM_InvalidateWindow(hWin); // Invalidate window so focus is drawn immediately
                break;
        case WM_SET_ID: // Triggered when calling WM_SetId()
                WidgetData.Id = pMsg->Data.v;
                break;
        case WM_GET_ID: // Triggered when calling WM_GetId()
                pMsg->Data.v = WidgetData.Id;
                break;
        default:
                WM_DefaultProc(pMsg);
        }
}

/*
 *  Get the handle of the generated QR object.
 */
GUI_HMEM _CustomWidgetGetQRHandle(QR_WIDGET_Handle hWin)
{
        QR_WIDGET_Obj WidgetData;
        WM_GetUserData(hWin, &WidgetData, sizeof(QR_WIDGET_Obj));

        return WidgetData.hQR;
}

/*
 * Widget creation routine
 *
 * \note The widget size is dependent on the QR data and so, it's calculated internally.
 *
 */
QR_WIDGET_Handle _CustomWidgetQRCreate(int x0, int y0, WM_HWIN hWinParent, U32 Style, WM_CALLBACK * pfCallback, const char *pQRData,  int NumExtraBytes, int Id)
{
        WM_HWIN hWin;
        GUI_QR_INFO QRInfo;
        QR_WIDGET_Obj WidgetData;
        WM_CALLBACK * pfUsed;
        const char *pQRText;

        WidgetData = WidgetDefaultData;

        // Routine should Check if user has declared their own QR data
        if (pQRData) {
                pQRText = pQRData;
        } else {
                pQRText = WidgetData.pQRData;
        }

        // Routine should check if user has defined their own callback function for the widget
        if (pfCallback) {
                pfUsed = pfCallback;
        } else {
                pfUsed = CustomWidgetQR_Callback;
        }
        WidgetData.NumExtraBytes = NumExtraBytes;
        WidgetData.Id = Id;

        // Create a QR object
        WidgetData.hQR = GUI_QR_Create(pQRText, QR_WIDGET_PIXEL_SIZE, GUI_QR_ECLEVEL_H, 0);
        GUI_QR_GetInfo(WidgetData.hQR, &QRInfo);

        // A widget is actually a window with enhanced functionality
        hWin = WM_CreateWindowAsChild(x0, y0, QRInfo.Size + QR_WIDGET_WHITE_FRAME_SIZE,  QRInfo.Size + QR_WIDGET_WHITE_FRAME_SIZE, hWinParent,
                                                                                                 Style, pfUsed, sizeof(QR_WIDGET_Obj) + NumExtraBytes);

        WM_SetId(hWin, Id);
        WM_SetUserData(hWin, &WidgetData, sizeof(WidgetData));

        return hWin;
}

/*
 * Get user-defined data set for the custom widget
 */
int _CustomWidgetGetUserData(QR_WIDGET_Handle hWin, void * pDest, int SizeOfBuffer)
{
        QR_WIDGET_Obj QR_DATA;
        int NumBytes;
        U8 * pExtraBytes;

        if (SizeOfBuffer <= 0) {
                return 0; // Invalid requested data
        }
        // Retrieve the data set with WM_SetUserData()
        WM_GetUserData(hWin, &QR_DATA, sizeof(QR_DATA));

        pExtraBytes = (U8 *)OS_MALLOC(sizeof(QR_WIDGET_Obj) + QR_DATA.NumExtraBytes);
        if(pExtraBytes) {
                // Retrieve all user-defined data
                WM_GetUserData(hWin, pExtraBytes, sizeof(QR_WIDGET_Obj) + QR_DATA.NumExtraBytes);
                // Check the boundaries
                if (SizeOfBuffer >= QR_DATA.NumExtraBytes) {
                        NumBytes = QR_DATA.NumExtraBytes;
                } else {
                        NumBytes = SizeOfBuffer;
                }
                GUI_MEMCPY(pDest, pExtraBytes + sizeof(QR_WIDGET_Obj), NumBytes);
                OS_FREE(pExtraBytes);

                return NumBytes; // Ok
        }
        return 0; // Memory allocation failed
}

/*
 *  Set user-defined data for the custom widget.
 */
int _CustomWidgetSetUserData(QR_WIDGET_Handle hWin, void * pSrc, int SizeOfBuffer)
{
        QR_WIDGET_Obj QR_DATA;
        int NumBytes;
        U8 * pExtraBytes;

        if (SizeOfBuffer <= 0) {
                return 0; // Invalid requested data
        }
        WM_GetUserData(hWin, &QR_DATA, sizeof(QR_DATA));

        pExtraBytes = (U8 *)OS_MALLOC(sizeof(QR_DATA) + QR_DATA.NumExtraBytes);
        if (pExtraBytes) {
                // Retrieve all user-defined data
                WM_GetUserData(hWin, pExtraBytes, sizeof(QR_DATA) + QR_DATA.NumExtraBytes);
                // Check the boundaries
                if (SizeOfBuffer >= QR_DATA.NumExtraBytes) {
                        NumBytes = QR_DATA.NumExtraBytes;
                } else {
                        NumBytes = SizeOfBuffer;
                }
                GUI_MEMCPY(pExtraBytes + sizeof(QR_WIDGET_Obj), pSrc, NumBytes);
                WM_SetUserData(hWin, pExtraBytes, sizeof(QR_DATA) + QR_DATA.NumExtraBytes);
                OS_FREE(pExtraBytes);

                return NumBytes; // Ok
        }
        return 0; // Memory allocation failed
}
