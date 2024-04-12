#include "hal.h"

#define USE_CRITICAL_SECTION 1

static const char *log_modules[] = 
{
#define X(MOD,INDEX) #MOD,
    XMACRO_DBG_MODULES
#undef X
};

static uint32_t utl_dbg_modules_activated = 0;

void utl_dbg_module_enable(utl_dbg_modules_t mod_idx)
{
    utl_dbg_modules_activated |= 1 << mod_idx;
}

void utl_dbg_module_disable(utl_dbg_modules_t mod_idx)
{
    utl_dbg_modules_activated &= ~((uint32_t)(1 << mod_idx));
}

bool utl_dbg_module_is_enabled(utl_dbg_modules_t mod_idx)
{
    return (utl_dbg_modules_activated & (1 << mod_idx)) > 0;
}

void utl_dbg_printfl(utl_dbg_modules_t mod_idx, const char *fmt, ...)
{
    if(utl_dbg_module_is_enabled(mod_idx))
    {
        va_list arg;
        va_start(arg, fmt);

        hal_rtc_date_time_t dt = { 0 };
        hal_rtc_get(&dt);

#if USE_CRITICAL_SECTION == 1
        uint32_t status = hal_cpu_critical_section_enter();
#endif        
        utl_printf("[%*.*s] %04u-%02u-%02uT%02u:%02u:%02u ",3,3,&log_modules[mod_idx][14],dt.year,dt.month,dt.day,dt.hour,dt.minute,dt.second);
        utl_vprintf(fmt,arg);
#if USE_CRITICAL_SECTION == 1
        hal_cpu_critical_section_leave(status);
#endif        

        va_end(arg);
    }
}

void utl_dbg_printfsl(utl_dbg_modules_t mod_idx, const char *fmt)
{
    if(utl_dbg_module_is_enabled(mod_idx))
    {
        hal_rtc_date_time_t dt = { 0 };
        hal_rtc_get(&dt);

#if USE_CRITICAL_SECTION == 1
        uint32_t status = hal_cpu_critical_section_enter();
#endif        
        utl_printf("[%*.*s] %04u-%02u-%02uT%02u:%02u:%02u ",3,3,&log_modules[mod_idx][14],dt.year,dt.month,dt.day,dt.hour,dt.minute,dt.second);
        utl_printf("%s",fmt);
#if USE_CRITICAL_SECTION == 1
        hal_cpu_critical_section_leave(status);
#endif        
    }
}

static void utl_dbg_dump_lines(char *stamp, uint8_t *data, uint16_t size)
{
    uint8_t *ptr = data;

    utl_printf("%s",stamp);

    for(uint32_t pos = 0 ; pos < size ; pos++)
    {
        if(pos && (pos % 32 == 0))
            utl_printf("\n%s",stamp);

        if(pos % 32 == 0)
            utl_printf("%04X ",(unsigned int)pos);

        utl_printf("%02X",*ptr++);
    }
    utl_printf("\n");
}

void utl_dbg_dump(utl_dbg_modules_t mod_idx, uint8_t *data, uint32_t size)
{
    if(utl_dbg_module_is_enabled(mod_idx))
    {
        hal_rtc_date_time_t dt = { 0 };
        hal_rtc_get(&dt);
        char stamp[64] = { 0 };

#if USE_CRITICAL_SECTION == 1
        uint32_t status = hal_cpu_critical_section_enter();
#endif        
        sprintf(stamp,"[%s] %04u-%02u-%02uT%02u:%02u:%02u ",&log_modules[mod_idx][14],dt.year,dt.month,dt.day,dt.hour,dt.minute,dt.second);
        utl_dbg_dump_lines(stamp,data,size);
#if USE_CRITICAL_SECTION == 1
        hal_cpu_critical_section_leave(status);
#endif        
    }
}

void utl_dbg_init(void)
{
    utl_dbg_module_enable(UTL_DBG_LEVEL_APP);
    utl_dbg_module_enable(UTL_DBG_LEVEL_PORT);
    utl_dbg_module_enable(UTL_DBG_LEVEL_ODS);
}
