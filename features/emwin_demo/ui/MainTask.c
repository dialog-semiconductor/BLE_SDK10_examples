/**
 ****************************************************************************************
 *
 * @file MainTask.c
 *
 * @brief emWin demo that shows touch functionality
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "DIALOG.h"
#include "QRWidget.h"
#include "ExternalResources.h"
#include "gdi.h"

/**********************************************************************
 *
 *       Externals
 *
 **********************************************************************
 */
extern GUI_CONST_STORAGE GUI_BITMAP bmDiaSemi_D_54x80;
extern GUI_CONST_STORAGE GUI_BITMAP bmButtonPause_42x42;
extern GUI_CONST_STORAGE GUI_BITMAP bmButtonPlay_42x42;
extern GUI_CONST_STORAGE GUI_BITMAP bmButtonReset_42x42;

/**********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define BM_DIASEMI_0                     bmDiaSemi_D_54x80
#define BM_BUTTON_PLAY                   bmButtonPlay_42x42
#define BM_BUTTON_PAUSE                  bmButtonPause_42x42
#define BM_BUTTON_RESET                  bmButtonReset_42x42
#define MSG_ANIM_END                     (WM_USER + 0x00)
#define ANIMATION_SLICE_SIZE             10
#define COLOR_BLUE                       GUI_MAKE_COLOR(0x009C5B2C)
#define GUI_ID_WIDGET_QR                 (GUI_ID_USER + 0x00)
#define QR_CODE_TEXT_VALUE               "www.dialog-semiconductor.com"
#define TIMER_ID_DISPLAY_OFF             1
#define TIMER_TM_MS_DISPLAY_OFF          300000UL // 5 minutes
#define TIMER_ID_TEXT_COLOR              2
#define TIMER_TM_MS_TEXT_COLOR           100
#define MOTION_NUM_OF_PAGES              2
#define MOTION_DECELERATION_PERIOD       500
#define LOG_TEXT_COLOR                   GUI_WHITE
#define LOG_PAGE_GRADIENT_COLOR_MAX      GUI_BLUE
#define LOG_PAGE_GRADIENT_COLOR_MIN      GUI_MAKE_COLOR(0x00FFF800)
#define LOG_PAGE_GRADIENT_CORNER_RADIUS  15
#define LOG_TOUCH_COORDINATES            "\n\r  TOUCH STATE: \n\r\n\r- Touch X: %d\n\r- Touch Y: %d\n\r- Touch State: %s\n\r"
#define LOG_PROJECT_NAME                 "DA1469x SmartBond\n\r\n\remWin Graphic Library\n\rand GUI"
#define LOG_PROJECT_NAME_UNICODE         "DA1469x SmartBond\x00AE\n\r\n\remWin Graphic Library\n\rand GUI"
#define USE_XBF_FONT                     0
#define USE_BITMAP_DATASTREAM            0
#define USE_TOUCH_OPERATIONS             GDI_TOUCH_ENABLE

/**********************************************************************
 *
 *       Macros
 *
 **********************************************************************
 */
#define LOG_DISPLAY_MODEL_INFO(_DisplayShield, _DisplayDriver, _DisplaySizeX, _DisplaySizeY, _DisplaySpeed,  _DisplayInteface)    \
                                                                                                                                  \
                                                                         "\n\r  DISPLAY INFORMATION: \n\r"                        \
                                                                         "\n\r- Display Shield: "#_DisplayShield                  \
                                                                         "\n\r- Display Driver: "#_DisplayDriver                  \
                                                                         "\n\r- Display X Size: "#_DisplaySizeX                   \
                                                                         "\n\r- Display Y Size: "#_DisplaySizeY                   \
                                                                         "\n\r- Display Speed: "#_DisplaySpeed                    \
                                                                         "\n\r- Display Interface: "#_DisplayInteface             \
                                                                         "\n\r\n\r"                                               \

#define LOG_TOUCH_DRIVER_INFO(_TouchDriver, _TouchInterface, _TouchDriverSpeed, _TouchPanel)    \
                                                                                                \
                                      "\n\r  TOUCH DRIVER INFORMATION: \n\r"                    \
                                      "\n\r- Touch Driver Model: "#_TouchDriver                 \
                                      "\n\r- Touch Driver Interface: "#_TouchInterface          \
                                      "\n\r- Touch Driver Speed: "#_TouchDriverSpeed            \
                                      "\n\r- Touch Panel: "#_TouchPanel                         \
                                      "\n\r\n\r"

/**********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef enum {
        APP_ANIM_DIASEMI_INIT = 0,
        APP_ANIM_DIASEMI_IN,
        APP_ANIM_DIASEMI_OUT,
        APP_ANIM_QR_IN,
        APP_ANIM_QR_OUT,
        APP_ANIM_END,
} APP_ANIM_INST;

typedef enum {
        WINDOW_PAGE_OVERLAP_MAX = - 2,
        WINDOW_PAGE_OVERLAP_MIN,
        WINDOW_PAGE_DESKTOP,
} SWIPE_Y_AXIS_PAGE;

typedef struct {
        int xPos;
        int yPos;
        int xDiff;
        int yDiff;
        int xDiffPrev;
        int yDiffPrev;
        int xSize;
        int ySize;
} ANIM_INST_DATA;

typedef struct {
        WM_HWIN hWin;
        APP_ANIM_INST state;
        ANIM_INST_DATA Data[3];
        int SliderValue;
} ANIM_DATA;

typedef enum {
        ANIM_STATE_PLAY   = 0,
        ANIM_STATE_PAUSE
} ANIM_STATE;

typedef struct {
        bool isImageMoved;
        WM_HTIMER hTimerDisplay;
        SLIDER_Handle hSliderAnimSpeed;
} USERDATA_WIDGET_IMAGE;

typedef struct {
        bool isQRCodeMoved;
        WM_HTIMER hTimerDisplay;
        SLIDER_Handle hSliderAnimSpeed;
} USERDATA_WIDGET_QRCODE;

typedef struct {
        SLIDER_Handle hSliderAnimSpeed;
        BUTTON_Handle hButtonPlayPause;
        BUTTON_Handle hButtonReset;
        IMAGE_Handle hDialogLogo;
        QR_WIDGET_Handle hQR;
        WM_HTIMER hTimerDisplay;
        GUI_COLOR FrameBkColor;
        ANIM_STATE AnimState;
        int SliderValue;
        int SliderAnimSpeedPosX;
        int SliderAnimSpeedPosY;
        int SliderAnimSpeedSizeX;
        int SliderAnimSpeedSizeY;
        int ButtonPlayPausePosX;
        int ButtonPlayPausePosY;
        int ButtonResetPosX;
        int ButtonResetPosY;
        int ButtonPlayPauseSizeX;
        int ButtonPlayPauseSizeY;
        int ButtonResetSizeX;
        int ButtonResetSizeY;
        int QRCodePosX;
        int QRCodePosY;
        int DialogLogoPosX;
        int DialogLogoPosY;
        int QRCodeSizeX;
        int QRCodeSizeY;
        bool isDialogLogoDisplayed;
        bool isSliderAnimSpeedValueChanged;
} APP_WIDGET_DATA;

/*********************************************************************
 *
 *       Static code
 *
 *********************************************************************
 */
/*********************************************************************
 *
 *       _AnimateInDiaSemi
 */
static void _AnimateInDiaSemi0(GUI_ANIM_INFO * pInfo, void * pVoid)
{
        ANIM_DATA * pData = (ANIM_DATA *)pVoid;

        if (pData->state < APP_ANIM_DIASEMI_IN) {
                pData->state = APP_ANIM_DIASEMI_IN;
        }
        pData->Data[0].xDiff = pData->Data[0].xSize
                - (pData->Data[0].xSize * pInfo->Pos) / GUI_ANIM_RANGE;
        WM_InvalidateWindow(pData->hWin);
}

/*********************************************************************
 *
 *       _AnimateOutDiaSemi
 */
static void _AnimateOutDiaSemi(GUI_ANIM_INFO * pInfo, void * pVoid)
{
        ANIM_DATA * pData = (ANIM_DATA *)pVoid;
        int Diff;

        if (pData->state < APP_ANIM_DIASEMI_OUT) {
                pData->state = APP_ANIM_DIASEMI_OUT;
        }
        Diff = -(pData->Data[0].xSize * pInfo->Pos) / GUI_ANIM_RANGE;
        pData->Data[0].xDiff = Diff;
        pData->Data[1].xDiff = Diff;
        WM_InvalidateWindow(pData->hWin);
}

/*********************************************************************
 *
 *       _AnimateInQR
 */
static void _AnimateInQR(GUI_ANIM_INFO * pInfo, void * pVoid)
{
        ANIM_DATA * pData = (ANIM_DATA *)pVoid;

        if (pData->state < APP_ANIM_QR_IN) {
                pData->state = APP_ANIM_QR_IN;
        }
        pData->Data[2].yDiff = pData->Data[2].ySize
                - (pData->Data[2].ySize * pInfo->Pos) / GUI_ANIM_RANGE;
        WM_InvalidateWindow(pData->hWin);
}

/*********************************************************************
 *
 *       _AnimateOutQR
 */
static void _AnimateOutQR(GUI_ANIM_INFO * pInfo, void * pVoid)
{
        ANIM_DATA * pData = (ANIM_DATA *)pVoid;
        int Diff;

        if (pData->state < APP_ANIM_QR_OUT) {
                pData->state = APP_ANIM_QR_OUT;
        }
        Diff = (pData->Data[1].xSize * pInfo->Pos) / GUI_ANIM_RANGE;
        pData->Data[1].xDiff = -pData->Data[1].xSize + Diff;
        pData->Data[2].xDiff = Diff;
        WM_InvalidateWindow(pData->hWin);
}

/*********************************************************************
 *
 *       _OnDelete
 */
static void _OnDelete(void * pVoid)
{
        ANIM_DATA * pData = (ANIM_DATA *)pVoid;
        WM_SendMessageNoPara(pData->hWin, MSG_ANIM_END);
}

/*************************************************************************
 *
 *       _AnimateCreate
 */
static void _AnimateCreate(GUI_ANIM_HANDLE * pAnim, ANIM_DATA * pAnimData)
{
        uint8_t AnimfItems;
        uint32_t AnimOffset, AnimWindowMax, AnimWindowMin, AnimSpeed, AnimTime;

        AnimfItems = 4;
        AnimSpeed  = pAnimData->SliderValue;
        AnimOffset = 500;
        //
        // Calculate the total Anim. time period.
        AnimTime   = ((AnimOffset * (AnimfItems << 1) - AnimOffset) + ((AnimSpeed + (AnimSpeed >> 2)) * AnimfItems));

        *pAnim = GUI_ANIM_Create(AnimTime, ANIMATION_SLICE_SIZE, (void *)pAnimData, 0);

        AnimWindowMin = 0;
        AnimWindowMax = AnimWindowMin + AnimOffset + AnimSpeed;
        GUI_ANIM_AddItem(*pAnim, AnimWindowMin, AnimWindowMax, ANIM_ACCELDECEL, (void *)pAnimData, _AnimateInDiaSemi0);

        AnimWindowMin = AnimWindowMax + AnimOffset + (AnimSpeed >> 2);
        AnimWindowMax = AnimWindowMin + AnimOffset + AnimSpeed;
        GUI_ANIM_AddItem(*pAnim, AnimWindowMin, AnimWindowMax, ANIM_ACCELDECEL, (void *)pAnimData, _AnimateOutDiaSemi);

        AnimWindowMin = AnimWindowMax + AnimOffset + (AnimSpeed >> 2);
        AnimWindowMax = AnimWindowMin + AnimOffset + AnimSpeed;
        GUI_ANIM_AddItem(*pAnim, AnimWindowMin, AnimWindowMax, ANIM_ACCELDECEL, (void *)pAnimData, _AnimateInQR);

        AnimWindowMin = AnimWindowMax + AnimOffset + (AnimSpeed >> 2);
        AnimWindowMax = AnimWindowMin + AnimOffset + AnimSpeed;
        GUI_ANIM_AddItem(*pAnim, AnimWindowMin, AnimWindowMax, ANIM_ACCELDECEL, (void *)pAnimData, _AnimateOutQR);

        GUI_ANIM_StartEx(*pAnim, 1, _OnDelete);
}

/***************************************************************************************
 *
 *       _SetupData
 */
static void _RestoreData(ANIM_DATA * pDataDst, ANIM_DATA * pDataSrc)
{
        ASSERT_WARNING(pDataDst);
        ASSERT_WARNING(pDataSrc);
        memcpy(pDataDst, pDataSrc, sizeof(ANIM_DATA));
}

/***************************************************************************************
 *
 * User-defined callback function used to overwrite the default Image widget callback
 */
static void _cbIMAGE(WM_MESSAGE * pMsg)
{


        switch (pMsg->MsgId) {
        case WM_TOUCH:
        {
                USERDATA_WIDGET_IMAGE UserDataDialogLogo;
                GUI_PID_STATE *pState = (GUI_PID_STATE *)pMsg->Data.p;

                IMAGE_GetUserData(pMsg->hWin, &UserDataDialogLogo, sizeof(UserDataDialogLogo));
                UserDataDialogLogo.isImageMoved = 1;
                IMAGE_SetUserData(pMsg->hWin, &UserDataDialogLogo, sizeof(UserDataDialogLogo));

#if USE_TOUCH_OPERATIONS
                WM_RestartTimer(UserDataDialogLogo.hTimerDisplay, 0);
#endif
                if (pState->Pressed) {
                    WM_SetUntouchable(UserDataDialogLogo.hSliderAnimSpeed, 1);
                }
                else {
                    WM_SetUntouchable(UserDataDialogLogo.hSliderAnimSpeed, 0);
                }
                IMAGE_Callback(pMsg); // Call the default widget's callback
                break;
        }
        default:
                IMAGE_Callback(pMsg);
                break;
        }
}

/***************************************************************************************
 *
 * User-defined callback function used to overwrite the default QR widget callback
 */
static void _cbQR(WM_MESSAGE * pMsg)
{
        switch (pMsg->MsgId) {
        case WM_TOUCH:
        {
                USERDATA_WIDGET_QRCODE UserDataQRCode;
                GUI_PID_STATE *pState = (GUI_PID_STATE *)pMsg->Data.p;

                _CustomWidgetGetUserData(pMsg->hWin, &UserDataQRCode, sizeof(UserDataQRCode));
                UserDataQRCode.isQRCodeMoved = 1;
                _CustomWidgetSetUserData(pMsg->hWin, &UserDataQRCode, sizeof(UserDataQRCode));

#if USE_TOUCH_OPERATIONS
                WM_RestartTimer(UserDataQRCode.hTimerDisplay, 0);
#endif
                if (pState->Pressed) {
                        WM_SetUntouchable(UserDataQRCode.hSliderAnimSpeed, 1);
                }
                else {
                        WM_SetUntouchable(UserDataQRCode.hSliderAnimSpeed, 0);
                }
                CustomWidgetQR_Callback(pMsg); // Call the default widget's callback
                break;
        }
        default:
                CustomWidgetQR_Callback(pMsg);
                break;
        }
}

/***************************************************************************************
 *
 * User-defined slider skinning callback function
 */
static int _DrawSkinFlex_SLIDER(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo)
{
        U8 ThumbRadius = 14;
        SLIDER_SKINFLEX_PROPS SkinFlexProp;
        static const GUI_COLOR d_blue = GUI_MAKE_COLOR(0xCDAC00);
        int x0, x1, h, h1, v, min, max;

        switch (pDrawItemInfo->Cmd) {
        case WIDGET_ITEM_DRAW_THUMB:
                GUI_EnableAlpha(1);
                GUI_SetColor(d_blue | GUI_MAKE_TRANS(0x80) << 24);
                GUI_FillCircle(pDrawItemInfo->x1 >> 1, pDrawItemInfo->y0 + ThumbRadius, ThumbRadius);
                GUI_EnableAlpha(0);
                GUI_SetColor(d_blue);
                GUI_FillCircle(pDrawItemInfo->x1 >> 1, pDrawItemInfo->y0 + ThumbRadius, ThumbRadius >> 1);
                break;
        case WIDGET_ITEM_DRAW_SHAFT:
                SLIDER_GetSkinFlexProps(&SkinFlexProp, SLIDER_SKINFLEX_PI_UNPRESSED);
                v = SLIDER_GetValue(pDrawItemInfo->hWin);
                SLIDER_GetRange(pDrawItemInfo->hWin, &min, &max);

                h  = pDrawItemInfo->x1 - pDrawItemInfo->x0 + 1 - SkinFlexProp.TickSize - 1;
                x0 = pDrawItemInfo->x0 + (h - SkinFlexProp.ShaftSize) / 2;
                x1 = x0 + SkinFlexProp.ShaftSize - 1;
                h1 = ((pDrawItemInfo->y1 - 1 - ThumbRadius - (pDrawItemInfo->y0 + 1 + ThumbRadius)) * v) / (max - min);

                GUI_SetColor(SkinFlexProp.aColorShaft[0]);
                GUI_DrawHLine(pDrawItemInfo->y0 + ThumbRadius, x0, x1);
                GUI_DrawVLine(x0, pDrawItemInfo->y0 + 1 + ThumbRadius, pDrawItemInfo->y1 - ThumbRadius);

                GUI_SetColor(SkinFlexProp.aColorShaft[1]);
                GUI_DrawHLine(pDrawItemInfo->y1 - ThumbRadius, x0 + 1, x1 - 1);
                GUI_DrawVLine(x1, pDrawItemInfo->y0 + 1 + ThumbRadius, pDrawItemInfo->y1 - ThumbRadius);

                GUI_SetColor(SkinFlexProp.aColorShaft[2]);
                GUI_FillRect(x0 + 1, pDrawItemInfo->y0 + 1 + ThumbRadius, x1 - 1, pDrawItemInfo->y0 + 1 + ThumbRadius + h1);
                GUI_SetColor(d_blue);
                GUI_FillRect(x0 + 1, pDrawItemInfo->y0 + 1 + ThumbRadius + h1, x1 - 1, pDrawItemInfo->y1 - 1 - ThumbRadius);
                break;
        default:
                return SLIDER_DrawSkinFlex(pDrawItemInfo);
                break;
        }
        return 0;
}

/***************************************************************************************
 *
 * Customize slider skin
 */
static void _SetSkin_SLIDER(SLIDER_Handle hObj)
{
        SLIDER_SetSkin(hObj, _DrawSkinFlex_SLIDER);
}

/*****************************************************************************************
 *
 * Slider widget flex skinning
 */
static void _SetSkinFlexProp_SLIDER(GUI_COLOR Color)
{
        SLIDER_SKINFLEX_PROPS SliderSkinPropUnpressed, SliderSkinPropPressed;

        SLIDER_GetSkinFlexProps(&SliderSkinPropPressed, SLIDER_SKINFLEX_PI_PRESSED);
        SliderSkinPropPressed.TickSize = 0;
        SliderSkinPropPressed.ShaftSize = 5;
        SliderSkinPropPressed.aColorShaft[2] = Color;
        SLIDER_SetSkinFlexProps(&SliderSkinPropPressed, SLIDER_SKINFLEX_PI_PRESSED);
        SLIDER_GetSkinFlexProps(&SliderSkinPropUnpressed, SLIDER_SKINFLEX_PI_UNPRESSED);
        SliderSkinPropUnpressed.TickSize = 0;
        SliderSkinPropUnpressed.ShaftSize = 5;
        SliderSkinPropUnpressed.aColorShaft[2] = Color;
        SLIDER_SetSkinFlexProps(&SliderSkinPropUnpressed, SLIDER_SKINFLEX_PI_UNPRESSED);
}

/****************************************************************************************
 *
 * Button widget flex skinning
 */
static void _SetSkinFlexProp_BUTTON(void)
{
        BUTTON_SKINFLEX_PROPS ButtonSkinPropPressed, ButtonSkinPropEnabled;

        BUTTON_GetSkinFlexProps(&ButtonSkinPropPressed, BUTTON_SKINFLEX_PI_PRESSED);
        ButtonSkinPropPressed.aColorFrame[0] = GUI_WHITE;
        ButtonSkinPropPressed.aColorFrame[1] = GUI_WHITE;
        ButtonSkinPropPressed.aColorFrame[2] = GUI_WHITE;
        ButtonSkinPropPressed.Radius = 20; // Make the button frame invisible by setting the radius of the rounded corner < 45
        BUTTON_SetSkinFlexProps(&ButtonSkinPropPressed, BUTTON_SKINFLEX_PI_PRESSED);

        BUTTON_GetSkinFlexProps(&ButtonSkinPropEnabled, BUTTON_SKINFLEX_PI_ENABLED);
        ButtonSkinPropEnabled.aColorFrame[0] = GUI_WHITE;
        ButtonSkinPropEnabled.aColorFrame[1] = GUI_WHITE;
        ButtonSkinPropEnabled.aColorFrame[2] = GUI_WHITE;
        ButtonSkinPropEnabled.Radius = 20; // Make the button frame invisible by setting the radius of the rounded corner < 45
        BUTTON_SetSkinFlexProps(&ButtonSkinPropEnabled, BUTTON_SKINFLEX_PI_ENABLED);
}

/*******************************************************************************************
 * Create all widgets required by the target Anim.
 */
static void _CreateWidgets(WM_HWIN hWinParent, int xSize, int ySize, APP_WIDGET_DATA * pData)
{
        int PositionOffset, SliderMinRange, SliderMaxRange;
        U8 ThumbDiameter;
        USERDATA_WIDGET_IMAGE UserDataDialogLogo;
        USERDATA_WIDGET_QRCODE UserDataQRCode;

        PositionOffset = 10;
        SliderMinRange = 0;
        SliderMaxRange = 2000;
        ThumbDiameter = 30;

        //
        // WIDGET Image Creation (widget is not visible at this phase)
        //
        pData->hDialogLogo = IMAGE_CreateUser(pData->DialogLogoPosX, pData->DialogLogoPosY, BM_DIASEMI_0.XSize,
                                                                                            BM_DIASEMI_0.YSize,
                                                                                            hWinParent,
                                                                                            WM_CF_SHOW | WM_CF_MOTION_X | WM_CF_MOTION_Y,
                                                                                            IMAGE_CF_AUTOSIZE, GUI_ID_IMAGE0,
                                                                                            sizeof(USERDATA_WIDGET_IMAGE)); // Allocate extra data for the widget

        // Disable touch functionality. PID events will be rooted to its parent window.
        WM_SetUntouchable(pData->hDialogLogo, 1);


        //
        // WIDGET QR Creation (widget is not visible at this phase)
        //
        pData->hQR = _CustomWidgetQRCreate(pData->QRCodePosX, pData->QRCodePosY, hWinParent, WM_CF_HIDE | WM_CF_MOTION_X | WM_CF_MOTION_Y,
                                                                                             NULL, QR_CODE_TEXT_VALUE,
                                                                                             sizeof(USERDATA_WIDGET_QRCODE), // Allocate extra data for the widget
                                                                                             GUI_ID_WIDGET_QR);

        // Disable touch functionality. PID events will be rooted to its parent window.
        WM_SetUntouchable(pData->hQR, 1);

        pData->QRCodeSizeX = WM_GetWindowSizeX(pData->hQR);
        pData->QRCodeSizeY = WM_GetWindowSizeY(pData->hQR);

        //
        // WIDGET Buttons Creation
        //
        pData->ButtonPlayPauseSizeX = BM_BUTTON_PLAY.XSize;
        pData->ButtonPlayPauseSizeY = BM_BUTTON_PLAY.YSize;
        pData->ButtonPlayPausePosX = PositionOffset;
        pData->ButtonPlayPausePosY = (ySize - pData->ButtonPlayPauseSizeY - PositionOffset);

        pData->hButtonPlayPause = BUTTON_CreateEx(pData->ButtonPlayPausePosX, pData->ButtonPlayPausePosY, pData->ButtonPlayPauseSizeX,
                                                                                                          pData->ButtonPlayPauseSizeY,
                                                                                                          hWinParent,
                                                                                                          WM_CF_SHOW, 0, GUI_ID_BUTTON0);

        pData->ButtonResetSizeX = BM_BUTTON_RESET.XSize;
        pData->ButtonResetSizeY = BM_BUTTON_RESET.YSize;
        pData->ButtonResetPosX = (xSize - pData->ButtonResetSizeX - PositionOffset);
        pData->ButtonResetPosY = (ySize - pData->ButtonResetSizeY - PositionOffset);

        pData->hButtonReset = BUTTON_CreateEx(pData->ButtonResetPosX, pData->ButtonResetPosY, pData->ButtonResetSizeX,
                                                                                              pData->ButtonResetSizeY,
                                                                                              hWinParent,
                                                                                              WM_CF_SHOW, 0, GUI_ID_BUTTON1);

#if !USE_TOUCH_OPERATIONS
        WM_HideWindow(pData->hButtonPlayPause);
        WM_HideWindow(pData->hButtonReset);
#endif
        //
        // WIDGET Button Properties
        //
        BUTTON_SetReactOnLevel();
        BUTTON_SetFocusable(pData->hButtonPlayPause, 0);
        BUTTON_SetFocusable(pData->hButtonReset, 0);
        BUTTON_SetBitmapEx(pData->hButtonPlayPause, BUTTON_BI_UNPRESSED, &BM_BUTTON_PAUSE, 0, 0);
        BUTTON_SetBitmapEx(pData->hButtonReset, BUTTON_BI_UNPRESSED, &BM_BUTTON_RESET, 0, 0);

        _SetSkinFlexProp_BUTTON();

        //
        // WIDGET Slider Creation
        //
        pData->SliderAnimSpeedSizeX = ThumbDiameter + 2;
        pData->SliderAnimSpeedSizeY = (ySize >> 1); // The size encompasses the shaft and thumb height
        pData->SliderAnimSpeedPosX = (xSize - pData->SliderAnimSpeedSizeX - (PositionOffset << 1));
        pData->SliderAnimSpeedPosY = ((ySize - pData->SliderAnimSpeedSizeY) >> 1);
        pData->hSliderAnimSpeed = SLIDER_CreateEx(pData->SliderAnimSpeedPosX, pData->SliderAnimSpeedPosY, pData->SliderAnimSpeedSizeX,
                                                                                                          pData->SliderAnimSpeedSizeY,
                                                                                                          hWinParent, WM_CF_HIDE,
                                                                                                          SLIDER_CF_VERTICAL, GUI_ID_SLIDER0);

        //
        // WIDGET Slider Properties
        //
        SLIDER_EnableFocusRect(pData->hSliderAnimSpeed, 0);
        SLIDER_SetNumTicks(pData->hSliderAnimSpeed, pData->SliderAnimSpeedSizeX >> 2);
        SLIDER_SetRange(pData->hSliderAnimSpeed, SliderMinRange, SliderMaxRange);
        SLIDER_SetValue(pData->hSliderAnimSpeed, 0);
        SLIDER_SetWidth(pData->hSliderAnimSpeed, ThumbDiameter + 1); // Set the thumb width

        _SetSkinFlexProp_SLIDER(pData->FrameBkColor);
        _SetSkin_SLIDER(pData->hSliderAnimSpeed);

        //
        // WIDGET Image User Data
        //
        IMAGE_GetUserData(pData->hDialogLogo, &UserDataDialogLogo, sizeof(USERDATA_WIDGET_IMAGE));
        UserDataDialogLogo.isImageMoved = 0;
        UserDataDialogLogo.hSliderAnimSpeed = pData->hSliderAnimSpeed;
#if USE_TOUCH_OPERATIONS
        UserDataDialogLogo.hTimerDisplay = pData->hTimerDisplay;
#endif
        IMAGE_SetUserData(pData->hDialogLogo, &UserDataDialogLogo, sizeof(USERDATA_WIDGET_IMAGE));

        //
        // WIDGET QR User Data
        //
        _CustomWidgetGetUserData(pData->hQR, &UserDataQRCode, sizeof(USERDATA_WIDGET_QRCODE));
        UserDataQRCode.isQRCodeMoved = 0;
        UserDataQRCode.hSliderAnimSpeed = pData->hSliderAnimSpeed;
#if USE_TOUCH_OPERATIONS
        UserDataQRCode.hTimerDisplay = pData->hTimerDisplay;
#endif
        _CustomWidgetSetUserData(pData->hQR, &UserDataQRCode, sizeof(USERDATA_WIDGET_QRCODE));
}

/*************************************************************************************
 *
 * Calculate the current page while swiping
 */
static int _AdvancedMotionFindPages(int NumPages, int DisplaySize, int MotionPos)
{
        if (MotionPos < 0) {
                return WINDOW_PAGE_OVERLAP_MIN;
        } else if (MotionPos == 0) {
                return WINDOW_PAGE_DESKTOP;
        } else if (MotionPos > (DisplaySize * NumPages)) {
                return WINDOW_PAGE_OVERLAP_MAX;
        } else {
                int min, max, page;
                for (page = 0; page < NumPages; page++) {
                        min = ((page * DisplaySize) + 1);
                        max = ((page + 1) * DisplaySize);
                        if ((MotionPos >= min) && (MotionPos <= max)) {
                                break;
                        }
                }
                return ++page; // paging starts from value '1'
        }
}

/*****************************************************************************************************************************************
 *
 * Draw the appropriate content to each page
 */
static void _FillPages(GUI_RECT * PageRect, int Page, char * buf, GUI_PID_STATE * pPID, GUI_FONT * pFont, GUI_COLOR ColorText)
{
        switch(Page) {
        case 0:
                if (pFont) { // Check if XBF data are valid
                        GUI_SetFont(pFont);
                }
                else {
                        GUI_SetFont(&GUI_Font20B_ASCII);
                }

                GUI_DrawGradientRoundedV(PageRect->x0, PageRect->y0, PageRect->x1, PageRect->y1, LOG_PAGE_GRADIENT_CORNER_RADIUS,
                                                                                                     LOG_PAGE_GRADIENT_COLOR_MAX,
                                                                                                     LOG_PAGE_GRADIENT_COLOR_MIN);

                GUI_SetColor(ColorText);
                GUI_SetBkColor(LOG_PAGE_GRADIENT_COLOR_MAX + 0x8000);
                GUI_DispStringInRect((pFont != NULL) ? LOG_PROJECT_NAME_UNICODE : LOG_PROJECT_NAME, PageRect, GUI_TA_VCENTER | GUI_TA_HCENTER);
                break;
        case 1:
                GUI_DrawGradientRoundedV(PageRect->x0, PageRect->y0, PageRect->x1, PageRect->y1, LOG_PAGE_GRADIENT_CORNER_RADIUS,
                                                                                                     LOG_PAGE_GRADIENT_COLOR_MAX,
                                                                                                     LOG_PAGE_GRADIENT_COLOR_MIN);

                GUI_SetColor(ColorText);
                GUI_SetFont(&GUI_Font16B_ASCII);
                GUI_SetBkColor(LOG_PAGE_GRADIENT_COLOR_MAX);
                GUI_DispStringInRect("\n\n\rDISPLAY/TOUCH INFO\n\n\n\r", PageRect, GUI_TA_HCENTER);
                GUI_SetColor(LOG_TEXT_COLOR);
                GUI_SetFont(GUI_GetDefaultFont());
                GUI_SetBkColor(LOG_PAGE_GRADIENT_COLOR_MAX + 0x3000);
                GUI_DispStringInRect(LOG_DISPLAY_MODEL_INFO(Adafruit TFT Shield, ILI9341, 240, 320, 48 MHz, MIPI-DBI (SPI4))LOG_TOUCH_DRIVER_INFO(FocalTech FT6206, I2C, 400 kHz, Capacitive), PageRect, GUI_TA_VERTICAL);
                GUI_SetBkColor(LOG_PAGE_GRADIENT_COLOR_MAX + 0xA000);
                sprintf(buf, LOG_TOUCH_COORDINATES, pPID->x, pPID->y, (pPID->Pressed) ? "Pressed" : "Unpressed");
                GUI_DispString(buf);
                break;
        default:
                break;
        }
}

/*****************************************************************************************************************************************
 *
 * Draw the appropriate pages while swiping
 */
static void _AdvancedMotionDrawPages(int NumPages, int xSize, int ySize, int MotionPos, GUI_PID_STATE * pPID, GUI_FONT * pFont, GUI_COLOR ColorText)
{
        GUI_RECT Rect;
        int Rect_Y0, Rect_Y1;
        char sBuffer[xSize];

        for (int i = 0; i < NumPages; i++) {
                Rect_Y0 = ((((i + 1) * ySize) * (-1)) + MotionPos - 1);
                Rect_Y1 = (((i * ySize) * (-1)) + MotionPos - 1);
                Rect.x0 = 0; Rect.x1 = xSize; Rect.y0 = Rect_Y0; Rect.y1 = Rect_Y1;

                // Draw the actual contents of a page
                _FillPages(&Rect,i, sBuffer, pPID, pFont, ColorText);
        }
}

/**********************************************************************************
 *
 * Move the necessary objects/widgets while swiping
 */
static void _AdvancedMotionMoveObjects(APP_WIDGET_DATA * pData, int Offset)
{
        WM_SetWindowPos(pData->hSliderAnimSpeed, pData->SliderAnimSpeedPosX, pData->SliderAnimSpeedPosY + Offset, pData->SliderAnimSpeedSizeX, pData->SliderAnimSpeedSizeY);
        WM_SetWindowPos(pData->hButtonPlayPause, pData->ButtonPlayPausePosX, pData->ButtonPlayPausePosY + Offset, pData->ButtonPlayPauseSizeX, pData->ButtonPlayPauseSizeY);
        WM_SetWindowPos(pData->hButtonReset, pData->ButtonResetPosX, pData->ButtonResetPosY + Offset, pData->ButtonResetSizeX, pData->ButtonResetSizeY);
}

/*********************************************************************
 *
 *       _cbWin
 */
static void _cbWin(WM_MESSAGE * pMsg)
{
        //
        // Static variables
        //
        static GUI_ANIM_HANDLE hAnim;
        static ANIM_DATA AnimData;
        static ANIM_DATA aAnimDefaults;
        static APP_WIDGET_DATA WidgetData;
#if USE_TOUCH_OPERATIONS
        static bool isTimerDisplayExpired;
#endif
        static bool isTimerTextColorReload;
        static int AdvancedMotionPosY;
        static int AdvancedMotionPage;
        static int AdvandedMotionOverlap;
        static GUI_PID_STATE TouchData;
        static GUI_FONT * pFontXBF;
#if USE_XBF_FONT
        static GUI_XBF_DATA * pDataXBF;
        static USERDATA_EXTERNAL_RESOURCES UserDataXBF;
#endif
#if USE_BITMAP_DATASTREAM
        static USERDATA_EXTERNAL_RESOURCES UserDataBitmapDataStream;
#endif
        static GUI_COLOR ColorText;
        static GUI_COLOR ColorTextIdx;

        //
        // Non-static variables
        //
        int xSize, ySize;
        WM_HWIN hWin;

        hWin = pMsg->hWin;
        xSize = WM_GetWindowSizeX(pMsg->hWin);
        ySize = WM_GetWindowSizeY(pMsg->hWin);

        switch (pMsg->MsgId) {
        case WM_CREATE:
        {
                GUI_QR_INFO QRInfo;
                GUI_HMEM ahQR;
#if USE_XBF_FONT
                // Prepare data required to retrieve the font information from the flash partition
                UserDataXBF.Offset = PARTITION_OFFSET_XBF_FONT;
                UserDataXBF.PartitionID = PARTITION_ID_XBF_FONT;

                // Retrieve the custom font stored in FLASH
                _XBFCreateFont(&pFontXBF, &pDataXBF, XBF_FONT_TYPE, &UserDataXBF);
#endif

#if USE_TOUCH_OPERATIONS
                // Initialize a WM timer used to turn off the display
                WidgetData.hTimerDisplay = WM_CreateTimer(hWin, TIMER_ID_DISPLAY_OFF, TIMER_TM_MS_DISPLAY_OFF, 0);
#endif
                // Initialize a WM timer used to change the text color
                WM_CreateTimer(hWin, TIMER_ID_TEXT_COLOR, TIMER_TM_MS_TEXT_COLOR, 0);
                isTimerTextColorReload = 1;

                // Create App. widgets
                WidgetData.FrameBkColor = GUI_WHITE;
                _CreateWidgets(hWin, xSize, ySize, &WidgetData);

                // Get info on the QR Code
                ahQR = _CustomWidgetGetQRHandle(WidgetData.hQR);
                GUI_QR_GetInfo(ahQR, &QRInfo);

                // Set default slider's value
                aAnimDefaults.SliderValue = SLIDER_GetValue(WidgetData.hSliderAnimSpeed);
                aAnimDefaults.hWin = hWin;
                aAnimDefaults.state = APP_ANIM_DIASEMI_INIT;

                //
                // Set up animation for Dialog Semiconductors
                //
                // Anim. 0
                aAnimDefaults.Data[0].xPos  = (xSize - BM_DIASEMI_0.XSize - 11) / 2;
                aAnimDefaults.Data[0].yPos  = (ySize - BM_DIASEMI_0.YSize) / 2;
                aAnimDefaults.Data[0].xDiff = xSize;
                aAnimDefaults.Data[0].yDiff = 0;
                aAnimDefaults.Data[0].xDiffPrev = xSize;
                aAnimDefaults.Data[0].yDiffPrev = 0;
                aAnimDefaults.Data[0].xSize = xSize;
                aAnimDefaults.Data[0].ySize = ySize;
                //
                // Anim. 1, used for moving left and right. THis one gets used by both DiaSemi and QR
                //
                aAnimDefaults.Data[1].xPos  = 0;
                aAnimDefaults.Data[1].yPos  = 0;
                aAnimDefaults.Data[1].xDiff = 0;
                aAnimDefaults.Data[1].yDiff = 0;
                aAnimDefaults.Data[1].xDiffPrev = 0;
                aAnimDefaults.Data[1].yDiffPrev = 0;
                aAnimDefaults.Data[1].xSize = xSize;
                aAnimDefaults.Data[1].ySize = ySize;
                //
                // Set up animation for QR
                //
                aAnimDefaults.Data[2].xPos  = (xSize - (QRInfo.Size + QR_WIDGET_WHITE_FRAME_SIZE)) / 2;
                aAnimDefaults.Data[2].yPos  = (ySize - (QRInfo.Size + QR_WIDGET_WHITE_FRAME_SIZE)) / 2;
                aAnimDefaults.Data[2].xDiff = 0;
                aAnimDefaults.Data[2].yDiff = ySize;
                aAnimDefaults.Data[2].xDiffPrev = 0;
                aAnimDefaults.Data[2].yDiffPrev = ySize;
                aAnimDefaults.Data[2].xSize = xSize;
                aAnimDefaults.Data[2].ySize = ySize;

                // Default locations for moveable objects (widgets)
                WidgetData.DialogLogoPosX = aAnimDefaults.Data[0].xPos;
                WidgetData.DialogLogoPosY = aAnimDefaults.Data[0].yPos;
                WidgetData.QRCodePosX = aAnimDefaults.Data[2].xPos;
                WidgetData.QRCodePosY = aAnimDefaults.Data[2].yPos;

                //
                // Copy default values into working structure
                //
                _RestoreData(&AnimData, &aAnimDefaults);

                //
                // Create animation
                //
                _AnimateCreate(&hAnim, &AnimData);
                break; /* WM_CREATE */
        }
        case WM_NOTIFY_PARENT:
                // Get the widget ID that sent the notification
                switch (WM_GetId(pMsg->hWinSrc)) {
                case GUI_ID_BUTTON0:
                        switch (pMsg->Data.v) { // Get message ID
                        case WM_NOTIFICATION_RELEASED:
                                WidgetData.AnimState ^= 1; // Toggle button's state
                                if (WidgetData.AnimState == ANIM_STATE_PAUSE) {
                                        // Update widget's bitmap to PAUSE state
                                        BUTTON_SetBitmap(WidgetData.hButtonPlayPause, BUTTON_BI_UNPRESSED, &BM_BUTTON_PLAY);

                                        // Slider widget should become visible
                                        WM_ShowWindow(WidgetData.hSliderAnimSpeed);

                                        // Stop animation
                                        GUI_ANIM_Stop(hAnim);

                                        // Enable touch functionality for moveable objects
                                        WM_SetUntouchable(WidgetData.hDialogLogo, 0);
                                        WM_SetUntouchable(WidgetData.hQR, 0);

                                        // Overwrite window callback (window will be redrawn)
                                        WM_SetCallback(WidgetData.hDialogLogo, _cbIMAGE);
                                        WM_SetCallback(WidgetData.hQR, _cbQR);
                                } else if (WidgetData.AnimState == ANIM_STATE_PLAY) {
                                        // Update widget's bitmap to PLAY state
                                        BUTTON_SetBitmap(WidgetData.hButtonPlayPause, BUTTON_BI_UNPRESSED, &BM_BUTTON_PAUSE);

                                        // Slider widget should not not be visible at this stage
                                        WM_HideWindow(WidgetData.hSliderAnimSpeed);

                                        // Disable touch functionality for moveable objects. Touch events are rooted to its parent window.
                                        WM_SetUntouchable(WidgetData.hDialogLogo, 1);
                                        WM_SetUntouchable(WidgetData.hQR, 1);

                                        // Restore window callback to default. (window will be redrawn)
                                        WM_SetCallback(WidgetData.hDialogLogo, IMAGE_Callback);
                                        WM_SetCallback(WidgetData.hQR, CustomWidgetQR_Callback);

                                        // Re-initiate animation
                                        WM_SendMessageNoPara(hWin, MSG_ANIM_END);
                                }
                                break;
                        default:
                                break;
                        }
                        break; /* GUI_ID_BUTTON0 */
                case GUI_ID_BUTTON1:
                        switch (pMsg->Data.v) {
                        case WM_NOTIFICATION_RELEASED:
                                // Relocate objects to its original position only if Anim. is paused.
                                if (WidgetData.AnimState == ANIM_STATE_PAUSE) {
                                        WidgetData.DialogLogoPosX = aAnimDefaults.Data[0].xPos;
                                        WidgetData.DialogLogoPosY = aAnimDefaults.Data[0].yPos;
                                        WidgetData.QRCodePosX = aAnimDefaults.Data[2].xPos;
                                        WidgetData.QRCodePosY = aAnimDefaults.Data[2].yPos;

                                        if ((AnimData.state == APP_ANIM_DIASEMI_IN) || (AnimData.state == APP_ANIM_DIASEMI_OUT)) {
                                                WM_SetWindowPos(WidgetData.hDialogLogo, WidgetData.DialogLogoPosX, WidgetData.DialogLogoPosY,
                                                                                                                          BM_DIASEMI_0.XSize,
                                                                                                                          BM_DIASEMI_0.YSize);

                                        }
                                        if ((AnimData.state == APP_ANIM_QR_IN) || (AnimData.state == APP_ANIM_QR_OUT)) {
                                                WM_SetWindowPos(WidgetData.hQR, WidgetData.QRCodePosX, WidgetData.QRCodePosY, WidgetData.QRCodeSizeX,
                                                                                                                              WidgetData.QRCodeSizeY);
                                        }
                                }
                                break;
                        default:
                                break;
                        }
                        break; /* GUI_ID_BUTTON1 */
                case GUI_ID_SLIDER0:
                        switch (pMsg->Data.v) {
                        case WM_NOTIFICATION_VALUE_CHANGED:
                                // Update slider's flag
                                WidgetData.isSliderAnimSpeedValueChanged = 1;
                                WidgetData.SliderValue = SLIDER_GetValue(WidgetData.hSliderAnimSpeed);
#if USE_TOUCH_OPERATIONS
                                WM_RestartTimer(WidgetData.hTimerDisplay, 0);
#endif

                                // Invalidate window so slider value is updated!!!
                                WM_InvalidateWindow(hWin);
                                break;
                        default:
                                break;
                        }
                        break; /*GUI_ID_SLIDER0  */
                case GUI_ID_WIDGET_QR:
                        switch (pMsg->Data.v) {
                        case QR_WIDGET_NOTIFICATION_CLICKED:
                                // Add your own functionality
                                break;
                        case QR_WIDGET_NOTIFICATION_UNPRESSED:
                                // Add your own functionality
                                break;
                        case QR_WIDGET_NOTIFICATION_MOVED_OUT:
                                // Add your own functionality
                                break;
                        default:
                                break;
                        }
                        break; /* WIDGET_ID_CUSTOM0 */
                default:
                        break;
                }
                break; /* WM_NOTIFY_PARENT */
        case WM_PID_STATE_CHANGED:
        {
                WM_PID_STATE_CHANGED_INFO * pState = (WM_PID_STATE_CHANGED_INFO *)pMsg->Data.p;
                (void)pState;
#if USE_TOUCH_OPERATIONS
                // Check if display is disabled and if so, enable it.
                if (isTimerDisplayExpired) {
                        if (pState) {
                                // React on release
                                if(pState->State == 0) {
                                        isTimerDisplayExpired = 0;
                                        isTimerTextColorReload = 1;

                                        // First make static/moveable object touchable (if disabled)
                                        WM_SetUntouchable(WidgetData.hSliderAnimSpeed, 0);
                                        WM_SetUntouchable(WidgetData.hButtonPlayPause, 0);
                                        WM_SetUntouchable(WidgetData.hButtonReset, 0);
                                        WM_SetUntouchable(WidgetData.hQR, 0);
                                        WM_SetUntouchable(WidgetData.hDialogLogo, 0);

                                        if (WidgetData.AnimState == ANIM_STATE_PLAY) {
                                                WM_SendMessageNoPara(hWin, MSG_ANIM_END);
                                        }
                                        LCD_On();
                                        WM_RestartTimer(WidgetData.hTimerDisplay, 0);
                                }
                        }
                }
#endif /* USE_TOUCH_OPERATIONS */
                break; /* WM_PID_STATE_CHANGED */
        }
        case WM_TOUCH:
        {
                GUI_PID_STATE * pTouchInfo = (GUI_PID_STATE *)pMsg->Data.p;
                TouchData.x = pTouchInfo->x;
                TouchData.y = pTouchInfo->y;
                TouchData.Pressed = pTouchInfo->Pressed;

#if USE_TOUCH_OPERATIONS
                // Restart timer as objects are being moved.
                WM_RestartTimer(WidgetData.hTimerDisplay, 0);
#endif
                // Invalidate window so that, log messages are being updated
                WM_InvalidateWindow(hWin);
                break; /* WM_TOUCH */
        }
        case WM_TIMER:
                switch(WM_GetTimerId(pMsg->Data.v)) {
#if USE_TOUCH_OPERATIONS
                case TIMER_ID_DISPLAY_OFF:
                        // Make static object untouchable so, the whole display area can be used for re-activating the display.
                        WM_SetUntouchable(WidgetData.hSliderAnimSpeed, 1);
                        WM_SetUntouchable(WidgetData.hButtonPlayPause, 1);
                        WM_SetUntouchable(WidgetData.hButtonReset, 1);
                        WM_SetUntouchable(WidgetData.hQR, 1);
                        WM_SetUntouchable(WidgetData.hDialogLogo, 1);

                        LCD_Off();
                        if (WidgetData.AnimState == ANIM_STATE_PLAY) {
                                GUI_ANIM_Stop(hAnim);
                        }
                        GUI_WaitEvent();

                        isTimerDisplayExpired = 1;
                        isTimerTextColorReload = 0;
                        break;
#endif /* USE_TOUCH_OPERATIONS */
                case TIMER_ID_TEXT_COLOR:
                        switch(ColorTextIdx++) {
                        case 0:
                                ColorText = GUI_GREEN;
                                break;
                        case 1:
                                ColorText = GUI_MAKE_COLOR(GUI_GREEN + 50);
                                break;
                        case 2:
                                ColorText = GUI_MAKE_COLOR(GUI_GREEN + 100);
                                break;
                        case 3:
                                ColorText = GUI_MAKE_COLOR(GUI_GREEN + 150);
                                break;
                        case 4:
                                ColorText = GUI_MAKE_COLOR(GUI_GREEN + 200);
                                break;
                        case 5:
                                ColorText = GUI_MAKE_COLOR(GUI_LIGHTGREEN - 100);
                                break;
                        case 6:
                                ColorText = GUI_MAKE_COLOR(GUI_LIGHTGREEN - 50);
                                break;
                        case 7:
                                ColorText = GUI_LIGHTGREEN;
                                break;
                        case 8:
                                ColorText = GUI_MAKE_COLOR(GUI_LIGHTGREEN + 50);
                                break;
                        case 9:
                                ColorText = GUI_MAKE_COLOR(GUI_LIGHTGREEN + 100);
                                ColorTextIdx = 0;
                                break;
                        default:
                                break;
                        }

                        if (isTimerTextColorReload) WM_RestartTimer(pMsg->Data.v, 0);
                        WM_InvalidateWindow(hWin);
                        break;
                default:
                        break;
                }
                break; /* WM_TIMER */
        case WM_MOTION:
        {
                int AdvancedMotionOverlap, yAdvancedMotionMin, yAdvancedMotionMax;
                WM_MOTION_INFO * pMotionInfo = (WM_MOTION_INFO *)pMsg->Data.p;

                switch (pMotionInfo->Cmd) {
                case WM_MOTION_INIT:
                        // Enable custom motion support if not already enabled
                        pMotionInfo->Flags = WM_MOTION_MANAGE_BY_WINDOW | WM_CF_MOTION_Y;
                        // Deceleration of Y-axis movement will stop exactly on Y-axis window/object boundaries.
                        pMotionInfo->SnapY = ySize;
                        // Define a short distance a window/object can be moved beyond its boundaries.
                        AdvandedMotionOverlap = ySize >> 3;
                        pMotionInfo->Overlap = AdvandedMotionOverlap;
                        // Define a period in which a window/object continues moving after PID event release.
                        pMotionInfo->Period = MOTION_DECELERATION_PERIOD;
                        break;
                case WM_MOTION_MOVE:
                        // Support custom motion support only when Anim. is playing. Note that WM_MOTION_SetMoveable() does not have any effect when custom motion is enabled
                        if (WidgetData.AnimState == ANIM_STATE_PLAY) {
                                // Distance increments (+) when swiping down and decrements when swiping up (-)
                                AdvancedMotionPosY += pMotionInfo->dy;
                                // Check if motion is in progress and designate when movement is beyond the expected boundaries.
                                if (pMotionInfo->IsDragging) {
                                        pMotionInfo->IsOutside = ((AdvancedMotionPosY < 0) || (AdvancedMotionPosY > (ySize * MOTION_NUM_OF_PAGES))) ? 1 : 0;
                                        AdvancedMotionOverlap = pMotionInfo->Overlap;
                                } else {
                                        AdvancedMotionOverlap = pMotionInfo->Overlap * pMotionInfo->IsOutside;
                                }
                                // Check if motion operations exceed the expected boundaries.
                                yAdvancedMotionMin = ((-1) * AdvancedMotionOverlap);
                                yAdvancedMotionMax = ((ySize * MOTION_NUM_OF_PAGES) + AdvancedMotionOverlap);
                                if (AdvancedMotionPosY < yAdvancedMotionMin) {
                                        AdvancedMotionPosY = yAdvancedMotionMin;
                                        pMotionInfo->StopMotion = pMotionInfo->IsDragging ^ 1;
                                } else if (AdvancedMotionPosY > yAdvancedMotionMax) {
                                        AdvancedMotionPosY = yAdvancedMotionMax;
                                        pMotionInfo->StopMotion = pMotionInfo->IsDragging ^ 1;
                                }

                                // Identify the current page. Note that, when swiping down 'dy' decrements whereas, when swiping up 'dy' increments
                                AdvancedMotionPage = _AdvancedMotionFindPages(MOTION_NUM_OF_PAGES, ySize, AdvancedMotionPosY);
                                // Update the BK color based on the current page
                                if (AdvancedMotionPage == WINDOW_PAGE_OVERLAP_MAX) {
                                        WidgetData.FrameBkColor = LOG_PAGE_GRADIENT_COLOR_MAX;
                                } else if (AdvancedMotionPage > WINDOW_PAGE_DESKTOP) {
                                        WidgetData.FrameBkColor = GUI_WHITE; // Make sure that desktop's BK color matches page's BK color
                                }
                                WM_InvalidateWindow(hWin); // Invalidate window so window/object is redrawn after a moving operation
                        }
                        break;
                case WM_MOTION_GETPOS:
                        // Return the current Y position
                        pMotionInfo->yPos = AdvancedMotionPosY;
                        break;
                default:
                        break;
                }
                break; /* WM_MOTION */
        }
        case WM_PAINT:
                if (GUI_GetBkColor() != WidgetData.FrameBkColor) {
                        GUI_SetBkColor(WidgetData.FrameBkColor);
                }
                //
                // Clear the display so that, the moveable objects are drawn clearly
                //
                GUI_Clear();
                //
                // Move all the static objects (widgets) as the desktop is moving (swipe operations)
                //
                _AdvancedMotionMoveObjects(&WidgetData, AdvancedMotionPosY);

               /*
                * ----------------------------
                * -                          -
                * -         Page #N          -
                * -                          -
                * ----------------------------
                *                ...
                * ----------------------------
                * -                          -
                * -          Page #1         -
                * -                          -
                * ----------------------------
                * ----------------------------
                * -                          -
                * -         Anim. Window     -
                * -        (Visible Area)    -
                * -                          -
                * ----------------------------
                */
                _AdvancedMotionDrawPages(MOTION_NUM_OF_PAGES, xSize, ySize, AdvancedMotionPosY, &TouchData, pFontXBF, ColorText);

                //
                // Check if slider's value is changed
                //
                if (WidgetData.isSliderAnimSpeedValueChanged) {
                        aAnimDefaults.SliderValue = WidgetData.SliderValue;
                        WidgetData.isSliderAnimSpeedValueChanged = 0;
                }

                switch (AnimData.state) {
                case APP_ANIM_DIASEMI_INIT:
                        //
                        // QR Code widget should not be visible at this stage
                        //
                        if (WM_IsVisible(WidgetData.hQR)) {
                                WM_HideWindow(WidgetData.hQR);
                        }
                        if (AdvancedMotionPage == WINDOW_PAGE_DESKTOP || AdvancedMotionPage == WINDOW_PAGE_OVERLAP_MIN) {
                                WidgetData.FrameBkColor = GUI_WHITE;
                                _SetSkinFlexProp_SLIDER(WidgetData.FrameBkColor);
                        }
                        break;
                case APP_ANIM_DIASEMI_IN:
                        //
                        // Display all the control icons once at the beginning
                        //
                        if (!WidgetData.isDialogLogoDisplayed) {
#if USE_BITMAP_DATASTREAM
                                //
                                // Prepare data required to retrieve bitmap stream data
                                //
                                UserDataBitmapDataStream.Offset = PARTITION_OFFSET_BITMAP_DATASTREAM;
                                UserDataBitmapDataStream.PartitionID = PARTITION_ID_BITMAP_DATASTREAM;

                                IMAGE_SetDTAEx(WidgetData.hDialogLogo, _cbGetBitmapStreamData, &UserDataBitmapDataStream);
#else
                                IMAGE_SetBitmap(WidgetData.hDialogLogo, &BM_DIASEMI_0);
#endif
                                WidgetData.isDialogLogoDisplayed = 1;
                        }
                        //
                        // Do not execute the following code block if moveable objects are being moved.
                        // Otherwise, no motion is observed as objects are returned back to their previous position.
                        //
                        if (WidgetData.AnimState == ANIM_STATE_PLAY) {
                                WM_SetWindowPos(WidgetData.hDialogLogo, AnimData.Data[0].xPos + AnimData.Data[0].xDiff,
                                                AnimData.Data[0].yPos + AnimData.Data[0].yDiff + AdvancedMotionPosY,
                                                BM_DIASEMI_0.XSize, BM_DIASEMI_0.YSize);
                        }
                        //
                        // Draw a rectangular to restore background white color
                        //
                        GUI_SetColor(GUI_WHITE);
                        GUI_FillRect(AnimData.Data[0].xPos + AnimData.Data[0].xDiff + BM_DIASEMI_0.XSize,
                                     AnimData.Data[0].yPos + AnimData.Data[0].yDiff + AdvancedMotionPosY,
                                     AnimData.Data[0].xPos + AnimData.Data[0].xDiffPrev + BM_DIASEMI_0.XSize,
                                     AnimData.Data[0].yPos + AnimData.Data[0].yDiff + BM_DIASEMI_0.YSize + AdvancedMotionPosY);
                        //
                        // When in Anim. page (desktop), store the BK color so that GUI_Clear() draws the correct color
                        //
                        if (AdvancedMotionPage == WINDOW_PAGE_DESKTOP || AdvancedMotionPage == WINDOW_PAGE_OVERLAP_MIN) {
                                WidgetData.FrameBkColor = GUI_WHITE;
                        }
                        break;
                case APP_ANIM_DIASEMI_OUT:
                        //
                        // Do not execute the following code block if moveable objects are being moved.
                        // Otherwise, no motion is observed as objects are returned back to their previous position.
                        //
                        if (WidgetData.AnimState == ANIM_STATE_PLAY) {
                                WM_SetWindowPos(WidgetData.hDialogLogo, AnimData.Data[0].xPos + AnimData.Data[0].xDiff,
                                                AnimData.Data[0].yPos + AnimData.Data[0].yDiff + AdvancedMotionPosY,
                                                BM_DIASEMI_0.XSize, BM_DIASEMI_0.YSize);
                        }
                        //
                        // Draw a rectangular to restore background white color
                        //
                        GUI_SetColor(GUI_WHITE);
                        GUI_FillRect(AnimData.Data[0].xPos + AnimData.Data[0].xDiff + BM_DIASEMI_0.XSize - 1,
                                     AnimData.Data[0].yPos + AnimData.Data[0].yDiff + AdvancedMotionPosY,
                                     AnimData.Data[0].xPos + AnimData.Data[0].xDiffPrev + BM_DIASEMI_0.XSize - 1,
                                     AnimData.Data[0].yPos + AnimData.Data[0].yDiff + BM_DIASEMI_0.YSize + AdvancedMotionPosY);
                        //
                        // Clear background with white and blue
                        //
                        GUI_SetColor(COLOR_BLUE);
                        GUI_FillRect(AnimData.Data[1].xSize + AnimData.Data[1].xDiff,
                                     AnimData.Data[1].yPos + AdvancedMotionPosY,
                                     AnimData.Data[1].xSize,
                                     // Extend the rectangular so that, it covers the overlapped area
                                     AnimData.Data[1].ySize - 1 + AdvandedMotionOverlap + AdvancedMotionPosY);
                        //
                        // When in Anim. page (desktop), store the BK color so that GUI_Clear() draws the correct color
                        //
                        if (AdvancedMotionPage == WINDOW_PAGE_DESKTOP || AdvancedMotionPage == WINDOW_PAGE_OVERLAP_MIN) {
                                if (-AnimData.Data[1].xDiff >= 0 && -AnimData.Data[1].xDiff <= xSize - 1) {
                                        WidgetData.FrameBkColor = GUI_WHITE;
                                } else {
                                        // Make sure the transition between adjacent (different) BK colors is smooth.
                                        WidgetData.FrameBkColor = COLOR_BLUE;
                                }
                        }
                        break;
                case APP_ANIM_QR_IN:
                        //
                        // QR Code widget should become visible
                        //
                        if (!WM_IsVisible(WidgetData.hQR)) {
                                WM_ShowWindow(WidgetData.hQR);
                        }
                        //
                        // Do not execute the following code block if moveable objects are being moved.
                        // Otherwise, no motion is observed as objects are returned back to their previous position.
                        //
                        if (WidgetData.AnimState == ANIM_STATE_PLAY) {
                                WM_SetWindowPos(WidgetData.hQR, AnimData.Data[2].xPos + AnimData.Data[2].xDiff,
                                                AnimData.Data[2].yPos + AnimData.Data[2].yDiff + AdvancedMotionPosY,
                                                WidgetData.QRCodeSizeX, WidgetData.QRCodeSizeY);
                        }
                        //
                        // When in Anim. page (desktop), store the BK color so that GUI_Clear() draws the correct color
                        //
                        if (AdvancedMotionPage == WINDOW_PAGE_DESKTOP || AdvancedMotionPage == WINDOW_PAGE_OVERLAP_MIN) {
                                WidgetData.FrameBkColor = COLOR_BLUE;
                        }
                        break;
                case APP_ANIM_QR_OUT:
                        //
                        // Do not execute the following code block if moveable objects are being moved.
                        // Otherwise, no motion is observed as objects are returned back to their previous position.
                        //
                        if (WidgetData.AnimState == ANIM_STATE_PLAY) {
                                WM_SetWindowPos(WidgetData.hQR, AnimData.Data[2].xPos + AnimData.Data[2].xDiff,
                                                AnimData.Data[2].yPos + AnimData.Data[2].yDiff + AdvancedMotionPosY,
                                                WidgetData.QRCodeSizeX, WidgetData.QRCodeSizeY);
                        }
                        //
                        // Clear background with white and blue
                        //
                        GUI_SetColor(COLOR_BLUE);
                        GUI_FillRect(
                                AnimData.Data[1].xPos + AnimData.Data[1].xSize - 1 + AnimData.Data[1].xDiff,
                                AnimData.Data[1].yPos + AdvancedMotionPosY,
                                AnimData.Data[1].xPos + AnimData.Data[1].xSize - 1,
                                AnimData.Data[1].ySize - 1 + AdvandedMotionOverlap + AdvancedMotionPosY);  // Extend the rectangular so that, it covers the overlapped area.
                        //
                        // When in Anim. page (desktop), store the BK color so that GUI_Clear() draws the correct color
                        //
                        if (AdvancedMotionPage == WINDOW_PAGE_DESKTOP || AdvancedMotionPage == WINDOW_PAGE_OVERLAP_MIN) {
                                WidgetData.FrameBkColor = GUI_WHITE;
                        }
                        break;
                default:
                        break;
                }

                for (int i = 0; i < 3; i++) {
                        AnimData.Data[i].xDiffPrev = AnimData.Data[i].xDiff;
                        AnimData.Data[i].yDiffPrev = AnimData.Data[i].yDiff;
                }
                break; /* WM_PAINT */
        case MSG_ANIM_END:
        {
                USERDATA_WIDGET_QRCODE UserDataQRCode;
                USERDATA_WIDGET_IMAGE UserDataDialogLogo;

                _RestoreData(&AnimData, &aAnimDefaults);

                //
                // If moveable objects have been moved, then retrieve their new positions.
                //
                IMAGE_GetUserData(WidgetData.hDialogLogo, &UserDataDialogLogo, sizeof(USERDATA_WIDGET_IMAGE));
                if (UserDataDialogLogo.isImageMoved) {
                        WidgetData.DialogLogoPosX = WM_GetWindowOrgX(WidgetData.hDialogLogo);
                        WidgetData.DialogLogoPosY = WM_GetWindowOrgY(WidgetData.hDialogLogo);
                        UserDataDialogLogo.isImageMoved = 0;
                }
                IMAGE_SetUserData(WidgetData.hDialogLogo, &UserDataDialogLogo, sizeof(USERDATA_WIDGET_IMAGE));

                _CustomWidgetGetUserData(WidgetData.hQR, &UserDataQRCode, sizeof(USERDATA_WIDGET_QRCODE));
                if (UserDataQRCode.isQRCodeMoved) {
                        WidgetData.QRCodePosX = WM_GetWindowOrgX(WidgetData.hQR);
                        WidgetData.QRCodePosY = WM_GetWindowOrgY(WidgetData.hQR);
                        UserDataQRCode.isQRCodeMoved = 0;
                }
                _CustomWidgetSetUserData(WidgetData.hQR, &UserDataQRCode, sizeof(USERDATA_WIDGET_QRCODE));

                //
                // Store the new default position of the moveable objects
                //
                AnimData.Data[0].xPos = WidgetData.DialogLogoPosX;
                AnimData.Data[0].yPos = WidgetData.DialogLogoPosY;
                AnimData.Data[2].xPos = WidgetData.QRCodePosX;
                AnimData.Data[2].yPos = WidgetData.QRCodePosY;

                _AnimateCreate(&hAnim, &AnimData);
                break; /* MSG_ANIM_END */
        }
        default:
                WM_DefaultProc(pMsg);
                break;
        }
}

/*********************************************************************
 *
 *       Public code
 *
 *********************************************************************
 */
/*********************************************************************
 *
 *       MainTask
 */
void MainTask(void)
{
        int xSize;
        int ySize;

        GUI_Init();
        WM_MOTION_Enable(1);
        WM_MULTIBUF_Enable(1);

        xSize = LCD_GetXSize();
        ySize = LCD_GetYSize();

        WM_CreateWindowAsChild(0, 0, xSize, ySize, WM_HBKWIN, WM_CF_SHOW, _cbWin, 0);

        while (1) {
                GUI_Delay(100);
        }
}
/*************************** End of file ****************************/
