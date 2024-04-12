
#include "hal.h"

static hal_cpu_driver_t *drv = &HAL_CPU_DRIVER;

uint32_t hal_cpu_critical_section_enter(void)
{
    return drv->critical_section_enter();
}

void hal_cpu_critical_section_leave(uint32_t state)
{
    drv->critical_section_leave(state);
}

void hal_cpu_sleep_ms(uint32_t tmr_ms)
{
    drv->sleep_ms(tmr_ms);
}

void hal_cpu_reset(void)
{
    drv->reset();
}

void hal_cpu_init(void)
{
    drv->init();
}

void hal_cpu_deinit(void)
{
    drv->deinit();
}
