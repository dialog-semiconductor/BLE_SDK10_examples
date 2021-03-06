#include <stddef.h>

#include "GUI.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GUI_CONST_STORAGE unsigned char _acHourGlassM_00[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x41, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_01[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x21, 0x11, 0x21, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_02[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x11, 0x11, 0x21, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_03[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_04[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x21, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_05[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x12, 0x11, 0x12, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x21, 0x21, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_06[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x12, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x21, 0x21, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_07[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x11, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_08[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x11, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x42, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_09[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x11, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x11, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x42, 0x12, 0x12, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_10[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x11, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x11, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x42, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x21, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x42, 0x12, 0x12, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_11[] = {
  0x00, 0x00, 0x03, 0x33, 0x33, 0x33, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x11, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x11, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x21, 0x11, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x51, 0x41, 0x11, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x12, 0x11, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x44, 0x21, 0x21, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x42, 0x12, 0x12, 0x11, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x51, 0x24, 0x21, 0x21, 0x21, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x36, 0x66, 0x63, 0x33, 0x33, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_12[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x20, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x32, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x63, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x36, 0x32, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x14, 0x43, 0x63, 0x20, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x33, 0x32, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x41, 0x11, 0x13, 0x33, 0x20,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x11, 0x11, 0x11, 0x33, 0x32,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x11, 0x11, 0x11, 0x12, 0x22,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x11, 0x11, 0x11, 0x20, 0x00,
  0x00, 0x00, 0x05, 0x55, 0x55, 0x41, 0x11, 0x11, 0x12, 0x00, 0x00,
  0x00, 0x00, 0x51, 0x44, 0x44, 0x11, 0x22, 0x22, 0x20, 0x00, 0x00,
  0x00, 0x05, 0x14, 0x41, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x33, 0x21, 0x24, 0x21, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x36, 0x32, 0x42, 0x12, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x63, 0x21, 0x21, 0x21, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x36, 0x32, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0x33, 0x21, 0x21, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x33, 0x32, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x33, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x33, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_13[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32,
  0x33, 0x25, 0x55, 0x50, 0x00, 0x00, 0x00, 0x05, 0x55, 0x53, 0x32,
  0x36, 0x21, 0x11, 0x12, 0x00, 0x00, 0x00, 0x51, 0x11, 0x13, 0x62,
  0x36, 0x21, 0x44, 0x41, 0x20, 0x00, 0x05, 0x14, 0x44, 0x43, 0x62,
  0x36, 0x21, 0x44, 0x41, 0x12, 0x00, 0x51, 0x44, 0x44, 0x43, 0x62,
  0x36, 0x21, 0x44, 0x41, 0x11, 0x25, 0x14, 0x41, 0x11, 0x13, 0x62,
  0x33, 0x21, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x13, 0x32,
  0x33, 0x21, 0x21, 0x21, 0x21, 0x22, 0x11, 0x11, 0x11, 0x13, 0x32,
  0x33, 0x22, 0x12, 0x12, 0x12, 0x00, 0x21, 0x11, 0x11, 0x13, 0x32,
  0x33, 0x21, 0x21, 0x21, 0x20, 0x00, 0x02, 0x11, 0x11, 0x13, 0x32,
  0x33, 0x22, 0x12, 0x12, 0x00, 0x00, 0x00, 0x21, 0x11, 0x13, 0x32,
  0x33, 0x22, 0x22, 0x20, 0x00, 0x00, 0x00, 0x02, 0x22, 0x22, 0x32,
  0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static GUI_CONST_STORAGE unsigned char _acHourGlassM_14[] = {
  0x00, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x23, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x33, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x23, 0x33, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x33, 0x31, 0x11, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x23, 0x63, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x02, 0x36, 0x31, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x23, 0x63, 0x11, 0x11, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x33, 0x31, 0x44, 0x21, 0x21, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x05, 0x12, 0x42, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x51, 0x24, 0x24, 0x21, 0x55, 0x55, 0x20, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x22, 0x22, 0x41, 0x11, 0x11, 0x12, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x11, 0x11, 0x11, 0x20, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x11, 0x11, 0x11, 0x12, 0x22,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x11, 0x11, 0x11, 0x23, 0x32,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x41, 0x11, 0x12, 0x33, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x44, 0x11, 0x23, 0x33, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x14, 0x42, 0x33, 0x30, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x23, 0x63, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x36, 0x30, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x63, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00
};

static GUI_CONST_STORAGE GUI_COLOR _ColorsHourGlassM[] = {
     0x0000FF,0xCCCCCC,0x000000,0x999900,0xFFFFFF,0x999999,0xFFFF00
};

static GUI_CONST_STORAGE GUI_LOGPALETTE _PalHourGlassM = {
  7,	/* number of entries */
  1, 	/* Has transparency */
  &_ColorsHourGlassM[0]
};

static GUI_CONST_STORAGE GUI_BITMAP _abmHourGlassM[] = {
  { 22, 22, 11, 4, _acHourGlassM_00, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_01, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_02, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_03, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_04, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_05, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_06, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_07, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_08, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_09, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_10, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_11, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_12, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_13, &_PalHourGlassM },
  { 22, 22, 11, 4, _acHourGlassM_14, &_PalHourGlassM },
};

static GUI_CONST_STORAGE GUI_BITMAP * _apbmHourGlassM[] = {
  &_abmHourGlassM[ 0],
  &_abmHourGlassM[ 1],
  &_abmHourGlassM[ 2],
  &_abmHourGlassM[ 3],
  &_abmHourGlassM[ 4],
  &_abmHourGlassM[ 5],
  &_abmHourGlassM[ 6],
  &_abmHourGlassM[ 7],
  &_abmHourGlassM[ 8],
  &_abmHourGlassM[ 9],
  &_abmHourGlassM[10],
  &_abmHourGlassM[11],
  &_abmHourGlassM[12],
  &_abmHourGlassM[13],
  &_abmHourGlassM[14],
};

static const GUI_CURSOR_ANIM _CursorAnimHourglassM = {
  _apbmHourGlassM,
  11,
  11,
  100,
  NULL,
  GUI_COUNTOF(_apbmHourGlassM)
};

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
  GUI_Init();
  GUI_CURSOR_SelectAnim(&_CursorAnimHourglassM);
  GUI_CURSOR_SetPosition(100, 100);
  while (1) {
    GUI_Delay(100);
  }
}
