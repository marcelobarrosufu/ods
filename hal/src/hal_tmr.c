#include "hal.h"

static hal_tmr_driver_t *drv = &HAL_TMR_DRIVER;

hal_tmr_id_t hal_tmr_create(uint64_t expiration_ms, bool cyclic, hal_tmr_args_t *args)
{
    return drv->create(expiration_ms, cyclic, args);
}

void hal_tmr_delete(hal_tmr_id_t tmr_id)
{
    drv->delete(tmr_id);
}

void hal_tmr_init(void)
{
    drv->init();
}