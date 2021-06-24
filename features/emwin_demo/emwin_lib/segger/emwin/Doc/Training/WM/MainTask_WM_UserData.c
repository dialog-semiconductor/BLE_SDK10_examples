#include "WM.h"

void _cbWin(WM_MESSAGE * pMsg) {
  int xSize, ySize;
  GUI_COLOR * pColor;

  switch (pMsg->MsgId) {
  case WM_PAINT:
    xSize = WM_GetWindowSizeX(pMsg->hWin);
    ySize = WM_GetWindowSizeY(pMsg->hWin);
    WM_GetUserData(pMsg->hWin, &pColor, sizeof(GUI_COLOR *));
    GUI_DrawGradientV(0, 0, xSize - 1, ySize - 1, *(pColor + 0), *(pColor + 1));
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

void MainTask(void) {
  WM_HWIN hWin;
  GUI_COLOR aColor[] = { GUI_MAGENTA, GUI_BLUE };
  GUI_COLOR * pColor = aColor;

  GUI_Init();
  WM_SetDesktopColor(GUI_BLACK);
  hWin = WM_CreateWindowAsChild(10, 10, 100, 100, WM_HBKWIN, WM_CF_SHOW, _cbWin, sizeof(GUI_COLOR *));
  WM_SetUserData(hWin, &pColor, sizeof(GUI_COLOR *));
  while (1) {
    GUI_Delay(100);
  }
}
