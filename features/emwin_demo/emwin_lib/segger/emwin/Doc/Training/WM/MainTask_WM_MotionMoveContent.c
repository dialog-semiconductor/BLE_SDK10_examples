#include "WM.h"

static void _cbWin(WM_MESSAGE * pMsg) {
  GUI_RECT Rect;
  WM_MOTION_INFO * pInfo;
  static int xPos;
  int i, xDist;

  switch (pMsg->MsgId) {
  case WM_MOTION:
    pInfo = (WM_MOTION_INFO *)pMsg->Data.p;
    switch (pInfo->Cmd) {
    case WM_MOTION_INIT:
      WM_GetClientRectEx(pMsg->hWin, &Rect);
      pInfo->Flags = WM_CF_MOTION_X | WM_MOTION_MANAGE_BY_WINDOW;
      pInfo->SnapX = Rect.x1 + 1;
      break;
    case WM_MOTION_MOVE:
      xPos += pInfo->dx;
      if (xPos < -960) {
        xPos = -960;
      } else if (xPos > 0) {
        xPos = 0;
      }
      WM_InvalidateWindow(pMsg->hWin);
      break;
    case WM_MOTION_GETPOS:
      pInfo->xPos = xPos;
      break;
    }
    break;
  case WM_PAINT:
    GUI_Clear();
    WM_GetClientRect(&Rect);
    xDist = Rect.x1 + 1;
    for (i = 0; i < 4; i++) {
      GUI_DrawRect(i * xDist + xPos, 0, (i + 1) * xDist + xPos - 1, Rect.y1);
      GUI_SetFont(GUI_FONT_32B_ASCII);
      GUI_DispStringAt("Page #", i * xDist + xPos + 40, Rect.y1 / 2 - 16);
      GUI_DispDecMin(i);
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

void MainTask(void) {
  int xSize, ySize;

  WM_SetCreateFlags(WM_CF_MEMDEV);
  GUI_Init();
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  WM_SetSize(WM_HBKWIN, xSize, ySize);
  WM_SetDesktopColor(GUI_BLACK);
  WM_MOTION_Enable(1);
  WM_CreateWindowAsChild(0, 0, xSize, ySize, WM_HBKWIN, WM_CF_SHOW, _cbWin, 0);
  while (1) {
    GUI_Delay(100);
  }
}
