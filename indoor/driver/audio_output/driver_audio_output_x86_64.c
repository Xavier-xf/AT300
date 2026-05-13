#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "driver_audio_output.h"
#include "db_common.h"
#include "db_ringbuffer.h"

#define AUDIO_OUTPUT_USER_MAX 4      // 最大用户个数
#define AUDIO_OUTPUT_FRAME_MAX 5     // 最大缓存帧数
#define AUDIO_OUTPUT_FRAME_PERIOD 20 // 单帧播放周期

typedef struct
{
    snd_pcm_t *handle;
    int channel;
    unsigned int rate;

    int sample_bytes;
    int frame_bytes;

    int user_count;
    void *user[AUDIO_OUTPUT_USER_MAX];
    // pthread_mutex_t mutex;
    pthread_t thread_id;
    bool is_running;
} sound_device_private_data;

typedef struct
{
    bool enable;
    float gain; // 0~1
    void *ringbuffer;
    sound_device_private_data *dev;
} sound_device_user_data;

static sound_device_private_data *sound_device_ctx = NULL;

static int _audio_output_device_open(sound_device_private_data *ctx)
{
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16;
    snd_pcm_uframes_t buffer_size = 0;
    int reslut = -1;
    int format_bits = 0;

    reslut = snd_pcm_open(&ctx->handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (reslut)
    {
        db_log_error("snd_pcm_open\n");
        return -1;
    }

    snd_pcm_hw_params_alloca(&hwparams);
    reslut = snd_pcm_hw_params_any(ctx->handle, hwparams);
    // if (reslut)
    // {
    //     db_log_error("snd_pcm_hw_params_any\n");
    //     goto fail;
    // }

    reslut = snd_pcm_hw_params_set_access(ctx->handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (reslut)
    {
        db_log_error("snd_pcm_hw_params_set_access\n");
        goto fail;
    }

    reslut = snd_pcm_hw_params_set_format(ctx->handle, hwparams, format);
    if (reslut)
    {
        db_log_error("snd_pcm_hw_params_set_format\n");
        goto fail;
    }

    reslut = snd_pcm_hw_params_set_channels(ctx->handle, hwparams, ctx->channel);
    if (reslut)
    {
        db_log_error("snd_pcm_hw_params_set_channels\n");
        goto fail;
    }

    reslut = snd_pcm_hw_params_set_rate_near(ctx->handle, hwparams, &ctx->rate, 0);
    if (reslut)
    {
        db_log_error("snd_pcm_hw_params_set_rate_near\n");
        goto fail;
    }

    format_bits = snd_pcm_format_physical_width(format);
    ctx->sample_bytes = format_bits / 8 * ctx->channel;
    ctx->frame_bytes = ctx->sample_bytes * ctx->rate * AUDIO_OUTPUT_FRAME_PERIOD / 1000;
    buffer_size = ctx->frame_bytes * AUDIO_OUTPUT_FRAME_MAX / ctx->sample_bytes;
    reslut = snd_pcm_hw_params_set_buffer_size_near(ctx->handle, hwparams, &buffer_size);
    if (reslut)
    {
        db_log_error("snd_pcm_hw_params_set_buffer_size_near\n");
        goto fail;
    }

    reslut = snd_pcm_hw_params_set_period_size(ctx->handle, hwparams, ctx->frame_bytes / ctx->sample_bytes, 0);
    if (reslut)
    {
        db_log_error("snd_pcm_hw_params_set_period_size\n");
        goto fail;
    }

    reslut = snd_pcm_hw_params(ctx->handle, hwparams);
    if (reslut)
    {
        db_log_error("snd_pcm_hw_params\n");
        goto fail;
    }

    db_log_debug("ch:%d rate:%d buffer_size:%d frame_bytes:%d\n", ctx->channel, ctx->rate, buffer_size, ctx->frame_bytes);

    return 0;

fail:
    snd_pcm_drop(ctx->handle);
    snd_pcm_close(ctx->handle);

    return -1;
}

static int _audio_output_device_write(sound_device_private_data *ctx, unsigned char *data, int len)
{
#if 0
    ssize_t r;
    ssize_t result = 0;

    len = len / ctx->sample_bytes;
    while (len > 0)
    {
        r = snd_pcm_writei(ctx->handle, data, len);
        if (r == -EAGAIN || (r >= 0 && (size_t)r < len))
        {
            snd_pcm_wait(ctx->handle, 1000);
        }
        else if (r == -EPIPE)
        {
            snd_pcm_prepare(ctx->handle);
            r = snd_pcm_writei(ctx->handle, data, len);
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
        }
        else if (r == -ESTRPIPE)
        {
            fprintf(stderr, "<<<<<<<<<<<<<<< Need suspend >>>>>>>>>>>>>>>\n");
        }
        else if (r < 0)
        {
            fprintf(stderr, "Error snd_pcm_writei: [%s]\n", snd_strerror(r));
            break;
        }
        if (r > 0)
        {
            result += r;
            len -= r;
            data += r * ctx->sample_bytes;
        }
    }
#else
    snd_pcm_sframes_t n;
    len = len / ctx->sample_bytes;
    n = snd_pcm_writei(ctx->handle, data, len);

    if (-EPIPE == n)
    {
        snd_pcm_prepare(ctx->handle);

        n = snd_pcm_writei(ctx->handle, data, len);
        if (n < 0)
        {
            db_log_error("snd_pcm_writei n < 0");
        }
    }
    else if (n < 0)
    {
        db_log_error("snd_pcm_writei n < 0");
    }
    else if (n != len)
    {
        db_log_error("snd_pcm_writei n != len");
    }
#endif
    return 0;
}

static int _audio_output_device_close(sound_device_private_data *ctx)
{
    snd_pcm_drop(ctx->handle);
    snd_pcm_close(ctx->handle);
    return 0;
}

static void *_audio_output_device_thread(void *arg)
{
    sound_device_private_data *ctx = (sound_device_private_data *)arg;
    sound_device_user_data *user = NULL;
    const int frame_bytes = ctx->frame_bytes;
    const int sample_num = frame_bytes / ctx->sample_bytes * ctx->channel;
    int32_t *mixer_data = (int32_t *)malloc(sample_num * sizeof(int32_t));
    uint8_t *pcm_data = (uint8_t *)malloc(frame_bytes);

    while (ctx->is_running)
    {
        int8_t got_frame = 0;
        for (int i = 0; i < AUDIO_OUTPUT_USER_MAX; i++)
        {
            if (ctx->user[i] != NULL)
            {
                user = ctx->user[i];
                if (user->enable == false)
                {
                    continue;
                }

                unsigned int valid_length = 0;
                db_ringbuffer_ioctl(user->ringbuffer, DB_RINGBUFFER_CMD_VALID_GET, &valid_length);
                // if (valid_length > (frame_bytes * AUDIO_OUTPUT_FRAME_MAX))
                if (valid_length < frame_bytes)
                {
                    // db_ringbuffer_ioctl(user->ringbuffer, DB_RINGBUFFER_CMD_FLUSH, NULL);
                    // db_log_warn("user[%d] clear ringbuffer data[%d]\n", i, valid_length);
                    continue;
                }

                if (db_ringbuffer_read(user->ringbuffer, pcm_data, frame_bytes) == 0)
                {
                    if (got_frame == 0)
                    {
                        memset((void *)mixer_data, 0, sample_num * sizeof(int32_t));
                    }
                    audio_output_gain((int16_t *)pcm_data, sample_num, user->gain);
                    audio_output_accumulate(mixer_data, (int16_t *)pcm_data, sample_num);
                    got_frame = 1;
                }
            }
        }
        if (got_frame)
        {
            int16_t *pcm_data_bit16 = (int16_t *)pcm_data;
            for (int i = 0; i < sample_num; i++)
            {
                pcm_data_bit16[i] = amplitude_limit(mixer_data[i]);
            }
            _audio_output_device_write(ctx, pcm_data, frame_bytes);
        }
        else
        {
            usleep(1000 * 1);
        }
    }
    free(mixer_data);
    free(pcm_data);
    db_log_debug("finish\n");
    return NULL;
}

static int _audio_output_user_insert(sound_device_private_data *ctx, sound_device_user_data **user)
{
    for (int i = 0; i < AUDIO_OUTPUT_USER_MAX; i++)
    {
        if (ctx->user[i] == NULL)
        {
            sound_device_user_data *user_data = (sound_device_user_data *)malloc(sizeof(sound_device_user_data));
            if (user_data == NULL)
            {
                db_log_error("malloc\n");
                return -1;
            }
            user_data->ringbuffer = db_ringbuffer_open(ctx->frame_bytes * AUDIO_OUTPUT_FRAME_MAX);
            if (user_data->ringbuffer == NULL)
            {
                free(user_data);
                db_log_error("db_ringbuffer_open\n");
                return -1;
            }
            user_data->dev = ctx;
            user_data->gain = 1.0;
            ctx->user[i] = *user = user_data;
            ctx->user_count++;
            return 0;
        }
    }
    return -1;
}

static int _audio_output_user_delete(sound_device_private_data *ctx, sound_device_user_data *user)
{
    for (int i = 0; i < AUDIO_OUTPUT_USER_MAX; i++)
    {
        if (ctx->user[i] == user)
        {
            db_ringbuffer_close(((sound_device_user_data *)ctx->user[i])->ringbuffer);
            free(ctx->user[i]);
            ctx->user[i] = NULL;
            ctx->user_count--;
            return 0;
        }
    }
    return -1;
}

static void *_driver_audio_output_open(void *arg)
{
    db_audio_output_config *config = (db_audio_output_config *)arg;

    if (!sound_device_ctx)
    {
        sound_device_ctx = (sound_device_private_data *)malloc(sizeof(sound_device_private_data));
        if (!sound_device_ctx)
        {
            db_log_error("malloc\n");
            return NULL;
        }
        memset(sound_device_ctx, 0, sizeof(sound_device_private_data));
        sound_device_ctx->channel = config->channel;
        sound_device_ctx->rate = config->rate;
        if (_audio_output_device_open(sound_device_ctx))
        {
            free(sound_device_ctx);
            return NULL;
        }
        sound_device_ctx->is_running = true;
        pthread_create(&sound_device_ctx->thread_id, NULL, _audio_output_device_thread, sound_device_ctx);
    }

    sound_device_user_data *user = NULL;
    if (_audio_output_user_insert(sound_device_ctx, &user))
    {
        return NULL;
    }

    db_log_debug("user_count:%d\n", sound_device_ctx->user_count);
    return user;
}

static int _driver_audio_output_write(void *ctx, void *data, int length)
{
    sound_device_user_data *user = (sound_device_user_data *)ctx;
    if (!user)
    {
        db_log_error("audio output context is nullptr\n");
        return -1;
    }
    // sound_device_private_data *dev = user->dev;
    // pthread_mutex_lock(&dev->mutex);
    if (user->enable == 0)
    {
        db_log_error("user->enable == 0\n");
        // pthread_mutex_unlock(&dev->mutex);
        return -1;
    }
    int reslut = db_ringbuffer_write(user->ringbuffer, data, length);
    // pthread_mutex_unlock(&dev->mutex);
    return reslut;
    // _audio_output_device_write(user->dev, data, length);
    // return 0;
}

static int _driver_audio_output_read(void *ctx, void *data, int length)
{
    return 0;
}

static int _driver_audio_output_close(void *ctx)
{
    sound_device_user_data *user = (sound_device_user_data *)ctx;
    if (!user)
    {
        db_log_error("user is nullptr\n");
        return -1;
    }

    sound_device_private_data *dev = user->dev;
    if (!dev)
    {
        db_log_error("device context is nullpter\n");
        return -1;
    }
    // pthread_mutex_t *mutex = &dev->mutex;
    // pthread_mutex_lock(mutex);
    if (_audio_output_user_delete(dev, user) != 0)
    {
        db_log_error("device destory failed\n");
        // pthread_mutex_unlock(mutex);
        return -1;
    }

    if (dev->user_count == 0)
    {
        _audio_output_device_close(dev);
        dev->is_running = false;
        pthread_join(dev->thread_id, NULL);
        free(sound_device_ctx);
        sound_device_ctx = NULL;
    }
    // pthread_mutex_unlock(mutex);

    // if (sound_device_ctx == NULL)
    // {
    //     pthread_mutex_destroy(mutex);
    // }
    return 0;
}

static int _driver_audio_output_ioctl(void *ctx, int cmd, void *arg, void *data)
{
    sound_device_user_data *user = (sound_device_user_data *)ctx;
    if (!user)
    {
        db_log_error("audio output context is nullptr\n");
        return -1;
    }
    // sound_device_private_data *dev = (sound_device_private_data *)user->dev;
    switch (cmd)
    {
    case AUDIO_OUTPUT_USER_ENABLE_CMD:
    {
        // pthread_mutex_lock(&dev->mutex);
        user->enable = *(bool *)arg;
        // pthread_mutex_unlock(&dev->mutex);
    }
    break;
    case AUDIO_OUTPUT_USER_FREE_CMD:
    {
        // pthread_mutex_lock(&dev->mutex);
        db_ringbuffer_ioctl(user->ringbuffer, DB_RINGBUFFER_CMD_FREE_GET, data);
        // pthread_mutex_unlock(&dev->mutex);
    }
    break;
    case AUDIO_OUTPUT_USER_TOTAL_CMD:
        // {
        //     unsigned int total = 0;
        //     pthread_mutex_lock(&dev->mutex);
        //     ringbuffer_ioctl(user->ringbuffer, RINGBUFFER_MAX_GET_CMD, NULL, &total);
        //     *(unsigned int *)data = total;
        //     pthread_mutex_unlock(&dev->mutex);
        // }
        break;
    case AUDIO_OUTPUT_USER_FRAME_NUM_CMD:
        // {
        //     pthread_mutex_lock(&dev->mutex);
        //     *(int *)data = dev->frmnum;
        //     pthread_mutex_unlock(&dev->mutex);
        // }
        break;
    case AUDIO_OUTPUT_DEVICE_CONFIG_GET_CMD:
        // {
        //     pthread_mutex_lock(&dev->mutex);
        //     *(audio_output_config *)data = dev->config;
        //     pthread_mutex_unlock(&dev->mutex);
        // }
        break;
    case AUDIO_OUTPUT_USER_VALID_CMD:
        // {
        //     unsigned int valid = 0;
        //     pthread_mutex_lock(&dev->mutex);
        //     ringbuffer_ioctl(user->ringbuffer, RINGBUFFER_VAILD_GET_CMD, NULL, &valid);
        //     *(unsigned int *)data = valid;
        //     pthread_mutex_unlock(&dev->mutex);
        // }
        break;
    case AUDIO_OUTPUT_CONFIG_GET_CMD:
    // {
    //     pthread_mutex_lock(&dev->mutex);
    //     audio_output_config *cfg = (audio_output_config *)data;
    //     *cfg = user->dev->config;
    //     pthread_mutex_unlock(&dev->mutex);
    // }
    default:
        break;
    }

    return 0;
}

db_hal_driver_t db_hal_audio_output_driver = {
    .open = _driver_audio_output_open,
    .close = _driver_audio_output_close,
    .read = _driver_audio_output_read,
    .write = _driver_audio_output_write,
    .ioctl = _driver_audio_output_ioctl,
};
