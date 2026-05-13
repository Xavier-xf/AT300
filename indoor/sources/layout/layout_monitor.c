#include "layout_common.h"
#include "db_rtsp_player.h"

enum
{
    monitor_back_btn,
    monitor_count_down_label,
    monitor_switch_camera1_btn,
    monitor_switch_camera2_btn,

    monitor_talk_btn,
    monitor_record_btn,
    monitor_lock_btn,

    monitor_bright_cont,
    monitor_outdoor_vol_cont,
    monitor_indoor_vol_cont,

    monitor_call_tips_img,
    monitor_open_tip_label,
    monitor_tuya_view_icon_img,
    monitor_tuya_view_tip_label,
    monitor_camera_signal_lost_tip_win,
};

static void *rtsp_context[4] = {NULL};

static void monitor_back_btn_click(lv_event_t *e)
{
    LAYOUT_GOTO(home, );
}

LAYOUT_ENTER_FUNC(monitor)
{
    // layout_common_bg_display(NULL, NULL, lv_color_hex(0));
    layout_common_background_display(0, LV_OPA_TRANSP, NULL, NULL, true);

    lv_obj_t *obj = layout_common_imgbtn_create(lv_scr_act(), monitor_back_btn, 30, 30, 50, 50, NULL, LV_ALIGN_DEFAULT,
                                                monitor_back_btn_click, lv_ui_res_path_get("monitor_back_default.png"), lv_ui_res_path_get("monitor_back_press.png"));
    lv_obj_set_ext_click_area(obj, 15);

    db_rtsp_player_config cfg = {
        .rtsp_url = "rtsp://admin:hk123456@192.168.7.4:554/Streaming/Channels/102",
        .width = 1024,
        .height = 600,
        .channel = 1,
        .rate = 8000,
        .mode = VIDEO_PLAYER_MODE_VDEC | VIDEO_PLAYER_MODE_ADEC | VIDEO_PLAYER_MODE_DISP | VIDEO_PLAYER_MODE_SOUND,
    };
    cfg.x = 0, cfg.y = 0;
    rtsp_context[0] = db_rtsp_player_start(&cfg);
    // cfg.x = 512, cfg.y = 0;
    // rtsp_context[1] = db_rtsp_player_start(&cfg);
    // cfg.x = 0, cfg.y = 300;
    // rtsp_context[2] = db_rtsp_player_start(&cfg);
    // cfg.x = 512, cfg.y = 300;
    // rtsp_context[3] = db_rtsp_player_start(&cfg);

    // camera_play_img = lv_img_create(lv_scr_act());
    // lv_img_set_src(camera_play_img, NULL);
    // lv_obj_center(camera_play_img);

    // if (camera_img_dsc.data == NULL)
    // {
    //     camera_img_dsc.data = (uint8_t *)malloc(MY_DISP_HOR_RES * MY_DISP_VER_RES * 4);
    // }

    // int width = 0;
    // int height = 0;
    // int bufsize = 0;
    // video_input_frame_get(&width, &height, &bufsize);
    // camera_img_dsc.header.w = width;
    // camera_img_dsc.header.h = height;
    // camera_img_dsc.data_size = bufsize;

    // video_input_start(video_frame_input_cb_func);

    // lv_layout_timer_create(monitor_switch_timer, 30, NULL);

    // layout_common_back_btn_create(lv_scr_act(), 30, 0, LV_EVENT_CLICKED, monitor_bg_btn_click, NULL);
}

LAYOUT_QUIT_FUNC(monitor)
{
    db_rtsp_player_stop(rtsp_context[0]);
    db_rtsp_player_stop(rtsp_context[1]);
    db_rtsp_player_stop(rtsp_context[2]);
    db_rtsp_player_stop(rtsp_context[3]);

    // video_input_stop();
}

LAYOUT_DEFINE(monitor)