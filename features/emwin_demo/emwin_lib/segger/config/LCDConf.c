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
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/* Copyright (c) 2019 Modified by Dialog Semiconductor */

#ifndef EMWIN_LIB_BUILD

#include "GUI.h"
#include "GUIDRV_Lin.h"

#include "gdi.h"
#include "platform_devices.h"

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS      GDI_DISP_RESX
#define YSIZE_PHYS      GDI_DISP_RESY

#if USE_COLOR_FORMAT == CF_NATIVE_RGB332
//
// Color conversion
//
#define COLOR_CONVERSION GUICC_M332

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_LIN_8
#elif USE_COLOR_FORMAT == CF_NATIVE_RGB565
//
// Color conversion
//
#define COLOR_CONVERSION GUICC_M565

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_LIN_16
#elif USE_COLOR_FORMAT == CF_NATIVE_RGBA8888
//
// Color conversion
//
#define COLOR_CONVERSION GUICC_8888

//
// Display driver
//
#define DISPLAY_DRIVER GUIDRV_LIN_32
#endif /* USE_COLOR_FORMAT */

//
// Buffers / VScreens
//
#define NUM_BUFFERS  GDI_SINGLE_FB_NUM // Number of multiple buffers to be used
#define NUM_VSCREENS 1 // Number of virtual screens to be used

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VRAM_ADDR
  #define VRAM_ADDR gdi_get_frame_buffer_addr()
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif

/*********************************************************************
*
*       Local code
*
**********************************************************************
*/
/*********************************************************************
*
*       _CopyBuffer
*/
static void _CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  gdi_buffer_memcpy(IndexDst, IndexSrc);
}

/*********************************************************************
 *
 *       _StoreTouchEvents
 */
static void _StoreTouchEvents(gdi_touch_data_t * pState)
{
        GUI_PID_STATE PID_State = {
                .x = pState->x,
                .y = pState->y,
                .Pressed = pState->pressed,
                .Layer = 0
        };
        GUI_PID_StoreState(&PID_State);
}


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description
*   Called during the initialization process in order to set up the
*   display driver configuration.
*/
void LCD_X_Config(void) {
  //
  // At first initialize use of multiple buffers on demand
  //
  #if (NUM_BUFFERS > 1)
    GUI_MULTIBUF_Config(NUM_BUFFERS);
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
  GUI_DIRTYDEVICE_Create();
  //
  // Set custom callback function for copy operation
  //
  LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void (*)())_CopyBuffer);

  //
  // Display driver configuration
  //
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }
  LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Function description
*   This function is called by the display driver for certain purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  switch (Cmd) {
  case LCD_X_INITCONTROLLER:
    gdi_init();
    gdi_set_callback_store_touch(_StoreTouchEvents);
    return 0;
  case LCD_X_ON:
    gdi_display_enable();
    return 0;
  case LCD_X_OFF:
    gdi_display_disable();
    return 0;
  case LCD_X_SHOWBUFFER: {
    LCD_X_SHOWBUFFER_INFO *p = (LCD_X_SHOWBUFFER_INFO *)pData;
    GUI_DIRTYDEVICE_INFO Info;
    if (GUI_DIRTYDEVICE_Fetch(&Info)) {
            gdi_set_partial_update_area(Info.x0, Info.y0, Info.x0 + Info.xSize - 1, Info.y0 + Info.ySize - 1);
            gdi_set_frame_buffer(p->Index);
            gdi_display_update_async(NULL, NULL);
    }
    GUI_MULTIBUF_ConfirmEx(LayerIndex, p->Index);
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

#endif /* EMWIN_LIB_BUILD */

/*************************** End of file ****************************/
