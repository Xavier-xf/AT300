#ifndef _DRIVER_GUI_DISPLAY_H_
#define _DRIVER_GUI_DISPLAY_H_

#include "../db_hal_driver.h"

#define MY_DISP_HOR_RES 1024
#define MY_DISP_VER_RES 600

enum
{
    GUI_DISPLAY_VIDEO_ENABLE_CMD,
    GUI_DISPLAY_FRAME_REFRESH_CMD,
};

typedef struct
{
    char path[128];
    int layer;
    int color;
    int fmt;
    int source_w, source_h;
    int x, y;
    int scale_w, scale_h;
    int frames;
} db_gui_framebuffer_config_t;

typedef struct
{
    unsigned char *data;
    db_hal_position_t pos;
} db_gui_framebuffer_frame_t;

int display_device_video_frame_write(db_hal_video_frame_t *frame, db_hal_position_t *pos);

int display_device_video_buffer_clean(void);

#endif // _DRIVER_GUI_DISPLAY_H_