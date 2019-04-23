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

#ifndef __VSF_KERNEL_SHELL_SIMPLE_H__
#define __VSF_KERNEL_SHELL_SIMPLE_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"

#if VSF_USE_SIMPLE_SHELL == ENABLED
#include "../../vsf_eda.h"
#include "./vsf_simple_bmevt.h"
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define static_task_instance(...)                                               \
        struct {__VA_ARGS__} static TPASTE2(__local_cb, __LINE__),              \
            *ptThis = &TPASTE2(__local_cb, __LINE__);

#define features_used(...)              __VA_ARGS__

#if __IS_COMPILER_IAR__
#   define mem_sharable(...)            union {uint8_t zzzz___; __VA_ARGS__};
#else
#   define mem_sharable(...)            union {__VA_ARGS__};
#endif
#define mem_nonsharable(...)            __VA_ARGS__



#if VSF_CFG_TIMER_EN == ENABLED
#   define vsf_delay(__MS)     if (VSF_EVT_TIMER == __vsf_delay(__MS))
#endif



#if VSF_CFG_SYNC_EN == ENABLED
#   define vsf_mutex_leave vsf_eda_mutex_leave
#   define vsf_sem_init(__psem, __cnt)                                          \
            vsf_eda_sync_init((__psem), (__cnt), 0x7FFF | VSF_SYNC_AUTO_RST)
#   define vsf_sem_post(__psem)            vsf_eda_sem_post((__psem))
            
#   define vsf_sem_pend(__psem, __timeout)                                      \
            for (   vsf_sync_reason_t reason = VSF_SYNC_CANCEL;                 \
                    reason == VSF_SYNC_CANCEL;)                                 \
                if ((reason =__vsf_sem_pend((__psem),                           \
                    (__timeout)), reason == VSF_SYNC_GET))
#   define on_sem_timeout()                                                     \
                else if (VSF_SYNC_TIMEOUT == reason)
#endif
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_CFG_TIMER_EN == ENABLED
SECTION("text.vsf.kernel.__vsf_delay")
extern vsf_evt_t __vsf_delay(uint_fast32_t ms);
#endif

#if VSF_CFG_SYNC_EN == ENABLED
SECTION("text.vsf.kernel.__vsf_sem_pend")
extern 
vsf_sync_reason_t __vsf_sem_pend(vsf_sem_t *psem, int_fast32_t time_out);

SECTION("text.vsf.kernel.vsf_mutex_enter")
extern 
vsf_sync_reason_t vsf_mutex_enter(vsf_mutex_t *pmtx, int_fast32_t time_out);
#endif

#endif

#endif
/* EOF */
