#ifndef _DRIVER_VIDEO_DECODER_H_
#define _DRIVER_VIDEO_DECODER_H_

#include "../db_hal_driver.h"

enum
{
    VIDEO_DECODER_FRAME_RELEASE_CMD,
};

typedef struct
{
    enum
    {
        VIDEO_DECODER_TYPE_H264,
        VIDEO_DECODER_TYPE_H265,
        VIDEO_DECODER_TYPE_MJPEG,
    } type;
} db_video_decoder_config;

#endif // _DRIVER_VIDEO_DECODER_H_