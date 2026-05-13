#include "layout_common.h"
#include "db_video_player.h"
#include "db_video_record.h"

enum
{
	home_setting_btn,
	home_monitor_btn,
	home_intercom_btn,
	home_media_btn,
	home_mute_btn,
	home_mode_btn,

	home_device_offline_img,
	home_new_media_img,

	home_clock_bg_img,
	home_clock_hour_img,
	home_clock_min_img,

	home_time_hour_label,
	home_time_colon_label,
	home_time_min_label,
	home_date_label,
	home_week_label,
	home_noon_label,

	home_device_label,
	home_wifi_img,
};

// static void home_device_state_change_cb(const dyc_ui_event *e);
// static void home_absent_mode_change_cb(const dyc_ui_event *e);
// static void home_device_id_conflict_cb(const dyc_ui_event *e);

static lv_obj_t *layout_home_main_img_text_btn_create(lv_obj_t *parent, int id, int x, int y, lv_event_cb_t cb, lv_opa_t def_bg_opa, const char *img, const char *txt, lv_font_t *font)
{
	lv_obj_t *obj = lv_obj_get_by_id(parent, id);
	if (obj != NULL)
	{
		return obj;
	}
	lv_obj_t *title_obj = NULL;
	{
		obj = layout_common_btn_create(parent, id, x, y, 136, 145, NULL, LV_ALIGN_DEFAULT,
									   cb, 0xFFFFFF, def_bg_opa, 0xFFFFFF, LV_OPA_20);
		lv_obj_set_style_radius(obj, 10, LV_STATE_DEFAULT);
		lv_obj_set_style_radius(obj, 10, LV_STATE_PRESSED);
	}
	{
		layout_common_img_create(obj, 0, 0, -36, 100, 100, obj, LV_ALIGN_BOTTOM_MID,
								 img, LV_ALIGN_BOTTOM_MID);
	}
	{
		title_obj = layout_common_text_create(obj, 1, 0, -15, 136, -1, obj, LV_ALIGN_BOTTOM_MID,
											  "", 0xFFFFFF, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, font);
		lv_obj_clear_flag(title_obj, LV_OBJ_FLAG_CLICKABLE);
		lv_label_set_long_mode(title_obj, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_label_set_text(title_obj, txt);
	}
	return obj;
}

static void home_setting_btn_click(lv_event_t *ev)
{
	// LAYOUT_GOTO(setting_general, LV_SCR_LOAD_ANIM_MOVE_TOP, );
}

static void home_monitor_btn_click(lv_event_t *ev)
{
	// if (asterisk_outdoor_online_check(MON_CH_DOOR1))
	// {
	// 	monitor_channel_set(MON_CH_DOOR1);
	// }
	// else if (asterisk_outdoor_online_check(MON_CH_DOOR2))
	// {
	// 	monitor_channel_set(MON_CH_DOOR2);
	// }
	// else
	// {
	// 	db_log_warn("device offline");
	// 	return;
	// }
	// monitor_enter_flag_set(MON_ENTER_MANUAL_DOOR_FLAG);
	LAYOUT_GOTO(monitor, );
}

static void home_device_offline_state_display(void)
{
	/* lv_obj_t *img =  */ layout_common_img_create(lv_scr_act(), home_device_offline_img, 187, 418, 32, 32, NULL, LV_ALIGN_DEFAULT,
													lv_ui_res_path_get("home_offline.png"), LV_ALIGN_CENTER);
	// lv_obj_t *label = lv_obj_get_by_id(lv_obj_get_by_id(lv_scr_act(), home_monitor_btn), 1);
	// if (img && label)
	// {
	// 	int online_total = 0;
	// 	asterisk_outdoor_online_total(&online_total);
	// 	if (online_total == 0)
	// 	{
	// 		lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
	// 		lv_label_set_text(label, language_string_get(LAYOUT_HOME_LANG_CAM_DISCONNET_ID));
	// 		lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	// 	}
	// 	else
	// 	{
	// 		lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
	// 		lv_label_set_text(label, language_string_get(LAYOUT_HOME_LANG_DOOR_CAM_ID));
	// 		lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	// 	}
	// }
}

static void home_new_media_state_display(void)
{
	// lv_obj_t *obj = layout_common_img_create(lv_scr_act(), home_new_media_img, 550, 418, 32, 32, NULL, LV_ALIGN_DEFAULT,
	// 										 lv_ui_res_path_get("home_new_media.png"), LV_ALIGN_CENTER);
	// if (obj)
	// {
	// 	unsigned long new_total = 0;
	// 	dyc_media_context ctx = dyc_media_open(dyc_flash_media_dir(), MEDIA_ATTR_ALL | MEDIA_TYPE_ALL | MEDIA_MODE_ALL | MEDIA_CHANNEL_ALL, 1);
	// 	dyc_media_first_page(ctx, NULL);
	// 	dyc_media_new_total_get(ctx, &new_total);
	// 	dyc_media_close(ctx);
	// 	if (new_total)
	// 	{
	// 		lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
	// 	}
	// 	else
	// 	{
	// 		lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
	// 	}
	// }
}

static void home_intercom_btn_click(lv_event_t *ev)
{
	// intercom_state_set(INTERCOM_STATE_IDLE);
	// intercom_device_set(INTERCOM_UNKNOWN_DEVICE);
	// intercom_mode_set(INTERCOM_INTRENAL_MODE);
	// LAYOUT_GOTO(intercom, );
}

static void home_media_btn_click(lv_event_t *ev)
{
	// db_video_player_config cfg = {
	// 	.path = "/home/wxj/视频/number-16k.mp4",
	// 	.disp_x = 0,
	// 	.disp_y = 0,
	// 	.disp_w = 1024,
	// 	.disp_h = 600,
	// };
	// db_video_player_start(&cfg);
	// db_video_record_config cfg = {
	// 	.path = "/home/wxj/视频/number-16k.mp4",
	// 	// .disp_x = 0,
	// 	// .disp_y = 0,
	// 	// .disp_w = 1024,
	// 	// .disp_h = 600,
	// };
	// db_video_record_start(&cfg);
	// LAYOUT_GOTO(playback, );
}

static void home_mute_btn_click(lv_event_t *ev)
{
	// user_data_get()->do_not_disturb = !user_data_get()->do_not_disturb;
	// user_data_save(false, false);
	// lv_obj_t *obj = lv_event_get_target(ev);
	// lv_obj_set_style_bg_opa(obj, user_data_get()->do_not_disturb ? LV_OPA_20 : LV_OPA_TRANSP, LV_STATE_DEFAULT);
	tuya_sub_version_report("0.12.01");
}

static void home_security_btn_click(lv_event_t *ev)
{
	// if (user_data_get()->alarm_enable)
	// {
	// 	input_pwd_mode_set(INPUT_PWD_MODE_ABSENT_AUTH);
	// 	LAYOUT_GOTO(input_pwd, );
	// }
	// else
	// {
	// 	LAYOUT_GOTO(security, );
	// }
	tuya_sub_version_report(NULL);
}
static void home_time_display(struct tm *tm)
{
	lv_obj_t *hour_label = layout_common_text_create(lv_scr_act(), home_time_hour_label, 660, 168, -1, -1, NULL, LV_ALIGN_DEFAULT,
													 NULL, 0xffffff, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_large_plus);
	if (user_data_get()->general.language == LANGUAGE_TYPE_ELuoSi)
		lv_label_set_text_fmt(hour_label, "%02d", tm->tm_hour);
	else
		lv_label_set_text_fmt(hour_label, "%02d", (tm->tm_hour % 12 == 0) ? 12 : tm->tm_hour % 12);

	lv_obj_t *colon_label = layout_common_text_create(lv_scr_act(), home_time_colon_label, 0, 0, -1, -1, hour_label, LV_ALIGN_OUT_RIGHT_BOTTOM,
													  ":", 0xffffff, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_large_plus);
	if (lv_obj_has_flag(colon_label, LV_OBJ_FLAG_HIDDEN))
		lv_obj_clear_flag(colon_label, LV_OBJ_FLAG_HIDDEN);
	else
		lv_obj_add_flag(colon_label, LV_OBJ_FLAG_HIDDEN);

	lv_obj_t *min_label = layout_common_text_create(lv_scr_act(), home_time_min_label, 0, 0, -1, -1, colon_label, LV_ALIGN_OUT_RIGHT_BOTTOM,
													NULL, 0xffffff, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_large_plus);
	lv_label_set_text_fmt(min_label, "%02d", tm->tm_min);
}

static void home_date_display(struct tm *tm)
{
	lv_obj_t *hour_label = lv_obj_get_by_id(lv_scr_act(), home_time_hour_label);
	if (!hour_label)
		return;

	lv_obj_t *date_label = layout_common_text_create(lv_scr_act(), home_date_label, 0, 0, -1, -1, hour_label, LV_ALIGN_OUT_LEFT_TOP,
													 NULL, 0xffffff, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_larger);
	lv_obj_set_style_text_letter_space(date_label, 3, LV_STATE_DEFAULT);
	char str[8] = {0};
	char buf[16] = {0};
	sprintf(str, "%02d/%02d", tm->tm_mon, tm->tm_mday);
	language_num_to_BaiJamjuree(str, buf);
	lv_label_set_text(date_label, buf);
	lv_obj_align_to(date_label, hour_label, LV_ALIGN_OUT_LEFT_TOP, -10, 20);
}

static void home_week_display(struct tm *tm)
{
	lv_obj_t *hour_label = lv_obj_get_by_id(lv_scr_act(), home_time_hour_label);
	if (!hour_label)
		return;

	lv_obj_t *week_label = layout_common_text_create(lv_scr_act(), home_week_label, 0, 0, -1, -1, hour_label, LV_ALIGN_OUT_LEFT_BOTTOM,
													 NULL, 0xffffff, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_large);

	lv_label_set_text(week_label, language_string_get(LAYOUT_HOME_LANG_MON_ID + tm->tm_wday - 1));
	lv_obj_set_style_text_color(week_label, lv_color_hex(tm->tm_wday > 5 ? 0xE53F3F : 0xBFC1C3), 0);
	lv_obj_align_to(week_label, hour_label, LV_ALIGN_OUT_LEFT_BOTTOM, -10, -10);
}

static void home_noon_display(struct tm *tm)
{
	if (user_data_get()->general.language != LANGUAGE_TYPE_ELuoSi)
	{
		lv_obj_t *min_label = lv_obj_get_by_id(lv_scr_act(), home_time_min_label);
		if (!min_label)
			return;

		lv_obj_t *noon_label = layout_common_text_create(lv_scr_act(), home_noon_label, 10, -10, -1, -1, min_label, LV_ALIGN_OUT_RIGHT_BOTTOM,
														 NULL, 0xBFC1C3, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_large);
		lv_label_set_text(noon_label, language_string_get(tm->tm_hour < 12 ? LAYOUT_HOME_LANG_AM_ID : LAYOUT_HOME_LANG_PM_ID));
	}
}

static void home_clock_display(struct tm *tm)
{
	lv_obj_t *parent = lv_obj_get_by_id(lv_scr_act(), home_clock_bg_img);
	if (parent == NULL)
	{
		db_log_warn("lv_obj_get_by_id");
		return;
	}
	lv_obj_t *hour = lv_obj_get_by_id(parent, home_clock_hour_img);
	if (hour)
	{
		lv_img_set_angle(hour, (3600 / 12) * (tm->tm_hour % 12) + (3600 / 60) * (tm->tm_min / 12));
	}
	lv_obj_t *min = lv_obj_get_by_id(parent, home_clock_min_img);
	if (min)
	{
		lv_img_set_angle(min, (3600 / 60) * (tm->tm_min));
	}
}

static void home_time_week_date_label_display_timer(lv_timer_t *t)
{
	struct tm tm = {0};
	static struct tm prev_tm = {0};
	db_time_get(&tm);

	home_time_display(&tm);
	if (prev_tm.tm_min != tm.tm_min)
	{
		home_noon_display(&tm);
		home_clock_display(&tm);
	}
	if (prev_tm.tm_mday != tm.tm_mday)
	{
		home_date_display(&tm);
		home_week_display(&tm);
	}
	prev_tm = tm;
}

static void home_time_create(void)
{
	lv_obj_t *clock_bg = lv_img_create(lv_scr_act());
	lv_obj_set_id(clock_bg, home_clock_bg_img);
	lv_img_set_src(clock_bg, lv_ui_res_path_get("home_clock.png"));
	lv_obj_align(clock_bg, LV_ALIGN_TOP_LEFT, 82, 70);

	lv_obj_t *minute = lv_img_create(clock_bg);
	lv_obj_set_id(minute, home_clock_min_img);
	lv_img_set_src(minute, lv_ui_res_path_get("home_min.png"));
	lv_img_set_pivot(minute, 5, 83 - 5);
	lv_obj_align(minute, LV_ALIGN_TOP_MID, 0, 84);

	lv_obj_t *hour = lv_img_create(clock_bg);
	lv_obj_set_id(hour, home_clock_hour_img);
	lv_img_set_src(hour, lv_ui_res_path_get("home_hour.png"));
	lv_img_set_pivot(hour, 5, 65 - 5);
	lv_obj_align(hour, LV_ALIGN_TOP_MID, 0, 102);

	struct tm tm = {0};
	db_time_get(&tm);
	home_time_display(&tm);
	home_date_display(&tm);
	home_week_display(&tm);
	home_noon_display(&tm);
	home_clock_display(&tm);
	lv_timer_recycle_create(home_time_week_date_label_display_timer, 1000, NULL);
}

static void home_wifi_state_display(bool force)
{
	static bool state = false, online = false;
	bool conneted = false, tuya_online = false;
	// wifi_device_connection_stauts(NULL, NULL, NULL, NULL, &conneted, NULL);
	// tuya_online = tuya_online_status_get();
	if (state != conneted || online != tuya_online || force)
	{
		lv_obj_t *obj = layout_common_img_create(lv_scr_act(), home_wifi_img, 897, 29, 40, 40, NULL, LV_ALIGN_DEFAULT,
												 NULL, LV_ALIGN_CENTER);
		lv_obj_set_style_bg_img_src(obj, (conneted && tuya_online) ? lv_ui_res_path_get("home_wifi_tuya.png") : (conneted ? lv_ui_res_path_get("home_wifi_on.png") : lv_ui_res_path_get("home_wifi_off.png")), 0);
	}
	state = conneted;
	online = tuya_online;
}

static void home_state_display_timer(lv_timer_t *t)
{
	home_wifi_state_display(false);
}

// static void home_goto_tuya_view_timer(lv_timer_t *t)
// {
// 	// monitor_close(0x01);
// 	// monitor_enter_flag_set(MON_ENTER_MANUAL_DOOR_FLAG);
// 	// LAYOUT_GOTO(monitor, );
// }

LAYOUT_ENTER_FUNC(home)
{
	layout_common_background_display(0x292e37, LV_OPA_TRANSP, NULL, NULL, true);
	// standby_timer_init(dyc_page_addres(standby), 30 * 1000);
	// standby_timer_restart(true);

	// if (tuya_client_num_get() > 0)
	// {
	// 	lv_timer_ready(lv_timer_recycle_create(home_goto_tuya_view_timer, 100, NULL));
	// 	return;
	// }

	layout_common_img_btn_create(lv_scr_act(), home_setting_btn, 16, 9, 80, 80, NULL, LV_ALIGN_DEFAULT,
								 home_setting_btn_click, 0, LV_OPA_TRANSP, 0, LV_OPA_TRANSP,
								 lv_ui_res_path_get("home_setting.png"), 0x4076BE, LV_OPA_COVER, LV_ALIGN_CENTER);

	layout_home_main_img_text_btn_create(lv_scr_act(), home_monitor_btn, 83, 418, home_monitor_btn_click, LV_OPA_TRANSP,
										 lv_ui_res_path_get("home_monitor.png"), language_string_get(LAYOUT_HOME_LANG_DOOR_CAM_ID), lv_font_small);

	layout_home_main_img_text_btn_create(lv_scr_act(), home_intercom_btn, 267, 418, home_intercom_btn_click, LV_OPA_TRANSP,
										 lv_ui_res_path_get("home_intercom.png"), language_string_get(LAYOUT_HOME_LANG_INTERCOM_ID), lv_font_small);

	layout_home_main_img_text_btn_create(lv_scr_act(), home_media_btn, 446, 418, home_media_btn_click, LV_OPA_TRANSP,
										 lv_ui_res_path_get("home_media.png"), language_string_get(LAYOUT_HOME_LANG_MEDIA_ID), lv_font_small);

	layout_home_main_img_text_btn_create(lv_scr_act(), home_mute_btn, 626, 418, home_mute_btn_click, user_data_get()->do_not_disturb ? LV_OPA_20 : LV_OPA_TRANSP,
										 lv_ui_res_path_get("home_mute.png"), language_string_get(LAYOUT_HOME_LANG_MUTE_ID), lv_font_small);

	layout_home_main_img_text_btn_create(lv_scr_act(), home_mode_btn, 810, 418, home_security_btn_click, user_data_get()->alarm_enable ? LV_OPA_20 : LV_OPA_TRANSP,
										 lv_ui_res_path_get("home_mode.png"), language_string_get(LAYOUT_HOME_LANG_SECURITY_ID), lv_font_small);

	home_device_offline_state_display();

	home_new_media_state_display();

	layout_common_text_create(lv_scr_act(), home_device_label, -146, 36, 400, -1, NULL, LV_ALIGN_TOP_RIGHT,
							  /* network_device_conflict_check() */ false ? language_string_get(LAYOUT_HOME_LANG_DEV_CONFLICT_ID) : language_string_get(user_device_id + LAYOUT_HOME_LANG_DEV1_ID),
							  /* network_device_conflict_check() */ false ? 0xFF0000 : 0xFFFFFF, LV_OPA_COVER, LV_TEXT_ALIGN_RIGHT, lv_font_smaller);

	home_wifi_state_display(true);

	home_time_create();

	lv_timer_recycle_create(home_state_display_timer, 2000, NULL);

	// dyc_ui_event_register(UI_EVENT_DEVICE_STATE_CHANGE, home_device_state_change_cb);
	// dyc_ui_event_register(UI_EVENT_ABSENT_MODE_CHANGE, home_absent_mode_change_cb);
	// dyc_ui_event_register(UI_EVENT_DEVICE_ID_CONFLICT, home_device_id_conflict_cb);

	tuya_api_weather_get();
}

LAYOUT_QUIT_FUNC(home)
{
	// standby_timer_init(dyc_page_addres(home), 30 * 1000);
	// standby_timer_restart(true);
	// dyc_ui_event_register(UI_EVENT_DEVICE_STATE_CHANGE, NULL);
	// dyc_ui_event_register(UI_EVENT_ABSENT_MODE_CHANGE, NULL);
	// dyc_ui_event_register(UI_EVENT_DEVICE_ID_CONFLICT, NULL);
}
LAYOUT_DEFINE(home);

// static void home_device_state_change_cb(const dyc_ui_event *e)
// {
// 	home_device_offline_state_display();
// }

// static void home_absent_mode_change_cb(const dyc_ui_event *e)
// {
// 	lv_obj_t *obj = lv_obj_get_by_id(lv_scr_act(), home_mode_btn);
// 	lv_obj_set_style_bg_opa(obj, user_data_get()->alarm_enable ? LV_OPA_20 : LV_OPA_TRANSP, LV_STATE_DEFAULT);
// }

// static void home_device_id_conflict_cb(const dyc_ui_event *e)
// {
// 	lv_obj_t *obj = lv_obj_get_by_id(lv_scr_act(), home_device_label);
// 	if (network_device_conflict_check())
// 	{
// 		lv_label_set_text(obj, language_string_get(LAYOUT_HOME_LANG_DEV_CONFLICT_ID));
// 		lv_obj_set_style_text_color(obj, lv_color_hex(0xFF0000), 0);
// 	}
// 	else
// 	{
// 		lv_label_set_text(obj, language_string_get(user_device_id + LAYOUT_HOME_LANG_DEV1_ID));
// 		lv_obj_set_style_text_color(obj, lv_color_hex(0xFFFFFF), 0);
// 	}
// }