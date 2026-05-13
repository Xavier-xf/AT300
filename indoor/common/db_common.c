
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "mbedtls/md5.h"
// #include "dyc_tfcard.h"
#include "db_common.h"

#define NETWORK_PACKAET_MAX (100 * 1024)

// int dyc_common_init(dyc_common_config_t *cfg)
// {

//     dyc_media_init(cfg->tf_det_dev_name, cfg->tf_mount_path, cfg->tf_volume_name, cfg->flash_media_path);
//     return 0;
// }

size_t db_file_size_get(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0)
    {
        printf("read file size failed,(%s) \n", path);
        return 0;
    }
    return st.st_size;
}

size_t db_file_read(const char *path, char *data, size_t size)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s fialed\n", path);
        return 0;
    }
    size_t ret = read(fd, data, size);
    close(fd);
    return ret;
}

/* 对一个文件做MD5检验，并返回一个32字节的检验码字符串 */
int db_md5_by_file(const char *file, char *buf, int len)
{
#define FILE_MD5_BUF_SIZE (512 * 1024)
    int ret = -1;
    FILE *fp = NULL;
    size_t read_size = 0;
    unsigned char *buffer = NULL;
    mbedtls_md5_context ctx;

    if (access(file, F_OK) != 0)
    {
        goto exit;
    }

    buffer = (unsigned char *)malloc(FILE_MD5_BUF_SIZE);
    if (buffer == NULL)
    {
        goto exit;
    }

    fp = fopen(file, "rb");
    if (fp == NULL)
    {
        goto exit;
    }

    mbedtls_md5_init(&ctx);

    if ((ret = mbedtls_md5_starts(&ctx)) != 0)
    {
        goto exit;
    }

    while ((read_size = fread(buffer, 1, FILE_MD5_BUF_SIZE, fp)) > 0)
    {
        if ((ret = mbedtls_md5_update(&ctx, buffer, read_size)) != 0)
        {
            goto exit;
        }
    }

    if ((ret = mbedtls_md5_finish(&ctx, buffer)) != 0)
    {
        goto exit;
    }

    snprintf(buf, len, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7],
             buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15]);

exit:
    mbedtls_md5_free(&ctx);
    if (fp)
        fclose(fp);
    if (buffer)
        free(buffer);
    return ret;
}

/* 对一段数据做MD5检验，并返回一个32字节的检验码字符串 */
int db_md5_by_data(const unsigned char *data, int len, char *buf, int size)
{
    int ret = -1;
    mbedtls_md5_context ctx;
    unsigned char md5_buf[16] = {0};

    if (data == NULL)
    {
        goto exit;
    }

    mbedtls_md5_init(&ctx);

    if ((ret = mbedtls_md5_starts(&ctx)) != 0)
    {
        goto exit;
    }

    if ((ret = mbedtls_md5_update(&ctx, data, len)) != 0)
    {
        goto exit;
    }

    if ((ret = mbedtls_md5_finish(&ctx, md5_buf)) != 0)
    {
        goto exit;
    }

    snprintf(buf, size, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             md5_buf[0], md5_buf[1], md5_buf[2], md5_buf[3], md5_buf[4], md5_buf[5], md5_buf[6], md5_buf[7],
             md5_buf[8], md5_buf[9], md5_buf[10], md5_buf[11], md5_buf[12], md5_buf[13], md5_buf[14], md5_buf[15]);

exit:
    mbedtls_md5_free(&ctx);
    return ret;
}

/* 获取指定网卡的ip、mac、mask */
bool db_ip_mac_address_get(const char *eth, char *ip, char *mac, char *mask)
{
    int sock;
    bool reslut = false;
    struct sockaddr_in sin;
    struct ifreq ifr;
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock == -1)
    {
        close(sock);
        // printf("Error:get local IP socket fail!\n");
        return false;
    }

    strncpy(ifr.ifr_name, eth, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ip != NULL)
    {
        if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
        {
            close(sock);
            // printf("Error:get local IP ioctl fail! \n");
            return false;
        }

        memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
        sprintf(ip, "%s", inet_ntoa(sin.sin_addr));
        reslut = true;
        printf("ip:%s\n", ip);
    }

    if (mask != NULL)
    {
        if (ioctl(sock, SIOCGIFNETMASK, &ifr) < 0)
        {
            close(sock);
            // printf("Error:get local IP ioctl fail! \n");
            return false;
        }

        memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
        sprintf(mask, "%s", inet_ntoa(sin.sin_addr));
        reslut = true;
    }

    if (mac != NULL)
    {

        if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
        {
            close(sock);
            // perror("Error:get local mac ioctl fail! \n");
            return false;
        }

        struct sockaddr sa;
        memcpy(&sa, &ifr.ifr_addr, sizeof(struct sockaddr_in));
        sprintf((char *)mac, "%02X:%02X:%02X:%02X:%02X:%02X", sa.sa_data[0], sa.sa_data[1], sa.sa_data[2], sa.sa_data[3], sa.sa_data[4], sa.sa_data[5]);
        reslut = true;
    }

    close(sock);
    return reslut;
}

/* 获取默认网关 */
bool db_default_gateway_get(const char *interface, char *default_gateway, int len)
{
    FILE *fp;
    char line[128];

    // Open the /proc/net/route file
    fp = fopen("/proc/net/route", "r");
    if (fp == NULL)
    {
        perror("Failed to open /proc/net/route");
        return false;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strstr(line, "Destination") != NULL)
            continue;

        char iface[16];
        unsigned long dest;
        unsigned long gateway;
        int flags;
        int refcnt;
        int use;
        int metric;
        unsigned long int mask;
        sscanf(line, "%s %lx %lx %x %d %d %d %lx", iface, &dest, &gateway, &flags, &refcnt, &use, &metric, &mask);

        if (dest == 0 && strcmp(interface, iface) == 0)
        {
            struct in_addr addr;
            addr.s_addr = gateway;
            strncpy(default_gateway, strdup(inet_ntoa(addr)), len);

            break;
        }
    }

    fclose(fp);
    return true;
}

/* 获取默认dns */
bool db_default_dns_get(const char *eth, char *_dns, int len)
{
    bool reslut = false;
    FILE *pfd = popen("cat /etc/resolv.conf", "r");
    if (pfd == NULL)
    {
        return reslut;
    }
    char dns[64] = {0};
    char buffer[128] = {0};
    while (fgets(buffer, sizeof(buffer), pfd))
    {
        if (strstr(buffer, "nameserver") != NULL)
        {
            memset(dns, 0, sizeof(dns));
            sscanf(buffer, "nameserver %s", dns);
            reslut = true;
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }
    pclose(pfd);

    if (reslut == true)
    {
        strncpy(_dns, dns, len);
        printf("dns:%s\n", _dns);
    }

    return reslut;
}

/* 杀死指定的进程 */
bool db_process_kill(const char *process_name)
{
#define MAX_BUFF_SIZE 1024
    bool reslut = false;
    char buffer[MAX_BUFF_SIZE];
    char cmd[128] = {0};
    sprintf(cmd, "ps aux | grep -v grep | grep -F \"%s\"", process_name);
    FILE *pipe = popen(cmd, "r");
    if (pipe == NULL)
    {
        return false;
    }
    while (fgets(buffer, MAX_BUFF_SIZE, pipe) != NULL)
    {
        if (strstr(buffer, process_name) != NULL)
        {
            int pid = 0;
            char temp[256] = {0};
            sscanf(buffer, "%d %s", &pid, temp);

            memset(cmd, 0, sizeof(cmd));
            sprintf(cmd, "kill -s 9 %d", pid);
            system(cmd);
            printf("kill %s pid is %d\n", process_name, pid);
            reslut = true;
        }
    }
    pclose(pipe);
    return reslut;
}

/* udhchc 获取IP */
void db_network_udhcpc_ip(const char *eth)
{
    char cmd[256] = {0};
    sprintf(cmd, "udhcpc -i %s -s /etc/init.d/udhcpc.script", eth);
    db_process_kill(cmd);
    sprintf(cmd, "udhcpc -b -i %s -s /etc/init.d/udhcpc.script", eth);
    db_process_kill(cmd);
    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "udhcpc -b -i %s -s /etc/init.d/udhcpc.script &", eth);
    system(cmd);
}

bool db_ipaddr_valid_check(const char *ip)
{
    if (ip == NULL || ip[0] == '\0')
    {
        return false;
    }
    int date1, date2, date3, date4;
    date1 = date2 = date3 = date4 = -1;
    sscanf(ip, "%d.%d.%d.%d", &date1, &date2, &date3, &date4);
    if (date1 >= 0 && date1 <= 255)
    {
        if (date2 >= 0 && date2 <= 255)
        {
            if (date3 >= 0 && date3 <= 255)
            {
                if (date4 >= 0 && date4 <= 255)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// 将子网掩码转换为prefix_length
int db_mask_to_prefix(const char *mask)
{
    struct in_addr addr;

    if (inet_pton(AF_INET, mask, &addr) != 1)
    {
        return -1; // 无效的子网掩码
    }

    unsigned long netmask = ntohl(addr.s_addr);

    // 检查是否为有效的子网掩码
    if ((~netmask & (~netmask + 1)) != 0)
    {
        return -1; // 无效的子网掩码
    }

    int prefix = 0;
    while (netmask & 0x80000000)
    {
        prefix++;
        netmask <<= 1;
    }

    return prefix;
}

// 将prefix_length转换为子网掩码
int db_prefix_to_mask(int prefix, char *mask, int len)
{
    if (prefix < 0 || prefix > 32)
    {
        return -1; // 无效的前缀长度
    }

    unsigned long netmask = 0;
    for (int i = 0; i < prefix; i++)
    {
        netmask |= (0x80000000 >> i);
    }

    struct in_addr addr;
    addr.s_addr = htonl(netmask);

    inet_ntop(AF_INET, &addr, mask, len);

    return 0; // 转换成功
}

int db_subnet_mask_convert(char *mask)
{
    if (mask == NULL)
    {
        return 0;
    }
    int length = 0;
    char *saveptr = NULL;
    char *token = strtok_r(mask, ".", &saveptr);
    while (token != NULL)
    {
        int number = atoi(token);
        length += number == 255 ? 8 : (number == 0 ? 0 : -1);
        token = strtok_r(NULL, ".", &saveptr);
    }
    return length;
}
