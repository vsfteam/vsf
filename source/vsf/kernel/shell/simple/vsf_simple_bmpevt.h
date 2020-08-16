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

#if     VSF_USE_KERNEL_SIMPLE_SHELL == ENABLED                                  \
    &&  VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT == ENABLED                          \
    &&  VSF_USE_KERNEL == ENABLED                                               \
    &&  VSF_KERNEL_CFG_SUPPORT_SYNC == ENABLED
    
#include "../../vsf_eda.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#   define using_grouped_evt            vsf_bmpevt_pender_t pender

#   define __declare_grouped_evts(__name)                                       \
            typedef vsf_bmpevt_t __name;                                        \
            typedef enum enum_of_##__name   enum_of_##__name;
            
#   define declare_grouped_evts(__name)       __declare_grouped_evts(__name)             

#   define __def_grouped_evts(__name)                                           \
        enum enum_of_##__name{                                                  \
            __##__name##_start = __COUNTER__ + 1,


#   define __end_def_grouped_evts(__name)                                       \
            all_evts_msk_of_##__name =                                          \
            ((uint64_t)1<<(__COUNTER__ - __##__name##_start)) - 1,              \
        };
#   define end_def_grouped_evts(__name)    __end_def_grouped_evts(__name)
        
#   define __def_evt(__name, __evt)                                             \
            __evt##_idx = __COUNTER__ - __##__name##_start,                     \
            __evt##_msk = (uint32_t)(1<<(__evt##_idx))
#   define def_evt(__name, __evt)      __def_evt(__name, __evt)

#   define def_adapter(__name, __evt)     __def_evt(__name, __evt)
        
#   define def_grouped_evts(__name)                                             \
            __def_grouped_evts(__name)


#   define __implement_grouped_evts(__name, ...)                                \
        static const vsf_bmpevt_adapter_t *__adapters_of_##__name[] = {         \
            __VA_ARGS__                                                         \
        };

#   define implement_grouped_evts(__name, ...)                                  \
            __implement_grouped_evts(__name, __VA_ARGS__)

#   define add_sync_adapter(__SYNC, __MSK, ...)                                 \
    &((vsf_bmpevt_adapter_sync_t){                                              \
            .sync = (__SYNC),                                                   \
            .use_as__vsf_bmpevt_adapter_eda_t.use_as__vsf_bmpevt_adapter_t = {  \
                .mask  = (__MSK),                                               \
                .op = &vsf_eda_bmpevt_adapter_sync_op,                          \
            },                                                                  \
            __VA_ARGS__                                                         \
        }.use_as__vsf_bmpevt_adapter_eda_t.use_as__vsf_bmpevt_adapter_t)



#   define __init_grouped_evts(__name, __evt_group, __auto_reset)               \
        __vsf_grouped_evts_init((__evt_group),                                  \
            (vsf_bmpevt_adapter_t **)&(__adapters_of_##__name),                 \
            UBOUND(__adapters_of_##__name),                                     \
            (__auto_reset))
        
#   define init_grouped_evts(__name, __evt_group, __auto_reset)                 \
            __init_grouped_evts(__name, __evt_group, (__auto_reset))
            
#   define __grouped_evts_info(__name)    __grouped_evts_##__name##_info
#   define grouped_evts_info(__name)      __grouped_evts_info(__name)
#   define __grouped_evts_adapter(__name, __INDEX)                              \
        __adapters_of_##__name[(__INDEX)]
#   define grouped_evts_adapter(__name, __INDEX)                                \
        __grouped_evts_adapter(__name, (__INDEX))


#   define wait_for_all_timeout(__group, __msk, __timeout)                      \
            this.pender.mask = (__msk);                                         \
            this.pender.op  = VSF_BMPEVT_AND;                                   \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason = __vsf_bmpevt_wait_for(                            \
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
            this.pender.op = VSF_BMPEVT_OR;                                     \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason = __vsf_bmpevt_wait_for(                            \
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
                        
#   define wait_for_one(__group, __msk)                                         \
            wait_for_any((__group), (__msk))
        
#   define wait_for_one_timeout(__group, __msk, __timeout)                      \
            wait_for_any_timeout((__group), (__msk), (__timeout))
        
#   define wait_for_one_timeout_ms(__group, __msk, __timeout)                   \
            wait_for_any_timeout_ms((__group), (__msk), (__timeout))
        
#   define wait_for_one_timeout_us(__group, __msk, __timeout)                   \
            wait_for_any_timeout_us((__group), (__msk), (__timeout))
        
#   define reset_grouped_evts(__group, __msk)                                   \
            vsf_eda_bmpevt_reset((__group),(__msk))

#   define set_grouped_evts(__group, __msk)                                     \
            vsf_eda_bmpevt_set((__group),(__msk))


/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

SECTION("text.vsf.kernel.__vsf_grouped_evts_init")
extern void __vsf_grouped_evts_init(  vsf_bmpevt_t *this_ptr, 
                                vsf_bmpevt_adapter_t **adapters_pptr, 
                                uint_fast8_t adapter_count,
                                uint_fast32_t auto_reset);
                
SECTION("text.vsf.kernel.__vsf_bmpevt_wait_for")    
extern vsf_sync_reason_t __vsf_bmpevt_wait_for(
                                            vsf_bmpevt_t *bmpevt_ptr,
                                            const vsf_bmpevt_pender_t *pender_ptr,
                                            int_fast32_t time_out);

#ifdef __cplusplus
}
#endif

#endif
#endif
/* EOF */
