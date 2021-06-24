#if 1

#include "GUI.h"

void MainTask(void) {
  GUI_Init();
  GUI_SetBkColor(GUI_YELLOW);
  GUI_SetColor(GUI_BLUE);
  GUI_Clear();
  GUI_DispString("Hello world");
  while (1) {
    GUI_Delay(10);
  }
}

#else

#include "GUI.h"

void MainTask(void) {
  GUI_Init();
  #if (GUI_USE_ARGB == 0)
    GUI_SetBkColor(0x00FFFF);
    GUI_SetColor(0xFF0000);
  #else
    GUI_SetBkColor(0xFFFFFF00);
    GUI_SetColor(0xFF0000FF);
  #endif
  GUI_Clear();
  GUI_DispString("Hello world");
  while (1) {
    GUI_Delay(10);
  }
}

#endif
