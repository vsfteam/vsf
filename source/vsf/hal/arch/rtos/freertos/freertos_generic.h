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

// TODO: real priority number is configMAX_PRIORITIES from freertos configuration
//  but freertos headers are not available here to avoid namespace pollution
//#define VSF_ARCH_RTOS_PRIORITY_NUM      configMAX_PRIORITIES
#define VSF_ARCH_RTOS_PRIORITY_NUM      4

// TODO: real stack type is StackType_t from freertos configuration
//  but freertos headers are not available here to avoid namespace pollution
//#define VSF_ARCH_RTOS_STACK_T           StackType_t
#define VSF_ARCH_RTOS_STACK_T           uint32_t

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

// TODO: real tick type is TickType_t from freertos configuration
//  but freertos headers are not available here to avoid namespace pollution
//typedef TickType_t                  vsf_systimer_cnt_t;
typedef uint32_t                    vsf_systimer_cnt_t;
typedef bool                        vsf_gint_state_t;

dcl_simple_class(vsf_arch_irq_thread_t)
dcl_simple_class(vsf_arch_irq_request_t)

def_simple_class(vsf_arch_irq_request_t) {
    private_member(
        // should be EventGroupHandle_t event, but avoid to expose header(s) of host os
        void                        * event;
        bool                        is_auto_reset;
        bool                        is_set_pending;
    )
};

def_simple_class(vsf_arch_irq_thread_t) {
    private_member(
        // should be TaskHandle_t thread, but avoid to expose header(s) of host os
        void                        * thread_handle;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_freertos_start(void);

extern bool __vsf_arch_model_low_level_init(void);

#if VSF_ARCH_RTOS_CFG_MODE == VSF_ARCH_RTOS_MODE_SUSPEND_RESUME
extern vsf_arch_prio_t __vsf_arch_model_get_current_priority(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
