#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "db_common.h"
#include "user_data.h"
// #include "asterisk.h"

#define USER_DATA_PATH "/app/data/user_data.cfg"
#define NETWORK_DATA_PATH "/app/data/network_data.cfg"

/* **************************************************************************************** */
/* ************************************** 用户数据 **************************************** */
/* **************************************************************************************** */

static user_data_info user_data = {0};

static const user_data_info user_data_default =
	{
		.general = {
			.device_id = 0,
			.app_enable = false,
			.language = 0,
			.auto_sync_time_enable = false,
		},
		.sound = {
			.calling_volume = {4, 4, 4, 4},
			.calling_ring = {1, 2, 5, 1},
			.talking_volume = {4, 4, 4, 4},
			.speaker_sensitivity = {1, 1, 1, 1},
			.mic_sensitivity = {1, 1, 1, 1},
		},
		.network = {
			.wifi_enable = true,
		},
		.mode = {
			.auto_image_capture = false,
			.always_on_display = true,
			.absent_mode_delay = 60,
			.monitoring_time = 30,
			.door_open_time = 0,
		},
		.other = {
			.screen_brightness = 60,
			.security_password = "0000",
		},
		.do_not_disturb = false,
		.sensor_enable = {false},
		.alarm_enable = false,
		.tuya_enable_status = 0,
		.power_up_count = 0,
		.fastboot_count = 0,
		.sync_timestamp = 0,
};

#define USER_DATA_CHECK_RANGE_OUT(cur, min, max)                                          \
	if ((user_##data.cur < min) || (user_##data.cur > max))                               \
	{                                                                                     \
		printf("user data error %d(%d,%d) \n", (int)user_##data.cur, (int)min, (int)max); \
		user_##data.cur = user_##data##_default.cur;                                      \
	}

#define USER_DATA_GENERAL_CHECK_RANGE_OUT(x, min, max) USER_DATA_CHECK_RANGE_OUT(general.x, min, max)

#define USER_DATA_SOUND_CHECK_RANGE_OUT(x, min, max) USER_DATA_CHECK_RANGE_OUT(sound.x, min, max)

#define USER_DATA_NETWORK_CHECK_RANGE_OUT(x, min, max) USER_DATA_CHECK_RANGE_OUT(network.x, min, max)

#define USER_DATA_MODE_CHECK_RANGE_OUT(x, min, max) USER_DATA_CHECK_RANGE_OUT(mode.x, min, max)

#define USER_DATA_OTHER_CHECK_RANGE_OUT(x, min, max) USER_DATA_CHECK_RANGE_OUT(other.x, min, max)

/*******************************************************************
 * @brief  : 检验数据是否合法
 * @return  {*}
 *******************************************************************/
static void user_data_check_valid(void)
{
	/* 通用设置 */
	USER_DATA_GENERAL_CHECK_RANGE_OUT(device_id, 0, 3);
	USER_DATA_GENERAL_CHECK_RANGE_OUT(app_enable, 0, 1);
	USER_DATA_GENERAL_CHECK_RANGE_OUT(language, 0, 5);
	USER_DATA_GENERAL_CHECK_RANGE_OUT(auto_sync_time_enable, 0, 1);

	/* 声音设置 */
	for (int i = 0; i < SOUND_INFO_TOTAL; i++)
	{
		USER_DATA_SOUND_CHECK_RANGE_OUT(calling_volume[i], 1, 6);
		USER_DATA_SOUND_CHECK_RANGE_OUT(calling_ring[i], 1, 6);
		USER_DATA_SOUND_CHECK_RANGE_OUT(talking_volume[i], 1, 6);
		USER_DATA_SOUND_CHECK_RANGE_OUT(speaker_sensitivity[i], 1, 2);
		USER_DATA_SOUND_CHECK_RANGE_OUT(mic_sensitivity[i], 1, 2);
	}

	/* 网络设置 */
	USER_DATA_NETWORK_CHECK_RANGE_OUT(wifi_enable, 0, 1);

	/* 模式设置 */
	USER_DATA_MODE_CHECK_RANGE_OUT(auto_image_capture, 0, 1);
	USER_DATA_MODE_CHECK_RANGE_OUT(always_on_display, 0, 1);
	if (user_data.mode.absent_mode_delay != 15 &&
		user_data.mode.absent_mode_delay != 30 &&
		user_data.mode.absent_mode_delay != 45 &&
		user_data.mode.absent_mode_delay != 60)
	{
		user_data.mode.absent_mode_delay = user_data_default.mode.absent_mode_delay;
	}
	if (user_data.mode.monitoring_time != 30 &&
		user_data.mode.monitoring_time != 60 &&
		user_data.mode.monitoring_time != 120 &&
		user_data.mode.monitoring_time != 180)
	{
		user_data.mode.monitoring_time = user_data_default.mode.monitoring_time;
	}
	USER_DATA_MODE_CHECK_RANGE_OUT(door_open_time, 0, 9);

	/* 其他设置 */
	USER_DATA_OTHER_CHECK_RANGE_OUT(screen_brightness, 10, 100);
	user_data.other.security_password[sizeof(user_data.other.security_password) - 1] = 0;
	if (strlen(user_data.other.security_password) == 0 || strlen(user_data.other.security_password) > 8)
	{
		memcpy(user_data.other.security_password, user_data_default.other.security_password, sizeof(user_data.other.security_password));
	}
	USER_DATA_CHECK_RANGE_OUT(do_not_disturb, 0, 1);
	USER_DATA_CHECK_RANGE_OUT(sensor_enable[0], 0, 1);
	USER_DATA_CHECK_RANGE_OUT(sensor_enable[1], 0, 1);
	// user_data.tuya_enable_status = 0;
	// if (user_data.general.app_enable)
	// {
	// 	user_data.tuya_enable_status |= (1 << user_data.general.device_id);
	// }
	// else
	// {
	// 	user_data.tuya_enable_status &= ~(1 << user_data.general.device_id);
	// }
}
/*******************************************************************
 * @brief  : 用户数据初始化
 * @return  {*}
 *******************************************************************/
bool user_data_init(void)
{
	int file_size = 0;
	if ((file_size = db_file_size_get(USER_DATA_PATH)) != sizeof(user_data_info))
	{
		db_log_warn("user data size change, file size:[%d] data size:[%d] \n", file_size, sizeof(user_data_info));
		user_data = user_data_default;
		system("rm -rf " USER_DATA_PATH);
		user_data_save(false, false);
		return true;
	}
	int fd = open(USER_DATA_PATH, O_RDONLY);
	if (fd < 0)
	{
		db_log_error("read open %s fail \n", USER_DATA_PATH);
		user_data = user_data_default;
		return false;
	}
	read(fd, &user_data, sizeof(user_data_info));
	close(fd);
	user_data_check_valid();
	user_data_save(false, false);
	return true;
}
/*******************************************************************
 * @brief  : 用户数据保存
 * @return  {*}
 * @param {bool} sync
 * @param {int} skip_dev 要过滤的设备
 *******************************************************************/
bool user_data_save(bool sync, int skip_dev)
{
	int fd = open(USER_DATA_PATH, O_WRONLY | O_CREAT, 0644);

	if (fd < 0)
	{
		printf("write open %s fail \n", USER_DATA_PATH);
		return false;
	}
	if (sync) // 需要同步数据，主机:同步给所有分机; 分机:同步给主机
	{
		// asterisk_user_data_sync_force(skip_dev);
	}
	// if (active) // 是否是主动更改用户数据
	// {
	// 	// user_data.sync_timestamp = user_timestamp_get();
	// }
	write(fd, &user_data, sizeof(user_data_info));

	close(fd);
	system("sync");

	return true;
}
/*******************************************************************
 * @brief  : 获取用户数据
 * @return  {*}
 *******************************************************************/
user_data_info *user_data_get(void)
{
	return &user_data;
}
/*******************************************************************
 * @brief  : 用户数据复位
 * @return  {*}
 *******************************************************************/
void user_data_reset(void)
{
	user_data_info user_data_tmp = user_data;

	user_data = user_data_default;

	user_data.general.language = user_data_tmp.general.language;
	user_data.alarm_enable = user_data_tmp.alarm_enable;
	user_data.sensor_enable[0] = user_data_tmp.sensor_enable[0];
	user_data.sensor_enable[1] = user_data_tmp.sensor_enable[1];

	user_data_save(true, user_device_id);
	system("sync");
}
/*******************************************************************
 * @brief  : 获取默认用户数据
 * @return  {*}
 *******************************************************************/
user_data_info const *user_default_data_get(void)
{
	return &user_data_default;
}

/* **************************************************************************************** */
/* ************************************** 网络数据 **************************************** */
/* **************************************************************************************** */
static network_data_info network_data = {0};

static const network_data_info network_data_default = {
	.network = {
		.udhcp = false,
		.ipaddr = {0},
		.mask = "255.0.0.0",
		.gateway = "10.0.0.1",
		.dns = "8.8.8.8",
	},

	.local_server_ip = {0},

	.unit_number = 102,
	.room_number = 202,
	.proxy_number = {0},
	// .server = {
	// 	.guid = {0},
	// 	.name = {0},
	// 	.server_addr = {0},
	// 	.server_port = 0,
	// 	.sip_addr = {0},
	// 	.sip_port = 0,
	// 	.aptId = {0},
	// },
	// .account = {
	// 	.username = {0},
	// 	.password = {0},
	// },
	// .door_device = {0},
	// .cctv_device = {0},

	.md5 = {0},
};

#define NETWORK_DATA_CHECK_RANGE_OUT(cur, min, max)                              \
	if ((network_##data.cur < min) || (network_##data.cur > max))                \
	{                                                                            \
		printf("network data error %d(%d,%d) \n", network_##data.cur, min, max); \
		network_##data.cur = network_##data##_default.cur;                       \
	}

#if 0
static void printf_register_device(void)
{
	printf("\n");
	for (int i = 0; i < IPCAMERA_NUM_MAX; i++)
	{
		if (network_data.door_device[i].sip_url[0] != 0)
		{
			printf("door camera :%s\n", network_data.door_device[i].name);
			printf("accout:[%s:%s]\n", network_data.door_device[i].username, network_data.door_device[i].password);
			printf("ipaddr:[%s:%d]\n", network_data.door_device[i].ipaddr, network_data.door_device[i].port);
			printf("sip_url:%s\n", network_data.door_device[i].sip_url);
			printf("rtsp_url:%s\n", network_data.door_device[i].rtsp_url);
			printf("#############################################\n");
		}
	}
	printf("\n");
	for (int i = 0; i < IPCAMERA_NUM_MAX; i++)
	{
		if (network_data.cctv_device[i].rtsp_url[0] != 0)
		{
			printf("cctv camera :%s\n", network_data.cctv_device[i].name);
			printf("accout:[%s:%s]\n", network_data.cctv_device[i].username, network_data.cctv_device[i].password);
			printf("ipaddr:[%s:%d]\n", network_data.cctv_device[i].ipaddr, network_data.cctv_device[i].port);
			printf("rtsp_url:%s\n", network_data.cctv_device[i].rtsp_url);
			printf("#############################################\n");
		}
	}
}
#endif
/*******************************************************************
 * @brief  : 检验数据是否合法
 * @return  {*}
 *******************************************************************/
static void network_data_check_valid(void)
{
	// printf_register_device();
}
/*******************************************************************
 * @brief  : 网络数据初始化
 * @return  {*}
 *******************************************************************/
bool network_data_init(void)
{
	int file_size = 0;
	if ((file_size = db_file_size_get(NETWORK_DATA_PATH)) != sizeof(network_data_info))
	{
		db_log_warn("network data size change, file size:[%d] data size:[%d] \n", file_size, sizeof(network_data_info));
		network_data = network_data_default;
		system("rm -rf " NETWORK_DATA_PATH);
		network_data_save();
		return false;
	}
	int fd = open(NETWORK_DATA_PATH, O_RDONLY);
	if (fd < 0)
	{
		db_log_error("read open %s fail \n", NETWORK_DATA_PATH);
		network_data = network_data_default;
		return false;
	}
	read(fd, &network_data, sizeof(network_data_info));
	close(fd);
#if 0
	char buf[sizeof(network_data.md5)] = {0};
	memcpy(buf, network_data.md5, sizeof(network_data.md5));
	memset(network_data.md5, 0, sizeof(network_data.md5));
	db_md5_by_data((unsigned char *)&network_data, sizeof(network_data_info), network_data.md5, sizeof(network_data.md5));
	if (strncmp(buf, network_data.md5, sizeof(network_data.md5)))
	{
		db_log_error("md5 check fail");
		network_data_reset();
	}
	else
#endif
	{
		network_data_check_valid();
		network_data_save();
	}
	// 当前版本需要清理以下内容
	memset(network_data.door_device, 0, sizeof(network_data.door_device));
	return true;
}
/*******************************************************************
 * @brief  : 网络数据保存
 * @return  {*}
 *******************************************************************/
bool network_data_save(void)
{
	int fd = open(NETWORK_DATA_PATH, O_WRONLY | O_CREAT, 0644);
	if (fd < 0)
	{
		printf("write open %s fail \n", NETWORK_DATA_PATH);
		return false;
	}
#if 0
	char buf[sizeof(network_data.md5)] = {0};
	memset(network_data.md5, 0, sizeof(network_data.md5));
	dyc_common_md5_by_data((unsigned char *)&network_data, sizeof(network_data_info), buf, sizeof(buf));
	memcpy(network_data.md5, buf, sizeof(network_data.md5));
#endif
	write(fd, &network_data, sizeof(network_data_info));
	close(fd);
	system("sync");

	// if (user_data.general.device_id == 0)
	// {
	// 	// asterisk_server_sync_network_data_force(true);
	// }

	return true;
}
/*******************************************************************
 * @brief  : 获取网络数据
 * @return  {*}
 *******************************************************************/
network_data_info *network_data_get(void)
{
	return &network_data;
}
/*******************************************************************
 * @brief  : 网络数据复位
 * @return  {*}
 *******************************************************************/
void network_data_reset(void)
{
	network_data = network_data_default;
	network_data_save();
	system("sync");
}
/*******************************************************************
 * @brief  : 获取默认网络数据
 * @return  {*}
 *******************************************************************/
network_data_info const *network_default_data_get(void)
{
	return &network_data_default;
}
