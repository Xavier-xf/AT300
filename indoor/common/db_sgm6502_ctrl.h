#ifndef _DB_SGM6502_CTRL_H_
#define _DB_SGM6502_CTRL_H_

typedef enum
{
    SGM6502_IN_NONE,
    SGM6502_IN_CH_1,
    SGM6502_IN_CH_2,
    SGM6502_IN_CH_3,
    SGM6502_IN_CH_4,
    SGM6502_IN_CH_5,
    SGM6502_IN_CH_6,
    SGM6502_IN_CH_7,
    SGM6502_IN_CH_8,
    SGM6502_IN_TOTAL,
} db_sgm6502_in_ch_t;

typedef enum
{
    SGM6502_OUT_CH_1,
    SGM6502_OUT_CH_2,
    SGM6502_OUT_CH_3,
    SGM6502_OUT_CH_4,
    SGM6502_OUT_CH_5,
    SGM6502_OUT_CH_6,
    SGM6502_OUT_TOTAL,
} db_sgm6502_out_ch_t;

typedef enum
{
    SGM6502_GAIN_6DB,
    SGM6502_GAIN_0DB,
    SGM6502_GAIN_TOTAL,
} db_sgm6502_gain_t;

/**
 * @brief 打开SGM6502设备
 * 
 * 初始化I2C连接，配置SGM6502设备，并返回设备上下文。
 * 
 * @param dev_id I2C总线设备ID(如1表示/dev/i2c-1)
 * @param addr SGM6502芯片的I2C地址
 * @return 成功返回设备上下文指针，失败返回NULL
 */
void *db_sgm6502_open(int dev_id, unsigned char addr);

/**
 * @brief 关闭SGM6502设备
 * 
 * 释放SGM6502设备的资源，包括关闭文件描述符、销毁互斥锁和释放内存。
 * 
 * @param context 设备上下文指针
 * @return 成功返回0，失败返回-1
 */
int db_sgm6502_close(void *context);

/**
 * @brief 配置SGM6502的输入输出通道和增益
 * 
 * 设置指定输出通道的输入源和增益值。
 * 
 * @param context 设备上下文指针
 * @param in 输入通道编号
 * @param out 输出通道编号
 * @param gain 增益值
 * @return 成功返回0，失败返回-1
 */
int db_sgm6502_write(void *context, db_sgm6502_in_ch_t in, db_sgm6502_out_ch_t out, db_sgm6502_gain_t gain);

/**
 * @brief 重置SGM6502设备
 * 
 * 将SGM6502的所有寄存器恢复为默认值，关闭所有输出通道并重置增益。
 * 
 * @param context 设备上下文指针
 * @return 成功返回0，失败返回-1
 */
int db_sgm6502_reset(void *context);

#endif // _DB_SGM6502_CTRL_H_