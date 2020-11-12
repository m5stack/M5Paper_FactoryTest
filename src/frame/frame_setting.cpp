#include "frame_setting.h"
#include "frame_setting_language.h"
#include "frame_setting_wallpaper.h"
#include "WiFi.h"

#define KEY_W 92
#define KEY_H 92

void key_shutdown_cb(epdgui_args_vector_t &args)
{
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    delay(600);
    M5.disableEPDPower();
    M5.disableEXTPower();
    M5.disableMainPower();
    esp_deep_sleep_start();
    while(1);
}

void key_restart_cb(epdgui_args_vector_t &args)
{
    esp_restart();
}

void key_language_cb(epdgui_args_vector_t &args)
{
    Frame_Base *frame = EPDGUI_GetFrame("Frame_Setting_Language");
    if(frame == NULL)
    {
        frame = new Frame_Setting_Language();
        EPDGUI_AddFrame("Frame_Setting_Language", frame);
    }
    EPDGUI_PushFrame(frame);
    *((int*)(args[0])) = 0;
}

void key_wallpaper_cb(epdgui_args_vector_t &args)
{
    Frame_Base *frame = EPDGUI_GetFrame("Frame_Setting_Wallpaper");
    if(frame == NULL)
    {
        frame = new Frame_Setting_Wallpaper();
        EPDGUI_AddFrame("Frame_Setting_Wallpaper", frame);
    }
    EPDGUI_PushFrame(frame);
    *((int*)(args[0])) = 0;
}

void key_synctime_cb(epdgui_args_vector_t &args)
{
    M5EPD_Canvas info(&M5.EPD);
    M5EPD_Canvas *title = (M5EPD_Canvas*)(args[0]);
    info.createCanvas(300, 100);
    info.fillCanvas(15);
    info.setTextSize(26);
    info.setTextColor(0);
    info.setTextDatum(CC_DATUM);
    uint8_t language = GetLanguage();
    if(WiFi.status() != WL_CONNECTED)
    {
        if(language == LANGUAGE_JA)
        {
            info.drawString("WLANが接続いません", 150, 55);
        }
        else if(language == LANGUAGE_ZH)
        {
            info.drawString("WLAN未连接", 150, 55);
        }
        else
        {
            info.drawString("WLAN not connected", 150, 55);
        }
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
        M5.EPD.WriteFullGram4bpp(GetWallpaper());
        title->pushCanvas(0, 8, UPDATE_MODE_NONE);
        EPDGUI_Draw(UPDATE_MODE_NONE);
        while(!M5.TP.avaliable());
        M5.EPD.UpdateFull(UPDATE_MODE_GL16);
        return;
    }
    LoadingAnime_32x32_Start(532 - 15 - 32, 220 + 14);
    bool ret = SyncNTPTime();
    LoadingAnime_32x32_Stop();

    if(ret == 0)
    {
        if(language == LANGUAGE_JA)
        {
            info.drawString("シンクロが失敗しました", 150, 55);
        }
        else if(language == LANGUAGE_ZH)
        {
            info.drawString("同步失败", 150, 55);
        }
        else
        {
            info.drawString("Time sync failed", 150, 55);
        }
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
    }
    else
    {
        if(language == LANGUAGE_JA)
        {
            info.drawString("成功", 150, 55);
        }
        else if(language == LANGUAGE_ZH)
        {
            info.drawString("成功", 150, 55);
        }
        else
        {
            info.drawString("Success", 150, 55);
        }
        info.pushCanvas(120, 430, UPDATE_MODE_GL16);
    }
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_Draw(UPDATE_MODE_NONE);
    while(!M5.TP.avaliable());
    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
}

Frame_Setting::Frame_Setting(void)
{
    _frame_name = "Frame_Setting";
 
    uint8_t language = GetLanguage();
    _key_wallpaper = new EPDGUI_Button(4, 100, 532, 61);
    _key_language = new EPDGUI_Button(4, 160, 532, 61);
    _key_syncntp = new EPDGUI_Button(4, 220, 532, 61);
    _key_restart = new EPDGUI_Button(4, 340, 532, 61);
    _key_shutdown = new EPDGUI_Button(4, 400, 532, 61);
    if(language == LANGUAGE_JA)
    {
        _key_wallpaper->setBMPButton("  壁紙", "\u25B6", ImageResource_item_icon_wallpaper_32x32);
        _key_language->setBMPButton("  言語", "\u25B6", ImageResource_item_icon_language_32x32);
        _key_syncntp->setBMPButton("  時間シンクロ", "", ImageResource_item_icon_ntptime_32x32);
        _key_restart->setBMPButton("  再起動", "", ImageResource_item_icon_restart_32x32);
        _key_shutdown->setBMPButton("  電源オフ", "", ImageResource_item_icon_shutdown_32x32);
        exitbtn("ホーム");
        _canvas_title->drawString("設定", 270, 34);
    }
    else if(language == LANGUAGE_ZH)
    {
        _key_wallpaper->setBMPButton("  壁纸", "\u25B6", ImageResource_item_icon_wallpaper_32x32);
        _key_language->setBMPButton("  语言", "\u25B6", ImageResource_item_icon_language_32x32);
        _key_syncntp->setBMPButton("  同步时间", "", ImageResource_item_icon_ntptime_32x32);
        _key_restart->setBMPButton("  重启", "", ImageResource_item_icon_restart_32x32);
        _key_shutdown->setBMPButton("  关机", "", ImageResource_item_icon_shutdown_32x32);
        exitbtn("主页");
        _canvas_title->drawString("设置", 270, 34);
    }
    else
    {
        _key_wallpaper->setBMPButton("  Wallpaper", "\u25B6", ImageResource_item_icon_wallpaper_32x32);
        _key_language->setBMPButton("  Language", "\u25B6", ImageResource_item_icon_language_32x32);
        _key_syncntp->setBMPButton("  Sync Time", "", ImageResource_item_icon_ntptime_32x32);
        _key_restart->setBMPButton("  Restart", "", ImageResource_item_icon_restart_32x32);
        _key_shutdown->setBMPButton("  Shutdown", "", ImageResource_item_icon_shutdown_32x32);
        exitbtn("Home");
        _canvas_title->drawString("Setting", 270, 34);
    }

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);

    _key_language->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_language->Bind(EPDGUI_Button::EVENT_RELEASED, &key_language_cb);

    _key_wallpaper->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_wallpaper->Bind(EPDGUI_Button::EVENT_RELEASED, &key_wallpaper_cb);

    _key_shutdown->Bind(EPDGUI_Button::EVENT_RELEASED, &key_shutdown_cb);
    _key_restart->Bind(EPDGUI_Button::EVENT_RELEASED, &key_restart_cb);
    _key_syncntp->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _canvas_title);
    _key_syncntp->Bind(EPDGUI_Button::EVENT_RELEASED, &key_synctime_cb);
}

Frame_Setting::~Frame_Setting(void)
{
    delete _key_wallpaper;
    delete _key_language;
    delete _key_shutdown;
    delete _key_restart;
    delete _key_syncntp;
}

int Frame_Setting::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_wallpaper);
    EPDGUI_AddObject(_key_language);
    EPDGUI_AddObject(_key_shutdown);
    EPDGUI_AddObject(_key_restart);
    EPDGUI_AddObject(_key_syncntp);
    EPDGUI_AddObject(_key_exit);
    return 3;
}