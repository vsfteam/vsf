/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/
#ifndef __VSF_EDA_H__
#define __VSF_EDA_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#include "./__class_eda.h"


/*============================ MACROS ========================================*/

#define VSF_SYNC_AUTO_RST               0x0000
#define VSF_SYNC_MANUAL_RST             0x8000
#define VSF_SYNC_HAS_OWNER              0x8000

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_eda_yield()         vsf_eda_post_evt(peda, VSF_EVT_NONE);

// SEMAPHORE
#define vsf_eda_sem_init(__psem, __cnt)                                         \
            vsf_eda_sync_init((__psem), (__cnt), 0x7FFF | VSF_SYNC_AUTO_RST)
#define vsf_eda_sem_post(__psem)            vsf_eda_sync_increase((__psem))
#define vsf_eda_sem_pend(__psem, __timeout) vsf_eda_sync_decrease((__psem), (__timeout))

// MUTEX
#define vsf_eda_mutex_init(__pmtx)                                              \
            vsf_eda_sync_init(  &((__pmtx)->use_as__vsf_sync_t),                \
                                1 | VSF_SYNC_HAS_OWNER,                         \
                                1 | VSF_SYNC_AUTO_RST)
#define vsf_eda_mutex_enter(__pmtx, __timeout)                                  \
            vsf_eda_sync_decrease(&((__pmtx)->use_as__vsf_sync_t), (__timeout))
#define vsf_eda_mutex_leave(__pmtx)                                             \
            vsf_eda_sync_increase(&((__pmtx)->use_as__vsf_sync_t))

// EVENT
#define vsf_eda_event_init(__pevt, __set, __auto_rst)                           \
            vsf_eda_sync_init(                                                  \
                    (__pevt),                                                   \
                    (__set),                                                    \
                    1 | ((__auto_rst) ? VSF_SYNC_AUTO_RST : VSF_SYNC_MANUAL_RST))
#define vsf_eda_event_set(__pevt)           vsf_eda_sync_increase((__pevt))
#define vsf_eda_event_reset(__pevt)         vsf_eda_sync_decrease((__pevt), false)
#define vsf_eda_event_wait(__pevt, __timeout)                                   \
            vsf_eda_sync_decrease((__pevt), (__timeout))

/*============================ TYPES =========================================*/

enum {
    VSF_EVT_INVALID = -1,               //!< compatible with fsm_rt_err
    VSF_EVT_CPL = 0,                    //!< compatible with fsm_rt_cpl
    VSF_EVT_NONE = 1,                   //!< compatible with fsm_rt_on_going
    
    VSF_EVT_SYSTEM = 0x100,
    VSF_EVT_DUMMY = VSF_EVT_SYSTEM + 0,
    VSF_EVT_INIT = VSF_EVT_SYSTEM + 1,
    VSF_EVT_FINI = VSF_EVT_SYSTEM + 2,

    VSF_EVT_ENTER = VSF_EVT_SYSTEM + 3,
    VSF_EVT_EXIT = VSF_EVT_SYSTEM + 4,

    // events for time
    VSF_EVT_TIMER = VSF_EVT_SYSTEM + 5,

    // events for sync
    VSF_EVT_SYNC = VSF_EVT_SYSTEM + 6,
    VSF_EVT_SYNC_CANCEL = VSF_EVT_SYSTEM + 7,
    VSF_EVT_SYNC_POLL = VSF_EVT_SYSTEM + 8,

    // events for message
    VSF_EVT_MESSAGE = VSF_EVT_SYSTEM + 9,

    VSF_EVT_USER = VSF_EVT_SYSTEM + 10,
};

// IPC
#if VSF_CFG_SYNC_EN == ENABLED
enum vsf_sync_reason_t {
    VSF_SYNC_FAIL,
    VSF_SYNC_TIMEOUT,
    VSF_SYNC_PENDING,
    VSF_SYNC_GET,
    VSF_SYNC_CANCEL,
};
typedef enum vsf_sync_reason_t vsf_sync_reason_t;

typedef vsf_sync_t          vsf_sem_t;
typedef vsf_sync_t          vsf_event_t;
typedef vsf_sync_owner_t    vsf_mutex_t;

struct vsf_bmpevt_adapter_sync_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_sync_t *psync;
};
typedef struct vsf_bmpevt_adapter_sync_t vsf_bmpevt_adapter_sync_t;

struct vsf_bmpevt_adapter_bmpevt_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_bmpevt_t *pbmpevt;
    vsf_bmpevt_pender_t pender;
};
typedef struct vsf_bmpevt_adapter_bmpevt_t vsf_bmpevt_adapter_bmpevt_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_CFG_TIMER_EN == ENABLED
extern uint_fast32_t vsf_timer_get_tick(void);
extern uint_fast32_t vsf_timer_get_duration(uint_fast32_t from_time, uint_fast32_t to_time);
extern uint_fast32_t vsf_timer_get_elapsed(uint_fast32_t from_time);
#endif

extern vsf_err_t vsf_eda_init(  vsf_eda_t *pthis, 
                                vsf_priority_t priotiry, 
                                bool is_stack_owner);
extern vsf_err_t vsf_eda_fini(vsf_eda_t *pthis);
extern vsf_eda_t *vsf_eda_get_cur(void);
extern vsf_evt_t vsf_eda_get_cur_evt(void);
extern void *vsf_eda_get_cur_msg(void);
extern bool vsf_eda_is_stack_owner(vsf_eda_t *pthis);

#if VSF_CFG_TIMER_EN == ENABLED
extern vsf_err_t vsf_teda_init(vsf_teda_t *pthis, 
                        vsf_priority_t priority, 
                        bool is_stack_owner);
extern vsf_err_t vsf_teda_fini(vsf_teda_t *pthis);
extern vsf_err_t vsf_teda_set_timer(uint_fast32_t tick);
extern vsf_err_t vsf_teda_cancel_timer(vsf_teda_t *pthis);
#endif

#ifdef VSF_CFG_EVTQ_LIST
extern vsf_err_t vsf_eda_set_priority(vsf_eda_t *pthis, vsf_priority_t priority);
#endif

extern vsf_err_t vsf_eda_post_evt(vsf_eda_t *pthis, vsf_evt_t evt);
extern vsf_err_t vsf_eda_post_msg(vsf_eda_t *pthis, void *pmsg);
extern vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *pmsg);

#if VSF_CFG_SYNC_EN == ENABLED
extern vsf_err_t vsf_eda_sync_init(vsf_sync_t *pthis, uint_fast16_t cur_value,
        uint_fast16_t max_value);
extern vsf_err_t vsf_eda_sync_increase(vsf_sync_t *pthis);
extern vsf_err_t vsf_eda_sync_decrease(vsf_sync_t *pthis, int_fast32_t timeout);
extern void vsf_eda_sync_cancel(vsf_sync_t *pthis);
extern vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *pthis, vsf_evt_t evt);

#if VSF_CFG_BMPEVT_EN == ENABLED
extern const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_sync_op;
extern const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_bmpevt_op;


extern vsf_err_t vsf_eda_bmpevt_init(vsf_bmpevt_t *pthis, uint_fast8_t adapter_count);
extern vsf_err_t vsf_eda_bmpevt_set(vsf_bmpevt_t *pthis, uint_fast32_t mask);
extern vsf_err_t vsf_eda_bmpevt_reset(vsf_bmpevt_t *pthis, uint_fast32_t mask);
extern vsf_err_t vsf_eda_bmpevt_cancel(vsf_bmpevt_t *pthis, uint_fast32_t mask);
extern vsf_err_t vsf_eda_bmpevt_pend(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *ppender, int_fast32_t timeout);
extern vsf_sync_reason_t vsf_eda_bmpevt_poll(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *ppender, vsf_evt_t evt);
#endif

#if VSF_CFG_QUEUE_EN == ENABLED
extern vsf_err_t vsf_eda_queue_init(vsf_queue_t *pthis, uint_fast16_t max);
extern vsf_err_t vsf_eda_queue_send(vsf_queue_t *pthis, vsf_slist_t *node);
extern vsf_err_t vsf_eda_queue_recv(vsf_queue_t *pthis, vsf_slist_t **ppnode, int_fast32_t timeout);
extern vsf_sync_reason_t vsf_eda_queue_get_reason(vsf_queue_t *pthis, vsf_evt_t evt, vsf_slist_t **ppnode);
#endif      // VSF_CFG_QUEUE_EN

#endif      // VSF_CFG_SYNC_EN

#endif      // __VSF_EDA_H__
