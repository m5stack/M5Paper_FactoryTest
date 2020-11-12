#include "epdgui_switch.h"

EPDGUI_Switch::EPDGUI_Switch(int16_t state_num, int16_t x, int16_t y, int16_t w, int16_t h): 
EPDGUI_Base(x, y, w, h)
{
    if(state_num > EPDGUI_SWITCH_MAX_STATE)
    {
        state_num = EPDGUI_SWITCH_MAX_STATE;
    }
    this->_state_num = state_num;

    for(int i = 0; i < _state_num; i++)
    {
        this->_canvas[i] = new M5EPD_Canvas(&M5.EPD);
        this->_canvas[i]->createCanvas(_w, _h);
        // this->_canvas[i]->fillCanvas(i & 0x01 ? 0 : 15);
        this->_canvas[i]->setTextSize(26);
        this->_canvas[i]->fillCanvas(0);
        this->_canvas[i]->drawRect(0, 0, _w, _h, 15);
    }
    _canvas_pressed = new M5EPD_Canvas(&M5.EPD);
    _canvas_pressed->createCanvas(_w, _h);
    _canvas_pressed->fillCanvas(15);
}

EPDGUI_Switch::~EPDGUI_Switch()
{
    for(int i = 0; i < _state_num; i++)
    {
        delete this->_canvas[i];
    }
}

M5EPD_Canvas* EPDGUI_Switch::Canvas(int16_t state)
{
    if(state == -1)
    {
        return _canvas_pressed;
    }
    return _canvas[state];
}

void EPDGUI_Switch::SetLabel(int16_t state, String label)
{
    if(state > EPDGUI_SWITCH_MAX_STATE || state < 0)
    {
        return;
    }

    this->_canvas[state]->fillCanvas(0);
    this->_canvas[state]->drawRect(0, 0, _w, _h, 15);
    this->_canvas[state]->setTextSize(26);
    this->_canvas[state]->setTextDatum(CC_DATUM);
    // this->_canvas[state]->setTextColor(state & 0x01 ? 15 : 0);
    this->_canvas[state]->setTextColor(15);
    this->_canvas[state]->drawString(label,  _w / 2, _h / 2 + 5);
    this->_label_array[state] = label;
}

void EPDGUI_Switch::Draw(m5epd_update_mode_t mode)
{
    if(_ishide)
    {
        return;
    }

    if(_event == EVENT_PRESSED)
    {
        _canvas_pressed->pushCanvas(_x, _y, mode);
        return;
    }

    this->_canvas[_state]->pushCanvas(_x, _y, mode);
}

void EPDGUI_Switch::Draw(M5EPD_Canvas* canvas)
{
    if(_ishide)
    {
        return;
    }

    if(_event == EVENT_PRESSED)
    {
        _canvas_pressed->pushToCanvas(_x, _y, canvas);
        return;
    }

    this->_canvas[_state]->pushToCanvas(_x, _y, canvas);
}

void EPDGUI_Switch::Bind(int16_t state, void (* func_cb)(epdgui_args_vector_t&))
{
    if(state > EPDGUI_SWITCH_MAX_STATE || state < 0)
    {
        return;
    }
    
    this->_func_cb_array[state] = func_cb;
}

void EPDGUI_Switch::UpdateState(int16_t x, int16_t y)
{
    if(!_isenable || _ishide)
    {
        return;
    }

    bool is_in_area = isInBox(x, y);

    if(is_in_area)
    {
        if(_event == EVENT_NONE)
        {
            _event = EVENT_PRESSED;
            Draw();
        }
    }
    else
    {
        if(_event == EVENT_PRESSED)
        {
            _event = EVENT_NONE;
            _state++;
            if(_state == _state_num)
            {
                _state = 0;
            }

            Draw();
            if(this->_func_cb_array[_state] != NULL)
            {
                this->_func_cb_array[_state](this->_func_cb_param_array[_state]);
            }
        }
    }
}

void EPDGUI_Switch::setState(int16_t state)
{
    if(state >= _state_num || state < 0)
    {
        return;
    }
    _state = state;
    Draw(UPDATE_MODE_NONE);
}

int16_t EPDGUI_Switch::getState(void)
{
    return _state;
}

void EPDGUI_Switch::AddArgs(int16_t state, uint16_t n, void* arg)
{
    if(state > EPDGUI_SWITCH_MAX_STATE || state < 0)
    {
        return;
    }
    
    if(this->_func_cb_param_array[state].size() > n)
    {
        this->_func_cb_param_array[state][n] = arg;
    }
    else
    {
        this->_func_cb_param_array[state].push_back(arg);
    }
}