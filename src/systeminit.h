#ifndef __SYSTEMINIT_H
#define __SYSTEMINIT_H

#include <M5EPD.h>

void SysInit_Start(void);
void SysInit_Loading(void *pvParameters);
void SysInit_UpdateInfo(M5EPD_Canvas &Info, String str);

#endif  //__SYSTEMINIT_H