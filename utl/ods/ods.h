/**
    ODS: On demand (state machine) scheduler

    Author: Marcelo Barros <marcelo.barros@ufu.br>
*/
#pragma once

#define ODS_MAX_ENTRIES      10
#define ODS_TICK_TIME_MS_DEF 50
#define ODS_TICK_TIME_MS_MIN 10

#define XMACRO_ODS_STATES \
    X(ODS_TASK_STATE_RUN,0,"RUN") \
    X(ODS_TASK_STATE_PAUSE,1,"PAUSE") \
    X(ODS_TASK_STATE_SLEEP,2,"SLEEP") \
    X(ODS_TASK_STATE_ADD,3,"ADD") \
    X(ODS_TASK_STATE_DEL,4,"DEL") \
    X(ODS_TASK_STATE_EVENT,5,"EVENT") \
    X(ODS_TASK_STATE_DONE,6,"DONE") 
    
typedef enum ods_task_event_mask_type_e
{
    ODS_TASK_EVENT_OR = 0,
    ODS_TASK_EVENT_AND
} ods_task_event_mask_type_t;

typedef enum ods_state_e 
{
#define X(ST,INDEX,STR) ST = INDEX,
    XMACRO_ODS_STATES
#undef X
} ods_task_state_t;

struct ods_ctx_s;
struct ods_task_ctx_s;

typedef void (*ods_task_exec_t)(struct ods_task_ctx_s *ctx);

typedef struct ods_task_ctx_s
{
    ods_task_state_t state;
    uint32_t exec_tick_counter;
    uint32_t sleep_tick_counter;
    struct event_s 
    {
        uint32_t events;
        uint32_t mask;
        ods_task_event_mask_type_t type;
        uint32_t event_tick_counter;
        bool timeout;
    } event;
    ods_task_exec_t func;
    void *arg;
    struct ods_ctx_s *ctx;
    struct ods_task_ctx_s *next;
} ods_task_ctx_t;

typedef struct ods_ctx_s
{
    ods_task_ctx_t *ods_exec_list;
    ods_task_ctx_t *ods_pre_exec_list;
    uint32_t ods_num_tasks;
    uint32_t ods_tick_time_ms;
    hal_tmr_args_t ods_tmr_args;
    hal_tmr_id_t ods_timer;
} ods_ctx_t;

void ods_init(ods_ctx_t *ctx);
void ods_start(ods_ctx_t *ctx, uint32_t tick_time_ms);
void ods_stop(ods_ctx_t *ctx);

bool ods_task_add(ods_ctx_t *ctx, ods_task_ctx_t *task_ctx);
void ods_task_del(ods_task_ctx_t *task_ctx);
void ods_task_pause(ods_task_ctx_t *task_ctx);
void ods_task_resume(ods_task_ctx_t *task_ctx);
void ods_task_sleep(ods_task_ctx_t *task_ctx, uint32_t time_ms);
void ods_task_done(ods_task_ctx_t *task_ctx);
ods_task_state_t ods_task_state_get(ods_task_ctx_t *task_ctx);
uint32_t ods_task_runtime_get_ms(ods_task_ctx_t *task_ctx);

void ods_task_event_set(ods_task_ctx_t *task_ctx, uint32_t event_mask, ods_task_event_mask_type_t type);
uint32_t ods_task_event_get(ods_task_ctx_t *task_ctx);
void ods_task_event_wait(ods_task_ctx_t *task_ctx, uint32_t event_mask, ods_task_event_mask_type_t type, uint32_t tmrout_ms);
bool ods_task_event_timed_out(ods_task_ctx_t *task_ctx);
