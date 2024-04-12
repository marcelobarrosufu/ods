#include <windows.h>

#include "hal.h"

// critical section for port_cpu_critical_section_enter/leave
static CRITICAL_SECTION port_cpu_cs;
static bool port_cpu_init_done = false;

BOOL WINAPI port_cpu_ctrlc_handler(DWORD event)
{
    switch (event)
    {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_PORT, "CTRL+C pressed!\n");
        exit(1);
        //app_terminate();
        return TRUE;

    default:
        return FALSE;
    }
}

static uint32_t port_cpu_critical_section_enter(void)
{
    if (port_cpu_init_done == false)
        return 0;

    EnterCriticalSection(&port_cpu_cs);

    return 0;
}

static void port_cpu_critical_section_leave(uint32_t state)
{
    if (port_cpu_init_done == false)
        return;

    LeaveCriticalSection(&port_cpu_cs);
}

static void port_cpu_sleep(uint32_t tmr_ms)
{
    Sleep(tmr_ms);
}

static void port_cpu_init(void)
{
    srand(time(NULL));

    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_PORT, "Creating critical section (interrupts enable/disable)\n");
    InitializeCriticalSection(&port_cpu_cs);

    if (!SetConsoleCtrlHandler(port_cpu_ctrlc_handler, TRUE)) 
    {
        UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_PORT, "CTRL+C handler installation failed!\n");
    }

    port_cpu_init_done = true;
}

static void port_cpu_deinit(void)
{
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_PORT, "Deleting critical section (interrupts enable/disable)\n");
    DeleteCriticalSection(&port_cpu_cs);
}

hal_cpu_driver_t HAL_CPU_DRIVER =
    {
        .critical_section_enter = port_cpu_critical_section_enter,
        .critical_section_leave = port_cpu_critical_section_leave,
        .sleep_ms = port_cpu_sleep,
        .init = port_cpu_init,
        .deinit = port_cpu_deinit,
};
