#include "frame_setting_language.h"


void sw_en_cb(epdgui_args_vector_t &args)
{
    SetLanguage(LANGUAGE_EN);
}

void sw_zh_cb(epdgui_args_vector_t &args)
{
    SetLanguage(LANGUAGE_ZH);
}

void sw_ja_cb(epdgui_args_vector_t &args)
{
    SetLanguage(LANGUAGE_JA);
}

Frame_Setting_Language::Frame_Setting_Language(void)
{
    _frame_name = "Frame_Setting_Language";

    _sw_en = new EPDGUI_Switch(2, 4, 100, 532, 61);
    _sw_zh = new EPDGUI_Switch(2, 4, 160, 532, 61);
    _sw_ja = new EPDGUI_Switch(2, 4, 220, 532, 61);

    if(isTTFLoaded())
    {
        _sw_en->SetLabel(0, "English");
        _sw_en->SetLabel(1, "English");
        _sw_en->Canvas(1)->ReverseColor();
        _sw_en->Bind(1, &sw_en_cb);
        _sw_zh->SetLabel(0, "中文");
        _sw_zh->SetLabel(1, "中文");
        _sw_zh->Canvas(1)->ReverseColor();
        _sw_zh->Bind(1, &sw_zh_cb);
        _sw_ja->SetLabel(0, "日本語");
        _sw_ja->SetLabel(1, "日本語");
        _sw_ja->Canvas(1)->ReverseColor();
        _sw_ja->Bind(1, &sw_ja_cb);
    }
    else
    {
        _sw_en->SetLabel(0, "English");
        _sw_en->SetLabel(1, "English");
        _sw_en->Canvas(1)->ReverseColor();
        _sw_en->Bind(1, &sw_en_cb);
        _sw_zh->SetLabel(0, "Chinese (Need .ttf file)");
        _sw_zh->SetLabel(1, "Chinese (Need .ttf file)");
        _sw_zh->Canvas(1)->ReverseColor();
        _sw_zh->Bind(1, &sw_zh_cb);
        _sw_ja->SetLabel(0, "Japanese (Need .ttf file)");
        _sw_ja->SetLabel(1, "Japanese (Need .ttf file)");
        _sw_ja->Canvas(1)->ReverseColor();
        _sw_ja->Bind(1, &sw_ja_cb);
    }

    _sw_mutex_group = new EPDGUI_MutexSwitch();
    _sw_mutex_group->Add(_sw_en);
    _sw_mutex_group->Add(_sw_zh);
    _sw_mutex_group->Add(_sw_ja);
 
    uint8_t language = GetLanguage();
    if(language == LANGUAGE_JA)
    {
        exitbtn("設定");
        _canvas_title->drawString("言語", 270, 34);
        _sw_ja->setState(1);
    }
    else if(language == LANGUAGE_ZH)
    {
        exitbtn("设置");
        _canvas_title->drawString("语言", 270, 34);
        _sw_zh->setState(1);
    }
    else
    {
        exitbtn("Setting");
        _canvas_title->drawString("Language", 270, 34);
        _sw_en->setState(1);
    }
    
    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}

Frame_Setting_Language::~Frame_Setting_Language(void)
{
    delete _sw_en;
    delete _sw_zh;
    delete _sw_ja;
    delete _sw_mutex_group;
}

int Frame_Setting_Language::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_sw_mutex_group);
    EPDGUI_AddObject(_key_exit);
    return 2;
}