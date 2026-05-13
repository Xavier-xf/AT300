// #include "layout_common.h"

// #define output_image_w_h 200

// typedef enum
// {
//     PLAY_MODE_NULL,        // 单曲播放自动结束
//     PLAY_MODE_AUTO_NEXT,   // 自动下一曲
//     PLAY_MODE_AUTO_PREV,   // 自动上一曲
//     PLAY_MODE_SINGLE_LOOP, // 单曲循环
//     PLAY_MODE_RANDOM,      // 随机播放
//     PLAY_MODE_TOTAL,
// } play_mode_t;

// static lv_obj_t *music_title_label = NULL;
// static lv_obj_t *music_disk_img = NULL;
// static lv_obj_t *music_name_label = NULL;
// static lv_obj_t *music_play_process_slider = NULL;
// static lv_obj_t *music_played_time_label = NULL;
// static lv_obj_t *music_total_time_label = NULL;
// static lv_obj_t *music_prev_btn = NULL;
// static lv_obj_t *music_play_btn = NULL;
// static lv_obj_t *music_next_btn = NULL;
// static lv_obj_t *music_vol_add_btn = NULL;
// static lv_obj_t *music_volume_label = NULL;
// static lv_obj_t *music_vol_sub_btn = NULL;
// static lv_obj_t *music_mode_btn = NULL;
// static lv_img_dsc_t music_img_dsc =
//     {
//         .header.always_zero = 0,
//         .header.cf = LV_IMG_CF_TRUE_COLOR,
//         .header.w = output_image_w_h,
//         .header.h = output_image_w_h,
//         .data_size = output_image_w_h * output_image_w_h * 4,
//         .data = NULL,
// };

// static const char *play_mode_string[PLAY_MODE_TOTAL] =
//     {
//         "单曲播放", "自动下一曲", "自动上一曲", "单曲循环", "随机播放"};

// static int music_volume = 10;
// static int music_index = 0;
// static int curr_music_index = 0;
// static int music_total = 0;
// static play_mode_t play_mode = PLAY_MODE_NULL;

// static bool music_process_slidar_press_flag = false;

// static void ffmpeg_decode_info_cb(const void *arg);
// static void ffmpeg_decode_finish_cb(const void *arg);
// static void music_load_success_cb_func(unsigned long arg1, unsigned long arg2);
// static void music_play_finish_cb_func(unsigned long arg1, unsigned long arg2);
// static void music_info_display_flush(void);
// static void music_play_start(void);

// static void music_title_label_create(void)
// {
//     music_title_label = lv_label_create(lv_scr_act());
//     lv_label_set_text(music_title_label, "音乐播放器");
//     lv_obj_set_style_text_font(music_title_label, WXJ_FONT(40), 0);
//     lv_obj_set_style_text_color(music_title_label, lv_color_hex(0x33FFFF), 0);
//     lv_obj_align(music_title_label, LV_ALIGN_TOP_MID, 0, 10);
// }

// static void music_back_btn_click(lv_event_t *e)
// {
//     goto_layout(pLAYOUT(home));
// }
// static void music_back_btn_create(void)
// {
//     layout_common_back_btn_create(lv_scr_act(), 30, 0, LV_EVENT_CLICKED, music_back_btn_click, NULL);
// }

// static void music_disk_img_create(void)
// {
//     music_disk_img = lv_img_create(lv_scr_act());
//     if (music_img_dsc.data_size)
//     {
//         lv_img_set_src(music_disk_img, &music_img_dsc);
//     }
//     else
//     {
//         lv_img_set_src(music_disk_img, UI_RES_PATH "ui_music_icon.png");
//     }
//     lv_obj_align(music_disk_img, LV_ALIGN_CENTER, 0, -50);
// }

// static void music_name_label_create(void)
// {
//     music_name_label = lv_label_create(lv_scr_act());
//     lv_obj_set_style_text_font(music_name_label, WXJ_FONT(16), 0);
//     lv_obj_set_style_text_color(music_name_label, lv_color_hex(0x33FFFF), 0);
// }

// static void music_play_process_slidar_event_cb(lv_event_t *e)
// {
//     lv_event_code_t code = lv_event_get_code(e);

//     if (code == LV_EVENT_PRESSED)
//     {
//         music_process_slidar_press_flag = true;
//     }
//     else if (code == LV_EVENT_CLICKED)
//     {
//         music_process_slidar_press_flag = false;
//         int total_time = 0;
//         ffmpeg_decode_duration_get(NULL, &total_time);
//         ffmpeg_decode_skip_frame(lv_map(lv_slider_get_value(music_play_process_slider), 0, 500, 0, total_time / 1000));
//     }
// }

// static void music_play_process_slidar_create(void)
// {
//     music_play_process_slider = lv_slider_create(lv_scr_act());
//     lv_obj_set_size(music_play_process_slider, 500, 10);
//     lv_obj_set_ext_click_area(music_play_process_slider, 15);
//     lv_obj_align(music_play_process_slider, LV_ALIGN_BOTTOM_MID, 0, -30);
//     lv_slider_set_range(music_play_process_slider, 0, 500);
//     lv_obj_add_event_cb(music_play_process_slider, music_play_process_slidar_event_cb, LV_EVENT_ALL, NULL);
// }

// static void music_played_time_label_create(void)
// {
//     music_played_time_label = lv_label_create(lv_scr_act());
//     lv_obj_set_style_text_font(music_played_time_label, WXJ_FONT(16), 0);
//     lv_obj_set_style_text_color(music_played_time_label, lv_color_hex(0x33FFFF), 0);
//     lv_label_set_text(music_played_time_label, "00:00");
//     lv_obj_align_to(music_played_time_label, music_play_process_slider, LV_ALIGN_OUT_LEFT_MID, -30, 0);
// }
// static void music_total_time_label_create(void)
// {
//     music_total_time_label = lv_label_create(lv_scr_act());
//     lv_obj_set_style_text_font(music_total_time_label, WXJ_FONT(16), 0);
//     lv_obj_set_style_text_color(music_total_time_label, lv_color_hex(0x33FFFF), 0);
//     lv_label_set_text(music_total_time_label, "00:00");
//     lv_obj_align_to(music_total_time_label, music_play_process_slider, LV_ALIGN_OUT_RIGHT_MID, 30, 0);
// }

// static void music_prev_btn_click(lv_event_t *e)
// {
//     printf("%s\n", __func__);
//     if (music_total <= 1)
//     {
//         return;
//     }

//     if (--music_index < 0)
//     {
//         music_index = music_total - 1;
//     }
//     music_info_display_flush();
//     music_play_start();
// }
// static void music_prev_btn_create(void)
// {
//     music_prev_btn = lv_btn_create(lv_scr_act());
//     lv_obj_set_size(music_prev_btn, 50, 50);
//     lv_obj_align(music_prev_btn, LV_ALIGN_BOTTOM_MID, -150, -70);
//     lv_obj_add_event_cb(music_prev_btn, music_prev_btn_click, LV_EVENT_CLICKED, NULL);

//     lv_obj_t *label = lv_label_create(music_prev_btn);
//     lv_label_set_text(label, "prev");
//     lv_obj_set_style_text_font(label, WXJ_FONT(20), 0);
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
// }

// static void music_play_btn_click(lv_event_t *e)
// {
//     printf("%s\n", __func__);
//     lv_obj_t *label = music_play_btn->user_data;
//     ffmpeg_decode_state_t state = ffmpeg_decode_state_get();
//     if (state == FFMPEG_DECODE_STATE_DECODE)
//     {
//         lv_label_set_text(label, "play");
//         ffmpeg_decode_pause();
//     }
//     else if (state == FFMPEG_DECODE_STATE_PAUSE)
//     {
//         lv_label_set_text(label, "pause");
//         ffmpeg_decode_start();
//         curr_music_index = music_index;
//     }
// }
// static void music_play_btn_create(void)
// {
//     music_play_btn = lv_btn_create(lv_scr_act());
//     lv_obj_set_size(music_play_btn, 60, 50);
//     lv_obj_align(music_play_btn, LV_ALIGN_BOTTOM_MID, 0, -70);
//     lv_obj_add_event_cb(music_play_btn, music_play_btn_click, LV_EVENT_CLICKED, NULL);

//     lv_obj_t *label = lv_label_create(music_play_btn);
//     music_play_btn->user_data = label;
//     if (ffmpeg_decode_state_get() == FFMPEG_DECODE_STATE_DECODE)
//     {
//         lv_label_set_text(label, "pause");
//     }
//     else
//     {
//         lv_label_set_text(label, "play");
//     }
//     lv_obj_set_style_text_font(label, WXJ_FONT(20), 0);
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
// }

// static void music_next_btn_click(lv_event_t *e)
// {
//     printf("%s\n", __func__);
//     if (music_total <= 1)
//     {
//         return;
//     }
//     if (++music_index >= music_total)
//     {
//         music_index = 0;
//     }
//     music_info_display_flush();
//     music_play_start();
// }
// static void music_next_btn_create(void)
// {
//     music_next_btn = lv_btn_create(lv_scr_act());
//     lv_obj_set_size(music_next_btn, 50, 50);
//     lv_obj_align(music_next_btn, LV_ALIGN_BOTTOM_MID, 150, -70);
//     lv_obj_add_event_cb(music_next_btn, music_next_btn_click, LV_EVENT_CLICKED, NULL);

//     lv_obj_t *label = lv_label_create(music_next_btn);
//     lv_label_set_text(label, "next");
//     lv_obj_set_style_text_font(label, WXJ_FONT(20), 0);
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
// }

// static void music_vol_set_btn_click(lv_event_t *e)
// {
//     lv_obj_t *obj = lv_event_get_target(e);
//     if (obj == music_vol_add_btn)
//     {
//         if (++music_volume > 10)
//         {
//             music_volume = 10;
//         }
//     }
//     else if (obj == music_vol_sub_btn)
//     {
//         if (--music_volume < 0)
//         {
//             music_volume = 0;
//         }
//     }
//     lv_label_set_text_fmt(music_volume_label, "%02d", music_volume);
//     audio_output_volume_set(music_volume * 10);
// }
// static void music_vol_add_btn_create(void)
// {
//     music_vol_add_btn = lv_btn_create(lv_scr_act());
//     lv_obj_set_size(music_vol_add_btn, 50, 50);
//     lv_obj_align(music_vol_add_btn, LV_ALIGN_LEFT_MID, 100, 0);
//     lv_obj_add_event_cb(music_vol_add_btn, music_vol_set_btn_click, LV_EVENT_CLICKED, NULL);

//     lv_obj_t *label = lv_label_create(music_vol_add_btn);
//     lv_label_set_text(label, "+");
//     lv_obj_set_style_text_font(label, WXJ_FONT(36), 0);
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
// }
// static void music_vol_sub_btn_create(void)
// {
//     music_vol_sub_btn = lv_btn_create(lv_scr_act());
//     lv_obj_set_size(music_vol_sub_btn, 50, 50);
//     lv_obj_align(music_vol_sub_btn, LV_ALIGN_LEFT_MID, 0, 0);
//     lv_obj_add_event_cb(music_vol_sub_btn, music_vol_set_btn_click, LV_EVENT_CLICKED, NULL);

//     lv_obj_t *label = lv_label_create(music_vol_sub_btn);
//     lv_label_set_text(label, "-");
//     lv_obj_set_style_text_font(label, WXJ_FONT(36), 0);
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
// }

// static void music_volume_label_create(void)
// {
//     music_volume_label = lv_label_create(lv_scr_act());
//     lv_label_set_text_fmt(music_volume_label, "%02d", music_volume);
//     lv_obj_set_style_text_font(music_volume_label, WXJ_FONT(40), 0);
//     lv_obj_set_style_text_color(music_volume_label, lv_color_hex(0x33FFFF), 0);
//     lv_obj_align_to(music_volume_label,
//                     music_vol_sub_btn,
//                     LV_ALIGN_OUT_RIGHT_MID,
//                     (lv_obj_get_x(music_vol_add_btn) - lv_obj_get_x2(music_vol_sub_btn) - lv_obj_get_width(music_volume_label)) / 2,
//                     0);
//     audio_output_volume_set(music_volume * 10);
// }

// static void music_mode_btn_click(lv_event_t *e)
// {
//     if (++play_mode > PLAY_MODE_RANDOM)
//     {
//         play_mode = PLAY_MODE_NULL;
//     }
//     lv_obj_t *label = music_mode_btn->user_data;
//     lv_label_set_text(label, play_mode_string[play_mode]);
// }
// static void music_mode_btn_create(void)
// {
//     music_mode_btn = lv_btn_create(lv_scr_act());
//     lv_obj_set_size(music_mode_btn, 100, 50);
//     lv_obj_align(music_mode_btn, LV_ALIGN_BOTTOM_RIGHT, -100, -70);
//     lv_obj_add_event_cb(music_mode_btn, music_mode_btn_click, LV_EVENT_CLICKED, NULL);

//     lv_obj_t *label = lv_label_create(music_mode_btn);
//     music_mode_btn->user_data = label;
//     lv_label_set_text(label, play_mode_string[play_mode]);
//     lv_obj_set_style_text_font(label, WXJ_FONT(20), 0);
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
// }

// static void music_process_slider_display_flush_timer(lv_timer_t *timer)
// {
//     if (ffmpeg_decode_state_get() != FFMPEG_DECODE_STATE_DECODE)
//         return;

//     int curr_time = 0, total_time = 0;
//     ffmpeg_decode_duration_get(&curr_time, &total_time);
//     if (total_time == 0)
//     {
//         lv_slider_set_value(music_play_process_slider, 0, LV_ANIM_ON);
//     }
//     else if (!music_process_slidar_press_flag)
//     {
//         int value = lv_slider_get_value(music_play_process_slider) * total_time / 500;
//         if (abs(curr_time - value) < 2000)
//             lv_slider_set_value(music_play_process_slider, curr_time * 500 / total_time, LV_ANIM_ON);
//     }

//     curr_time /= 1000;
//     lv_label_set_text_fmt(music_played_time_label, "%02d:%02d", curr_time / 60, curr_time % 60);
//     lv_obj_align_to(music_played_time_label, music_play_process_slider, LV_ALIGN_OUT_LEFT_MID, -30, 0);

//     total_time /= 1000;
//     lv_label_set_text_fmt(music_total_time_label, "%02d:%02d", total_time / 60, total_time % 60);
//     lv_obj_align_to(music_total_time_label, music_play_process_slider, LV_ALIGN_OUT_RIGHT_MID, 30, 0);
// }

// static void music_info_display_flush(void)
// {
//     if (ffmpeg_decode_state_get() == FFMPEG_DECODE_STATE_DECODE)
//     {
//         lv_label_set_text(music_play_btn->user_data, "pause");
//     }
//     else
//     {
//         lv_label_set_text(music_play_btn->user_data, "play");
//     }
//     lv_label_set_text(music_name_label, user_file_name_get(FILE_TYPE_FLASH_MUSIC, music_index));
//     lv_obj_align(music_name_label, LV_ALIGN_BOTTOM_MID, 0, -150);

//     lv_slider_set_value(music_play_process_slider, 0, LV_ANIM_ON);

//     lv_label_set_text(music_played_time_label, "00:00");
//     lv_obj_align_to(music_played_time_label, music_play_process_slider, LV_ALIGN_OUT_LEFT_MID, -30, 0);
// }

// LAYOUT_ENETER_FUNC(music)
// {
//     user_file_total_get(FILE_TYPE_FLASH_MUSIC, &music_total, NULL);
//     layout_common_bg_display(NULL, NULL, lv_color_hex(0));
//     music_title_label_create();
//     music_back_btn_create();
//     music_disk_img_create();
//     music_name_label_create();
//     music_play_process_slidar_create();
//     music_played_time_label_create();
//     music_total_time_label_create();
//     music_prev_btn_create();
//     music_play_btn_create();
//     music_next_btn_create();
//     music_vol_add_btn_create();
//     music_vol_sub_btn_create();
//     music_volume_label_create();
//     music_mode_btn_create();

//     music_info_display_flush();

//     lv_layout_timer_create(music_process_slider_display_flush_timer, 200, NULL);
//     user_msg_event_cb_register(MSG_EVENT_MUSIC_LOAD_SUCCESS, music_load_success_cb_func);
//     user_msg_event_cb_register(MSG_EVENT_MUSIC_PLAY_FINISH, music_play_finish_cb_func);

//     if (music_img_dsc.data == NULL)
//     {
//         music_img_dsc.data = (uint8_t *)malloc(music_img_dsc.data_size);
//     }

//     if (ffmpeg_decode_state_get() == FFMPEG_DECODE_STATE_IDLE)
//     {
//         ffmpeg_decode_open(user_file_path_get(FILE_TYPE_FLASH_MUSIC, music_index), NULL, ffmpeg_decode_info_cb, ffmpeg_decode_finish_cb, 200);
//     }
// }

// LAYOUT_QUIT_FUNC(music)
// {
//     play_mode = PLAY_MODE_NULL;
//     music_process_slidar_press_flag = false;
//     user_msg_event_cb_register(MSG_EVENT_MUSIC_LOAD_SUCCESS, NULL);
//     user_msg_event_cb_register(MSG_EVENT_MUSIC_PLAY_FINISH, NULL);
// }

// CREATE_LAYOUT(music)

// static void ffmpeg_decode_finish_cb(const void *arg)
// {
//     user_msg_event_send(MSG_EVENT_MUSIC_PLAY_FINISH, 0, 0);
// }

// static void ffmpeg_decode_info_cb(const void *arg)
// {
//     ff_out_info_t *out_info = (ff_out_info_t *)arg;
//     music_img_dsc.data_size = out_info->data_size;
//     lv_memcpy((uint8_t *)music_img_dsc.data, out_info->data, out_info->data_size);
//     user_msg_event_send(MSG_EVENT_MUSIC_LOAD_SUCCESS, 0, 0);
// }

// static void music_load_success_cb_func(unsigned long arg1, unsigned long arg2)
// {
//     int total_time = 0;
//     ffmpeg_decode_duration_get(NULL, &total_time);
//     total_time /= 1000;
//     printf("========>> [%s:%d] total_time:[%d]\n", __func__, __LINE__, total_time);
//     lv_label_set_text_fmt(music_total_time_label, "%02d:%02d", total_time / 60, total_time % 60);
//     lv_obj_align_to(music_total_time_label, music_play_process_slider, LV_ALIGN_OUT_RIGHT_MID, 30, 0);

//     if (music_img_dsc.data_size)
//     {
//         lv_img_set_src(music_disk_img, &music_img_dsc);
//     }
//     else
//     {
//         lv_img_set_src(music_disk_img, UI_RES_PATH "ui_music_icon.png");
//     }
//     lv_obj_align(music_disk_img, LV_ALIGN_CENTER, 0, -50);
// }

// static void music_play_finish_cb_func(unsigned long arg1, unsigned long arg2)
// {
//     printf("=============>>>%s<<<<============curr_music_index:%d music_index:%d play_mode:%d\n", __func__, curr_music_index, music_index, play_mode);
//     if (curr_music_index != music_index)
//     {
//         return;
//     }

//     switch (play_mode)
//     {
//     case PLAY_MODE_NULL:
//         music_info_display_flush();
//         ffmpeg_decode_open(user_file_path_get(FILE_TYPE_FLASH_MUSIC, music_index), NULL, ffmpeg_decode_info_cb, ffmpeg_decode_finish_cb, 200);
//         break;

//     case PLAY_MODE_AUTO_NEXT:
//         if (music_total <= 1)
//         {
//             return;
//         }
//         if (++music_index >= music_total)
//         {
//             music_index = 0;
//         }
//         music_info_display_flush();
//         ffmpeg_decode_open(user_file_path_get(FILE_TYPE_FLASH_MUSIC, music_index), NULL, ffmpeg_decode_info_cb, ffmpeg_decode_finish_cb, 200);
//         lv_label_set_text(music_play_btn->user_data, "pause");
//         ffmpeg_decode_start();
//         curr_music_index = music_index;
//         break;

//     case PLAY_MODE_AUTO_PREV:
//         if (music_total <= 1)
//         {
//             return;
//         }
//         if (--music_index < 0)
//         {
//             music_index = music_total - 1;
//         }
//         music_info_display_flush();
//         ffmpeg_decode_open(user_file_path_get(FILE_TYPE_FLASH_MUSIC, music_index), NULL, ffmpeg_decode_info_cb, ffmpeg_decode_finish_cb, 200);
//         lv_label_set_text(music_play_btn->user_data, "pause");
//         ffmpeg_decode_start();
//         curr_music_index = music_index;
//         break;

//     case PLAY_MODE_SINGLE_LOOP:
//         music_info_display_flush();
//         ffmpeg_decode_open(user_file_path_get(FILE_TYPE_FLASH_MUSIC, music_index), NULL, ffmpeg_decode_info_cb, ffmpeg_decode_finish_cb, 200);
//         lv_label_set_text(music_play_btn->user_data, "pause");
//         ffmpeg_decode_start();
//         curr_music_index = music_index;
//         break;

//     case PLAY_MODE_RANDOM:
//         srand((unsigned)time(NULL));
//         music_index = rand() % music_total;
//         printf("=================>> 随机索引：%d\n", music_index);
//         music_info_display_flush();
//         ffmpeg_decode_open(user_file_path_get(FILE_TYPE_FLASH_MUSIC, music_index), NULL, ffmpeg_decode_info_cb, ffmpeg_decode_finish_cb, 200);
//         lv_label_set_text(music_play_btn->user_data, "pause");
//         ffmpeg_decode_start();
//         curr_music_index = music_index;
//         break;

//     default:
//         break;
//     }
// }

// static void music_play_start(void)
// {
//     ffmpeg_decode_state_t state = ffmpeg_decode_state_get();
//     ffmpeg_decode_open(user_file_path_get(FILE_TYPE_FLASH_MUSIC, music_index), NULL, ffmpeg_decode_info_cb, ffmpeg_decode_finish_cb, 200);
//     if (state == FFMPEG_DECODE_STATE_DECODE)
//     {
//         lv_label_set_text(music_play_btn->user_data, "pause");
//         ffmpeg_decode_start();
//         curr_music_index = music_index;
//     }
// }