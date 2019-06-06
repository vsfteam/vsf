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

#include "service/vsf_service.h"
/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
   
#if     defined(__VSF_EDA_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#elif   defined(__VSF_EDA_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"


/*============================ MACROS ========================================*/

#define VSF_SYNC_AUTO_RST               0x0000
#define VSF_SYNC_MANUAL_RST             0x8000
#define VSF_SYNC_HAS_OWNER              0x8000

#define VSF_SYNC_MAX                    0x7FFF

/*============================ MACROFIED FUNCTIONS ===========================*/

// SEMAPHORE
#define vsf_eda_sem_init(__psem, __cnt)                                         \
            vsf_eda_sync_init((__psem), (__cnt), VSF_SYNC_MAX | VSF_SYNC_AUTO_RST)
#define vsf_eda_sem_post(__psem)            vsf_eda_sync_increase((__psem))
#define vsf_eda_sem_pend(__psem, __timeout) vsf_eda_sync_decrease((__psem), (__timeout))

// CRIT
#define vsf_eda_crit_init(__pcrit)                                              \
            vsf_eda_sync_init((__pcrit), 1, 1 | VSF_SYNC_AUTO_RST)
#define vsf_eda_crit_enter(__pcrit, __timeout)                                  \
            vsf_eda_sync_decrease((__pcrit), (__timeout))
#define vsf_eda_crit_leave(__pcrit)                                             \
            vsf_eda_sync_increase((__pcrit))

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

typedef vsf_systimer_cnt_t     vsf_timer_tick_t;

enum {
    VSF_EVT_INVALID = -1,               //!< compatible with fsm_rt_err
    VSF_EVT_NONE = 0,                   //!< compatible with fsm_rt_cpl
    VSF_EVT_RETURN = 0,                 
    VSF_EVT_YIELD = 1,                  //!< compatible with fsm_rt_on_going

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

declare_simple_class(vsf_eda_t)
declare_simple_class(vsf_teda_t)
declare_simple_class(vsf_sync_t)
declare_simple_class(vsf_sync_owner_t)
declare_simple_class(vsf_bmpevt_t)
declare_simple_class(vsf_bmpevt_pender_t)
declare_simple_class(vsf_bmpevt_adapter_t)
declare_simple_class(vsf_bmpevt_adapter_eda_t)
declare_simple_class(vsf_queue_t)
declare_simple_class(vsf_callback_timer_t)

typedef uint16_t vsf_evt_t;

typedef struct vsf_eda_frame_t vsf_eda_frame_t;

typedef void (*vsf_eda_evthandler_t)(vsf_eda_t *eda, vsf_evt_t evt);
typedef void (*vsf_eda_on_terminate_t)(vsf_eda_t *eda);
typedef fsm_rt_t (*vsf_fsm_entry_t)(vsf_eda_frame_t *frame, vsf_evt_t evt);
typedef void (*vsf_param_eda_evthandler_t)(vsf_eda_frame_t *frame, vsf_evt_t evt);

#if VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
struct vsf_eda_frame_t {
    implement(vsf_slist_node_t)
    union {
        void                            *func;
        vsf_eda_evthandler_t            evthandler;
        vsf_param_eda_evthandler_t      param_evthandler;
        vsf_fsm_entry_t                 fsm_entry;
    };
    
#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    union {
        struct {
            uint32_t        : 8;        //!< reserved for state
            uint32_t        : 23;       //!< reserved for future
            uint32_t is_fsm : 1;
        };
        uint8_t  state;
        uint32_t flag;
    };
#   else
        uint32_t state;
#   endif
    union {
        void *param;
        void *target;
    };
};


declare_vsf_pool(vsf_eda_frame_pool)
def_vsf_pool(vsf_eda_frame_pool, vsf_eda_frame_t)

#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
struct vsf_eda_cfg_t {
    union {
        void                    *func;
        vsf_eda_evthandler_t    evthandler;
        vsf_fsm_entry_t         fsm_entry;
    };
    vsf_priority_t priority;
    void *target;
    bool is_fsm;
    bool is_stack_owner;
};
typedef struct vsf_eda_cfg_t vsf_eda_cfg_t;
#endif

//! \name eda
//! @{
def_simple_class(vsf_eda_t) {

    public_member(
        // you can add public member here
        union {
            vsf_eda_evthandler_t    evthandler;
        #if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
            vsf_slist_t             frame_list;
            vsf_eda_frame_t         *frame;
        #endif
        };
    #ifdef VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE
        vsf_eda_on_terminate_t  on_terminate;
    #endif
    )

    protected_member(
    #if VSF_CFG_SYNC_EN == ENABLED
        vsf_dlist_node_t    pending_node;
    #endif
    
    #if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        fsm_rt_t            fsm_return_state;
    #endif

    #if VSF_CFG_PREMPT_EN == ENABLED
    #   if VSF_CFG_DYNAMIC_PRIOTIRY_EN
        vsf_dlist_node_t    rdy_node;
        vsf_slist_queue_t   evt_list;
        uint8_t             cur_priority;
        uint8_t             new_priority;
        uint8_t             priority;

        union {
            struct {
                uint8_t     is_ready        : 1;
                uint8_t     is_new_prio     : 1;
    #   else
        uint8_t             evt_cnt;
        union {
            uint8_t         priority;
            uint8_t         cur_priority;
        };
        union {
            struct {
    #   endif
            uint8_t         is_to_exit      : 1;
    #else
        uint32_t            evt_pending;
        union {
            struct {
                uint8_t     is_processing   : 1;
    #endif

    #   if VSF_CFG_SYNC_EN == ENABLED
                /* if limitted is set, eda can only receive 1 event */
                uint8_t     is_limitted     : 1;
                uint8_t     is_sync_got     : 1;
    #   endif

    #   if VSF_CFG_TIMER_EN == ENABLED
                /* has_timer and timed is used in teda */
                uint8_t     is_timed        : 1;
    #   endif
    #   if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
                uint8_t     is_use_frame    : 1;
    #       if      VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                   \
                ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED 
                uint8_t     is_evt_incoming : 1;        
    #       endif
    #   endif
    #   if VSF_USE_SIMPLE_SHELL == ENABLED
                uint8_t     polling_state   : 1; 
                uint8_t     is_stack_owner  : 1;
            };
            uint16_t        flag;                       
    #   else
            };
            uint8_t         flag;
    #   endif
        };
    )
};
//! @}

#if VSF_CFG_TIMER_EN == ENABLED
//! \name timed eda
//! @{
def_simple_class(vsf_teda_t)  {
    which(
        implement(vsf_eda_t)
    )
    private_member(
        vsf_dlist_node_t    timer_node;
        vsf_timer_tick_t    due;
    );
};
//! @}

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
//! \name callback_timer
//! @{
def_simple_class(vsf_callback_timer_t) {
    public_member(
        void (*on_timer)(vsf_callback_timer_t *timer);
    )
    private_member(
        vsf_dlist_node_t timer_node;
        vsf_timer_tick_t due;
    )
};
//! @}
#endif
#endif

#if VSF_CFG_SYNC_EN == ENABLED
//! \name sync
//! @{
def_simple_class(vsf_sync_t) {

    protected_member(
        union {
            struct {
                uint16_t    cur         : 15;
                uint16_t    has_owner   : 1;
            };
            uint16_t        cur_value;
        };
        union {
            struct {
                uint16_t    max         : 15;
                uint16_t    manual_rst  : 1;
            };
            uint16_t        max_value;
        };

        vsf_dlist_t         pending_list;
    )
};
//! @}

//! \name sync_with_owner
//! @{
def_simple_class(vsf_sync_owner_t) {
    which(
        implement(vsf_sync_t)
    )
    private_member(
        vsf_eda_t           *eda_owner;
    )
};
//! @}

#ifndef __VSF_BITMAP_EVT_DEFINED__
#define __VSF_BITMAP_EVT_DEFINED__

#define VSF_BMPEVT_OR               0
#define VSF_BMPEVT_AND              1

struct vsf_bmpevt_adapter_op_t {
    vsf_err_t (*init)(vsf_bmpevt_adapter_t *pthis);
    vsf_err_t (*reset)(vsf_bmpevt_adapter_t *pthis);
};
typedef struct vsf_bmpevt_adapter_op_t vsf_bmpevt_adapter_op_t;
#endif
 
//! \name bmpevt_adapter
//! @{
def_simple_class(vsf_bmpevt_adapter_t) {

    public_member (
        const vsf_bmpevt_adapter_op_t   *op;
        const uint32_t                  mask;
    )
    private_member(
        vsf_bmpevt_t                    *bmpevt_host;
    )
};
//! @}

//! \name bmpevt_adapter_eda
//! @{
def_simple_class(vsf_bmpevt_adapter_eda_t) {
    which(
        implement(vsf_bmpevt_adapter_t)
    )
    private_member(
        vsf_eda_t           eda;
    )
};
//! @}

//! \name bmpevt_pender
//! @{
def_simple_class(vsf_bmpevt_pender_t) {

    public_member (
        uint32_t        mask;
        uint8_t         operator    : 1;
    )

    private_member(
        vsf_eda_t           *eda_pending;
    )
};
//! @}

//! \name bmpevt
//! @{
def_simple_class(vsf_bmpevt_t) {

    public_member (
        uint32_t                auto_reset;
        vsf_bmpevt_adapter_t    **adapters;
    )

    private_member(
        vsf_dlist_t                 pending_list;
        uint32_t                    value;
        uint32_t                    cancelled_value;
    )

    private_member(
        union {
            struct {
                uint8_t             adapter_count   : 5;
                uint8_t             is_cancelling   : 1;
                uint8_t             is_polling      : 1;
                uint8_t             is_to_repoll    : 1;
            };
            uint8_t                 flag;
        };
    )
};
//! @}

#if VSF_CFG_QUEUE_EN == ENABLED
struct vsf_queue_op_t {
    bool (*enqueue)(vsf_queue_t *pthis, void *node);
    bool (*dequeue)(vsf_queue_t *pthis, void **node);
};
typedef struct vsf_queue_op_t vsf_queue_op_t;

//! \name queue
//! @{
def_simple_class(vsf_queue_t) {
    union {
        implement(vsf_sync_t)
#if VSF_CFG_QUEUE_MULTI_TX_EN == ENABLED
            
        protected_member(
            union {
                uint16_t        __cur_value;
            };
            union {
                struct {
                    uint16_t    __max         : 15;
                    uint16_t    tx_processing : 1;
                };
                uint16_t        __max_value;
            };
        )
#else
        protected_member(
            uint16_t            __cur_value;
            uint16_t            __max_value;
            vsf_eda_t           *eda_tx;
        )
#endif
    };

    public_member(
        vsf_queue_op_t  op;
    )

    protected_member(
        vsf_eda_t *eda_rx;
    )
};
//! @}
#endif

// IPC
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
typedef vsf_sync_t          vsf_crit_t;

// vsf_mutex_t support priority inherit
// so who claim mutex, he must free the mutex himself
typedef vsf_sync_owner_t    vsf_mutex_t;

struct vsf_bmpevt_adapter_sync_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_sync_t *sync;
}ALIGN(4);
typedef struct vsf_bmpevt_adapter_sync_t vsf_bmpevt_adapter_sync_t;

struct vsf_bmpevt_adapter_bmpevt_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_bmpevt_t *bmpevt;
    vsf_bmpevt_pender_t pender;
}ALIGN(4);
typedef struct vsf_bmpevt_adapter_bmpevt_t vsf_bmpevt_adapter_bmpevt_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_CFG_TIMER_EN == ENABLED
SECTION(".text.vsf.kernel.teda")
extern vsf_timer_tick_t vsf_timer_get_tick(void);

SECTION(".text.vsf.kernel.vsf_timer_get_duration")
extern uint_fast32_t vsf_timer_get_duration(vsf_timer_tick_t from_time, vsf_timer_tick_t to_time);

SECTION(".text.vsf.kernel.vsf_timer_get_elapsed")
extern uint_fast32_t vsf_timer_get_elapsed(vsf_timer_tick_t from_time);

#endif
SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_init(  vsf_eda_t *pthis, 
                                vsf_priority_t priotiry, 
                                bool is_stack_owner);

SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_fini(vsf_eda_t *pthis);

SECTION(".text.vsf.kernel.eda")
extern vsf_eda_t *vsf_eda_get_cur(void);

SECTION(".text.vsf.kernel.vsf_eda_get_cur_evt")
extern vsf_evt_t vsf_eda_get_cur_evt(void);

SECTION(".text.vsf.kernel.vsf_eda_get_cur_msg")
extern void *vsf_eda_get_cur_msg(void);

SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
extern bool vsf_eda_is_stack_owner(vsf_eda_t *pthis);

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION(".text.vsf.kernel.eda_nesting")
extern bool vsf_eda_return(void);

SECTION(".text.vsf.kernel.eda_nesting")
extern vsf_err_t vsf_eda_call_eda(vsf_eda_evthandler_t evthandler, void *param);

SECTION(".text.vsf.kernel.vsf_eda_yield")
extern void vsf_eda_yield(void);

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
extern fsm_rt_t vsf_eda_call_fsm(vsf_fsm_entry_t entry, void *param);

SECTION(".text.vsf.kernel.vsf_eda_init_ex")
vsf_err_t vsf_eda_init_ex(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg);
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_FSM
#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL

#if VSF_CFG_TIMER_EN == ENABLED
SECTION(".text.vsf.kernel.teda")
extern vsf_err_t vsf_teda_init(vsf_teda_t *pthis, 
                        vsf_priority_t priority, 
                        bool is_stack_owner);

SECTION(".text.vsf.kernel.vsf_teda_init_ex")
vsf_err_t vsf_teda_init_ex(vsf_teda_t *pthis, vsf_eda_cfg_t *cfg);

SECTION(".text.vsf.kernel.teda")
extern vsf_err_t vsf_teda_fini(vsf_teda_t *pthis);

SECTION(".text.vsf.kernel.teda")
extern vsf_err_t vsf_teda_set_timer(uint_fast32_t tick);

SECTION(".text.vsf.kernel.vsf_teda_set_timer_ms")
extern vsf_err_t vsf_teda_set_timer_ms(uint_fast32_t ms);

SECTION(".text.vsf.kernel.vsf_teda_set_timer_us")
extern vsf_err_t vsf_teda_set_timer_us(uint_fast32_t us);

SECTION(".text.vsf.kernel.vsf_teda_cancel_timer")
extern vsf_err_t vsf_teda_cancel_timer(vsf_teda_t *pthis);

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
SECTION(".text.vsf.kernel.vsf_callback_timer_add")
vsf_err_t vsf_callback_timer_add(vsf_callback_timer_t *timer, uint_fast32_t tick);

SECTION(".text.vsf.kernel.vsf_callback_timer_add_ms")
vsf_err_t vsf_callback_timer_add_ms(vsf_callback_timer_t *timer, uint_fast32_t ms);

SECTION(".text.vsf.kernel.vsf_callback_timer_add_us")
vsf_err_t vsf_callback_timer_add_us(vsf_callback_timer_t *timer, uint_fast32_t us);
#endif
#endif

#ifdef VSF_CFG_EVTQ_LIST
extern vsf_err_t vsf_eda_set_priority(vsf_eda_t *pthis, vsf_priority_t priority);
#endif

SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_post_evt(vsf_eda_t *pthis, vsf_evt_t evt);

SECTION(".text.vsf.kernel.vsf_eda_post_msg")
extern vsf_err_t vsf_eda_post_msg(vsf_eda_t *pthis, void *msg);
extern vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg);

#if VSF_CFG_SYNC_EN == ENABLED
SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_init(vsf_sync_t *pthis, uint_fast16_t cur_value,
        uint_fast16_t max_value);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_increase(vsf_sync_t *pthis);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_increase_ex(vsf_sync_t *pthis, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_decrease(vsf_sync_t *pthis, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_sync_cancel")
extern void vsf_eda_sync_cancel(vsf_sync_t *pthis);

SECTION(".text.vsf.kernel.vsf_eda_sync_get_reason")
extern vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *pthis, vsf_evt_t evt);

#if VSF_CFG_BMPEVT_EN == ENABLED
SECTION(".data.vsf.kernel.vsf_eda_bmpevt_adapter_sync_op")
extern const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_sync_op;

SECTION(".data.vsf.kernel.vsf_eda_bmpevt_adapter_bmpevt_op")
extern const vsf_bmpevt_adapter_op_t vsf_eda_bmpevt_adapter_bmpevt_op;

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_init")
extern vsf_err_t vsf_eda_bmpevt_init(vsf_bmpevt_t *pthis, uint_fast8_t adapter_count);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_set")
extern vsf_err_t vsf_eda_bmpevt_set(vsf_bmpevt_t *pthis, uint_fast32_t mask);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_reset")
extern vsf_err_t vsf_eda_bmpevt_reset(vsf_bmpevt_t *pthis, uint_fast32_t mask);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_cancel")
extern vsf_err_t vsf_eda_bmpevt_cancel(vsf_bmpevt_t *pthis, uint_fast32_t mask);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_pend")
extern vsf_err_t vsf_eda_bmpevt_pend(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_eda_bmpevt_poll")
extern vsf_sync_reason_t vsf_eda_bmpevt_poll(vsf_bmpevt_t *pthis, vsf_bmpevt_pender_t *pender, vsf_evt_t evt);
#endif

#if VSF_CFG_QUEUE_EN == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_queue_init")
extern vsf_err_t vsf_eda_queue_init(vsf_queue_t *pthis, uint_fast16_t max);

SECTION(".text.vsf.kernel.vsf_eda_queue_send")
extern vsf_err_t vsf_eda_queue_send(vsf_queue_t *pthis, void *node, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_eda_queue_send_ex")
extern vsf_err_t vsf_eda_queue_send_ex(vsf_queue_t *pthis, void *node, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_queue_send_get_reason")
extern vsf_sync_reason_t vsf_eda_queue_send_get_reason(vsf_queue_t *pthis, vsf_evt_t evt, void *node);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv")
extern vsf_err_t vsf_eda_queue_recv(vsf_queue_t *pthis, void **node, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_ex")
extern vsf_err_t vsf_eda_queue_recv_ex(vsf_queue_t *pthis, void **node, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_get_reason")
extern vsf_sync_reason_t vsf_eda_queue_recv_get_reason(vsf_queue_t *pthis, vsf_evt_t evt, void **node);
#endif      // VSF_CFG_QUEUE_EN


#endif      // VSF_CFG_SYNC_EN

#undef __VSF_EDA_CLASS_INHERIT
#undef __VSF_EDA_CLASS_IMPLEMENT
#endif      // __VSF_EDA_H__
