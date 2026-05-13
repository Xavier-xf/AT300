/**
 * @file lv_demo_music.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_music.h"
#include "gui_display/driver_gui_display.h"
#include "db_adc_ctrl.h"
#include "db_key_state.h"
#if LV_USE_DEMO_MUSIC

#include "lv_demo_music_main.h"
#include "lv_demo_music_list.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_DEMO_MUSIC_AUTO_PLAY
static void auto_step_cb(lv_timer_t *timer);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t *ctrl;
static lv_obj_t *list;

static const char *title_list[] = {
    "Waiting for true love",
    "Need a Better Future",
    "Vibrations",
    "Why now?",
    "Never Look Back",
    "It happened Yesterday",
    "Feeling so High",
    "Go Deeper",
    "Find You There",
    "Until the End",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
};

static const char *artist_list[] = {
    "The John Smith Band",
    "My True Name",
    "Robotics",
    "John Smith",
    "My True Name",
    "Robotics",
    "Robotics",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
    "Unknown artist",
};

static const char *genre_list[] = {
    "Rock - 1997",
    "Drum'n bass - 2016",
    "Psy trance - 2020",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
    "Metal - 2015",
};

static const uint32_t time_list[] = {
    1 * 60 + 14,
    2 * 60 + 26,
    1 * 60 + 54,
    2 * 60 + 24,
    2 * 60 + 37,
    3 * 60 + 33,
    1 * 60 + 56,
    3 * 60 + 31,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
    2 * 60 + 20,
    2 * 60 + 19,
};

#if LV_DEMO_MUSIC_AUTO_PLAY
static lv_timer_t *auto_step_timer;
#endif

static lv_color_t original_screen_bg_color;
static lv_timer_t *standby_timer_handle = NULL;
static lv_timer_t *adc_scan_timer_handle = NULL;
static lv_timer_t *standby_display_timer_handle = NULL;
static lv_obj_t *standby_background = NULL;
static uint32_t standby_display_index = 0;
static void *adc_key_state_ctx = NULL;
static bool manual_mode = false;
static bool music_demo_active = false;
static bool adc_key_initialized = false;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void obj_pressed_callback(lv_event_t *e)
{
    if (music_demo_active && standby_timer_handle)
    {
        lv_timer_reset(standby_timer_handle);
    }
}

static void standby_background_click(lv_event_t *e)
{
    if (!music_demo_active)
    {
        return;
    }

    printf("自动模式\n");
    manual_mode = false;
    if (standby_background)
    {
        lv_obj_del(standby_background);
        standby_background = NULL;
    }
    if (standby_display_timer_handle)
    {
        lv_timer_del(standby_display_timer_handle);
        standby_display_timer_handle = NULL;
    }
    if (standby_timer_handle)
    {
        lv_timer_resume(standby_timer_handle);
    }
}

static void standby_display_timer(lv_timer_t *t)
{
    if (!music_demo_active || !standby_background)
    {
        return;
    }

    const uint32_t gray_levels[] = {0xFFFFFF, 0xE0E0E0, 0xC0C0C0, 0xA0A0A0, 0x808080, 0x606060, 0x404040, 0x202020, 0x000000};
    const uint32_t color_levels[] = {0xFFFFFF, 0x000000, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0x00FFFF, 0xFF00FF};
    int block = 0;
    printf("standby_display_index:%d\n", standby_display_index);
    switch (standby_display_index % 20)
    {
    case 0: // 白
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_TRANSP, 0);
        break;
    case 1: // 黑
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 2: // 红
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFF0000), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 3: // 绿
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0x00FF00), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 4: // 蓝
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0x0000FF), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 5: // 横向灰阶
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        block = MY_DISP_HOR_RES / (sizeof(gray_levels) / sizeof(gray_levels[0]));
        for (int i = 0; i < sizeof(gray_levels) / sizeof(gray_levels[0]); i++)
        {
            lv_obj_t *obj = lv_obj_create(standby_background);
            lv_obj_set_pos(obj, block * i, 0);
            if (i == (sizeof(gray_levels) / sizeof(gray_levels[0]) - 1))
            {
                lv_obj_set_size(obj, MY_DISP_HOR_RES - (block * (sizeof(gray_levels) / sizeof(gray_levels[0]) - 1)), MY_DISP_VER_RES);
            }
            else
            {
                lv_obj_set_size(obj, block, MY_DISP_VER_RES);
            }
            lv_obj_set_style_bg_color(obj, lv_color_hex(gray_levels[i]), 0);
            lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
            lv_obj_set_style_radius(obj, 0, 0);
            lv_obj_set_style_border_width(obj, 0, 0);
            lv_obj_set_style_pad_all(obj, 0, 0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        break;
    case 6: // 竖向灰阶
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        block = MY_DISP_VER_RES / (sizeof(gray_levels) / sizeof(gray_levels[0]));
        for (int i = 0; i < sizeof(gray_levels) / sizeof(gray_levels[0]); i++)
        {
            lv_obj_t *obj = lv_obj_create(standby_background);
            lv_obj_set_pos(obj, 0, block * i);
            if (i == (sizeof(gray_levels) / sizeof(gray_levels[0]) - 1))
            {
                lv_obj_set_size(obj, MY_DISP_HOR_RES, MY_DISP_VER_RES - (block * (sizeof(gray_levels) / sizeof(gray_levels[0]) - 1)));
            }
            else
            {
                lv_obj_set_size(obj, MY_DISP_HOR_RES, block);
            }
            lv_obj_set_style_bg_color(obj, lv_color_hex(gray_levels[i]), 0);
            lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
            lv_obj_set_style_radius(obj, 0, 0);
            lv_obj_set_style_border_width(obj, 0, 0);
            lv_obj_set_style_pad_all(obj, 0, 0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        break;
    case 7: // 横向色块
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        block = MY_DISP_HOR_RES / (sizeof(color_levels) / sizeof(color_levels[0]));
        for (int i = 0; i < sizeof(color_levels) / sizeof(color_levels[0]); i++)
        {
            lv_obj_t *obj = lv_obj_create(standby_background);
            lv_obj_set_pos(obj, block * i, 0);
            if (i == (sizeof(color_levels) / sizeof(color_levels[0]) - 1))
            {
                lv_obj_set_size(obj, MY_DISP_HOR_RES - (block * (sizeof(color_levels) / sizeof(color_levels[0]) - 1)), MY_DISP_VER_RES);
            }
            else
            {
                lv_obj_set_size(obj, block, MY_DISP_VER_RES);
            }
            lv_obj_set_style_bg_color(obj, lv_color_hex(color_levels[i]), 0);
            lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
            lv_obj_set_style_radius(obj, 0, 0);
            lv_obj_set_style_border_width(obj, 0, 0);
            lv_obj_set_style_pad_all(obj, 0, 0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        break;
    case 8: // 竖向色块
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        block = MY_DISP_VER_RES / (sizeof(color_levels) / sizeof(color_levels[0]));
        for (int i = 0; i < sizeof(color_levels) / sizeof(color_levels[0]); i++)
        {
            lv_obj_t *obj = lv_obj_create(standby_background);
            lv_obj_set_pos(obj, 0, block * i);
            if (i == (sizeof(color_levels) / sizeof(color_levels[0]) - 1))
            {
                lv_obj_set_size(obj, MY_DISP_HOR_RES, MY_DISP_VER_RES - (block * (sizeof(color_levels) / sizeof(color_levels[0]) - 1)));
            }
            else
            {
                lv_obj_set_size(obj, MY_DISP_HOR_RES, block);
            }
            lv_obj_set_style_bg_color(obj, lv_color_hex(color_levels[i]), 0);
            lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
            lv_obj_set_style_radius(obj, 0, 0);
            lv_obj_set_style_border_width(obj, 0, 0);
            lv_obj_set_style_pad_all(obj, 0, 0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        }
        break;
    case 9: // 黑白交错色块
        lv_obj_clean(standby_background);
        lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
        int block_width = MY_DISP_HOR_RES / 8;
        int block_hight = MY_DISP_VER_RES / 8;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                lv_obj_t *obj = lv_obj_create(standby_background);
                lv_obj_set_pos(obj, block_width * j, block_hight * i);
                lv_obj_set_size(obj, block_width, block_hight);
                if (i % 2 == 0)
                {
                    if (j % 2 == 0)
                    {
                        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), 0);
                    }
                    else
                    {
                        lv_obj_set_style_bg_color(obj, lv_color_hex(0x000000), 0);
                    }
                }
                else
                {
                    if (j % 2 == 0)
                    {
                        lv_obj_set_style_bg_color(obj, lv_color_hex(0x000000), 0);
                    }
                    else
                    {
                        lv_obj_set_style_bg_color(obj, lv_color_hex(0xFFFFFF), 0);
                    }
                }
                lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
                lv_obj_set_style_radius(obj, 0, 0);
                lv_obj_set_style_border_width(obj, 0, 0);
                lv_obj_set_style_pad_all(obj, 0, 0);
                lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            }
        }
        break;
    case 10: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image0.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 11: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image1.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 12: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image2.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 13: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image3.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 14: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image4.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 15: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image6.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 16: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image7.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 17: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image8.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    case 18: // 图片
        lv_obj_clean(standby_background);
        lv_img_cache_invalidate_src(NULL);
        lv_obj_set_style_bg_img_src(standby_background, "A:/app/data/photo/image9.jpg", 0);
        lv_obj_set_style_bg_img_opa(standby_background, LV_OPA_COVER, 0);
        break;
    default:
        break;
    }
    standby_display_index++;
}

static void standby_timer(lv_timer_t *t)
{
    if (!music_demo_active)
    {
        return;
    }

    if (standby_timer_handle)
    {
        lv_timer_pause(standby_timer_handle);
    }
    standby_display_index = 0;
    if (standby_background)
    {
        lv_obj_del(standby_background);
        standby_background = NULL;
    }
    standby_background = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(standby_background, 0, 0);
    lv_obj_set_size(standby_background, MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(standby_background, 0, 0);
    lv_obj_set_style_border_width(standby_background, 0, 0);
    lv_obj_set_style_pad_all(standby_background, 0, 0);
    lv_obj_add_event_cb(standby_background, standby_background_click, LV_EVENT_CLICKED, NULL);
    standby_display_timer_handle = lv_timer_create(standby_display_timer, 2000, NULL);
    lv_timer_ready(standby_display_timer_handle);
}

static void adc_scan_timer_timer(lv_timer_t *t)
{
    if (music_demo_active && adc_key_state_ctx)
    {
        db_key_state_process(adc_key_state_ctx);
    }
}

static bool adc_key_pressed(int *key_value)
{
    const int key_threshold[][2] =
        {
            {3015, 2993},
            {1071, 1054},
            {306, 291},
            {1663, 1646},
        };
    int adc_value = db_adc_value_read();
    for (int i = 0; i < sizeof(key_threshold) / sizeof(key_threshold[0]); i++)
    {
        if ((key_threshold[i][0] + 50) > adc_value && adc_value > (key_threshold[i][1] - 50))
        {
            *key_value = i + 1;
            return true;
        }
    }

    return false;
}

static void adc_key_event_cb(db_key_event_t event, int key_value)
{
    if (!music_demo_active)
    {
        return;
    }

    if (event == KEY_EVENT_CLICK)
    {
        switch (key_value)
        {
        case 1:
            if (manual_mode == true)
                standby_display_timer(NULL);
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            if (manual_mode == false)
            {
                printf("手动模式\n");

                if (standby_display_timer_handle)
                {
                    if (standby_background)
                    {
                        lv_obj_del(standby_background);
                        standby_background = NULL;
                    }
                    lv_timer_del(standby_display_timer_handle);
                    standby_display_timer_handle = NULL;
                }

                manual_mode = true;
                if (standby_timer_handle)
                {
                    lv_timer_pause(standby_timer_handle);
                }
                standby_display_index = 0;
                standby_background = lv_obj_create(lv_scr_act());
                lv_obj_set_pos(standby_background, 0, 0);
                lv_obj_set_size(standby_background, MY_DISP_HOR_RES, MY_DISP_VER_RES);
                lv_obj_set_style_bg_color(standby_background, lv_color_hex(0xFFFFFF), 0);
                lv_obj_set_style_bg_opa(standby_background, LV_OPA_COVER, 0);
                lv_obj_set_style_radius(standby_background, 0, 0);
                lv_obj_set_style_border_width(standby_background, 0, 0);
                lv_obj_set_style_pad_all(standby_background, 0, 0);
                lv_obj_add_event_cb(standby_background, standby_background_click, LV_EVENT_CLICKED, NULL);
            }
            else
            {
                printf("自动模式\n");
                manual_mode = false;
                if (standby_background)
                {
                    lv_obj_del(standby_background);
                    standby_background = NULL;
                }
                if (standby_display_timer_handle)
                {
                    lv_timer_del(standby_display_timer_handle);
                    standby_display_timer_handle = NULL;
                }
                if (standby_timer_handle)
                {
                    lv_timer_resume(standby_timer_handle);
                }
            }
            break;
        default:
            break;
        }
    }
}

void lv_demo_music(void)
{
    music_demo_active = true;
    lv_event_pressed_callback_register(obj_pressed_callback);

    if (!adc_key_initialized)
    {
        printf("insmod /usr/modules/ak_saradc.ko\n");
        system("insmod /usr/modules/ak_saradc.ko 2>/dev/null");
        adc_key_state_ctx = db_key_state_open(1, 30, 60, adc_key_pressed, adc_key_event_cb);
        adc_key_initialized = adc_key_state_ctx != NULL;
    }

    if (!standby_timer_handle)
    {
        standby_timer_handle = lv_timer_create(standby_timer, 30000, NULL);
    }
    else
    {
        lv_timer_reset(standby_timer_handle);
        lv_timer_resume(standby_timer_handle);
    }

    if (!adc_scan_timer_handle)
    {
        adc_scan_timer_handle = lv_timer_create(adc_scan_timer_timer, 30, NULL);
    }
    else
    {
        lv_timer_resume(adc_scan_timer_handle);
    }

    original_screen_bg_color = lv_obj_get_style_bg_color(lv_scr_act(), 0);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x343247), 0);

    list = _lv_demo_music_list_create(lv_scr_act());
    ctrl = _lv_demo_music_main_create(lv_scr_act());

#if LV_DEMO_MUSIC_AUTO_PLAY
    auto_step_timer = lv_timer_create(auto_step_cb, 1000, NULL);
#endif
}

void lv_demo_music_close(void)
{
    music_demo_active = false;
    manual_mode = false;

    if (standby_display_timer_handle)
    {
        lv_timer_del(standby_display_timer_handle);
        standby_display_timer_handle = NULL;
    }

    if (standby_background)
    {
        lv_obj_del(standby_background);
        standby_background = NULL;
    }

    if (standby_timer_handle)
    {
        lv_timer_pause(standby_timer_handle);
    }

    if (adc_scan_timer_handle)
    {
        lv_timer_pause(adc_scan_timer_handle);
    }

    /*Delete all aniamtions*/
    lv_anim_del(NULL, NULL);

#if LV_DEMO_MUSIC_AUTO_PLAY
    lv_timer_del(auto_step_timer);
#endif
    _lv_demo_music_list_close();
    _lv_demo_music_main_close();

    lv_obj_clean(lv_scr_act());

    lv_obj_set_style_bg_color(lv_scr_act(), original_screen_bg_color, 0);
}

const char *_lv_demo_music_get_title(uint32_t track_id)
{
    if (track_id >= sizeof(title_list) / sizeof(title_list[0]))
        return NULL;
    return title_list[track_id];
}

const char *_lv_demo_music_get_artist(uint32_t track_id)
{
    if (track_id >= sizeof(artist_list) / sizeof(artist_list[0]))
        return NULL;
    return artist_list[track_id];
}

const char *_lv_demo_music_get_genre(uint32_t track_id)
{
    if (track_id >= sizeof(genre_list) / sizeof(genre_list[0]))
        return NULL;
    return genre_list[track_id];
}

uint32_t _lv_demo_music_get_track_length(uint32_t track_id)
{
    if (track_id >= sizeof(time_list) / sizeof(time_list[0]))
        return 0;
    return time_list[track_id];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_DEMO_MUSIC_AUTO_PLAY
static void auto_step_cb(lv_timer_t *t)
{
    LV_UNUSED(t);
    static uint32_t state = 0;

#if LV_DEMO_MUSIC_LARGE
    const lv_font_t *font_small = &lv_font_montserrat_22;
    const lv_font_t *font_large = &lv_font_montserrat_32;
#else
    const lv_font_t *font_small = &lv_font_montserrat_12;
    const lv_font_t *font_large = &lv_font_montserrat_16;
#endif

    switch (state)
    {
    case 5:
        _lv_demo_music_album_next(true);
        break;

    case 6:
        _lv_demo_music_album_next(true);
        break;
    case 7:
        _lv_demo_music_album_next(true);
        break;
    case 8:
        _lv_demo_music_play(0);
        break;
#if LV_DEMO_MUSIC_SQUARE || LV_DEMO_MUSIC_ROUND
    case 11:
        lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
        break;
    case 13:
        lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
        break;
#else
    case 12:
        lv_obj_scroll_by(ctrl, 0, -LV_VER_RES, LV_ANIM_ON);
        break;
#endif
    case 15:
        lv_obj_scroll_by(list, 0, -300, LV_ANIM_ON);
        break;
    case 16:
        lv_obj_scroll_by(list, 0, 300, LV_ANIM_ON);
        break;
    case 18:
        _lv_demo_music_play(1);
        break;
    case 19:
        lv_obj_scroll_by(ctrl, 0, LV_VER_RES, LV_ANIM_ON);
        break;
#if LV_DEMO_MUSIC_SQUARE || LV_DEMO_MUSIC_ROUND
    case 20:
        lv_obj_scroll_by(ctrl, 0, LV_VER_RES, LV_ANIM_ON);
        break;
#endif
    case 30:
        _lv_demo_music_play(2);
        break;
    case 40:
    {
        lv_obj_t *bg = lv_layer_top();
        lv_obj_set_style_bg_color(bg, lv_color_hex(0x6f8af6), 0);
        lv_obj_set_style_text_color(bg, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(bg, LV_OPA_COVER, 0);
        lv_obj_fade_in(bg, 400, 0);
        lv_obj_t *dsc = lv_label_create(bg);
        lv_obj_set_style_text_font(dsc, font_small, 0);
        lv_label_set_text(dsc, "The average FPS is");
        lv_obj_align(dsc, LV_ALIGN_TOP_MID, 0, 90);

        lv_obj_t *num = lv_label_create(bg);
        lv_obj_set_style_text_font(num, font_large, 0);
#if LV_USE_PERF_MONITOR
        lv_label_set_text_fmt(num, "%d", lv_refr_get_fps_avg());
#endif
        lv_obj_align(num, LV_ALIGN_TOP_MID, 0, 120);

        lv_obj_t *attr = lv_label_create(bg);
        lv_obj_set_style_text_align(attr, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(attr, font_small, 0);
#if LV_DEMO_MUSIC_SQUARE || LV_DEMO_MUSIC_ROUND
        lv_label_set_text(attr, "Copyright 2020 LVGL Kft.\nwww.lvgl.io | lvgl@lvgl.io");
#else
        lv_label_set_text(attr, "Copyright 2020 LVGL Kft. | www.lvgl.io | lvgl@lvgl.io");
#endif
        lv_obj_align(attr, LV_ALIGN_BOTTOM_MID, 0, -10);
        break;
    }
    case 41:
        lv_scr_load(lv_obj_create(NULL));
        _lv_demo_music_pause();
        break;
    }
    state++;
}

#endif /*LV_DEMO_MUSIC_AUTO_PLAY*/

#endif /*LV_USE_DEMO_MUSIC*/
