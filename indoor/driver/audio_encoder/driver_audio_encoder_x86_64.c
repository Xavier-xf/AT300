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

#include "driver_audio_encoder.h"
#include "db_queue.h"
#include "db_common.h"
#include "db_time.h"

typedef struct
{
    AVCodecContext *codec_ctx;
    AVPacket *pkt;
    AVFrame *frame;
} encoder_device_private_data;

static void *_driver_audio_encoder_open(void *arg)
{
    const AVCodec *codec = NULL;
    db_audio_encoder_config *cfg = (db_audio_encoder_config *)arg;
    if (!cfg)
    {
        db_log_error("config\n");
        return NULL;
    }
    encoder_device_private_data *ctx = (encoder_device_private_data *)malloc(sizeof(encoder_device_private_data));
    if (!ctx)
    {
        db_log_error("malloc\n");
        return NULL;
    }

    memset(ctx, 0, sizeof(encoder_device_private_data));

    codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec)
    {
        db_log_error("avcodec_find_encoder\n");
        goto fail;
    }

    ctx->codec_ctx = avcodec_alloc_context3(codec);
    if (!ctx->codec_ctx)
    {
        db_log_error("avcodec_alloc_context3\n");
        goto fail;
    }
    ctx->codec_ctx->bit_rate = 64000;
    ctx->codec_ctx->codec_type = AVMEDIA_TYPE_AUDIO;
    ctx->codec_ctx->sample_rate = cfg->rate;
    ctx->codec_ctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    av_channel_layout_copy(&ctx->codec_ctx->ch_layout, &(AVChannelLayout)AV_CHANNEL_LAYOUT_MONO);

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

    ctx->frame->nb_samples = ctx->codec_ctx->frame_size;
    ctx->frame->format = ctx->codec_ctx->sample_fmt;
    ctx->frame->sample_rate = ctx->codec_ctx->sample_rate;
    av_channel_layout_copy(&ctx->frame->ch_layout, &ctx->codec_ctx->ch_layout);
    db_log_debug("sample:%d rate:%d ch:%d\n", ctx->frame->nb_samples, ctx->frame->sample_rate, ctx->frame->ch_layout.nb_channels);
    return ctx;

fail:
    avcodec_free_context(&ctx->codec_ctx);
    av_frame_free(&ctx->frame);
    av_packet_free(&ctx->pkt);
    free(ctx);
    return NULL;
}

static int _driver_audio_encoder_write(void *context, void *data, int length)
{
    encoder_device_private_data *ctx = (encoder_device_private_data *)context;

    if (context == NULL || data == NULL || length == 0)
    {
        db_log_error("NULL\n");
        return -1;
    }

    ctx->frame->data[0] = data;

    if (avcodec_send_frame(ctx->codec_ctx, ctx->frame) < 0)
    {
        db_log_error("avcodec_send_frame\n");
        return -1;
    }

    return 0;
}

static int _driver_audio_encoder_read(void *context, void *data, int length)
{
    encoder_device_private_data *ctx = (encoder_device_private_data *)context;
    db_hal_audio_packet_t *packet = (db_hal_audio_packet_t *)data;

    int ret = -1;

    if (context == NULL || data == NULL)
    {
        db_log_error("NULL\n");
        return -1;
    }

    if (length != sizeof(db_hal_audio_packet_t))
    {
        db_log_error("length != sizeof(db_hal_audio_packet_t)\n");
        return -1;
    }

    ret = avcodec_receive_packet(ctx->codec_ctx, ctx->pkt);
    if (ret)
    {
        db_log_error("avcodec_receive_packet\n");
        return -1;
    }
    packet->data = ctx->pkt->data;
    packet->size = ctx->pkt->size;
    return ret;
}

static int _driver_audio_encoder_close(void *context)
{
    encoder_device_private_data *ctx = (encoder_device_private_data *)context;

    if (context == NULL)
    {
        db_log_error("NULL\n");
        return -1;
    }

    avcodec_free_context(&ctx->codec_ctx);
    av_frame_free(&ctx->frame);
    av_packet_free(&ctx->pkt);
    free(ctx);

    return 0;
}

static int _driver_audio_encoder_ioctl(void *context, int cmd, void *arg, void *data)
{
    encoder_device_private_data *ctx = (encoder_device_private_data *)context;

    if (context == NULL)
    {
        db_log_error("NULL\n");
        return -1;
    }

    (void)ctx;

    switch (cmd)
    {
    case AUDIO_ENCODER_PACKET_RELEASE_CMD:
    {
        av_packet_unref(ctx->pkt);
    }
    break;
    default:

        break;
    }
    return 0;
}

db_hal_driver_t db_hal_audio_encoder_driver = {
    .open = _driver_audio_encoder_open,
    .write = _driver_audio_encoder_write,
    .read = _driver_audio_encoder_read,
    .close = _driver_audio_encoder_close,
    .ioctl = _driver_audio_encoder_ioctl,
};