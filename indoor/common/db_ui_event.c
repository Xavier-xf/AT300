#include <stdlib.h>
#include "lvgl.h"
#include "db_ui_event.h"
#include "db_queue.h"
#include "db_common.h"

static void (*event_callback_func[MSG_EVENT_TOTAL])(db_ui_event_t *) = {NULL};
static void (*default_callback_func[MSG_EVENT_TOTAL])(db_ui_event_t *) = {NULL};

static void *event_queue_context = NULL;

static void _ui_event_process_timer(lv_timer_t *t)
{
    db_ui_event_t ui_event;
    if (event_queue_context == NULL)
    {
        db_log_error("UI event queue not initialized");
        return;
    }
    
    int ret = db_queue_read(event_queue_context, &ui_event);
    if (ret == 0)
    {
        if (default_callback_func[ui_event.event] != NULL)
        {
            default_callback_func[ui_event.event](&ui_event);
        }
        if (event_callback_func[ui_event.event] != NULL)
        {
            event_callback_func[ui_event.event](&ui_event);
        }
        if (ui_event.type == 1 && ui_event.data.p != NULL)
        {
            free(ui_event.data.p);
            ui_event.data.p = NULL;  // Prevent double free
        }
    }
}

void db_ui_event_init(void)
{
    event_queue_context = db_queue_open(sizeof(db_ui_event_t), 32);
    if (event_queue_context == NULL)
    {
        db_log_error("Failed to create UI event queue");
        return;
    }
    
    lv_timer_t *timer = lv_timer_create(_ui_event_process_timer, 30, NULL);
    if (timer == NULL)
    {
        db_log_error("Failed to create UI event process timer");
        db_queue_close(event_queue_context);
        event_queue_context = NULL;
        return;
    }
    
    lv_timer_ready(timer);
    db_log_info("UI event system initialized");
}

int db_ui_event_send(int event, unsigned long arg1, unsigned long arg2)
{
    if (event >= MSG_EVENT_TOTAL)
    {
        db_log_error("Invalid event ID[%d], send failed", event);
        return -1;
    }
    
    if (event_queue_context == NULL)
    {
        db_log_error("UI event queue not initialized");
        return -1;
    }
    
    db_ui_event_t ui_event;
    ui_event.event = event;
    ui_event.type = 0;
    ui_event.arg[0] = arg1;
    ui_event.arg[1] = arg2;
    
    int ret = db_queue_write(event_queue_context, &ui_event);
    if (ret != 0)
    {
        db_log_error("Failed to write UI event to queue, error: %d", ret);
    }
    
    return ret;
}

int db_ui_data_send(int event, unsigned char *data, unsigned long size)
{
    if (event >= MSG_EVENT_TOTAL)
    {
        db_log_error("Invalid event ID[%d], send failed", event);
        return -1;
    }
    
    if (event_queue_context == NULL)
    {
        db_log_error("UI event queue not initialized");
        return -1;
    }
    
    if (data == NULL && size > 0)
    {
        db_log_error("Invalid data pointer for non-zero size");
        return -1;
    }
    
    db_ui_event_t ui_event;
    ui_event.event = event;
    ui_event.type = 1;
    ui_event.data.size = size;
    
    if (size > 0)
    {
        ui_event.data.p = (void *)malloc(size);
        if (ui_event.data.p == NULL)
        {
            db_log_error("Failed to allocate memory for UI event data, size: %lu", size);
            return -1;
        }
        memcpy(ui_event.data.p, data, size);
    }
    else
    {
        ui_event.data.p = NULL;
    }
    
    int ret = db_queue_write(event_queue_context, &ui_event);
    if (ret != 0)
    {
        db_log_error("Failed to write UI data event to queue, error: %d", ret);
        if (ui_event.data.p != NULL)
        {
            free(ui_event.data.p);
        }
    }
    
    return ret;
}

int db_ui_event_cb_register(int event, void (*cb)(db_ui_event_t *))
{
    if (event >= MSG_EVENT_TOTAL)
    {
        db_log_error("Invalid event ID[%d], register failed", event);
        return -1;
    }
    
    event_callback_func[event] = cb;
    db_log_debug("UI event callback registered for event: %d", event);
    return 0;
}

int db_ui_default_cb_register(int event, void (*cb)(db_ui_event_t *))
{
    if (event >= MSG_EVENT_TOTAL)
    {
        db_log_error("Invalid event ID[%d], register failed", event);
        return -1;
    }
    
    default_callback_func[event] = cb;
    db_log_debug("UI default event callback registered for event: %d", event);
    return 0;
}