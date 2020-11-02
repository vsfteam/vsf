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

#ifndef __RTOS_MODEL_H__
#define __RTOS_MODEL_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "timers.h"

#if     defined(__VSF_ARCH_RTOS_MODEL_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_SYSTIMER_CFG_IMPL_MODE
#   define VSF_SYSTIMER_CFG_IMPL_MODE   VSF_SYSTIMER_IMPL_REQUEST_RESPONSE
#endif

#define VSF_ARCH_RTOS_PRIORITY_NUM      configMAX_PRIORITIES

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef TickType_t                  vsf_systimer_cnt_t;
typedef bool                        vsf_gint_state_t;

dcl_simple_class(vsf_arch_irq_thread_t)
dcl_simple_class(vsf_arch_irq_request_t)

def_simple_class(vsf_arch_irq_request_t) {
    private_member(
        EventGroupHandle_t          event;
#if configSUPPORT_STATIC_ALLOCATION
        StaticEventGroup_t          static_event;
#endif
    )
};

def_simple_class(vsf_arch_irq_thread_t) {
    private_member(
        TaskHandle_t                thread;
#if configSUPPORT_STATIC_ALLOCATION
        StaticTask_t                static_task;
        StackType_t                 stack[VSF_ARCH_RTOS_CFG_STACK_DEPTH];
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_freertos_start(void);

extern bool __vsf_arch_model_low_level_init(void);
extern bool __vsf_arch_model_is_current_task(vsf_arch_irq_thread_t *irq_thread);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
