#ifndef _DB_RINGBUFFER_H_
#define _DB_RINGBUFFER_H_

enum
{
    DB_RINGBUFFER_CMD_FREE_GET,
    DB_RINGBUFFER_CMD_VALID_GET,
    DB_RINGBUFFER_CMD_FLUSH,
};

void *db_ringbuffer_open(unsigned int buf_size);
void db_ringbuffer_close(void *context);
int db_ringbuffer_write(void *context, const void *data, int len);
int db_ringbuffer_read(void *context, void *data, int len);
int db_ringbuffer_ioctl(void *context, int cmd, void *param);

#endif // _DB_RINGBUFFER_H_