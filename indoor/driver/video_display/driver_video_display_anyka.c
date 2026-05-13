#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "db_common.h"
#include "driver_video_display.h"
#include "gui_display/driver_gui_display.h"

static void *_driver_video_display_open(void *arg)
{

    return NULL;
}

static int _driver_video_display_close(void *arg)
{

    return 0;
}

static int _driver_video_display_read(void *arg, void *data, int length)
{
    return 0;
}

static int _driver_video_display_write(void *arg, void *data, int length)
{

    return 0;
}

static int _driver_video_display_ioctl(void *arg, int cmd, void *param, void *data)
{

    return 0;
}

db_hal_driver_t db_hal_video_display_driver =
    {
        .open = _driver_video_display_open,
        .close = _driver_video_display_close,
        .read = _driver_video_display_read,
        .write = _driver_video_display_write,
        .ioctl = _driver_video_display_ioctl,
};