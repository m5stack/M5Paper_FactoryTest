#ifndef _FRAME_PICTUREVIEWER_H_
#define _FRAME_PICTUREVIEWER_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"

class Frame_PictureViewer : public Frame_Base
{
public:
    Frame_PictureViewer(String path);
    ~Frame_PictureViewer();
    int init(epdgui_args_vector_t &args);
    int run();
    void err(String info);

private:
    M5EPD_Canvas *_canvas_picture;
    String _pic_path;
    bool _is_first;
};

#endif //_FRAME_PICTUREVIEWER_H_