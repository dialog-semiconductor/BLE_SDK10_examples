#include "WM.h"

static void _cbWin(WM_MESSAGE * pMsg) {
  GUI_RECT Rect;
  WM_MOTION_INFO * pInfo;
  int i, xDist, xPos;

  switch (pMsg->MsgId) {
  case WM_MOTION:
    pInfo = (WM_MOTION_INFO *)pMsg->Data.p;
    switch (pInfo->Cmd) {
    case WM_MOTION_INIT:
      WM_GetClientRectEx(pMsg->hWin, &Rect);
      pInfo->SnapX = (Rect.x1 + 1) / 4;
      break;
    }
    break;
  case WM_PAINT:
    GUI_Clear();
    WM_GetClientRect(&Rect);
    xDist = (Rect.x1 + 1) / 4;
    for (i = 0; i < 4; i++) {
      xPos = i * xDist;
      GUI_DrawRect(xPos, 0, xPos + xDist - 1, Rect.y1);
      GUI_SetFont(GUI_FONT_32B_ASCII);
      GUI_DispStringAt("Page #", xPos + 40, Rect.y1 / 2 - 16);
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
  WM_CreateWindowAsChild(0, 0, xSize * 4, ySize, WM_HBKWIN, WM_CF_SHOW | WM_CF_MOTION_X, _cbWin, 0);
  while (1) {
    GUI_Delay(100);
  }
}

