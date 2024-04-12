#pragma once

#define HAL_CPU_ID_SIZE 12

typedef struct hal_cpu_driver_s
{
    uint32_t (*critical_section_enter)(void);
    void (*critical_section_leave)(uint32_t state);
    void (*sleep_ms)(uint32_t tmr_ms);
    void (*init)(void);
    void (*deinit)(void);
} hal_cpu_driver_t;


uint32_t hal_cpu_critical_section_enter(void);
void hal_cpu_critical_section_leave(uint32_t state);
void hal_cpu_sleep_ms(uint32_t tmr_ms);
void hal_cpu_init(void);
void hal_cpu_deinit(void);


