#include "driver_interface.h"
#include "video_capture_driver_single.h"
#include "dyc_common.h"
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <dyc_memory_pool.h>

#define RESOURCE_PATH "/home/leo/Pictures/yuv420sp/"

typedef struct {
    pthread_mutex_t mutex;
    int fps;
    int width;
    int height;
    int frame_index;
    unsigned long long last_timestamp;
    int fd;
}capture_channel_private_data;
typedef struct {
    capture_channel_private_data display;
    capture_channel_private_data encoder;
    void* pool;
}x86_64_video_capture_private_data;

#define x86_64_video_capture_param_init(d,cfg) \
    do{ \
        d.width = cfg.width; \
        d.height = cfg.height; \
        d.frame_index = 0; \
        d.fps = cfg.fps; \
        d.last_timestamp = 0; \
        pthread_mutex_init(&d.mutex, NULL);\
    }while(0);

static void* x86_64_video_capture_driver_open(void* arg) {
    x86_64_video_capture_private_data* d = NULL;
    dyc_video_capture_config_s* cfg = (dyc_video_capture_config_s*)arg;

    d = (x86_64_video_capture_private_data*)malloc(sizeof(x86_64_video_capture_private_data));
    if (!d) {
        dyc_error_log("create video private data failed\n");
        return NULL;
    }
    memset(d, 0, sizeof(x86_64_video_capture_private_data));

    x86_64_video_capture_param_init(d->display, cfg->display);
    x86_64_video_capture_param_init(d->encoder, cfg->encoder);

    dyc_memory_pool_config_s pool_cfg;
    memset(&pool_cfg, 0, sizeof(dyc_memory_pool_config_s));
    size_t node_szie = (sizeof(dyc_video_common_s) + 64) / 64 * 64 * 6;
    pool_cfg.max_size = pool_cfg.size = node_szie;
    d->pool = dyc_memory_pool_open(&pool_cfg);
    return d;
}

static int x86_64_video_capture_driver_close(void* context) {
    x86_64_video_capture_private_data* d = (x86_64_video_capture_private_data*)context;
    if (!d) {
        dyc_error_log("vca context is nullpter\n");
        return -1;
    }
    pthread_mutex_lock(&d->display.mutex);
    pthread_mutex_lock(&d->encoder.mutex);
    dyc_memory_pool_close(d->pool);
    if (d->display.fd) {
        close(d->display.fd);
    }
    if (d->encoder.fd) {
        close(d->encoder.fd);
    }
    pthread_mutex_unlock(&d->display.mutex);
    pthread_mutex_unlock(&d->encoder.mutex);

    pthread_mutex_destroy(&d->display.mutex);
    pthread_mutex_destroy(&d->encoder.mutex);

    free(d);
    return 0;
}

static int x86_64_video_capture_driver_write(void* context, void* data, int length) {

    return 0;
}

static int x86_64_video_capture_driver_read(void* context, void* data, int length) {
    x86_64_video_capture_private_data* d = (x86_64_video_capture_private_data*)context;
    if (length != sizeof(dyc_video_common_s)) {
        dyc_error_log("There is an issue with the size of the parameter data passed in.\n");
        return -1;
    }
    pthread_mutex_lock(&d->display.mutex);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long current_timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    unsigned long long interval_timestamp = 1000 / d->display.fps;
    if (abs(current_timestamp - d->display.last_timestamp) < interval_timestamp) {
        pthread_mutex_unlock(&d->display.mutex);
        return -1;
    }
    d->display.last_timestamp = current_timestamp;


    if (d->display.fd == 0) {
        char path[128] = { 0 };
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path) - 1, RESOURCE_PATH"%dx%d/output.yuv", d->display.width, d->display.height);

        d->display.fd = open(path, O_RDONLY);
        if (d->display.fd < 0) {
            pthread_mutex_unlock(&d->display.mutex);
            return -1;
        }

    }

    if (d->display.frame_index >= 800) {
        d->display.frame_index = 0;
        lseek(d->display.fd, 0, SEEK_SET);
        dyc_warning_log("seek capture restart\n");

    }


    mem_size_t size = sizeof(dyc_video_common_s);
    if (dyc_memory_pool_ioctl(d->pool, DYC_MEMORY_POOL_ALLOC_CMD, &size, data)) {
        pthread_mutex_unlock(&d->display.mutex);
        dyc_error_log("capture video single pool memory failed\n");
        return -1;
    }
    dyc_video_common_s* frame = *(dyc_video_common_s**)data;
    frame->frame.w = d->display.width;
    frame->frame.h = d->display.height;
    frame->frame.line_size[0] = d->display.width;
    frame->frame.line_size[1] = d->display.width;
    frame->frame.data[0] = (char*)malloc(d->display.height * frame->frame.line_size[0]);
    frame->frame.data[1] = (char*)malloc(d->display.height * frame->frame.line_size[1] / 2);
    read(d->display.fd, frame->frame.data[0], d->display.height * frame->frame.line_size[0]);
    read(d->display.fd, frame->frame.data[1], d->display.height * frame->frame.line_size[1] / 2);
    d->display.frame_index++;
    pthread_mutex_unlock(&d->display.mutex);
    return 0;
}

static int x86_64_video_capture_driver_ioctl(void* context, int cmd, void* arg, void* data) {
    x86_64_video_capture_private_data* d = (x86_64_video_capture_private_data*)context;
    switch (cmd) {
    case VIDEO_CAPTURE_DISPLAY_FRAME_RELEASE_CMD:
    {
        pthread_mutex_lock(&d->display.mutex);
        dyc_video_common_s* frame = (dyc_video_common_s*)arg;
        free(frame->frame.data[0]);
        free(frame->frame.data[1]);
        dyc_memory_pool_ioctl(d->pool, DYC_MEMORY_POOL_FREE_CMD, frame, NULL);
        pthread_mutex_unlock(&d->display.mutex);
    }
    break;
    case VIDEO_CAPTURE_ENCODER_FRAME_GET_CMD:
    {

        pthread_mutex_lock(&d->encoder.mutex);
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned long long current_timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        unsigned long long interval_timestamp = 1000 / d->encoder.fps;
        if (abs(current_timestamp - d->encoder.last_timestamp) < interval_timestamp) {
            pthread_mutex_unlock(&d->encoder.mutex);
            return -1;
        }
        d->encoder.last_timestamp = current_timestamp;
        char path[128] = { 0 };
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path) - 1, RESOURCE_PATH"frame_%dx%d_yuv420sp/%d.yuv420sp", d->encoder.width, d->encoder.height, d->encoder.frame_index++);
        if (d->encoder.frame_index >= 800) {
            d->encoder.frame_index = 0;
        }

        mem_size_t size = sizeof(dyc_video_common_s);
        if (dyc_memory_pool_ioctl(d->pool, DYC_MEMORY_POOL_ALLOC_CMD, &size, arg)) {
            pthread_mutex_unlock(&d->display.mutex);
            return -1;
        }
        dyc_video_common_s* frame = *(dyc_video_common_s**)arg;

        int fd = open(path, O_RDONLY);
        if (fd < 0) {
            pthread_mutex_unlock(&d->encoder.mutex);
            return -1;
        }
        frame->frame.w = d->encoder.width;
        frame->frame.h = d->encoder.height;
        frame->frame.line_size[0] = d->encoder.width;
        frame->frame.line_size[1] = d->encoder.width / 2;
        frame->frame.data[0] = (char*)malloc(frame->frame.h * frame->frame.line_size[0]);
        frame->frame.data[1] = (char*)malloc(frame->frame.h * frame->frame.line_size[1]);
        read(fd, frame->frame.data[0], frame->frame.h * frame->frame.line_size[0]);
        read(fd, frame->frame.data[1], frame->frame.h * frame->frame.line_size[1] / 2);
        close(fd);
        pthread_mutex_unlock(&d->encoder.mutex);
    }
    break;
    case VIDEO_CAPTURE_ENCODER_FRAME_RELEASE_CMD:
    {
        pthread_mutex_lock(&d->encoder.mutex);
        dyc_video_common_s* frame = (dyc_video_common_s*)arg;
        free(frame->frame.data[0]);
        free(frame->frame.data[1]);
        dyc_memory_pool_ioctl(d->pool, DYC_MEMORY_POOL_FREE_CMD, frame, NULL);
        pthread_mutex_unlock(&d->encoder.mutex);
    }
    break;
    default:
        break;
    }
    return 0;
}
dyc_driver_interface dyc_video_capture_driver = {
    .open = x86_64_video_capture_driver_open,
    .close = x86_64_video_capture_driver_close,
    .write = x86_64_video_capture_driver_write,
    .read = x86_64_video_capture_driver_read,
    .ioctl = x86_64_video_capture_driver_ioctl
};

