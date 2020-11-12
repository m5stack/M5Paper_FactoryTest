#ifndef _FRAME_HOME_H_
#define _FRAME_HOME_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_Home : public Frame_Base
{
public:
    Frame_Home();
    ~Frame_Home();
    int init(epdgui_args_vector_t &args);
    void InitSwitch(EPDGUI_Switch* sw, String title, String subtitle, const uint8_t *img1, const uint8_t *img2);
private:
    EPDGUI_Switch *_sw_light1;
    EPDGUI_Switch *_sw_light2;
    EPDGUI_Switch *_sw_socket1;
    EPDGUI_Switch *_sw_socket2;
    EPDGUI_Switch *_sw_air_1;
    EPDGUI_Switch *_sw_air_2;
    EPDGUI_Button *_key_air_1_plus;
    EPDGUI_Button *_key_air_1_minus;
    EPDGUI_Button *_key_air_2_plus;
    EPDGUI_Button *_key_air_2_minus;
};

#endif //_FRAME_HOME_H_