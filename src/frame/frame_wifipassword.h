#ifndef _FRAME_WIFIPASSWORD_H_
#define _FRAME_WIFIPASSWORD_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_WifiPassword : public Frame_Base
{
public:
    Frame_WifiPassword(bool isHorizontal = false);
    ~Frame_WifiPassword();
    int run();
    int init(epdgui_args_vector_t &args);

private:
    EPDGUI_Textbox *inputbox;
    EPDGUI_Keyboard *keyboard;
    EPDGUI_Button *key_textclear;
};

#endif //_FRAME_WIFIPASSWORD_H_