#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include "db_motion_detect.h"
#include "db_common.h"

#define MOTION_DETECT_SKIP_FRAME 1
#define MACRO_BLOCK_HOR_NUM 8
#define MACRO_BLOCK_VER_NUM 8

#define db_motion_detect_lock(ctx) pthread_mutex_lock(&(ctx->mutex))
#define db_motion_detect_unlock(ctx) pthread_mutex_unlock(&(ctx->mutex))

typedef struct db_motion_detect_context_t
{
    unsigned char *buffer;
    unsigned char macro_block[MACRO_BLOCK_HOR_NUM * MACRO_BLOCK_VER_NUM];
    int width;
    int height;
    unsigned char threshold;
    unsigned char sensitivity;
    void (*trigger_cb)(void);
    bool running;
    sem_t sem;
    pthread_t thread_id;
    pthread_mutex_t mutex;
} db_motion_detect_context_t;

static void *db_motion_detect_task(void *param)
{
    db_motion_detect_context_t *context = (db_motion_detect_context_t *)param;
    int frame_count = 0;
    int trigger_count = 0;
    int block_width = context->width / MACRO_BLOCK_HOR_NUM;
    int block_height = context->height / MACRO_BLOCK_VER_NUM;
    int block_pixel = block_width * block_height;
    while (1)
    {
        sem_wait(&context->sem);
        if (!context->running)
        {
            break;
        }
        trigger_count = 0;
        db_motion_detect_lock(context);
        for (int i = 0; i < MACRO_BLOCK_VER_NUM; i++)
        {
            for (int j = 0; j < MACRO_BLOCK_HOR_NUM; j++)
            {
                unsigned char *p_buf = context->buffer + (i * block_height * context->width) + (j * block_width);
                unsigned int sum = 0;
                for (int k = 0; k < block_height; k++)
                {
                    for (int l = 0; l < block_width; l++)
                    {
                        sum += p_buf[l];
                    }
                    p_buf += context->width;
                }
                unsigned char temp = sum / block_pixel;
                if (abs(context->macro_block[i * MACRO_BLOCK_HOR_NUM + j] - temp) >= context->threshold)
                {
                    trigger_count++;
                }
                context->macro_block[i * MACRO_BLOCK_HOR_NUM + j] = temp;
            }
        }
        db_motion_detect_unlock(context);
        if (frame_count++ > MOTION_DETECT_SKIP_FRAME)
        {
            if (trigger_count >= context->sensitivity)
            {
                db_log_debug("Moving, trigger macro block:%d", trigger_count);
                if (context->trigger_cb)
                {
                    context->trigger_cb();
                }
            }
        }
    }
    pthread_exit(NULL);
    return NULL;
}

/**
 * @brief 打开运动检测模块
 * @param width 视频帧宽度
 * @param height 视频帧高度
 * @param threshold 宏块亮度变化阈值 (1~255)
 * @param sensitivity 灵敏度，即触发运动检测所需的最小变化宏块数量 (1~64)
 * @param trigger_cb 运动检测触发回调函数
 * @return 成功返回运动检测上下文指针，失败返回NULL
 */
db_motion_detect_context_t *db_motion_detect_open(int width, int height, int threshold, int sensitivity, void (*trigger_cb)(void))
{
    db_motion_detect_context_t *context = NULL;
    if (width <= 0 || height <= 0 || threshold < 1 || threshold > 255 || sensitivity < 1 || sensitivity > 64)
    {
        db_log_error("Invalid param");
        return NULL;
    }

    context = (db_motion_detect_context_t *)malloc(sizeof(db_motion_detect_context_t));
    if (context == NULL)
    {
        db_log_error("malloc");
        return NULL;
    }

    memset(context, 0, sizeof(db_motion_detect_context_t));

    context->buffer = (unsigned char *)malloc(width * height);
    if (context->buffer == NULL)
    {
        db_log_error("malloc");
        free(context);
        return NULL;
    }

    if (pthread_mutex_init(&(context->mutex), NULL) != 0)
    {
        db_log_error("pthread_mutex_init");
        free(context->buffer);
        free(context);
        return NULL;
    }

    if (sem_init(&context->sem, 0, 0) != 0)
    {
        db_log_error("sem_init");
        pthread_mutex_destroy(&(context->mutex));
        free(context->buffer);
        free(context);
        return NULL;
    }

    context->width = width;
    context->height = height;
    context->threshold = threshold;
    context->sensitivity = sensitivity;
    context->trigger_cb = trigger_cb;
    context->running = true;
    if (pthread_create(&(context->thread_id), NULL, db_motion_detect_task, context) != 0)
    {
        db_log_error("pthread_create");
        sem_destroy(&context->sem);
        pthread_mutex_destroy(&(context->mutex));
        free(context->buffer);
        free(context);
        return NULL;
    }

    db_log_debug("opened");
    return context;
}

/**
 * @brief 关闭运动检测模块
 * @param context 运动检测上下文指针
 * @return 成功返回0，失败返回-1
 */
int db_motion_detect_close(db_motion_detect_context_t *context)
{
    if (context == NULL)
    {
        db_log_error("Invalid param");
        return -1;
    }
    context->running = false;
    sem_post(&context->sem);
    pthread_join(context->thread_id, NULL);
    sem_destroy(&context->sem);
    if (pthread_mutex_destroy(&(context->mutex)) != 0)
    {
        db_log_warn("pthread_mutex_destroy");
    }
    if (context->buffer)
    {
        free(context->buffer);
    }
    free(context);
    db_log_debug("closed");
    return 0;
}

/**
 * @brief 写入YUV帧数据进行运动检测
 * @param context 运动检测上下文指针
 * @param yuv_frame YUV帧数据指针
 * @param size 帧数据大小
 * @return 成功返回0，失败返回-1
 */
int db_motion_detect_write(db_motion_detect_context_t *context, const unsigned char *yuv_frame, unsigned int size)
{
    if (context == NULL || yuv_frame == NULL)
    {
        db_log_error("Invalid param");
        return -1;
    }
    db_motion_detect_lock(context);
    if (context->height * context->width * 3 / 2 != size)
    {
        db_motion_detect_unlock(context);
        db_log_error("frame size error %d", size);
        return -1;
    }
    memcpy(context->buffer, yuv_frame, context->height * context->width);
    sem_post(&context->sem);
    db_motion_detect_unlock(context);
    return 0;
}
