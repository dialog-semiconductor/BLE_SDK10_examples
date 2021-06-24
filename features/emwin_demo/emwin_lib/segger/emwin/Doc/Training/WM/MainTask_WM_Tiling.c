#include "WM.h"

void _cbBk(WM_MESSAGE * pMsg) {
  GUI_COLOR aColor[] = { GUI_BLUE, GUI_GREEN, GUI_RED, GUI_CYAN, GUI_MAGENTA, GUI_YELLOW };
  static int Index;

  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(aColor[Index++]);
    if (Index == GUI_COUNTOF(aColor)) {
      Index = 0;
    }
    GUI_Clear();
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

void _cbWin(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_Clear();
    GUI_DispStringAt("Hello world!", 10, 10);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

void MainTask(void) {
  WM_HWIN hWin, hWinBk;

  GUI_Init();
  hWinBk = WM_CreateWindowAsChild(0, 0, 320, 240, WM_HBKWIN, WM_CF_SHOW, _cbBk, 0);
  hWin = WM_CreateWindowAsChild(10, 10, 100, 100, hWinBk,    WM_CF_SHOW, _cbWin, 0);
  while (1) {
    GUI_Delay(100);
  }
}
