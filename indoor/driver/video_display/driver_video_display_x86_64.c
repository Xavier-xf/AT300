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

typedef struct
{
    struct SwsContext *sws_ctx;

    db_hal_position_t pos;

    unsigned char *buffer;

} video_display_private_data;

static void *_driver_video_display_open(void *arg)
{
    db_video_display_config *cfg = (db_video_display_config *)arg;

    video_display_private_data *ctx = (video_display_private_data *)malloc(sizeof(video_display_private_data));
    if (!ctx)
    {
        db_log_error("malloc\n");
        return NULL;
    }

    memset(ctx, 0, sizeof(video_display_private_data));
    ctx->pos = cfg->pos;
    ctx->buffer = (unsigned char *)malloc(cfg->pos.w * cfg->pos.h * 3 / 2);

    return ctx;
}

static int _driver_video_display_close(void *arg)
{
    video_display_private_data *ctx = (video_display_private_data *)arg;
    if (!ctx)
    {
        db_log_error("video display context is nullptr\n");
        return -1;
    }

    display_device_video_buffer_clean();

    free(ctx->buffer);
    free(ctx);
    return 0;
}

static int _driver_video_display_read(void *arg, void *data, int length)
{
    return 0;
}

static int _driver_video_display_write(void *arg, void *data, int length)
{
    video_display_private_data *ctx = (video_display_private_data *)arg;
    db_hal_video_frame_t *frame = (db_hal_video_frame_t *)data;

    if (length != sizeof(db_hal_video_frame_t))
    {
        db_log_error("db_hal_video_frame_t\n");
        return -1;
    }

    display_device_video_frame_write(frame, &ctx->pos);

    return 0;
}

static int _driver_video_display_ioctl(void *arg, int cmd, void *param, void *data)
{
    // video_display_private_data *ctx = (video_display_private_data *)arg;

    switch (cmd)
    {
    // case GUI_DISPLAY_VIDEO_ENABLE_CMD:
    //     ctx->is_video_mode = *(bool *)param;
    //     break;
    // case GUI_DISPLAY_FRAME_REFRESH_CMD:
    //     sem_post(&ctx->refresh_sem);
    //     break;
    default:
        break;
    }

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