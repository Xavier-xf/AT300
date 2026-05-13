#ifndef _USER_DATA_H_
#define _USER_DATA_H_

#include <stdbool.h>
#include <time.h>
// #include "kocom.h"

#define IPCAMERA_NUM_MAX 2

/* **************************************************************************************** */
/* ************************************** 用户数据 **************************************** */
/* **************************************************************************************** */

#define user_device_id (user_data_get()->general.device_id)

enum
{
	SOUND_INFO_CAMERA1,
	SOUND_INFO_CAMERA2,
	SOUND_INFO_INTERCOM,
	SOUND_INFO_LOBBY,
	SOUND_INFO_TOTAL,
};

typedef struct
{
	char device_id;				// 设备id 0~3
	bool app_enable;			// app使能 0~1
	char language;				// 语言 0:英语 1:韩语 2:西班牙语 3:阿拉伯语 4:越南语 5:俄语
	bool auto_sync_time_enable; // 自动同步网络时间使能 0~1
} user_general_info;

typedef struct
{
	char calling_volume[SOUND_INFO_TOTAL];		// 呼叫音量 1~6
	char calling_ring[SOUND_INFO_TOTAL];		// 呼叫铃声 1~6
	char talking_volume[SOUND_INFO_TOTAL];		// 通话音量 1~6
	char speaker_sensitivity[SOUND_INFO_TOTAL]; // 扬声器灵敏度 1~2
	char mic_sensitivity[SOUND_INFO_TOTAL];		// 麦克风灵敏度 1~2
} user_sound_info;

typedef struct
{
	bool wifi_enable; // wifi使能 0~1
} user_network_info;

typedef struct
{
	bool auto_image_capture; // 自动拍照 0~1
	bool always_on_display;	 // 待机显示 0~1
	char absent_mode_delay;	 // 安防报警启动延时 15, 30, 45, 60
	char monitoring_time;	 // 监控时长 30, 60, 120, 180
	char door_open_time;	 // 开门时长 0~9 0=0.2s
} user_mode_info;

typedef struct
{
	char screen_brightness;		// 屏幕亮度 10~100
	char security_password[16]; // 安防密码 1~8个字符
} user_other_info;

typedef struct
{
	user_general_info general;
	user_sound_info sound;
	user_network_info network;
	user_mode_info mode;
	user_other_info other;
	bool do_not_disturb;   // 勿扰模式 0~1
	bool sensor_enable[2]; // sensor使能 0~1
	bool alarm_enable;	   // 报警使能 0~1

	char tuya_enable_status; // bit0:dev1 bit1:dev2 bit2:dev3 bit3:dev4
	// 正常启动次数统计
	unsigned int power_up_count;
	// 快速启动次数统计
	unsigned int fastboot_count;

	unsigned long long sync_timestamp;

} user_data_info;

bool user_data_save(bool sync, int skip_dev);
bool user_data_init(void);
user_data_info *user_data_get(void);
void user_data_reset(void);
user_data_info const *user_default_data_get(void);

/* **************************************************************************************** */
/* ************************************** 网络数据 **************************************** */
/* **************************************************************************************** */

struct ipcamera_info
{
	char name[32];
	char ipaddr[16];
	int port;
	char username[32];
	char password[32];
	char sip_url[128];
	char rtsp_url[128];
	int channel;
};

struct network_info
{
	bool udhcp;
	char ipaddr[24];
	char mask[24];
	char dns[24];
	char gateway[24];
};

typedef struct
{
	struct network_info network;

	char local_server_ip[16];

	int unit_number; // 栋号
	int room_number; // 户号
	char proxy_number[32];
	// kocom_aptm_server_t server;	  // 服务器信息
	// kocom_aptm_account_t account; // 申请到的账号

	struct ipcamera_info door_device[IPCAMERA_NUM_MAX];
	// struct ipcamera_info cctv_device[IPCAMERA_NUM_MAX];
	char md5[32]; // 用于给服务器和账号信息校验
} network_data_info;

bool network_data_save(void);
bool network_data_init(void);
network_data_info *network_data_get(void);
void network_data_reset(void);
network_data_info const *network_default_data_get(void);

#endif
