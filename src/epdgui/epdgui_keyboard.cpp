#include "epdgui_keyboard.h"

static const char* kKeyAlphaMapLowerCase[26] = {
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", //10
       "a", "s", "d", "f", "g", "h", "j", "k", "l",   //9
            "z", "x", "c", "v", "b", "n", "m",   //7
    };

static const char* kKeyAlphaMapUpperCase[26] = {
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", //10
       "A", "S", "D", "F", "G", "H", "J", "K", "L",   //9
            "Z", "X", "C", "V", "B", "N", "M",   //7
    };

static const char* kKeyAlphaMapNumber[26] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", //10
       "-", "/", ":", ";", "(", ")", "$", "&", "@",   //9
            "_", "\"", ".", ",", "?", "!", "'",   //7
    };

static const char* kKeyAlphaMapSymbol[26] = {
    "[", "]", "{", "}", "#", "%", "^", "*", "+", "=", //10
       "_", "\\", "|", "~", "<", ">", "€", "£", "¥",   //9
            "•", "✿", "\u221A", "\u221E", "\u2103", "\u2109", "\u2116",   //7
    };


// static const char* kRomaSpell[] = {
//     "あアぁァ"
//     "ばバびビぶブべべぼボびゃビャびゅビュびょビョ"
// };

enum
{
    kKeySpace = 26,
    kKeyBackspace,
    kKeyWrap,
    kKeyCase,
    kKeySwitch,
    kKeyNumber
};

enum
{
    kSWCase = 0,
    kSWSwitch,
    kSWNumber
};

enum
{
    kLayoutLowerAlpha = 0,
    kLayoutUpperAlpha,
    kLayoutNumber,
    kLayoutSymbol
};

EPDGUI_Keyboard::EPDGUI_Keyboard(int16_t dir, uint32_t style): EPDGUI_Base()
{
    uint8_t language = GetLanguage();
    
    if(dir) // Horizontal
    {
        const uint16_t kKeyWidth = 72;
        const uint16_t kKeyHeight = 44;
        const uint16_t kKeyInterval = 8;
        const uint16_t kFirstLineY = 302;
        const uint16_t kSecondLineY = 356;
        const uint16_t kThirdLineY = 410;
        const uint16_t k4thLineY = 464;
        const uint16_t kBaseX = 84;

        for(int i = 0; i < 10; i++)
        {
            _btn[i] = new EPDGUI_Button(kKeyAlphaMapLowerCase[i], kBaseX + (kKeyInterval + kKeyWidth) * i, kFirstLineY, kKeyWidth, kKeyHeight);
        }
        for(int i = 10; i < 19; i++)
        {
            _btn[i] = new EPDGUI_Button(kKeyAlphaMapLowerCase[i], kBaseX + 40 + (kKeyInterval + kKeyWidth) * (i - 10), kSecondLineY, kKeyWidth, kKeyHeight);
        }
        for(int i = 19; i < 26; i++)
        {
            _btn[i] = new EPDGUI_Button(kKeyAlphaMapLowerCase[i], kBaseX + 118 + (kKeyInterval + kKeyWidth) * (i - 19), kThirdLineY, kKeyWidth, kKeyHeight);
        }

        _btn[kKeyBackspace] = new EPDGUI_Button("", kBaseX + 792 - 96, kThirdLineY, 96, kKeyHeight);
        _btn[kKeyBackspace]->CanvasNormal()->pushImage(_btn[kKeyBackspace]->getW() / 2 - 16, _btn[kKeyBackspace]->getH() / 2 - 16, 32, 32, ImageResource_backspace_32x32);
        _btn[kKeyBackspace]->CanvasPressed()->fillCanvas(0);
        _btn[kKeyBackspace]->CanvasPressed()->pushImage(_btn[kKeyBackspace]->getW() / 2 - 16, _btn[kKeyBackspace]->getH() / 2 - 16, 32, 32, ImageResource_backspace_32x32);
        _btn[kKeyBackspace]->CanvasPressed()->ReverseColor();

        if(language == LANGUAGE_JA)
        {
            _btn[kKeySpace] = new EPDGUI_Button("空白", kBaseX + 162, k4thLineY, 468, kKeyHeight);
            if(style & STYLE_INPUTMODE_NORMALTEXT)
                _btn[kKeyWrap] = new EPDGUI_Button("改行", kBaseX + 792 - 152, k4thLineY, 152, kKeyHeight);
            else if(style & STYLE_INPUTMODE_NEEDCONFIRM)
                _btn[kKeyWrap] = new EPDGUI_Button("確認", kBaseX + 792 - 152, k4thLineY, 152, kKeyHeight);
        }
        else if(language == LANGUAGE_ZH)
        {
            _btn[kKeySpace] = new EPDGUI_Button("空格", kBaseX + 162, k4thLineY, 468, kKeyHeight);
            if(style & STYLE_INPUTMODE_NORMALTEXT)
                _btn[kKeyWrap] = new EPDGUI_Button("换行", kBaseX + 792 - 152, k4thLineY, 152, kKeyHeight);
            else if(style & STYLE_INPUTMODE_NEEDCONFIRM)
                _btn[kKeyWrap] = new EPDGUI_Button("确认", kBaseX + 792 - 152, k4thLineY, 152, kKeyHeight);
        }
        else
        {
            _btn[kKeySpace] = new EPDGUI_Button("Space", kBaseX + 162, k4thLineY, 468, kKeyHeight);
            if(style & STYLE_INPUTMODE_NORMALTEXT)
                _btn[kKeyWrap] = new EPDGUI_Button("Wrap", kBaseX + 792 - 152, k4thLineY, 152, kKeyHeight);
            else if(style & STYLE_INPUTMODE_NEEDCONFIRM)
                _btn[kKeyWrap] = new EPDGUI_Button("Confirm", kBaseX + 792 - 152, k4thLineY, 152, kKeyHeight);
        }

        // function key
        _sw[kSWCase] = new EPDGUI_Switch(2, kBaseX, kThirdLineY, 96, kKeyHeight);
        _sw[kSWSwitch] = new EPDGUI_Switch(2, kBaseX, k4thLineY, 68, kKeyHeight);
        _sw[kSWNumber] = new EPDGUI_Switch(2, kBaseX + 162 - kKeyInterval - 68, k4thLineY, 68, kKeyHeight);
    }
    else
    {
        const uint16_t kBaseX = 16;
        const uint16_t kBaseY = 700;
        const uint16_t kKeyWidth = 44;
        const uint16_t kKeyHeight = 52;
        const uint16_t kKeyInterval = 8;
        const uint16_t kFirstLineY = kBaseY;
        const uint16_t kSecondLineY = kBaseY + 64;
        const uint16_t kThirdLineY = kSecondLineY + 64;
        const uint16_t k4thLineY = kThirdLineY + 64;

        for(int i = 0; i < 10; i++)
        {
            _btn[i] = new EPDGUI_Button(kKeyAlphaMapLowerCase[i], kBaseX + (kKeyInterval + kKeyWidth) * i, kFirstLineY, kKeyWidth, kKeyHeight);
        }
        for(int i = 10; i < 19; i++)
        {
            _btn[i] = new EPDGUI_Button(kKeyAlphaMapLowerCase[i], kBaseX + 28 + (kKeyInterval + kKeyWidth) * (i - 10), kSecondLineY, kKeyWidth, kKeyHeight);
        }
        for(int i = 19; i < 26; i++)
        {
            _btn[i] = new EPDGUI_Button(kKeyAlphaMapLowerCase[i], kBaseX + 80 + (kKeyInterval + kKeyWidth) * (i - 19), kThirdLineY, kKeyWidth, kKeyHeight);
        }

        _btn[kKeyBackspace] = new EPDGUI_Button("", kBaseX + 512 - 60, kThirdLineY, 60, kKeyHeight);
        _btn[kKeyBackspace]->CanvasNormal()->pushImage(_btn[kKeyBackspace]->getW() / 2 - 16, _btn[kKeyBackspace]->getH() / 2 - 16, 32, 32, ImageResource_backspace_32x32);
        _btn[kKeyBackspace]->CanvasPressed()->fillCanvas(0);
        _btn[kKeyBackspace]->CanvasPressed()->pushImage(_btn[kKeyBackspace]->getW() / 2 - 16, _btn[kKeyBackspace]->getH() / 2 - 16, 32, 32, ImageResource_backspace_32x32);
        _btn[kKeyBackspace]->CanvasPressed()->ReverseColor();

        if(language == LANGUAGE_JA)
        {
            _btn[kKeySpace] = new EPDGUI_Button("空白", kBaseX + 132, k4thLineY, 244, kKeyHeight);
            if(style & STYLE_INPUTMODE_NORMALTEXT)
                _btn[kKeyWrap] = new EPDGUI_Button("改行", kBaseX + 512 - 128, k4thLineY, 128, kKeyHeight);
            else if(style & STYLE_INPUTMODE_NEEDCONFIRM)
                _btn[kKeyWrap] = new EPDGUI_Button("確認", kBaseX + 512 - 128, k4thLineY, 128, kKeyHeight);
        }
        else if(language == LANGUAGE_ZH)
        {
            _btn[kKeySpace] = new EPDGUI_Button("空格", kBaseX + 132, k4thLineY, 244, kKeyHeight);
            if(style & STYLE_INPUTMODE_NORMALTEXT)
                _btn[kKeyWrap] = new EPDGUI_Button("换行", kBaseX + 512 - 128, k4thLineY, 128, kKeyHeight);
            else if(style & STYLE_INPUTMODE_NEEDCONFIRM)
                _btn[kKeyWrap] = new EPDGUI_Button("确认", kBaseX + 512 - 128, k4thLineY, 128, kKeyHeight);
        }
        else
        {
            _btn[kKeySpace] = new EPDGUI_Button("Space", kBaseX + 132, k4thLineY, 244, kKeyHeight);
            if(style & STYLE_INPUTMODE_NORMALTEXT)
                _btn[kKeyWrap] = new EPDGUI_Button("Wrap", kBaseX + 512 - 128, k4thLineY, 128, kKeyHeight);
            else if(style & STYLE_INPUTMODE_NEEDCONFIRM)
                _btn[kKeyWrap] = new EPDGUI_Button("Confirm", kBaseX + 512 - 128, k4thLineY, 128, kKeyHeight);
        }

        // function key
        _sw[kSWCase] = new EPDGUI_Switch(2, kBaseX, kThirdLineY, 60, kKeyHeight);
        _sw[kSWSwitch] = new EPDGUI_Switch(2, kBaseX, k4thLineY, 56, kKeyHeight);
        _sw[kSWNumber] = new EPDGUI_Switch(2, kBaseX + 56 + kKeyInterval, k4thLineY, 60, kKeyHeight);
    }

    _sw[kSWCase]->Canvas(0)->pushImage(_sw[kSWCase]->getW() / 2 - 16, _sw[kSWCase]->getH() / 2 - 16, 32, 32, ImageResource_upper_32x32);
    _sw[kSWCase]->Canvas(1)->pushImage(_sw[kSWCase]->getW() / 2 - 16, _sw[kSWCase]->getH() / 2 - 16, 32, 32, ImageResource_upper_32x32);
    _sw[kSWCase]->Canvas(1)->ReverseColor();
    _sw[kSWSwitch]->SetLabel(0, "あ"); //TODO: Support for kana input
    _sw[kSWSwitch]->SetLabel(1, "Aa");
    _sw[kSWNumber]->SetLabel(0, "123");
    _sw[kSWNumber]->SetLabel(1, "Abc");

    for(int i = 0; i < 29; i++)
    {
        _key[i] = _btn[i];
    }
    _key[kKeyCase] = _sw[kSWCase];
    _key[kKeySwitch] = _sw[kSWSwitch];
    _key[kKeyNumber] = _sw[kSWNumber];

    // _sw[kSWSwitch]->SetHide(true);

    _layout = kLayoutLowerAlpha;
}

EPDGUI_Keyboard::~EPDGUI_Keyboard()
{
    for(int i = 0; i < 29; i++)
    {
        delete _btn[i];
    }
    for(int i = 0; i < 3; i++)
    {
        delete _sw[i];
    }
}

void EPDGUI_Keyboard::Draw(m5epd_update_mode_t mode)
{
    if(_ishide)
    {
        return;
    }

    for(int i = 0; i < 32; i++)
    {
        _key[i]->Draw(mode);
    }
}

void EPDGUI_Keyboard::Draw(M5EPD_Canvas* canvas)
{
    if(_ishide)
    {
        return;
    }

    for(int i = 0; i < 32; i++)
    {
        _key[i]->Draw(canvas);
    }
}

void EPDGUI_Keyboard::Bind(int16_t state, void (* func_cb)(epdgui_args_vector_t&))
{
    
}

void EPDGUI_Keyboard::UpdateState(int16_t x, int16_t y)
{
    if(!_isenable)
    {
        return;
    }
    // log_d("UpdateState %d, %d", x, y);
    for(int i = 0; i < 32; i++)
    {
        bool keypressed = _key[i]->isInBox(x, y);
        _key[i]->UpdateState(x, y);
        if(keypressed)
        {
            if(i < 26)
            {
                switch(_layout)
                {
                    case kLayoutLowerAlpha: _data += kKeyAlphaMapLowerCase[i]; break;
                    case kLayoutUpperAlpha: _data += kKeyAlphaMapUpperCase[i]; break;
                    case kLayoutNumber: _data += kKeyAlphaMapNumber[i]; break;
                    case kLayoutSymbol: _data += kKeyAlphaMapSymbol[i]; break;
                }
            }
            switch(i)
            {
                case kKeyBackspace: _data += "\u0008"; break;
                case kKeySpace: _data += " "; break;
                case kKeyWrap: _data += "\n"; break;
                case kKeyCase:
                {
                    if(_layout == kLayoutNumber || _layout == kLayoutSymbol)
                    {
                        if(_sw[kSWCase]->getState() == 1)
                        {
                            for(int j = 0; j < 26; j++)
                            {
                                _btn[j]->setLabel(kKeyAlphaMapNumber[j]);
                                _layout = kLayoutNumber;
                            }
                        }
                        else
                        {
                            for(int j = 0; j < 26; j++)
                            {
                                _btn[j]->setLabel(kKeyAlphaMapSymbol[j]);
                                _layout = kLayoutSymbol;
                            }
                        }
                        _sw[kSWCase]->UpdateState(-1, -1);
                        Draw(UPDATE_MODE_NONE);
                        M5.EPD.UpdateFull(UPDATE_MODE_GL16);
                    }
                    else
                    {
                        if(_sw[kSWCase]->getState() == 1)
                        {
                            for(int j = 0; j < 26; j++)
                            {
                                _btn[j]->setLabel(kKeyAlphaMapLowerCase[j]);
                                _layout = kLayoutLowerAlpha;
                            }
                        }
                        else
                        {
                            for(int j = 0; j < 26; j++)
                            {
                                _btn[j]->setLabel(kKeyAlphaMapUpperCase[j]);
                                _layout = kLayoutUpperAlpha;
                            }
                        }
                        _sw[kSWCase]->UpdateState(-1, -1);
                        Draw(UPDATE_MODE_NONE);
                        M5.EPD.UpdateFull(UPDATE_MODE_GL16);
                    }
                    break;
                }
                case kKeyNumber:
                {
                    if(_sw[kSWNumber]->getState() == 1)
                    {
                        _sw[kSWCase]->setState(0);
                        _sw[kSWCase]->Canvas(0)->fillCanvas(0);
                        _sw[kSWCase]->Canvas(1)->fillCanvas(0);
                        _sw[kSWCase]->Canvas(0)->drawRect(0, 0, _sw[kSWCase]->getW(), _sw[kSWCase]->getH(), 15);
                        _sw[kSWCase]->Canvas(1)->drawRect(0, 0, _sw[kSWCase]->getW(), _sw[kSWCase]->getH(), 15);
                        _sw[kSWCase]->Canvas(0)->pushImage(_sw[kSWCase]->getW() / 2 - 16, _sw[kSWCase]->getH() / 2 - 16, 32, 32, ImageResource_upper_32x32);
                        _sw[kSWCase]->Canvas(1)->pushImage(_sw[kSWCase]->getW() / 2 - 16, _sw[kSWCase]->getH() / 2 - 16, 32, 32, ImageResource_upper_32x32);
                        _sw[kSWCase]->Canvas(1)->ReverseColor();

                        for(int j = 0; j < 26; j++)
                        {
                            _btn[j]->setLabel(kKeyAlphaMapLowerCase[j]);
                            _layout = kLayoutLowerAlpha;
                        }
                    }
                    else
                    {
                        _sw[kSWCase]->setState(0);
                        _sw[kSWCase]->SetLabel(0, "#+-");
                        _sw[kSWCase]->SetLabel(1, "123");

                        for(int j = 0; j < 26; j++)
                        {
                            _btn[j]->setLabel(kKeyAlphaMapNumber[j]);
                            _layout = kLayoutNumber;
                        }
                    }
                    _sw[kSWNumber]->UpdateState(-1, -1);
                    Draw(UPDATE_MODE_NONE);
                    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
                    break;
                }
            }
        }
    }
}

String EPDGUI_Keyboard::getData(void)
{
    String data = _data;
    _data = "";
    return data;   
}