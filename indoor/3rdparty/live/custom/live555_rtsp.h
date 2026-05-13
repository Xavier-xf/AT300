#ifndef _LIVE555_RTSP_H_
#define _LIVE555_RTSP_H_

#include "db_hal_driver.h"

typedef struct
{
    const char *url;
    db_hal_frame_cb v_recv_cb;
    db_hal_frame_cb a_recv_cb;
    void *user_data;
} rtsp_media_client_config;

void *rtsp_media_client_open(rtsp_media_client_config *cfg);

int rtsp_media_client_close(void *context);

#endif // _LIVE555_RTSP_H_