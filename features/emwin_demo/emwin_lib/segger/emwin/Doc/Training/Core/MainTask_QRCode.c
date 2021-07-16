#include "GUI.h"

#define PIXEL_SIZE 4

static const char sText[] = "www.segger.com";

void MainTask(void) {
  GUI_HMEM hQR;
  GUI_QR_INFO Info;
  
  GUI_Init();
  GUI_SetBkColor(GUI_WHITE);
  GUI_SetColor(GUI_BLACK);
  GUI_Clear();
  hQR = GUI_QR_Create(sText, PIXEL_SIZE, GUI_QR_ECLEVEL_H, 0);
  GUI_QR_Draw(hQR, 50, 50);
  GUI_QR_GetInfo(hQR, &Info);
  GUI_QR_Delete(hQR);
  GUI_GotoXY(50, 70 + Info.Size);
  GUI_SetLBorder(50);
  GUI_DispString("Size of bitmap:    ");
  GUI_DispDecMin(Info.Size);
  GUI_DispNextLine();
  GUI_DispString("Number of modules: ");
  GUI_DispDecMin(Info.Width);
  GUI_DispNextLine();
  GUI_DispString("Version:           ");
  GUI_DispDecMin(Info.Version);
  GUI_DispNextLine();
  while (1) {
    GUI_Delay(100);
  }
}
