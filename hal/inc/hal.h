#pragma once

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdalign.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "utl_printf.h"
#include "utl_dbg.h"

//#include "hal_cfg.h"
#include "hal_rtc.h"
#include "hal_tmr.h"
#include "hal_cpu.h"

#if defined(__GNUC__)
#define __WEAK __attribute__((weak))
#define __UNUSED __attribute__((unused))
#else
#define __WEAK
#define __UNUSED
#endif

extern hal_rtc_driver_t HAL_RTC_DRIVER;
extern hal_tmr_driver_t HAL_TMR_DRIVER;
extern hal_cpu_driver_t HAL_CPU_DRIVER;

void hal_init(void);
void hal_deinit(void);
