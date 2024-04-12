#include "hal.h"
#include "ods.h"

static void tsk_a_exec(struct ods_task_ctx_s *tsk_ctx)
{
    UTL_DBG_PRINTFL(UTL_DBG_LEVEL_APP,"Running A task %s...\n",__func__);
}

static void tsk_b_exec(struct ods_task_ctx_s *tsk_ctx)
{
    UTL_DBG_PRINTFL(UTL_DBG_LEVEL_APP,"Running B task %s...\n",__func__);
}

int main(int argc, char *argv[])
{
    hal_init();

    // create context for scheduler
    ods_ctx_t sch_ctx;
    // create context for tasks (no arguments provided, in this case)
    ods_task_ctx_t tsk1_ctx = { .func = tsk_a_exec, .arg = NULL };
    ods_task_ctx_t tsk2_ctk = { .func = tsk_b_exec, .arg = NULL };

    // initialize scheduler
    ods_init(&sch_ctx);    
    // add tasks to scheduler    
    ods_task_add(&sch_ctx,&tsk1_ctx);
    ods_task_add(&sch_ctx,&tsk2_ctk);
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"Starting scheduler...\n");
    // start scheduler with 50ms tick time
    ods_start(&sch_ctx,100);

    // wait schedular execution for 5 seconds
    hal_cpu_sleep_ms(1000);

    // stop scheduler
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"Stopping scheduler...\n");
    ods_stop(&sch_ctx);

    return 0;
}
