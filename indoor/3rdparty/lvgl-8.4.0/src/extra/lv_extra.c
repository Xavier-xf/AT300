/**
 * @file lv_extra.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void *lv_font_base_create(int weight);
/**********************
 *  STATIC VARIABLES
 **********************/
void *lv_font_small = NULL;
void *lv_font_smaller = NULL;
void *lv_font_normal = NULL;
void *lv_font_large = NULL;
void *lv_font_larger = NULL;
void *lv_font_large_s = NULL;
void *lv_font_large_plus = NULL;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_extra_init(void)
{
#if LV_USE_FLEX
    lv_flex_init();
#endif

#if LV_USE_GRID
    lv_grid_init();
#endif

#if LV_USE_MSG
    lv_msg_init();
#endif

#if LV_USE_FS_FATFS != '\0'
    lv_fs_fatfs_init();
#endif

#if LV_USE_FS_LITTLEFS != '\0'
    lv_fs_littlefs_init();
#endif

#if LV_USE_FS_STDIO != '\0'
    lv_fs_stdio_init();
#endif

#if LV_USE_FS_POSIX != '\0'
    lv_fs_posix_init();
#endif

#if LV_USE_FS_WIN32 != '\0'
    lv_fs_win32_init();
#endif

#if LV_USE_FFMPEG
    lv_ffmpeg_init();
#endif

#if LV_USE_PNG
    lv_png_init();
#endif

#if LV_USE_SJPG
    lv_split_jpeg_init();
#endif

#if LV_USE_BMP
    lv_bmp_init();
#endif

#if LV_USE_FREETYPE
    /*Init freetype library*/
#  if LV_FREETYPE_CACHE_SIZE >= 0
    lv_freetype_init(LV_FREETYPE_CACHE_FT_FACES, LV_FREETYPE_CACHE_FT_SIZES, LV_FREETYPE_CACHE_SIZE);
#  else
    lv_freetype_init(0, 0, 0);
#  endif
    lv_font_small = lv_font_base_create(16);
    lv_font_smaller = lv_font_base_create(18);
    lv_font_normal = lv_font_base_create(22);
    lv_font_large = lv_font_base_create(27);
    lv_font_larger = lv_font_base_create(35);
    lv_font_large_s = lv_font_base_create(40);
    lv_font_large_plus = lv_font_base_create(90);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#if LV_USE_FREETYPE
static void* lv_font_base_create(int weight)
{
    static lv_ft_info_t small_font_info;
    small_font_info.name = LV_RESOURCE_PATH "/font/my_font.ttf";
    small_font_info.weight = weight;
    small_font_info.style = FT_FONT_STYLE_NORMAL;
    small_font_info.mem = NULL;
    if (!lv_ft_font_init(&small_font_info)) {
        LV_LOG_ERROR("create failed.:%s", small_font_info.name);
    }
    return small_font_info.font;
}
#endif