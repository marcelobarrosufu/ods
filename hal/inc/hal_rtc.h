#pragma once

typedef enum hal_rtc_weekday_e
{
    HAL_RTC_WEEKDAY_INVALID   = 0,
    HAL_RTC_WEEKDAY_MONDAY    = 1,
    HAL_RTC_WEEKDAY_TUESDAY   = 2,
    HAL_RTC_WEEKDAY_WEDNESDAY = 3,
    HAL_RTC_WEEKDAY_THURSDAY  = 4,
    HAL_RTC_WEEKDAY_FRIDAY    = 5,
    HAL_RTC_WEEKDAY_SATURDAY  = 6,
    HAL_RTC_WEEKDAY_SUNDAY    = 7,
} hal_rtc_weekday_t;

typedef enum hal_rtc_month_e
{
    HAL_RTC_MONTH_INVALID   = 0,
    HAL_RTC_MONTH_JANUARY   = 1,
    HAL_RTC_MONTH_FEBRUARY  = 2,
    HAL_RTC_MONTH_MARCH     = 3,
    HAL_RTC_MONTH_APRIL     = 4,
    HAL_RTC_MONTH_MAY       = 5,
    HAL_RTC_MONTH_JUNE      = 6,
    HAL_RTC_MONTH_JULY      = 7,
    HAL_RTC_MONTH_AUGUST    = 8,
    HAL_RTC_MONTH_SEPTEMBER = 9,
    HAL_RTC_MONTH_OCTOBER   = 10,
    HAL_RTC_MONTH_NOVEMBER  = 11,
    HAL_RTC_MONTH_DECEMBER  = 12,
} hal_rtc_month_t;

typedef enum hal_rtc_alarm_type_e
{
    HAL_RTC_ALARM_ONE_SHOT = 0,
    HAL_RTC_ALARM_EVERY_HOUR,
    HAL_RTC_ALARM_EVERY_DAY,
    HAL_RTC_ALARM_EVERY_WEEK,
} hal_rtc_alarm_type_t;

typedef struct hal_rtc_date_time_s
{
    uint16_t year;
    hal_rtc_month_t month;
    uint8_t day;
    hal_rtc_weekday_t wday;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} hal_rtc_date_time_t;

typedef struct hal_rtc_driver_s
{
	void (*set)(hal_rtc_date_time_t *dt);
	void (*get)(hal_rtc_date_time_t *dt);
	void (*init)(void);
} hal_rtc_driver_t;

void hal_rtc_time_from_basetime(uint32_t time_sec, hal_rtc_date_time_t *dt);
uint32_t hal_rtc_secs_since_basetime(hal_rtc_date_time_t *dt);
void hal_rtc_set(hal_rtc_date_time_t *dt);
void hal_rtc_get(hal_rtc_date_time_t *dt);
void hal_rtc_init(void);
void hal_rtc_nvram_write(uint32_t index, uint32_t val);
uint32_t hal_rtc_nvram_read(uint32_t index);
void hal_rtc_nvram_format(void);

