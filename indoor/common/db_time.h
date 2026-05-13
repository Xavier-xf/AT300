#ifndef _DB_TIME_H_
#define _DB_TIME_H_

#include <sys/time.h>
#include <time.h>

int db_cur_month_last_day(struct tm *date);

void db_time_get(struct tm *tm);

void db_time_set(struct tm *tm);

unsigned long long db_timestamp_ms_get(void);

unsigned long long db_timestamp_us_get(void);

unsigned long long db_sys_clock_ms_get(void);

unsigned long long db_sys_clock_us_get(void);

void db_timestamp_to_time(const time_t *timep, struct tm *tm);

time_t db_timestamp_network_utc_get(void);

// Persian Calendar (Jalali) conversion functions
int db_jalali_to_gregorian(int jalali_year, int jalali_month, int jalali_day, int *gregorian_year, int *gregorian_month, int *gregorian_day);
int db_gregorian_to_jalali(int gregorian_year, int gregorian_month, int gregorian_day, int *jalali_year, int *jalali_month, int *jalali_day);

#endif