#include "GUI.h"
#include "GUIDRV_Lin.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define XSIZE_PHYS 240
#define YSIZE_PHYS 180

/*********************************************************************
*
*       LCD_X_Config
*/
void LCD_X_Config(void) {
  int Layer;
  const GUI_DEVICE_API     * apDeviceAPI[] = {
    GUIDRV_LIN_16,      // Layer 0: 16bpp
    GUIDRV_LIN_32,      // Layer 1: 32bpp
    GUIDRV_LIN_1        // Layer 2:  1bpp
  };
  const LCD_API_COLOR_CONV * apColorConvAPI[] = {
    GUICC_565,          // Layer 0: 16bpp
    GUICC_8888,         // Layer 1: 32bpp
    GUICC_1             // Layer 2:  1bpp
  };
  void * apVRAM_Addr[] = {
    (void *)0x20000000, // Address of frame buffer layer 0
    (void *)0x20025800, // Address of frame buffer layer 1
    (void *)0x20070800, // Address of frame buffer layer 2
  };
  for (Layer = 0; Layer < 3; Layer++) {
    //
    // Set display driver and color conversion for 1st layer
    //
    GUI_DEVICE_CreateAndLink(apDeviceAPI[Layer], apColorConvAPI[Layer], 0, Layer);
    //
    // Display driver configuration
    //
    if (LCD_GetSwapXY()) {
      LCD_SetSizeEx (Layer, YSIZE_PHYS, XSIZE_PHYS);
      LCD_SetVSizeEx(Layer, YSIZE_PHYS, XSIZE_PHYS);
    } else {
      LCD_SetSizeEx (Layer, XSIZE_PHYS, YSIZE_PHYS);
      LCD_SetVSizeEx(Layer, XSIZE_PHYS, YSIZE_PHYS);
    }
    LCD_SetVRAMAddrEx(Layer, apVRAM_Addr[Layer]);
  }
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  return 0;
}
