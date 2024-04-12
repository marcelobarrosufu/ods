#include "hal.h"
#include "ods.h"

typedef enum tsk_state_e
{
    TSK_STATE_IDLE = 0,
    TSK_STATE_EXEC,
    TSK_STATE_SEND,
    TSK_STATE_RECV,
    TSK_STATE_DONE
} tsk_state_t;

struct tsk_ctrl_s
{
    tsk_state_t state;
    uint32_t counter;
    uint32_t noper;
    struct ods_task_ctx_s *peer;
};

static void tsk_exec(struct ods_task_ctx_s *tsk_ctx)
{
    UTL_DBG_PRINTFL(UTL_DBG_LEVEL_APP,"        task %p:%p runtime %u (%s)\n",tsk_ctx->ctx,tsk_ctx,ods_task_runtime_get_ms(tsk_ctx),__func__);
}

static void tsk_consumer(struct ods_task_ctx_s *tsk_ctx)
{
    struct tsk_ctrl_s *ctrl = (struct tsk_ctrl_s *)tsk_ctx->arg;

    tsk_state_t last = ctrl->state;

    switch(ctrl->state)
    {
        case TSK_STATE_IDLE:
            ctrl->counter = 0;
            ctrl->noper = 0;
            ctrl->state = TSK_STATE_RECV;
            break;
        case TSK_STATE_RECV:
            UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"RECV: waiting event\n");
            ctrl->counter = 0;
            ctrl->state = TSK_STATE_EXEC;
            ods_task_event_wait(tsk_ctx,0x01,ODS_TASK_EVENT_AND,1000);
            break;
        case TSK_STATE_EXEC:
            if(ods_task_event_timed_out(tsk_ctx))
                UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"RECV: event timeout\n");
            else
                UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"RECV: event received\n");
            if(++ctrl->noper > 2)
                ctrl->state = TSK_STATE_DONE;
            else
                ctrl->state = TSK_STATE_RECV;
            break;
        case TSK_STATE_DONE:
            ctrl->counter = 0;
            ctrl->noper = 0;
            break;
        default:
            break;
    }

    UTL_DBG_PRINTFL(UTL_DBG_LEVEL_APP,"CONS: %u -> %u\n",last,ctrl->state);
}

static void tsk_producer(struct ods_task_ctx_s *tsk_ctx)
{
    struct tsk_ctrl_s *ctrl = (struct tsk_ctrl_s *)tsk_ctx->arg;

    tsk_state_t last = ctrl->state;

    switch(ctrl->state)
    {
        case TSK_STATE_IDLE:
            ctrl->counter = 0;
            ctrl->noper = 0;
            ctrl->state = TSK_STATE_EXEC;
            break;
        case TSK_STATE_EXEC:
            if(++ctrl->counter > 3)
            {
                ctrl->counter = 0;
                ctrl->state = TSK_STATE_SEND;
            }
            break;
        case TSK_STATE_SEND:
            if(++ctrl->noper > 2)
                ctrl->state = TSK_STATE_DONE;
            else
            {
                UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"PROD: event sent\n");
                ods_task_event_set(ctrl->peer,0x01,ODS_TASK_EVENT_OR);
                ctrl->state = TSK_STATE_EXEC;
            }
            break;
        case TSK_STATE_DONE:
            ctrl->counter = 0;
            ctrl->noper = 0;
            break;
        default:
            break;
    }

    UTL_DBG_PRINTFL(UTL_DBG_LEVEL_APP,"PROD: %u -> %u\n",last,ctrl->state);
}

int main(int argc, char *argv[])
{
    hal_init();

    ods_ctx_t sch1_ctx;
    ods_ctx_t sch2_ctx;
    ods_task_ctx_t tsk1_ctx = { .func = tsk_exec, .arg = NULL };
    ods_task_ctx_t tsk2_ctk = { .func = tsk_exec, .arg = NULL };
    ods_task_ctx_t tsk3_ctx = { .func = tsk_exec, .arg = NULL };
    ods_task_ctx_t tsk4_ctk = { .func = tsk_exec, .arg = NULL };



    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"---------========1=========---------\n");
    ods_init(&sch1_ctx);        
    ods_task_add(&sch1_ctx,&tsk1_ctx);
    ods_task_add(&sch1_ctx,&tsk2_ctk);
    ods_start(&sch1_ctx,500);

    hal_cpu_sleep_ms(5000);
    ods_task_del(&tsk1_ctx);
    hal_cpu_sleep_ms(2000);
    ods_task_done(&tsk2_ctk);
    hal_cpu_sleep_ms(1000);
    ods_task_del(&tsk2_ctk);
    hal_cpu_sleep_ms(2000);
    ods_stop(&sch1_ctx);

    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"---------========2=========---------\n");
    ods_init(&sch1_ctx);        
    ods_init(&sch2_ctx);        
    ods_task_add(&sch1_ctx,&tsk1_ctx);
    ods_task_add(&sch1_ctx,&tsk2_ctk);
    ods_task_add(&sch2_ctx,&tsk3_ctx);
    ods_task_add(&sch2_ctx,&tsk4_ctk);
    ods_start(&sch1_ctx,250);
    ods_start(&sch2_ctx,500);

    hal_cpu_sleep_ms(2100);
    ods_stop(&sch1_ctx);
    ods_stop(&sch2_ctx);
    hal_cpu_sleep_ms(1000);

    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_APP,"---------========3=========---------\n");
    struct tsk_ctrl_s ctrl_prod = { .state = TSK_STATE_IDLE, .counter = 0, .noper = 0 };
    struct tsk_ctrl_s ctrl_cons = { .state = TSK_STATE_IDLE, .counter = 0, .noper = 0 };

    ods_task_ctx_t tsk_prod_ctx = { .func = tsk_producer, .arg = (void *) &ctrl_prod };
    ods_task_ctx_t tsk_cons_ctx = { .func = tsk_consumer, .arg = (void *) &ctrl_cons };
    
    ctrl_prod.peer = &tsk_cons_ctx;
    ctrl_cons.peer = &tsk_prod_ctx;
    
    ods_init(&sch1_ctx);     
    ods_task_add(&sch1_ctx,&tsk_prod_ctx);
    ods_task_add(&sch1_ctx,&tsk_cons_ctx);
    ods_start(&sch1_ctx,250);
    
    while(ctrl_prod.state != TSK_STATE_DONE || ctrl_cons.state != TSK_STATE_DONE)
        hal_cpu_sleep_ms(250);

    ods_stop(&sch1_ctx);



    return 0;
}