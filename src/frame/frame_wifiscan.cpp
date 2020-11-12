#include "frame_wifiscan.h"
#include "frame_wifipassword.h"
#include <WiFi.h>

#define MAX_BTN_NUM     14
#define MAX_WIFI_NUM    (MAX_BTN_NUM - 1)
bool _update_flag = false;
EPDGUI_Button *_connect_key = NULL;

const uint8_t *kIMGWifiLevel[4] = {
    NULL,
    ImageResource_item_icon_wifi_1_32x32,
    ImageResource_item_icon_wifi_2_32x32,
    ImageResource_item_icon_wifi_3_32x32
};

void key_wifi_cb(epdgui_args_vector_t &args)
{
    if(((EPDGUI_Button*)(args[0]))->GetCustomString() == "_$refresh$_")
    {
        _update_flag = true;
    }
    else
    {
        _connect_key = (EPDGUI_Button*)(args[0]);
        Frame_Base *frame = EPDGUI_GetFrame("Frame_WifiPassword");
        if(frame == NULL)
        {
            frame = new Frame_WifiPassword(false);
            EPDGUI_AddFrame("Frame_WifiPassword", frame);
        }
        EPDGUI_PushFrame(frame);
        *((int*)(args[1])) = 0;
    }
}


Frame_WifiScan::Frame_WifiScan(void)
{
    _frame_name = "Frame_WifiScan";

    for(int i = 0; i < MAX_BTN_NUM; i++)
    {
        _key_wifi[i] = new EPDGUI_Button(4, 100 + i * 60, 532, 61);
        _key_wifi[i]->SetHide(true);
        _key_wifi[i]->CanvasNormal()->setTextSize(26);
        _key_wifi[i]->CanvasNormal()->setTextDatum(CL_DATUM);
        _key_wifi[i]->CanvasNormal()->setTextColor(15);
        _key_wifi[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, _key_wifi[i]);
        _key_wifi[i]->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void*)(&_is_run));
        _key_wifi[i]->Bind(EPDGUI_Button::EVENT_RELEASED, key_wifi_cb);
    }
 
    _language = GetLanguage();
    if(_language == LANGUAGE_JA)
    {
        exitbtn("ホーム");
        _canvas_title->drawString("WLAN", 270, 34);
    }
    else if(_language == LANGUAGE_ZH)
    {
        exitbtn("主页");
        _canvas_title->drawString("无线局域网", 270, 34);
    }
    else
    {
        exitbtn("Home");
        _canvas_title->drawString("WLAN", 270, 34);
    }

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);

    _update_flag = true;
    _connected = 0;
}

Frame_WifiScan::~Frame_WifiScan(void)
{
    for(int i = 0; i < MAX_BTN_NUM; i++)
    {
        delete _key_wifi[i];
    }
}

void Frame_WifiScan::DrawItem(EPDGUI_Button *btn, String ssid, int rssi)
{
    int level = 0;
    if(rssi > -55)
    {
        level = 3;
    }
    else if(rssi > -88)
    {
        level = 2;
    }
    else
    {
        level = 1;
    }
    if(ssid.length() > 22)
    {
        ssid = ssid.substring(0, 22) + "...";
    }
    btn->SetHide(false);
    btn->CanvasNormal()->fillCanvas(0);
    btn->CanvasNormal()->drawRect(0, 0, 532, 61, 15);
    btn->CanvasNormal()->drawString(ssid, 15, 35);
    btn->SetCustomString(ssid);
    btn->CanvasNormal()->pushImage(532 - 15 - 32, 14, 32, 32, kIMGWifiLevel[level]);
    *(btn->CanvasPressed()) = *(btn->CanvasNormal());
    btn->CanvasPressed()->ReverseColor();
}

int Frame_WifiScan::run()
{
    if(_connect)
    {
        _connect = false;
        Connect();
    }
    if(_update_flag)
    {
        _update_flag = false;
        scan();
    }
    return 1;
}

int Frame_WifiScan::scan()
{
    WiFi.mode(WIFI_STA);
    // WiFi.disconnect();
    WiFi.scanNetworks(true);

    if(_scan_count > 0)
    {
        M5.EPD.WriteFullGram4bpp(GetWallpaper());
        _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
        _key_exit->Draw(UPDATE_MODE_NONE);
        M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    }
    _scan_count++;
    
    int wifi_num;
    while(1)
    {
        wifi_num = WiFi.scanComplete();
        if(wifi_num >= 0)
        {
            break;
        }
    }

    int connect_wifi_idx = -1;
    if(_connected)
    {
        for(int i = 0; i < wifi_num; i++)
        {
            String ssid = WiFi.SSID(i);
            
            if(ssid == _connect_ssid)
            {
                connect_wifi_idx = i;
                if(WiFi.RSSI(i) < -90)
                {
                    connect_wifi_idx = -1;
                }
                break;
            }
        }
        if(connect_wifi_idx == -1)
        {
            WiFi.disconnect();
            _key_wifi[0]->SetEnable(true);
            _connected = 0;
            for(int i = 1; i < MAX_BTN_NUM; i++)
            {
                _key_wifi[i]->SetPos(_key_wifi[i]->getX(), _key_wifi[i]->getY() - 32);
            }
        }
    }

    wifi_num = wifi_num > MAX_WIFI_NUM ? MAX_WIFI_NUM : wifi_num;
    wifi_num -= _connected;

    for(int i = _connected; i < MAX_BTN_NUM; i++)
    {
        _key_wifi[i]->SetHide(true);
    }

    if(_connected)
    {
        _key_wifi[0]->Draw(UPDATE_MODE_A2);
    }

    int idx = 0, cnt = _connected;
    while(1)
    {
        if(idx == connect_wifi_idx)
        {
            idx++;
            continue;
        }

        String ssid = WiFi.SSID(idx);
        DrawItem(_key_wifi[cnt], ssid, WiFi.RSSI(idx));
        _key_wifi[cnt]->Draw(UPDATE_MODE_A2);

        idx++;
        if(idx == wifi_num)
        {
            break;
        }

        cnt++;
    }
    
    _key_wifi[wifi_num]->SetCustomString("_$refresh$_");
    _key_wifi[wifi_num]->SetHide(false);
    _key_wifi[wifi_num]->CanvasNormal()->fillCanvas(0);
    _key_wifi[wifi_num]->CanvasNormal()->drawRect(0, 0, 532, 61, 15);
    _key_wifi[wifi_num]->CanvasNormal()->pushImage(15, 14, 32, 32, ImageResource_item_icon_refresh_32x32);
    if(_language == LANGUAGE_JA)
    {
        _key_wifi[wifi_num]->CanvasNormal()->drawString("刷新", 58, 35);
    }
    else if(_language == LANGUAGE_ZH)
    {
        _key_wifi[wifi_num]->CanvasNormal()->drawString("刷新", 58, 35);
    }
    else
    {
        _key_wifi[wifi_num]->CanvasNormal()->drawString("Refresh", 58, 35);
    }
    *(_key_wifi[wifi_num]->CanvasPressed()) = *(_key_wifi[wifi_num]->CanvasNormal());
    _key_wifi[wifi_num]->CanvasPressed()->ReverseColor();
    _key_wifi[wifi_num]->Draw(UPDATE_MODE_A2);

    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
    // M5.EPD.UpdateArea(0, 64, 540, 72, UPDATE_MODE_GL16);

    WiFi.scanDelete();

    return 0;
}

void Frame_WifiScan::Connect()
{
    int anime_cnt = 0;
    int x = 532 - 15 - 32;
    int y = _connect_key->getY() + 14;
    M5EPD_Canvas loading(&M5.EPD);
    loading.createCanvas(32, 32);
    loading.fillCanvas(0);
    loading.pushCanvas(x, y, UPDATE_MODE_GL16);
    _connect_ssid = _connect_key->GetCustomString();
    log_d("SSID = [%s]\n", _connect_ssid.c_str());
    log_d("PSWD = [%s]\n", _connect_password.c_str());
    WiFi.disconnect();
    WiFi.begin(_connect_ssid.c_str(), _connect_password.c_str());
    uint32_t start_time = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        loading.pushImage(0, 0, 32, 32, GetLoadingIMG_32x32(anime_cnt));
        loading.pushCanvas(x, y, UPDATE_MODE_DU4);
        anime_cnt++;
        if(anime_cnt == 16)
        {
            anime_cnt = 0;
        }

        if (millis() - start_time > 10000)
        {
            M5EPD_Canvas err(&M5.EPD);
            err.createCanvas(300, 100);
            err.fillCanvas(15);
            err.setTextSize(26);
            err.setTextColor(0);
            err.setTextDatum(CC_DATUM);
            if(_language == LANGUAGE_JA)
            {
                err.drawString("パスワードが違います", 150, 55);
            }
            else if(_language == LANGUAGE_ZH)
            {
                err.drawString("密码错误", 150, 55);
            }
            else
            {
                err.drawString("Wrong password", 150, 55);
            }
            err.pushCanvas(120, 430, UPDATE_MODE_GL16);
            return;
        }
    }

    for(int i = 1; i < MAX_BTN_NUM; i++)
    {
        _key_wifi[i]->SetPos(_key_wifi[i]->getX(), _key_wifi[i]->getY() + 32);
    }

    _connect_key->CanvasNormal()->pushImage(532 - 15 - 32, 14, 32, 32, ImageResource_item_icon_success_32x32);
    
    _key_wifi[0]->SetEnable(false);
    _key_wifi[0]->SetHide(false);
    if(_connect_key != _key_wifi[0])
    {
         *(_key_wifi[0]->CanvasNormal()) = *(_connect_key->CanvasNormal());
        *(_key_wifi[0]->CanvasPressed()) = *(_connect_key->CanvasNormal());
        _key_wifi[0]->CanvasPressed()->ReverseColor();
    }
    loading.pushImage(0, 0, 32, 32, ImageResource_item_icon_success_32x32);
    loading.pushCanvas(x, y, UPDATE_MODE_GL16);
    _connected = 1;

    SetWifi(_connect_ssid, _connect_password);
    // SyncNTPTime();
    scan();
}

void Frame_WifiScan::SetConnected(String ssid, int rssi)
{
    _connect_ssid = ssid;
    DrawItem(_key_wifi[0], ssid, rssi);
    for(int i = 1; i < MAX_BTN_NUM; i++)
    {
        _key_wifi[i]->SetPos(_key_wifi[i]->getX(), _key_wifi[i]->getY() + 32);
    }    
    _key_wifi[0]->SetEnable(false);
    _key_wifi[0]->SetHide(false);
    _connected = 1;
}

int Frame_WifiScan::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    _connect = false;
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    if(args.size() > 0)
    {
        String *password = (String*)(args[0]);
        _connect_password = *password;
        delete password;
        args.pop_back();

        for(int i = 0; i < MAX_BTN_NUM; i++)
        {
            EPDGUI_AddObject(_key_wifi[i]);
        }
        _update_flag = false;
        _connect = true;
    }
    else
    {
        for(int i = 0; i < MAX_BTN_NUM; i++)
        {
            _key_wifi[i]->SetHide(true);
            EPDGUI_AddObject(_key_wifi[i]);
        }
        if(_connected)
        {
            _key_wifi[0]->SetHide(false);
        }
        _update_flag = true;
        _scan_count = 0;
        _connect = false;
    }
    EPDGUI_AddObject(_key_exit);
    
    return 3;
}