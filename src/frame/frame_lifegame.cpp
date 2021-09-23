#include "frame_lifegame.h"

// #define CELL_W 10
// #define CELL_H 10
// #define CELL_X 54 + 2
// #define CELL_Y 88 + 2


#define CELL_W 4
#define CELL_H 4
#define CELL_X 135 + 2
#define CELL_Y 222 + 2

// #define CELL_W 2
// #define CELL_H 2
// #define CELL_X 270 + 2
// #define CELL_Y 444 + 2

static uint8_t lifegame_cells_flag = 0;
M5EPD_Canvas lifegame_cells_0(&M5.EPD);
M5EPD_Canvas lifegame_cells_1(&M5.EPD);
M5EPD_Canvas lifegame_canvas_0(&M5.EPD);
M5EPD_Canvas lifegame_canvas_1(&M5.EPD);

void LifeGame_RandomCell()
{
    lifegame_cells_flag = 0;
    uint8_t randbuf[CELL_X];
    for (int y = 0; y < CELL_Y; y++)
    {
        esp_fill_random(randbuf, CELL_X);
        for (int x = 0; x < CELL_X; x++)
        {
            lifegame_cells_0.drawPixel(x, y, randbuf[x] & 0x01 ? 15 : 0);
        }
    }
    lifegame_cells_1.fillCanvas(0);
}

uint8_t LifeGame_Count3x3_0(uint8_t x, uint8_t y)
{
    uint8_t count = 0;
    for(int j = y - 1; j < y + 2; j++)
    {
        for(int i = x - 1; i < x + 2; i++)
        {
            if(lifegame_cells_0.readPixel(i, j))
            {
                count++;
            }
        }
    }
    if(lifegame_cells_0.readPixel(x, y))
    {
        count--;
    }
    return count;
}

uint8_t LifeGame_Count3x3_1(uint8_t x, uint8_t y)
{
    uint8_t count = 0;
    for(int j = y - 1; j < y + 2; j++)
    {
        for(int i = x - 1; i < x + 2; i++)
        {
            if(lifegame_cells_1.readPixel(i, j))
            {
                count++;
            }
        }
    }
    if(lifegame_cells_1.readPixel(x, y))
    {
        count--;
    }
    return count;
}

void LifeGame_CellProcess_0(uint8_t x, uint8_t y)
{
    uint8_t count = LifeGame_Count3x3_0(x, y);
    if(lifegame_cells_0.readPixel(x, y))
    {
        //dead
        if(count < 2 || count > 3)
        {
            lifegame_cells_1.drawPixel(x, y, 0);
        }
        else
        {
            lifegame_cells_1.drawPixel(x, y, 15);
        }
    }
    else
    {
        //new life
        if(count == 3)
        {
            lifegame_cells_1.drawPixel(x, y, 15);
        }
        else
        {
            lifegame_cells_1.drawPixel(x, y, 0);
        }
    }
}

void LifeGame_CellProcess_1(uint8_t x, uint8_t y)
{
    uint8_t count = LifeGame_Count3x3_1(x, y);
    if(lifegame_cells_1.readPixel(x, y))
    {
        //dead
        if(count < 2 || count > 3)
        {
            lifegame_cells_0.drawPixel(x, y, 0);
        }
        else
        {
            lifegame_cells_0.drawPixel(x, y, 15);
        }
    }
    else
    {
        //new life
        if(count == 3)
        {
            lifegame_cells_0.drawPixel(x, y, 15);
        }
        else
        {
            lifegame_cells_0.drawPixel(x, y, 0);
        }
    }
}

void LifeGame_RoundProcess()
{
    if(lifegame_cells_flag == 0)
    {
        for (int y = 1; y < CELL_Y - 1; y++)
        {
            for (int x = 1; x < CELL_X - 1; x++)
            {
                LifeGame_CellProcess_0(x, y);
                lifegame_canvas_0.fillRect((x - 1) * CELL_W, (y - 1) * CELL_H, CELL_W, CELL_H, lifegame_cells_1.readPixel(x, y));
            }
        }
        lifegame_cells_flag = 1;
    }
    else
    {
        for (int y = 1; y < CELL_Y - 1; y++)
        {
            for (int x = 1; x < CELL_X - 1; x++)
            {
                LifeGame_CellProcess_1(x, y);
                lifegame_canvas_1.fillRect((x - 1) * CELL_W, (y - 1) * CELL_H, CELL_W, CELL_H, lifegame_cells_0.readPixel(x, y));
            }
        }
        lifegame_cells_flag = 0;
    }
}

void LifeGame_NextGen(void *pvParameters)
{
    LifeGame_RoundProcess();
    vTaskDelete(NULL);
}

int Frame_Lifegame::run()
{
    Frame_Base::run();
    M5.update();
    if(M5.BtnP.wasReleased())
    {
        EPDGUI_UpdateGlobalLastActiveTime();
        LifeGame_RandomCell();
    }
    xTaskCreatePinnedToCore(LifeGame_NextGen, "LifeGame_NextGen", 4096, NULL, 1, NULL, 0);
    if(lifegame_cells_flag == 0)
    {
        lifegame_canvas_1.pushCanvas(0, 72, UPDATE_MODE_DU4);
    }
    else
    {
        lifegame_canvas_0.pushCanvas(0, 72, UPDATE_MODE_DU4);
    }
    log_d("");
    return 1;
}

void Frame_Lifegame::exit()
{
    lifegame_cells_0.deleteCanvas();
    lifegame_cells_1.deleteCanvas();
    lifegame_canvas_0.deleteCanvas();
    lifegame_canvas_1.deleteCanvas();
}

Frame_Lifegame::Frame_Lifegame(void)
{
    _frame_name = "Frame_Lifegame";
 
    uint8_t language = GetLanguage();
    if(language == LANGUAGE_JA)
    {
        exitbtn("ホーム");
        _canvas_title->drawString("ライフゲーム", 270, 34);
    }
    else if(language == LANGUAGE_ZH)
    {
        exitbtn("主页");
        _canvas_title->drawString("生命游戏", 270, 34);
    }
    else
    {
        exitbtn("Home");
        _canvas_title->drawString("Life Game", 270, 34);
    }

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void*)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &Frame_Base::exit_cb);
}

Frame_Lifegame::~Frame_Lifegame(void)
{
}

int Frame_Lifegame::init(epdgui_args_vector_t &args)
{
    _is_run = 1;

    lifegame_cells_0.createCanvas(CELL_X, CELL_Y);
    lifegame_cells_1.createCanvas(CELL_X, CELL_Y);
    lifegame_canvas_0.createCanvas(540, 888);
    lifegame_canvas_1.createCanvas(540, 888);

    lifegame_cells_0.fillCanvas(0);
    lifegame_cells_1.fillCanvas(0);
    lifegame_canvas_0.fillCanvas(0);
    lifegame_canvas_1.fillCanvas(0);

    lifegame_cells_0.pushImage(2, 2, 132, 120, ImageResource_lifegame_seeder_132x120);
    lifegame_cells_flag = 0;

    M5.EPD.Clear();
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_exit);
    return 3;
}