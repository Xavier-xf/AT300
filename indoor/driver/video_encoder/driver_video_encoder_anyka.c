#include "driver_interface.h"
#include "video_encoder_h264_driver.h"
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

#include "libavcodec/avcodec.h"
#include "libavcodec/packet.h"
#include "libavutil/imgutils.h"  
#include "libavutil/opt.h"
#include "libswscale/swscale.h"

#include "dyc_memory_pool/dyc_memory_pool.h"

typedef struct {
    AVCodecContext* codec_ctx;
    struct SwsContext* sws_ctx;
    AVPacket* packet;
    AVFrame* frame;
    int packet_unref;
    unsigned long frame_index;
    unsigned long packet_index;

    void* pool;
}x86_64_encoder_h264_private_data;

static void* x86_64_encoder_h264_driver_open(void* arg) {
    x86_64_encoder_h264_private_data* d = NULL;
    video_encoder_h264_config_s* config = (video_encoder_h264_config_s*)arg;

    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        dyc_error_log("codec not found AV_CODEC_ID_H264\n");
        return NULL;
    }

    AVCodecContext* c = avcodec_alloc_context3(codec);
    if (!c) {
        dyc_error_log("avcodec alloc context failed \n");
        return NULL;
    }
    av_opt_set_defaults(c);
    c->bit_rate = config->bit_rate;
    c->rc_max_rate = config->bit_rate;
    c->rc_buffer_size = config->bit_rate / 2;

    c->width = config->width;
    c->height = config->height;
    c->time_base = (AVRational){ 1,config->fps };
    c->framerate = (AVRational){ config->fps,1 };
    c->gop_size = config->fps * 2;
    c->max_b_frames = 0;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    c->me_range = 16;
    c->qmin = 10;
    c->qmax = 51;
    c->max_qdiff = 4;

    if (avcodec_open2(c, codec, NULL) < 0) {
        dyc_error_log("avcodec open2 failed\n");
        avcodec_free_context(&c);
        return NULL;
    }

    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        dyc_error_log("avcodec not allcoate video frame \n");
        avcodec_free_context(&c);
        return NULL;
    }
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;
    if (av_frame_get_buffer(frame, 0) < 0) {
        dyc_error_log("Could not allocate the image for the frame\n");
        avcodec_free_context(&c);
        av_frame_free(&frame);
        return NULL;
    }
    AVPacket* pkt = av_packet_alloc();

    struct SwsContext* sws_ctx = sws_getContext(c->width, c->height, AV_PIX_FMT_NV12, c->width, c->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        dyc_error_log("sws_getContext failed\n");
        avcodec_free_context(&c);
        return NULL;
    }

    d = (x86_64_encoder_h264_private_data*)malloc(sizeof(x86_64_encoder_h264_private_data));
    if (!d) {
        dyc_error_log("malloc private data failed\n");
        return NULL;
    }
    dyc_memory_pool_config_s pool_cfg;
    memset(&pool_cfg, 0, sizeof(dyc_memory_pool_config_s));


    size_t node_szie = (sizeof(dyc_video_common_s) + 64) / 64 * 64 * 10;
    pool_cfg.max_size = pool_cfg.size = node_szie;
    d->pool = dyc_memory_pool_open(&pool_cfg);

    d->packet_unref = 0;
    d->codec_ctx = c;
    d->sws_ctx = sws_ctx;
    d->frame = frame;
    d->packet = pkt;
    d->frame_index = 0;
    d->packet_index = 0;
    return d;
}

static int x86_64_encoder_h264_driver_write(void* context, void* data, int length) {
    int reslut = 0;
    x86_64_encoder_h264_private_data* d = (x86_64_encoder_h264_private_data*)context;
    dyc_video_common_s* frame = (dyc_video_common_s*)data;

    av_frame_make_writable(d->frame);//&d->codec_ctx->width
    sws_scale(d->sws_ctx, (const uint8_t* const*)frame->frame.data, frame->frame.line_size, 0, d->codec_ctx->height, d->frame->data, d->frame->linesize);
    d->frame->pts = d->frame_index++;
    return avcodec_send_frame(d->codec_ctx, d->frame);
}

static int x86_64_encoder_h264_driver_read(void* context, void* data, int length) {
    int reslut = 0;
    x86_64_encoder_h264_private_data* d = (x86_64_encoder_h264_private_data*)context;

    if (d->packet_unref) {
        dyc_error_log("Please release the data obtained previously\n");
        return -1;
    }

    reslut = avcodec_receive_packet(d->codec_ctx, d->packet);
    if (reslut != 0) {
        //dyc_warning_log("get encoder packet failed\n");
        return -1;
    }


    mem_size_t size = sizeof(dyc_video_common_s);
    if (dyc_memory_pool_ioctl(d->pool, DYC_MEMORY_POOL_ALLOC_CMD, &size, data) != 0) {
        dyc_error_log("h264 encoder memory alloc failed\n");
        return -1;
    }
    dyc_video_common_s* frame = *(dyc_video_common_s**)data;
    frame->packet.data = (char*)d->packet->data;
    frame->packet.size = d->packet->size;
    frame->packet.pts = d->packet_index++;
    d->packet_unref++;

    //  dyc_right_log("get success\n");
    return reslut;
}

static int x86_64_encoder_h264_driver_ioctl(void* context, int cmd, void* arg, void* data) {
    x86_64_encoder_h264_private_data* d = (x86_64_encoder_h264_private_data*)context;

    switch (cmd) {
    case VIDEO_ENCODER_PACKET_RELEASE_CMD:
    {
        if (!d->packet_unref) {
            return -1;
        }

        dyc_video_common_s* frame = (dyc_video_common_s*)arg;
        dyc_memory_pool_ioctl(d->pool, DYC_MEMORY_POOL_FREE_CMD, frame, NULL);
        av_packet_unref(d->packet);
        d->packet_unref--;
    }
    default:

        break;
    }
    return 0;
}
static int x86_64_encoder_h264_driver_close(void* context) {
    x86_64_encoder_h264_private_data* d = (x86_64_encoder_h264_private_data*)context;

    sws_freeContext(d->sws_ctx);
    av_frame_free(&d->frame);
    av_packet_free(&d->packet);
    dyc_memory_pool_close(d->pool);
    avcodec_free_context(&d->codec_ctx);
    free(d);
    dyc_right_log("h264 encoder close\n");
    return 0;
}

dyc_driver_interface dyc_encoder_h264_driver = {
    .open = x86_64_encoder_h264_driver_open,
    .write = x86_64_encoder_h264_driver_write,
    .read = x86_64_encoder_h264_driver_read,
    .close = x86_64_encoder_h264_driver_close,
    .ioctl = x86_64_encoder_h264_driver_ioctl
};