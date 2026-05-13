#ifndef _VIDEO_CAPTURE_DRIVER_SINGLE_H_
#define _VIDEO_CAPTURE_DRIVER_SINGLE_H_
#include "driver_interface.h"

#define VIDEO_CAPTURE_DISPLAY_FRAME_RELEASE_CMD 0X01
#define VIDEO_CAPTURE_ENCODER_FRAME_GET_CMD 0X02
#define VIDEO_CAPTURE_ENCODER_FRAME_RELEASE_CMD 0X03


typedef struct {
    int fps;
    int width;
    int height;
}capture_param_config_s;

typedef struct {
    capture_param_config_s display;
    capture_param_config_s encoder;
}dyc_video_capture_config_s;



#endif
