#ifndef _DB_VIDEO_PLAYER_H_
#define _DB_VIDEO_PLAYER_H_

#include "db_hal_driver.h"

typedef struct
{
    const char *path;

    db_hal_frame_cb v_pkt_cb;
    db_hal_frame_cb a_pkt_cb;
    db_hal_frame_cb v_frame_cb;
    db_hal_frame_cb a_frame_cb;
    db_hal_frame_cb finish_cb;

    int disp_x;
    int disp_y;
    int disp_w;
    int disp_h;

    int channel;
    int rate;

    enum
    {
        PLAYER_MODE_VDEC = 1 << 0,
        PLAYER_MODE_ADEC = 1 << 1,
        PLAYER_MODE_DISP = 1 << 4,
        PLAYER_MODE_SOUND = 1 << 5,
    } mode;

    void *user_data;
} db_video_player_config;

void *db_video_player_start(db_video_player_config *cfg);

int db_video_player_stop(void *context);

#endif // _DB_VIDEO_PLAYER_H_