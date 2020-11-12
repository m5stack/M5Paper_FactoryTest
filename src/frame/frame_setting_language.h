#ifndef _FRAME_LANGUAGE_H_
#define _FRAME_LANGUAGE_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_Setting_Language : public Frame_Base
{
public:
    Frame_Setting_Language();
    ~Frame_Setting_Language();
    int init(epdgui_args_vector_t &args);

private:
    EPDGUI_Switch* _sw_en;
    EPDGUI_Switch* _sw_zh;
    EPDGUI_Switch* _sw_ja;
    EPDGUI_MutexSwitch* _sw_mutex_group;
};

#endif //_FRAME_LANGUAGE_H_