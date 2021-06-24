#include "DIALOG.h"

static int _cbOwner(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  char acBuffer[20];
  const GUI_FONT GUI_UNI_PTR * pFont;
  int Sel;

  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_GET_XSIZE:
    LISTBOX_GetItemText(pDrawItemInfo->hWin, pDrawItemInfo->ItemIndex, acBuffer, sizeof(acBuffer));
    return GUI_GetStringDistX(acBuffer);
  case WIDGET_ITEM_GET_YSIZE:
    pFont = LISTBOX_GetFont(pDrawItemInfo->hWin);
    return GUI_GetYSizeOfFont(pFont);
  case WIDGET_ITEM_DRAW:
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    LISTBOX_GetItemText(pDrawItemInfo->hWin, pDrawItemInfo->ItemIndex, acBuffer, sizeof(acBuffer));
    Sel = LISTBOX_GetSel(pDrawItemInfo->hWin);
    if (pDrawItemInfo->ItemIndex == Sel) {
      GUI_SetColor(GUI_RED);
    } else {
      GUI_SetColor(GUI_BLACK);
    }
    GUI_DispStringAt(acBuffer, pDrawItemInfo->x0, pDrawItemInfo->y0);
    break;
  }
  return 0;
}

void MainTask(void) {
  WM_HWIN hWin;
  const char * apText[] = { "Item0", "Item1", "Item2", NULL };

  GUI_Init();
  hWin = LISTBOX_CreateEx(10, 10, 100, 100, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_LISTBOX0, apText);
  LISTBOX_SetOwnerDraw(hWin, _cbOwner);
  while (1) {
    GUI_Delay(100);
  }
}
