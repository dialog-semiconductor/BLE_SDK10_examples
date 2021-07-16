#include <windows.h>

#include "GUI.h"

/*********************************************************************
*
*       _GetData0: BMP, JPEG, GIF
*/
static int _GetData0(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  static char acBuffer[0x200];
  HANDLE    * phFile;
  DWORD       NumBytesRead;

  phFile = (HANDLE *)p;
  //
  // Check buffer size
  //
  if (NumBytesReq > sizeof(acBuffer)) {
    NumBytesReq = sizeof(acBuffer);
  }
  //
  // Set file pointer to the required position
  //
  SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
  //
  // Read data into buffer
  //
  ReadFile(*phFile, acBuffer, NumBytesReq, &NumBytesRead, NULL);
  //
  // Set data pointer to the beginning of the buffer
  //
  *ppData = acBuffer;
  //
  // Return number of available bytes
  //
  return NumBytesRead;
}

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
  HANDLE hFile;

  GUI_Init();
  hFile = CreateFile("C:\\Work\\emWin_Training\\Samples\\ImageFiles\\Sunset_150x100.jpg", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  GUI_JPEG_DrawEx(_GetData0, (void *)&hFile, 10, 10);
  while (1) {
    GUI_Delay(100);
  }
}
