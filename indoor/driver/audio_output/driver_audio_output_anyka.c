
#include "driver_audio_output.h"

static void *_driver_audio_output_open(void *arg)
{
    return NULL;
}

static int _driver_audio_output_write(void *ctx, void *data, int length)
{
    return 0;
}

static int _driver_audio_output_read(void *ctx, void *data, int length)
{
    return 0;
}

static int _driver_audio_output_close(void *ctx)
{
    return 0;
}

static int _driver_audio_output_ioctl(void *ctx, int cmd, void *arg, void *data)
{
    return 0;
}

db_hal_driver_t db_hal_audio_output_driver = {
    .open = _driver_audio_output_open,
    .close = _driver_audio_output_close,
    .read = _driver_audio_output_read,
    .write = _driver_audio_output_write,
    .ioctl = _driver_audio_output_ioctl,
};
