#include "hal.h"
#include "ods.h"

static const char *ods_state_str[] = 
{
#define X(ST,INDEX,STR) STR,
    XMACRO_ODS_STATES
#undef X
};

static bool ods_task_event_check(ods_task_ctx_t *task_ctx);

static void ods_list_print(ods_task_ctx_t *head)
{
    ods_task_ctx_t *current = head;

    while(current != NULL)
    {
        UTL_DBG_PRINTFL(UTL_DBG_LEVEL_ODS,"%p ->\n",current);
        current = current->next;
    }
    UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_ODS,"0\n\n");
}

static bool ods_list_add(ods_task_ctx_t **head, ods_task_ctx_t *item)
{
    item->next = *head;
    *head = item;

    return true;
}

static bool ods_list_del(ods_task_ctx_t **head, ods_task_ctx_t *item)
{
    ods_task_ctx_t *current = *head;

    if(current == NULL) 
    {
        UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_ODS,"Empty list\n");
        return false;
    }

    if(current == item)
    {
        UTL_DBG_PRINTFL(UTL_DBG_LEVEL_ODS,"First element removed %p, new next %p\n",item,item->next);
        *head = item->next;
        return true;
    }

    while (current->next != NULL)
    {
        if(current->next == item) 
            break;

        current = current->next;
    }
    
    if(current->next == NULL) 
    {
        UTL_DBG_PRINTFSL(UTL_DBG_LEVEL_ODS,"Not found\n");
        return false; 
    }

    current->next = current->next->next;

    return true;
}

static void ods_pre_exec(ods_ctx_t *ctx)
{
    bool changed = false;
    ods_task_ctx_t *item = ctx->ods_pre_exec_list;

    while(item != NULL)
    {
        ods_task_ctx_t *saved_item = item->next;

        if(ods_list_add(&ctx->ods_exec_list,item))
        {
            item->exec_tick_counter = 0;
            item->sleep_tick_counter = 0;
            memset(&item->event,0,sizeof(item->event));
            item->state = ODS_TASK_STATE_RUN;
            ctx->ods_num_tasks++;
            changed = true;
        }

        item = saved_item;
    }

    if(changed)
    {
        UTL_DBG_PRINTFL(UTL_DBG_LEVEL_ODS,"Tasks in ctx %p:\n",ctx);
        ods_list_print(ctx->ods_exec_list);
    }

    ctx->ods_pre_exec_list = NULL;
}

static void ods_post_exec(ods_ctx_t *ctx)
{
    ods_task_ctx_t *item = ctx->ods_exec_list;

    while(item != NULL)
    {
        if(item->state == ODS_TASK_STATE_DEL)
        {
            if(ods_list_del(&ctx->ods_exec_list,item))
            {
                UTL_DBG_PRINTFL(UTL_DBG_LEVEL_ODS,"Removing task %p. ctx %p from exec list\n",item, ctx);
                ctx->ods_num_tasks--;
                ods_list_print(ctx->ods_exec_list);
            }
        }

        item = item->next;
    }
}

static void ods_exec(ods_ctx_t *ctx)
{
    uint32_t tsk_cnt = 0;
    ods_task_ctx_t *item = ctx->ods_exec_list;

    while(item != NULL)
    {
        if(item->state == ODS_TASK_STATE_SLEEP)
        {
            if(item->sleep_tick_counter > 0)
                item->sleep_tick_counter -= 1;

            if(item->sleep_tick_counter == 0)
                item->state = ODS_TASK_STATE_RUN;
        }

        if(item->state == ODS_TASK_STATE_EVENT)
        {
            if(ods_task_event_check(item))
            {
                item->event.timeout = false;
                item->state = ODS_TASK_STATE_RUN;
            }
            else
            {
                if(item->event.event_tick_counter > 0)
                    item->event.event_tick_counter -= 1;

                if(item->event.event_tick_counter == 0)
                {
                    item->event.timeout = true;
                    item->state = ODS_TASK_STATE_RUN;
                }
            }
        }

        UTL_DBG_PRINTFL(UTL_DBG_LEVEL_ODS,"[%02u/%02u] task %p:%p, state %s, ticks %u, sleep ticks %u...\n",
                    ++tsk_cnt,
                    ctx->ods_num_tasks,
                    ctx,
                    item,
                    ods_state_str[item->state],
                    item->exec_tick_counter,
                    item->sleep_tick_counter);

        if(item->state == ODS_TASK_STATE_RUN)
        {
            if(item->func)
                item->func(item);
        }
        
        item->exec_tick_counter += 1;
        item = item->next;
    }
}

static void ods_process(void *arg)
{
    ods_ctx_t *ctx = (ods_ctx_t *) arg;
    
    ods_pre_exec(ctx);
    ods_exec(ctx);
    ods_post_exec(ctx);
}

void ods_init(ods_ctx_t *ctx)
{
    ctx->ods_tick_time_ms = ODS_TICK_TIME_MS_MIN;
    ctx->ods_exec_list = NULL;
    ctx->ods_pre_exec_list = NULL;
    ctx->ods_num_tasks = 0;
    ctx->ods_timer = NULL;
    ctx->ods_tmr_args.cbk = ods_process;
    ctx->ods_tmr_args.arg = (void *) ctx;
}

bool ods_task_add(ods_ctx_t *ctx, ods_task_ctx_t *task_ctx)
{
    bool ret = false;
    assert(ctx);
    assert(task_ctx);

    if(ods_list_add(&ctx->ods_pre_exec_list,task_ctx))
    {
        task_ctx->state = ODS_TASK_STATE_ADD;
        task_ctx->ctx = ctx;
        ret = true;
        UTL_DBG_PRINTFL(UTL_DBG_LEVEL_ODS,"Task %p:%p queued to be added\n",ctx,task_ctx);
    }

    return ret;
}

void ods_task_del(ods_task_ctx_t *task_ctx)
{
    assert(task_ctx);

    task_ctx->state = ODS_TASK_STATE_DEL;
    UTL_DBG_PRINTFL(UTL_DBG_LEVEL_ODS,"Task %p:%p queued to be removed\n",task_ctx->ctx,task_ctx);
}

void ods_start(ods_ctx_t *ctx, uint32_t tick_time_ms)
{
    ctx->ods_tick_time_ms = tick_time_ms < ODS_TICK_TIME_MS_MIN ? ODS_TICK_TIME_MS_MIN : tick_time_ms;
    ctx->ods_timer = hal_tmr_create(ctx->ods_tick_time_ms,true,&ctx->ods_tmr_args);
}

void ods_stop(ods_ctx_t *ctx)
{
    if(ctx->ods_timer)
    {
        hal_tmr_delete(ctx->ods_timer);
        ctx->ods_timer = NULL;
    }
}

uint32_t ods_task_runtime_get_ms(ods_task_ctx_t *task_ctx)
{
    return task_ctx->exec_tick_counter*task_ctx->ctx->ods_tick_time_ms;
}

void ods_task_sleep(ods_task_ctx_t *task_ctx, uint32_t time_ms)
{
    if(task_ctx->state == ODS_TASK_STATE_RUN)
    {
        task_ctx->state = ODS_TASK_STATE_SLEEP;

        uint32_t ticks = time_ms/task_ctx->ctx->ods_tick_time_ms;
        ticks = ticks == 0 ? 1 : ticks;
        task_ctx->sleep_tick_counter = ticks;
    }
}

void ods_task_pause(ods_task_ctx_t *task_ctx)
{
    if(task_ctx->state != ODS_TASK_STATE_DEL && task_ctx->state != ODS_TASK_STATE_ADD)
        task_ctx->state = ODS_TASK_STATE_PAUSE;
}

void ods_task_resume(ods_task_ctx_t *task_ctx)
{
    if(task_ctx->state != ODS_TASK_STATE_DEL && task_ctx->state != ODS_TASK_STATE_ADD)
        task_ctx->state = ODS_TASK_STATE_RUN;
}

void ods_task_done(ods_task_ctx_t *task_ctx)
{
    task_ctx->state = ODS_TASK_STATE_DONE;
}

ods_task_state_t ods_task_state_get(ods_task_ctx_t *task_ctx)
{
    return task_ctx->state;
}

void ods_task_event_set(ods_task_ctx_t *task_ctx, uint32_t event_mask, ods_task_event_mask_type_t type)
{
    if(type == ODS_TASK_EVENT_OR)
        task_ctx->event.events |= event_mask;
    else
        task_ctx->event.events &= event_mask;
}

void ods_task_event_wait(ods_task_ctx_t *task_ctx, uint32_t event_mask, ods_task_event_mask_type_t type, uint32_t tmrout_ms)
{
    task_ctx->event.mask = event_mask;
    task_ctx->event.type = type;
    task_ctx->event.timeout = false;
    task_ctx->event.event_tick_counter = tmrout_ms/task_ctx->ctx->ods_tick_time_ms;
    task_ctx->event.event_tick_counter = task_ctx->event.event_tick_counter == 0 ? 1 : task_ctx->event.event_tick_counter;
    task_ctx->state = ODS_TASK_STATE_EVENT;
}

static bool ods_task_event_check(ods_task_ctx_t *task_ctx)
{
    bool event_set = false;

    if(task_ctx->event.type == ODS_TASK_EVENT_OR)
        event_set = (task_ctx->event.events & task_ctx->event.mask) > 0;
    else
        event_set = (task_ctx->event.events & task_ctx->event.mask) == task_ctx->event.mask;

    if(event_set)
    {
        if(task_ctx->event.type == ODS_TASK_EVENT_OR)
            task_ctx->event.events &= ~(task_ctx->event.events & task_ctx->event.mask);
        else
            task_ctx->event.events = 0;
    }

    return event_set;
}

uint32_t ods_task_event_get(ods_task_ctx_t *task_ctx)
{
    return task_ctx->event.events;
}

bool ods_task_event_timed_out(ods_task_ctx_t *task_ctx)
{
    return task_ctx->event.timeout;
}