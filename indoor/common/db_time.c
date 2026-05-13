#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include "db_time.h"
#include "db_common.h"

#define NTP_SERVER "pool.ntp.org"         // 全球可用的 NTP 服务器
#define NTP_PORT 123                      // NTP 默认端口
#define NTP_PACKET_SIZE 48                // NTP 协议包大小
#define NTP_TIMESTAMP_DELTA 2208988800ULL // 1970 和 1900 的时间差（秒）

typedef struct
{
    uint8_t li_vn_mode;       // Leap Indicator, Version Number, Mode
    uint8_t stratum;          // Stratum level
    uint8_t poll;             // Poll interval
    uint8_t precision;        // Precision
    uint32_t root_delay;      // Root delay
    uint32_t root_dispersion; // Root dispersion
    uint32_t ref_id;          // Reference ID
    uint32_t ref_ts_sec;      // Reference timestamp (seconds)
    uint32_t ref_ts_frac;     // Reference timestamp (fraction)
    uint32_t orig_ts_sec;     // Origin timestamp (seconds)
    uint32_t orig_ts_frac;    // Origin timestamp (fraction)
    uint32_t recv_ts_sec;     // Receive timestamp (seconds)
    uint32_t recv_ts_frac;    // Receive timestamp (fraction)
    uint32_t trans_ts_sec;    // Transmit timestamp (seconds)
    uint32_t trans_ts_frac;   // Transmit timestamp (fraction)
} ntp_packet;

int db_time_week_get(int year, int month, int day)
{
    if (year < 1 || month < 1 || month > 12 || day < 1 || day > 31)
    {
        db_log_error("Invalid date parameters: year=%d, month=%d, day=%d", year, month, day);
        return -1;
    }
    
    if (month == 1 || month == 2)
    {
        month += 12;
        year--;
    }
    return (day + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7 + 1;
}

int db_cur_month_last_day(struct tm *date)
{
    if (date == NULL)
    {
        db_log_error("Invalid date pointer");
        return -1;
    }
    
    unsigned int year = date->tm_year;
    unsigned char mon = date->tm_mon;
    
    if (mon < 1 || mon > 12)
    {
        db_log_error("Invalid month: %d", mon);
        return -1;
    }
    
    if (mon == 2)
    {
        if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400) == 0)
        {
            return 29;
        }
        else
        {
            return 28;
        }
    }
    else
    {
        switch (mon)
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        default:
            break;
        }
    }
    
    return -1;
}

void db_time_get(struct tm *tm)
{
    if (tm == NULL)
    {
        db_log_error("Invalid time pointer");
        return;
    }
    
    time_t t = time(NULL);
    if (t == -1)
    {
        db_log_error("Failed to get current time");
        memset(tm, 0, sizeof(struct tm));
        return;
    }
    
    localtime_r(&t, tm);
    tm->tm_year += 1900;
    tm->tm_mon += 1;
    
    int weekday = db_time_week_get(tm->tm_year, tm->tm_mon, tm->tm_mday);
    if (weekday != -1)
    {
        tm->tm_wday = weekday;
    }
}

void db_time_set(struct tm *tm)
{
    if (tm == NULL)
    {
        db_log_error("Invalid time pointer");
        return;
    }
    
    if (tm->tm_year < 1970 || tm->tm_mon < 1 || tm->tm_mon > 12 || tm->tm_mday < 1 || tm->tm_mday > 31 ||
        tm->tm_hour < 0 || tm->tm_hour > 23 || tm->tm_min < 0 || tm->tm_min > 59 || tm->tm_sec < 0 || tm->tm_sec > 60)
    {
        db_log_error("Invalid time values: %04d-%02d-%02d %02d:%02d:%02d", 
                    tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        return;
    }
    
    char string[64] = {0};
    snprintf(string, sizeof(string), "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"", 
             tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    
    if (system(string) != 0)
    {
        db_log_error("Failed to set system date: %s", string);
        return;
    }
    
    if (system("hwclock -w") != 0)
    {
        db_log_error("Failed to set hardware clock");
    }
    else
    {
        db_log_info("System time set to: %s", string);
    }
}

unsigned long long db_timestamp_ms_get(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0)
    {
        db_log_error("Failed to get current time of day");
        return 0;
    }
    return tv.tv_sec * 1000ULL + tv.tv_usec / 1000;
}

unsigned long long db_timestamp_us_get(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0)
    {
        db_log_error("Failed to get current time of day");
        return 0;
    }
    return tv.tv_sec * 1000000ULL + tv.tv_usec;
}

unsigned long long db_sys_clock_ms_get(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        db_log_error("Failed to get monotonic clock");
        return 0;
    }
    return ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000;
}

unsigned long long db_sys_clock_us_get(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        db_log_error("Failed to get monotonic clock");
        return 0;
    }
    return ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
}

void db_timestamp_to_time(const time_t *timep, struct tm *tm)
{
    if (timep == NULL || tm == NULL)
    {
        db_log_error("Invalid time pointers");
        return;
    }
    
    localtime_r(timep, tm);
    tm->tm_year += 1900;
    tm->tm_mon += 1;
    
    int weekday = db_time_week_get(tm->tm_year, tm->tm_mon, tm->tm_mday);
    if (weekday != -1)
    {
        tm->tm_wday = weekday;
    }
}

time_t db_timestamp_network_utc_get(void)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    ntp_packet packet;
    time_t ntp_time = -1;
    
    // Set timeout for socket operations
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
    {
        db_log_error("Socket creation failed: %s", strerror(errno));
        return -1;
    }
    
    // Set socket timeout
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        db_log_warn("Failed to set socket timeout: %s", strerror(errno));
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NTP_PORT);
    
    // Try multiple NTP servers for reliability
    const char *ntp_servers[] = {
        "162.159.200.1",
        "pool.ntp.org",
        "time.nist.gov"
    };
    
    int success = 0;
    for (int i = 0; i < sizeof(ntp_servers)/sizeof(ntp_servers[0]); i++)
    {
        if (inet_pton(AF_INET, ntp_servers[i], &serv_addr.sin_addr) <= 0)
        {
            db_log_warn("Invalid NTP server address: %s", ntp_servers[i]);
            continue;
        }

        memset(&packet, 0, sizeof(packet));
        packet.li_vn_mode = (0x03 << 6) | (0x03 << 3) | 0x03;

        if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            db_log_warn("Failed to send NTP request to %s: %s", ntp_servers[i], strerror(errno));
            continue;
        }

        if (recv(sockfd, &packet, sizeof(packet), 0) < 0)
        {
            db_log_warn("Failed to receive NTP response from %s: %s", ntp_servers[i], strerror(errno));
            continue;
        }
        
        // Successfully received response
        ntp_time = ntohl(packet.trans_ts_sec) - NTP_TIMESTAMP_DELTA;
        db_log_info("Successfully retrieved NTP time from %s", ntp_servers[i]);
        success = 1;
        break;
    }

    close(sockfd);
    
    if (!success)
    {
        db_log_error("Failed to retrieve NTP time from all servers");
    }
    
    return ntp_time;
}

// Persian Calendar (Jalali) to Gregorian Calendar conversion
int db_jalali_to_gregorian(int jalali_year, int jalali_month, int jalali_day, int *gregorian_year, int *gregorian_month, int *gregorian_day)
{
    if (gregorian_year == NULL || gregorian_month == NULL || gregorian_day == NULL)
    {
        db_log_error("Invalid output pointers for Gregorian date");
        return -1;
    }
    
    // Check Jalali date validity
    if (jalali_year < 1 || jalali_month < 1 || jalali_month > 12 || jalali_day < 1)
    {
        db_log_error("Invalid Jalali date: %d-%d-%d", jalali_year, jalali_month, jalali_day);
        return -1;
    }
    
    // Jalali to Gregorian conversion algorithm
    long jy = jalali_year - 979;
    long jm = jalali_month - 1;
    long jd = jalali_day - 1;
    
    long j_day_no = 365 * jy + (jy / 33) * 8 + ((jy % 33) + 3) / 4;
    for (long i = 0; i < jm; ++i)
        j_day_no += (i < 6) ? 31 : 30;
    j_day_no += jd;
    
    long g_day_no = j_day_no + 79;
    long gy = 1600 + 400 * (g_day_no / 146097);
    g_day_no = g_day_no % 146097;
    
    bool leap = true;
    if (g_day_no >= 36525)
    {
        g_day_no--;
        gy += 100 * (g_day_no / 36524);
        g_day_no = g_day_no % 36524;
        
        if (g_day_no >= 365)
            g_day_no++;
        else
            leap = false;
    }
    
    gy += 4 * (g_day_no / 1461);
    g_day_no = g_day_no % 1461;
    
    if (g_day_no >= 366)
    {
        leap = false;
        g_day_no--;
        gy += g_day_no / 365;
        g_day_no = g_day_no % 365;
    }
    
    long gd = g_day_no + 1;
    long gm = 0;
    const long kDaysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    for (long i = 0; i < 12 && gd > kDaysInMonth[i] + (i == 1 && leap); ++i)
    {
        gd -= kDaysInMonth[i] + (i == 1 && leap);
        gm++;
    }
    gm++;
    
    *gregorian_year = gy;
    *gregorian_month = gm;
    *gregorian_day = gd;
    
    return 0;
}

// Gregorian Calendar to Persian Calendar (Jalali) conversion
int db_gregorian_to_jalali(int gregorian_year, int gregorian_month, int gregorian_day, int *jalali_year, int *jalali_month, int *jalali_day)
{
    if (jalali_year == NULL || jalali_month == NULL || jalali_day == NULL)
    {
        db_log_error("Invalid output pointers for Jalali date");
        return -1;
    }
    
    // Check Gregorian date validity
    if (gregorian_year < 1970 || gregorian_month < 1 || gregorian_month > 12 || gregorian_day < 1)
    {
        db_log_error("Invalid Gregorian date: %d-%d-%d", gregorian_year, gregorian_month, gregorian_day);
        return -1;
    }
    
    // Gregorian to Jalali conversion algorithm
    long gy = gregorian_year - 1600;
    long gm = gregorian_month - 1;
    long gd = gregorian_day - 1;
    
    long g_day_no = 365 * gy + (gy + 3) / 4 - (gy + 99) / 100 + (gy + 399) / 400;
    for (long i = 0; i < gm; ++i)
        g_day_no += (i < 1) ? 31 : (i < 5) ? 30 : 31;
    g_day_no += gd;
    
    long j_day_no = g_day_no - 79;
    long j_np = j_day_no / 12053;
    j_day_no %= 12053;
    
    long jy = 979 + 33 * j_np + 4 * (j_day_no / 1461);
    j_day_no %= 1461;
    
    if (j_day_no >= 366)
    {
        jy += (j_day_no - 1) / 365;
        j_day_no = (j_day_no - 1) % 365;
    }
    
    long jm = 0;
    const long kDaysInJalaliMonth[] = {31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29};
    
    for (long i = 0; i < 12 && j_day_no >= kDaysInJalaliMonth[i] + (i == 11 && !(jy % 33 % 4)); ++i)
    {
        j_day_no -= kDaysInJalaliMonth[i] + (i == 11 && !(jy % 33 % 4));
        jm++;
    }
    
    *jalali_year = jy;
    *jalali_month = jm + 1;
    *jalali_day = j_day_no + 1;
    
    return 0;
}