#include "frame_pictureviewer.h"
#include "SD.h"

void key_pictureviewer_exit_cb(epdgui_args_vector_t &args)
{
    EPDGUI_PopFrame(true);
    *((int*)(args[0])) = 0;
}

Frame_PictureViewer::Frame_PictureViewer(String path)
{
    _frame_name = "Frame_PictureViewer";
    _pic_path = path;

    _canvas_picture = new M5EPD_Canvas(&M5.EPD);
    _canvas_picture->createCanvas(540, 888);
    _canvas_picture->setTextSize(26);
    _canvas_picture->setTextColor(0);
    _canvas_picture->setTextDatum(CC_DATUM);
 
    uint8_t language = GetLanguage();
    if(language == LANGUAGE_JA)
    {
        exitbtn("戻る");
    }
    else if(language == LANGUAGE_ZH)
    {
        exitbtn("返回");
    }
    else
    {
        exitbtn("Back");
    }

    _canvas_title->drawString(path.substring(path.lastIndexOf("/") + 1, path.lastIndexOf(".")), 270, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &key_pictureviewer_exit_cb);

    _is_first = true;
}

Frame_PictureViewer::~Frame_PictureViewer(void)
{
    delete _canvas_picture;
}

void Frame_PictureViewer::err(String info)
{
    _canvas_picture->fillCanvas(0);
    _canvas_picture->fillRect(254 - 150, 500 - 50, 300, 100, 15);
    _canvas_picture->drawString(info, 150, 55);
}

int Frame_PictureViewer::run()
{
    Frame_Base::run();
    if(_is_first)
    {
        _is_first = false;
        LoadingAnime_32x32_Start(254, 500);
        String suffix = _pic_path.substring(_pic_path.lastIndexOf("."));
        if((suffix.indexOf("bmp") >= 0) || (suffix.indexOf("BMP") >= 0))
        {
            bool ret = _canvas_picture->drawBmpFile(SD, _pic_path.c_str(), 0, 0);
            if(ret == 0)
            {
                err("Error opening " + _pic_path.substring(_pic_path.lastIndexOf("/")));
            }
        }
        else if ((suffix.indexOf("png") >= 0) || (suffix.indexOf("PNG") >= 0))
        {
            bool ret = _canvas_picture->drawPngFile(SD, _pic_path.c_str());
            if(ret == 0)
            {
                err("Error opening " + _pic_path.substring(_pic_path.lastIndexOf("/")));
            }
        }
        else if ((suffix.indexOf("jpg") >= 0) || (suffix.indexOf("JPG") >= 0))
        {
            bool ret = _canvas_picture->drawJpgFile(SD, _pic_path.c_str());
            if(ret == 0)
            {
                err("Error opening " + _pic_path.substring(_pic_path.lastIndexOf("/")));
            }
        }
        LoadingAnime_32x32_Stop();
        _canvas_picture->pushCanvas(0, 72, UPDATE_MODE_GC16);
    }
    return 1;
}

int Frame_PictureViewer::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_exit);
    return 3;
}


