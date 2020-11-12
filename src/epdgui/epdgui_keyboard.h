#ifndef __EPDGUI_KEYBOARD_H
#define __EPDGUI_KEYBOARD_H

#include "epdgui_button.h"
#include "epdgui_switch.h"

class EPDGUI_Keyboard: public EPDGUI_Base
{
public:
    static const uint32_t STYLE_INPUTMODE_NORMALTEXT = 0x00000001;
    static const uint32_t STYLE_INPUTMODE_NEEDCONFIRM = 0x00000002;
    static const uint32_t STYLE_DEFAULT = STYLE_INPUTMODE_NORMALTEXT;

public:
    EPDGUI_Keyboard(int16_t dir = 1, uint32_t style = STYLE_DEFAULT);
    ~EPDGUI_Keyboard();
    void Draw(m5epd_update_mode_t mode = UPDATE_MODE_DU4);
    void Draw(M5EPD_Canvas* canvas);
    void Bind(int16_t event, void (* func_cb)(epdgui_args_vector_t&));
    void UpdateState(int16_t x, int16_t y);
    String getData(void);

private:
    EPDGUI_Button *_btn[29];
    EPDGUI_Switch *_sw[3];
    EPDGUI_Base *_key[32];
    String _data;
    uint8_t _layout = 0;

public:

};


#endif //__EPDGUI_KEYBOARD_H