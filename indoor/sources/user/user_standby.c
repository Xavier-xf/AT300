#include <stdio.h>
#include "lvgl.h"
#include "user_standby.h"

/***
** 日期: 2022-05-10 08:45
** 作者: leo.liu
** 函数作用：超时时间
** 返回参数说明：
***/
static int standby_timeout = 30000;

/***
** 日期: 2022-05-10 08:45
** 作者: leo.liu
** 函数作用：待机使能
** 返回参数说明：
***/
static bool standby_timer_enable = false;
/***
** 日期: 2022-05-10 08:33
** 作者: leo.liu
** 函数作用：待机跳转页面
** 返回参数说明：
***/
static layout_info *standby_goto_page = NULL;

/***
** 日期: 2022-05-10 08:38
** 作者: leo.liu
** 函数作用：定时器溢出时间
** 返回参数说明：
***/
// static unsigned long long standby_timeout_timestamp = 0;
static lv_timer_t *standby_dection_timer_handle = NULL;
/***
** 日期: 2022-05-10 08:41
** 作者: leo.liu
** 函数作用：检测待机超时定时器
** 返回参数说明：
***/
static void standby_dection_timer(lv_timer_t *t)
{
	if (standby_timer_enable == false)
	{
		return;
	}
	{
		if (standby_goto_page != NULL)
		{
			_layout_goto(standby_goto_page);
		}
	}
}

/***
** 日期: 2022-05-10 08:33
** 作者: leo.liu
** 函数作用：初始化待机检测
** 返回参数说明：
***/
bool standby_timer_init(layout_info *page, int timeout)
{
	if (page != NULL)
	{
		standby_goto_page = page;
	}
	standby_timeout = timeout;
	standby_timer_enable = false;
	if (standby_dection_timer_handle)
		lv_timer_del(standby_dection_timer_handle);
	standby_dection_timer_handle = lv_timer_create(standby_dection_timer, /* 1000 */ standby_timeout, NULL);
	// lv_timer_ready(standby_dection_timer_handle);
	return true;
}

/***
** 日期: 2022-05-10 08:47
** 作者: leo.liu
** 函数作用：待机检测重新及时
** 返回参数说明：
***/
bool standby_timer_restart(bool fouce_enable)
{
	if (fouce_enable)
	{
		standby_timer_enable = true;
	}
	if (standby_dection_timer_handle)
		lv_timer_reset(standby_dection_timer_handle);
	return true;
}

/************************************************************
** 函数说明: 重置待机时间
** 作者: xiaoxiao
** 日期: 2023-06-07 14:52:47
** 参数说明:
** 注意事项:
************************************************************/
bool standby_timer_reset(int timeout)
{
	standby_timeout = timeout;
	if (standby_dection_timer_handle)
	{
		lv_timer_set_period(standby_dection_timer_handle, standby_timeout);
		lv_timer_reset(standby_dection_timer_handle);
	}
	return true;
}
/***
** 日期: 2022-05-10 08:42
** 作者: leo.liu
** 函数作用：关闭待机检测
** 返回参数说明：
***/
bool standby_timer_close(void)
{
	if (standby_timer_enable == false)
	{
		return false;
	}

	standby_timer_enable = false;
	return true;
}
/***
** 日期: 2022-05-10 08:42
** 作者: leo.liu
** 函数作用：待机检测状态获取
** 返回参数说明：
***/
bool standby_timer_status_get(void)
{
	return standby_timer_enable;
}
