#ifdef WIN32
  #include <windows.h>
#else
  #include "FS.h"
#endif

#include "GUI.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define CACHE_SIZE (50 * 1024)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
#ifdef WIN32
  HANDLE hFile1;     // Table-Access
  HANDLE hFile2;     // Data-Access
#else
  FS_FILE * pFile1;  // Table-Access
  FS_FILE * pFile2;  // Data-Access
#endif
  U32 Threshold;
} FONT_FILE_HANDLE;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Read16
*/
U16 _Read16(const U8 ** ppData) {
  const U8 * pData;
  U16  Value;
  pData = *ppData;
  Value = *pData;
  Value |= (U16)(*(pData + 1) << 8);
  pData += 2;
  *ppData = pData;
  return Value;
}

/*********************************************************************
*
*       _cbGetData
*
* Purpose:
*   Callback function for getting font data
*
* Parameters:
*   Off      - Position of XBF file to be read
*   NumBytes - Number of requested bytes
*   pVoid    - Application defined pointer
*   pBuffer  - Pointer to buffer to be filled by the function
*
* Return value:
*   0 on success, 1 on error
*/
static int _cbGetDataXBF(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer) {
  FONT_FILE_HANDLE * pFontFile;

#ifdef WIN32
  HANDLE hFile;
  U32 NumBytesRead;
  
  pFontFile = (FONT_FILE_HANDLE *)pVoid;
  if (Off < pFontFile->Threshold) {
    hFile = pFontFile->hFile1;  // Table-Access
  } else {
    hFile = pFontFile->hFile2;  // Data-Access
  }
  if (SetFilePointer(hFile, Off, 0, FILE_BEGIN) == 0xFFFFFFFF) {
    return 1; // Error
  }
  if (!ReadFile(hFile, pBuffer, NumBytes, &NumBytesRead, 0)) {
    return 1; // Error
  }
  if (NumBytesRead != NumBytes) {
    return 1; // Error
  }
  return 0;
#else
  FS_FILE * pFile;

  pFontFile = (FONT_FILE_HANDLE *)pVoid;
  if (Off < pFontFile->Threshold) {
    pFile = pFontFile->pFile1;  // Table-Access
  } else {
    pFile = pFontFile->pFile2;  // Data-Access
  }
  if(FS_FSeek(pFile, Off, FS_SEEK_SET)){
    return 1;
  }
  if(FS_Read(pFile, pBuffer, NumBytes) != NumBytes){
    return 1;
  }
  return 0;
#endif
}

/*********************************************************************
*
*       _InitFontFileHandle
*/
static int _InitFontFileHandle(const char * pFileName, FONT_FILE_HANDLE * pFontFile) {
  U8 aBuffer[18];
  U32 NumBytesRead;
  U8 * pBuffer;
  U16 First, Last;

#ifdef WIN32
  pFontFile->hFile1 = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  pFontFile->hFile2 = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if ((pFontFile->hFile1 != INVALID_HANDLE_VALUE) && (pFontFile->hFile2 != INVALID_HANDLE_VALUE)) {
    SetFilePointer(pFontFile->hFile1, 0, 0, FILE_BEGIN);
    if (ReadFile(pFontFile->hFile1, aBuffer, sizeof(aBuffer), &NumBytesRead, 0)) {
      if (NumBytesRead == sizeof(aBuffer)) {
        pBuffer = aBuffer + 14;
        First = _Read16(&pBuffer);
        Last  = _Read16(&pBuffer);
        pFontFile->Threshold = 18 + (Last - First + 1) * 6;
        return 0;
      }
    }
  }
#else
  pFontFile->pFile1 = FS_FOpen(pFileName, "r");
  pFontFile->pFile2 = FS_FOpen(pFileName, "r");
  if (pFontFile->pFile1 && pFontFile->pFile2) {
    FS_FSeek(pFontFile->pFile1, 0, FS_SEEK_SET);
    NumBytesRead = FS_Read(pFontFile->pFile1, aBuffer, sizeof(aBuffer));
    if (NumBytesRead == sizeof(aBuffer)) {
      pBuffer = aBuffer + 14;
      First = _Read16(&pBuffer);
      Last  = _Read16(&pBuffer);
      pFontFile->Threshold = 18 + (Last - First + 1) * 6;
      return 0;
    }
  }
#endif
  return 1;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  FONT_FILE_HANDLE FontFile;
  GUI_FONT         Font;
  GUI_XBF_DATA     XBF_Data;
  int xSize, ySize;
  const char * pFilename = {
  #ifdef WIN32
    "C:\\Work\\emWin\\Doc\\Training\\Samples\\FontConverter\\FontASCII_16_EXT_XBF.xbf"
  #else
    "FontASCII_16_EXT_XBF.xbf"
  #endif
  };

#ifndef WIN32
  static char _acCache[CACHE_SIZE]; // Allocate RAM for cache buffer
  
  FS_Init();
  FS_FAT_SupportLFN();
  FS_AssignCache("", _acCache, sizeof(_acCache), FS_CACHE_ALL);
#endif
  GUI_Init();
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  //
  // Get file handle
  //
  _InitFontFileHandle(pFilename, &FontFile);
  //
  // Create XBF font
  //
  GUI_XBF_CreateFont(&Font,                 // Pointer to GUI_FONT structure in RAM
                     &XBF_Data,             // Pointer to GUI_XBF_DATA structure in RAM
                     GUI_XBF_TYPE_PROP_EXT, // Font type to be created
                     _cbGetDataXBF,         // Pointer to callback function
                     (void *)&FontFile);    // Pointer to be passed to GetData function
  //
  // Draw some text
  //
  GUI_DispStringHCenterAt("Hello world!", xSize / 2, ySize / 2 - 4);
  while (1) {
    GUI_Delay(100);
  }
}

/*************************** End of file ****************************/
