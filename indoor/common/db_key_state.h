#ifndef _DB_KEY_STATE_H_
#define _DB_KEY_STATE_H_

#include <stdbool.h>

/* 按键状态机的状态 */
typedef enum
{
    KEY_EVENT_RAISE, // 按键抬起
    KEY_EVENT_PRESS, // 按键按下
    KEY_EVENT_CLICK, // 按键点击
    KEY_EVENT_SHORT, // 按键短按
    KEY_EVENT_LONG,  // 按键长按
} db_key_event_t;

void *db_key_state_open(int sure_num, int short_num, int long_num, bool (*key_pressed)(int *), void (*event_cb)(db_key_event_t, int));

int db_key_state_close(void *context);

void db_key_state_process(void *context);

#endif //_DB_KEY_STATE_H_