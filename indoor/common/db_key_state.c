#include <stdlib.h>
#include <stdbool.h>
#include "db_key_state.h"
#include "db_common.h"

typedef enum
{
    KEY_STATE_NULL,  // 无动作
    KEY_STATE_SURE,  // 确认状态
    KEY_STATE_RAISE, // 按键抬起
    KEY_STATE_PRESS, // 按键按下
    KEY_STATE_CLICK, // 按键点击
    KEY_STATE_SHORT, // 短按
    KEY_STATE_LONG,  // 长按
} key_state_t;

typedef struct
{
    int sure_num;  // 消抖次数
    int short_num; // 短按次数
    int long_num;  // 长按次数
    int count;
    int level;
    key_state_t state;
    bool (*key_pressed)(int *);
    void (*event_cb)(db_key_event_t, int);
} key_state_context_t;

/*******************************************************************
 * @brief  : 按键状态机初始化
 * @return  {*}上下文
 * @param {int} sure_num消抖检测次数
 * @param {int} short_num短按检测次数
 * @param {int} long_num长按检测次数
 * @param {key_pressed} 按键状态读取回调函数，获取按键按下状态，通过指针参数返回键值（用于ADC按键）
 * @param {event_cb} 按键事件回调函数，key_event_t事件类型，int键值（用于ADC按键）
 *******************************************************************/
void *db_key_state_open(int sure_num, int short_num, int long_num, bool (*key_pressed)(int *), void (*event_cb)(db_key_event_t, int))
{
    // 参数有效性检查
    if (sure_num <= 0 || short_num <= sure_num || long_num <= short_num)
    {
        db_log_error("Invalid parameters: sure_num=%d, short_num=%d, long_num=%d", sure_num, short_num, long_num);
        return NULL;
    }
    
    if (key_pressed == NULL || event_cb == NULL)
    {
        db_log_error("Invalid callback functions");
        return NULL;
    }

    key_state_context_t *ks = (key_state_context_t *)malloc(sizeof(key_state_context_t));
    if (!ks)
    {
        db_log_error("Failed to allocate memory for key state context");
        return NULL;
    }

    ks->sure_num = sure_num;
    ks->short_num = short_num;
    ks->long_num = long_num;
    ks->count = 0;
    ks->level = 0;
    ks->state = KEY_STATE_NULL;
    ks->key_pressed = key_pressed;
    ks->event_cb = event_cb;
    
    db_log_info("Key state machine initialized successfully");
    return ks;
}
/*******************************************************************
 * @brief  : 按键状态机反初始化
 * @return  {*}
 * @param {void *} context上下文
 *******************************************************************/
int db_key_state_close(void *context)
{
    if (!context)
    {
        db_log_error("Invalid context pointer");
        return -1;
    }
    
    free(context);
    db_log_info("Key state machine deinitialized successfully");
    return 0;
}
/*******************************************************************
 * @brief  : 按键状态机处理
 * @return  {*}
 * @param {void *} context上下文
 *******************************************************************/
void db_key_state_process(void *context)
{
    if (!context)
    {
        db_log_error("Invalid context pointer");
        return;
    }
    
    key_state_context_t *ks = (key_state_context_t *)context;
    bool pressed = ks->key_pressed(&ks->level);
    
    switch (ks->state)
    {
    case KEY_STATE_NULL:
        if (pressed)
        {
            ks->state = KEY_STATE_SURE;
            ks->count = 1; // 开始计数
        }
        break;

    case KEY_STATE_SURE:
        if (pressed)
        {
            if (ks->count >= ks->sure_num)
            {
                ks->state = KEY_STATE_PRESS;
                ks->event_cb(KEY_EVENT_PRESS, ks->level);
                // 继续计数，不需要重置
            }
            else
            {
                ks->count++;
            }
        }
        else
        {
            // 消抖失败，回到初始状态
            ks->state = KEY_STATE_NULL;
            ks->count = 0;
        }
        break;

    case KEY_STATE_PRESS:
        if (pressed)
        {
            if (ks->count >= ks->short_num)
            {
                ks->state = KEY_STATE_SHORT;
                ks->event_cb(KEY_EVENT_SHORT, ks->level);
            }
            else
            {
                ks->count++;
            }
        }
        else
        {
            // 按键释放，触发点击事件
            ks->state = KEY_STATE_RAISE;
            ks->event_cb(KEY_EVENT_CLICK, ks->level);
        }
        break;

    case KEY_STATE_SHORT:
        if (pressed)
        {
            if (ks->count >= ks->long_num)
            {
                ks->state = KEY_STATE_LONG;
                ks->event_cb(KEY_EVENT_LONG, ks->level);
            }
            else
            {
                ks->count++;
            }
        }
        else
        {
            // 短按后释放，触发点击事件
            ks->state = KEY_STATE_RAISE;
            ks->event_cb(KEY_EVENT_CLICK, ks->level);
        }
        break;

    case KEY_STATE_LONG:
        if (!pressed)
        {
            // 长按后释放，只触发抬起事件，不触发点击事件
            ks->state = KEY_STATE_RAISE;
            // 注意：长按后释放通常不应该再触发点击事件
        }
        break;

    case KEY_STATE_RAISE:
        ks->state = KEY_STATE_NULL;
        ks->count = 0;
        ks->event_cb(KEY_EVENT_RAISE, ks->level);
        break;

    default:
        // 未知状态，重置
        db_log_warn("Unknown key state: %d, resetting", ks->state);
        ks->state = KEY_STATE_NULL;
        ks->count = 0;
        break;
    }
}