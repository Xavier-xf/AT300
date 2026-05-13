#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "db_queue.h"
#include "db_common.h"

#define queue_lock() pthread_mutex_lock(&(q->mutex))
#define queue_unlock() pthread_mutex_unlock(&(q->mutex))

typedef struct
{
    unsigned char *buffer;
    int node_size;
    int node_max;
    int head;
    int count;

    pthread_mutex_t mutex;
} db_queue_t;

void *db_queue_open(unsigned int node_size, unsigned int node_max)
{
    db_queue_t *q = (db_queue_t *)malloc(sizeof(db_queue_t));
    if (q == NULL)
    {
        db_log_error("Failed to allocate queue structure");
        return NULL;
    }
    
    q->buffer = (unsigned char *)malloc(node_size * node_max);
    if (q->buffer == NULL)
    {
        db_log_error("Failed to allocate queue buffer");
        free(q);
        return NULL;
    }
    
    q->node_size = node_size;
    q->node_max = node_max;
    q->head = 0;
    q->count = 0;
    
    if (pthread_mutex_init(&(q->mutex), NULL) != 0)
    {
        db_log_error("Failed to initialize mutex");
        free(q->buffer);
        free(q);
        return NULL;
    }
    
    db_log_debug("Queue created: node_size=%u, node_max=%u", node_size, node_max);
    return q;
}

void db_queue_close(void *context)
{
    db_queue_t *q = (db_queue_t *)context;
    if (q == NULL)
    {
        return;
    }
    
    if (pthread_mutex_destroy(&(q->mutex)) != 0)
    {
        db_log_warn("Failed to destroy mutex");
    }
    
    if (q->buffer)
    {
        free(q->buffer);
    }
    
    free(q);
    db_log_debug("Queue closed");
}

int db_queue_write(void *context, const void *node)
{
    db_queue_t *q = (db_queue_t *)context;
    if (q == NULL || node == NULL)
    {
        return -1;
    }

    queue_lock();

    if (q->count >= q->node_max)
    {
        queue_unlock();
        db_log_warn("Queue is full, node_max=%d", q->node_max);
        return -1;
    }

    int insert_pos = (q->head + q->count) % q->node_max;
    memcpy(q->buffer + insert_pos * q->node_size, node, q->node_size);
    q->count++;
    
    db_log_debug("Queue write successful, current count:%d", q->count);
    queue_unlock();
    return 0;
}

int db_queue_read(void *context, void *node)
{
    db_queue_t *q = (db_queue_t *)context;
    if (q == NULL || node == NULL)
    {
        return -1;
    }

    queue_lock();

    if (q->count == 0)
    {
        queue_unlock();
        db_log_debug("Queue is empty");
        return -1;
    }

    memcpy(node, q->buffer + q->head * q->node_size, q->node_size);
    q->count--;
    q->head = (q->head + 1) % q->node_max;
    
    db_log_debug("Queue read successful, current count:%d", q->count);
    queue_unlock();

    return 0;
}

int db_queue_ioctl(void *context, int cmd, void *param)
{
    db_queue_t *q = (db_queue_t *)context;
    if (q == NULL)
    {
        return -1;
    }

    queue_lock();
    
    switch (cmd)
    {
    case DB_QUEUE_CMD_FREE_GET:
        if (param)
        {
            int *free = (int *)param;
            *free = q->node_max - q->count;
            db_log_debug("Queue free nodes: %d", *free);
        }
        break;
    
    case DB_QUEUE_CMD_VALID_GET:
        if (param)
        {
            int *valid = (int *)param;
            *valid = q->count;
            db_log_debug("Queue valid nodes: %d", *valid);
        }
        break;
    
    case DB_QUEUE_CMD_RELEASE:
        // Implement release functionality if needed
        break;
    
    case DB_QUEUE_CMD_CLEAR:
        q->head = 0;
        q->count = 0;
        db_log_debug("Queue cleared");
        break;
    
    default:
        db_log_warn("Invalid queue command: %d", cmd);
        queue_unlock();
        return -1;
    }
    
    queue_unlock();
    return 0;
}