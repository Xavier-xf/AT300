#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "onvif.h"
#include <getopt.h>
#include <unistd.h>
#include "db_onvif.h"

static int db_profile_check(struct OnvifData *onvif_data, int index)
{
    if (getProfileToken(onvif_data, index))
    {
        printf("ERROR: get profile token - %s\n", onvif_data->last_error);
        return -1;
    }
    if (strlen(onvif_data->profileToken) == 0)
    {
        printf("ERROR: invalid profile token - %d\n", index);
        return -1;
    }
    printf("  Profile set to %s\n\n", onvif_data->profileToken);
    if (getProfile(onvif_data))
    {
        printf("ERROR: get profile - %s\n", onvif_data->last_error);
        return -1;
    }
    return 0;
}

int db_onvif_discovery_device(db_onvif_discovery_info_t *info, int max)
{
    struct OnvifSession *onvif_session = (struct OnvifSession *)calloc(1, sizeof(struct OnvifSession));
    struct OnvifData *onvif_data = (struct OnvifData *)calloc(1, sizeof(struct OnvifData));

    initializeSession(onvif_session);
    getActiveNetworkInterfaces(onvif_session);
    int index = 0;
    int total = 0;
    while (index < ONVIF_MAX_INTERFACES)
    {
        char *element = onvif_session->active_network_interfaces[index];
        if (strlen(element))
        {
            char *ip_address = element;
            memset(onvif_session->preferred_network_address, 0, sizeof(onvif_session->preferred_network_address));
            strcpy(onvif_session->preferred_network_address, ip_address);

            int n = broadcast(onvif_session);
            printf("Found %d cameras on interface %s\n", n, ip_address);
            for (int i = 0; i < n; i++)
            {
                if (prepareOnvifData(i, onvif_session, onvif_data))
                {
                    char temp[32] = {0};
                    const char *start, *end;
                    int len = 0;
                    if ((start = strstr(onvif_data->xaddrs, "//")) == NULL)
                    {
                        continue;
                    }
                    start += 2;
                    if ((end = strchr(start, '/')) == NULL)
                    {
                        continue;
                    }
                    len = end - start;
                    if (len >= sizeof(temp))
                    {
                        continue;
                    }
                    strncpy(temp, start, len);
                    if ((end = strchr(start, ':')) == NULL)
                    {
                        strncpy(info[i].ip, temp, sizeof(info[i].ip));
                        info[i].port = 80;
                    }
                    else
                    {
                        len = end - start;
                        strncpy(info[i].ip, temp, len);
                        info[i].port = atoi(end + 1);
                    }
                    getHostname(onvif_data);
                    printf("%s:%d %s(%s)\n", info[i].ip, info[i].port,
                           onvif_data->host_name,
                           onvif_data->camera_name);
                    printf("%s\n", onvif_data->xaddrs);
                    strncpy(info[i].host, onvif_data[i].host_name, sizeof(info[i].host));
                    strncpy(info[i].name, onvif_data[i].camera_name, sizeof(info[i].name));
                    info[i].host[sizeof(info[i].host) - 1] = 0;
                    info[i].name[sizeof(info[i].name) - 1] = 0;
                    total++;
                    max--;
                    if (max == 0)
                    {
                        goto finish;
                    }
                }
                else
                {
                    printf("found invalid xaddrs in device response\n");
                }
            }

            index++;
        }
        else
        {
            break;
        }
    }
finish:
    closeSession(onvif_session);
    free(onvif_session);
    free(onvif_data);
    return total;
}

int db_onvif_get_rtsp(const char *ip, int port, const char *username, const char *password)
{
    int index = 0;
    struct OnvifData *onvif_data = (struct OnvifData *)calloc(1, sizeof(struct OnvifData));
    sprintf(onvif_data->xaddrs, "http://%s:%d/onvif/device_service", ip, port);
    strcpy(onvif_data->device_service, onvif_data->xaddrs);
    extractOnvifService(onvif_data->device_service, true);
    sprintf(onvif_data->host, "%s", ip);
    strcpy(onvif_data->username, username);
    strcpy(onvif_data->password, password);
    if (getCapabilities(onvif_data))
    {
        printf("ERROR: get capabilities - %s \n", onvif_data->last_error);
    }
    while (1)
    {
        if (db_profile_check(onvif_data, index) < 0)
            break;
        if (getStreamUri(onvif_data))
        {
            printf("ERROR: get stream uri - %s\n", onvif_data->last_error);
            break;
        }
        if (password[0] != '\0')
        {
            printf("profile%d media uri:[rtsp://%s:%s@%s]\n", index, username, password, &onvif_data->stream_uri[7]);
        }
        else
        {
            printf("profile%d media uri:[%s]\n", index, onvif_data->stream_uri);
        }
        index++;
    }
    free(onvif_data);
    return 0;
}

int db_onvif_get_network(const char *ip, int port, const char *username, const char *password)
{
    struct OnvifData *onvif_data = (struct OnvifData *)calloc(1, sizeof(struct OnvifData));
    sprintf(onvif_data->xaddrs, "http://%s:%d/onvif/device_service", ip, port);
    strcpy(onvif_data->device_service, onvif_data->xaddrs);
    extractOnvifService(onvif_data->device_service, true);
    sprintf(onvif_data->host, "%s", ip);
    strcpy(onvif_data->username, username);
    strcpy(onvif_data->password, password);
    if (getNetworkInterfaces(onvif_data))
    {
        printf("ERROR: get network interfaces - %s\n", onvif_data->last_error);
        goto fail;
    }
    if (getNetworkDefaultGateway(onvif_data))
    {
        printf("ERROR: get network default gateway - %s\n", onvif_data->last_error);
        goto fail;
    }
    if (getDNS(onvif_data))
    {
        printf("ERROR: get DNS - %s\n", onvif_data->last_error);
        goto fail;
    }
    printf("  IP Address: %s\n", onvif_data->ip_address_buf);
    printf("  Gateway:    %s\n", onvif_data->default_gateway_buf);
    printf("  DNS:        %s\n", onvif_data->dns_buf);
    printf("  DHCP:       %s\n\n", onvif_data->dhcp_enabled ? "YES" : "NO");
fail:
    free(onvif_data);
    return 0;
}

int db_onvif_get_imaging(const char *ip, int port, const char *username, const char *password)
{
    struct OnvifData *onvif_data = (struct OnvifData *)calloc(1, sizeof(struct OnvifData));
    sprintf(onvif_data->xaddrs, "http://%s:%d/onvif/device_service", ip, port);
    strcpy(onvif_data->device_service, onvif_data->xaddrs);
    extractOnvifService(onvif_data->device_service, true);
    sprintf(onvif_data->host, "%s", ip);
    strcpy(onvif_data->username, username);
    strcpy(onvif_data->password, password);
    if (getCapabilities(onvif_data))
    {
        printf("ERROR: get capabilities - %s \n", onvif_data->last_error);
    }
    if (db_profile_check(onvif_data, 0) < 0)
    {
        goto fail;
    }
    if (getOptions(onvif_data))
    {
        printf("ERROR: get options - %s\n", onvif_data->last_error);
        goto fail;
    }
    printf("  Min Brightness: %d\n", onvif_data->brightness_min);
    printf("  Max Brightness: %d\n", onvif_data->brightness_max);
    printf("  Min ColorSaturation: %d\n", onvif_data->saturation_min);
    printf("  Max ColorSaturation: %d\n", onvif_data->saturation_max);
    printf("  Min Contrast: %d\n", onvif_data->contrast_min);
    printf("  Max Contrast: %d\n", onvif_data->contrast_max);
    printf("  Min Sharpness: %d\n", onvif_data->sharpness_min);
    printf("  Max Sharpness: %d\n\n", onvif_data->sharpness_max);

    if (getImagingSettings(onvif_data))
    {
        printf("ERROR: get imaging settings - %s\n", onvif_data->last_error);
        goto fail;
    }
    printf("  Brightness: %d\n", onvif_data->brightness);
    printf("  Contrast:   %d\n", onvif_data->contrast);
    printf("  Saturation: %d\n", onvif_data->saturation);
    printf("  Sharpness:  %d\n\n", onvif_data->sharpness);

fail:
    free(onvif_data);
    return 0;
}