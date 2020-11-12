#ifndef __EPDGUI_BUTTON_H
#define __EPDGUI_BUTTON_H

#include "epdgui_base.h"

class EPDGUI_Button: public EPDGUI_Base
{
public:
    static const int16_t EVENT_NONE = 0;
    static const int16_t EVENT_PRESSED = 1;
    static const int16_t EVENT_RELEASED = 2;

    static const uint32_t STYLE_BORDERLESS = 0x00000001;
    static const uint32_t STYLE_SOLIDBORDER = 0x00000002;
    static const uint32_t STYLE_ALIGN_LEFT = 0x00000004;
    static const uint32_t STYLE_ALIGN_RIGHT = 0x00000008;
    static const uint32_t STYLE_ALIGN_CENTER = 0x00000010;
    static const uint32_t STYLE_INVISABLE = 0x00000020;
    static const uint32_t STYLE_DEFAULT = STYLE_SOLIDBORDER | STYLE_ALIGN_CENTER;

public:
    EPDGUI_Button(int16_t x, int16_t y, int16_t w, int16_t h);
    EPDGUI_Button(String label, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t style = STYLE_DEFAULT);
    ~EPDGUI_Button();
    void Draw(m5epd_update_mode_t mode = UPDATE_MODE_DU4);
    void Draw(M5EPD_Canvas* canvas);
    void Bind(int16_t event, void (* func_cb)(epdgui_args_vector_t&));
    void UpdateState(int16_t x, int16_t y);
    void setLabel(String label);
    String getLabel(void) {return _label;}
    void AddArgs(int16_t event, uint16_t n, void* arg);
    void setBMPButton(String label_l, String label_r, const uint8_t *bmp32x32);

    M5EPD_Canvas* CanvasNormal();
    M5EPD_Canvas* CanvasPressed();

private:
    void (*_pressed_cb)(epdgui_args_vector_t& args) = NULL;
    void (*_released_cb)(epdgui_args_vector_t& args) = NULL;
    epdgui_args_vector_t _pressed_cb_args;
    epdgui_args_vector_t _released_cb_args;
    int16_t _state = EVENT_NONE;
    String _label;
    bool _is_invisable = false;

public:
    M5EPD_Canvas* _CanvasNormal = NULL;
    M5EPD_Canvas* _CanvasPressed = NULL;
};


#endif //__EPDGUI_BUTTON_H