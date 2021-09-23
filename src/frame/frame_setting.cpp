#include "frame_setting.h"
#include "frame_setting_language.h"
#include "frame_setting_wallpaper.h"
#include "WiFi.h"

#define KEY_W 92
#define KEY_H 92
const uint16_t kTimeZoneY = 520;

void key_shutdown_cb(epdgui_args_vector_t &args)
{
    Shutdown();
}

void key_restart_cb(epdgui_args_vector_t &args)
{
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    SaveSetting();
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
    SaveSetting();
    M5EPD_Canvas info(&M5.EPD);
    M5EPD_Canvas *title = (M5EPD_Canvas*)(args[0]);
    M5EPD_Canvas *tzone = (M5EPD_Canvas*)(args[1]);
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
        tzone->pushCanvas(4, kTimeZoneY, UPDATE_MODE_NONE);
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
    tzone->pushCanvas(4, kTimeZoneY, UPDATE_MODE_NONE);
    EPDGUI_Draw(UPDATE_MODE_NONE);
    while(!M5.TP.avaliable());
    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
}

void key_timezone_plus_cb(epdgui_args_vector_t &args)
{
    int *tz = (int*)(args[0]);
    (*tz)++;
    if((*tz) > 12)
    {
        (*tz) = 12;
    }
    String str = String(*tz);
    if((*tz) > 0)
    {
        str = "+" + str;
    }
    ((EPDGUI_Button*)(args[1]))->setLabel(str);
    ((EPDGUI_Button*)(args[1]))->Draw(UPDATE_MODE_GL16);
    SetTimeZone(*tz);
}

void key_timezone_minus_cb(epdgui_args_vector_t &args)
{
    int *tz = (int*)(args[0]);
    (*tz)--;
    if((*tz) < -11)
    {
        (*tz) = -11;
    }
    String str = String(*tz);
    if((*tz) > 0)
    {
        str = "+" + str;
    }
    ((EPDGUI_Button*)(args[1]))->setLabel(str);
    ((EPDGUI_Button*)(args[1]))->Draw(UPDATE_MODE_GL16);
    SetTimeZone(*tz);
}

void key_timezone_reset_cb(epdgui_args_vector_t &args)
{
    int *tz = (int*)(args[0]);
    (*tz) = 0;
    ((EPDGUI_Button*)(args[1]))->setLabel(String(*tz));
    ((EPDGUI_Button*)(args[1]))->Draw(UPDATE_MODE_GL16);
    SetTimeZone(*tz);
}

Frame_Setting::Frame_Setting(void)
{
    _frame_name = "Frame_Setting";

    _timezone_canvas = new M5EPD_Canvas(&M5.EPD);
    _timezone_canvas->createCanvas(540, 60);
    _timezone_canvas->fillCanvas(0);
    _timezone_canvas->setTextSize(26);
    _timezone_canvas->setTextColor(15);
    _timezone_canvas->setTextDatum(CL_DATUM);
 
    uint8_t language = GetLanguage();
    _key_wallpaper = new EPDGUI_Button(4, 100, 532, 61);
    _key_language = new EPDGUI_Button(4, 160, 532, 61);
    _key_syncntp = new EPDGUI_Button(4, 220, 532, 61);
    _key_restart = new EPDGUI_Button(4, 340, 532, 61);
    _key_shutdown = new EPDGUI_Button(4, 400, 532, 61);

    key_timezone_plus = new EPDGUI_Button("+", 448, kTimeZoneY, 88, 52);
    String str = String(GetTimeZone());
    if(GetTimeZone() > 0)
    {
        str = "+" + str;
    }
    key_timezone_reset = new EPDGUI_Button(str, 360, kTimeZoneY, 88, 52);
    key_timezone_minus = new EPDGUI_Button("-", 272, kTimeZoneY, 88, 52);
    
    key_timezone_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, &_timezone);
    key_timezone_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, key_timezone_reset);
    key_timezone_plus->Bind(EPDGUI_Button::EVENT_RELEASED, key_timezone_plus_cb);

    key_timezone_reset->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, &_timezone);
    key_timezone_reset->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, key_timezone_reset);
    key_timezone_reset->Bind(EPDGUI_Button::EVENT_RELEASED, key_timezone_reset_cb);

    key_timezone_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, &_timezone);
    key_timezone_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, key_timezone_reset);
    key_timezone_minus->Bind(EPDGUI_Button::EVENT_RELEASED, key_timezone_minus_cb);

    if(language == LANGUAGE_JA)
    {
        _key_wallpaper->setBMPButton("  壁紙", "\u25B6", ImageResource_item_icon_wallpaper_32x32);
        _key_language->setBMPButton("  言語", "\u25B6", ImageResource_item_icon_language_32x32);
        _key_syncntp->setBMPButton("  時間シンクロ", "", ImageResource_item_icon_ntptime_32x32);
        _key_restart->setBMPButton("  再起動", "", ImageResource_item_icon_restart_32x32);
        _key_shutdown->setBMPButton("  電源オフ", "", ImageResource_item_icon_shutdown_32x32);
        _timezone_canvas->drawString("時間帯 (UTC)", 15, 35);
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
        _timezone_canvas->drawString("时区 (UTC)", 15, 35);
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
        _timezone_canvas->drawString("Time zone (UTC)", 15, 35);
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
    _key_syncntp->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, _timezone_canvas);
    _key_syncntp->Bind(EPDGUI_Button::EVENT_RELEASED, &key_synctime_cb);

    _timezone = GetTimeZone();
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
    _timezone_canvas->pushCanvas(0, kTimeZoneY, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_wallpaper);
    EPDGUI_AddObject(_key_language);
    EPDGUI_AddObject(_key_shutdown);
    EPDGUI_AddObject(_key_restart);
    EPDGUI_AddObject(_key_syncntp);
    EPDGUI_AddObject(_key_exit);
    EPDGUI_AddObject(key_timezone_plus);
    EPDGUI_AddObject(key_timezone_reset);
    EPDGUI_AddObject(key_timezone_minus);
    return 3;
}