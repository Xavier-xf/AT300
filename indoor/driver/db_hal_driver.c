#include "db_hal_driver.h"

extern db_hal_driver_t db_hal_gui_display_driver;
extern db_hal_driver_t db_hal_video_display_driver;
extern db_hal_driver_t db_hal_input_event_driver;
extern db_hal_driver_t db_hal_video_decoder_driver;
extern db_hal_driver_t db_hal_audio_decoder_driver;
extern db_hal_driver_t db_hal_audio_encoder_driver;
extern db_hal_driver_t db_hal_audio_output_driver;

#define DB_HAL_OPEN_FUNC(func)                   \
    void *db_hal_##func##_open(void *cfg)        \
    {                                            \
        return db_hal_##func##_driver.open(cfg); \
    }

#define DB_HAL_CLOSE_FUNC(func)                   \
    int db_hal_##func##_close(void *cfg)          \
    {                                             \
        return db_hal_##func##_driver.close(cfg); \
    }

#define DB_HAL_READ_FUNC(func)                                  \
    int db_hal_##func##_read(void *ctx, void *data, int length) \
    {                                                           \
        return db_hal_##func##_driver.read(ctx, data, length);  \
    }

#define DB_HAL_WRITE_FUNC(func)                                  \
    int db_hal_##func##_write(void *ctx, void *data, int length) \
    {                                                            \
        return db_hal_##func##_driver.write(ctx, data, length);  \
    }

#define DB_HAL_IOCTL_FUNC(func)                                          \
    int db_hal_##func##_ioctl(void *ctx, int cmd, void *arg, void *data) \
    {                                                                    \
        return db_hal_##func##_driver.ioctl(ctx, cmd, arg, data);        \
    }

#define DB_HAL_FUNC_DEFINE(func) \
    DB_HAL_OPEN_FUNC(func)       \
    DB_HAL_CLOSE_FUNC(func)      \
    DB_HAL_READ_FUNC(func)       \
    DB_HAL_WRITE_FUNC(func)      \
    DB_HAL_IOCTL_FUNC(func)

DB_HAL_FUNC_DEFINE(gui_display);
DB_HAL_FUNC_DEFINE(video_display);
DB_HAL_FUNC_DEFINE(input_event);
DB_HAL_FUNC_DEFINE(video_decoder);
DB_HAL_FUNC_DEFINE(audio_decoder);
DB_HAL_FUNC_DEFINE(audio_encoder);
DB_HAL_FUNC_DEFINE(audio_output);
