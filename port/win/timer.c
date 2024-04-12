#include <windows.h>

#include "hal.h"

static HANDLE tmr_queue = NULL;

static VOID CALLBACK port_tmr_general_cbk(_In_ PVOID param, _In_ BOOLEAN timer_or_wait_fired)
{
    hal_tmr_args_t *args = (hal_tmr_args_t *)param;
    
    if(args->cbk)
    {
        uint32_t st = hal_cpu_critical_section_enter();
        args->cbk(args->arg);
        hal_cpu_critical_section_leave(st);
    }
}

static void *port_tmr_create(uint64_t expiration_ms, bool cyclic, hal_tmr_args_t *args)
{
    HANDLE htmr = NULL;
    BOOL ret = CreateTimerQueueTimer(&htmr, tmr_queue, port_tmr_general_cbk, (PVOID) args, expiration_ms, cyclic ? expiration_ms : 0, 0);
    assert(ret == TRUE);

    return (void *)htmr;
}

static void port_tmr_delete(hal_tmr_id_t tmr_id)
{
     DeleteTimerQueueTimer(tmr_queue, (HANDLE)tmr_id, INVALID_HANDLE_VALUE);
}

static void port_tmr_init(void) 
{
    tmr_queue = CreateTimerQueue();
    assert(tmr_queue != NULL);
}

hal_tmr_driver_t HAL_TMR_DRIVER =
{
    .create = port_tmr_create,
    .delete = port_tmr_delete,
    .init = port_tmr_init
};



