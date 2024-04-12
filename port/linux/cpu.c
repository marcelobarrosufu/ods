#define _XOPEN_SOURCE   600
#define _POSIX_C_SOURCE 200112L

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "hal.h"

static pthread_mutex_t port_cpu_cs;
static pthread_mutexattr_t port_cpu_cs_attr;
static bool port_cpu_init_done = false;

void port_cpu_ctrlc_handler(int signum)
{
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_PORT, "CTRL+C pressed!\n");
    exit(1);
}

static uint32_t port_cpu_critical_section_enter(void)
{
    if (port_cpu_init_done == false)
        return 0;

    pthread_mutex_lock(&port_cpu_cs);

    return 0;
}

static void port_cpu_critical_section_leave(uint32_t st)
{
    if (port_cpu_init_done == false)
        return;

    pthread_mutex_unlock(&port_cpu_cs);
}

static void port_cpu_sleep(uint32_t tmr_ms)
{
    usleep(tmr_ms * 1000);
}

static void port_cpu_init(void)
{
    srand(time(NULL));

    pthread_mutexattr_init(&port_cpu_cs_attr);
    pthread_mutexattr_settype(&port_cpu_cs_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&port_cpu_cs, &port_cpu_cs_attr);
    
    if (signal(SIGINT, port_cpu_ctrlc_handler) == SIG_ERR) 
    {
        UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_PORT, "CTRL+C handler installation failed!\n");
    }

    port_cpu_init_done = true;
}

static void port_cpu_deinit(void)
{
}

hal_cpu_driver_t HAL_CPU_DRIVER =
    {
        .critical_section_enter = port_cpu_critical_section_enter,
        .critical_section_leave = port_cpu_critical_section_leave,
        .sleep_ms = port_cpu_sleep,
        .init = port_cpu_init,
        .deinit = port_cpu_deinit,
};
