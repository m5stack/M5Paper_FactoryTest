#ifndef __EPDGUI_TEXTBOX_H
#define __EPDGUI_TEXTBOX_H

#include "epdgui_base.h"

class EPDGUI_Textbox : public EPDGUI_Base
{
public:
    static const int16_t EVENT_NONE = 0;
    static const int16_t EVENT_PRESSED = 1;

public:
    EPDGUI_Textbox(int16_t x, int16_t y, int16_t w, int16_t h);
    ~EPDGUI_Textbox();
    void Draw(m5epd_update_mode_t mode = UPDATE_MODE_DU4);
    void Draw(M5EPD_Canvas* canvas);
    void Bind(int16_t event, void (*func_cb)(epdgui_args_vector_t &));
    void UpdateState(int16_t x, int16_t y);
    void SetState(int16_t state);
    void SetText(String text);
    String GetText(void) {return _data;}
    void AddText(String text);
    void Remove(int16_t idx);
    void SetTextSize(uint16_t size);
    bool isSelected(void) {return _state;}
    void SetTextMargin(int16_t left, int16_t top, int16_t right, int16_t bottom);

private:
    M5EPD_Canvas *_canvas = NULL;
    String _data;
    uint16_t _size;
    bool _thiscreat;
    int16_t _state;
    int16_t _margin_left, _margin_right, _margin_top, _margin_bottom;
    static uint32_t _textbox_touching_id;

public:
};

#endif //__EPDGUI_KEYBOARD_H