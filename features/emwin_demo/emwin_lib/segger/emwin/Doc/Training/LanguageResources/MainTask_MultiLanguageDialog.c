#include <stddef.h>
#include <string.h>

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
/*********************************************************************
*
*       Widget IDs
*/
#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
#define ID_BUTTON_0    (GUI_ID_USER + 0x01)
#define ID_BUTTON_1    (GUI_ID_USER + 0x02)
#define ID_BUTTON_2    (GUI_ID_USER + 0x03)
#define ID_CHECKBOX_0  (GUI_ID_USER + 0x04)
#define ID_RADIO_0     (GUI_ID_USER + 0x05)

/*********************************************************************
*
*       Messages
*/
#define APP_SETLANG (WM_USER + 1)

/*********************************************************************
*
*       Text IDs
*/
#define APP_TI_COLORS   0
#define APP_TI_HELP     1
#define APP_TI_EXIT     2
#define APP_TI_CONNECT  3
#define APP_TI_ENGLISH  4
#define APP_TI_GERMAN   5
#define APP_TI_ITALIAN  6
#define APP_TI_SETTINGS 7

#define MAX_TEXT_LEN 50

/*********************************************************************
*
*       Language index
*/
#define LANG_ENGLISH 0
#define LANG_GERMAN  1
#define LANG_ITALIAN 2

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 10)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static int _IsClosed;

/*********************************************************************
*
*       CSV-file in binary format containing the text resources
*/
static const U8 _acCSV[277UL + 1] = {
  0x22, 0x43, 0x6F, 0x6C, 0x6F, 0x72, 0x73, 0x2E, 0x2E, 0x2E, 0x22, 0x2C, 0x22, 0x46, 0x61, 0x72, 0x62, 0x65, 0x6E, 0x2E, 0x2E, 0x2E, 0x22, 0x2C, 0x22, 0x43, 0x6F, 0x6C, 0x6F, 0x72, 0x69, 0x2E, 0x2E, 0x22, 0x0D, 0x0A, 0x22, 0x48, 0x65, 0x6C,
  0x70, 0x2E, 0x2E, 0x2E, 0x22, 0x2C, 0x22, 0x48, 0x69, 0x6C, 0x66, 0x65, 0x2E, 0x2E, 0x2E, 0x22, 0x2C, 0x22, 0x41, 0x73, 0x73, 0x69, 0x73, 0x74, 0x65, 0x6E, 0x7A, 0x61, 0x2E, 0x2E, 0x2E, 0x22, 0x0D, 0x0A, 0x22, 0x45, 0x78, 0x69, 0x74, 0x22,
  0x2C, 0x22, 0x42, 0x65, 0x65, 0x6E, 0x64, 0x65, 0x6E, 0x22, 0x2C, 0x22, 0x46, 0x69, 0x6E, 0x69, 0x72, 0x65, 0x22, 0x0D, 0x0A, 0x22, 0x43, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x22, 0x2C, 0x22, 0x56, 0x65, 0x72, 0x62, 0x69, 0x6E, 0x64, 0x65,
  0x6E, 0x22, 0x2C, 0x22, 0x43, 0x6F, 0x6E, 0x65, 0x74, 0x74, 0x65, 0x72, 0x65, 0x22, 0x0D, 0x0A, 0x22, 0x45, 0x6E, 0x67, 0x6C, 0x69, 0x73, 0x68, 0x22, 0x2C, 0x22, 0x45, 0x6E, 0x67, 0x6C, 0x69, 0x73, 0x63, 0x68, 0x22, 0x2C, 0x22, 0x49, 0x6E,
  0x67, 0x6C, 0x65, 0x73, 0x65, 0x22, 0x0D, 0x0A, 0x22, 0x47, 0x65, 0x72, 0x6D, 0x61, 0x6E, 0x22, 0x2C, 0x22, 0x44, 0x65, 0x75, 0x74, 0x73, 0x63, 0x68, 0x22, 0x2C, 0x22, 0x54, 0x65, 0x64, 0x65, 0x73, 0x63, 0x6F, 0x22, 0x0D, 0x0A, 0x22, 0x49,
  0x74, 0x61, 0x6C, 0x69, 0x61, 0x6E, 0x22, 0x2C, 0x22, 0x49, 0x74, 0x61, 0x6C, 0x69, 0x65, 0x6E, 0x69, 0x73, 0x63, 0x68, 0x22, 0x2C, 0x22, 0x49, 0x74, 0x61, 0x6C, 0x69, 0x61, 0x6E, 0x6F, 0x22, 0x0D, 0x0A, 0x22, 0x53, 0x65, 0x74, 0x74, 0x69,
  0x6E, 0x67, 0x73, 0x22, 0x2C, 0x22, 0x45, 0x69, 0x6E, 0x73, 0x74, 0x65, 0x6C, 0x6C, 0x75, 0x6E, 0x67, 0x65, 0x6E, 0x22, 0x2C, 0x22, 0x49, 0x6D, 0x70, 0x6F, 0x73, 0x74, 0x61, 0x7A, 0x69, 0x6F, 0x6E, 0x65, 0x22, 0x0D, 0x0A, 0x00
};

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "", ID_FRAMEWIN_0,   0,  0, 320, 120, 0, 0x64,   0 },
  { BUTTON_CreateIndirect,   "", ID_BUTTON_0,    15, 10,  80,  20, 0, 0x0,    0 },
  { BUTTON_CreateIndirect,   "", ID_BUTTON_1,    15, 40,  80,  20, 0, 0x0,    0 },
  { BUTTON_CreateIndirect,   "", ID_BUTTON_2,    15, 70,  80,  20, 0, 0x0,    0 },
  { CHECKBOX_CreateIndirect, "", ID_CHECKBOX_0, 125, 10,  80,  20, 0, 0x0,    0 },
  { RADIO_CreateIndirect,    "", ID_RADIO_0,    215, 10,  80,  80, 0, 0x1e03, 0 },
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _SetLanguage
*/
static void _SetLanguage(int Language) {
  WM_MESSAGE Message = {0};
  
  //
  // First set up new language in language module
  //
  GUI_LANG_SetLang(Language);
  //
  // Broadcast a message to all windows
  //
  Message.MsgId = APP_SETLANG;
  WM_BroadcastMessage(&Message);
}

/*********************************************************************
*
*       _GetData
*/
static int _GetData(void * pVoid, const U8 ** ppData, unsigned NumBytes, U32 Off) {
  U8 * pData;
  U8 * pCSV;

  pData = (U8 *)*ppData; // Pointer to be used for getting data
  pCSV  = (U8 *)pVoid;   // Use custom void pointer for getting a pointer to the file data
  memcpy(pData, pCSV + Off, NumBytes);
  return NumBytes;
}

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     Id, NCode, Language;
  char acBuffer[MAX_TEXT_LEN];

  switch (pMsg->MsgId) {
  //
  // Init dialog and set language (APP_SETLANG)
  //
  case WM_INIT_DIALOG:
    Language = GUI_LANG_GetLang();
    hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);    
    RADIO_SetValue(hItem, Language);
  //
  // React on APP_SETLANG - message
  //
  case APP_SETLANG:
    //
    // Initialization of 'Framewin'
    //
    hItem = pMsg->hWin;
    GUI_LANG_GetTextBuffered(APP_TI_SETTINGS, acBuffer, sizeof(acBuffer));
    FRAMEWIN_SetText(hItem, acBuffer);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    GUI_LANG_GetTextBuffered(APP_TI_COLORS, acBuffer, sizeof(acBuffer));
    BUTTON_SetText(hItem, acBuffer);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    GUI_LANG_GetTextBuffered(APP_TI_HELP, acBuffer, sizeof(acBuffer));
    BUTTON_SetText(hItem, acBuffer);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    GUI_LANG_GetTextBuffered(APP_TI_EXIT, acBuffer, sizeof(acBuffer));
    BUTTON_SetText(hItem, acBuffer);
    //
    // Initialization of 'Checkbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
    GUI_LANG_GetTextBuffered(APP_TI_CONNECT, acBuffer, sizeof(acBuffer));
    CHECKBOX_SetText(hItem, acBuffer);
    //
    // Initialization of 'Radio'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_RADIO_0);
    GUI_LANG_GetTextBuffered(APP_TI_ENGLISH, acBuffer, sizeof(acBuffer));
    RADIO_SetText(hItem, acBuffer, 0);
    GUI_LANG_GetTextBuffered(APP_TI_GERMAN, acBuffer, sizeof(acBuffer));
    RADIO_SetText(hItem, acBuffer, 1);
    GUI_LANG_GetTextBuffered(APP_TI_ITALIAN, acBuffer, sizeof(acBuffer));
    RADIO_SetText(hItem, acBuffer, 2);
    break;
  //
  // React on user input
  //
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_RADIO_0: // Notifications sent by 'Radio'
      switch(NCode) {
      case WM_NOTIFICATION_VALUE_CHANGED:
        //
        // Get language index from widget...
        //
        Language = RADIO_GetValue(pMsg->hWinSrc);
        //
        // ...and use it in the application
        //
        _SetLanguage(Language);
        break;
      }
      break;
    case ID_BUTTON_2:
      switch(NCode) {
      case WM_NOTIFICATION_RELEASED:
        GUI_EndDialog(pMsg->hWin, 0);
        _IsClosed = 1; 
        break;
      }  
    }
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}
/*********************************************************************
*
*       _cbBkWindow
*/
static void _cbBkWindow(WM_MESSAGE * pMsg) {
  GUI_RECT Rect = {0, 180, 320, 240};

  switch(pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
    break; 
  }
}
/*********************************************************************
*
*       _CreateFramewin
*/
static WM_HWIN _CreateFramewin(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 60);
  _IsClosed = 0;
  return hWin;
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
#if GUI_SUPPORT_MEMDEV
  //
  // Use memory devices on all windows to avoid flicker
  //
  WM_SetCreateFlags(WM_CF_MEMDEV);
#endif
  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  //
  // Set background callback
  //
  WM_SetCallback(WM_HBKWIN, _cbBkWindow);
  //
  // Load CSV file
  //
  GUI_LANG_LoadCSVEx(_GetData, (void *)_acCSV);
  //
  //  Set start language
  //
  GUI_LANG_SetLang(LANG_GERMAN);
  //
  // Create a dialog...
  //
  _CreateFramewin();
  while (1) {
    if (_IsClosed) {
      GUI_Delay(1000);
      _CreateFramewin();
    }
    GUI_Delay(100);
  }
}

/*************************** End of file ****************************/
