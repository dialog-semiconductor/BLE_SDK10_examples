#include "DIALOG.h"

static int _ScrollbarSkinCust(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_CREATE:
    WM_SetHasTrans(pDrawItemInfo->hWin);
    break;
  case WIDGET_ITEM_DRAW_BUTTON_L:
  case WIDGET_ITEM_DRAW_BUTTON_R:
    GUI_SetColor(GUI_GRAY);
    GUI_FillRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, 
                        pDrawItemInfo->x1, pDrawItemInfo->y1, 4);
    GUI_SetColor(GUI_WHITE);
    GUI_DrawRoundedRect(pDrawItemInfo->x0, pDrawItemInfo->y0, 
                        pDrawItemInfo->x1, pDrawItemInfo->y1, 4);
    GUI_SetColor(GUI_BLACK);
    GUI_FillCircle((pDrawItemInfo->x1 + pDrawItemInfo->x0) / 2, 
                   (pDrawItemInfo->y1 + pDrawItemInfo->y0) / 2, 4);
    break;
  case WIDGET_ITEM_DRAW_SHAFT_L:
  case WIDGET_ITEM_DRAW_SHAFT_R:
    GUI_SetColor(GUI_WHITE);
    GUI_DrawLine(pDrawItemInfo->x0, pDrawItemInfo->y0, pDrawItemInfo->x1, pDrawItemInfo->y1);
    GUI_DrawLine(pDrawItemInfo->x0, pDrawItemInfo->y1, pDrawItemInfo->x1, pDrawItemInfo->y0);
    break;
  default:
    return SCROLLBAR_DrawSkinFlex(pDrawItemInfo);
  }
  return 0;
}

void MainTask(void) {
  WM_HWIN hWin0, hWin1, hWin2;

  WM_SetCreateFlags(WM_CF_MEMDEV);
  GUI_Init();
  WM_SetDesktopColor(GUI_BLUE);
  SCROLLBAR_SetDefaultSkinClassic();
  hWin0 = SCROLLBAR_CreateEx(10, 10, 200, 20, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_SCROLLBAR0);
  SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
  hWin1 = SCROLLBAR_CreateEx(10, 50, 200, 20, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_SCROLLBAR1);
  hWin2 = SCROLLBAR_CreateEx(10, 90, 200, 20, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_SCROLLBAR2);
  SCROLLBAR_SetSkin(hWin2, _ScrollbarSkinCust);
  SCROLLBAR_SetValue(hWin0, 30);
  SCROLLBAR_SetValue(hWin1, 70);
  SCROLLBAR_SetValue(hWin2, 30);
  while (1) {
    GUI_Delay(100);
  }
}
