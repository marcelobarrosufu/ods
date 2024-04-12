#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "hal.h"

void port_tmr_general_cbk(int sig, siginfo_t *si, void *uc)
{
    hal_tmr_args_t *args = (hal_tmr_args_t *) si->_sifields._rt.si_sigval.sival_ptr;

    if(args->cbk)
    {
        uint32_t st = hal_cpu_critical_section_enter();
        args->cbk(args->arg);
        hal_cpu_critical_section_leave(st);
    }
}

static hal_tmr_id_t port_tmr_create(uint64_t expiration_ms, bool cyclic, hal_tmr_args_t *args)
{
    int res = 0;
    timer_t timerId = 0;
    struct sigevent sev = { 0 };
    struct itimerspec its = {   
        .it_value.tv_sec  = expiration_ms / 1000,
        .it_value.tv_nsec =  (expiration_ms % 1000) * 1000000,
        .it_interval.tv_sec = 0,
        .it_interval.tv_nsec = 0,
        };
                                
    if(cyclic)
    {
        its.it_interval.tv_sec  = its.it_value.tv_sec;
        its.it_interval.tv_nsec = its.it_value.tv_nsec;
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = args;

    res = timer_create(CLOCK_REALTIME, &sev, &timerId);

    if (res != 0)
    {
        fprintf(stderr, "Error timer_create: %s\n", strerror(errno));
        exit(-1);
    }

    struct sigaction sa = { 0 };
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = port_tmr_general_cbk;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGRTMIN, &sa, NULL) == -1)
    {
        fprintf(stderr, "Error sigaction: %s\n", strerror(errno));
        exit(-1);
    }

    res = timer_settime(timerId, 0, &its, NULL);

    if (res != 0)
    {
        fprintf(stderr, "Error timer_settime: %s\n", strerror(errno));
        exit(-1);
    }

    return (hal_tmr_id_t) timerId;
}

static void port_tmr_delete(hal_tmr_id_t _tmr_id)
{
    timer_delete( (timer_t ) _tmr_id);
}

static void port_tmr_init(void) 
{

}

hal_tmr_driver_t HAL_TMR_DRIVER =
{
    .create = port_tmr_create,
    .delete = port_tmr_delete,
    .init = port_tmr_init
};



