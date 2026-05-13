#ifndef _DB_MSG_EVENT_H_
#define _DB_MSG_EVENT_H_

enum
{
    MSG_EVENT_IMAGE_DECODE_FINISH,
    MSG_EVENT_MUSIC_LOAD_SUCCESS,
    MSG_EVENT_MUSIC_PLAY_FINISH,
    MSG_EVENT_VIDEO_DECODE_FRAME,
    MSG_EVENT_TOTAL,
};

typedef struct
{
    void *p;
    unsigned long size;
} msg_data_t;

typedef struct
{
    int event;
    int type;
    union
    {
        unsigned long arg[2];
        msg_data_t data;
    };
} db_ui_event_t;

void db_ui_event_init(void);
int db_ui_event_send(int event, unsigned long arg1, unsigned long arg2);
int db_ui_data_send(int event, unsigned char *data, unsigned long size);
int db_ui_event_cb_register(int event, void (*cb)(db_ui_event_t *));
int db_ui_default_cb_register(int event, void (*cb)(db_ui_event_t *));

#endif // _DB_MSG_EVENT_H_