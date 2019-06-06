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

#ifndef __VSF_KERNEL_SHELL_SIMPLE_BMEVT_H__
#define __VSF_KERNEL_SHELL_SIMPLE_BMEVT_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if     VSF_USE_SIMPLE_SHELL == ENABLED                                         \
    &&  VSF_CFG_SYNC_EN == ENABLED                                              \
    &&  VSF_CFG_BMPEVT_EN == ENABLED
    
#include "../../vsf_eda.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/


#   define vsf_bmpevt_init              vsf_eda_bmpevt_init
#   define vsf_bmpevt_set               vsf_eda_bmpevt_set
#   define vsf_bmpevt_reset             vsf_eda_bmpevt_reset
#   define vsf_bmpevt_poll              vsf_eda_bmpevt_poll

#   define using_grouped_evt            vsf_bmpevt_pender_t pender


#   define __declare_grouped_evts(__NAME)                                       \
            typedef vsf_bmpevt_t __NAME;                                        \
            typedef enum enum_of_##__NAME   enum_of_##__NAME;
            
#   define declare_grouped_evts(__NAME)       __declare_grouped_evts(__NAME)             

#   define __def_grouped_evts(__NAME)                                           \
        enum enum_of_##__NAME{                                                  \
            __##__NAME##_start = __COUNTER__ + 1,


#   define __end_def_grouped_evts(__NAME)                                       \
            all_evts_msk_of_##__NAME =                                          \
            ((uint64_t)1<<(__COUNTER__ - __##__NAME##_start)) - 1,              \
        };
#   define end_def_grouped_evts(__NAME)    __end_def_grouped_evts(__NAME)
        
#   define __def_evt(__NAME, __EVT)                                             \
            __EVT##_idx = __COUNTER__ - __##__NAME##_start,                     \
            __EVT##_msk = (uint32_t)(1<<(__EVT##_idx))
#   define def_evt(__NAME, __EVT)      __def_evt(__NAME, __EVT)

#   define def_adapter(__NAME, __EVT)     __def_evt(__NAME, __EVT)
        
#   define def_grouped_evts(__NAME)                                             \
            __def_grouped_evts(__NAME)


#   define __implement_grouped_evts(__NAME, ...)                                \
        static const vsf_bmpevt_adapter_t *__adapters_of_##__NAME[] = {         \
            __VA_ARGS__                                                         \
        };

#   define implement_grouped_evts(__NAME, ...)                                  \
            __implement_grouped_evts(__NAME, __VA_ARGS__)

#   define add_sync_adapter(__SYNC, __MSK, ...)                                 \
    &((vsf_bmpevt_adapter_sync_t){                                              \
            .sync = (__SYNC),                                                   \
            .use_as__vsf_bmpevt_adapter_eda_t.use_as__vsf_bmpevt_adapter_t = {  \
                .mask  = (__MSK),                                               \
                .op = &vsf_eda_bmpevt_adapter_sync_op,                          \
            },                                                                  \
            __VA_ARGS__                                                         \
        }.use_as__vsf_bmpevt_adapter_eda_t.use_as__vsf_bmpevt_adapter_t)



#   define __init_grouped_evts(__NAME, __EVT_GROUP, __AUTO_RESET)               \
        grouped_evts_init((__EVT_GROUP),                                        \
            (vsf_bmpevt_adapter_t **)&(__adapters_of_##__NAME),                 \
            UBOUND(__adapters_of_##__NAME),                                     \
            (__AUTO_RESET))
        
#   define init_grouped_evts(__NAME, __EVT_GROUP, __AUTO_RESET)                 \
            __init_grouped_evts(__NAME, __EVT_GROUP, (__AUTO_RESET))
            
#   define __grouped_evts_info(__NAME)    __grouped_evts_##__NAME##_info
#   define grouped_evts_info(__NAME)      __grouped_evts_info(__NAME)
#   define __grouped_evts_adapter(__NAME, __INDEX)                              \
        __adapters_of_##__NAME[(__INDEX)]
#   define grouped_evts_adapter(__NAME, __INDEX)                                \
        __grouped_evts_adapter(__NAME, (__INDEX))


#   define wait_for_all_timeout(__group, __msk, __timeout)                      \
            this.pender.mask = (__msk);                                         \
            this.pender.operator = VSF_BMPEVT_AND;                              \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason = vsf_bmpevt_wait_for(                              \
                    (__group),                                                  \
                    (vsf_bmpevt_pender_t *)                                     \
                    &this.pender, (__timeout)),                                 \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

#   define wait_for_all_timeout_ms(__group, __msk, __timeout)                   \
            wait_for_all_timeout(   __group,                                    \
                                    (__msk),                                    \
                                    vsf_systimer_ms_to_tick(__timeout))

#   define wait_for_all_timeout_us(__group, __msk, __timeout)                   \
            wait_for_all_timeout(   __group,                                    \
                                    (__msk),                                    \
                                    vsf_systimer_us_to_tick(__timeout))

#   define wait_for_all(__group, __msk)                                         \
            wait_for_all_timeout( __group, (__msk), -1)

#   define wait_for_any_timeout(__group, __msk, __timeout)                      \
            this.pender.mask = (__msk);                                         \
            this.pender.operator = VSF_BMPEVT_OR;                               \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason = vsf_bmpevt_wait_for(                              \
                    (__group),                                                  \
                    (vsf_bmpevt_pender_t *)                                     \
                    &this.pender, (__timeout)),                                 \
                    (reason == VSF_SYNC_GET || reason == VSF_SYNC_TIMEOUT)))

#   define wait_for_any_timeout_ms(__group, __msk, __timeout)                   \
            wait_for_any_timeout(   __group,                                    \
                                    (__msk),                                    \
                                    vsf_systimer_ms_to_tick(__timeout))

#   define wait_for_any_timeout_us(__group, __msk, __timeout)                   \
            wait_for_any_timeout(   __group,                                    \
                                    (__msk),                                    \
                                    vsf_systimer_us_to_tick(__timeout))

#   define wait_for_any(__group, __msk)                                         \
            wait_for_any_timeout( __group, (__msk), -1)

#   define on_bmevt_timeout()                                                   \
                if (VSF_SYNC_TIMEOUT == vsf_eda_get_cur_evt())
                        
#   define wait_for_one(__group, __msk)                                         \
            wait_for_any((__group), (__msk))
        
#   define wait_for_one_timeout(__group, __msk, __timeout)                      \
            wait_for_any_timeout((__group), (__msk), (__timeout))
        
#   define wait_for_one_timeout_ms(__group, __msk, __timeout)                   \
            wait_for_any_timeout_ms((__group), (__msk), (__timeout))
        
#   define wait_for_one_timeout_us(__group, __msk, __timeout)                   \
            wait_for_any_timeout_us((__group), (__msk), (__timeout))
        
#   define reset_grouped_evts(__group, __msk)                                   \
            vsf_bmpevt_reset((__group),(__msk))

#   define set_grouped_evts(__group, __msk)                                     \
            vsf_bmpevt_set((__group),(__msk))


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION("text.vsf.kernel.grouped_evts_init")
extern void grouped_evts_init(  vsf_bmpevt_t *pthis, 
                                vsf_bmpevt_adapter_t **ppadapters, 
                                uint_fast8_t adapter_count,
                                uint_fast32_t auto_reset);
                
SECTION("text.vsf.kernel.vsf_bmpevt_wait_for")    
extern vsf_sync_reason_t vsf_bmpevt_wait_for(
                                            vsf_bmpevt_t *pbmpevt,
                                            const vsf_bmpevt_pender_t *ppender,
                                            int_fast32_t time_out);

#endif

#endif
/* EOF */
