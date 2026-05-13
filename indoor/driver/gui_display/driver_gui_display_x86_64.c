#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <SDL2/SDL.h>

#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#include "db_common.h"
#include "driver_gui_display.h"

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture_gui;
    SDL_Texture *texture_video;
} sdl_private_data;

typedef struct
{
    sdl_private_data sdl;

    struct SwsContext *sws_ctx;

    sem_t refresh_sem;

    pthread_t thread_id;
    bool is_running;

    bool is_video_mode;

    unsigned char *gui_buffer;
    unsigned char *video_buffer;

} display_device_private_data;

static display_device_private_data *display_device_ctx = NULL;

static int sdl_windows_init(display_device_private_data *ctx)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        db_log_error("SDL_Init\n");
        return -1;
    }

    ctx->sdl.window = SDL_CreateWindow("DB Windows", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       MY_DISP_HOR_RES, MY_DISP_VER_RES, SDL_WINDOW_SHOWN);
    if (!ctx->sdl.window)
    {
        db_log_error("SDL_CreateWindow\n");
        return -1;
    }

    ctx->sdl.renderer = SDL_CreateRenderer(ctx->sdl.window, -1, SDL_RENDERER_ACCELERATED);
    if (!ctx->sdl.renderer)
    {
        db_log_error("SDL_CreateRenderer\n");
        return -1;
    }

    ctx->sdl.texture_video = SDL_CreateTexture(ctx->sdl.renderer, SDL_PIXELFORMAT_IYUV,
                                               SDL_TEXTUREACCESS_STREAMING, MY_DISP_HOR_RES, MY_DISP_VER_RES);
    if (!ctx->sdl.texture_video)
    {
        db_log_error("SDL_CreateTexture\n");
        return -1;
    }

    ctx->sdl.texture_gui = SDL_CreateTexture(ctx->sdl.renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STATIC, MY_DISP_HOR_RES, MY_DISP_VER_RES);
    if (!ctx->sdl.texture_gui)
    {
        db_log_error("SDL_CreateTexture\n");
        return -1;
    }
    SDL_SetTextureBlendMode(ctx->sdl.texture_gui, SDL_BLENDMODE_BLEND);
    return 0;
}

static int sdl_windows_deinit(display_device_private_data *ctx)
{
    SDL_DestroyTexture(ctx->sdl.texture_gui);
    SDL_DestroyTexture(ctx->sdl.texture_video);

    SDL_DestroyRenderer(ctx->sdl.renderer);
    SDL_DestroyWindow(ctx->sdl.window);

    SDL_Quit();
    return 0;
}

static int scale_yuv_region(unsigned char *src_buf, int src_width, int src_height,
                            int src_region[4],
                            unsigned char *dst_buf, int dst_width, int dst_height,
                            int dst_region[4])
{
    struct SwsContext *sws_ctx = NULL;
    uint8_t *src_data[4] = {0};
    uint8_t *dst_data[4] = {0};
    int src_linesize[4] = {0};
    int dst_linesize[4] = {0};
    int ret = -1;

    // 计算YUV420P平面指针和步长
    src_data[0] = src_buf;                                                               // Y平面
    src_data[1] = src_buf + src_width * src_height;                                      // U平面
    src_data[2] = src_buf + src_width * src_height + (src_width / 2) * (src_height / 2); // V平面

    src_linesize[0] = src_width;     // Y步长
    src_linesize[1] = src_width / 2; // U步长
    src_linesize[2] = src_width / 2; // V步长

    dst_data[0] = dst_buf;                                                               // Y平面
    dst_data[1] = dst_buf + dst_width * dst_height;                                      // U平面
    dst_data[2] = dst_buf + dst_width * dst_height + (dst_width / 2) * (dst_height / 2); // V平面

    dst_linesize[0] = dst_width;     // Y步长
    dst_linesize[1] = dst_width / 2; // U步长
    dst_linesize[2] = dst_width / 2; // V步长

    // 创建缩放上下文
    sws_ctx = sws_getContext(
        src_region[2], src_region[3], // 源区域宽高
        AV_PIX_FMT_YUV420P,           // 源像素格式
        dst_region[2], dst_region[3], // 目标区域宽高
        AV_PIX_FMT_YUV420P,           // 目标像素格式
        SWS_BILINEAR,                 // 缩放算法
        NULL, NULL, NULL);

    if (!sws_ctx)
    {
        fprintf(stderr, "Failed to create scaling context\n");
        goto end;
    }

    // 调整源数据指针到指定区域
    uint8_t *src_planes[3] = {0};
    int src_stride[3] = {0};

    for (int i = 0; i < 3; i++)
    {
        int shift = (i > 0) ? 1 : 0; // 色度分量需要除以2

        src_planes[i] = src_data[i] +
                        src_region[1] / (shift + 1) * src_linesize[i] +
                        src_region[0] / (shift + 1);
        src_stride[i] = src_linesize[i];
    }

    // 调整目标数据指针到指定区域
    uint8_t *dst_planes[3] = {0};
    int dst_stride[3] = {0};

    for (int i = 0; i < 3; i++)
    {
        int shift = (i > 0) ? 1 : 0; // 色度分量需要除以2

        dst_planes[i] = dst_data[i] +
                        dst_region[1] / (shift + 1) * dst_linesize[i] +
                        dst_region[0] / (shift + 1);
        dst_stride[i] = dst_linesize[i];
    }

    // 执行缩放操作
    ret = sws_scale(sws_ctx,
                    (const uint8_t *const *)src_planes, src_stride,
                    0, src_region[3], // 从第0行开始，处理src_region[3]行
                    dst_planes, dst_stride);

    if (ret < 0)
    {
        fprintf(stderr, "Failed to scale image\n");
        goto end;
    }

    ret = 0; // 成功

end:
    if (sws_ctx)
    {
        sws_freeContext(sws_ctx);
    }
    return ret;
}

static void *sdl_renderer_task(void *arg)
{
    display_device_private_data *ctx = (display_device_private_data *)arg;
    sdl_windows_init(ctx);
    while (ctx->is_running)
    {
        sem_wait(&ctx->refresh_sem);

        if (ctx->is_video_mode) // 视频模式
        {
            // 更新视频纹理数据
            SDL_UpdateYUVTexture(
                ctx->sdl.texture_video,
                NULL,                                                          // 更新整个纹理
                ctx->video_buffer,                                             // Y 分量
                MY_DISP_HOR_RES,                                               // Y 的 pitch（一行字节数）
                ctx->video_buffer + MY_DISP_HOR_RES * MY_DISP_VER_RES,         // U 分量
                MY_DISP_HOR_RES / 2,                                           // U 的 pitch（4:2:0 采样）
                ctx->video_buffer + MY_DISP_HOR_RES * MY_DISP_VER_RES * 5 / 4, // V 分量
                MY_DISP_HOR_RES / 2                                            // V 的 pitch
            );
        }
        // 更新UI纹理数据
        SDL_UpdateTexture(ctx->sdl.texture_gui, NULL, ctx->gui_buffer, MY_DISP_HOR_RES * sizeof(Uint32));

        // 清除渲染器并复制纹理到渲染器
        SDL_RenderClear(ctx->sdl.renderer);
        if (ctx->is_video_mode)
            SDL_RenderCopy(ctx->sdl.renderer, ctx->sdl.texture_video, NULL, NULL);
        SDL_RenderCopy(ctx->sdl.renderer, ctx->sdl.texture_gui, NULL, NULL);

        // 显示渲染内容
        SDL_RenderPresent(ctx->sdl.renderer);
    }
    sdl_windows_deinit(ctx);
    return NULL;
}

static void *_driver_gui_display_open(void *arg)
{
    // db_gui_framebuffer_config_t *cfg = (db_gui_framebuffer_config_t *)arg;
    if (display_device_ctx)
    {
        db_log_warn("gui display device is ready\n");
        return display_device_ctx;
    }

    display_device_ctx = (display_device_private_data *)malloc(sizeof(display_device_private_data));
    if (!display_device_ctx)
    {
        db_log_error("malloc\n");
        return NULL;
    }
    memset(display_device_ctx, 0, sizeof(display_device_private_data));

    sem_init(&display_device_ctx->refresh_sem, 0, 0);
    display_device_ctx->gui_buffer = (void *)malloc(MY_DISP_HOR_RES * MY_DISP_VER_RES * 4);
    display_device_ctx->video_buffer = (void *)malloc(MY_DISP_HOR_RES * MY_DISP_VER_RES * 3 / 2);
    memset(display_device_ctx->video_buffer, 0, MY_DISP_HOR_RES * MY_DISP_VER_RES);
    memset(display_device_ctx->video_buffer + MY_DISP_HOR_RES * MY_DISP_VER_RES, 128, MY_DISP_HOR_RES * MY_DISP_VER_RES / 2);
    display_device_ctx->is_running = true;
    display_device_ctx->is_video_mode = false;
    pthread_create(&display_device_ctx->thread_id, NULL, sdl_renderer_task, display_device_ctx);

    // db_log_info("open success\n");
    return display_device_ctx;
}

static int _driver_gui_display_close(void *arg)
{
    display_device_private_data *ctx = (display_device_private_data *)arg;
    if (!ctx)
    {
        db_log_error("gui display device is nullptr\n");
        return -1;
    }

    ctx->is_running = false;
    sem_post(&ctx->refresh_sem);
    pthread_join(ctx->thread_id, NULL);

    free(ctx->gui_buffer);
    free(ctx->video_buffer);
    sem_destroy(&display_device_ctx->refresh_sem);
    free(ctx);

    display_device_ctx = NULL;
    return 0;
}

static int _driver_gui_display_read(void *arg, void *data, int length)
{
    return 0;
}

static int _driver_gui_display_write(void *arg, void *data, int length)
{
    display_device_private_data *ctx = (display_device_private_data *)arg;
    db_gui_framebuffer_frame_t *frame = (db_gui_framebuffer_frame_t *)data;

    if (length != sizeof(db_gui_framebuffer_frame_t))
    {
        db_log_error("db_gui_framebuffer_frame_t\n");
        return -1;
    }

    int x = frame->pos.x;
    int y = frame->pos.y;
    int w = frame->pos.w;
    int h = frame->pos.h;

    unsigned char *src = frame->data;
    unsigned char *dst = ctx->gui_buffer + (MY_DISP_HOR_RES * y + x) * 4;

    for (int i = 0; i < h; i++)
    {
        memcpy(dst, src, w * 4);
        src += w * 4;
        dst += MY_DISP_HOR_RES * 4;
    }
    return 0;
}

static int _driver_gui_display_ioctl(void *arg, int cmd, void *param, void *data)
{
    display_device_private_data *ctx = (display_device_private_data *)arg;

    switch (cmd)
    {
    case GUI_DISPLAY_VIDEO_ENABLE_CMD:
        ctx->is_video_mode = *(bool *)param;
        break;
    case GUI_DISPLAY_FRAME_REFRESH_CMD:
        sem_post(&ctx->refresh_sem);
        break;
    default:
        break;
    }

    return 0;
}

int display_device_video_frame_write(db_hal_video_frame_t *frame, db_hal_position_t *pos)
{
    if (display_device_ctx == NULL)
    {
        return -1;
    }

    int src_region[4] = {0, 0, frame->width, frame->hight}; // x, y, width, height
    int dst_region[4] = {pos->x, pos->y, pos->w, pos->h};   // x, y, width, height

    scale_yuv_region(frame->data, frame->width, frame->hight, src_region,
                     display_device_ctx->video_buffer, MY_DISP_HOR_RES, MY_DISP_VER_RES, dst_region);

    sem_post(&display_device_ctx->refresh_sem);
    return 0;
}

int display_device_video_buffer_clean(void)
{
    if (display_device_ctx == NULL)
    {
        return -1;
    }
    memset(display_device_ctx->video_buffer, 0, MY_DISP_HOR_RES * MY_DISP_VER_RES);
    memset(display_device_ctx->video_buffer + MY_DISP_HOR_RES * MY_DISP_VER_RES, 128, MY_DISP_HOR_RES * MY_DISP_VER_RES / 2);
    return 0;
}

db_hal_driver_t db_hal_gui_display_driver =
    {
        .open = _driver_gui_display_open,
        .close = _driver_gui_display_close,
        .read = _driver_gui_display_read,
        .write = _driver_gui_display_write,
        .ioctl = _driver_gui_display_ioctl,
};