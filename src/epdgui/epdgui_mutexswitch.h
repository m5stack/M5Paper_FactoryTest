#ifndef _EPDGUI_MUTEXSWITCH_H
#define _EPDGUI_MUTEXSWITCH_H

#include "epdgui_switch.h"
#include <list>

class EPDGUI_MutexSwitch : public EPDGUI_Base
{
public:
    EPDGUI_MutexSwitch();
    
    void Draw(m5epd_update_mode_t mode);
    void Draw(M5EPD_Canvas* canvas);
    void Bind(int16_t event, void (* func_cb)(epdgui_args_vector_t&));
    void UpdateState(int16_t x, int16_t y);

    void Add(EPDGUI_Switch* object) {_object_list.push_back(object);}
    void SetExclusive(bool isExclusive) {_is_exclusive = isExclusive;}
    void SetDefault(uint16_t idx);

private:
    bool _is_exclusive = true;
    uint16_t _default_idx = 0;
    std::list<EPDGUI_Switch*> _object_list;
    std::list<EPDGUI_Switch*>::iterator _last_pressed;
};


#endif

