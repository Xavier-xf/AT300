#include "lvgl.h"
#include "user_layout.h"
#include "db_common.h"

static const layout_info *curr_layout = NULL;
static const layout_info *prev_layout = NULL;
static const layout_info *next_layout = NULL;

int _layout_goto(const layout_info *layout)
{
    if ((layout == NULL) || (layout->enter == NULL))
    {
        return -1;
    }

    lv_timer_recycle();
    lv_anim_del_all();
    lv_obj_clean(lv_scr_act());

    next_layout = layout;

    if ((curr_layout != NULL) && (curr_layout->quit != NULL))
    {
        db_log_debug("layout %s quit\n", curr_layout->name);
        curr_layout->quit();
    }

    prev_layout = curr_layout;
    curr_layout = layout;

    if (curr_layout->enter)
    {
        db_log_debug("layout %s enter\n", curr_layout->name);
        curr_layout->enter();
    }

    return 0;
}

const layout_info *curr_layout_get(void)
{
    return curr_layout;
}

const layout_info *prev_layout_get(void)
{
    return prev_layout;
}

const layout_info *next_layout_get(void)
{
    return next_layout;
}
