#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "db_ringbuffer.h"
#include "db_common.h"

#define ringbuffer_lock() pthread_mutex_lock(&(rb->mutex))
#define ringbuffer_unlock() pthread_mutex_unlock(&(rb->mutex))

#define ringbuffer_free_size(rb) (rb->buf_size - rb->valid_size)
#define ringbuffer_valid_size(rb) (rb->valid_size)

typedef struct
{
    unsigned char *buffer;
    int buf_size;
    int valid_size;
    int head;
    int tail;

    pthread_mutex_t mutex;
} db_ringbuffer_t;

void *db_ringbuffer_open(unsigned int buf_size)
{
    db_ringbuffer_t *rb = (db_ringbuffer_t *)malloc(sizeof(db_ringbuffer_t));
    if (rb == NULL)
    {
        db_log_error("Failed to allocate ringbuffer structure");
        return NULL;
    }

    rb->buffer = (unsigned char *)malloc(buf_size);
    if (rb->buffer == NULL)
    {
        db_log_error("Failed to allocate ringbuffer buffer");
        free(rb);
        return NULL;
    }

    rb->buf_size = buf_size;
    rb->valid_size = 0;
    rb->head = 0;
    rb->tail = 0;

    if (pthread_mutex_init(&(rb->mutex), NULL) != 0)
    {
        db_log_error("Failed to initialize mutex");
        free(rb->buffer);
        free(rb);
        return NULL;
    }

    db_log_debug("Ringbuffer created: buf_size=%u", buf_size);
    return rb;
}

void db_ringbuffer_close(void *context)
{
    db_ringbuffer_t *rb = (db_ringbuffer_t *)context;
    if (rb == NULL)
    {
        return;
    }

    if (pthread_mutex_destroy(&(rb->mutex)) != 0)
    {
        db_log_warn("Failed to destroy mutex");
    }

    if (rb->buffer)
    {
        free(rb->buffer);
    }

    free(rb);
    db_log_debug("Ringbuffer closed");
}

int db_ringbuffer_write(void *context, const void *data, int len)
{
    db_ringbuffer_t *rb = (db_ringbuffer_t *)context;
    if (rb == NULL || data == NULL || len <= 0)
    {
        return -1;
    }

    ringbuffer_lock();

    int free = ringbuffer_free_size(rb);
    if (free < len)
    {
        ringbuffer_unlock();
        // db_log_warn("Ring buffer is not enough! data len:[%d] free:[%d]", len, free);
        return -1;
    }

    int write_size = rb->buf_size - rb->head;
    if (write_size >= len)
    {
        memcpy(rb->buffer + rb->head, data, len);
        rb->head += len;
    }
    else
    {
        memcpy(rb->buffer + rb->head, data, write_size);
        memcpy(rb->buffer, data + write_size, len - write_size);
        rb->head = len - write_size;
    }

    rb->valid_size += len;

    // db_log_debug("Ringbuffer write successful: len=%d, head=%d, tail=%d, valid_size=%d",
    //              len, rb->head, rb->tail, rb->valid_size);

    ringbuffer_unlock();
    return 0;
}

int db_ringbuffer_read(void *context, void *data, int len)
{
    db_ringbuffer_t *rb = (db_ringbuffer_t *)context;
    if (rb == NULL || data == NULL || len <= 0)
    {
        return -1;
    }

    ringbuffer_lock();

    if (ringbuffer_valid_size(rb) < len)
    {
        ringbuffer_unlock();
        // db_log_debug("Ring buffer valid:[%d] read:[%d]", ringbuffer_valid_size(rb), len);
        return -1;
    }

    int read_size = rb->buf_size - rb->tail;
    if (read_size >= len)
    {
        memcpy(data, rb->buffer + rb->tail, len);
        rb->tail += len;
    }
    else
    {
        memcpy(data, rb->buffer + rb->tail, read_size);
        memcpy(data + read_size, rb->buffer, len - read_size);
        rb->tail = len - read_size;
    }

    rb->valid_size -= len;

    // db_log_debug("Ringbuffer read successful: len=%d, head=%d, tail=%d, valid_size=%d",
    //              len, rb->head, rb->tail, rb->valid_size);

    ringbuffer_unlock();
    return 0;
}

int db_ringbuffer_ioctl(void *context, int cmd, void *param)
{
    db_ringbuffer_t *rb = (db_ringbuffer_t *)context;
    if (rb == NULL)
    {
        return -1;
    }

    ringbuffer_lock();

    switch (cmd)
    {
    case DB_RINGBUFFER_CMD_FREE_GET:
        if (param)
        {
            int *free = (int *)param;
            *free = ringbuffer_free_size(rb);
            // db_log_debug("Ringbuffer free size: %d", *free);
        }
        break;

    case DB_RINGBUFFER_CMD_VALID_GET:
        if (param)
        {
            int *valid = (int *)param;
            *valid = ringbuffer_valid_size(rb);
            // db_log_debug("Ringbuffer valid size: %d", *valid);
        }
        break;

    case DB_RINGBUFFER_CMD_FLUSH:
        rb->valid_size = rb->head = rb->tail = 0;
        // db_log_debug("Ringbuffer flushed");
        break;

    default:
        db_log_warn("Invalid ringbuffer command: %d", cmd);
        ringbuffer_unlock();
        return -1;
    }

    ringbuffer_unlock();
    return 0;
}
