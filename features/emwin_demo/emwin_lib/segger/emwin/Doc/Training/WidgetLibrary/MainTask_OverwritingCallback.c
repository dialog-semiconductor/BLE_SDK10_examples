#include "DIALOG.h"

typedef struct {
  char * apText[2];
} INFO;

static void _cbButton(WM_MESSAGE * pMsg) {
  GUI_COLOR aColor[] = { 0x444444, 0x666666 };
  INFO * pInfo;
  int Index;
  GUI_RECT Rect;

  switch (pMsg->MsgId) {
  case WM_PAINT:
    WM_GetClientRect(&Rect);
    BUTTON_GetUserData(pMsg->hWin, &pInfo, sizeof(pInfo));
    Index = BUTTON_IsPressed(pMsg->hWin);
    GUI_SetBkColor(aColor[Index]);
    GUI_Clear();
    GUI_DispStringInRect(pInfo->apText[Index], &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    break;
  default:
    BUTTON_Callback(pMsg);
    break;
  }
}

void MainTask(void) {
  WM_HWIN hWin;
  INFO Info = { { "Pressed", "Unpressed" } };
  INFO * pInfo = &Info;

  GUI_Init();
  hWin = BUTTON_CreateUser(10, 10, 100, 20, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_BUTTON0, sizeof(INFO *));
  BUTTON_SetUserData(hWin, &pInfo, sizeof(INFO *));
  WM_SetCallback(hWin, _cbButton);
  while (1) {
    GUI_Delay(100);
  }
}
