#include <windows.h>

#include "GUI.h"

#define GERMAN  0
#define ENGLISH 1

/*********************************************************************
*
*       _GetData
*/
static int _GetData(void * pVoid, const U8 ** ppData, unsigned NumBytes, U32 Off) {
  HANDLE * phFile;
  DWORD    NumBytesRead;
  U8     * pData;

  phFile = (HANDLE *)pVoid;
  pData  = (U8 *)*ppData;
  SetFilePointer(*phFile, Off, 0, FILE_BEGIN);
  ReadFile(*phFile, pData, NumBytes, &NumBytesRead, 0);
  return NumBytesRead;
}

/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  int NumLanguages, i, Language, NumItems;
  const char * pString;
  HANDLE hFile;

  GUI_Init();
  //
  // Enable UTF8 encoding (if required)
  //
  GUI_UC_SetEncodeUTF8();
  //
  // Open file handle
  //
  hFile = CreateFile("GUI_LANG_CSV_Ger_Eng.csv", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (hFile != INVALID_HANDLE_VALUE) {
    //
    // Read CSV file
    //
    NumLanguages = GUI_LANG_LoadCSVEx(_GetData, &hFile);
    //
    // Set language
    //
    Language = GERMAN;
    GUI_LANG_SetLang(Language);
    GUI_LANG_GetNumItems(Language);
    NumItems = GUI_LANG_GetNumItems(GERMAN);
    //
    // Draw all 
    //
    for (i = 0; i < NumItems; i++) {
      pString = GUI_LANG_GetText(i);
      GUI_DispString(pString);
      GUI_DispNextLine();
    }
    //
    // Close file
    //
    CloseHandle(hWine);
  }
  while (1) {
    GUI_Delay(100);
  }
}
