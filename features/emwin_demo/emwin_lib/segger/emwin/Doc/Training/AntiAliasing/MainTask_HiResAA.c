#include "GUI.h"

#define TIME_MIN         20
#define TIME_PER_TURN 40000
#define DURATION           (TIME_PER_TURN / 16)

typedef struct {
  void (* pFunc)(const GUI_POINT * pPoints, int NumPoints, int x0, int y0);
  int Factor;
} PARA;

static GUI_POINT _aPoint[] = {
  {  -5,  -5 },  {   0, -50 },  {   5,  -5 },  {  50,   0 },  
  {   5,   5 },  {   0,  50 },  {  -5,   5 },  { -50,   0 },
};

static void _MagnifyPoints(const GUI_POINT * pPointSrc, GUI_POINT * pPointDest, int NumPoints, int Factor) {
  int i;
  for (i = 0; i < NumPoints; i++) {
    (pPointDest + i)->x = (pPointSrc + i)->x * Factor;
    (pPointDest + i)->y = (pPointSrc + i)->y * Factor;
  }
}

static void _Loop(void (* pFunc)(const GUI_POINT * pPoints, int NumPoints, int x0, int y0), int Factor, GUI_MEMDEV_Handle hMem, int x, int y) {
  GUI_POINT aPoint   [GUI_COUNTOF(_aPoint)];
  GUI_POINT aPointRot[GUI_COUNTOF(_aPoint)];
  int TimeStart, Time0, TimeUsed;
  float a;

  GUI_AA_SetFactor(Factor);
  _MagnifyPoints(_aPoint, aPoint, GUI_COUNTOF(_aPoint), Factor);
  TimeStart = GUI_GetTime();
  do {
    Time0 = GUI_GetTime();
    a = (float)(Time0 - TimeStart) / TIME_PER_TURN * 3.1415f * 2;
    GUI_RotatePolygon(aPointRot, aPoint, GUI_COUNTOF(_aPoint), a);
    GUI_Clear();
    GUI_DispStringHCenterAt("Factor: ", x - 5, 10);
    GUI_DispDecMin(Factor);
    pFunc(aPointRot, GUI_COUNTOF(_aPoint), x * Factor, y * Factor);
    GUI_MEMDEV_CopyToLCD(hMem);
    TimeUsed = GUI_GetTime() - Time0;
    if (TIME_MIN > TimeUsed) {
      GUI_X_Delay(TIME_MIN - TimeUsed);
    }
  } while ((Time0 - TimeStart) < DURATION);
}

void MainTask(void) {
  int xSize, ySize, i;
  GUI_MEMDEV_Handle hMem;
  PARA aPara[] = {
    {(void (*)(const GUI_POINT * pPoints, int NumPoints, int x0, int y0))GUI_FillPolygon,    1},
    {(void (*)(const GUI_POINT * pPoints, int NumPoints, int x0, int y0))GUI_AA_FillPolygon, 2},
    {(void (*)(const GUI_POINT * pPoints, int NumPoints, int x0, int y0))GUI_AA_FillPolygon, 4},
    {(void (*)(const GUI_POINT * pPoints, int NumPoints, int x0, int y0))GUI_AA_FillPolygon, 8},
  };

  GUI_Init();
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  hMem = GUI_MEMDEV_Create(0, 0, xSize, ySize);
  GUI_MEMDEV_Select(hMem);
  GUI_AA_EnableHiRes();
  while (1) {
    for (i = 0; i < GUI_COUNTOF(aPara); i++) {
      _Loop(aPara[i].pFunc, aPara[i].Factor, hMem, xSize / 2, ySize / 2);
    }
  }
}
