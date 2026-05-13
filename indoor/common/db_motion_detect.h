#ifndef _DB_MOTION_DETECT_H_
#define _DB_MOTION_DETECT_H_

typedef struct db_motion_detect_context_t db_motion_detect_context_t;

/**
 * @brief 打开运动检测模块
 * @param width 视频帧宽度
 * @param height 视频帧高度
 * @param threshold 宏块亮度变化阈值 (1~255)
 * @param sensitivity 灵敏度，即触发运动检测所需的最小变化宏块数量 (1~64)
 * @param trigger_cb 运动检测触发回调函数
 * @return 成功返回运动检测上下文指针，失败返回NULL
 */
db_motion_detect_context_t *db_motion_detect_open(int width, int height, int threshold, int sensitivity, void (*trigger_cb)(void));

/**
 * @brief 关闭运动检测模块
 * @param context 运动检测上下文指针
 * @return 成功返回0，失败返回-1
 */
int db_motion_detect_close(db_motion_detect_context_t *context);

/**
 * @brief 写入YUV帧数据进行运动检测
 * @param context 运动检测上下文指针
 * @param yuv_frame YUV帧数据指针
 * @param size 帧数据大小
 * @return 成功返回0，失败返回-1
 */
int db_motion_detect_write(db_motion_detect_context_t *context, const unsigned char *yuv_frame, unsigned int size);

#endif // _DB_MOTION_DETECT_H_