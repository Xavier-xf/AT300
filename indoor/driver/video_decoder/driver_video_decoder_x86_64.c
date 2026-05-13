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

#include "driver_video_decoder.h"
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

static void *_driver_video_decoder_open(void *arg)
{
    const AVCodec *codec = NULL;
    db_video_decoder_config *cfg = (db_video_decoder_config *)arg;
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

    enum AVCodecID codec_id = AV_CODEC_ID_H264;
    switch (cfg->type)
    {
    case VIDEO_DECODER_TYPE_H264:
        codec_id = AV_CODEC_ID_H264;
        break;
    case VIDEO_DECODER_TYPE_H265:
        codec_id = AV_CODEC_ID_H265;
        break;
    case VIDEO_DECODER_TYPE_MJPEG:
        codec_id = AV_CODEC_ID_MJPEG;
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

static int _driver_video_decoder_write(void *context, void *data, int length)
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
        // db_log_error("avcodec_send_packet\n");
        pthread_mutex_unlock(&ctx->mutex);
        return -1;
    }
    pthread_mutex_unlock(&ctx->mutex);
    return 0;
}

static void _yuvj420p_pix_copy(AVFrame *frame, uint8_t *dest)
{
    int width = frame->width;
    int height = frame->height;

    int y_size = width * height;
    int uv_size = width * height / 4;

    if (!frame->data[0] || !frame->data[1] || !frame->data[2])
    {
        return;
    }

    uint8_t *y_plane = dest;
    uint8_t *u_plane = dest + y_size;
    uint8_t *v_plane = dest + y_size + uv_size;

    for (int i = 0; i < height; i++)
    {
        memcpy(y_plane + i * width, frame->data[0] + i * frame->linesize[0], width);
    }
    for (int i = 0; i < height / 2; i++)
    {
        memcpy(u_plane + i * (width / 2), frame->data[1] + i * frame->linesize[1], width / 2);
    }
    for (int i = 0; i < height / 2; i++)
    {
        memcpy(v_plane + i * (width / 2), frame->data[2] + i * frame->linesize[2], width / 2);
    }
}

static int _driver_video_decoder_read(void *context, void *data, int length)
{
    decoder_device_private_data *ctx = (decoder_device_private_data *)context;
    db_hal_video_frame_t *frame = (db_hal_video_frame_t *)data;

    int ret = -1, data_size = 0;

    if (context == NULL || data == NULL)
    {
        db_log_error("NULL\n");
        return -1;
    }

    if (length != sizeof(db_hal_video_frame_t))
    {
        db_log_error("length != sizeof(db_hal_video_frame_t)\n");
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

    frame->width = ctx->frame->width;
    frame->hight = ctx->frame->height;

    frame->data = (unsigned char *)malloc(frame->width * frame->hight * 3 / 2);
    // memcpy(frame->data, ctx->frame->data[0], ctx->frame->linesize[0] * frame->hight);
    // memcpy(frame->data + ctx->frame->linesize[0] * frame->hight, ctx->frame->data[1], ctx->frame->linesize[1] * frame->hight / 2);
    // memcpy(frame->data + ctx->frame->linesize[1] * frame->hight / 2, ctx->frame->data[2], ctx->frame->linesize[2] * frame->hight / 2);

    _yuvj420p_pix_copy(ctx->frame, frame->data);
    pthread_mutex_unlock(&ctx->mutex);
    // db_log_debug("width:%d hight:%d [%d %d %d %d] format:%s\n", frame->width, frame->hight, ctx->frame->linesize[0], ctx->frame->linesize[1], ctx->frame->linesize[2], ctx->frame->linesize[3], av_get_pix_fmt_name(ctx->codec_ctx->pix_fmt));

    return ret;
}

static int _driver_video_decoder_close(void *context)
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

static int _driver_video_decoder_ioctl(void *context, int cmd, void *arg, void *data)
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
    case VIDEO_DECODER_FRAME_RELEASE_CMD:
    {
        db_hal_video_frame_t *frame = (db_hal_video_frame_t *)arg;
        if (frame && frame->data)
        {
            free(frame->data);
        }
    }
    break;

    default:

        break;
    }
    return 0;
}

db_hal_driver_t db_hal_video_decoder_driver = {
    .open = _driver_video_decoder_open,
    .write = _driver_video_decoder_write,
    .read = _driver_video_decoder_read,
    .close = _driver_video_decoder_close,
    .ioctl = _driver_video_decoder_ioctl,
};