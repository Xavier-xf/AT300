#ifndef _DB_COMMON_H_
#define _DB_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>

#define LOG_LEVEL_INFO 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3

#define USER_LOG_LEVEL LOG_LEVEL_ERROR

#define _LOG_COLOR_WHITE "\033[0m"
#define _LOG_COLOR_RED "\033[0;31m"
#define _LOG_COLOR_GREEN "\033[0;32m"
#define _LOG_COLOR_YELLOW "\033[0;33m"
#define _LOG_COLOR_BLUE "\033[0;34m"
#define _LOG_COLOR_PINK "\033[0;35m"
#define _LOG_COLOR_CYAN "\033[0;36m"
#define _LOG_COLOR_NONE "\033[0m"

#if (USER_LOG_LEVEL >= LOG_LEVEL_ERROR)
#define db_log_error(format, ...) \
    printf(_LOG_COLOR_RED "\r[ERROR][%s:%04u]  " format _LOG_COLOR_NONE "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define db_log_error(format, ...)
#endif

#if (USER_LOG_LEVEL >= LOG_LEVEL_WARN)
#define db_log_warn(format, ...) \
    printf(_LOG_COLOR_YELLOW "\r[WARNING][%s:%04u]  " format _LOG_COLOR_NONE "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define db_log_warn(format, ...)
#endif

#if (USER_LOG_LEVEL >= LOG_LEVEL_DEBUG)
#define db_log_debug(format, ...) \
    printf(_LOG_COLOR_GREEN "\r[DEBUG][%s:%04u]  " format _LOG_COLOR_NONE "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define db_log_debug(format, ...)
#endif

#if (USER_LOG_LEVEL >= LOG_LEVEL_INFO)
#define db_log_info(format, ...) \
    printf(_LOG_COLOR_WHITE "\r[INFO][%s:%04u]  " format _LOG_COLOR_NONE "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define db_log_info(format, ...)
#endif

#define COMPILE_TIME_ASSERT(cond) typedef char static_assertion[(cond) ? 1 : -1]

/************************************************************
** 函数说明: 线程堆栈设置
** 作者: DYC.liu
** 日期：2024-04-01 13:38:45
** 参数说明:
** 注意事项：
************************************************************/
static inline pthread_attr_t *pthread_stack_attr(void)
{
    static pthread_attr_t thread_attr;
    size_t stacksize = 200 * 1024;
    pthread_attr_setstacksize(&thread_attr, stacksize);
    return &thread_attr;
}

// 判断属于子字符串的首地址
static inline char *extract_string_end_char_prefix(const char *input, const char *prefix, char e, char *output, size_t size)
{
    const char *pos = NULL;
    const char *end = NULL;
    size_t output_len = 0;
    if ((pos = strstr(input, prefix)) != NULL)
    {
        pos += strlen(prefix);
        if ((end = strchr(pos, e)) != NULL)
        {
            output_len = end - pos;
            strncpy(output, pos, output_len > size ? size : output_len);
        }
        output[size - 1] = '\0';
    }
    else
    {
        output[0] = '\0';
    }
    return (char *)end;
}

// 判断属于子字符串的首地址
static inline char *extract_string_prefix(const char *input, const char *prefix, char *output, size_t size)
{
    return extract_string_end_char_prefix(input, prefix, '\n', output, size);
}
// 获取第n个不连续的空格的首个地址
static inline char *string_spaces_after_char(const char *string, int n)
{
    int space_count = 0;
    const char *ptr = string;
    while ((*ptr) != '\0')
    {
        if ((*ptr) == ' ')
        {
            if ((*(ptr + 1) != ' ') && (*(ptr + 1) != '\0'))
            {
                space_count++;
                if (space_count == n)
                {
                    return (char *)(ptr + 1);
                }
            }
        }
        ++ptr;
    }
    return NULL;
}

size_t db_file_size_get(const char *path);

size_t db_file_read(const char *path, char *data, size_t size);

/* 对一个文件做MD5检验，并返回一个32字节的检验码字符串 */
int db_md5_by_file(const char *file, char *buf, int len);

/* 对一段数据做MD5检验，并返回一个32字节的检验码字符串 */
int db_md5_by_data(const unsigned char *data, int len, char *buf, int size);

bool db_ip_mac_address_get(const char *eth, char *ip, char *mac, char *mask);

bool db_process_kill(const char *process_name);

void db_network_udhcpc_ip(const char *eth);

bool db_default_gateway_get(const char *interface, char *default_gateway, int len);

bool db_default_dns_get(const char *eth, char *_dns, int len);

bool db_ipaddr_valid_check(const char *ip);

int db_mask_to_prefix(const char *mask);

int db_prefix_to_mask(int prefix, char *mask, int len);

int db_subnet_mask_convert(char *mask);


#endif // _DB_COMMON_H_
