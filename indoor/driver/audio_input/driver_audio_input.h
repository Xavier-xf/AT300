#ifndef _AUDIO_INPUT_DRIVER_H_
#define _AUDIO_INPUT_DRIVER_H_
#include "stdint.h"


#define DYC_AUDIO_INPUT_USER_ENABLE_CMD 0X01
#define DYC_AUDIO_INPUT_USER_VAILD_CMD 0X02
#define DYC_AUDIO_INPUT_USER_TOTAL_CMD 0X03
#define DYC_AUDIO_INPUT_USER_FRAME_NUM_CMD 0X04
#define DYC_AUDIO_INPUT_DEVICE_CONFIG_GET_CMD 0X05
#define DYC_AUDIO_INPUT_USER_FLUSH_CMD 0X06

typedef struct {
    int channel; //0:left,1:right,2:立体声
    unsigned int sample;
    int bit;
    int volume;
}dyc_audio_input_config;

#endif
