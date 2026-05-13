#include "layout_common.h"
#include "tuya_sdk.h"

static void logo_system_init_timer(lv_timer_t *t)
{
    // user_msg_event_init();
    // audio_output_open(SND_PCM_FORMAT_S16_LE, 1, 44100);
    // audio_inpit_init();
    // ffmpeg_decoder_init();
    // user_file_manager_init();
    // jpg_decoder_init();
    // video_input_init();
    // jpg_decode_start("/home/wallpaper/1.jpg", JPG_OUT_MODE_AUTO, NULL, NULL);
    // printf("=======================>>> [%s:%d]\n", __func__, __LINE__);

    // static bool first_init = true;
    // if (first_init == false)
    // {
    //     // return -1;
    // }
    // first_init = false;
//     char uuid[64] = "uuid90ba1cfdff83c8ae";
//     char key[64] = "lm5A0fCanAA5QgoSViRutQN4tNYASqRi";
//     // if (tuya_uuid_and_key_read((unsigned char *)uuid, (unsigned char *)key) == true)
//     {
//         tuya_init_config_t tuya_cfg;
//         memset(&tuya_cfg, 0, sizeof(tuya_cfg));
//         strncpy(tuya_cfg.pid, APP_TUYA_PID, sizeof(tuya_cfg.pid));
//         strncpy(tuya_cfg.uuid, uuid, sizeof(tuya_cfg.uuid));
//         strncpy(tuya_cfg.key, key, sizeof(tuya_cfg.key));
//         strncpy(tuya_cfg.ver, /* SYSTEM_VERSION */"2.1.12", sizeof(tuya_cfg.ver));
// #ifdef PLATFORM_TYPE_X86_64
//         strncpy(tuya_cfg.net_dev, "ens33", sizeof(tuya_cfg.net_dev));
//         strncpy(tuya_cfg.cache_dir, TUYA_CACHE_PATH, sizeof(tuya_cfg.cache_dir));
//         strncpy(tuya_cfg.sd_dir, "/tmp/tf/", sizeof(tuya_cfg.sd_dir));
// #elif PLATFORM_TYPE_SSD20X
//         strncpy(tuya_cfg.net_dev, "wlan0", sizeof(tuya_cfg.net_dev));
//         strncpy(tuya_cfg.cache_dir, TUYA_CACHE_PATH, sizeof(tuya_cfg.cache_dir));
//         strncpy(tuya_cfg.sd_dir, "/tmp/tf/", sizeof(tuya_cfg.sd_dir));
// #endif
//         tuya_sdk_init(&tuya_cfg);
//     }
    LAYOUT_GOTO(home, );
}

LAYOUT_ENTER_FUNC(logo)
{
    layout_common_background_display(0x00, LV_OPA_COVER, lv_ui_res_path_get("anyka_logo.png"), NULL, false);
    lv_timer_recycle_create(logo_system_init_timer, 500, NULL);
}

LAYOUT_QUIT_FUNC(logo)
{
}

LAYOUT_DEFINE(logo)