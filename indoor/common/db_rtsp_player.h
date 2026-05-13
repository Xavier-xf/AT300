#ifndef _DB_RTSP_PLAYER_H_
#define _DB_RTSP_PLAYER_H_

#include "db_hal_driver.h"

typedef struct
{
    const char *rtsp_url;

    db_hal_frame_cb v_pkt_cb;
    db_hal_frame_cb a_pkt_cb;
    db_hal_frame_cb v_frame_cb;
    db_hal_frame_cb a_frame_cb;
    db_hal_frame_cb finish_cb;

    int x;
    int y;
    int width;
    int height;

    int channel;
    int rate;

    enum
    {
        VIDEO_PLAYER_MODE_VDEC = 1 << 0,
        VIDEO_PLAYER_MODE_ADEC = 1 << 1,
        VIDEO_PLAYER_MODE_DISP = 1 << 2,
        VIDEO_PLAYER_MODE_SOUND = 1 << 3,
    } mode;

    void *user_data;
} db_rtsp_player_config;

void *db_rtsp_player_start(db_rtsp_player_config *cfg);

int db_rtsp_player_stop(void *context);

#endif // _DB_RTSP_PLAYER_H_