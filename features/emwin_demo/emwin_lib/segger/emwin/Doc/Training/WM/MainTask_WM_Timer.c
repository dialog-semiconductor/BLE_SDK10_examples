#include "WM.h"

void _cbWin(WM_MESSAGE * pMsg) {
  int xSize, ySize;
  static int Value;
  WM_KEY_INFO * pInfo;

  switch (pMsg->MsgId) {
  case WM_PAINT:
    xSize = WM_GetWindowSizeX(pMsg->hWin);
    ySize = WM_GetWindowSizeY(pMsg->hWin);
    GUI_Clear();
    GUI_DrawRect(0, 0, xSize - 1, ySize - 1);
    GUI_DispStringHCenterAt("Window", xSize / 2, 10);
    GUI_GotoXY(xSize / 2, 50);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_DispDecMin(Value);
    break;
  case WM_KEY:
    pInfo = (WM_KEY_INFO *)pMsg->Data.p;
    if (pInfo->PressedCnt) {
      switch (pInfo->Key) {
      case GUI_KEY_UP:
        Value++;
        break;
      case GUI_KEY_DOWN:
        Value--;
        break;
      }
      WM_InvalidateWindow(pMsg->hWin);
    }
    break;
  case WM_TIMER:
    Value++;
    WM_RestartTimer(pMsg->Data.v, 500);
    WM_InvalidateWindow(pMsg->hWin);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

void _cbBk(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_DrawGradientV(0, 0, 319, 239, GUI_BLUE, GUI_MAGENTA);
  }
}

void MainTask(void) {
  WM_HWIN hWin;

  GUI_Init();
  WM_SetCallback(WM_HBKWIN, _cbBk);
  hWin = WM_CreateWindowAsChild(10, 10, 100, 100, WM_HBKWIN, WM_CF_SHOW, _cbWin, 0);
  WM_SetFocus(hWin);
  WM_CreateTimer(hWin, 0, 2000, 0);
  while (1) {
    GUI_Delay(100);
  }
}
