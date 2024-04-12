#include "hal.h"

static hal_rtc_driver_t *drv = &HAL_RTC_DRIVER;

static time_t hal_rtc_basetime = 0;

void hal_rtc_time_from_basetime(uint32_t time_sec, hal_rtc_date_time_t *dt)
{
    time_t cur_time = hal_rtc_basetime + time_sec;
    struct tm *cur_tm = localtime(&cur_time);

    dt->year = cur_tm->tm_year + 1900;
    dt->month = cur_tm->tm_mon + 1;
    dt->day = cur_tm->tm_mday;
    dt->hour = cur_tm->tm_hour;
    dt->minute = cur_tm->tm_min;
    dt->second = cur_tm->tm_sec;
}

uint32_t hal_rtc_secs_since_basetime(hal_rtc_date_time_t *dt)
{
	struct tm cur_time = {0};

    cur_time.tm_year = dt->year - 1900;
    cur_time.tm_mon = dt->month - 1;
    cur_time.tm_mday = dt->day;
    cur_time.tm_hour = dt->hour;
    cur_time.tm_min = dt->minute;
    cur_time.tm_sec = dt->second;

    return (uint32_t) difftime(mktime(&cur_time), hal_rtc_basetime);
}

void hal_rtc_set(hal_rtc_date_time_t *dt)
{
	drv->set(dt);
}

void hal_rtc_get(hal_rtc_date_time_t *dt)
{
	drv->get(dt);
}

void hal_rtc_init(void)
{
	drv->init();

    // set base time to 2023/01/01 00:00:00
	struct tm base_time = {0};

    base_time.tm_year = 123;
    base_time.tm_mon = 0;
    base_time.tm_mday = 1;
    base_time.tm_hour = 0;
    base_time.tm_min = 0;
    base_time.tm_sec = 0;

    hal_rtc_basetime = mktime(&base_time);
}

void hal_rtc_nvram_write(uint32_t index, uint32_t val)
{
    drv->nvram_write(index, val);
}

uint32_t hal_rtc_nvram_read(uint32_t index)
{
    return drv->nvram_read(index);
}

void hal_rtc_nvram_format(void)
{
	drv->nvram_format();
}
