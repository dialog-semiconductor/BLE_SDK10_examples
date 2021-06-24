#include "GUI.h"
#include "GUIDRV_Lin.h"

#define XSIZE_PHYS 320
#define YSIZE_PHYS 240

#define NUM_BUFFERS 3

#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif

void LCD_X_Config(void) {
  GUI_DEVICE_CreateAndLink(GUIDRV_LIN_16, GUICC_565, 0, 0);
  if (LCD_GetSwapXY()) {
    LCD_SetSizeEx (0, YSIZE_PHYS,  XSIZE_PHYS);
    LCD_SetVSizeEx(0, VYSIZE_PHYS, VXSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS,  YSIZE_PHYS);
    LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
  }
  LCD_SetVRAMAddrEx(0, (void *)0x200000);
}

int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  return 0;
}
