#include <windows.h> // Required only for file handling

#include "GUI.h"

/*********************************************************************
*
*       _GetData1: DTA, PNG
*/
static int _GetData1(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  HANDLE * phFile;
  DWORD    NumBytesRead;
  U8     * pData;

  pData  = (U8 *)*ppData;
  phFile = (HANDLE *)p;
  //
  // Set file pointer to the required position
  //
  SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
  //
  // Read data into buffer
  //
  ReadFile(*phFile, pData, NumBytesReq, &NumBytesRead, NULL);
  //
  // Return number of available bytes
  //
  return NumBytesRead;
}


/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  GUI_MEMDEV_Handle hMem;
  HANDLE hFile;

  GUI_Init();
  GUI_DrawGradientV(0, 0, 319, 239, GUI_WHITE, GUI_GRAY);
  hMem = GUI_MEMDEV_CreateFixed(0, 0, 162, 150, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUICC_8888);
  hFile = CreateFile("C:\\Work\\emWin_Training\\Samples\\BitmapConverter\\Toucan_162x150.png", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  GUI_MEMDEV_Select(hMem);
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_Clear();
  GUI_PNG_DrawEx(_GetData1, (void *)&hFile, 10, 10);
  GUI_MEMDEV_Select(0);
  CloseHandle(hFile);
  GUI_MEMDEV_WriteAt(hMem, 0, 0);
  GUI_MEMDEV_WriteAt(hMem, 160, 0);
  while (1) {
    GUI_Delay(100);
  }
}
