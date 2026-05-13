#ifndef _DRIVER_AUDIO_OUTPUT_H_
#define _DRIVER_AUDIO_OUTPUT_H_

#include "../db_hal_driver.h"

enum
{
    AUDIO_OUTPUT_USER_ENABLE_CMD,
    AUDIO_OUTPUT_USER_FREE_CMD,
    AUDIO_OUTPUT_USER_TOTAL_CMD,
    AUDIO_OUTPUT_USER_FRAME_NUM_CMD,
    AUDIO_OUTPUT_DEVICE_CONFIG_GET_CMD,
    AUDIO_OUTPUT_USER_FULSH_CMD,
    AUDIO_OUTPUT_USER_VALID_CMD,
    AUDIO_OUTPUT_CONFIG_GET_CMD,
    AUDIO_OUTPUT_VOLUME_SET_CMD, // 0~100
};

typedef struct
{
    int channel;
    unsigned int rate;
} db_audio_output_config;

static inline void audio_output_accumulate(int32_t *sum, int16_t *contrib, int nwords)
{
    for (int i = 0; i < nwords; ++i)
    {
        sum[i] += contrib[i];
    }
}

static inline int16_t amplitude_limit(int32_t s)
{
    if (s > 32767)
        return 32767;
    if (s < -32768)
        return -32768;
    return (int16_t)s;
}

static inline void audio_output_gain(int16_t *samples, int nsamples, float gain)
{
    if (gain != 1)
    {
        for (int i = 0; i < nsamples; ++i)
        {
            samples[i] = amplitude_limit((int)(gain * (float)samples[i]));
        }
    }
}
#endif // _DRIVER_AUDIO_OUTPUT_H_
