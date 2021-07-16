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
File        : MESSAGEBOX.h
Purpose     : Message box interface
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "WM.h"

#if GUI_WINSUPPORT

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

WM_HWIN MESSAGEBOX_Create(const char * sMessage, const char * sCaption, int Flags);

/*********************************************************************
*
*       The callback ...
*
* Do not call it directly ! It is only to be used from within an
* overwritten callback.
*/
void MESSAGEBOX_Callback(WM_MESSAGE * pMsg);

#if defined(__cplusplus)
  }
#endif

#endif /* GUI_WINSUPPORT */

#endif /* MESSAGEBOX */

/*************************** End of file ****************************/
