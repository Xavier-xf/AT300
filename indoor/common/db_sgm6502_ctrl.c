#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include "db_sgm6502_ctrl.h"
#include "db_common.h"

#define db_sgm6502_lock() pthread_mutex_lock(&(sgm6502->mutex))
#define db_sgm6502_unlock() pthread_mutex_unlock(&(sgm6502->mutex))

typedef struct
{
    int fd;
    unsigned char value[3];
    unsigned char gain;
    pthread_mutex_t mutex;
} db_sgm6502_context_t;

/**
 * @brief 打开SGM6502设备
 * 
 * 初始化I2C连接，配置SGM6502设备，并返回设备上下文。
 * 
 * @param dev_id I2C总线设备ID(如1表示/dev/i2c-1)
 * @param addr SGM6502芯片的I2C地址
 * @return 成功返回设备上下文指针，失败返回NULL
 */
void *db_sgm6502_open(int dev_id, unsigned char addr)
{
    char path[16] = {0};
    db_sgm6502_context_t *sgm6502 = NULL;
    if (dev_id <= 0 || addr == 0)
    {
        db_log_error("Invalid param");
        return NULL;
    }

    sgm6502 = (db_sgm6502_context_t *)malloc(sizeof(db_sgm6502_context_t));
    if (sgm6502 == NULL)
    {
        db_log_error("malloc");
        return NULL;
    }

    memset(sgm6502, 0, sizeof(db_sgm6502_context_t));

    sprintf(path, "/dev/i2c-%d", dev_id);
    sgm6502->fd = open(path, O_RDWR);
    if (sgm6502->fd < 0)
    {
        db_log_error("open");
        free(sgm6502);
        return NULL;
    }

    if (ioctl(sgm6502->fd, I2C_SLAVE, addr) < 0)
    {
        db_log_error("ioctl");
        close(sgm6502->fd);
        free(sgm6502);
        return NULL;
    }

    if (pthread_mutex_init(&(sgm6502->mutex), NULL) != 0)
    {
        db_log_error("pthread_mutex_init");
        close(sgm6502->fd);
        free(sgm6502);
        return NULL;
    }

    db_log_debug("opened");
    return sgm6502;
}

/**
 * @brief 关闭SGM6502设备
 * 
 * 释放SGM6502设备的资源，包括关闭文件描述符、销毁互斥锁和释放内存。
 * 
 * @param context 设备上下文指针
 * @return 成功返回0，失败返回-1
 */
int db_sgm6502_close(void *context)
{
    db_sgm6502_context_t *sgm6502 = (db_sgm6502_context_t *)context;
    if (sgm6502 == NULL)
    {
        db_log_error("Invalid param");
        return -1;
    }
    close(sgm6502->fd);
    if (pthread_mutex_destroy(&(sgm6502->mutex)) != 0)
    {
        db_log_warn("pthread_mutex_destroy");
    }
    free(sgm6502);
    db_log_debug("closed");
    return 0;
}

/**
 * @brief 配置SGM6502的输入输出通道和增益
 * 
 * 设置指定输出通道的输入源和增益值。
 * 
 * @param context 设备上下文指针
 * @param in 输入通道编号(SGM6502_IN_NONE到SGM6502_IN_CH_8)
 * @param out 输出通道编号(SGM6502_OUT_CH_1到SGM6502_OUT_CH_6)
 * @param gain 增益值(SGM6502_GAIN_6DB或SGM6502_GAIN_0DB)
 * @return 成功返回0，失败返回-1
 */
int db_sgm6502_write(void *context, db_sgm6502_in_ch_t in, db_sgm6502_out_ch_t out, db_sgm6502_gain_t gain)
{
    const unsigned char reg_table[] = {0x00, 0x01, 0x02};
    db_sgm6502_context_t *sgm6502 = (db_sgm6502_context_t *)context;
    if (sgm6502 == NULL ||
        in < SGM6502_IN_NONE || in >= SGM6502_IN_TOTAL ||
        out < SGM6502_OUT_CH_1 || out >= SGM6502_OUT_TOTAL ||
        gain < SGM6502_GAIN_6DB || gain >= SGM6502_GAIN_TOTAL)
    {
        db_log_error("Invalid param");
        return -1;
    }
    int index = out / 2;
    unsigned char buf[2];

    db_sgm6502_lock();
    sgm6502->value[index] |= (out % 2 ? ((in << 4) & 0xF0) : (in & 0x0F));
    buf[0] = reg_table[index];
    buf[1] = sgm6502->value[index];
    if (write(sgm6502->fd, buf, 2) != 2)
    {
        db_sgm6502_unlock();
        db_log_error("write");
        return -1;
    }

    if (gain == SGM6502_GAIN_6DB)
    {
        sgm6502->gain &= ~(1 << out); // 设置6dB增益
    }
    else
    {
        sgm6502->gain |= (1 << out); // 设置0dB增益
    }
    buf[0] = 0x04;
    buf[1] = sgm6502->gain;
    if (write(sgm6502->fd, buf, 2) != 2)
    {
        db_sgm6502_unlock();
        db_log_error("write");
        return -1;
    }
    db_sgm6502_unlock();

    return 0;
}

/**
 * @brief 重置SGM6502设备
 * 
 * 将SGM6502的所有寄存器恢复为默认值，关闭所有输出通道并重置增益。
 * 
 * @param context 设备上下文指针
 * @return 成功返回0，失败返回-1
 */
int db_sgm6502_reset(void *context)
{
    const unsigned char reg_table[] = {0x00, 0x01, 0x02};
    db_sgm6502_context_t *sgm6502 = (db_sgm6502_context_t *)context;
    if (sgm6502 == NULL)
    {
        db_log_error("Invalid param");
        return -1;
    }
    unsigned char buf[2];

    db_sgm6502_lock();
    for (int i = 0; i < sizeof(reg_table) / sizeof(reg_table[0]); i++)
    {
        buf[0] = reg_table[i];
        buf[1] = sgm6502->value[i] = 0;
        if (write(sgm6502->fd, buf, 2) != 2)
        {
            db_sgm6502_unlock();
            db_log_error("write");
            return -1;
        }
    }

    buf[0] = 0x04;
    buf[1] = sgm6502->gain = 0;
    if (write(sgm6502->fd, buf, 2) != 2)
    {
        db_sgm6502_unlock();
        db_log_error("write");
        return -1;
    }
    db_sgm6502_unlock();

    return 0;
}