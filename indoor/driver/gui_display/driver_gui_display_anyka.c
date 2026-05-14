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

#include "anyka/ak_mem.h"
#include "anyka/ak_common_graphics.h"
#include "anyka/ak_common.h"
#include "anyka/ak_tde.h"
#include "anyka/ak_vdec.h"

#include "db_common.h"
#include "db_time.h"
#include "driver_gui_display.h"

#define FB_PATH "/dev/fb0"
#define TDE_MIN_AREA_WIDTH 18
#define TDE_MIN_AREA_HEIGHT 18

struct lv_lcd_layer
{
    struct ak_tde_layer layer;    // 图形加速器层级对象
    unsigned char *data;          // 虚拟内存
    unsigned long long timestamp; // 刷新时间戳
};

typedef struct
{
    int fd; // 设备句柄
    struct fb_var_screeninfo var_info;
    struct fb_fix_screeninfo fix_info;
    /* 三个层级对象 */
    struct lv_lcd_layer lv_fb_layer;    // framebuffer层
    struct lv_lcd_layer lv_video_layer; // 视频层
    struct lv_lcd_layer lv_gui_layer;   // gui层

    struct SwsContext *sws_ctx;

    sem_t refresh_sem;

    pthread_t thread_id;
    pthread_mutex_t lock;
    bool is_running;

    bool is_video_mode;
    bool dirty_valid;
    bool force_full_sync;
    db_hal_position_t dirty_area;

} display_device_private_data;

static display_device_private_data *display_device_ctx = NULL;

/* framebuffer层初始化 */
static void _lv_fb_layer_init(display_device_private_data *ctx)
{
    ctx->lv_fb_layer.timestamp = 0;
    ctx->lv_fb_layer.data = osal_fb_mmap_viraddr(ctx->fix_info.smem_len, ctx->fd);
    memset((void *)ctx->lv_fb_layer.data, 0, ctx->fix_info.smem_len);
    tde_layer_layer_init(ctx->lv_fb_layer.layer, GP_FORMAT_RGB888, ctx->var_info.xres, ctx->var_info.yres, 0, 0, ctx->var_info.xres, ctx->var_info.yres);
    ctx->lv_fb_layer.layer.phyaddr = ctx->fix_info.smem_start;
}

/* gui层初始化 */
static void _lv_gui_layer_init(display_device_private_data *ctx)
{
    ctx->lv_gui_layer.timestamp = 0;
    ctx->lv_gui_layer.data = (unsigned char *)ak_mem_dma_alloc(MODULE_ID_VO, MY_DISP_HOR_RES * MY_DISP_VER_RES * 4);
    memset((void *)ctx->lv_gui_layer.data, 0, MY_DISP_HOR_RES * MY_DISP_VER_RES * 4);
    ak_mem_dma_vaddr2paddr((void *)ctx->lv_gui_layer.data, (unsigned long *)&ctx->lv_gui_layer.layer.phyaddr);
    tde_layer_layer_init(ctx->lv_gui_layer.layer, GP_FORMAT_ARGB8888, MY_DISP_HOR_RES, MY_DISP_VER_RES, 0, 0, MY_DISP_HOR_RES, MY_DISP_VER_RES);
}

// /* 视频层初始化 */
// static void _lv_video_layer_init(display_device_private_data *ctx)
// {
//     ctx->lv_video_layer.timestamp = 0;db_log_debug("====>>");
//     ctx->lv_video_layer.data = (unsigned char *)ak_mem_dma_alloc(MODULE_ID_VO, MY_DISP_HOR_RES * MY_DISP_VER_RES * 3);db_log_debug("====>>");
//     memset((void *)ctx->lv_video_layer.data, 0, MY_DISP_HOR_RES * MY_DISP_VER_RES * 3);db_log_debug("====>>");
//     ak_mem_dma_vaddr2paddr((void *)ctx->lv_video_layer.data, (unsigned long *)&ctx->lv_video_layer.layer.phyaddr);db_log_debug("====>>");
//     tde_layer_layer_init(ctx->lv_video_layer.layer, GP_FORMAT_RGB888, MY_DISP_HOR_RES, MY_DISP_VER_RES, 0, 0, MY_DISP_HOR_RES, MY_DISP_VER_RES);db_log_debug("====>>");
// }

// /* gui层数据拷贝到视频层 */
// static inline void _lv_gui_layer_to_video_layer(display_device_private_data *ctx)
// {
//     struct ak_tde_cmd opt;
//     memcpy(&(opt.tde_layer_src), &ctx->lv_gui_layer.layer, sizeof(struct ak_tde_layer));
//     memcpy(&(opt.tde_layer_dst), &ctx->lv_video_layer.layer, sizeof(struct ak_tde_layer));
//     opt.opt = GP_OPT_BLIT;
//     ak_tde_opt(&opt);
// }

// /* 视频层数据拷贝到framebuffer层 */
// static inline void _lv_video_layer_to_fb_layer(display_device_private_data *ctx)
// {
//     struct ak_tde_cmd opt;
//     memcpy(&(opt.tde_layer_src), &ctx->lv_video_layer.layer, sizeof(struct ak_tde_layer));
//     memcpy(&(opt.tde_layer_dst), &ctx->lv_fb_layer.layer, sizeof(struct ak_tde_layer));
//     /***** 切换到未在显示的buffer *****/
//     if (ctx->var_info.reserved[0] == 0)
//     {
//         opt.tde_layer_dst.phyaddr += ctx->var_info.xres * ctx->var_info.yres * 3;
//     }
//     opt.opt = GP_OPT_BLIT;
//     ak_tde_opt(&opt);
// }

// /* gui层数据拷贝到framebuffer层，用于视频模式，可根据刷新区域选择性刷新ui */
// static inline void _lv_gui_layer_to_fb_layer_in_video_mode(void)
// {
//     struct ak_tde_cmd opt;
//     memcpy(&(opt.tde_layer_src), &lv_gui_layer.layer, sizeof(struct ak_tde_layer));
//     memcpy(&(opt.tde_layer_dst), &lv_fb_layer.layer, sizeof(struct ak_tde_layer));
//     /***** 切换到未在显示的buffer *****/
//     if (var_info.reserved[0] == 0)
//     {
//         opt.tde_layer_dst.phyaddr += var_info.xres * var_info.yres * 3;
//     }
//     opt.opt = GP_OPT_BLIT;
//     for (int i = 0; i < gui_area_count; i++)
//     {
//         tde_layer_pos_init(opt.tde_layer_src, gui_area_group[i].pos_left, gui_area_group[i].pos_top, gui_area_group[i].pos_width, gui_area_group[i].pos_height);
//         tde_layer_pos_init(opt.tde_layer_dst, gui_area_group[i].pos_left, gui_area_group[i].pos_top, gui_area_group[i].pos_width, gui_area_group[i].pos_height);
//         ak_tde_opt(&opt);
//     }
// }

static inline unsigned long _lv_fb_back_buffer_phyaddr(display_device_private_data *ctx)
{
    unsigned long phyaddr = ctx->lv_fb_layer.layer.phyaddr;
    if (ctx->var_info.reserved[0] == 0)
    {
        phyaddr += ctx->var_info.xres * ctx->var_info.yres * 3;
    }
    return phyaddr;
}

static inline unsigned long _lv_fb_front_buffer_phyaddr(display_device_private_data *ctx)
{
    unsigned long phyaddr = ctx->lv_fb_layer.layer.phyaddr;
    if (ctx->var_info.reserved[0] != 0)
    {
        phyaddr += ctx->var_info.xres * ctx->var_info.yres * 3;
    }
    return phyaddr;
}

static inline unsigned char *_lv_fb_back_buffer_vaddr(display_device_private_data *ctx)
{
    unsigned char *addr = ctx->lv_fb_layer.data;
    if (ctx->var_info.reserved[0] == 0)
    {
        addr += ctx->var_info.xres * ctx->var_info.yres * 3;
    }
    return addr;
}

static inline unsigned char *_lv_fb_front_buffer_vaddr(display_device_private_data *ctx)
{
    unsigned char *addr = ctx->lv_fb_layer.data;
    if (ctx->var_info.reserved[0] != 0)
    {
        addr += ctx->var_info.xres * ctx->var_info.yres * 3;
    }
    return addr;
}

static inline bool _lv_tde_area_supported(const db_hal_position_t *area)
{
    return area->w >= TDE_MIN_AREA_WIDTH && area->h >= TDE_MIN_AREA_HEIGHT;
}

static inline void _lv_argb_to_fb_back_area_soft(display_device_private_data *ctx, const db_hal_position_t *area, unsigned char *data)
{
    unsigned char *dst = _lv_fb_back_buffer_vaddr(ctx) + (area->y * ctx->var_info.xres + area->x) * 3;
    unsigned char *src = data;

    for (int row = 0; row < area->h; row++)
    {
        unsigned char *dst_pixel = dst;
        unsigned char *src_pixel = src;

        for (int col = 0; col < area->w; col++)
        {
            if (src_pixel[3] == 0xff)
            {
                dst_pixel[0] = src_pixel[0];
                dst_pixel[1] = src_pixel[1];
                dst_pixel[2] = src_pixel[2];
            }
            else if (src_pixel[3] != 0)
            {
                unsigned int alpha = src_pixel[3];
                unsigned int inv_alpha = 255 - alpha;
                dst_pixel[0] = (src_pixel[0] * alpha + dst_pixel[0] * inv_alpha + 127) / 255;
                dst_pixel[1] = (src_pixel[1] * alpha + dst_pixel[1] * inv_alpha + 127) / 255;
                dst_pixel[2] = (src_pixel[2] * alpha + dst_pixel[2] * inv_alpha + 127) / 255;
            }

            src_pixel += 4;
            dst_pixel += 3;
        }

        src += area->w * 4;
        dst += ctx->var_info.xres * 3;
    }
}

static inline void _lv_fb_front_to_back_area_soft(display_device_private_data *ctx, const db_hal_position_t *area)
{
    unsigned char *src = _lv_fb_front_buffer_vaddr(ctx) + (area->y * ctx->var_info.xres + area->x) * 3;
    unsigned char *dst = _lv_fb_back_buffer_vaddr(ctx) + (area->y * ctx->var_info.xres + area->x) * 3;

    for (int row = 0; row < area->h; row++)
    {
        memcpy(dst, src, area->w * 3);
        src += ctx->var_info.xres * 3;
        dst += ctx->var_info.xres * 3;
    }
}

static inline void _lv_gui_layer_to_fb_layer_area(display_device_private_data *ctx, const db_hal_position_t *area)
{
    struct ak_tde_layer src, dst;
    memcpy(&src, &ctx->lv_gui_layer.layer, sizeof(struct ak_tde_layer));
    memcpy(&dst, &ctx->lv_fb_layer.layer, sizeof(struct ak_tde_layer));
    tde_layer_pos_init(src, area->x, area->y, area->w, area->h);
    tde_layer_pos_init(dst, area->x, area->y, area->w, area->h);
    dst.phyaddr = _lv_fb_back_buffer_phyaddr(ctx);
    ak_tde_opt_format(&src, &dst);
}

static inline int _lv_dma_argb_to_fb_layer_area(display_device_private_data *ctx, const db_hal_position_t *area, unsigned long src_phyaddr)
{
    struct ak_tde_layer src, dst;
    if (!_lv_tde_area_supported(area) || area->x != 0 || area->w != ctx->var_info.xres)
    {
        return -1;
    }

    tde_layer_layer_init(src, GP_FORMAT_ARGB8888, area->w, area->h, 0, 0, area->w, area->h);
    src.phyaddr = src_phyaddr;
    tde_layer_layer_init(dst, GP_FORMAT_RGB888, area->w, area->h, 0, 0, area->w, area->h);
    dst.phyaddr = _lv_fb_back_buffer_phyaddr(ctx) + area->y * ctx->var_info.xres * 3;
    return ak_tde_opt_format(&src, &dst);
}

static inline void _lv_fb_front_to_back_area(display_device_private_data *ctx, const db_hal_position_t *area)
{
    struct ak_tde_layer src, dst;
    if (!_lv_tde_area_supported(area))
    {
        _lv_fb_front_to_back_area_soft(ctx, area);
        return;
    }

    memcpy(&src, &ctx->lv_fb_layer.layer, sizeof(struct ak_tde_layer));
    memcpy(&dst, &ctx->lv_fb_layer.layer, sizeof(struct ak_tde_layer));
    tde_layer_pos_init(src, area->x, area->y, area->w, area->h);
    tde_layer_pos_init(dst, area->x, area->y, area->w, area->h);
    src.phyaddr = _lv_fb_front_buffer_phyaddr(ctx);
    dst.phyaddr = _lv_fb_back_buffer_phyaddr(ctx);
    ak_tde_opt_blit(&src, &dst);
}

static inline void _lv_fb_front_to_back_full(display_device_private_data *ctx)
{
    db_hal_position_t area = {
        .x = 0,
        .y = 0,
        .w = ctx->var_info.xres,
        .h = ctx->var_info.yres,
    };

    _lv_fb_front_to_back_area(ctx, &area);
}

static inline void _lv_gui_dirty_area_merge(display_device_private_data *ctx, const db_hal_position_t *area)
{
    int x1 = area->x;
    int y1 = area->y;
    int x2 = area->x + area->w;
    int y2 = area->y + area->h;

    if (!ctx->dirty_valid)
    {
        ctx->dirty_area = *area;
        ctx->dirty_valid = true;
        return;
    }

    if (x1 < ctx->dirty_area.x)
    {
        ctx->dirty_area.w += ctx->dirty_area.x - x1;
        ctx->dirty_area.x = x1;
    }
    if (y1 < ctx->dirty_area.y)
    {
        ctx->dirty_area.h += ctx->dirty_area.y - y1;
        ctx->dirty_area.y = y1;
    }

    if (x2 > ctx->dirty_area.x + ctx->dirty_area.w)
    {
        ctx->dirty_area.w = x2 - ctx->dirty_area.x;
    }
    if (y2 > ctx->dirty_area.y + ctx->dirty_area.h)
    {
        ctx->dirty_area.h = y2 - ctx->dirty_area.y;
    }
}

/* framebuffer双缓冲区切换 */
static inline void _lv_fb_layer_phyaddr_swap(display_device_private_data *ctx)
{
    ctx->var_info.reserved[0] = ctx->var_info.reserved[0] ? 0 : 1;
    ioctl(ctx->fd, FBIOPUT_VSCREENINFO, &ctx->var_info);
}

static void *sdl_renderer_task(void *arg)
{
    display_device_private_data *ctx = (display_device_private_data *)arg;
    while (ctx->is_running)
    {
        sem_wait(&ctx->refresh_sem);
        pthread_mutex_lock(&ctx->lock);
        if (ctx->lv_gui_layer.timestamp != 0 && ctx->dirty_valid)
        {
            ctx->lv_gui_layer.timestamp = 0;
            _lv_fb_layer_phyaddr_swap(ctx);
            if (ctx->force_full_sync)
            {
                _lv_fb_front_to_back_full(ctx);
                ctx->force_full_sync = false;
            }
            else
            {
                _lv_fb_front_to_back_area(ctx, &ctx->dirty_area);
            }
            ctx->dirty_valid = false;
        }
        pthread_mutex_unlock(&ctx->lock);
    }
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
    display_device_ctx->fd = open(FB_PATH, O_RDWR);
    if (display_device_ctx->fd < 0)
    {
        db_log_error("open\n");
        return NULL;
    }
    ioctl(display_device_ctx->fd, FBIOGET_VSCREENINFO, &display_device_ctx->var_info);
    display_device_ctx->var_info.activate |= FB_ACTIVATE_FORCE;
    display_device_ctx->var_info.activate |= FB_ACTIVATE_NOW;
    display_device_ctx->var_info.xres = display_device_ctx->var_info.xres_virtual;
    display_device_ctx->var_info.yres = display_device_ctx->var_info.yres_virtual;
    display_device_ctx->var_info.bits_per_pixel = 24;
    display_device_ctx->var_info.red.offset = 16;
    display_device_ctx->var_info.red.length = 8;
    display_device_ctx->var_info.green.offset = 8;
    display_device_ctx->var_info.green.length = 8;
    display_device_ctx->var_info.blue.offset = 0;
    display_device_ctx->var_info.blue.length = 8;
    ioctl(display_device_ctx->fd, FBIOPUT_VSCREENINFO, &display_device_ctx->var_info);
    ioctl(display_device_ctx->fd, FBIOGET_FSCREENINFO, &display_device_ctx->fix_info);
    _lv_fb_layer_phyaddr_swap(display_device_ctx);
    _lv_fb_layer_init(display_device_ctx);
    _lv_gui_layer_init(display_device_ctx);
    // _lv_video_layer_init(display_device_ctx);
    sem_init(&display_device_ctx->refresh_sem, 0, 0);
    pthread_mutex_init(&display_device_ctx->lock, NULL);
    display_device_ctx->is_running = true;
    display_device_ctx->is_video_mode = false;
    display_device_ctx->dirty_valid = false;
    display_device_ctx->force_full_sync = true;
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

    sem_destroy(&display_device_ctx->refresh_sem);
    pthread_mutex_destroy(&display_device_ctx->lock);
    close(ctx->fd);
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

    pthread_mutex_lock(&ctx->lock);

    unsigned long src_phyaddr = 0;
    if (!_lv_tde_area_supported(&frame->pos))
    {
        _lv_argb_to_fb_back_area_soft(ctx, &frame->pos, frame->data);
    }
    else if (ak_mem_dma_vaddr2paddr(frame->data, &src_phyaddr) == 0 && src_phyaddr != 0 &&
             _lv_dma_argb_to_fb_layer_area(ctx, &frame->pos, src_phyaddr) == 0)
    {
        /* Fast path: LVGL DMA draw buffer was converted directly into the back framebuffer. */
    }
    else
    {
        unsigned char *src = frame->data;
        unsigned char *dst = ctx->lv_gui_layer.data + (MY_DISP_HOR_RES * y + x) * 4;

        for (int i = 0; i < h; i++)
        {
            memcpy(dst, src, w * 4);
            src += w * 4;
            dst += MY_DISP_HOR_RES * 4;
        }
        _lv_gui_layer_to_fb_layer_area(ctx, &frame->pos);
    }

    _lv_gui_dirty_area_merge(ctx, &frame->pos);

    pthread_mutex_unlock(&ctx->lock);
    return 0;
}

static int _driver_gui_display_ioctl(void *arg, int cmd, void *param, void *data)
{
    display_device_private_data *ctx = (display_device_private_data *)arg;
    if (ctx == NULL)
    {
        return -1;
    }
    switch (cmd)
    {
    case GUI_DISPLAY_VIDEO_ENABLE_CMD:
        ctx->is_video_mode = *(bool *)param;
        break;
    case GUI_DISPLAY_FRAME_REFRESH_CMD:
        ctx->lv_gui_layer.timestamp = db_timestamp_ms_get();
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

    // int src_region[4] = {0, 0, frame->width, frame->hight}; // x, y, width, height
    // int dst_region[4] = {pos->x, pos->y, pos->w, pos->h};   // x, y, width, height

    // scale_yuv_region(frame->data, frame->width, frame->hight, src_region,
    //                  display_device_ctx->video_buffer, MY_DISP_HOR_RES, MY_DISP_VER_RES, dst_region);

    sem_post(&display_device_ctx->refresh_sem);
    return 0;
}

int display_device_video_buffer_clean(void)
{
    if (display_device_ctx == NULL)
    {
        return -1;
    }
    // memset(display_device_ctx->video_buffer, 0, MY_DISP_HOR_RES * MY_DISP_VER_RES);
    // memset(display_device_ctx->video_buffer + MY_DISP_HOR_RES * MY_DISP_VER_RES, 128, MY_DISP_HOR_RES * MY_DISP_VER_RES / 2);
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
