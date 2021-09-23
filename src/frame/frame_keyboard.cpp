#include "frame_keyboard.h"

uint16_t textsize = 26;

void key_textclear_cb(epdgui_args_vector_t &args)
{
    ((EPDGUI_Textbox*)(args[0]))->SetText("");
}

void key_textsize_plus_cb(epdgui_args_vector_t &args)
{
    textsize += 4;
    if(textsize > 96)
    {
        textsize = 96;
    }
    char buf[10];
    sprintf(buf, "%d", textsize);
    ((EPDGUI_Button*)(args[1]))->setLabel(buf);
    ((EPDGUI_Button*)(args[1]))->Draw(UPDATE_MODE_GL16);
    ((EPDGUI_Textbox*)(args[0]))->SetTextSize(textsize);
}

void key_textsize_minus_cb(epdgui_args_vector_t &args)
{
    textsize -= 4;
    if(textsize < 12)
    {
        textsize = 12;
    }
    char buf[10];
    sprintf(buf, "%d", textsize);
    ((EPDGUI_Button*)(args[1]))->setLabel(buf);
    ((EPDGUI_Button*)(args[1]))->Draw(UPDATE_MODE_GL16);
    ((EPDGUI_Textbox*)(args[0]))->SetTextSize(textsize);
}

void key_textsize_reset_cb(epdgui_args_vector_t &args)
{
    textsize = 26;
    char buf[10];
    sprintf(buf, "%d", textsize);
    ((EPDGUI_Button*)(args[1]))->setLabel(buf);
    ((EPDGUI_Button*)(args[1]))->Draw(UPDATE_MODE_GL16);
    ((EPDGUI_Textbox*)(args[0]))->SetTextSize(textsize);
}

Frame_Keyboard::Frame_Keyboard(bool isHorizontal) : Frame_Base()
{
    _frame_name = "Frame_Keyboard";
    uint8_t language = GetLanguage();
    if(isHorizontal)
    {
        inputbox = new EPDGUI_Textbox(84, 25, 712, 250);
        if(language == LANGUAGE_JA)
            key_textclear = new EPDGUI_Button("削除", 804, 25, 72, 120);
        else if(language == LANGUAGE_ZH)
            key_textclear = new EPDGUI_Button("清屏", 804, 25, 72, 120);
        else
            key_textclear = new EPDGUI_Button("CLR", 804, 25, 72, 120);

        key_textsize_plus = new EPDGUI_Button("+", 804, 157, 72, 40);
        key_textsize_reset = new EPDGUI_Button("26", 804, 196, 72, 40);
        key_textsize_minus = new EPDGUI_Button("-", 804, 235, 72, 40);
    }
    else
    {
        const uint16_t kKeyBaseY = 628;
        inputbox = new EPDGUI_Textbox(4, 100, 532, 512);
        if(language == LANGUAGE_JA)
            key_textclear = new EPDGUI_Button("削除", 4, kKeyBaseY, 260, 52);
        else if(language == LANGUAGE_ZH)
            key_textclear = new EPDGUI_Button("清屏", 4, kKeyBaseY, 260, 52);
        else
            key_textclear = new EPDGUI_Button("CLR", 4, kKeyBaseY, 260, 52);

        key_textsize_plus = new EPDGUI_Button("+", 448, kKeyBaseY, 88, 52);
        key_textsize_reset = new EPDGUI_Button("26", 360, kKeyBaseY, 88, 52);
        key_textsize_minus = new EPDGUI_Button("-", 272, kKeyBaseY, 88, 52);
    }
    
    inputbox->SetState(EPDGUI_Textbox::EVENT_PRESSED);

    keyboard = new EPDGUI_Keyboard(isHorizontal);
    
    key_textclear->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)inputbox);
    key_textclear->Bind(EPDGUI_Button::EVENT_RELEASED, key_textclear_cb);
    key_textsize_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)inputbox);
    key_textsize_plus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void*)key_textsize_reset);
    key_textsize_plus->Bind(EPDGUI_Button::EVENT_RELEASED, key_textsize_plus_cb);
    key_textsize_reset->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)inputbox);
    key_textsize_reset->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void*)key_textsize_reset);
    key_textsize_reset->Bind(EPDGUI_Button::EVENT_RELEASED, key_textsize_reset_cb);
    key_textsize_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)inputbox);
    key_textsize_minus->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void*)key_textsize_reset);
    key_textsize_minus->Bind(EPDGUI_Button::EVENT_RELEASED, key_textsize_minus_cb);

    if(language == LANGUAGE_JA)
    {
        exitbtn("ホーム");
        _canvas_title->drawString("鍵盤", 270, 34);
    }
    else if(language == LANGUAGE_ZH)
    {
        exitbtn("主页");
        _canvas_title->drawString("键盘", 270, 34);
    }
    else
    {
        exitbtn("Home");
        _canvas_title->drawString("Keyboard", 270, 34);
    }
    
    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}

Frame_Keyboard::~Frame_Keyboard()
{
    delete inputbox;
    delete keyboard;
    delete key_textclear;
    delete key_textsize_plus;
    delete key_textsize_reset;
    delete key_textsize_minus;
}

int Frame_Keyboard::init(epdgui_args_vector_t &args)
{
    _is_run = 1;
    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(inputbox);
    EPDGUI_AddObject(keyboard);
    EPDGUI_AddObject(_key_exit);
    EPDGUI_AddObject(key_textclear);
    EPDGUI_AddObject(key_textsize_plus);
    EPDGUI_AddObject(key_textsize_reset);
    EPDGUI_AddObject(key_textsize_minus);
    return 6;
}

int Frame_Keyboard::run(void)
{
    Frame_Base::run();
    inputbox->AddText(keyboard->getData());
    return 1;
}
