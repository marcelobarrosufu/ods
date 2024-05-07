#include "hal.h"
#include "ods.h"

static void tsk_exec(struct ods_task_ctx_s *tsk_ctx)
{
    uint32_t sleep_ms = *((uint32_t *)tsk_ctx->arg);

    ods_task_sleep(tsk_ctx,sleep_ms);
    // WARNING:
    // take care that the current task only will effectivelly sleep after 
    // return from task function ! If you want an immediate sleep, you must
    // return from task after calling ods_task_sleep() function.
    UTL_DBG_PRINTFL(UTL_DBG_LEVEL_APP,"Task will sleep %u ms\n",sleep_ms);
}

int main(int argc, char *argv[])
{
    hal_init();

    // create context for scheduler
    ods_ctx_t sch_ctx;

    // create context for tasks (arguments provided)
    static uint32_t sleep_time1 = 200;
    static uint32_t sleep_time2 = 250;
    ods_task_ctx_t tsk1_ctx = { .func = tsk_exec, .arg = &sleep_time1 };
    ods_task_ctx_t tsk2_ctx = { .func = tsk_exec, .arg = &sleep_time2 };

    // initialize scheduler
    ods_init(&sch_ctx);

    // add tasks to scheduler    
    ods_task_add(&sch_ctx,&tsk1_ctx);
    ods_task_add(&sch_ctx,&tsk2_ctx);
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"Starting scheduler...\n");

    // start scheduler with 50ms tick time
    ods_start(&sch_ctx,50);

    // wait schedular execution for 1 second
    hal_cpu_sleep_ms(1000);

    // stop schedulers
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"Stopping scheduler...\n");
    ods_stop(&sch_ctx);

    return 0;
}
