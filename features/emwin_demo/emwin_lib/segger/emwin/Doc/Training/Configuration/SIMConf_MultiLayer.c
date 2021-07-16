#include "LCD_SIM.h"

void SIM_X_Config() {
  int xSize, ySize;

  xSize = LCD_GetXSizeEx(0);
  ySize = LCD_GetYSizeEx(0);
  SIM_GUI_SetCompositeSize(xSize, ySize);
  SIM_GUI_SetTransMode(1, GUI_TRANSMODE_PIXELALPHA);
  SIM_GUI_SetTransMode(2, GUI_TRANSMODE_ZERO);
}
