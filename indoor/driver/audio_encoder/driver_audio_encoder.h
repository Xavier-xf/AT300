#ifndef _DRIVER_AUDIO_ENCODER_H_
#define _DRIVER_AUDIO_ENCODER_H_

#include "../db_hal_driver.h"

enum
{
    AUDIO_ENCODER_PACKET_RELEASE_CMD,
};

typedef struct
{
    enum
    {
        AUDIO_ENCODER_TYPE_AAC,
        AUDIO_ENCODER_TYPE_MP3,
        AUDIO_ENCODER_TYPE_G711,
    } type;
    int rate;
} db_audio_encoder_config;

#endif // _DRIVER_AUDIO_ENCODER_H_