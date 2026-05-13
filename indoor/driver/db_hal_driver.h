#ifndef _DB_HAL_DRIVER_H_
#define _DB_HAL_DRIVER_H_

// #include <stdint.h>
#include "db_common.h"
// #include "gui_display/driver_gui_display.h"
// #include "input_event/driver_input_event.h"

typedef void (*db_hal_frame_cb)(unsigned char *, unsigned int, unsigned long long, void *);

typedef struct
{
    void *(*open)(void *arg);
    int (*write)(void *context, void *data, int len);
    int (*read)(void *context, void *data, int len);
    int (*ioctl)(void *context, int cmd, void *arg, void *data);
    int (*close)(void *context);
} db_hal_driver_t;

typedef struct
{
    int x, y, w, h;
} db_hal_position_t;

typedef struct
{
    unsigned char *data;
    unsigned int size;
    unsigned long long pts;
} db_hal_video_packet_t;

typedef struct
{
    unsigned char *data;
    int width;
    int hight;
} db_hal_video_frame_t;

typedef struct
{
    unsigned char *data[2];
    int size;
    int channel;
    int rate;
} db_hal_audio_frame_t;

typedef struct
{
    unsigned char *data;
    unsigned int size;
    unsigned long long pts;
} db_hal_audio_packet_t;

#define DB_HAL_FUNC_EXTERN(func)                                  \
    void *db_hal_##func##_open(void *cfg);                        \
    int db_hal_##func##_close(void *ctx);                         \
    int db_hal_##func##_read(void *ctx, void *data, int length);  \
    int db_hal_##func##_write(void *ctx, void *data, int length); \
    int db_hal_##func##_ioctl(void *ctx, int cmd, void *arg, void *data);

DB_HAL_FUNC_EXTERN(gui_display);
DB_HAL_FUNC_EXTERN(video_display);
DB_HAL_FUNC_EXTERN(input_event);
DB_HAL_FUNC_EXTERN(video_decoder);
DB_HAL_FUNC_EXTERN(audio_decoder);
DB_HAL_FUNC_EXTERN(audio_encoder);
DB_HAL_FUNC_EXTERN(audio_output);

#endif // _DB_HAL_DRIVER_H_