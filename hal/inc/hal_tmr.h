
#pragma once

typedef void *hal_tmr_id_t;

typedef void (*hal_tmr_cbk_t)(void *arg);
typedef struct hal_tmr_args_s
{
    hal_tmr_cbk_t cbk;
    void *arg;
} hal_tmr_args_t;

typedef struct hal_tmr_driver_s
{
    hal_tmr_id_t (*create)(uint64_t expiration_ms, bool cyclic, hal_tmr_args_t *args);
    void (*delete)(hal_tmr_id_t tmr_id);
    void (*init)(void);
} hal_tmr_driver_t;

hal_tmr_id_t hal_tmr_create(uint64_t expiration_ms, bool cyclic, hal_tmr_args_t *args);
void hal_tmr_delete(hal_tmr_id_t tmr_id);
void hal_tmr_init(void);