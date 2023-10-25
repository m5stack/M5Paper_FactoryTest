#include "systeminit.h"
#include "epdgui/epdgui.h"
#include "frame/frame.h"
#include "Free_Fonts.h"
#include "global_setting.h"
#include "resources/binaryttf.h"
#include <WiFi.h>

M5EPD_Canvas _initcanvas(&M5.EPD);

QueueHandle_t xQueue_Info = xQueueCreate(20, sizeof(uint32_t));

#define tryUpdateAnim() do { tryUpdateLoadingAnimation(animIndex, animUpdateTime, LoadingIMG); } while (0);

const uint16_t kPosy = 548;

void WaitForUser(M5EPD_Canvas &LoadingIMG) {
    LoadingIMG.pushImage(0, 0, 96, 96,
                            ImageResource_loading_error_96x96);
    LoadingIMG.pushCanvas(220, kPosy + 80, UPDATE_MODE_GL16);
    LoadingIMG.fillCanvas(0);
    while (1) {
        M5.update();
        if (M5.BtnP.wasReleased()) {
            // Clear with empty canvas.
            LoadingIMG.pushCanvas(220, kPosy + 80, UPDATE_MODE_GC16);
            return;
        }
    }
}

void Screen_Test(void) {
    Serial.println("Start Screen Life Test...");
    _initcanvas.createCanvas(540, 960);
    _initcanvas.setTextSize(4);
    delay(1000);
    float min = 0;
    while (1) {
        for (uint8_t pos = 0; pos < 2; pos++) {
            for (uint8_t index = 0; index < 16; index++) {
                _initcanvas.fillRect(0, index * 60, 540, 60, index);
            }
            int possition = random(960);
            _initcanvas.drawString("Test Time: " + String(min) + "min", 20,
                                   possition);
            _initcanvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
            delay(10000);
            for (uint8_t index = 0; index < 16; index++) {
                _initcanvas.fillRect(0, index * 60, 540, 60, (15 - index));
            }
            _initcanvas.drawString("Test Time: " + String(min) + "min", 20,
                                   possition);
            _initcanvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
            delay(10000);
        }
        min += 1;
    }
}

const uint8_t *kLD[] = {
    ImageResource_loading_01_96x96, ImageResource_loading_02_96x96,
    ImageResource_loading_03_96x96, ImageResource_loading_04_96x96,
    ImageResource_loading_05_96x96, ImageResource_loading_06_96x96,
    ImageResource_loading_07_96x96, ImageResource_loading_08_96x96,
    ImageResource_loading_09_96x96, ImageResource_loading_10_96x96,
    ImageResource_loading_11_96x96, ImageResource_loading_12_96x96,
    ImageResource_loading_13_96x96, ImageResource_loading_14_96x96,
    ImageResource_loading_15_96x96, ImageResource_loading_16_96x96,
};


void tryUpdateLoadingAnimation(int &animIndex, uint32_t animUpdateTime, M5EPD_Canvas &LoadingIMG){
    if (millis() - animUpdateTime > 250) {
        animUpdateTime = millis();
        LoadingIMG.pushImage(0, 0, 96, 96, kLD[animIndex]);
        LoadingIMG.pushCanvas(220, kPosy + 80, UPDATE_MODE_DU4);
        animIndex++;
        if (animIndex == 16) {
            animIndex = 0;
        }
    }
}

void SysInit_Start(void) {
    bool ret = false;
    Serial.begin(115200);
    Serial.flush();
    delay(100);
    log_d("M5EPD initializing...");

    pinMode(M5EPD_EXT_PWR_EN_PIN, OUTPUT);
    pinMode(M5EPD_EPD_PWR_EN_PIN, OUTPUT);
    pinMode(M5EPD_KEY_RIGHT_PIN, INPUT);
    pinMode(M5EPD_KEY_PUSH_PIN, INPUT);
    pinMode(M5EPD_KEY_LEFT_PIN, INPUT);
    M5.enableEPDPower();
    M5.enableEXTPower();
    delay(50);

    M5.EPD.begin(M5EPD_SCK_PIN, M5EPD_MOSI_PIN, M5EPD_MISO_PIN, M5EPD_CS_PIN,
                 M5EPD_BUSY_PIN);
    M5.EPD.Clear(true);
    M5.EPD.SetRotation(M5EPD_Driver::ROTATE_90);
    M5.TP.SetRotation(GT911::ROTATE_90);

    // Show first M5PAPER splash background logo
    M5.EPD.WritePartGram4bpp(92, 182, 356, 300, ImageResource_logo_356x300);
    M5.EPD.UpdateFull(UPDATE_MODE_GC16);

    M5EPD_Canvas LoadingIMG(&M5.EPD);
    M5EPD_Canvas Info(&M5.EPD);
    LoadingIMG.createCanvas(96, 96);
    Info.createCanvas(540, 50);
    Info.setFreeFont(FF18);
    Info.setTextSize(1);
    Info.setTextDatum(CC_DATUM);
    Info.setTextColor(15);

    int animIndex = 0;
    uint32_t animUpdateTime = 0;
    tryUpdateAnim();

    if (!digitalRead(39)) {
        delay(10);
        if (!digitalRead(39)) {
            Screen_Test();
        }
    }
    // Disable WDT at the beginning for other tasks (in settings frame and lifegame frame)
    disableCore0WDT();
    // SysInit_UpdateInfo(Info, "Initializing SD card...");
    bool is_factory_test;
    ret = SD.begin(4, *M5.EPD.GetSPI(), 20000000);
    if (!ret) {
        is_factory_test = true;
        SetInitStatus(0, 0);
        // log_e("Failed to initialize SD card.");
        // SysInit_UpdateInfo(Info, "[ERROR] Failed to initialize SD card.");
        // WaitForUser(LoadingIMG);
    } else {
        is_factory_test = SD.exists("/__factory_test_flag__");
    }

    // Try loading full font from SD card
    if (!is_factory_test && SD.exists("/font.ttf")) {
        SysInit_UpdateInfo(Info, "Loading font from SD...");
        _initcanvas.loadFont("/font.ttf", SD);
        SetTTFLoaded(true);
    } else {
        SysInit_UpdateInfo(Info, "Loading default font...");
        _initcanvas.loadFont(binaryttf, sizeof(binaryttf));
        SetTTFLoaded(false);
        SetLanguage(LANGUAGE_EN);
        is_factory_test = true;
    }

    SysInit_UpdateInfo(Info, "Initializing Touch pad...");
    if (M5.TP.begin(21, 22, 36) != ESP_OK) {
        SetInitStatus(1, 0);
        log_e("Touch pad initialization failed.");
        SysInit_UpdateInfo(Info, "[ERROR] Failed to initialize Touch pad.");
        WaitForUser(LoadingIMG);
    }

    M5.BatteryADCBegin();
    LoadSetting();

    tryUpdateAnim();
    if (is_factory_test) {
        SysInit_UpdateInfo(Info, "Success");
    } else {
        SysInit_UpdateInfo(Info, "Initializing system...");
    }

    tryUpdateAnim();
    _initcanvas.createRender(26, 128);
    tryUpdateAnim();

    Frame_Main *frame_main = new Frame_Main();
    EPDGUI_PushFrame(frame_main);
    Frame_FactoryTest *frame_factorytest = new Frame_FactoryTest();
    EPDGUI_AddFrame("Frame_FactoryTest", frame_factorytest);
    tryUpdateAnim();
    if (!is_factory_test) {
        Frame_Setting *frame_setting = new Frame_Setting();
        EPDGUI_AddFrame("Frame_Setting", frame_setting);
        Frame_Setting_Wallpaper *frame_wallpaper =
            new Frame_Setting_Wallpaper();
        EPDGUI_AddFrame("Frame_Setting_Wallpaper", frame_wallpaper);
        Frame_Setting_Language *frame_language = new Frame_Setting_Language();
        EPDGUI_AddFrame("Frame_Setting_Language", frame_language);
        Frame_Keyboard *frame_keyboard = new Frame_Keyboard(0);
        EPDGUI_AddFrame("Frame_Keyboard", frame_keyboard);
        Frame_WifiScan *frame_wifiscan = new Frame_WifiScan();
        EPDGUI_AddFrame("Frame_WifiScan", frame_wifiscan);
        Frame_WifiPassword *frame_wifipassword = new Frame_WifiPassword();
        EPDGUI_AddFrame("Frame_WifiPassword", frame_wifipassword);
        Frame_Lifegame *frame_lifegame = new Frame_Lifegame();
        EPDGUI_AddFrame("Frame_Lifegame", frame_lifegame);
        Frame_Compare *frame_compare = new Frame_Compare();
        EPDGUI_AddFrame("Frame_Compare", frame_compare);
        Frame_Home *frame_home = new Frame_Home();
        EPDGUI_AddFrame("Frame_Home", frame_home);

        tryUpdateAnim();
        if (isWiFiConfiged()) {
            SysInit_UpdateInfo(Info, "Connect to " + GetWifiSSID() + "...");
            WiFi.begin(GetWifiSSID().c_str(), GetWifiPassword().c_str());
            uint32_t t = millis();
            while (1) {
                if (millis() - t > 8000) {
                    break;
                }

                if (WiFi.status() == WL_CONNECTED) {
                    frame_wifiscan->SetConnected(GetWifiSSID(), WiFi.RSSI());
                    break;
                }
            }
        }
    }

    tryUpdateAnim();

    if (!is_factory_test) {
        SysInit_UpdateInfo(Info, "Success");
    }

    // Show loading success icon
    LoadingIMG.pushImage(0, 0, 96, 96, ImageResource_loading_success_96x96);
    LoadingIMG.pushCanvas(220, kPosy + 80, UPDATE_MODE_GL16);
    log_e("OK");
}

void SysInit_UpdateInfo(M5EPD_Canvas &Info, String msg) {
    Serial.println(msg);
    Info.fillCanvas(0);
    Info.drawString(msg, 270, 20);
    Info.pushCanvas(0, kPosy, UPDATE_MODE_DU);
}
