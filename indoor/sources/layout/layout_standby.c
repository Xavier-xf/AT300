// #include "layout_common.h"

// static lv_obj_t *time_cont = NULL;
// static lv_obj_t *hour_label = NULL;
// static lv_obj_t *min_label = NULL;
// static lv_obj_t *colon_label = NULL;
// static lv_obj_t *date_label = NULL;
// static lv_img_dsc_t standby_img_dsc;
// static int wallpaper_index = 0;
// static int wallpaper_total = 0;

// static void standby_bg_decode_finish_cb(unsigned long arg1, unsigned long arg2);
// static void image_decode_end_cb(const void *info);

// // 时间显示
// static void standby_time_display(struct tm *time)
// {
// 	lv_label_set_text_fmt(hour_label, "%02d", time->tm_hour);
// 	lv_label_set_text_fmt(min_label, "%02d", time->tm_min);
// }

// // 日期显示
// static void standby_date_display(struct tm *time)
// {
// 	lv_label_set_text_fmt(date_label, "%04d-%02d-%02d", time->tm_year, time->tm_mon, time->tm_mday);
// }

// // 时间和日期的刷新任务
// static void standby_time_display_timer(lv_timer_t *timer_t)
// {
// 	struct tm tm = {0};
// 	static struct tm prev_tm = {0};
// 	user_time_read(&tm);

// 	if (prev_tm.tm_min != tm.tm_min || timer_t == NULL)
// 	{
// 		standby_time_display(&tm);
// 	}
// 	if (prev_tm.tm_mday != tm.tm_mday || timer_t == NULL)
// 	{
// 		standby_date_display(&tm);
// 	}
// 	if (lv_obj_has_flag(colon_label, LV_OBJ_FLAG_HIDDEN))
// 	{
// 		lv_obj_clear_flag(colon_label, LV_OBJ_FLAG_HIDDEN);
// 	}
// 	else
// 	{
// 		lv_obj_add_flag(colon_label, LV_OBJ_FLAG_HIDDEN);
// 	}
// 	prev_tm = tm;
// }

// // 创建时间显示对象
// static bool standby_time_date_text_create(lv_obj_t *parent)
// {
// 	/***** 创建时间容器 *****/
// 	time_cont = lv_obj_create(parent);
// 	lv_obj_remove_style_all(time_cont);
// 	lv_obj_set_size(time_cont, 300, 200);
// 	lv_obj_clear_flag(time_cont, LV_OBJ_FLAG_CLICKABLE);
// 	lv_obj_set_style_bg_opa(time_cont, LV_OPA_TRANSP, LV_STATE_DEFAULT);
// 	lv_obj_set_style_border_width(time_cont, 0, LV_STATE_DEFAULT);
// 	lv_obj_align(time_cont, LV_ALIGN_TOP_MID, 0, 120);

// 	/***** 创建小时label控件 *****/
// 	hour_label = lv_label_create(time_cont);
// 	if (hour_label == NULL)
// 	{
// 		printf("standby create time(hour) label failed \n");
// 		return false;
// 	}
// 	lv_obj_set_style_text_font(hour_label, WXJ_FONT(64), LV_STATE_DEFAULT);
// 	lv_obj_set_style_text_color(hour_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
// 	lv_obj_set_style_text_letter_space(hour_label, 2, LV_STATE_DEFAULT);
// 	lv_label_set_text(hour_label, "00");

// 	/***** 创建分钟label控件 *****/
// 	min_label = lv_label_create(time_cont);
// 	if (min_label == NULL)
// 	{
// 		printf("standby create time(min) label failed \n");
// 		return false;
// 	}
// 	lv_obj_set_style_text_font(min_label, WXJ_FONT(64), LV_STATE_DEFAULT);
// 	lv_obj_set_style_text_color(min_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
// 	lv_obj_set_style_text_letter_space(min_label, 2, LV_STATE_DEFAULT);
// 	lv_label_set_text(min_label, "00");

// 	/***** 创建":"文本控件 *****/
// 	colon_label = lv_label_create(time_cont);
// 	if (colon_label == NULL)
// 	{
// 		printf("standby create time(:) obj failed \n");
// 		return false;
// 	}
// 	lv_obj_set_style_text_font(colon_label, WXJ_FONT(64), LV_STATE_DEFAULT);
// 	lv_obj_set_style_text_color(colon_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
// 	lv_obj_set_style_text_letter_space(colon_label, 2, LV_STATE_DEFAULT);
// 	lv_label_set_text(colon_label, ":");

// 	/***** 创建日期文本控件 *****/
// 	date_label = lv_label_create(time_cont);
// 	if (date_label == NULL)
// 	{
// 		printf("standby create time(:) obj failed \n");
// 		return false;
// 	}
// 	lv_obj_set_style_text_font(date_label, WXJ_FONT(26), LV_STATE_DEFAULT);
// 	lv_obj_set_style_text_color(date_label, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
// 	lv_obj_set_style_text_letter_space(date_label, 2, LV_STATE_DEFAULT);
// 	lv_label_set_text(date_label, "2022-12-12");

// 	struct tm tm = {0};
// 	user_time_read(&tm);
// 	standby_time_display(&tm);
// 	standby_date_display(&tm);

// 	lv_obj_align_to(colon_label, time_cont, LV_ALIGN_TOP_MID, 0, 0);
// 	lv_obj_align_to(hour_label, colon_label, LV_ALIGN_OUT_LEFT_MID, 0, 0);
// 	lv_obj_align_to(min_label, colon_label, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
// 	lv_obj_align_to(date_label, colon_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

// 	lv_layout_timer_create(standby_time_display_timer, 1000, NULL);
// 	return true;
// }

// static void standby_bg_btn_click(lv_event_t *e)
// {
// 	goto_layout(pLAYOUT(home));
// }

// static void standby_wallpaper_flush(void)
// {
//     jpg_decode_start(user_file_path_get(FILE_TYPE_FLASH_WALLPAPER, wallpaper_index), JPG_OUT_MODE_FULL, NULL, image_decode_end_cb);
// }

// static void standby_wallpaper_switch_timer(lv_timer_t *t)
// {
//     if (++wallpaper_index >= wallpaper_total)
//     {
//         wallpaper_index = 0;
//     }
//     standby_wallpaper_flush();
// }

// LAYOUT_ENETER_FUNC(standby)
// {
//     layout_common_bg_display(UI_RES_PATH "ui_home_bg_icon.png", standby_bg_btn_click, lv_color_hex(0));
// 	lv_memset_00(&standby_img_dsc, sizeof(lv_img_dsc_t));
// 	standby_time_date_text_create(lv_scr_act());

//     user_file_total_get(FILE_TYPE_FLASH_WALLPAPER, &wallpaper_total, NULL);

// 	if(wallpaper_total == 0)
//     {
//         return;
//     }

// 	lv_timer_t *timer = lv_layout_timer_create(standby_wallpaper_switch_timer, 10000, NULL);
//     if(wallpaper_total == 1)
//     {
//         lv_timer_set_repeat_count(timer, 1);
//     }

// 	user_msg_event_cb_register(MSG_EVENT_IMAGE_DECODE_FINISH, standby_bg_decode_finish_cb);
// }

// LAYOUT_QUIT_FUNC(standby)
// {
// 	user_msg_event_cb_register(MSG_EVENT_IMAGE_DECODE_FINISH, NULL);
// }

// CREATE_LAYOUT(standby)

// static void standby_bg_decode_finish_cb(unsigned long arg1, unsigned long arg2)
// {
// 	lv_obj_set_style_bg_img_src(lv_scr_act(), &standby_img_dsc, 0);
// 	lv_obj_set_style_bg_img_opa(lv_scr_act(), LV_OPA_COVER, 0);
// }

// static void image_decode_end_cb(const void *info)
// {
//     jpg_out_info_t *out_info = (jpg_out_info_t *)info;
//     standby_img_dsc.header.always_zero = 0;
//     standby_img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
//     standby_img_dsc.header.w = out_info->width;
//     standby_img_dsc.header.h = out_info->height;
//     standby_img_dsc.data_size = out_info->data_size;
//     standby_img_dsc.data = out_info->data;
//     user_msg_event_send(MSG_EVENT_IMAGE_DECODE_FINISH, 0, 0);
// }