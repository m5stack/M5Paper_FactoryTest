#include "epdgui_base.h"

EPDGUI_Base::EPDGUI_Base(int16_t x, int16_t y, int16_t w, int16_t h)
{
    x = (x + 3) & 0xFFFC;
    w = (w + 3) & 0xFFFC;
    this->_x = x;
    this->_y = y;
    this->_w = w;
    this->_h = h;
    _rx = _x + _w;
    _by = _y + _h;
}

EPDGUI_Base::EPDGUI_Base()
{
}

bool EPDGUI_Base::isSelected(void)
{
    return _issel;
}

bool EPDGUI_Base::isInBox(int16_t x, int16_t y)
{
    if (x == -1 || y == -1)
    {
        return false;
    }

    if ((x > _x) && (x < _rx) && (y > _y) && (y < _by))
    {
        _issel = true;
        return 1;
    }
    _issel = false;
    return 0;
}

void EPDGUI_Base::SetHide(bool ishide)
{
    this->_ishide = ishide;
}

void EPDGUI_Base::SetEnable(bool isenable)
{
    this->_isenable = isenable;
}

void EPDGUI_Base::SetGeometry(int16_t x, int16_t y, int16_t w, int16_t h)
{
    x = (x + 3) & 0xFFFC;
    w = (w + 3) & 0xFFFC;
    this->_x = x;
    this->_y = y;
    this->_w = w;
    this->_h = h;
}

void EPDGUI_Base::SetPos(int16_t x, int16_t y)
{
    x = (x + 3) & 0xFFFC;
    this->_x = x;
    this->_y = y;
}

void EPDGUI_Base::UpdateGram(m5epd_update_mode_t mode)
{
    M5.EPD.UpdateArea(_x, _y, _w, _h, mode);
}
