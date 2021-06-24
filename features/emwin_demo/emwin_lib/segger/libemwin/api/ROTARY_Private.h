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
File        : ROTARY.h
Purpose     : ROTARY include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef ROTARY_PRIVATE_H
#define ROTARY_PRIVATE_H

#include "ROTARY.h"
#include "GUI_Private.h"

#if (GUI_SUPPORT_MEMDEV && GUI_WINSUPPORT)

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  I32 Period;    // Time in ms it takes to stop the knob in ms
  I32 TickSize;  // Movement range in 1/10 of degree for one key push
} ROTARY_PROPS;

typedef struct {
  WIDGET       Widget;            // (obvious)
  ROTARY_PROPS Props;             // (obvious)
  WM_HMEM      hContext;          // Motion context
  I32          Angle;             // Current angle within the given range (AngNeg & AngPos)
  I32          Snap;              // Snap section
  I32          MinVRange;         // Minimum of value range
  I32          MaxVRange;         // Maximum of value range
  I32          AngPos;            // Positive turning angle in 10th of degrees
  I32          AngNeg;            // Negative turning angle in 10th of degrees
  int          Radius;            // Mid point difference between widget and marker
  I32          Offset;            // Angle offset for drawing marker
  U8           DoRotate;          // If set the marker image is rotated
  WM_HMEM      hDrawObjBk;        // Background image
  WM_HMEM      hDrawObjMarker;    // Draw object for marker image
  WM_HMEM      hDrawObjMarkerHR;  // Draw object for marker image (HR)
} ROTARY_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define ROTARY_INIT_ID(p) p->Widget.DebugId = ROTARY_ID
#else
  #define ROTARY_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  ROTARY_OBJ * ROTARY_LockH(ROTARY_Handle h);
  #define ROTARY_LOCK_H(h)   ROTARY_LockH(h)
#else
  #define ROTARY_LOCK_H(h)   (ROTARY_OBJ *)GUI_LOCK_H(h)
#endif

/*********************************************************************
*
*       Module internal data
*
**********************************************************************
*/
extern ROTARY_PROPS ROTARY__DefaultProps;

#endif   // (GUI_SUPPORT_MEMDEV && GUI_WINSUPPORT)
#endif   // ROTARY_PRIVATE_H
