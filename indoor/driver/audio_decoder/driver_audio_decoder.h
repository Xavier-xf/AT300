#ifndef _DRIVER_AUDIO_DECODER_H_
#define _DRIVER_AUDIO_DECODER_H_

#include "../db_hal_driver.h"

enum
{
    AUDIO_DECODER_FRAME_RELEASE_CMD,
};

typedef struct
{
    enum
    {
        AUDIO_DECODER_TYPE_AAC,
        AUDIO_DECODER_TYPE_MP3,
        AUDIO_DECODER_TYPE_PCMU,
    } type;
    int rate;
    int channel;
} db_audio_decoder_config;

#endif // _DRIVER_AUDIO_DECODER_H_