#ifndef __EPDGUI_H
#define __EPDGUI_H

#include "epdgui_base.h"
#include "epdgui_button.h"
#include "epdgui_switch.h"
#include "epdgui_keyboard.h"
#include "epdgui_textbox.h"
#include "epdgui_mutexswitch.h"
#include "../frame/frame_base.h"

void EPDGUI_AddObject(EPDGUI_Base* object);
void EPDGUI_Draw(m5epd_update_mode_t mode = UPDATE_MODE_GC16);
void EPDGUI_Process(void);
void EPDGUI_Process(int16_t x, int16_t y);
void EPDGUI_Clear(void);
void EPDGUI_Run(Frame_Base* frame);
void EPDGUI_MainLoop(void);
void EPDGUI_PushFrame(Frame_Base* frame);
void EPDGUI_PopFrame(bool isDelete = false);
void EPDGUI_OverwriteFrame(Frame_Base* frame);
void EPDGUI_AddFrame(String name, Frame_Base* frame);
void EPDGUI_AddFrameArg(String name, int n, void* arg);
Frame_Base* EPDGUI_GetFrame(String name);
void EPDGUI_SetAutoUpdate(bool isAuto);

#endif //__EPDGUI_H