#include "GUI.h"

void MainTask(void) {
  GUI_MEMDEV_Handle hMem0, hMem1;
  GUI_RECT Rect = { 10, 10, 109, 34 };

  GUI_Init();
  hMem0 = GUI_MEMDEV_CreateFixed(Rect.x0, Rect.y0, 
                                 Rect.x1 - Rect.x0 + 1, Rect.y1 - Rect.y0 + 1, 
                                 GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUICC_8888);
  hMem1 = GUI_MEMDEV_CreateFixed(Rect.x0, Rect.y0, 
                                 Rect.x1 - Rect.x0 + 1, Rect.y1 - Rect.y0 + 1, 
                                 GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUICC_8888);
  GUI_SetFont(GUI_FONT_20B_ASCII);
  GUI_MEMDEV_Select(hMem0);
  GUI_SetColor(GUI_LIGHTBLUE);
  GUI_DrawRectEx(&Rect);
  GUI_DispStringInRect("Memdev", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  GUI_MEMDEV_Select(hMem1);
  GUI_SetColor(GUI_WHITE);
  GUI_DrawRectEx(&Rect);
  GUI_DispStringInRect("Fading", &Rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  while (1) {
    GUI_MEMDEV_FadeDevices(hMem0, hMem1, 500);
    GUI_MEMDEV_FadeDevices(hMem1, hMem0, 500);
  }
}
