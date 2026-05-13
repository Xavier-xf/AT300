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

#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>

#include "driver_audio_decoder.h"
#include "db_common.h"
#include "db_time.h"

typedef struct
{
    AVCodecContext *codec_ctx;
    // AVCodecParserContext *parser_ctx;
    AVPacket *pkt;
    AVFrame *frame;
    pthread_mutex_t mutex;
} decoder_device_private_data;

static void *_driver_audio_decoder_open(void *arg)
{
    const AVCodec *codec = NULL;
    db_audio_decoder_config *cfg = (db_audio_decoder_config *)arg;
    if (!cfg)
    {
        db_log_error("config\n");
        return NULL;
    }
    decoder_device_private_data *ctx = (decoder_device_private_data *)malloc(sizeof(decoder_device_private_data));
    if (!ctx)
    {
        db_log_error("malloc\n");
        return NULL;
    }

    memset(ctx, 0, sizeof(decoder_device_private_data));

    enum AVCodecID codec_id = AV_CODEC_ID_AAC;
    switch (cfg->type)
    {
    case AUDIO_DECODER_TYPE_AAC:
        codec_id = AV_CODEC_ID_AAC;
        break;
    case AUDIO_DECODER_TYPE_MP3:
        codec_id = AV_CODEC_ID_MP3;
        break;
    case AUDIO_DECODER_TYPE_PCMU:
        codec_id = AV_CODEC_ID_PCM_MULAW;
        break;
    default:
        break;
    }
    codec = avcodec_find_decoder(codec_id);
    if (!codec)
    {
        db_log_error("avcodec_find_decoder\n");
        goto fail;
    }

    // ctx->parser_ctx = av_parser_init(codec->id);
    // if (!ctx->parser_ctx)
    // {
    //     db_log_error("av_parser_init\n");
    //     goto fail;
    // }

    ctx->codec_ctx = avcodec_alloc_context3(codec);
    if (!ctx->codec_ctx)
    {
        db_log_error("avcodec_alloc_context3\n");
        goto fail;
    }
    ctx->codec_ctx->codec_type = AVMEDIA_TYPE_AUDIO;
    ctx->codec_ctx->codec_id = codec_id;
    ctx->codec_ctx->sample_rate = cfg->rate;
    ctx->codec_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
    if (cfg->channel == 2)
    {
        av_channel_layout_copy(&ctx->codec_ctx->ch_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_STEREO);
    }
    else
    {
        av_channel_layout_copy(&ctx->codec_ctx->ch_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_MONO);
    }

    if (avcodec_open2(ctx->codec_ctx, codec, NULL) < 0)
    {
        db_log_error("avcodec_open2\n");
        goto fail;
    }

    ctx->pkt = av_packet_alloc();
    if (!ctx->pkt)
    {
        db_log_error("av_packet_alloc\n");
        goto fail;
    }

    ctx->frame = av_frame_alloc();
    if (!ctx->frame)
    {
        db_log_error("av_frame_alloc\n");
        goto fail;
    }

    pthread_mutex_init(&ctx->mutex, NULL);

    return ctx;

fail:
    avcodec_free_context(&ctx->codec_ctx);
    // av_parser_close(ctx->parser_ctx);
    av_frame_free(&ctx->frame);
    av_packet_free(&ctx->pkt);
    free(ctx);
    return NULL;
}

static int _driver_audio_decoder_write(void *context, void *data, int length)
{
    decoder_device_private_data *ctx = (decoder_device_private_data *)context;

    if (context == NULL || data == NULL || length == 0)
    {
        db_log_error("NULL\n");
        return -1;
    }
    pthread_mutex_lock(&ctx->mutex);
    ctx->pkt->data = data;
    ctx->pkt->size = length;

    if (avcodec_send_packet(ctx->codec_ctx, ctx->pkt) < 0)
    {
        db_log_error("avcodec_send_packet\n");
        pthread_mutex_unlock(&ctx->mutex);
        return -1;
    }
    pthread_mutex_unlock(&ctx->mutex);
    return 0;
}

static int _driver_audio_decoder_read(void *context, void *data, int length)
{
    decoder_device_private_data *ctx = (decoder_device_private_data *)context;
    db_hal_audio_frame_t *frame = (db_hal_audio_frame_t *)data;

    int ret = -1, data_size = 0;

    if (context == NULL || data == NULL)
    {
        db_log_error("NULL\n");
        return -1;
    }

    if (length != sizeof(db_hal_audio_frame_t))
    {
        db_log_error("length != sizeof(db_hal_audio_frame_t)\n");
        return -1;
    }
    pthread_mutex_lock(&ctx->mutex);
    ret = avcodec_receive_frame(ctx->codec_ctx, ctx->frame);
    if (ret)
    {
        // db_log_error("avcodec_receive_frame\n");
        pthread_mutex_unlock(&ctx->mutex);
        return -1;
    }
    data_size = av_get_bytes_per_sample(ctx->codec_ctx->sample_fmt);
    if (data_size < 0)
    {
        db_log_error("av_get_bytes_per_sample\n");
        pthread_mutex_unlock(&ctx->mutex);
        return -1;
    }

    frame->data[0] = ctx->frame->data[0];
    frame->data[1] = ctx->frame->data[1];
    frame->channel = ctx->codec_ctx->ch_layout.nb_channels;
    frame->rate = ctx->codec_ctx->sample_rate;
    frame->size = ctx->frame->nb_samples * data_size;
    pthread_mutex_unlock(&ctx->mutex);
    // db_log_debug("ch:%d rate:%d format:%s\n", frame->channel, frame->rate, av_get_sample_fmt_name(ctx->codec_ctx->sample_fmt));

    return ret;
}

static int _driver_audio_decoder_close(void *context)
{
    decoder_device_private_data *ctx = (decoder_device_private_data *)context;

    if (context == NULL)
    {
        db_log_error("NULL\n");
        return -1;
    }

    avcodec_free_context(&ctx->codec_ctx);
    // av_parser_close(ctx->parser_ctx);
    av_frame_free(&ctx->frame);
    av_packet_free(&ctx->pkt);
    pthread_mutex_destroy(&ctx->mutex);
    free(ctx);

    return 0;
}

static int _driver_audio_decoder_ioctl(void *context, int cmd, void *arg, void *data)
{
    decoder_device_private_data *ctx = (decoder_device_private_data *)context;

    if (context == NULL)
    {
        db_log_error("NULL\n");
        return -1;
    }

    (void)ctx;

    switch (cmd)
    {
    case AUDIO_DECODER_FRAME_RELEASE_CMD:
    {
    }
    break;
    default:

        break;
    }
    return 0;
}

db_hal_driver_t db_hal_audio_decoder_driver = {
    .open = _driver_audio_decoder_open,
    .write = _driver_audio_decoder_write,
    .read = _driver_audio_decoder_read,
    .close = _driver_audio_decoder_close,
    .ioctl = _driver_audio_decoder_ioctl,
};