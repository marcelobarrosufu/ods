#include <unistd.h>
#include <fcntl.h>

#include "hal.h"

#define PORT_FILE_NAME_LEN    256
#define PORT_NVRAM_NUM_WORDS 32
#define PORT_NVRAM_WORD_SIZE 4

typedef struct port_rtc_ctrl_s
{
    FILE *pf;
    uint8_t name[PORT_FILE_NAME_LEN];
    bool is_open;
} port_rtc_ctrl_t;

port_rtc_ctrl_t port_rtc_ctrl = { 0 };

static void port_rtc_get(hal_rtc_date_time_t *dt)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    dt->year = tm.tm_year + 1900;
    dt->month = tm.tm_mon + 1;
    dt->day = tm.tm_mday;
    dt->hour = tm.tm_hour;
    dt->minute = tm.tm_min;
    dt->second = tm.tm_sec;
}

static void port_rtc_set(hal_rtc_date_time_t *dt)
{
    // N.A.
}

static void port_rtc_init(void)
{
}
hal_rtc_driver_t HAL_RTC_DRIVER =
{
    .get = port_rtc_get,
    .set = port_rtc_set,
	.init = port_rtc_init,
};
