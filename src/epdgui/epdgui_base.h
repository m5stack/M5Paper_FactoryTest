#ifndef __EPDGUI_BASE_H
#define __EPDGUI_BASE_H

#include <M5EPD.h>
#include <vector>
#include "../global_setting.h"
#include "../resources/ImageResource.h"

typedef std::vector<void*> epdgui_args_vector_t;

class EPDGUI_Base
{
public:
    EPDGUI_Base(int16_t x, int16_t y, int16_t w, int16_t h);
    EPDGUI_Base();
    virtual ~EPDGUI_Base() {};
    virtual void Draw(m5epd_update_mode_t mode) = 0;
    virtual void Bind(int16_t event, void (* func_cb)(epdgui_args_vector_t&)) = 0;
    virtual void UpdateState(int16_t x, int16_t y) = 0;
    virtual void Draw(M5EPD_Canvas* canvas) = 0;

    bool isInBox(int16_t x, int16_t y);
    virtual bool isSelected(void);
    void SetHide(bool ishide);
    void SetEnable(bool isenable);
    void SetGeometry(int16_t x, int16_t y, int16_t w, int16_t h);
    void SetPos(int16_t x, int16_t y);
    void UpdateGram(m5epd_update_mode_t mode = UPDATE_MODE_DU4);
    uint32_t GetID(void) { return _id;}
    uint32_t* GetID_P(void) { return &_id;}
    void SetID(uint32_t id) { _id = id;}
    void SetCustomString(String s) {_user_custom_str = s;}
    String GetCustomString(void) {return _user_custom_str;}

    inline int16_t getX() {return _x;}
    inline int16_t getY() {return _y;}
    inline int16_t getW() {return _w;}
    inline int16_t getH() {return _h;}
    inline int16_t getRX() {return _rx;}
    inline int16_t getBY() {return _by;}

protected:
    String _user_custom_str;
    int16_t _x, _y, _w, _h;
    int16_t _rx, _by;
    uint32_t _id;
    bool _issel = false;
    bool _ishide = false;
    bool _isenable = true;
};


#endif //__EPDGUI_BASE_H