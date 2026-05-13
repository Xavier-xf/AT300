// #include "layout_common.h"

// static void ffmpeg_decode_info_cb(const void *arg);
// static void video_decode_image_cb(unsigned long arg1, unsigned long arg2);

// static lv_obj_t *video_play_img = NULL;

// static lv_img_dsc_t video_img_dsc =
//     {
//         .header.always_zero = 0,
//         .header.cf = LV_IMG_CF_TRUE_COLOR,
//         .header.w = 0,
//         .header.h = 0,
//         .data_size = 0,
//         .data = NULL,
// };

// static void video_return_btn_click(lv_event_t *e)
// {
//     goto_layout(pLAYOUT(home));
// }

// static void video_decode_wait_timer(lv_timer_t *t)
// {
//     if (video_img_dsc.data_size != 0)
//     {
//         lv_img_set_src(video_play_img, &video_img_dsc);
//         lv_obj_center(video_play_img);
//         lv_timer_del(t);
//     }
// }

// LAYOUT_ENETER_FUNC(video)
// {
//     layout_common_bg_display(NULL, NULL, lv_color_hex(0));

//     // if (ffmpeg_decode_state_get() == FFMPEG_DECODE_STATE_IDLE)
//         ffmpeg_decode_open("/mnt/workdir/lvgl_arm_project/resource/video/video1.mp4", NULL, ffmpeg_decode_info_cb, NULL, -1);

//     video_play_img = lv_img_create(lv_scr_act());
//     lv_img_set_src(video_play_img, NULL);
//     lv_obj_center(video_play_img);

//     lv_timer_create(video_decode_wait_timer, 20, NULL);

//     if (video_img_dsc.data == NULL)
//     {
//         video_img_dsc.data = (uint8_t *)malloc(MY_DISP_HOR_RES * MY_DISP_VER_RES * 4);
//     }

//     user_msg_event_cb_register(MSG_EVENT_VIDEO_DECODE_FRAME, video_decode_image_cb);

//     ffmpeg_decode_start();
//     ffmpeg_decode_skip_frame(50000);
//     layout_common_back_btn_create(lv_scr_act(), 30, 0, LV_EVENT_CLICKED, video_return_btn_click, NULL);
// }

// LAYOUT_QUIT_FUNC(video)
// {
//     user_msg_event_cb_register(MSG_EVENT_VIDEO_DECODE_FRAME, NULL);
//     ffmpeg_decode_close();
//     video_img_dsc.data_size = 0;
// }

// CREATE_LAYOUT(video)

// static void ffmpeg_decode_info_cb(const void *arg)
// {
//     ff_out_info_t *out_info = (ff_out_info_t *)arg;
//     video_img_dsc.header.w = out_info->width;
//     video_img_dsc.header.h = out_info->height;
//     video_img_dsc.data_size = out_info->data_size;
//     // video_img_dsc.data = out_info->data;
//     lv_memcpy((uint8_t *)video_img_dsc.data, out_info->data, out_info->data_size);
//     user_msg_event_send(MSG_EVENT_VIDEO_DECODE_FRAME, 0, 0);
// }

// static void video_decode_image_cb(unsigned long arg1, unsigned long arg2)
// {
//     lv_img_cache_invalidate_src(video_play_img);
//     lv_obj_invalidate(video_play_img);
// }