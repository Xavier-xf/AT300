#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>

#include "db_common.h"
#include "driver_input_event.h"

typedef struct
{
    SDL_Event event;
    void *gui_context;
} input_event_private_data;

static int sdl_mouse_handler(SDL_Event *event, db_input_event_t *map)
{
    switch (event->type)
    {
    case SDL_MOUSEBUTTONUP:
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            map->state = 0;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            map->state = 1;
            map->x = event->motion.x;
            map->y = event->motion.y;
        }
        break;
    case SDL_MOUSEMOTION:
        map->x = event->motion.x;
        map->y = event->motion.y;
        break;

    case SDL_FINGERUP:
        map->state = 0;
        map->x = event->tfinger.x;
        map->y = event->tfinger.y;
        break;
    case SDL_FINGERDOWN:
        map->state = 1;
        map->x = event->tfinger.x;
        map->y = event->tfinger.y;
        break;
    case SDL_FINGERMOTION:
        map->x = event->tfinger.x;
        map->y = event->tfinger.y;
        break;
    default:
        return -1;
        break;
    }
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
    d->gui_context = arg;
    return d;
}

int _driver_input_event_write(void *ctx, void *data, int length)
{
    return 0;
}

int _driver_input_event_read(void *ctx, void *data, int length)
{
    int reslut = -1;
    input_event_private_data *d = (input_event_private_data *)ctx;
    db_input_event_t *map = (db_input_event_t *)data;

    while (SDL_PollEvent(&d->event))
    {
        if (sdl_mouse_handler(&d->event, map) == 0)
        {
            reslut = 0;
        }
        if (d->event.type == SDL_QUIT)
        {
            exit(0);
        }
    }
    return reslut;
}

int _driver_input_event_close(void *ctx)
{
    input_event_private_data *d = (input_event_private_data *)ctx;
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
