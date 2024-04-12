#include "hal.h"

void hal_deinit(void)
{
}

void hal_init(void)
{
    utl_dbg_init();
    hal_rtc_init();
    hal_cpu_init();
    hal_tmr_init();
}
