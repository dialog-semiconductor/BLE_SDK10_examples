#include <windows.h>

#include "GUI.h"

#define TIME_MIN 20

static int _GetData1(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  HANDLE * phFile;
  DWORD    NumBytesRead;
  U8     * pData;

  pData  = (U8 *)*ppData;
  phFile = (HANDLE *)p;
  SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
  ReadFile(*phFile, pData, NumBytesReq, &NumBytesRead, NULL);
  return NumBytesRead;
}

void MainTask(void) {
  GUI_MEMDEV_Handle hMemImage, hMemWork, hMemBk;
  HANDLE hFile;
  I32 a1000, Add, TimeStart, Time0, TimeUsed;

  GUI_Init();
  GUI_DrawGradientV(0, 0, 319, 239, GUI_WHITE, GUI_GRAY);
  hMemImage = GUI_MEMDEV_CreateFixed(10, 10, 50, 50, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUICC_8888);
  hMemWork  = GUI_MEMDEV_CreateFixed(10, 10, 50, 50, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUICC_8888);
  hMemBk    = GUI_MEMDEV_CreateFixed(10, 10, 50, 50, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUICC_8888);
  hFile = CreateFile("C:\\Work\\emWin_Training\\Samples\\BitmapConverter\\Printer_50x50_ARGB.png", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  GUI_MEMDEV_Select(hMemImage);
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_Clear();
  GUI_PNG_DrawEx(_GetData1, (void *)&hFile, 10, 10);
  GUI_MEMDEV_Select(0);
  CloseHandle(hFile);
  GUI_MEMDEV_CopyFromLCD(hMemBk);
  a1000 = Add = 0;
  TimeStart = GUI_GetTime();
  while (1) {
    Time0 = GUI_GetTime();
    GUI_MEMDEV_Select(hMemWork);
    GUI_MEMDEV_Write(hMemBk);
    GUI_MEMDEV_RotateHQ(hMemImage, hMemWork, 0, 0, -a1000, 1000); // High quality
    a1000 = (GUI_GetTime() - TimeStart) * 90 - Add;
    if (a1000 > 360000) {
      Add   += 360000;
      a1000 -= 360000;
    }
    GUI_MEMDEV_CopyToLCD(hMemWork);
    TimeUsed = GUI_GetTime() - Time0;
    if (TIME_MIN > TimeUsed) {
      GUI_X_Delay(TIME_MIN - TimeUsed);
    }
  }
}
