#include <stddef.h>

#include "DIALOG.h"

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
  WM_Exec();
  FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
  FRAMEWIN_SetDefaultFont(GUI_FONT_20B_ASCII);
  FRAMEWIN_SetDefaultTextColor(FRAMEWIN_CI_ACTIVE,   GUI_DARKGRAY);
  FRAMEWIN_SetDefaultTextColor(FRAMEWIN_CI_INACTIVE, GUI_DARKGRAY);
  FRAMEWIN_SetDefaultTextAlign(GUI_TA_HCENTER);
  hWin = FRAMEWIN_CreateEx(60, 60, 200, 120, WM_HBKWIN, WM_CF_SHOW, 0, 0, "FRAMEWIN", NULL);
  while (1) {
    GUI_MEMDEV_FadeInWindow  (hWin, 400);
    GUI_Delay(400);
    GUI_MEMDEV_FadeOutWindow (hWin, 400);
    GUI_MEMDEV_MoveInWindow  (hWin,   0, 240,  45, 400);
    GUI_Delay(400);
    GUI_MEMDEV_MoveOutWindow (hWin, 320, 240, -45, 400);
    GUI_MEMDEV_ShiftInWindow (hWin, 400, GUI_MEMDEV_EDGE_TOP);
    GUI_Delay(400);
    GUI_MEMDEV_ShiftOutWindow(hWin, 400, GUI_MEMDEV_EDGE_BOTTOM);
  }
}
