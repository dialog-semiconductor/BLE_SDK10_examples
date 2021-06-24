/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2019  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.50 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Software GmbH
Licensed to:              Dialog Semiconductor BV, Het Zuiderkruis 53, 5215 MV S-Hertogenbosch, The Netherlands
Licensed SEGGER software: emWin
License number:           GUI-00793
License model:            Buyout SRC [Buyout Source Code License], according to Amendment No. 3 [signed June 3rd 2019 by both parties] to emUSB Buyout License Agreement, signed August 6th 2016
Licensed product:         Any
Licensed platform:        DA1469x
Licensed number of seats: -
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2019-06-28 - 2020-06-28
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCD_Protected.h
Purpose     : LCD level - To be used only internally by the GUI
----------------------------------------------------------------------
*/

#ifndef LCD_PROTECTED_H
#define LCD_PROTECTED_H

#include "LCD.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

//#ifndef NULL
//  #define NULL ((void *)0)
//#endif

/*********************************************************************
*
*       Data types
*/
typedef struct {
  LCD_COLOR * paColor;
  I16         NumEntries;
} LCD_LUT_INFO;

typedef struct {
  tLCDDEV_DrawPixel  * pfDrawPixel;
  tLCDDEV_DrawHLine  * pfDrawHLine;
  tLCDDEV_DrawVLine  * pfDrawVLine;
  tLCDDEV_FillRect   * pfFillRect;
  tLCDDEV_DrawBitmap * pfDrawBitmap;
} LCD_API_LIST;

/*********************************************************************
*
*       External data
*/
extern GUI_CONST_STORAGE U8 LCD_aMirror[256];
extern LCD_PIXELINDEX * LCD__aConvTable;

/*********************************************************************
*
*       Misc functions
*/
void LCD_UpdateColorIndices   (void);
int  LCD_PassingBitmapsAllowed(void);
void LCD_EnableCursor         (int OnOff);
void LCD_SelectLCD            (void);

void LCD_DrawBitmap(int x0,    int y0,
                    int xsize, int ysize,
                    int xMul,  int yMul,
                    int BitsPerPixel,
                    int BytesPerLine,
                    const U8 * pPixel,
                    const LCD_PIXELINDEX * pTrans);

void LCD__DrawBitmap_1bpp(int x0,    int y0,
                          int xsize, int ysize,
                          int xMul,  int yMul,
                          int BitsPerPixel,
                          int BytesPerLine,
                          const U8 * pPixel,
                          const LCD_PIXELINDEX * pTrans,
                          int OffData);

/*********************************************************************
*
*       Internal used color conversion routines
*/
tLCDDEV_Index2Color LCD_Index2Color_444_12;
tLCDDEV_Index2Color LCD_Index2Color_M444_12;
tLCDDEV_Index2Color LCD_Index2Color_444_12_1;
tLCDDEV_Index2Color LCD_Index2Color_M444_12_1;
tLCDDEV_Index2Color LCD_Index2Color_444_16;
tLCDDEV_Index2Color LCD_Index2Color_M444_16;
tLCDDEV_Index2Color LCD_Index2Color_555;
tLCDDEV_Index2Color LCD_Index2Color_565;
tLCDDEV_Index2Color LCD_Index2Color_8666;
tLCDDEV_Index2Color LCD_Index2Color_888;
tLCDDEV_Index2Color LCD_Index2Color_8888;
tLCDDEV_Index2Color LCD_Index2Color_M8888I;
tLCDDEV_Index2Color LCD_Index2Color_M555;
tLCDDEV_Index2Color LCD_Index2Color_M565;
tLCDDEV_Index2Color LCD_Index2Color_M888;

tLCDDEV_Color2Index LCD_Color2Index_8666;

#if defined(__cplusplus)
}
#endif

#endif /* LCD_PROTECTED_H */

/*************************** End of file ****************************/

