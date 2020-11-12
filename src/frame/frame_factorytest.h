#ifndef _FRAME_FACTORYTEST_H_
#define _FRAME_FACTORYTEST_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_FactoryTest : public Frame_Base
{
public:
    Frame_FactoryTest();
    ~Frame_FactoryTest();
    int run();
    void scan(String *ssid, int32_t *rssi);
    int init(epdgui_args_vector_t &args);

private:
    void drawItem(m5epd_update_mode_t mode);
    void drawItem(uint16_t flag, const char* str, int y);
    void drawGrove(m5epd_update_mode_t mode);
    bool checkGrove(int sda, int scl);
    void drawPassCount(m5epd_update_mode_t mode);


private:
    M5EPD_Canvas *_canvas_base;
    M5EPD_Canvas *_canvas_data;
    M5EPD_Canvas *_canvas_pos;
    M5EPD_Canvas *_canvas_btn;
    M5EPD_Canvas *_canvas_pass;
    uint8_t _language;
    uint32_t _time;
    uint8_t _btn;
    bool _psram_success;
    bool _isfirst;
    String _wifistr;
    uint32_t _last_x, _last_y;
    uint16_t _pass_flag;
    uint8_t _prev_sec;
    float _prev_temp;
    float _prev_hum;
    uint16_t _prev_vol;
};

#endif //_FRAME_FACTORYTEST_H_