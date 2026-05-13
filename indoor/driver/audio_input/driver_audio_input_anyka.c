#include "driver_interface.h"
#include "audio_input_driver.h"
#include "stdlib.h"
#include "dyc_common.h"
#include "dyc_ringbuffer/dyc_ringbuffer.h"
#include <alsa/asoundlib.h>    

typedef struct {
    int ref_count;
    snd_pcm_t* handle;
    pthread_t tid;
    int8_t runing;
    pthread_mutex_t mutex;
    dyc_audio_input_config config;
    void* ringbuffer;
    int32_t frmnum;
    int8_t enable;
}x86_64_audio_input_private_data;

static x86_64_audio_input_private_data* x86_64_audio_input_dev = NULL;

static int x86_64_dyc_audio_input_hardware_init(x86_64_audio_input_private_data* d, dyc_audio_input_config* cfg) {

    int reslut = 0;
    snd_pcm_hw_params_t* params = NULL;

    if ((reslut = snd_pcm_open(&d->handle, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        dyc_error_log("open snd pcm open failed\n");
        return -1;
    }

    snd_pcm_hw_params_alloca(&params);

    if ((reslut = snd_pcm_hw_params_any(d->handle, params)) < 0) {
        dyc_error_log("hw prams any failed\n");
        goto FAIL;
    }

    if ((reslut = snd_pcm_hw_params_set_access(d->handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        dyc_error_log("prams set access failed\n");
        goto FAIL;
    }

    if ((reslut = snd_pcm_hw_params_set_format(d->handle, params, SND_PCM_FORMAT_S16_LE)) < 0) {
        dyc_error_log("set foramt failed\n");
        goto FAIL;
    }

    if ((reslut = snd_pcm_hw_params_set_channels(d->handle, params, cfg->channel)) < 0) {
        dyc_error_log("set channels failed\n");
        goto FAIL;
    }


    if ((reslut = snd_pcm_hw_params_set_rate_near(d->handle, params, &cfg->sample, 0)) < 0) {
        dyc_error_log("set rate failed\n");
        goto FAIL;
    }

    if ((reslut = snd_pcm_hw_params(d->handle, params)) < 0) {
        dyc_error_log("set hw params failed\n");
        goto FAIL;
    }

    // snd_pcm_hw_params_free(params);

    return 0;
FAIL:
    if (d->handle) {
        snd_pcm_close(d->handle);
    }
    return -1;
}

static int x86_64_dyc_audio_input_hardware_deinit(x86_64_audio_input_private_data* d) {
    snd_pcm_close(d->handle);
    return 0;
}

static void* x86_64_dyc_audio_input_thread(void* arg) {
    x86_64_audio_input_private_data* d = (x86_64_audio_input_private_data*)arg;
    int reslut = 0;

    char* buffer = (char*)malloc(d->frmnum);
    while (d->runing) {
        if (d->enable == 0) {
            dyc_usleep(10 * 1000);
            continue;
        }
        int nframe = d->config.channel == 2 ? (d->frmnum / 4) : (d->frmnum / 2);
        if ((reslut = snd_pcm_readi(d->handle, buffer, nframe)) != nframe) {
            dyc_warning_log("read from audio interface failed\n");
            dyc_usleep(10 * 1000);
            continue;
        }

        unsigned int free_length = 0;
        dyc_ringbuffer_ioctl(d->ringbuffer, DYC_RINGBUFFER_FREE_GET_CMD, NULL, &free_length);
        if (free_length < d->frmnum) {
            dyc_usleep(10 * 1000);
            continue;
        }

        if (dyc_ringbuffer_write(d->ringbuffer, buffer, d->frmnum) != 0) {
            dyc_warning_log("write ringbuffer audio output failed\n");
        }
        dyc_usleep(10 * 1000);
    }

    free(buffer);
    return NULL;
}


static x86_64_audio_input_private_data* x86_64_dyc_audio_input_alloc(dyc_audio_input_config* cfg) {

    x86_64_audio_input_private_data* d = (x86_64_audio_input_private_data*)malloc(sizeof(x86_64_audio_input_private_data));
    if (!d) {
        dyc_error_log("malloc x86_64_audio_input_private_data failed\n");
        return NULL;
    }
    memset(d, 0, sizeof(x86_64_audio_input_private_data));

    if (x86_64_dyc_audio_input_hardware_init(d, cfg) != 0) {
        dyc_error_log("audio input hardwrate init failed\n");
        free(d);
        return NULL;
    }

    d->config = *cfg;
    d->frmnum = cfg->sample * 20 * 2 / 1000;
    if (cfg->channel == 2) {
        d->frmnum *= 2;
    }
    d->ref_count = 1;
    d->runing = 1;
    d->enable = 0;
    int ringbuffer_size = d->frmnum * 20;
    d->ringbuffer = dyc_ringbuffer_open(&ringbuffer_size);
    pthread_mutex_init(&d->mutex, NULL);
    snd_pcm_prepare(d->handle);
    pthread_create(&d->tid, pthread_stack_attr(), x86_64_dyc_audio_input_thread, d);
    return d;
}

static int x86_64_dyc_audio_input_destory(x86_64_audio_input_private_data* d) {

    d->runing = 0;
    pthread_join(d->tid, NULL);
    x86_64_dyc_audio_input_hardware_deinit(d);
    dyc_ringbuffer_close(d->ringbuffer);
    free(d);
    return 0;
}

static void* x86_64_dyc_audio_input_driver_open(void* arg) {
    dyc_audio_input_config* config = (dyc_audio_input_config*)arg;

    if (x86_64_audio_input_dev) {
        dyc_warning_log("the audio input device has been created\n");
        x86_64_audio_input_dev->ref_count++;
        return x86_64_audio_input_dev;
    }

    x86_64_audio_input_dev = x86_64_dyc_audio_input_alloc(config);
    if (!x86_64_audio_input_dev) {
        dyc_warning_log("audio input device create failed\n");
        return NULL;
    }


    return x86_64_audio_input_dev;
}

static int x86_64_dyc_audio_input_driver_close(void* ctx) {
    x86_64_audio_input_private_data* d = (x86_64_audio_input_private_data*)ctx;
    if (d != x86_64_audio_input_dev) {
        dyc_error_log("audio input addres diffrent \n");
        return -1;
    }
    x86_64_audio_input_dev->ref_count--;
    if (x86_64_audio_input_dev->ref_count) {
        return 0;
    }
    pthread_mutex_t* mutex = &d->mutex;
    pthread_mutex_lock(mutex);
    x86_64_dyc_audio_input_destory(d);
    x86_64_audio_input_dev = NULL;
    pthread_mutex_unlock(mutex);
    pthread_mutex_destroy(mutex);
    dyc_right_log("audio input device close\n");
    return 0;
}

static int x86_64_dyc_audio_input_driver_read(void* ctx, void* data, int size) {
    x86_64_audio_input_private_data* d = (x86_64_audio_input_private_data*)ctx;
    if (!d) {
        dyc_error_log("audio handle is nullptr\n");
        return -1;
    }

    pthread_mutex_lock(&d->mutex);
    if (d->enable == 0) {
        pthread_mutex_unlock(&d->mutex);
        return -1;
    }
    int reslut = dyc_ringbuffer_read(d->ringbuffer, data, size);
    pthread_mutex_unlock(&d->mutex);
    return reslut;
}

static int x86_64_dyc_audio_input_driver_write(void* ctx, void* data, int size) {

    return 0;
}

static int x86_64_dyc_audio_input_driver_ioctl(void* ctx, int cmd, void* arg, void* data) {
    x86_64_audio_input_private_data* dev = (x86_64_audio_input_private_data*)ctx;
    if (!dev) {
        dyc_error_log("audio input context is nullptr\n");
        return -1;
    }
    switch (cmd) {
    case DYC_AUDIO_INPUT_USER_ENABLE_CMD:
    {
        pthread_mutex_lock(&dev->mutex);
        dev->enable = *(int8_t*)arg;
        pthread_mutex_unlock(&dev->mutex);
    }
    break;
    case DYC_AUDIO_INPUT_USER_VAILD_CMD:
    {
        unsigned int vaild = 0;
        pthread_mutex_lock(&dev->mutex);
        dyc_ringbuffer_ioctl(dev->ringbuffer, DYC_RINGBUFFER_VAILD_GET_CMD, NULL, &vaild);
        *(unsigned int*)data = vaild;
        pthread_mutex_unlock(&dev->mutex);
    }
    break;
    case DYC_AUDIO_INPUT_USER_TOTAL_CMD:
    {
        unsigned int total = 0;
        pthread_mutex_lock(&dev->mutex);
        dyc_ringbuffer_ioctl(dev->ringbuffer, DYC_RINGBUFFER_MAX_GET_CMD, NULL, &total);
        pthread_mutex_unlock(&dev->mutex);
    }
    break;
    case DYC_AUDIO_INPUT_USER_FRAME_NUM_CMD:
    {
        pthread_mutex_lock(&dev->mutex);
        *(int*)data = dev->frmnum;
        pthread_mutex_unlock(&dev->mutex);
    }
    break;
    case DYC_AUDIO_INPUT_DEVICE_CONFIG_GET_CMD:
    {
        pthread_mutex_lock(&dev->mutex);
        *(dyc_audio_input_config*)data = dev->config;
        pthread_mutex_unlock(&dev->mutex);
    }
    break;
    default:
        break;
    }

    return 0;
}

dyc_driver_interface dyc_audio_input_driver = {
    .open = x86_64_dyc_audio_input_driver_open,
    .close = x86_64_dyc_audio_input_driver_close,
    .read = x86_64_dyc_audio_input_driver_read,
    .write = x86_64_dyc_audio_input_driver_write,
    .ioctl = x86_64_dyc_audio_input_driver_ioctl,
};