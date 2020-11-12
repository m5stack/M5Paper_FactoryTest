#ifndef _FRAME_SETTING_H_
#define _FRAME_SETTING_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_Setting : public Frame_Base
{
public:
    Frame_Setting();
    ~Frame_Setting();
    int init(epdgui_args_vector_t &args);


private:
    EPDGUI_Button *_key_wallpaper;
    EPDGUI_Button *_key_language;
    EPDGUI_Button *_key_shutdown;
    EPDGUI_Button *_key_restart;
    EPDGUI_Button *_key_syncntp;
};

#endif //_FRAME_SETTING_H_