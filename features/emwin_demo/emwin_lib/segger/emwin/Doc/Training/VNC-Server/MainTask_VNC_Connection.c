#include "GUI_VNC.h"

#ifndef WIN32

#include "RTOS.h"
#include "IP.h"

enum {
  TASKPRIO_IPMAIN = 101,
  TASKPRIO_IPRX,
};

static OS_STACKPTR int _IP_Stack[512];
static OS_TASK         _IP_TCB;
static OS_STACKPTR int _IPRx_Stack[256];
static OS_TASK         _IPRx_TCB;

static void _InitVNC(void) {
  //
  // Init IP
  //
  IP_Init();
  //
  // Start TCP/IP task
  //
  OS_CREATETASK(&_IP_TCB,   "IP_Task",   IP_Task,   TASKPRIO_IPMAIN, _IP_Stack);
  OS_CREATETASK(&_IPRx_TCB, "IP_RxTask", IP_RxTask, TASKPRIO_IPRX,   _IPRx_Stack);
  //
  // Wait until IP interface is ready to use
  //
  while (IP_IFaceIsReady() == 0) {
    OS_Delay(100);
  }
}

#endif

void MainTask(void) {
  #ifndef WIN32
    U32  IPAddr;
  #endif
  char acIP[16] = "192.168.4.15";
  int NumConnections;

  #ifndef WIN32
    _InitVNC();
  #endif
  //
  // Initialize emWin
  //
  GUI_Init();
  //
  // Start VNC-server
  //
  GUI_VNC_SetPassword("1234");
  GUI_VNC_X_StartServer(0, 0);
  do {
    #ifndef WIN32
      //
      // Get IP address
      //
      IPAddr = IP_GetIPAddr(0);
      IP_PrintIPAddr(acIP, IPAddr, sizeof(acIP));
    #endif
    //
    // Wait for connection
    //
    GUI_DispString("Waiting for connection...");
    do {
      NumConnections = GUI_VNC_GetNumConnections();
    } while (NumConnections == 0);
    //
    // Draw something
    //
    GUI_Clear();
    GUI_DispString("Connected. IP-Address is: ");
    GUI_DispString(acIP);
    while (GUI_VNC_GetNumConnections()) {
      GUI_Delay(100);
    }
    GUI_Clear();
  } while (1);
}
