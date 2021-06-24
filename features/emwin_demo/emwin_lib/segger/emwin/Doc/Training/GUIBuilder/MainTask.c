#include "DIALOG.h"

WM_HWIN CreateFramewin(void);
WM_HWIN CreateWindow(void);

WM_HWIN hWin0;
WM_HWIN hWin1;

void MainTask(void) {
  WM_SetCreateFlags(WM_CF_MEMDEV);
  GUI_Init();
  FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
  BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
  hWin0 = CreateWindow();
  WM_HideWindow(hWin0);
  hWin1 = CreateFramewin();
  while (1) {
    GUI_Delay(100);
  }
}

