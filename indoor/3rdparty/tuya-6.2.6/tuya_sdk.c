#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>
#include "db_common.h"
#include "tuya_sdk.h"

typedef struct
{
    tuya_init_config_t cfg;
    int runing;
    pthread_t tid;
    pthread_mutex_t mutex;
} tuya_private_data;

static tuya_private_data tuya_ctx = {
    .runing = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
};

static char tuya_network_online_check(const char *dev)
{
    char on_line = 0x00;

    char network_state[128] = {0};
    sprintf(network_state, "/sys/class/net/%s/operstate", dev);
    int fd = open(network_state, O_RDONLY);

    if (fd < 0)
    {
        return on_line;
    }

    char buffer[128] = {0};
    read(fd, buffer, 2);
    close(fd);

    if (strncmp(buffer, "up", 2))
    {
        return on_line;
    }

    on_line = 0x01;

#define PING_WWW "www.microsoft.com"

    struct hostent *url = gethostbyname(PING_WWW);

    if (url != NULL)
    {
        printf("official hostname:%s\n", url->h_name);
        on_line = 0x02;
    }

    return on_line;
}

static void *tuya_sdk_thread(void *arg)
{
    tuya_private_data *d = (tuya_private_data *)arg;

    while (tuya_network_online_check(d->cfg.net_dev) != 0x02)
    {
        sleep(2);
    }

    ty_sdk_main(&d->cfg);

    while (d->runing)
    {
        sleep(10);
    }
    return NULL;
}

int tuya_sdk_init(const tuya_init_config_t *cfg)
{
    pthread_mutex_lock(&tuya_ctx.mutex);
    if (tuya_ctx.runing)
    {
        db_log_error("tuya context runing \n");
        pthread_mutex_unlock(&tuya_ctx.mutex);
        return -1;
    }
    memcpy(&tuya_ctx.cfg, cfg, sizeof(tuya_init_config_t));
    tuya_ctx.runing = 1;
    pthread_create(&tuya_ctx.tid, pthread_stack_attr(), tuya_sdk_thread, &tuya_ctx);
    pthread_mutex_unlock(&tuya_ctx.mutex);
    return 0;
}

int tuya_sdk_deinit(void)
{
    pthread_mutex_lock(&tuya_ctx.mutex);
    if (tuya_ctx.runing)
    {
        tuya_ctx.runing = 0;
        pthread_join(tuya_ctx.tid, NULL);
    }

    pthread_mutex_unlock(&tuya_ctx.mutex);
    return 0;
}