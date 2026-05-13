#include "lvgl.h"
#include "layout_common.h"

/*******************************************************************
 * @brief  : 创建一个基础对象
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_obj_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align, uint32_t bg_color, lv_opa_t bg_opa,
                                   int radius, uint32_t boarder_color, lv_opa_t boarder_opa, int boarder_width, lv_border_side_t boarder_side)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_obj_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_obj_set_bg_color(obj, bg_color, bg_opa, 0);
    layout_common_style_set_border(obj, radius, boarder_color, boarder_opa, boarder_width, boarder_side, 0);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个文本对象
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_text_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                    const char *txt, uint32_t color, lv_opa_t opa, lv_text_align_t txt_align, lv_font_t *font)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_label_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    layout_common_style_set_text(obj, txt, color, opa, txt_align, font, 0);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    // layout_common_obj_set_bg_color(obj, 0x00ff00, LV_OPA_COVER, 0);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个图标对象
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_img_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                   const char *img, lv_align_t img_align)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_obj_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_style_set_bg_img(obj, img, 0x0, LV_OPA_TRANSP, img_align);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个带背景容器的文本
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_text_cont_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align, uint32_t bg_color, lv_opa_t bg_opa,
                                         int radius, uint32_t boarder_color, lv_opa_t boarder_opa, int boarder_width, lv_border_side_t boarder_side,
                                         const char *txt, uint32_t txt_color, lv_opa_t txt_opa, lv_text_align_t txt_align, lv_font_t *font)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_obj_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_obj_set_bg_color(obj, bg_color, bg_opa, 0);
    layout_common_style_set_border(obj, radius, boarder_color, boarder_opa, boarder_width, boarder_side, 0);
    lv_obj_t *label = lv_label_create(obj);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    layout_common_style_set_text(label, txt, txt_color, txt_opa, txt_align, font, 0);
    layout_common_obj_set_base(label, 0, 0, 0, w, -1, obj, LV_ALIGN_CENTER);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_btn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                   lv_event_cb_t cb, uint32_t def_bg_color, lv_opa_t def_bg_opa, uint32_t pres_bg_color, lv_opa_t pres_bg_opa)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_obj_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_obj_set_bg_color(obj, def_bg_color, def_bg_opa, LV_STATE_DEFAULT);
    layout_common_obj_set_bg_color(obj, pres_bg_color, pres_bg_opa, LV_STATE_PRESSED);
    layout_common_obj_set_click_event(obj, cb, cb);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个图标按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_img_btn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                       lv_event_cb_t cb, uint32_t def_bg_color, lv_opa_t def_bg_opa, uint32_t pres_bg_color, lv_opa_t pres_bg_opa,
                                       const char *img, uint32_t recolor, lv_opa_t recolor_opa, lv_align_t img_align)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_obj_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_obj_set_bg_color(obj, def_bg_color, def_bg_opa, LV_STATE_DEFAULT);
    layout_common_obj_set_bg_color(obj, pres_bg_color, pres_bg_opa, LV_STATE_PRESSED);
    layout_common_style_set_bg_img(obj, img, recolor, recolor_opa, img_align);
    layout_common_obj_set_click_event(obj, cb, cb);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个文本按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_text_btn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                        lv_event_cb_t cb, uint32_t def_bg_color, lv_opa_t def_bg_opa, uint32_t pres_bg_color, lv_opa_t pres_bg_opa,
                                        const char *txt, uint32_t txt_color, lv_opa_t txt_opa, lv_text_align_t txt_align, lv_font_t *font)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_obj_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_obj_set_bg_color(obj, def_bg_color, def_bg_opa, LV_STATE_DEFAULT);
    layout_common_obj_set_bg_color(obj, pres_bg_color, pres_bg_opa, LV_STATE_PRESSED);
    layout_common_obj_set_click_event(obj, cb, cb);
    lv_obj_t *label = lv_label_create(obj);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    layout_common_style_set_text(label, txt, txt_color, txt_opa, txt_align, font, 0);
    layout_common_obj_set_base(label, 0, 0, 0, w, -1, obj, LV_ALIGN_CENTER);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个按下可切换图标的按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_imgbtn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                      lv_event_cb_t cb, const char *def_img, const char *pres_img)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_imgbtn_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_obj_set_click_event(obj, cb, cb);
    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, def_img, NULL);
    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, pres_img, NULL);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个图标文本按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_img_text_btn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                            lv_event_cb_t cb, uint32_t def_bg_color, lv_opa_t def_bg_opa, uint32_t pres_bg_color, lv_opa_t pres_bg_opa,
                                            int def_radius, uint32_t def_boarder_color, lv_opa_t def_boarder_opa, int def_boarder_width, lv_border_side_t def_boarder_side,
                                            int pres_radius, uint32_t pres_boarder_color, lv_opa_t pres_boarder_opa, int pres_boarder_width, lv_border_side_t pres_boarder_side,
                                            int img_x, int img_y, int img_w, int img_h, lv_align_t img_align, const char *img,
                                            int txt_x, int txt_y, int txt_w, int txt_h, lv_align_t txt_align, const char *txt,
                                            uint32_t txt_color, lv_opa_t txt_opa, lv_text_align_t text_align, lv_font_t *font)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj != NULL)
    {
        return obj;
    }
    {

        obj = layout_common_btn_create(parent, id, x, y, w, h, base, align,
                                       cb, def_bg_color, def_bg_opa, pres_bg_color, pres_bg_opa);
        layout_common_style_set_border(obj, def_radius, def_boarder_color, def_boarder_opa, def_boarder_width, def_boarder_side, LV_STATE_DEFAULT);
        layout_common_style_set_border(obj, pres_radius, pres_boarder_color, pres_boarder_opa, pres_boarder_width, pres_boarder_side, LV_STATE_PRESSED);
    }
    {
        layout_common_img_create(obj, 0, img_x, img_y, img_w, img_h, obj, img_align,
                                 img, LV_ALIGN_CENTER);
    }
    {
        lv_obj_t *label = layout_common_text_create(obj, 1, txt_x, txt_y, txt_w, txt_h, obj, txt_align,
                                                    txt, txt_color, txt_opa, text_align, font);
        lv_obj_clear_flag(label, LV_OBJ_FLAG_CLICKABLE);
    }
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个按下可切换图标的文本按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_imgbtn_text_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                           lv_event_cb_t cb, const char *def_img, const char *pres_img,
                                           const char *txt, uint32_t txt_color, lv_opa_t txt_opa, lv_text_align_t txt_align, lv_font_t *font)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj)
    {
        return obj;
    }
    obj = lv_imgbtn_create(parent);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_obj_set_click_event(obj, cb, cb);
    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, def_img, NULL);
    lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, pres_img, NULL);
    lv_obj_t *label = lv_label_create(obj);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLLABLE);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    layout_common_style_set_text(label, txt, txt_color, txt_opa, txt_align, font, 0);
    layout_common_obj_set_base(label, 0, 0, 0, w, -1, obj, LV_ALIGN_CENTER);
    return obj;
}
/*******************************************************************
 * @brief  : 创建一个通用键盘
 * @return  {*}
 *******************************************************************/
// lv_obj_t *layout_common_general_keyboard_create(lv_obj_t *parent, int id, lv_event_cb_t cb)
// {
//     lv_obj_t *keyboard = lv_obj_get_by_id(parent, id);
//     if (keyboard != NULL)
//     {
//         return keyboard;
//     }
//     keyboard = lv_keyboard_create(parent);
//     layout_common_obj_set_base(keyboard, id, 0, 170, 1024, 430, parent, LV_ALIGN_TOP_MID);
//     layout_common_obj_set_bg_color(keyboard, 0x272727, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_DEFAULT);
//     layout_common_obj_set_bg_color(keyboard, 0x3F57CF, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_PRESSED);
//     layout_common_style_set_border(keyboard, 0, 0x818080, LV_OPA_COVER, 1, LV_BORDER_SIDE_TOP, LV_STATE_DEFAULT);
//     layout_common_style_set_text(keyboard, NULL, 0xFFFFFF, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_large_s, LV_STATE_DEFAULT);
//     layout_common_style_set_pad(keyboard, 24, 18, 0, 18, 7, 7, LV_STATE_DEFAULT);
//     layout_common_obj_set_click_event(keyboard, cb, cb);
//     // lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_CLICK_FOCUSABLE);
//     static const char *btnm_img_map[3][45] = {{NULL}};
//     if (btnm_img_map[0][10] == NULL)
//     {
//         btnm_img_map[0][10] = app_ui_res_path_get("kb_delete.png");
//         btnm_img_map[1][10] = btnm_img_map[0][10];
//         btnm_img_map[2][10] = btnm_img_map[0][10];
//     }
//     if (btnm_img_map[0][41] == NULL)
//     {
//         btnm_img_map[0][41] = app_ui_res_path_get("kb_enter.png");
//         btnm_img_map[1][41] = btnm_img_map[0][41];
//         btnm_img_map[2][41] = btnm_img_map[0][41];
//     }
//     if (btnm_img_map[0][43] == NULL)
//     {
//         btnm_img_map[0][43] = app_ui_res_path_get("kb_space.png");
//         btnm_img_map[1][43] = btnm_img_map[0][43];
//         btnm_img_map[2][43] = btnm_img_map[0][43];
//     }
//     if (btnm_img_map[0][44] == NULL)
//     {
//         btnm_img_map[0][44] = app_ui_res_path_get("kb_capital.png");
//         btnm_img_map[1][44] = btnm_img_map[0][44];
//         btnm_img_map[2][44] = btnm_img_map[0][44];
//     }

//     lv_keyboard_update_img_map(keyboard, btnm_img_map);
//     return keyboard;
// }
/*******************************************************************
 * @brief  : 创建一个数字键盘
 * @return  {*}
 *******************************************************************/
// lv_obj_t *layout_common_number_keyboard_create(lv_obj_t *parent, int id, lv_event_cb_t cb)
// {
//     lv_obj_t *btnmatrix = lv_obj_get_by_id(parent, id);
//     if (btnmatrix != NULL)
//     {

//         return btnmatrix;
//     }
//     btnmatrix = lv_btnmatrix_create(parent);
//     layout_common_obj_set_base(btnmatrix, id, 0, 230, 300, 370, parent, LV_ALIGN_TOP_MID);
//     layout_common_style_set_text(btnmatrix, NULL, 0xFFFFFF, LV_OPA_COVER, LV_TEXT_ALIGN_CENTER, lv_font_large_s, LV_STATE_DEFAULT);
//     layout_common_obj_set_bg_color(btnmatrix, 0x272727, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_DEFAULT);
//     layout_common_obj_set_bg_color(btnmatrix, 0x3F57CF, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_PRESSED);
//     lv_obj_set_style_pad_row(btnmatrix, 10, LV_PART_MAIN);
//     lv_obj_set_style_pad_column(btnmatrix, 10, LV_PART_MAIN);
//     // lv_obj_clear_flag(btnmatrix, LV_OBJ_FLAG_CLICK_FOCUSABLE);
//     layout_common_obj_set_click_event(btnmatrix, cb, cb);

//     static const char *btnm_map[] = {
//         "1", "2", "3", "\n",
//         "4", "5", "6", "\n",
//         "7", "8", "9", "\n",
//         "OK", "0", " ", ""};

//     static const char *btnm_img_map[] = {
//         NULL, NULL, NULL,
//         NULL, NULL, NULL,
//         NULL, NULL, NULL,
//         NULL, NULL, app_ui_res_path_get("kb_delete.png")};

//     lv_btnmatrix_set_map(btnmatrix, btnm_map);
//     lv_btnmatrix_set_btn_bg_map(btnmatrix, (const void **)btnm_img_map);
//     return btnmatrix;
// }
/*******************************************************************
 * @brief  : 创建一个文本输入框
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_textarea_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
                                        const char *txt, lv_text_align_t txt_align, const lv_font_t *font, int txt_max, int txt_ofs_y,
                                        lv_event_cb_t cb, bool pwd_mode)
{
    lv_obj_t *obj = lv_obj_get_by_id(parent, id);
    if (obj != NULL)
    {

        return obj;
    }
    obj = lv_textarea_create(parent);
    layout_common_obj_set_base(obj, id, x, y, w, h, base, align);
    layout_common_obj_set_bg_color(obj, 0x00, LV_OPA_COVER, LV_STATE_DEFAULT);
    layout_common_obj_set_bg_color(obj, 0xFFFFFF, LV_OPA_COVER, LV_PART_CURSOR);
    layout_common_style_set_text(obj, NULL, 0Xffffff, LV_OPA_COVER, txt_align, font, 0);
    if (txt != NULL)
    {
        lv_obj_set_style_text_color(obj, lv_color_hex(0x929292), LV_PART_TEXTAREA_PLACEHOLDER);
        lv_obj_set_style_text_letter_space(obj, 5, LV_PART_TEXTAREA_PLACEHOLDER);
        lv_obj_set_style_text_opa(obj, LV_OPA_COVER, LV_PART_TEXTAREA_PLACEHOLDER);
        lv_textarea_set_placeholder_text(obj, txt);
    }
    lv_textarea_set_one_line(obj, false); // 单行模式会导致文本框的高度限制成字高
    lv_textarea_set_max_length(obj, txt_max);
    layout_common_obj_set_click_event(obj, cb, cb);

    lv_obj_set_style_pad_top(obj, txt_ofs_y, LV_PART_MAIN);
    lv_obj_set_style_anim_time(obj, 500, LV_PART_CURSOR);
    lv_obj_set_style_pad_left(obj, -2, LV_PART_CURSOR);
    lv_obj_set_style_pad_right(obj, -2, LV_PART_CURSOR);

    // lv_obj_add_state(obj, LV_STATE_FOCUSED);
    lv_textarea_set_password_bullet(obj, "*");
    lv_textarea_set_password_mode(obj, pwd_mode);
    return obj;
}
/*******************************************************************
 * @brief  : 创建消息对话框容器
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_msgdialog_cont_create(uint32_t color, lv_opa_t opa)
{
    lv_obj_t *cont = lv_obj_get_by_id(lv_scr_act(), LAYOUT_COMMON_MSGDIALOG_CONT);
    if (cont != NULL)
    {
        lv_obj_del(cont);
    }
    cont = layout_common_obj_create(lv_scr_act(), LAYOUT_COMMON_MSGDIALOG_CONT, 0, 0, 1024, 600, NULL, LV_ALIGN_DEFAULT, color, opa,
                                    0, 0x00, LV_OPA_TRANSP, 0, LV_BORDER_SIDE_NONE);
    return cont;
}
/*******************************************************************
 * @brief  : 删除消息对话框容器
 * @return  {*}
 *******************************************************************/
void layout_common_msgdialog_cont_del(void)
{
    lv_obj_t *cont = lv_obj_get_by_id(lv_scr_act(), LAYOUT_COMMON_MSGDIALOG_CONT);
    if (cont != NULL)
    {
        lv_obj_del(cont);
    }
}

int layout_common_background_display(uint32_t color, lv_opa_t opa, const void *img, lv_event_cb_t click_cb, bool transp)
{
    static lv_event_cb_t old_click_cb = NULL;
    if (click_cb != NULL)
    {
        if (old_click_cb != NULL)
        {
            lv_obj_remove_event_cb(lv_scr_act(), old_click_cb);
        }
        lv_obj_add_flag(lv_scr_act(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(lv_scr_act(), click_cb, LV_EVENT_CLICKED, NULL);
        old_click_cb = click_cb;
    }
    else
    {
        if (old_click_cb != NULL)
        {
            lv_obj_remove_event_cb(lv_scr_act(), old_click_cb);
            old_click_cb = NULL;
        }
        lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_CLICKABLE);
    }
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), opa, 0);

    lv_obj_set_style_bg_img_src(lv_scr_act(), img, 0);
    lv_obj_set_style_bg_img_opa(lv_scr_act(), img ? LV_OPA_COVER : LV_OPA_TRANSP, 0);

    lv_disp_t *disp = lv_disp_get_default();
    if (!disp)
    {
        return -1;
    }
    disp->driver->screen_transp = transp ? 1 : 0;
    lv_disp_set_bg_opa(disp, transp ? LV_OPA_TRANSP : LV_OPA_COVER);
    lv_port_disp_video(transp);

    return 0;
}