#include "frame_setting_wallpaper.h"

void sw_wallpapers_cb(epdgui_args_vector_t &args)
{
    SetWallpaper(*((uint32_t*)(args[0])));
}

Frame_Setting_Wallpaper::Frame_Setting_Wallpaper(void)
{
    _frame_name = "Frame_Setting_Wallpaper";

    _sw_mutex_group = new EPDGUI_MutexSwitch();

    for(int i = 0; i < WALLPAPER_NUM; i++)
    {
        _sw_wallpapers[i] = new EPDGUI_Switch(2, 4, 100 + i * 60, 532, 61);
        _sw_mutex_group->Add(_sw_wallpapers[i]);
        _sw_wallpapers[i]->SetLabel(0, GetWallpaperName(i));
        _sw_wallpapers[i]->SetLabel(1, GetWallpaperName(i));
        _sw_wallpapers[i]->Canvas(1)->ReverseColor();
        _sw_wallpapers[i]->SetID(i);
        _sw_wallpapers[i]->AddArgs(1, 0, (void*)(_sw_wallpapers[i]->GetID_P()));
        _sw_wallpapers[i]->Bind(1, &sw_wallpapers_cb);
    }
    _sw_wallpapers[GetWallpaperID()]->setState(1);

    uint8_t language = GetLanguage();
    if(language == LANGUAGE_JA)
    {
        exitbtn("設定");
        _canvas_title->drawString("壁紙", 270, 34);
    }
    else if(language == LANGUAGE_ZH)
    {
        exitbtn("设置");
        _canvas_title->drawString("壁纸", 270, 34);
    }
    else
    {
        exitbtn("Setting");
        _canvas_title->drawString("Wallpaper", 270, 34);
    }
    
    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}

Frame_Setting_Wallpaper::~Frame_Setting_Wallpaper(void)
{
    for(int i = 0; i < WALLPAPER_NUM; i++)
    {
        delete _sw_wallpapers[i];
    }
    delete _sw_mutex_group;
}

int Frame_Setting_Wallpaper::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_sw_mutex_group);
    EPDGUI_AddObject(_key_exit);
    return 2;
}