
#ifndef _USER_MONITOR_H_
#define _USER_MONITOR_H_

#include <stdbool.h>

typedef enum
{
	MON_CH_NONE = -1,
	MON_CH_DOOR1,
	MON_CH_DOOR2,
	MON_CH_DOOR3,
	MON_CH_DOOR4,
	MON_CH_DOOR5,
	MON_CH_DOOR6,
	MON_CH_CCTV1,
	MON_CH_CCTV2,
	MON_CH_CCTV3,
	MON_CH_CCTV4,
	MON_CH_CCTV5,
	MON_CH_CCTV6,
	MON_CH_LOBBY,
	MON_CH_GUARD,
	MON_CH_TOTAL,
} mon_ch_t;

enum
{
	MON_CH_TYPE_NONE = -1,
	MON_CH_TYPE_DOOR,
	MON_CH_TYPE_CCTV,
	MON_CH_TYPE_LOBBY,
	MON_CH_TYPE_GUARD,
};

typedef enum
{
	MON_ENTER_CALL_FLAG,
	MON_ENTER_TUYA_TALK_FLAG,
	MON_ENTER_MANUAL_TALK_FLAG,
	MON_ENTER_CALL_TALK_FLAG,
	MON_ENTER_MANUAL_DOOR_FLAG,
	MON_ENTER_MANUAL_CCTV_FLAG
} mon_enter_flag_t;

/* 获取监控通道 */
int monitor_channel_get(void);
/* 设置监控通道 */
void monitor_channel_set(int ch);
/* 获取进入监控标志 */
mon_enter_flag_t monitor_enter_flag_get(void);
/* 设置进入监控标志 */
void monitor_enter_flag_set(mon_enter_flag_t flag);
/* 根据通道号获取设备类型 */
int monitor_channel_type_get(int ch);
/* 拉取ipc的rtsp流 */
void monitor_ipcamera_open(const char *url, bool disp);
/* 关闭rtsp流 */
void monitor_ipcamera_close(void);
/* 打开监控 */
void monitor_open(bool refresh, bool rtsp);
/* 关闭监控 */
void monitor_close(char flag);
// /* 获取监控通道的地址 */
// const char *monitor_channel_url_get(int channel, bool rtsp);
// /* 依据uri获取门口机通道 */
// int monitor_index_get_by_uri(const char *uri);
// /* 依据uri获取分机通道 */
// int extern_index_get_by_uri(const char *uri);
// /* 获取上一个有效通道 */
// int monitor_valid_channel_prev_get(void);
// /* 获取下一个有效通道 */
// int monitor_valid_channel_next_get(void);
// /* 获取第一个有效的通道 */
// int monitor_valid_channel_first_get(void);
// /* 获取最后一个有效通道 */
// int monitor_door_last_valid_get(bool door_camera);

// // 获取门口机和CCTV的注册状态
// bool monitor_door_registered_status_get(void);
// /* 判断通道是否有效 */
// bool monitor_valid_channel_check(int channel);
// /* 门口机注册数量获取 */
// int door_camera_register_num_get(void);
// /* CCTV注册数量获取 */
// int cctv_camera_register_num_get(void);

#endif // _USER_MONITOR_H_