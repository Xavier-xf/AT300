#ifndef _DB_ONVIF_H_
#define _DB_ONVIF_H_

typedef struct
{
    char ip[16];
    int port;
    char host[32];
    char name[32];
} db_onvif_discovery_info_t;

typedef struct
{
    char ip[16];
    int port;
    char username[32];
    char password[32];
} db_onvif_device_info_t;

int db_onvif_discovery_device(db_onvif_discovery_info_t *info, int max);

int db_onvif_get_rtsp(const char *ip, int port, const char *username, const char *password);

int db_onvif_get_network(const char *ip, int port, const char *username, const char *password);

int db_onvif_get_imaging(const char *ip, int port, const char *username, const char *password);

#endif // _DB_ONVIF_H_