#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// #include "dyc_video_player.h"
// #include "dyc_video_record.h"
// #include "dyc_common.h"
// #include "dyc_time.h"
// #include "baresip_call_info.h"
// #include "lv_conf.h"
// #include "user_data.h"
// #include "monitor.h"
// #include "dycio_ev.h"
// #include "asterisk.h"

// static int monitor_channel = MON_CH_NONE;
// static mon_enter_flag_t monitor_enter_flag = MON_ENTER_MANUAL_DOOR_FLAG;
// static player_context_t monitor_ipcamera_ctx = NULL;

// /***********************************************
// ** 作者: leo.liu
// ** 日期: 2022-12-28 8:37:5
// ** 说明: 获取通道
// ***********************************************/
// int monitor_channel_get(void)
// {
// 	return monitor_channel;
// }

// /***********************************************
// ** 作者: leo.liu
// ** 日期: 2022-12-28 8:37:5
// ** 说明: 设置通道
// ***********************************************/
// void monitor_channel_set(int ch)
// {
// 	monitor_channel = ch;
// }

// /***********************************************
// ** 作者: leo.liu
// ** 日期: 2022-12-28 9:43:34
// ** 说明: 进入监控标志
// ***********************************************/
// void monitor_enter_flag_set(mon_enter_flag_t flag)
// {
// 	monitor_enter_flag = flag;
// }

// /***********************************************
// ** 作者: leo.liu
// ** 日期: 2022-12-28 9:44:13
// ** 说明: 获取进入监控标志
// ***********************************************/
// mon_enter_flag_t monitor_enter_flag_get(void)
// {
// 	return monitor_enter_flag;
// }

// /***********************************************
// ** 作者: leo.liu
// ** 日期: 2022-12-28 9:44:13
// ** 说明: 获取通道类型 0:door 1:cctv 2:other -1:error
// ***********************************************/
// int monitor_channel_type_get(int ch)
// {
// 	if (ch >= MON_CH_DOOR1 && ch <= MON_CH_DOOR6)
// 	{
// 		return MON_CH_TYPE_DOOR;
// 	}
// 	else if (ch >= MON_CH_CCTV1 && ch <= MON_CH_CCTV6)
// 	{
// 		return MON_CH_TYPE_CCTV;
// 	}
// 	else if (ch == MON_CH_LOBBY)
// 	{
// 		return MON_CH_TYPE_LOBBY;
// 	}
// 	else if (ch == MON_CH_GUARD)
// 	{
// 		return MON_CH_TYPE_GUARD;
// 	}
// 	return MON_CH_TYPE_NONE;
// }

// static void video_player_packet_cb(uint8_t *data, uint32_t size, uint64_t pts, void *user_data)
// {
// 	video_record_video_write((char *)data, size, !((data[4] & 0x1f) == 0x01));
// }
// static void audio_player_frame_cb(uint8_t *data, uint32_t size, uint64_t pts, void *user_data)
// {
// 	video_record_mic_write((char *)data, size);
// }

// void monitor_ipcamera_open(const char *url, bool disp)
// {
// 	dyc_video_player_stop(monitor_ipcamera_ctx);
// 	if (url == NULL || url[0] == '\0')
// 		return;
// 	player_config cfg = {0};
// 	cfg.file_name = url;
// 	cfg.v_pkt_cb = video_player_packet_cb;
// 	cfg.a_frame_cb = audio_player_frame_cb;
// 	cfg.x = 0;
// 	cfg.y = 0;
// 	cfg.width = LV_DISPLAY_WIDTH;
// 	cfg.height = LV_DISPLAY_HEIGHT;
// 	cfg.mode = PLAYER_MODE_VDEC | (disp ? PLAYER_MODE_DISP : 0);
// 	monitor_ipcamera_ctx = dyc_video_player_start(&cfg);
// }

// void monitor_ipcamera_close(void)
// {
// 	dyc_video_player_stop(monitor_ipcamera_ctx);
// 	monitor_ipcamera_ctx = NULL;
// }

// const char *monitor_channel_url_get(int channel, bool rtsp)
// {
// 	if (monitor_channel_type_get(channel) == MON_CH_TYPE_DOOR)
// 	{
// 		if (rtsp)
// 		{
// 			return network_data_get()->door_device[channel - MON_CH_DOOR1].rtsp_url;
// 		}
// 		else
// 		{
// 			return network_data_get()->door_device[channel - MON_CH_DOOR1].sip_url;
// 		}
// 	}
// 	else if (monitor_channel_type_get(channel) == MON_CH_TYPE_CCTV)
// 	{
// 		// return network_data_get()->cctv_device[channel - MON_CH_CCTV1].rtsp_url;
// 	}
// 	return NULL;
// }

// void monitor_open(bool refresh, bool rtsp)
// {
// 	if ((monitor_enter_flag == MON_ENTER_MANUAL_DOOR_FLAG) || (monitor_enter_flag == MON_ENTER_MANUAL_CCTV_FLAG))
// 	{
// 		if (monitor_channel_type_get(monitor_channel) == MON_CH_TYPE_CCTV)
// 		{
// 			monitor_ipcamera_open(monitor_channel_url_get(monitor_channel, rtsp), refresh);
// 		}
// 		else
// 		{
// 			if (rtsp == true)
// 			{
// 				monitor_ipcamera_open(monitor_channel_url_get(monitor_channel, rtsp), refresh);
// 			}
// 			else
// 			{
// 				char uri[128] = {0};
// 				sprintf(uri, "%s:5066", monitor_channel_url_get(monitor_channel, false));
// 				dyc_error_log(uri);
// 				baresip_event_dial(uri);
// 			}
// 		}
// 	}
// }
// /***********************************************
// ** 作者: leo.liu
// ** 日期: 2022-12-28 9:44:13
// ** 说明:关闭监控 bit0:sip bit1:ipc
// ***********************************************/
// void monitor_close(char flag)
// {
// 	if (flag & 0x01)
// 	{
// 		baresip_event_hangup(baresip_call_data_current_call_id());
// 	}

// 	if (flag & 0x02)
// 	{
// 		monitor_ipcamera_close();
// 	}
// }
