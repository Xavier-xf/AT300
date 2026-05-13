#ifndef _DB_QUEUE_H_
#define _DB_QUEUE_H_

// typedef void (*db_queue_release_cb)(void *node, void *user_data);

enum
{
    DB_QUEUE_CMD_FREE_GET,
    DB_QUEUE_CMD_VALID_GET,
    DB_QUEUE_CMD_RELEASE,
    DB_QUEUE_CMD_CLEAR,
};

void *db_queue_open(unsigned int node_size, unsigned int node_max);
void db_queue_close(void *context);
int db_queue_write(void *context, const void *node);
int db_queue_read(void *context, void *node);
int db_queue_ioctl(void *context, int cmd, void *param);

#endif // _DB_QUEUE_H_