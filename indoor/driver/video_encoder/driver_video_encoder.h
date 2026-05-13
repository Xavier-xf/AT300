#ifndef _VIDEO_ENCODER_H264_DRIVER_H_
#define _VIDEO_ENCODER_H264_DRIVER_H_
#include "driver_interface.h"

#define VIDEO_ENCODER_PACKET_RELEASE_CMD 0X01

typedef struct {
    int bit_rate;
    int width;
    int height;
    int fps;
}video_encoder_h264_config_s;

#endif