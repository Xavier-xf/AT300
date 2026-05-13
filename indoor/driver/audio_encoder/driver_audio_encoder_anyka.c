

#include "driver_audio_encoder.h"

static void *_driver_audio_encoder_open(void *arg)
{
    
    return NULL;
}

static int _driver_audio_encoder_write(void *context, void *data, int length)
{
    
    return 0;
}

static int _driver_audio_encoder_read(void *context, void *data, int length)
{
    
    return 0;
}

static int _driver_audio_encoder_close(void *context)
{
   
    return 0;
}

static int _driver_audio_encoder_ioctl(void *context, int cmd, void *arg, void *data)
{
    
    return 0;
}

db_hal_driver_t db_hal_audio_encoder_driver = {
    .open = _driver_audio_encoder_open,
    .write = _driver_audio_encoder_write,
    .read = _driver_audio_encoder_read,
    .close = _driver_audio_encoder_close,
    .ioctl = _driver_audio_encoder_ioctl,
};