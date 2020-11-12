#ifndef _FRAME_COMPARE_H_
#define _FRAME_COMPARE_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_Compare : public Frame_Base
{
public:
    Frame_Compare();
    ~Frame_Compare();
    int init(epdgui_args_vector_t &args);
    int run();

private:
    EPDGUI_Button *_key_updatemode[8];
    uint8_t _update_flag;
    M5EPD_Canvas *_canvas;
    M5EPD_Canvas *_canvas_time;
};

#endif //_FRAME_COMPARE_H_