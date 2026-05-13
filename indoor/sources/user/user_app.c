#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "db_time.h"
#include "db_gpio_ctrl.h"
#include "user_app.h"
#include "user_layout.h"
#include "db_common.h"

#define POWER_LED_GPIO 0

typedef enum
{
    DEMO_MUSIC,
    DEMO_WIDGETS,
    DEMO_COUNT,
} demo_id_t;

static demo_id_t current_demo = DEMO_MUSIC;

// static bool user_app_fast_boot = false;

#if 0
static void video_refresh_timer(lv_timer_t* t) {
    void* gui = lv_video_output_device_get();
    if (!gui) {
        return;
    }

    dyc_video_output_ioctl(gui, GUI_REFRESH_DISPLAY_CMD, NULL, NULL);
}
static void lv_demo_test(void) {
    lv_disp_set_bg_opa(NULL, LV_OPA_TRANSP);
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_opa(obj, LV_OPA_50, LV_PART_MAIN);

    lv_timer_create(video_refresh_timer, 30, NULL);
}
#endif
/* 获取应用层编译时间 */
int user_app_compile_time_get(struct tm *tm)
{
#define MONTHS_PER_YEAR 12         // 一年12月
#define DATE_STRING_BUFFER_SIZE 20 // 年月日缓存大小
#define TIME_STRING_BUFFER_SIZE 20 // 时分秒缓存大小

    const char year_month[MONTHS_PER_YEAR][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char compile_date[DATE_STRING_BUFFER_SIZE] = {0};
    char compile_time[TIME_STRING_BUFFER_SIZE] = {0};
    char str_month[4] = {0};

    sprintf(compile_date, "%s", __DATE__); // "Aug 23 2016"
    sprintf(compile_time, "%s", __TIME__); // "10:59:19"

    sscanf(compile_date, "%s %d %d", str_month, &(tm->tm_mday), &(tm->tm_year));
    sscanf(compile_time, "%d:%d:%d", &(tm->tm_hour), &(tm->tm_min), &(tm->tm_sec));

    for (int i = 0; i < MONTHS_PER_YEAR; ++i)
    {
        if (strncmp(str_month, year_month[i], 3) == 0)
        {
            tm->tm_mon = i + 1;
            return 0;
        }
    }
    return -1;
}

void user_app_system_exit(bool reboot)
{
    // backlight_enable(false);
    // if (reboot)
    // {
    //     system("reboot");
    // }
    // else
    // {
    //     while (1)
    //     {
    //         system("killall SAT_SSD20X.BIN");
    //         dyc_usleep(500 * 1000);
    //     }
    // }
}

void user_app_tuya_cache_clean(void)
{
    // system("rm -rf " TUYA_CACHE_PATH "*");
    // system("sync");
}

// bool user_app_fast_boot_get(void)
// {
//     return user_app_fast_boot;
// }

// void app_obj_pressed_default_callback(lv_event_t *e)
// {
//     ring_touch_tone_play();
//     standby_timer_restart(false);
// }

// static void lv_goto_logo(void)
// {
//     dyc_page_goto(logo, LV_SCR_LOAD_ANIM_NONE, );
// }

static void *lvgl_tick_task(void *arg)
{
    unsigned long long timestamp1, timestamp2;
    timestamp1 = db_sys_clock_ms_get();
    while (1)
    {
        usleep(1000);
        timestamp2 = db_sys_clock_ms_get();
        lv_tick_inc(timestamp2 - timestamp1 - lv_tick_get());
    }
    return NULL;
}

static void power_led_init(void)
{
    if (db_gpio_open(POWER_LED_GPIO, GPIO_DIR_OUT, false) == 0)
    {
        db_gpio_level_set(POWER_LED_GPIO, GPIO_LEVEL_HIGH);
    }
}

static void demo_close_current(void)
{
    switch (current_demo)
    {
    case DEMO_MUSIC:
        lv_demo_music_close();
        break;
    case DEMO_WIDGETS:
        lv_demo_widgets_close();
        break;
    default:
        break;
    }
}

static void demo_switch_arrows_create(void);

static void demo_load(demo_id_t demo)
{
    demo_close_current();
    lv_obj_clean(lv_scr_act());
    lv_obj_clean(lv_layer_top());
    lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);
    current_demo = demo;

    switch (demo)
    {
    case DEMO_MUSIC:
        lv_demo_music();
        break;
    case DEMO_WIDGETS:
        lv_demo_widgets();
        break;
    default:
        lv_demo_music();
        current_demo = DEMO_MUSIC;
        break;
    }

    demo_switch_arrows_create();
}

static void demo_switch_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED)
    {
        int step = (int)(intptr_t)lv_event_get_user_data(e);
        int next_demo = (int)current_demo + step;
        if (next_demo < 0)
        {
            next_demo = DEMO_COUNT - 1;
        }
        else if (next_demo >= DEMO_COUNT)
        {
            next_demo = 0;
        }

        demo_load((demo_id_t)next_demo);
    }
}

static void demo_switch_arrow_create(lv_align_t align, const char *text, int step)
{
    lv_obj_t *btn = lv_btn_create(lv_layer_top());
    lv_obj_set_size(btn, 52, 118);
    lv_obj_set_style_radius(btn, 26, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x101722), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_40, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_align(btn, align, align == LV_ALIGN_LEFT_MID ? 8 : -8, 0);
    lv_obj_add_event_cb(btn, demo_switch_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)step);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    lv_obj_center(label);
}

static void demo_switch_arrows_create(void)
{
    demo_switch_arrow_create(LV_ALIGN_LEFT_MID, "<", -1);
    demo_switch_arrow_create(LV_ALIGN_RIGHT_MID, ">", 1);
}

int user_app_main(int argc, char **argv)
{
    power_led_init();

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    // _layout_goto(pLAYOUT(logo));
    current_demo = DEMO_MUSIC;
    lv_demo_music();
    demo_switch_arrows_create();

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, lvgl_tick_task, NULL);
    while (1)
    {
        uint32_t delay = lv_task_handler();
        if (delay)
            usleep(delay * 1000);
    }
    return 0;
}
