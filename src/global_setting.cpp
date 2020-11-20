#include "global_setting.h"
#include "./resources/ImageResource.h"
#include "esp32-hal-log.h"
#include <WiFi.h>

#define DEFAULT_WALLPAPER 2
SemaphoreHandle_t _xSemaphore_LoadingAnime = NULL;
static uint8_t _loading_anime_eixt_flag = false;
esp_err_t __espret__;
#define NVS_CHECK(x)              \
    __espret__ = x;           \
    if (__espret__ != ESP_OK) \
    {                         \
        nvs_close(nvs_arg);   \
        log_e("Check Err");   \
        return __espret__;    \
    }

const uint8_t *wallpapers[] = {
    ImageResource_wallpaper_m5stack_540x960,
    ImageResource_wallpaper_engine_540x960,
    ImageResource_wallpaper_penrose_triangle_540x960
};

const uint8_t *kIMGLoading[16] = {
    ImageResource_item_loading_01_32x32,
    ImageResource_item_loading_02_32x32,
    ImageResource_item_loading_03_32x32,
    ImageResource_item_loading_04_32x32,
    ImageResource_item_loading_05_32x32,
    ImageResource_item_loading_06_32x32,
    ImageResource_item_loading_07_32x32,
    ImageResource_item_loading_08_32x32,
    ImageResource_item_loading_09_32x32,
    ImageResource_item_loading_10_32x32,
    ImageResource_item_loading_11_32x32,
    ImageResource_item_loading_12_32x32,
    ImageResource_item_loading_13_32x32,
    ImageResource_item_loading_14_32x32,
    ImageResource_item_loading_15_32x32,
    ImageResource_item_loading_16_32x32
};
const char *wallpapers_name_en[] = {
    "M5Paper",
    "Engine",
    "Penrose Triangle"
};
const char *wallpapers_name_zh[] = {
    "M5Paper",
    "引擎",
    "彭罗斯三角"
};
const char *wallpapers_name_ja[] = {
    "M5Paper",
    "エンジン",
    "ペンローズの三角形"
};
uint16_t global_wallpaper = DEFAULT_WALLPAPER;
uint8_t global_language = LANGUAGE_EN;
String global_wifi_ssid;
String global_wifi_password;
uint8_t global_wifi_configed = false;
uint16_t global_reader_textsize = 32;
uint8_t global_time_synced = false;
uint8_t global_ttf_file_loaded = false;
uint8_t global_init_status = 0xFF;
int8_t global_timezone = 8;


int8_t GetTimeZone(void)
{
    return global_timezone;
}

void SetTimeZone(int8_t time_zone)
{
    global_timezone = time_zone;
}

void SetInitStatus(uint8_t idx, uint8_t val)
{
    global_init_status &= ~(1 << idx);
    global_init_status |= (val << idx);
}

uint8_t GetInitStatus(uint8_t idx)
{
    return (global_init_status & (1 << idx)) ? true : false;
}

void SetTTFLoaded(uint8_t val)
{
    global_ttf_file_loaded = val;
}

uint8_t isTTFLoaded()
{
    return global_ttf_file_loaded;
}

uint8_t isTimeSynced(void)
{
    return global_time_synced;
}

void SetTimeSynced(uint8_t val)
{
    global_time_synced = val;
    SaveSetting();
}

void SetLanguage(uint8_t language)
{
    if (language >= LANGUAGE_EN && language <= LANGUAGE_ZH)
    {
        global_language = language;
    }
    SaveSetting();
}

uint8_t GetLanguage(void)
{
    return global_language;
}

void SetWallpaper(uint16_t wallpaper_id)
{
    global_wallpaper = wallpaper_id;
    SaveSetting();
}

uint16_t GetWallpaperID(void)
{
    return global_wallpaper;
}

const uint8_t *GetWallpaper(void)
{
    return wallpapers[global_wallpaper];
}

const char *GetWallpaperName(uint16_t wallpaper_id)
{
    switch (global_language)
    {
    case LANGUAGE_ZH:
        return wallpapers_name_zh[wallpaper_id];
    case LANGUAGE_JA:
        return wallpapers_name_ja[wallpaper_id];
    default:
        return wallpapers_name_en[wallpaper_id];
    }
}

esp_err_t LoadSetting(void)
{
    nvs_handle nvs_arg;
    NVS_CHECK(nvs_open("Setting", NVS_READONLY, &nvs_arg));
    NVS_CHECK(nvs_get_u16(nvs_arg, "Wallpaper", &global_wallpaper));
    NVS_CHECK(nvs_get_u8(nvs_arg, "Language", &global_language));
    NVS_CHECK(nvs_get_u8(nvs_arg, "Timesync", &global_time_synced));
    nvs_get_i8(nvs_arg, "timezone", &global_timezone);

    if(global_wallpaper >= WALLPAPER_NUM)
    {
        global_wallpaper = DEFAULT_WALLPAPER;
    }
    
    size_t length = 128;
    char buf[128];
    NVS_CHECK(nvs_get_str(nvs_arg, "ssid", buf, &length));
    global_wifi_ssid = String(buf);
    length = 128;
    NVS_CHECK(nvs_get_str(nvs_arg, "pswd", buf, &length));
    global_wifi_password = String(buf);
    global_wifi_configed = true;
    nvs_close(nvs_arg);
    return ESP_OK;
}

esp_err_t SaveSetting(void)
{
    nvs_handle nvs_arg;
    NVS_CHECK(nvs_open("Setting", NVS_READWRITE, &nvs_arg));
    NVS_CHECK(nvs_set_u16(nvs_arg, "Wallpaper", global_wallpaper));
    NVS_CHECK(nvs_set_u8(nvs_arg, "Language", global_language));
    NVS_CHECK(nvs_set_u8(nvs_arg, "Timesync", global_time_synced));
    NVS_CHECK(nvs_set_i8(nvs_arg, "timezone", global_timezone));
    NVS_CHECK(nvs_set_str(nvs_arg, "ssid", global_wifi_ssid.c_str()));
    NVS_CHECK(nvs_set_str(nvs_arg, "pswd", global_wifi_password.c_str()));
    NVS_CHECK(nvs_commit(nvs_arg));
    nvs_close(nvs_arg);
    return ESP_OK;
}

void SetWifi(String ssid, String password)
{
    global_wifi_ssid = ssid;
    global_wifi_password = password;
    SaveSetting();
}

uint8_t isWiFiConfiged(void)
{
    return global_wifi_configed;
}

String GetWifiSSID(void)
{
    return global_wifi_ssid;
}

String GetWifiPassword(void)
{
    return global_wifi_password;
}

bool SyncNTPTime(void)
{
    const char *ntpServer = "time.cloudflare.com";
    configTime(global_timezone * 3600, 0, ntpServer);

    struct tm timeInfo;
    if (getLocalTime(&timeInfo))
    {
        rtc_time_t time_struct;
        time_struct.hour = timeInfo.tm_hour;
        time_struct.min = timeInfo.tm_min;
        time_struct.sec = timeInfo.tm_sec;
        M5.RTC.setTime(&time_struct);
        rtc_date_t date_struct;
        date_struct.week = timeInfo.tm_wday;
        date_struct.mon = timeInfo.tm_mon + 1;
        date_struct.day = timeInfo.tm_mday;
        date_struct.year = timeInfo.tm_year + 1900;
        M5.RTC.setDate(&date_struct);
        SetTimeSynced(1);
        return 1;
    }
    log_d("Time Sync failed");
    return 0;
}

uint16_t GetTextSize()
{
    return global_reader_textsize;
}

void SetTextSize(uint16_t size)
{
    global_reader_textsize = size;
}

const uint8_t* GetLoadingIMG_32x32(uint8_t id)
{
    return kIMGLoading[id];
}

void __LoadingAnime_32x32(void *pargs)
{
    uint16_t *args = (uint16_t *)pargs;
    uint16_t x = args[0];
    uint16_t y = args[1];
    free(pargs);
    M5EPD_Canvas loading(&M5.EPD);
    loading.createCanvas(32, 32);
    loading.fillCanvas(0);
    loading.pushCanvas(x, y, UPDATE_MODE_GL16);
    int anime_cnt = 0;
    uint32_t time = 0;
    while (1)
    {
        if(millis() - time > 200)
        {
            time = millis();
            loading.pushImage(0, 0, 32, 32, GetLoadingIMG_32x32(anime_cnt));
            loading.pushCanvas(x, y, UPDATE_MODE_DU4);
            anime_cnt++;
            if(anime_cnt == 16)
            {
                anime_cnt = 0;
            }
        }

        xSemaphoreTake(_xSemaphore_LoadingAnime, portMAX_DELAY);
        if(_loading_anime_eixt_flag == true)
        {
            xSemaphoreGive(_xSemaphore_LoadingAnime);
            break;
        }
        xSemaphoreGive(_xSemaphore_LoadingAnime);
    }
    vTaskDelete(NULL);
}

void LoadingAnime_32x32_Start(uint16_t x, uint16_t y)
{
    if(_xSemaphore_LoadingAnime == NULL)
    {
        _xSemaphore_LoadingAnime = xSemaphoreCreateMutex();
    }
    _loading_anime_eixt_flag = false;
    uint16_t *pos = (uint16_t*)calloc(2, sizeof(uint16_t));
    pos[0] = x;
    pos[1] = y;
    xTaskCreatePinnedToCore(__LoadingAnime_32x32, "__LoadingAnime_32x32", 16 * 1024, pos, 1, NULL, 0);
}

void LoadingAnime_32x32_Stop()
{
    xSemaphoreTake(_xSemaphore_LoadingAnime, portMAX_DELAY);
    _loading_anime_eixt_flag = true;
    xSemaphoreGive(_xSemaphore_LoadingAnime);
    delay(200);
}
