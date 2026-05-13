#include "driver_audio_decoder.h"



static void *_driver_audio_decoder_open(void *arg)
{
    return NULL;
}

static int _driver_audio_decoder_write(void *context, void *data, int length)
{
    return 0;
}

static int _driver_audio_decoder_read(void *context, void *data, int length)
{

    return 0;
}

static int _driver_audio_decoder_close(void *context)
{

    return 0;
}

static int _driver_audio_decoder_ioctl(void *context, int cmd, void *arg, void *data)
{
    
    return 0;
}

db_hal_driver_t db_hal_audio_decoder_driver = {
    .open = _driver_audio_decoder_open,
    .write = _driver_audio_decoder_write,
    .read = _driver_audio_decoder_read,
    .close = _driver_audio_decoder_close,
    .ioctl = _driver_audio_decoder_ioctl,
};