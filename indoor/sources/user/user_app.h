#ifndef _USER_APP_H_
#define _USER_APP_H_

#include <stdbool.h>

// #include "dyc_common.h"
// #include "dyc_time.h"
// #include "dyc_onvif.h"
// #include "dyc_wifi_api.h"
// #include "dyc_tfcard.h"
// #include "dyc_video_record.h"
// #include "dyc_video_player.h"
// #include "user_data.h"
// #include "dycio_ev.h"
// #include "tuya.h"
// #include "standby.h"
// #include "ring.h"
// #include "asterisk.h"
// #include "call.h"
// #include "monitor.h"
// #include "intercom.h"
// #include "language.h"
// #include "lang_xls.h"
// #include "network.h"
// #include "record.h"
// #include "gpio.h"
// #include "doorcamera.h"
// #include "baresip_call_info.h"
// #include "tuya_sdk.h"
// #include "tuya_xls.h"

#ifndef LV_RESOURCE_PATH
#define LV_RESOURCE_PATH "/app/resource"
#endif

#ifndef TUYA_CACHE_PATH
#define TUYA_CACHE_PATH "/tmp/tuya_cache/"
#endif

#ifndef SYSTEM_VERSION
#define SYSTEM_VERSION "1.0.0"
#endif

#define lv_ui_res_path_get(name) "A:" LV_RESOURCE_PATH "/ui/" name
#define lv_wp_res_path_get(name) "A:" LV_RESOURCE_PATH "/wallpaper/" name

#define APP_TUYA_PID "egyq7uvexpiim6pd"

int user_app_main(int argc, char **argv);

// int app_compile_time_get(struct tm *tm);

void user_app_system_exit(bool reboot);

void user_app_tuya_cache_clean(void);

// bool app_fast_boot_flag_get(void);

// void app_obj_pressed_default_callback(lv_event_t *e);

// // 获取预览的数据库句柄
// void *playback_media_context_get(void);

#endif // _USER_APP_H_
