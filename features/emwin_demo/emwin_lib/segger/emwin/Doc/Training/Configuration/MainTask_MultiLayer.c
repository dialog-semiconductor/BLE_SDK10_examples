#include "GUI.h"

void MainTask(void) {
  int xSize, ySize;

  GUI_Init();
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  GUI_SetLayerVisEx(0, 1);
  GUI_SetLayerVisEx(1, 1);
  GUI_SetLayerVisEx(2, 1);
  GUI_DrawGradientV(0, 0, xSize, ySize, GUI_MAGENTA, GUI_CYAN);
  GUI_SelectLayer(1);
  GUI_DrawGradientH(0, 0, xSize, ySize, 0xFF000000, 0x0000FFFF);
  GUI_SetColor(GUI_TRANSPARENT);
  GUI_DrawRoundedFrame(20, 20, xSize - 20, ySize - 20, 20, 10);
  GUI_SelectLayer(2);
  GUI_SetBkColorIndex(0);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(GUI_FONT_24B_ASCII);
  GUI_DispStringHCenterAt("Different\ntransparency\nmodes", xSize / 2, ySize / 3);
  while (1) {
    GUI_Delay(100);
  }
}
