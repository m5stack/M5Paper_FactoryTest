#include "frame_txtreader.h"

void key_txtreader_exit_cb(epdgui_args_vector_t &args)
{
    EPDGUI_PopFrame(true);
    *((int*)(args[0])) = 0;
}

int8_t _key_operation = 0;
void key_nextpage_released_cb(epdgui_args_vector_t &args)
{
    _key_operation = 1;
    // *((int8_t*)(args[0])) = 1;
}

void key_prevpage_released_cb(epdgui_args_vector_t &args)
{
    _key_operation = -1;
    // *((int8_t*)(args[0])) = -1;
}

Frame_txtReader::Frame_txtReader(String path)
{
    _frame_name = "Frame_txtReader";
    _txt_path = path;
    _text_size = GetTextSize();

    _canvas_prev = new M5EPD_Canvas(&M5.EPD);
    _canvas_current = new M5EPD_Canvas(&M5.EPD);
    _canvas_next = new M5EPD_Canvas(&M5.EPD);
    _canvas_page = new M5EPD_Canvas(&M5.EPD);

    _key_next = new EPDGUI_Button("", 270, 72, 270, 888, EPDGUI_Button::STYLE_INVISABLE);
    _key_prev = new EPDGUI_Button("", 0, 72, 270, 888, EPDGUI_Button::STYLE_INVISABLE);
    // _key_next->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, &_key_operation);
    _key_next->Bind(EPDGUI_Button::EVENT_RELEASED, key_nextpage_released_cb);
    // _key_prev->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, &_key_operation);
    _key_prev->Bind(EPDGUI_Button::EVENT_RELEASED, key_prevpage_released_cb);

    _language = GetLanguage();
    if(_language == LANGUAGE_JA)
    {
        exitbtn("戻る");
    }
    else if(_language == LANGUAGE_ZH)
    {
        exitbtn("返回");
    }
    else
    {
        exitbtn("Back");
    }

    _canvas_title->drawString(path.substring(path.lastIndexOf("/") + 1, path.lastIndexOf(".")), 270, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &key_txtreader_exit_cb);
}

Frame_txtReader::~Frame_txtReader(void)
{
    if(_text_size != 26)
    {
        _canvas_prev->destoryRender(_text_size);
    }
    delete _canvas_prev;
    delete _canvas_current;
    delete _canvas_next;
    delete _canvas_page;
    delete _key_next;
    delete _key_prev;
    _txt_file.close();
}

uint32_t Frame_txtReader::renderText(uint32_t cursor, uint32_t length, M5EPD_Canvas *canvas)
{
    canvas->fillCanvas(0);
    canvas->setTextArea(10, 10, 520, 868);
    canvas->setTextSize(_text_size);

    char buf[length + 1];
    _txt_file.seek(cursor, SeekSet);
    uint32_t readlen = _txt_file.readBytes(buf, length);
    buf[readlen] = '\0';
    canvas->print(buf);
    return canvas->getExceedOffset();
}

int Frame_txtReader::run()
{
    Frame_Base::run();
    if(_is_first)
    {
        LoadingAnime_32x32_Start(254, 500);
        _is_first = false;
        uint32_t cursor;
        _page_cursor.insert(std::pair<uint32_t, uint32_t>(0, 0));
        cursor = renderText(0, _render_len, _canvas_current);
        if(cursor == 0)
        {
            _page_end = 0;
            _end_accessed = true;
        }
        else
        {
            _page_end = _page + 1;
            _page_cursor.insert(std::pair<uint32_t, uint32_t>(1, cursor));
            uint32_t offset = renderText(_page_cursor[1], _render_len, _canvas_next);
            if(offset == 0)
            {
                _page_end = 1;
                _end_accessed = true;
            }
            else
            {
                _page_cursor.insert(std::pair<uint32_t, uint32_t>(2, cursor + offset));
            }
        }
        LoadingAnime_32x32_Stop();
        _canvas_current->pushCanvas(0, 72, UPDATE_MODE_GC16);
    }
    else
    {
        M5.update();
        if(M5.BtnR.wasReleased() || (_key_operation == 1))
        {
            EPDGUI_UpdateGlobalLastActiveTime();
            _key_operation = 0;
            if(_page != _page_end)
            {
                _page++;
                _canvas_next->pushCanvas(0, 72, UPDATE_MODE_GC16);
                memcpy(_canvas_prev->frameBuffer(), _canvas_current->frameBuffer(), _canvas_current->getBufferSize());
                memcpy(_canvas_current->frameBuffer(), _canvas_next->frameBuffer(), _canvas_next->getBufferSize());

                if((_end_accessed == false) || (_page != _page_end))
                {
                    uint32_t offset = renderText(_page_cursor[_page + 1], _render_len, _canvas_next);

                    if(offset != 0)
                    {
                        if(_page_cursor.count(_page + 2) == 0)
                        {
                            _page_cursor.insert(std::pair<uint32_t, uint32_t>(_page + 2, _page_cursor[_page + 1] + offset));
                        }
                    }
                    else if(_end_accessed == false)
                    {
                        _page_end = _page + 1;
                        _end_accessed = true;
                    }
                    if(!_end_accessed)
                    {
                        _page_end = _page + 1;
                    }
                }
            }
        }
        else if(M5.BtnL.wasReleased() || (_key_operation == -1))
        {
            EPDGUI_UpdateGlobalLastActiveTime();
            _key_operation = 0;
            if(_page > 0)
            {
                _page--;
                _canvas_prev->pushCanvas(0, 72, UPDATE_MODE_GC16);
                memcpy(_canvas_next->frameBuffer(), _canvas_current->frameBuffer(), _canvas_current->getBufferSize());
                memcpy(_canvas_current->frameBuffer(), _canvas_prev->frameBuffer(), _canvas_prev->getBufferSize());
                if(_page != 0)
                {
                    renderText(_page_cursor[_page - 1], _render_len, _canvas_prev);
                }
            }
        }
    }
    if(_last_page != _page)
    {
        _last_page = _page;
        _canvas_page->setTextSize(26);
        _canvas_page->fillCanvas(0);
        char buf[5];
        sprintf(buf, "%d", _page + 1);
        _canvas_page->drawString(buf, 100, 42);
        _canvas_page->pushCanvas(440 - 15, 0, UPDATE_MODE_GL16);
    }
    return 1;
}

int Frame_txtReader::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    _canvas_prev->createCanvas(540, 888);
    _canvas_current->createCanvas(540, 888);
    _canvas_next->createCanvas(540, 888);
    _canvas_page->createCanvas(100, 60);
    _canvas_page->setTextDatum(CR_DATUM);
    if(!_canvas_prev->isRenderExist(_text_size))
    {
        _canvas_prev->createRender(_text_size, 128);
    }
    EPDGUI_AddObject(_key_exit);
    EPDGUI_AddObject(_key_next);
    EPDGUI_AddObject(_key_prev);

    _txt_file = SD.open(_txt_path);
    _last_page = 0;
    return 3;
}


