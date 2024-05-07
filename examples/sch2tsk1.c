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

    // create two context for schedulers
    ods_ctx_t sch_ctx1;
    ods_ctx_t sch_ctx2;

    // create context for tasks (no arguments provided, in this case)
    ods_task_ctx_t tsk1_ctx = { .func = tsk_a_exec, .arg = NULL };
    ods_task_ctx_t tsk2_ctx = { .func = tsk_b_exec, .arg = NULL };

    // initialize each scheduler
    ods_init(&sch_ctx1);
    ods_init(&sch_ctx2);

    // add one tasks to each scheduler    
    ods_task_add(&sch_ctx1,&tsk1_ctx);
    ods_task_add(&sch_ctx2,&tsk2_ctx);
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"Starting schedulers...\n");

    // start scheduler 1 with 100ms tick time
    ods_start(&sch_ctx1,100);

    // start scheduler 2 with 50ms tick time
    ods_start(&sch_ctx2,50);

    // wait schedular execution for 1 second
    hal_cpu_sleep_ms(1000);

    // stop schedulers
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"Stopping schedulers...\n");
    ods_stop(&sch_ctx1);
    ods_stop(&sch_ctx2);

    return 0;
}
