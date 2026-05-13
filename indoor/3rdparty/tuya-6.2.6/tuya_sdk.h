#ifndef _TUYA_SDK_H_
#define _TUYA_SDK_H_

#include <stdbool.h>
#include "db_common.h"
#include "db_time.h"
// #include "dyc_media_mgr.h"
// #include "dyc_video_player.h"
// #include "dyc_ui_event.h"
// #include "dyc_ringbuffer/dyc_ringbuffer.h"

#define STREAM_CLIENT_MAX 4

enum
{
    TUYA_EVENT_CMD_VIDEO_START,
    TUYA_EVENT_CMD_VIDEO_STOP,
    TUYA_EVENT_CMD_AUDIO_START,
    TUYA_EVENT_CMD_AUDIO_STOP,
    TUYA_EVENT_CMD_ONLINE_STATUS,
    TUYA_EVENT_CMD_SWITCH_CHANNEL,
    TUYA_EVENT_CMD_MOTION_ENBALE,
    TUYA_EVENT_CMD_DOOR_LOCK,
    TUYA_EVENT_CMD_ABSENT_MODE,
};

enum
{
    TUYA_OTA_UPGRADE_STATE_START,
    TUYA_OTA_UPGRADE_STATE_NO_SDCARD,
    TUYA_OTA_UPGRADE_STATE_CREATE_FAIL,
    TUYA_OTA_UPGRADE_STATE_WRITE_FAIL,
    TUYA_OTA_UPGRADE_STATE_CHECK_FAIL,
    TUYA_OTA_UPGRADE_STATE_CHECK_SUCCESS,
};

typedef struct
{
    char pid[24];
    char uuid[36];
    char key[36];
    char ver[16];
    char net_dev[8];
    char cache_dir[64];
    char sd_dir[64];
} tuya_init_config_t;

typedef struct
{
    int ch;
    const char *name;
} tuya_ch_info_t;

/*******************************************************************
 * @brief  : tuya sdk 初始化
 * @return  {*}
 * @param {tuya_init_config_t} *cfg
 *******************************************************************/
int tuya_sdk_init(const tuya_init_config_t *cfg);
/*******************************************************************
 * @brief  : tuya实时视频流推送至app
 * @return  {*}
 * @param {unsigned char} *data
 * @param {int} size
 * @param {unsigned long long} pts
 *******************************************************************/
int tuya_realtime_video_put_frame(unsigned char *data, int size, unsigned long long pts);
/*******************************************************************
 * @brief  : tuya实时音频流推送至app
 * @return  {*}
 * @param {unsigned char} *data
 * @param {int} size
 * @param {unsigned long long} pts
 *******************************************************************/
int tuya_realtime_audio_put_frame(unsigned char *data, int size, unsigned long long pts);
/*******************************************************************
 * @brief  : 从tuya app拉取实时音频流
 * @return  {*}
 * @param {unsigned char} *data
 * @param {int} size
 *******************************************************************/
int tuya_realtime_audio_get_frame(unsigned char *data, int size);
/*******************************************************************
 * @brief  : tuya有效通道上报
 * @return  {*}
 * @param {int} curr_ch
 * @param {tuya_ch_info_t} *info
 * @param {int} total
 *******************************************************************/
int tuya_channel_valid_report(int curr_ch, tuya_ch_info_t *info, int num);
/*******************************************************************
 * @brief  : 获取tuya二维码短链接
 * @return  {*}
 *******************************************************************/
const char *tuya_qrcode_shorturl_get(void);
/*******************************************************************
 * @brief  : 获取tuya在线状态
 * @return  {*}
 *******************************************************************/
bool tuya_online_status_get(void);
/*******************************************************************
 * @brief  : 获取tuya观看视频流客户端数量
 * @return  {*}
 *******************************************************************/
int tuya_client_num_get(void);
/*******************************************************************
 * @brief  : 获取tuya OTA升级包下载进度
 * @return  {*}
 *******************************************************************/
int tuya_ota_upgrade_progress_get(void);
/*******************************************************************
 * @brief  : 上报开启多锁功能
 * @return  {*}
 *******************************************************************/
int tuya_lock_support_report(void);
/*******************************************************************
 * @brief  : 上报开锁状态
 * @return  {*}
 * @param {int} index
 * @param {bool} state
 *******************************************************************/
int tuya_door_lock_report(int index, bool state);
/*******************************************************************
 * @brief  : 呼叫tuya app并上报图片
 * @return  {*}
 * @param {int} ch
 * @param {uint8_t} *jpeg_buf
 * @param {int} size
 *******************************************************************/
int tuya_notify_call_event(int ch, const uint8_t *jpeg_buf, int size);
/*******************************************************************
 * @brief  : 上报报警图片
 * @return  {*}
 * @param {int} ch
 * @param {uint8_t} *jpeg_buf
 * @param {int} size
 *******************************************************************/
int tuya_notify_alarm_event(int ch, const uint8_t *jpeg_buf, int size);
/*******************************************************************
 * @brief  : 上报移动侦测照片
 * @return  {*}
 * @param {int} ch
 * @param {uint8_t} *jpeg_buf
 * @param {int} size
 *******************************************************************/
int tuya_notify_motion_event(int ch, const uint8_t *jpeg_buf, int size);
/*******************************************************************
 * @brief  : 同步网络时间
 * @return  {*}
 * @param {tm} *new_tm
 *******************************************************************/
int tuya_network_time_sync(struct tm *new_tm);
/*******************************************************************
 * @brief  : 上报离家模式状态
 * @return  {*}
 * @param {bool} state
 *******************************************************************/
int tuya_absent_mode_report(bool state);
/*******************************************************************
 * @brief  : 设备激活
 * @return  {*}
 *******************************************************************/
int tuya_device_active_report(void);
/*******************************************************************
 * @brief  : 开锁消息
 * @return  {*}
 *******************************************************************/
int tuya_abnormal_unlock_report(void);
/*******************************************************************
 * @brief  : 详细报警消息
 * @return  {*}
 * @param {unsigned int} device 0~3
 * @param {unsigned int} sensor 0~1
 *******************************************************************/
int tuya_alarm_detailed_report(unsigned int device, unsigned int sensor);
/*******************************************************************
 * @brief  : tuya环形缓冲区清理
 * @return  {*}
 *******************************************************************/
void tuya_ring_buffer_clear(void);

int tuya_media_stream_stop(void);

int tuya_api_weather_get(void);

void tuya_sub_version_report(const char *version);

#endif