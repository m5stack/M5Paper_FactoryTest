#include "epdgui_button.h"

EPDGUI_Button::EPDGUI_Button(int16_t x, int16_t y, int16_t w, int16_t h): 
EPDGUI_Base(x, y, w, h)
{
    this->_CanvasNormal = new M5EPD_Canvas(&M5.EPD);
    this->_CanvasPressed = new M5EPD_Canvas(&M5.EPD);
    this->_CanvasNormal->createCanvas(_w, _h);
    this->_CanvasPressed->createCanvas(_w, _h);
}

EPDGUI_Button::EPDGUI_Button(String label, int16_t x, int16_t y, int16_t w, int16_t h, uint32_t style): 
EPDGUI_Base(x, y, w, h)
{
    if(style & STYLE_INVISABLE)
    {
        _is_invisable = true;
        return;
    }
    
    this->_label = label;
    
    this->_CanvasNormal = new M5EPD_Canvas(&M5.EPD);
    this->_CanvasPressed = new M5EPD_Canvas(&M5.EPD);

    // log_d("[%s] %d, %d", label.c_str(), _x, _y);
    
    this->_CanvasNormal->createCanvas(_w, _h);
    this->_CanvasPressed->createCanvas(_w, _h);

    this->_CanvasNormal->fillCanvas(0);
    this->_CanvasNormal->setTextSize(26);
    this->_CanvasNormal->setTextColor(15);

    this->_CanvasPressed->fillCanvas(15);
    this->_CanvasPressed->setTextSize(26);
    this->_CanvasPressed->setTextColor(0);
    if(style & STYLE_SOLIDBORDER)
    {
        this->_CanvasNormal->drawRect(0, 0, _w, _h, 15);
    }
    
    if(style & STYLE_ALIGN_LEFT)
    {
        this->_CanvasNormal->setTextDatum(CL_DATUM);
        this->_CanvasPressed->setTextDatum(CL_DATUM);
        this->_CanvasNormal->drawString(_label,  5, _h / 2 + 3);
        this->_CanvasPressed->drawString(_label, 5, _h / 2 + 3);
    }
    else if(style & STYLE_ALIGN_RIGHT)
    {
        this->_CanvasNormal->setTextDatum(CR_DATUM);
        this->_CanvasPressed->setTextDatum(CR_DATUM);
        this->_CanvasNormal->drawString(_label,  _w - 5, _h / 2 + 3);
        this->_CanvasPressed->drawString(_label, _w - 5, _h / 2 + 3);
    }
    else if(style & STYLE_ALIGN_CENTER)
    {
        this->_CanvasNormal->setTextDatum(CC_DATUM);
        this->_CanvasPressed->setTextDatum(CC_DATUM);
        this->_CanvasNormal->drawString(_label,  _w / 2, _h / 2 + 3);
        this->_CanvasPressed->drawString(_label, _w / 2, _h / 2 + 3);
    }
}

EPDGUI_Button::~EPDGUI_Button()
{
    delete this->_CanvasNormal;
    delete this->_CanvasPressed;
}

M5EPD_Canvas* EPDGUI_Button::CanvasNormal()
{
    return this->_CanvasNormal;
}

M5EPD_Canvas* EPDGUI_Button::CanvasPressed()
{
    return this->_CanvasPressed;
}

void EPDGUI_Button::Draw(m5epd_update_mode_t mode)
{
    if(_ishide || _is_invisable)
    {
        return;
    }

    if(_state == EVENT_NONE || _state == EVENT_RELEASED)
    {
        this->_CanvasNormal->pushCanvas(_x, _y, mode);
    }
    else if(_state == EVENT_PRESSED)
    {
        this->_CanvasPressed->pushCanvas(_x, _y, mode);
    }
}

void EPDGUI_Button::Draw(M5EPD_Canvas* canvas)
{
    if(_ishide)
    {
        return;
    }

    if(_state == EVENT_NONE || _state == EVENT_RELEASED)
    {
        _CanvasNormal->pushToCanvas(_x, _y, canvas);
    }
    else if(_state == EVENT_PRESSED)
    {
        _CanvasPressed->pushToCanvas(_x, _y, canvas);
    }
}

void EPDGUI_Button::Bind(int16_t event, void (* func_cb)(epdgui_args_vector_t&))
{
    if(event == EVENT_PRESSED)
    {
        _pressed_cb = func_cb;
    }
    else if(event == EVENT_RELEASED)
    {
        _released_cb = func_cb;
    }
}

void EPDGUI_Button::UpdateState(int16_t x, int16_t y)
{
    if(!_isenable || _ishide)
    {
        return;
    }

    bool is_in_area = isInBox(x, y);

    if(is_in_area)
    {
        if(_state == EVENT_NONE)
        {
            _state = EVENT_PRESSED;
            // Serial.printf("%s Pressed ", _label.c_str());
            Draw();
            if(_pressed_cb != NULL)
            {
                _pressed_cb(_pressed_cb_args);
            }
        }
    }
    else
    {
        if(_state == EVENT_PRESSED)
        {
            _state = EVENT_NONE;
            Draw();
            if(_released_cb != NULL)
            {
                _released_cb(_released_cb_args);
            }
        }
    }
}

void EPDGUI_Button::setBMPButton(String label_l, String label_r, const uint8_t *bmp32x32)
{
    _CanvasNormal->fillCanvas(0);
    _CanvasNormal->drawRect(0, 0, _w, _h, 15);
    _CanvasNormal->setTextSize(26);
    _CanvasNormal->setTextColor(15);
    if(label_l.length())
    {
        _CanvasNormal->setTextDatum(CL_DATUM);
        _CanvasNormal->drawString(label_l, 47 + 8, (_h >> 1) + 5);
    }
    if(label_r.length())
    {
        _CanvasNormal->setTextDatum(CR_DATUM);
        _CanvasNormal->drawString(label_r, _w - 15, (_h >> 1) + 5);
    }
    _CanvasNormal->pushImage(15, (_h >> 1) - 16, 32, 32, bmp32x32);
    *(_CanvasPressed) = *(_CanvasNormal);
    _CanvasPressed->ReverseColor();
}

void EPDGUI_Button::setLabel(String label)
{
    _label = label;
    this->_CanvasNormal->fillCanvas(0);
    this->_CanvasNormal->drawRect(0, 0, _w, _h, 15);
    this->_CanvasNormal->setTextSize(26);
    this->_CanvasNormal->setTextDatum(CC_DATUM);
    this->_CanvasNormal->setTextColor(15);
    this->_CanvasNormal->drawString(_label,  _w / 2, _h / 2 + 3);

    this->_CanvasPressed->fillCanvas(15);
    this->_CanvasPressed->setTextSize(26);
    this->_CanvasPressed->setTextDatum(CC_DATUM);
    this->_CanvasPressed->setTextColor(0);
    this->_CanvasPressed->drawString(_label, _w / 2, _h / 2 + 3);
}

void EPDGUI_Button::AddArgs(int16_t event, uint16_t n, void* arg)
{
    if(event == EVENT_PRESSED)
    {
        if(_pressed_cb_args.size() > n)
        {
            _pressed_cb_args[n] = arg;
        }
        else
        {
            _pressed_cb_args.push_back(arg);
        }
    }
    else if(event == EVENT_RELEASED)
    {
        if(_released_cb_args.size() > n)
        {
            _released_cb_args[n] = arg;
        }
        else
        {
            _released_cb_args.push_back(arg);
        }
    }
}
