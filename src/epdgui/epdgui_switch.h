#ifndef __EPDGUI_SWITCH_H
#define __EPDGUI_SWITCH_H

#include "epdgui_base.h"

#define EPDGUI_SWITCH_MAX_STATE 5

class EPDGUI_Switch: public EPDGUI_Base
{
public:
    static const int16_t EVENT_NONE = 0;
    static const int16_t EVENT_PRESSED = 1;
    static const int16_t EVENT_RELEASED = 2;


public:
    EPDGUI_Switch(int16_t state_num, int16_t x, int16_t y, int16_t w, int16_t h);
    ~EPDGUI_Switch();
    void Draw(m5epd_update_mode_t mode = UPDATE_MODE_DU4);
    void Draw(M5EPD_Canvas* canvas);
    void Bind(int16_t state, void (* func_cb)(epdgui_args_vector_t&));
    void UpdateState(int16_t x, int16_t y);
    void AddArgs(int16_t state, uint16_t n, void* arg);

    int16_t getState(void);
    void setState(int16_t state);
    void SetLabel(int16_t state, String label);
    M5EPD_Canvas* Canvas(int16_t state);

private:
    int16_t _state_num;
    void (*_func_cb_array[EPDGUI_SWITCH_MAX_STATE])(epdgui_args_vector_t &args) = {NULL};
    epdgui_args_vector_t _func_cb_param_array[EPDGUI_SWITCH_MAX_STATE];
    int16_t _state = 0;
    int16_t _event = EVENT_NONE;
    String _label_array[EPDGUI_SWITCH_MAX_STATE];

public:
    M5EPD_Canvas* _canvas[EPDGUI_SWITCH_MAX_STATE] = {NULL};
    M5EPD_Canvas* _canvas_pressed = NULL;
};


#endif //__EPDGUI_BUTTON_H