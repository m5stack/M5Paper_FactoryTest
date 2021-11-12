#include <stack>
#include <map>
#include <list>
#include "epdgui.h"

typedef struct
{
    Frame_Base* frame;
    epdgui_args_vector_t args;
}frame_struct_t;

std::list<EPDGUI_Base*> epdgui_object_list;
uint32_t obj_id = 1;
Frame_Base* wait_for_delete = NULL;
std::stack <Frame_Base*> frame_stack;
std::map<String, frame_struct_t> frame_map;
uint8_t frame_switch_count = 0;
bool _is_auto_update = true;

uint16_t _is_last_finger_up = 0xFFFF;
uint16_t _last_pos_x = 0xFFFF, _last_pos_y = 0xFFFF;

uint32_t g_last_active_time_millis = 0;

void EPDGUI_AddObject(EPDGUI_Base* object)
{
    object->SetID(obj_id);
    obj_id++;
    epdgui_object_list.push_back(object);
}

void EPDGUI_Draw(m5epd_update_mode_t mode)
{
    for(std::list<EPDGUI_Base*>::iterator p = epdgui_object_list.begin(); p != epdgui_object_list.end(); p++)
    {
        (*p)->Draw(mode);
    }
}

void EPDGUI_Process(void)
{
    for(std::list<EPDGUI_Base*>::iterator p = epdgui_object_list.begin(); p != epdgui_object_list.end(); p++)
    {
        (*p)->UpdateState(-1, -1);
    }
}

void EPDGUI_Process(int16_t x, int16_t y)
{
    for(std::list<EPDGUI_Base*>::iterator p = epdgui_object_list.begin(); p != epdgui_object_list.end(); p++)
    {
        // log_d("%d, %d -> %d, %d, %d, %d", x, y, (*p)->getX(), (*p)->getY(), (*p)->getRX(), (*p)->getBY());
        (*p)->UpdateState(x, y);
    }
}

void EPDGUI_Clear(void)
{
    epdgui_object_list.clear();
}

void EPDGUI_Run(Frame_Base* frame)
{
    uint32_t last_active_time = 0;

    if(frame->isRun() == 0)
    {
        frame->exit();
        log_d("Exit %s", frame->GetFrameName().c_str());
        if(wait_for_delete != NULL)
        {
            delete wait_for_delete;
            wait_for_delete = NULL;
        }
        return;
    }

    EPDGUI_Draw(UPDATE_MODE_NONE);
    if((frame->GetFrameID() == 1) || (frame_switch_count > 3))
    {
        frame_switch_count = 0;
        M5.EPD.UpdateFull(UPDATE_MODE_GC16);
    }
    else
    {
        M5.EPD.UpdateFull(UPDATE_MODE_GL16);
        frame_switch_count++;
    }

    while (1)
    {
        if((frame->isRun() == 0) || (frame->run() == 0))
        {
            frame->exit();
            M5.EPD.Clear(true);
            log_d("Exit %s", frame->GetFrameName().c_str());
            if(wait_for_delete != NULL)
            {
                log_d("delete %s", wait_for_delete->GetFrameName().c_str());
                delete wait_for_delete;
                wait_for_delete = NULL;
            }
            return;
        }

        if (M5.TP.avaliable())
        {
            M5.TP.update();
            uint16_t is_finger_up = M5.TP.isFingerUp() ? 1 : 0;
            uint16_t pos_x = M5.TP.readFingerX(0);
            uint16_t pos_y = M5.TP.readFingerY(0);
            // Avoid duplicate events
            if (_is_last_finger_up != is_finger_up || _last_pos_x != pos_x || _last_pos_y != pos_y)
            {
                EPDGUI_UpdateGlobalLastActiveTime();
                _last_pos_x = pos_x;
                _last_pos_y = pos_y;
                _is_last_finger_up = is_finger_up;
                if(is_finger_up)
                {
                    EPDGUI_Process();
                    last_active_time = millis();
                }
                else
                {
                    EPDGUI_Process(pos_x, pos_y);
                    last_active_time = 0;
                }
            }
            
            
            M5.TP.flush();
        }

        if((last_active_time != 0) && (millis() - last_active_time > 2000))
        {
            if(M5.EPD.UpdateCount() > 4)
            {
                M5.EPD.ResetUpdateCount();
                if(_is_auto_update)
                {
                    M5.EPD.UpdateFull(UPDATE_MODE_GL16);
                }
            }
            last_active_time = 0;
        }
    }
}

void EPDGUI_MainLoop(void)
{
    if((!frame_stack.empty()) && (frame_stack.top() != NULL))
    {
        Frame_Base *frame = frame_stack.top();
        log_d("Run %s", frame->GetFrameName().c_str());
        EPDGUI_Clear();
        _is_auto_update = true;
        frame->init(frame_map[frame->GetFrameName()].args);
        EPDGUI_Run(frame);
    }
}

void EPDGUI_AddFrame(String name, Frame_Base* frame)
{
    frame_struct_t f;
    f.frame = frame;
    frame_map.insert(std::pair<String, frame_struct_t>(name, f));
}

void EPDGUI_AddFrameArg(String name, int n, void* arg)
{
    if(frame_map.count(name) == 0)
    {
        return;
    }
    if(frame_map[name].args.size() > n)
    {
        frame_map[name].args[n] = arg;
    }
    else
    {
        frame_map[name].args.push_back(arg);
    }
    log_d("%d", frame_map[name].args.size());
}

Frame_Base* EPDGUI_GetFrame(String name)
{
    if(frame_map.count(name) > 0)
    {
        return frame_map[name].frame;
    }
    return NULL;
}

void EPDGUI_PushFrame(Frame_Base* frame)
{
    frame_stack.push(frame);
}

void EPDGUI_PopFrame(bool isDelete)
{
    if(isDelete)
    {
        wait_for_delete = frame_stack.top();
    }
    frame_stack.pop();
}

void EPDGUI_OverwriteFrame(Frame_Base* frame)
{
    while(!frame_stack.empty())
    {
        frame_stack.pop();
    }
    frame_stack.push(frame);
}

void EPDGUI_SetAutoUpdate(bool isAuto)
{
    _is_auto_update = isAuto;
}

/// Update active time to avoid power saving
void EPDGUI_UpdateGlobalLastActiveTime()
{
    g_last_active_time_millis = millis();
}
