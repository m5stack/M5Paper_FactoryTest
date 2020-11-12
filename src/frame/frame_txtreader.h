#ifndef _FRAME_TXTREADER_H_
#define _FRAME_TXTREADER_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"
#include "SD.h"

class Frame_txtReader : public Frame_Base
{
public:
    Frame_txtReader(String path);
    ~Frame_txtReader();
    int init(epdgui_args_vector_t &args);
    uint32_t renderText(uint32_t cursor, uint32_t length, M5EPD_Canvas *canvas);
    int run();

private:
    EPDGUI_Button *_key_next;
    EPDGUI_Button *_key_prev;
    uint8_t _language;
    uint16_t _text_size = 32;
    M5EPD_Canvas *_canvas_prev;
    M5EPD_Canvas *_canvas_current;
    M5EPD_Canvas *_canvas_next;
    M5EPD_Canvas *_canvas_page;
    uint32_t _cursor_prev = 0;
    uint32_t _cursor_current = 0;
    uint32_t _cursor_next = 0;
    uint32_t _render_len = 1000;
    uint32_t _page = 0;
    uint32_t _last_page = 0xFFFFFFFF;
    String _txt_path;
    File _txt_file;
    std::map<uint32_t, uint32_t> _page_cursor;
    uint32_t _page_end = 0;
    bool _end_accessed = false;
    bool _is_first = true;
};

#endif //_FRAME_TXTREADER_H_