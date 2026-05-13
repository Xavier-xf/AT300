#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>

#include "db_common.h"
#include "gui_display/driver_gui_display.h"
#include "driver_input_event.h"

#define EVDEV_NAME "/dev/input/event0"

typedef struct
{
    int evdev_fd;
} input_event_private_data;

static int _input_device_open(input_event_private_data *ctx)
{
    ctx->evdev_fd = open(EVDEV_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
    if (ctx->evdev_fd == -1)
    {
        perror("unable open evdev interface:");
        return -1;
    }

    fcntl(ctx->evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    return 0;
}

static int _input_device_read(input_event_private_data *ctx, db_input_event_t *map)
{
    static int evdev_root_x = 0;
    static int evdev_root_y = 0;
    static int evdev_state = 0;
    struct input_event in;
    while (read(ctx->evdev_fd, &in, sizeof(struct input_event)) > 0)
    {
        if (in.type == EV_REL)
        {
            if (in.code == REL_X)
#if EVDEV_SWAP_AXES
                evdev_root_y += in.value;
#else
                evdev_root_x += in.value;
#endif
            else if (in.code == REL_Y)
#if EVDEV_SWAP_AXES
                evdev_root_x += in.value;
#else
                evdev_root_y += in.value;
#endif
        }
        else if (in.type == EV_ABS)
        {
            if (in.code == ABS_X)
#if EVDEV_SWAP_AXES
                evdev_root_y = in.value;
#else
                evdev_root_x = in.value;
#endif
            else if (in.code == ABS_Y)
#if EVDEV_SWAP_AXES
                evdev_root_x = in.value;
#else
                evdev_root_y = in.value;
#endif
            else if (in.code == ABS_MT_POSITION_X)
#if EVDEV_SWAP_AXES
                evdev_root_y = in.value;
#else
                evdev_root_x = in.value;
#endif
            else if (in.code == ABS_MT_POSITION_Y)
#if EVDEV_SWAP_AXES
                evdev_root_x = in.value;
#else
                evdev_root_y = in.value;
#endif
            else if (in.code == 48)
            {
                if (in.value == 0)
                {
                    evdev_state = 0;
                }
                else
                {
                    evdev_state = 1;
                }
                db_log_debug("state:%d x:%d y:%d", map->state, evdev_root_x, evdev_root_y);
            }
        }
        else if (in.type == EV_KEY)
        {

            if (in.code == BTN_MOUSE || in.code == BTN_TOUCH)
            {
                if (in.value == 0)
                    evdev_state = 0;
                else if (in.value == 1)
                    evdev_state = 1;
            }
        }
    }

// #define SRC_HOR_RES 1024
// #define SRC_VER_RES 600
#ifdef SRC_HOR_RES && SRC_VER_RES
    map->x = evdev_root_x * MY_DISP_HOR_RES / SRC_HOR_RES; // SRC_HOR_RES -> MY_DISP_HOR_RES
    map->y = evdev_root_y * MY_DISP_VER_RES / SRC_VER_RES; // SRC_VER_RES -> MY_DISP_VER_RES
#else
    map->x = evdev_root_x;
    map->y = evdev_root_y;
#endif
    if (map->x < 0)
        map->x = 0;
    if (map->y < 0)
        map->y = 0;
    if (map->x >= MY_DISP_HOR_RES)
        map->x = MY_DISP_HOR_RES - 1;
    if (map->y >= MY_DISP_VER_RES)
        map->y = MY_DISP_VER_RES - 1;
    map->state = evdev_state;
    // map->x = MY_DISP_HOR_RES - 1 - map->x; // 水平镜像
    // map->y = MY_DISP_VER_RES - 1 - map->y; // 垂直镜像

    return 0;
}

static int _input_device_close(input_event_private_data *ctx)
{
    close(ctx->evdev_fd);
    return 0;
}

void *_driver_input_event_open(void *arg)
{
    input_event_private_data *d = (input_event_private_data *)malloc(sizeof(input_event_private_data));
    if (!d)
    {
        db_log_error("malloc\n");
        return NULL;
    }

    memset(d, 0, sizeof(input_event_private_data));
    _input_device_open(d);
    return d;
}

int _driver_input_event_write(void *ctx, void *data, int length)
{
    return 0;
}

int _driver_input_event_read(void *ctx, void *data, int length)
{
    input_event_private_data *d = (input_event_private_data *)ctx;
    db_input_event_t *map = (db_input_event_t *)data;

    _input_device_read(d, map);
    return 0;
}

int _driver_input_event_close(void *ctx)
{
    input_event_private_data *d = (input_event_private_data *)ctx;
    _input_device_close(ctx);
    free(d);
    return 0;
}

int _driver_input_event_ioctl(void *ctx, int cmd, void *arg, void *data)
{
    return 0;
}

db_hal_driver_t db_hal_input_event_driver =
    {
        .open = _driver_input_event_open,
        .read = _driver_input_event_read,
        .write = _driver_input_event_write,
        .close = _driver_input_event_close,
        .ioctl = _driver_input_event_ioctl,
};
