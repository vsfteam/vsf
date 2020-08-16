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

#if VSF_USE_KERNEL == ENABLED
#include "service/vsf_service.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file 
 *!        included in this file
 */
   
#if     defined(__VSF_EDA_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(VSF_EDA_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
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
#if VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ == ENABLED
#   define vsf_eda_sem_post_irq(__psem)     vsf_eda_sync_increase_irq((__psem))
#endif

// MUTEX
#define vsf_eda_mutex_init(__pmtx)                                              \
            vsf_eda_sync_init(  &((__pmtx)->use_as__vsf_sync_t),                \
                                1 | VSF_SYNC_HAS_OWNER,                         \
                                1 | VSF_SYNC_AUTO_RST)
#define vsf_eda_mutex_try_to_enter(__pmtx, __timeout)                           \
            vsf_eda_sync_decrease(&((__pmtx)->use_as__vsf_sync_t), (__timeout))

#define vsf_eda_mutex_enter(__pmtx)                                             \
            vsf_eda_sync_decrease(&((__pmtx)->use_as__vsf_sync_t), -1)

#define vsf_eda_mutex_leave(__pmtx)                                             \
            vsf_eda_sync_increase(&((__pmtx)->use_as__vsf_sync_t))

// CRIT
#define vsf_eda_crit_init(__pcrit)                                              \
            vsf_eda_mutex_init((__pcrit))

#define vsf_eda_crit_try_to_enter(__pcrit, __timeout)                           \
            vsf_eda_mutex_try_to_enter((__pcrit), (__timeout))

#define vsf_eda_crit_enter(__pcrit)                                             \
            vsf_eda_mutex_enter((__pcrit))

#define vsf_eda_crit_leave(__pcrit)                                             \
            vsf_eda_mutex_leave((__pcrit))

// EVENT
#define vsf_eda_trig_init(__pevt, __set, __auto_rst)                            \
            vsf_eda_sync_init(                                                  \
                    (__pevt),                                                   \
                    (__set),                                                    \
                    1 | ((__auto_rst) ? VSF_SYNC_AUTO_RST : VSF_SYNC_MANUAL_RST))
#define vsf_eda_trig_set(__pevt)           vsf_eda_sync_increase((__pevt))
#define vsf_eda_trig_reset(__pevt)         vsf_eda_sync_force_reset((__pevt))
#define vsf_eda_trig_wait(__pevt, __timeout)                                    \
            vsf_eda_sync_decrease((__pevt), (__timeout))
#if VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ == ENABLED
#   define vsf_eda_trig_set_irq(__pevt)     vsf_eda_sync_increase_irq((__pevt))
#endif

// CRIT without priority boost, internal use only
// only used for edas with same priority
#define __vsf_eda_crit_npb_init(__pcrit)                                        \
            vsf_eda_sync_init(  (__pcrit),                                      \
                                1,                                              \
                                1 | VSF_SYNC_AUTO_RST)

#define __vsf_eda_crit_npb_try_to_enter(__pcrit, __timeout)                     \
            vsf_eda_sync_decrease((__pcrit), (__timeout))

#define __vsf_eda_crit_npb_enter(__pcrit)                                       \
            vsf_eda_sync_decrease((__pcrit), -1)

#define __vsf_eda_crit_npb_leave(__pcrit)                                       \
            vsf_eda_sync_increase((__pcrit))


#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
#   define vsf_eda_call_eda(__evthandler)                                       \
            __vsf_eda_call_eda((uintptr_t)__evthandler, NULL)
#   define vsf_eda_call_param_eda(__param_evthandler, __param)                  \
            __vsf_eda_call_eda((uintptr_t)__param_evthandler, (uintptr_t)__param)
#endif

/*============================ TYPES =========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
typedef vsf_systimer_cnt_t     vsf_timer_tick_t;
#endif

enum {
    /*!\ note wait for invalid also means wait for any evt */
    VSF_EVT_INVALID             = -1,       //!< compatible with fsm_rt_err 
    VSF_EVT_NONE                = 0,        //!< compatible with fsm_rt_cpl
    VSF_EVT_YIELD               = 1,        //!< compatible with fsm_rt_on_going

    VSF_EVT_SYSTEM              = 0x100,
    VSF_EVT_DUMMY               = VSF_EVT_SYSTEM + 0,
    VSF_EVT_INIT                = VSF_EVT_SYSTEM + 1,
    VSF_EVT_FINI                = VSF_EVT_SYSTEM + 2,
    VSF_EVT_ENTER               = VSF_EVT_SYSTEM + 3,
    VSF_EVT_EXIT                = VSF_EVT_SYSTEM + 4,
    VSF_EVT_RETURN              = VSF_EVT_EXIT,        
    
    // events for time
    VSF_EVT_TIMER               = VSF_EVT_SYSTEM + 5,

    // events for sync
    VSF_EVT_SYNC                = VSF_EVT_SYSTEM + 6,
    VSF_EVT_SYNC_CANCEL         = VSF_EVT_SYSTEM + 7,
    VSF_EVT_SYNC_POLL           = VSF_EVT_SYSTEM + 8,

    // events for message
    VSF_EVT_MESSAGE             = VSF_EVT_SYSTEM + 9,
    VSF_EVT_USER                = VSF_EVT_SYSTEM + 10,
};

declare_simple_class(vsf_eda_t)
declare_simple_class(vsf_teda_t)
declare_simple_class(vsf_sync_t)
declare_simple_class(vsf_sync_owner_t)
declare_simple_class(vsf_bmpevt_t)
declare_simple_class(vsf_bmpevt_pender_t)
declare_simple_class(vsf_bmpevt_adapter_t)
declare_simple_class(vsf_bmpevt_adapter_eda_t)
declare_simple_class(vsf_eda_queue_t)
declare_simple_class(vsf_callback_timer_t)

typedef int16_t vsf_evt_t;

typedef void (*vsf_eda_evthandler_t)(vsf_eda_t *eda, vsf_evt_t evt);
typedef void (*vsf_eda_on_terminate_t)(vsf_eda_t *eda);
typedef fsm_rt_t (*vsf_fsm_entry_t)(uintptr_t target, vsf_evt_t evt);
typedef void (*vsf_param_eda_evthandler_t)(uintptr_t target, vsf_evt_t evt);

#if VSF_KERNEL_CFG_FRAME_USER_BITS <= 6
#   define __VSF_KERNEL_CFG_FRAME_UINT_TYPE         uint8_t
#elif VSF_KERNEL_CFG_FRAME_USER_BITS <= 14
#   define __VSF_KERNEL_CFG_FRAME_UINT_TYPE         uint16_t
#elif VSF_KERNEL_CFG_FRAME_USER_BITS <= 30
#   define __VSF_KERNEL_CFG_FRAME_UINT_TYPE         uint32_t
#else
#   error  The number of user bits should not exceed 30!!! Please give the macro VSF_KERNEL_CFG_FRAME_USER_BITS a correct value.
#endif


union __vsf_eda_frame_state_t {
    struct {
        __VSF_KERNEL_CFG_FRAME_UINT_TYPE is_fsm             : 1;
        __VSF_KERNEL_CFG_FRAME_UINT_TYPE is_stack_owner     : 1;
        __VSF_KERNEL_CFG_FRAME_UINT_TYPE user               : VSF_KERNEL_CFG_FRAME_USER_BITS;
    } bits;
    __VSF_KERNEL_CFG_FRAME_UINT_TYPE flag;
};
typedef union __vsf_eda_frame_state_t __vsf_eda_frame_state_t;

declare_simple_class(__vsf_eda_frame_t)
def_simple_class(__vsf_eda_frame_t) {
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
    protected_member (
        implement(vsf_slist_node_t)
        union {
            uintptr_t                       func;
            vsf_eda_evthandler_t            evthandler;
            vsf_param_eda_evthandler_t      param_evthandler;
            vsf_fsm_entry_t                 fsm_entry;
        } fn;

        union {
            uintptr_t param;
            uintptr_t target;
        } ptr;
    )

#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    protected_member (
        __vsf_eda_frame_state_t             state;
    )
#   endif
#else
    protected_member (
        implement(vsf_slist_node_t)
        union {
            uintptr_t                       func;
            vsf_eda_evthandler_t            evthandler;
            vsf_param_eda_evthandler_t      param_evthandler;
            vsf_fsm_entry_t                 fsm_entry;
        } fn;
        
#if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                               \
    ||  VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
        __vsf_eda_frame_state_t             state;
#endif
        union {
            uintptr_t param;
            uintptr_t target;
        } ptr;
    )
#endif
};

#if __VSF_KERNEL_CFG_EDA_FRAME_POOL == ENABLED
declare_vsf_pool(vsf_eda_frame_pool)
def_vsf_pool(vsf_eda_frame_pool, __vsf_eda_frame_t)
#endif

struct vsf_eda_cfg_t {
    union {
        uintptr_t                   func;
        vsf_eda_evthandler_t        evthandler;
        vsf_param_eda_evthandler_t  param_evthandler;
        vsf_fsm_entry_t             fsm_entry;
    } fn;
    vsf_prio_t priority;
    uintptr_t target;
    vsf_eda_on_terminate_t      on_terminate;
    bool is_fsm;
    bool is_stack_owner;
};
typedef struct vsf_eda_cfg_t vsf_eda_cfg_t;

union __vsf_eda_state_t {
    struct {
#if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#   if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        uint8_t         is_ready        : 1;
        uint8_t         is_new_prio     : 1;
#   endif
        uint8_t         is_to_exit      : 1;
#else
        uint8_t         is_processing   : 1;
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    /* if is_limitted, eda can only receive 1 event */
        uint8_t         is_limitted     : 1;
        uint8_t         is_sync_got     : 1;
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        uint8_t         is_timed        : 1;
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        uint8_t         is_use_frame    : 1;
#   if      VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED                           \
        ||  VSF_KERNEL_CFG_EDA_SUPPORT_PT == ENABLED 
        uint8_t         is_evt_incoming : 1;        
#   endif
#endif

#if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
        uint8_t         polling_state   : 1;
        uint8_t         is_stack_owner  : 1;
#endif
    } bits;
    // TODO: flag is not always 16-bit here
    uint16_t            flag;                       
};
typedef union __vsf_eda_state_t __vsf_eda_state_t;

//! \name eda
//! @{
def_simple_class(vsf_eda_t) {

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE == ENABLED
    protected_member(
        vsf_eda_on_terminate_t      on_terminate;
    )
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
    protected_member(

        union {
            vsf_eda_evthandler_t    evthandler;
            vsf_slist_t             frame_list;
            __vsf_eda_frame_t       *frame;
        } fn;
    )
#else
    protected_member(
        union {
            vsf_eda_evthandler_t    evthandler;
        } fn;
    )
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#   if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    protected_member(
        vsf_dlist_node_t    pending_node;
    )
#   endif

#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
    protected_member(
        fsm_rt_t            fsm_return_state;
    )
#   endif

#   if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#       if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
    protected_member(
        vsf_dlist_node_t    rdy_node;
        vsf_slist_queue_t   evt_list;
        uint8_t             cur_priority;
        uint8_t             new_priority;
        uint8_t             priority;
    )
#       else
    protected_member(
        uint8_t             evt_cnt;
        uint8_t             priority;
    )
#       endif
#   else
    protected_member(
        uintptr_t           evt_pending;
    )
#   endif

    protected_member(
        __vsf_eda_state_t   state;
    )
#else
    protected_member(
#   if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
        vsf_dlist_node_t    pending_node;
#   endif
    
#   if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        fsm_rt_t            fsm_return_state;
#   endif

#   if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
#       if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED
        vsf_dlist_node_t    rdy_node;
        vsf_slist_queue_t   evt_list;
        uint8_t             cur_priority;
        uint8_t             new_priority;
        uint8_t             priority;
#       else
        uint8_t             evt_cnt;
        uint8_t             priority;
#       endif
#   else
        uintptr_t           evt_pending;
#   endif
        __vsf_eda_state_t   state;
    )
#endif
};
//! @}

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
//! \name timed eda
//! @{
def_simple_class(vsf_teda_t)  {
    which(
        implement(vsf_eda_t)
    )
    private_member(
        vsf_dlist_node_t    timer_node;
        vsf_timer_tick_t    due;
    )
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

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
//! \name sync
//! @{
def_simple_class(vsf_sync_t) {

    protected_member(
        union {
            struct {
                uint16_t    cur         : 15;
                uint16_t    has_owner   : 1;
            } bits;
            uint16_t        cur_value;
        } cur_union;
        union {
            struct {
                uint16_t    max         : 15;
                uint16_t    manual_rst  : 1;
            } bits;
            uint16_t        max_value;
        } max_union;

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
        vsf_dlist_t             pending_list;
        uint32_t                value;
        uint32_t                cancelled_value;
    )

    private_member(
        union {
            struct {
                uint8_t         adapter_count   : 5;
                uint8_t         is_cancelling   : 1;
                uint8_t         is_polling      : 1;
                uint8_t         is_to_repoll    : 1;
            } bits;
            uint8_t             flag;
        } state;
    )
};
//! @}

#if __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE == ENABLED
struct vsf_eda_queue_op_t {
    bool (*enqueue)(vsf_eda_queue_t *pthis, void *node);
    bool (*dequeue)(vsf_eda_queue_t *pthis, void **node);
};
typedef struct vsf_eda_queue_op_t vsf_eda_queue_op_t;

//! \brief define alias for vsf_eda_queue_t. osa means os-aware 
typedef struct vsf_eda_queue_t vsf_osa_queue_t;
typedef struct vsf_eda_queue_op_t vsf_osa_queue_op_t;

//! \name queue
//! @{
def_simple_class(vsf_eda_queue_t) {
    union {
        implement(vsf_sync_t)
#if VSF_KERNEL_CFG_QUEUE_MULTI_TX_EN == ENABLED
            
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
        vsf_eda_queue_op_t  op;
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
    VSF_SYNC_CANCEL
};
typedef enum vsf_sync_reason_t vsf_sync_reason_t;

typedef vsf_sync_t          vsf_sem_t;
typedef vsf_sync_t          vsf_trig_t;
// internal use only
typedef vsf_sync_t          __vsf_crit_npb_t;

// vsf_mutex_t support priority inherit
// so who claim mutex, he must free the mutex himself
typedef vsf_sync_owner_t    vsf_mutex_t;
typedef vsf_mutex_t         vsf_crit_t;

typedef struct vsf_bmpevt_adapter_sync_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_sync_t *sync;
} vsf_bmpevt_adapter_sync_t;

typedef struct vsf_bmpevt_adapter_bmpevt_t {
    implement(vsf_bmpevt_adapter_eda_t)
    vsf_bmpevt_t *bmpevt;
    vsf_bmpevt_pender_t pender;
} vsf_bmpevt_adapter_bmpevt_t;
#endif

typedef enum vsf_kernel_error_t {
    VSF_KERNEL_ERR_NONE = 0,
    VSF_KERNEL_ERR_NULL_EDA_PTR,
    VSF_KERNEL_ERR_INVALID_USAGE,
    VSF_KERNEL_ERR_EDA_DOES_NOT_SUPPORT_TIMER,
    VSF_KERNEL_ERR_SHOULD_NOT_USE_PRIO_INHERIT_IN_IDLE_OR_ISR
}vsf_kernel_error_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION(".text.vsf.kernel.teda")
extern vsf_timer_tick_t vsf_timer_get_tick(void);

SECTION(".text.vsf.kernel.vsf_timer_get_duration")
extern uint_fast32_t vsf_timer_get_duration(vsf_timer_tick_t from_time, vsf_timer_tick_t to_time);

SECTION(".text.vsf.kernel.vsf_timer_get_elapsed")
extern uint_fast32_t vsf_timer_get_elapsed(vsf_timer_tick_t from_time);

#endif

#if defined(VSF_EDA_CLASS_INHERIT) || defined(__VSF_EDA_CLASS_IMPLEMENT)
SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
extern vsf_err_t vsf_eda_set_evthandler(vsf_eda_t *pthis, 
                                        vsf_eda_evthandler_t evthandler);

SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_init(  vsf_eda_t *pthis, 
                                vsf_prio_t priotiry, 
                                bool is_stack_owner);

#   if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED

SECTION(".text.vsf.kernel.vsf_eda_polling_state_get")
extern bool vsf_eda_polling_state_get(vsf_eda_t *pthis);

SECTION(".text.vsf.kernel.vsf_eda_polling_state_set")
extern void vsf_eda_polling_state_set(vsf_eda_t *pthis, bool state);

#   endif

#endif

SECTION(".text.vsf.kernel.vsf_eda_set_evthandler")
extern vsf_err_t vsf_eda_go_to(uintptr_t evthandler);

SECTION(".text.vsf.kernel.vsf_eda_init_ex")
vsf_err_t vsf_eda_init_ex(vsf_eda_t *pthis, vsf_eda_cfg_t *cfg);

SECTION(".text.vsf.kernel.eda")
extern vsf_eda_t *vsf_eda_get_cur(void);

SECTION(".text.vsf.kernel.vsf_eda_get_cur_evt")
extern vsf_evt_t vsf_eda_get_cur_evt(void);

SECTION(".text.vsf.kernel.vsf_eda_get_cur_msg")
extern void *vsf_eda_get_cur_msg(void);

#if VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_is_stack_owner")
extern bool vsf_eda_is_stack_owner(vsf_eda_t *pthis);
#endif

SECTION(".text.vsf.kernel.vsf_eda_return")
extern bool vsf_eda_return(void);

SECTION(".text.vsf.kernel.vsf_eda_yield")
extern void vsf_eda_yield(void);

#if defined(VSF_EDA_CLASS_INHERIT) || defined(__VSF_EDA_CLASS_IMPLEMENT)
/* vsf_eda_fini() enables you to kill other eda tasks.
   We highly recommend that DO NOT use this api until you 100% sure.
   please make sure that the resources are properly freed when you trying to kill
   an eda other than your own. We highly recommend that please send a semaphore to
   the target eda to ask it killing itself after properly freeing all the resources.
 */
SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_fini(vsf_eda_t *pthis);
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
SECTION(".text.vsf.kernel.__vsf_eda_call_eda")
extern vsf_err_t __vsf_eda_call_eda(uintptr_t evthandler, uintptr_t param);

SECTION(".text.vsf.kernel.__vsf_eda_go_to_ex")
extern vsf_err_t __vsf_eda_go_to_ex(uintptr_t evthandler, uintptr_t param);

SECTION(".text.vsf.kernel.eda_nesting")
extern vsf_err_t __vsf_eda_call_eda_ex( uintptr_t func, 
                                        uintptr_t param, 
                                        __vsf_eda_frame_state_t state,
                                        bool is_sub_call);

SECTION(".text.vsf.kernel.vsf_eda_target_set")
extern vsf_err_t vsf_eda_target_set(uintptr_t param);

SECTION(".text.vsf.kernel.vsf_eda_target_get")
extern uintptr_t vsf_eda_target_get(void);

SECTION(".text.vsf.kernel.vsf_eda_frame_user_value_set")
extern
vsf_err_t vsf_eda_frame_user_value_set(__VSF_KERNEL_CFG_FRAME_UINT_TYPE value);

SECTION(".text.vsf.kernel.vsf_eda_frame_user_value_get")
extern
vsf_err_t vsf_eda_frame_user_value_get(__VSF_KERNEL_CFG_FRAME_UINT_TYPE* pvalue);

#if     VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
SECTION(".text.vsf.kernel.eda_fsm")
extern fsm_rt_t __vsf_eda_call_fsm(vsf_fsm_entry_t entry, uintptr_t param);
#   endif      // VSF_KERNEL_CFG_EDA_SUPPORT_FSM

#endif      // VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
SECTION(".text.vsf.kernel.teda")
extern vsf_err_t vsf_teda_init(vsf_teda_t *pthis, 
                        vsf_prio_t priority, 
                        bool is_stack_owner);

SECTION(".text.vsf.kernel.vsf_teda_init_ex")
extern vsf_err_t vsf_teda_init_ex(vsf_teda_t *pthis, vsf_eda_cfg_t *cfg);

SECTION(".text.vsf.kernel.vsf_teda_set_timer")
extern vsf_err_t vsf_teda_set_timer(uint_fast32_t tick);

SECTION(".text.vsf.kernel.vsf_teda_set_timer_ex")
vsf_err_t vsf_teda_set_timer_ex(vsf_teda_t *pthis, uint_fast32_t tick);

SECTION(".text.vsf.kernel.vsf_teda_set_timer_ms")
extern vsf_err_t vsf_teda_set_timer_ms(uint_fast32_t ms);

SECTION(".text.vsf.kernel.vsf_teda_set_timer_us")
extern vsf_err_t vsf_teda_set_timer_us(uint_fast32_t us);

SECTION(".text.vsf.kernel.vsf_teda_cancel_timer")
extern vsf_err_t vsf_teda_cancel_timer(vsf_teda_t *pthis);

#if VSF_KERNEL_CFG_CALLBACK_TIMER == ENABLED
SECTION(".text.vsf.kernel.vsf_callback_timer_add")
vsf_err_t vsf_callback_timer_add(vsf_callback_timer_t *timer, uint_fast32_t tick);

SECTION(".text.vsf.kernel.vsf_callback_timer_remove")
vsf_err_t vsf_callback_timer_remove(vsf_callback_timer_t *timer);

SECTION(".text.vsf.kernel.vsf_callback_timer_add_ms")
vsf_err_t vsf_callback_timer_add_ms(vsf_callback_timer_t *timer, uint_fast32_t ms);

SECTION(".text.vsf.kernel.vsf_callback_timer_add_us")
vsf_err_t vsf_callback_timer_add_us(vsf_callback_timer_t *timer, uint_fast32_t us);
#endif
#endif

SECTION(".text.vsf.kernel.eda")
extern vsf_err_t vsf_eda_post_evt(vsf_eda_t *pthis, vsf_evt_t evt);

SECTION(".text.vsf.kernel.vsf_eda_post_msg")
extern vsf_err_t vsf_eda_post_msg(vsf_eda_t *pthis, void *msg);
#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_post_evt_msg")
extern vsf_err_t vsf_eda_post_evt_msg(vsf_eda_t *pthis, vsf_evt_t evt, void *msg);
#endif

#if VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_init(vsf_sync_t *pthis, uint_fast16_t cur_value,
        uint_fast16_t max_value);

#if VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ == ENABLED
SECTION(".text.vsf.kernel.vsf_sync")
vsf_err_t vsf_eda_sync_increase_irq(vsf_sync_t *pthis);
#endif

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_increase(vsf_sync_t *pthis);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_increase_ex(vsf_sync_t *pthis, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_sync")
extern void vsf_eda_sync_force_reset(vsf_sync_t *pthis);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_decrease(vsf_sync_t *pthis, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_sync")
extern vsf_err_t vsf_eda_sync_decrease_ex(vsf_sync_t *pthis, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_sync_cancel")
extern void vsf_eda_sync_cancel(vsf_sync_t *pthis);

SECTION(".text.vsf.kernel.vsf_eda_sync_get_reason")
extern vsf_sync_reason_t vsf_eda_sync_get_reason(vsf_sync_t *pthis, vsf_evt_t evt);

#if VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED
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

#if __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE == ENABLED
SECTION(".text.vsf.kernel.vsf_eda_queue_init")
extern vsf_err_t vsf_eda_queue_init(vsf_eda_queue_t *pthis, uint_fast16_t max);

SECTION(".text.vsf.kernel.vsf_eda_queue_send")
extern vsf_err_t vsf_eda_queue_send(vsf_eda_queue_t *pthis, void *node, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_eda_queue_send_ex")
extern vsf_err_t vsf_eda_queue_send_ex(vsf_eda_queue_t *pthis, void *node, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_queue_send_get_reason")
extern vsf_sync_reason_t vsf_eda_queue_send_get_reason(vsf_eda_queue_t *pthis, vsf_evt_t evt, void *node);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv")
extern vsf_err_t vsf_eda_queue_recv(vsf_eda_queue_t *pthis, void **node, int_fast32_t timeout);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_ex")
extern vsf_err_t vsf_eda_queue_recv_ex(vsf_eda_queue_t *pthis, void **node, int_fast32_t timeout, vsf_eda_t *eda);

SECTION(".text.vsf.kernel.vsf_eda_queue_recv_get_reason")
extern vsf_sync_reason_t vsf_eda_queue_recv_get_reason(vsf_eda_queue_t *pthis, vsf_evt_t evt, void **node);
#endif      // __VSF_KERNEL_CFG_SUPPORT_GENERIC_QUEUE


#endif      // VSF_KERNEL_CFG_SUPPORT_SYNC

#undef VSF_EDA_CLASS_INHERIT
#undef __VSF_EDA_CLASS_IMPLEMENT

#endif
#endif      // __VSF_EDA_H__
