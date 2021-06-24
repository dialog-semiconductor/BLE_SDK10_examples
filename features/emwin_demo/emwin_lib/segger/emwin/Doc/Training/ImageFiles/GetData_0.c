#include <windows.h>

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
