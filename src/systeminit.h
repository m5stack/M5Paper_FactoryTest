#ifndef __SYSTEMINIT_H
#define __SYSTEMINIT_H

#include <M5EPD.h>

void SysInit_Start(void);
void SysInit_Loading(void *pvParameters);
void SysInit_UpdateInfo(String info);


#endif //__SYSTEMINIT_H