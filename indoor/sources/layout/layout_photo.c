// #include "layout_common.h"

// static lv_obj_t *photo_name_label = NULL;
// static lv_obj_t *photo_index_label = NULL;
// static lv_obj_t *photo_play_img = NULL;
// static lv_obj_t *photo_load_cont = NULL;

// static lv_img_dsc_t photo_img_dsc;
// static int photo_index = 0;
// static int photo_total = 0;

// static void photo_decode_finish_cb(unsigned long arg1, unsigned long arg2);

// static void photo_decode_end_cb(const void *info)
// {
//     jpg_out_info_t *out_info = (jpg_out_info_t *)info;
//     photo_img_dsc.header.always_zero = 0;
//     photo_img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
//     photo_img_dsc.header.w = out_info->width;
//     photo_img_dsc.header.h = out_info->height;
//     photo_img_dsc.data_size = out_info->data_size;
//     photo_img_dsc.data = out_info->data;
//     user_msg_event_send(MSG_EVENT_IMAGE_DECODE_FINISH, 0, 0);
// }

// static void photo_flush(void)
// {
//     int ret = jpg_decode_start(user_file_path_get(FILE_TYPE_FLASH_PHOTO, photo_index), JPG_OUT_MODE_AUTO, NULL, photo_decode_end_cb);
//     if (ret != 0)
//     {
//         return;
//     }
//     lv_label_set_text(photo_name_label, user_file_name_get(FILE_TYPE_FLASH_PHOTO, photo_index));
//     lv_label_set_text_fmt(photo_index_label, "%02d/%02d", photo_index + 1, photo_total);
//     lv_obj_clear_flag(photo_load_cont, LV_OBJ_FLAG_HIDDEN);
// }

// static void photo_prev_btn_click(lv_event_t *e)
// {
//     if (photo_total <= 1)
//     {
//         return;
//     }
//     if (--photo_index < 0)
//     {
//         photo_index = photo_total - 1;
//     }
//     photo_flush();
// }
// static lv_obj_t *photo_prev_btn_create(void)
// {
//     lv_obj_t *btn = lv_obj_create(lv_scr_act());
//     lv_obj_remove_style_all(btn);
//     lv_obj_set_size(btn, 400, 480);
//     lv_obj_align(btn, LV_ALIGN_LEFT_MID, 0, 0);

//     lv_obj_add_event_cb(btn, photo_prev_btn_click, LV_EVENT_CLICKED, NULL);

//     return btn;
// }

// static void photo_next_btn_click(lv_event_t *e)
// {
//     if (photo_total <= 1)
//     {
//         return;
//     }

//     if (++photo_index >= photo_total)
//     {
//         photo_index = 0;
//     }
//     photo_flush();
// }
// static lv_obj_t *photo_next_btn_create(void)
// {
//     lv_obj_t *btn = lv_obj_create(lv_scr_act());
//     lv_obj_remove_style_all(btn);
//     lv_obj_set_size(btn, 400, 480);
//     lv_obj_align(btn, LV_ALIGN_RIGHT_MID, 0, 0);

//     lv_obj_add_event_cb(btn, photo_next_btn_click, LV_EVENT_CLICKED, NULL);

//     return btn;
// }

// void photo_loader_create(void)
// {
//     photo_load_cont = lv_obj_create(lv_scr_act());
//     lv_obj_remove_style_all(photo_load_cont);
//     lv_obj_set_size(photo_load_cont, 100, 100);
//     lv_obj_set_style_bg_color(photo_load_cont, lv_color_hex(0x53868B), 0);
//     lv_obj_set_style_bg_opa(photo_load_cont, LV_OPA_COVER, 0);
//     lv_obj_set_style_radius(photo_load_cont, 10, 0);
//     lv_obj_align(photo_load_cont, LV_ALIGN_TOP_MID, 0, 0);
//     lv_obj_center(photo_load_cont);

//     lv_obj_t *label = lv_label_create(photo_load_cont);
//     lv_obj_set_style_text_font(label, WXJ_FONT(16), 0);
//     lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
//     lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
//     lv_label_set_text(label, "loading...");

//     lv_obj_t *spinner = lv_spinner_create(photo_load_cont, 1000, 60);
//     lv_obj_set_size(spinner, 50, 50);
//     lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, -20);

//     lv_obj_add_flag(photo_load_cont, LV_OBJ_FLAG_HIDDEN);
// }

// static void photo_back_btn_click(lv_event_t *e)
// {
//     goto_layout(pLAYOUT(home));
// }

// LAYOUT_ENETER_FUNC(photo)
// {
//     photo_index = 0;
//     photo_total = 0;

//     user_file_total_get(FILE_TYPE_FLASH_PHOTO, &photo_total, NULL);

//     layout_common_bg_display(NULL, NULL, lv_color_hex(0));

//     lv_memset_00(&photo_img_dsc, sizeof(lv_img_dsc_t));
//     photo_play_img = lv_img_create(lv_scr_act());

//     photo_name_label = lv_label_create(lv_scr_act());
//     lv_obj_set_style_text_font(photo_name_label, WXJ_FONT(16), 0);
//     lv_obj_set_style_text_color(photo_name_label, lv_color_hex(0x33FFFF), 0);
//     lv_obj_set_style_bg_color(photo_name_label, lv_color_hex(0x000000), 0);
//     lv_obj_set_style_bg_opa(photo_name_label, LV_OPA_50, 0);
//     lv_obj_align(photo_name_label, LV_ALIGN_TOP_MID, 0, 0);
//     lv_label_set_text(photo_name_label, "");

//     photo_index_label = lv_label_create(lv_scr_act());
//     lv_obj_set_style_text_font(photo_index_label, WXJ_FONT(16), 0);
//     lv_obj_set_style_text_color(photo_index_label, lv_color_hex(0x33FFFF), 0);
//     lv_obj_set_style_bg_color(photo_index_label, lv_color_hex(0x000000), 0);
//     lv_obj_set_style_bg_opa(photo_index_label, LV_OPA_50, 0);
//     lv_obj_align(photo_index_label, LV_ALIGN_TOP_RIGHT, 0, 0);
//     lv_label_set_text(photo_index_label, "00/00");

//     photo_loader_create();
//     photo_prev_btn_create();
//     photo_next_btn_create();

//     layout_common_back_btn_create(lv_scr_act(), 30, 0, LV_EVENT_CLICKED, photo_back_btn_click, NULL);

//     if (photo_total == 0)
//     {
//         return;
//     }

//     photo_flush();

//     user_msg_event_cb_register(MSG_EVENT_IMAGE_DECODE_FINISH, photo_decode_finish_cb);
// }

// LAYOUT_QUIT_FUNC(photo)
// {
//     user_msg_event_cb_register(MSG_EVENT_IMAGE_DECODE_FINISH, NULL);
// }

// CREATE_LAYOUT(photo)

// static void photo_decode_finish_cb(unsigned long arg1, unsigned long arg2)
// {
//     lv_img_set_src(photo_play_img, &photo_img_dsc);
//     lv_obj_center(photo_play_img);
//     lv_obj_add_flag(photo_load_cont, LV_OBJ_FLAG_HIDDEN);
// }