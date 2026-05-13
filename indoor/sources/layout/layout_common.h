#ifndef _LAYOUT_COMMON_H_
#define _LAYOUT_COMMON_H_

#include "lvgl.h"
#include "db_time.h"
#include "db_common.h"
#include "tuya_sdk.h"
#include "user/user_app.h"
#include "user/user_layout.h"
#include "user/language.h"
#include "user/user_data.h"

#define layout_common_obj_set_base(obj, id, x_ofs, y_ofs, width, height, base, align) \
	{                                                                                 \
		if (id >= 0)                                                                  \
			lv_obj_set_id(obj, id);                                                   \
		if (width > 0)                                                                \
			lv_obj_set_style_width(obj, width, LV_STATE_DEFAULT);                     \
		if (height > 0)                                                               \
			lv_obj_set_style_height(obj, height, LV_STATE_DEFAULT);                   \
		if (align == LV_ALIGN_DEFAULT)                                                \
		{                                                                             \
			lv_obj_set_style_x(obj, x_ofs, LV_STATE_DEFAULT);                         \
			lv_obj_set_style_y(obj, y_ofs, LV_STATE_DEFAULT);                         \
		}                                                                             \
		else                                                                          \
			lv_obj_align_to(obj, base, align, x_ofs, y_ofs);                          \
	}

#define layout_common_obj_set_click_event(obj, cb, click)         \
	{                                                             \
		if (click)                                                \
			lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);          \
		else                                                      \
			lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);        \
		if (cb)                                                   \
			lv_obj_add_event_cb(obj, cb, LV_EVENT_CLICKED, NULL); \
	}

#define layout_common_obj_set_bg_color(obj, color, opa, state)      \
	{                                                               \
		lv_obj_set_style_bg_color(obj, lv_color_hex(color), state); \
		lv_obj_set_style_bg_opa(obj, opa, state);                   \
	}

#define layout_common_style_set_text(obj, str, color, opa, align, font, state) \
	{                                                                          \
		if (str != NULL)                                                       \
			lv_label_set_text(obj, str);                                       \
		if (font != NULL)                                                      \
			lv_obj_set_style_text_font(obj, font, state);                      \
		lv_obj_set_style_text_align(obj, align, state);                        \
		lv_obj_set_style_text_color(obj, lv_color_hex(color), state);          \
		lv_obj_set_style_text_opa(obj, LV_OPA_COVER, state);                   \
	}

#define layout_common_style_set_bg_img(obj, img, pres_recolor, pres_recolor_opa, align)         \
	{                                                                                           \
		if (img != NULL)                                                                        \
		{                                                                                       \
			lv_obj_set_style_bg_img_src(obj, img, LV_STATE_DEFAULT);                            \
			lv_obj_set_style_bg_img_align(obj, align, LV_STATE_DEFAULT);                        \
			lv_obj_set_style_bg_img_recolor(obj, lv_color_hex(pres_recolor), LV_STATE_PRESSED); \
			lv_obj_set_style_bg_img_recolor_opa(obj, pres_recolor_opa, LV_STATE_PRESSED);       \
		}                                                                                       \
	}

#define layout_common_style_set_pad(obj, top, left, buttom, right, column, row, part) \
	{                                                                                 \
		lv_obj_set_style_pad_top(obj, top, part);                                     \
		lv_obj_set_style_pad_left(obj, left, part);                                   \
		lv_obj_set_style_pad_bottom(obj, buttom, part);                               \
		lv_obj_set_style_pad_right(obj, right, part);                                 \
		lv_obj_set_style_pad_column(obj, column, part);                               \
		lv_obj_set_style_pad_row(obj, row, part);                                     \
	}

#define layout_common_style_set_border(obj, radius, color, opa, width, side, part) \
	{                                                                              \
		lv_obj_set_style_radius(obj, radius, part);                                \
		lv_obj_set_style_border_opa(obj, opa, part);                               \
		if (opa > LV_OPA_MIN)                                                      \
		{                                                                          \
			lv_obj_set_style_border_color(obj, lv_color_hex(color), part);         \
			lv_obj_set_style_border_width(obj, width, part);                       \
			lv_obj_set_style_border_side(obj, side, part);                         \
		}                                                                          \
	}

enum // 公共的对象id，layout页不能使用
{
	LAYOUT_COMMON_MSGDIALOG_CONT = 2000,
};

/*******************************************************************
 * @brief  : 创建一个基础对象
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_obj_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align, uint32_t bg_color, lv_opa_t bg_opa,
								   int radius, uint32_t boarder_color, lv_opa_t boarder_opa, int boarder_width, lv_border_side_t boarder_side);
/*******************************************************************
 * @brief  : 创建一个文本对象
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_text_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
									const char *txt, uint32_t color, lv_opa_t opa, lv_text_align_t txt_align, lv_font_t *font);
/*******************************************************************
 * @brief  : 创建一个图标对象
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_img_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
								   const char *img, lv_align_t img_align);
/*******************************************************************
 * @brief  : 创建一个带背景容器的文本
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_text_cont_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align, uint32_t bg_color, lv_opa_t bg_opa,
										 int radius, uint32_t boarder_color, lv_opa_t boarder_opa, int boarder_width, lv_border_side_t boarder_side,
										 const char *txt, uint32_t txt_color, lv_opa_t txt_opa, lv_text_align_t txt_align, lv_font_t *font);
/*******************************************************************
 * @brief  : 创建一个按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_btn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
								   lv_event_cb_t cb, uint32_t def_bg_color, lv_opa_t def_bg_opa, uint32_t pres_bg_color, lv_opa_t pres_bg_opa);
/*******************************************************************
 * @brief  : 创建一个图标按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_img_btn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
									   lv_event_cb_t cb, uint32_t def_bg_color, lv_opa_t def_bg_opa, uint32_t pres_bg_color, lv_opa_t pres_bg_opa,
									   const char *img, uint32_t recolor, lv_opa_t recolor_opa, lv_align_t img_align);
/*******************************************************************
 * @brief  : 创建一个文本按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_text_btn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
										lv_event_cb_t cb, uint32_t def_bg_color, lv_opa_t def_bg_opa, uint32_t pres_bg_color, lv_opa_t pres_bg_opa,
										const char *txt, uint32_t txt_color, lv_opa_t txt_opa, lv_text_align_t txt_align, lv_font_t *font);
/*******************************************************************
 * @brief  : 创建一个按下可切换图标的按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_imgbtn_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
									  lv_event_cb_t cb, const char *def_img, const char *pres_img);
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
											uint32_t txt_color, lv_opa_t txt_opa, lv_text_align_t text_align, lv_font_t *font);
/*******************************************************************
 * @brief  : 创建一个按下可切换图标的文本按键
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_imgbtn_text_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
										   lv_event_cb_t cb, const char *def_img, const char *pres_img,
										   const char *txt, uint32_t txt_color, lv_opa_t txt_opa, lv_text_align_t txt_align, lv_font_t *font);
/*******************************************************************
 * @brief  : 创建一个通用键盘
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_general_keyboard_create(lv_obj_t *parent, int id, lv_event_cb_t cb);
/*******************************************************************
 * @brief  : 创建一个数字键盘
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_number_keyboard_create(lv_obj_t *parent, int id, lv_event_cb_t cb);
/*******************************************************************
 * @brief  : 创建一个文本输入框
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_textarea_create(lv_obj_t *parent, int id, int x, int y, int w, int h, const lv_obj_t *base, lv_align_t align,
										const char *txt, lv_text_align_t txt_align, const lv_font_t *font, int txt_max, int txt_ofs_y,
										lv_event_cb_t cb, bool pwd_mode);
/*******************************************************************
 * @brief  : 创建消息对话框容器
 * @return  {*}
 *******************************************************************/
lv_obj_t *layout_common_msgdialog_cont_create(uint32_t color, lv_opa_t opa);
/*******************************************************************
 * @brief  : 删除消息对话框容器
 * @return  {*}
 *******************************************************************/
void layout_common_msgdialog_cont_del(void);

int layout_common_background_display(uint32_t color, lv_opa_t opa, const void *img, lv_event_cb_t click_cb, bool transp);

#endif // _LAYOUT_COMMON_H_
