#include "DIALOG.h"

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Window", 0,            10, 10, 100, 50 },
  { TEXT_CreateIndirect,     "Text",   GUI_ID_TEXT0, 10, 10,  50, 20 },
};

void _cbFrame(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetFont(hItem, GUI_FONT_20F_ASCII);
    TEXT_SetTextColor(hItem, GUI_GREEN);
    break;
  }
}

void MainTask(void) {
  WM_HWIN hWin;

  GUI_Init();
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbFrame, 0, 0, 0);
  while (1) {
    GUI_Delay(100);
  }
}
