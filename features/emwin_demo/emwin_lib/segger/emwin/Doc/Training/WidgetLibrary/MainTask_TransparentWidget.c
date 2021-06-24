#include "DIALOG.h"

static void _DrawStripes(void) {
  int xSize, ySize, x;
  int Cnt = 0;
  GUI_COLOR aColor[] = {
    GUI_RED, GUI_GREEN, GUI_BLUE, 0x4080c0, GUI_WHITE, GUI_BLACK, GUI_YELLOW, GUI_MAGENTA, GUI_CYAN, 0xc04080
  };
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  for (x = 0; x < xSize; x += 5) {
    GUI_SetColor(aColor[Cnt++]);
    GUI_FillRect(x, 0, x + 4, ySize - 1);
    Cnt = (Cnt == GUI_COUNTOF(aColor)) ? 0 : Cnt;
  }
}

static void _cbClient(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_EnableAlpha(1);
    GUI_SetBkColor(0xc0000000);
    GUI_Clear();
    GUI_EnableAlpha(0);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

static void _cbFrame(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_EnableAlpha(1);
    FRAMEWIN_Callback(pMsg);
    GUI_EnableAlpha(0);
    break;
  default:
    FRAMEWIN_Callback(pMsg);
  }
}

static void _cbBk(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    _DrawStripes();
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

void MainTask(void) {
  WM_HWIN hWin, hClient;
  FRAMEWIN_SKINFLEX_PROPS Props;
  int i, j;
  int aIndex[] = {
    FRAMEWIN_CI_INACTIVE, FRAMEWIN_CI_ACTIVE
  };

  WM_SetCreateFlags(WM_CF_MEMDEV);
  GUI_Init();
  WM_SetCallback(WM_HBKWIN, _cbBk);
  FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
  for (i = 0; i < GUI_COUNTOF(aIndex); i++) {
    FRAMEWIN_GetSkinFlexProps(&Props, aIndex[i]);
    for (j = 0; j < GUI_COUNTOF(Props.aColorFrame); j++) {
      Props.aColorFrame[j] |= 0x30000000;
    }
    for (j = 0; j < GUI_COUNTOF(Props.aColorTitle); j++) {
      Props.aColorTitle[j] |= 0x30000000;
    }
    FRAMEWIN_SetSkinFlexProps(&Props, aIndex[i]);
  }
  hWin = FRAMEWIN_CreateEx(10, 10, 100, 100, WM_HBKWIN, WM_CF_SHOW, FRAMEWIN_CF_MOVEABLE, 0, "Window", _cbClient);
  hClient = WM_GetClientWindow(hWin);
  WM_SetHasTrans(hClient);
  FRAMEWIN_SetClientColor(hWin, GUI_INVALID_COLOR);
  WM_SetCallback(hWin, _cbFrame);
  while (1) {
    GUI_Delay(100);
  }
}
