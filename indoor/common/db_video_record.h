#ifndef _DB_VIDEO_RECORD_H_
#define _DB_VIDEO_RECORD_H_

#include "db_hal_driver.h"

typedef struct
{
    const char *path;

    int channel;
    int rate;

    enum
    {
        RECORD_MODE_VDEC = 1 << 0,
        RECORD_MODE_ADEC = 1 << 1,
        RECORD_MODE_DISP = 1 << 4,
        RECORD_MODE_SOUND = 1 << 5,
    } mode;

    void *user_data;
} db_video_record_config;

void *db_video_record_start(db_video_record_config *cfg);

int db_video_record_write_nal(void *context, unsigned char *data, int size);

int db_video_record_write_mic(void *context, unsigned char *data, int size);

int db_video_record_write_spk(void *context, unsigned char *data, int size);

int db_video_record_stop(void *context);

#endif // _DB_VIDEO_RECORD_H_