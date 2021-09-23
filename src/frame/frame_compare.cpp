#include "frame_compare.h"

void UpdateCompareCanvasDraw(uint8_t mode, M5EPD_Canvas *update_canvas)
{
    update_canvas->fillCanvas(0);
    for(int i = 0; i < 16; i++)
    {
        update_canvas->fillRect(i * 27, 0, 27, 50, i);
    }
    switch(mode)
    {
        case UPDATE_MODE_INIT :
        {
            update_canvas->drawString("Display initialization", 8, 60);
            break;
        }
        case UPDATE_MODE_DU   :
        {
            update_canvas->drawString("Monochrome menu, text input ", 8, 60);
            break;
        }
        case UPDATE_MODE_GC16 :
        {
            update_canvas->drawString("High quality images", 8, 60);
            break;
        }
        case UPDATE_MODE_GL16 :
        {
            update_canvas->drawString("Text with white background", 8, 60);
            break;
        }
        case UPDATE_MODE_GLR16:
        {
            update_canvas->drawString("Text with white background", 8, 60);
            break;
        }
        case UPDATE_MODE_GLD16:
        {
            update_canvas->drawString("Graphics with white background", 8, 60);
            break;
        }
        case UPDATE_MODE_DU4  :
        {
            update_canvas->drawString("Fast page flipping", 8, 60);
            break;
        }
        case UPDATE_MODE_A2   :
        {
            update_canvas->drawString("Anti-aliased text in menus", 8, 60);
            break;
        }
    }
    update_canvas->drawRect(0, 0, 432, 100, 15);
}

void key_update_mode_cb(epdgui_args_vector_t &args)
{
    EPDGUI_Button *btn = (EPDGUI_Button*)(args[0]);
    M5EPD_Canvas *canvas = (M5EPD_Canvas*)(args[1]);
    M5EPD_Canvas *canvas_time = (M5EPD_Canvas*)(args[2]);
    int mode = btn->GetCustomString().toInt();
    UpdateCompareCanvasDraw(mode, canvas);
    canvas_time->fillCanvas(0);
    uint32_t time = millis();
    canvas->pushCanvas(104, 168 + (mode-1) * 108, (m5epd_update_mode_t)mode);
    M5.EPD.CheckAFSR();
    char buf[30];
    sprintf(buf, "%lu ms", millis() - time);
    canvas_time->drawString(buf, 200, 15);
    canvas_time->pushCanvas(330, 925, UPDATE_MODE_GL16);
}

void key_update_reset_cb(epdgui_args_vector_t &args)
{
    M5.EPD.FillPartGram4bpp(104, 168, 432, 748, 0xFFFF);
    M5.EPD.UpdateArea(104, 168, 432, 748, UPDATE_MODE_INIT);
}

Frame_Compare::Frame_Compare(void)
{
    _frame_name = "Frame_Compare";
    
    _canvas_time = new M5EPD_Canvas(&M5.EPD);
    _canvas = new M5EPD_Canvas(&M5.EPD);
    _canvas->createCanvas(432, 100);
    _canvas_time->createCanvas(200, 30);
    _canvas->setTextSize(26);
    _canvas_time->setTextSize(26);
    _canvas_time->setTextDatum(CR_DATUM);

    uint8_t language = GetLanguage();
    if(language == LANGUAGE_JA)
    {
        exitbtn("ホーム");
        _canvas_title->drawString("比較", 270, 34);
        _key_updatemode[UPDATE_MODE_INIT] = new EPDGUI_Button("リセット", 4, 88, 532, 60);
    }
    else if(language == LANGUAGE_ZH)
    {
        exitbtn("主页");
        _canvas_title->drawString("比较", 270, 34);
        _key_updatemode[UPDATE_MODE_INIT] = new EPDGUI_Button("全部重置", 4, 88, 532, 60);
    }
    else
    {
        exitbtn("Home");
        _canvas_title->drawString("Compare", 270, 34);
        _key_updatemode[UPDATE_MODE_INIT] = new EPDGUI_Button("Reset all", 4, 88, 532, 60);
    }

    _key_updatemode[UPDATE_MODE_INIT]->Bind(EPDGUI_Button::EVENT_RELEASED, key_update_reset_cb);

    for(int i = 1; i < 8; i++)
    {
        _key_updatemode[i] = new EPDGUI_Button(0, 168 + (i-1) * 108, 100, 100);
        char buf[10];
        sprintf(buf, "%d", i);
        _key_updatemode[i]->SetCustomString(buf);
        _key_updatemode[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_updatemode[i]);
        _key_updatemode[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _canvas);
        _key_updatemode[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 2, _canvas_time);
        _key_updatemode[i]->Bind(EPDGUI_Button::EVENT_RELEASED, key_update_mode_cb);
    }

    _key_updatemode[UPDATE_MODE_DU   ]->setLabel("DU");
    _key_updatemode[UPDATE_MODE_GC16 ]->setLabel("GC16");
    _key_updatemode[UPDATE_MODE_GL16 ]->setLabel("GL16");
    _key_updatemode[UPDATE_MODE_GLR16]->setLabel("GLR16");
    _key_updatemode[UPDATE_MODE_GLD16]->setLabel("GLD16");
    _key_updatemode[UPDATE_MODE_DU4  ]->setLabel("DU4");
    _key_updatemode[UPDATE_MODE_A2   ]->setLabel("A2");

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}

Frame_Compare::~Frame_Compare(void)
{
    for(int i = 0; i < 8; i++)
    {
        delete _key_updatemode[i];
    }
    delete _canvas;
}

int Frame_Compare::run()
{
    Frame_Base::run();
    switch(_update_flag)
    {
        case 1:
        {
            _update_flag = 0;
            for(int i = 1; i < 8; i++)
            {
                UpdateCompareCanvasDraw(i, _canvas);
                _canvas->pushCanvas(104, 168 + (i-1) * 108, (m5epd_update_mode_t)i);
            }
            break;
        }
    }
    return 1;
}

int Frame_Compare::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    _update_flag = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_exit);
    for(int i = 0; i < 8; i++)
    {
        EPDGUI_AddObject(_key_updatemode[i]);
    }
    EPDGUI_SetAutoUpdate(false);
    return 3;
}