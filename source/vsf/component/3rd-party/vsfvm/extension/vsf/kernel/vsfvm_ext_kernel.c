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

/*============================ INCLUDES ======================================*/
#include "vsf_vm_cfg.h"

#if (VSFVM_CFG_RUNTIME_EN == ENABLED) || (VSFVM_CFG_COMPILER_EN == ENABLED)

#define VSF_EDA_CLASS_INHERIT
// TODO: use dedicated include
#include "vsf.h"

#define VSFVM_RUNTIME_INHERIT
#include "common/vsfvm_common.h"
#include "runtime/vsfvm_runtime.h"
#include "extension/std/vsfvm_ext_std.h"
#include "./vsfvm_ext_kernel.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct vsfvm_ext_kernel_t {
    implement(vsfvm_ext_t)
};
typedef struct vsfvm_ext_kernel_t vsfvm_ext_kernel_t;

enum {
    VSFVM_KERNEL_EXTFUNC_DELAY_MS = 0,
    VSFVM_KERNEL_EXTFUNC_NUM,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsfvm_ext_kernel_t vsfvm_ext_kernel;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSFVM_CFG_RUNTIME_EN == ENABLED

static void vsfvm_ext_kernel_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    if (evt == VSF_EVT_TIMER) {
        vsfvm_thread_ready((vsfvm_thread_t *)eda);
    }
}

static vsfvm_ret_t vsfvm_ext_kernel_delay_ms(vsfvm_thread_t *thread)
{
    if (!thread->fn.evthandler) {
        vsfvm_var_t *timeout = vsfvm_get_func_argu_ref(thread, 0);

        thread->fn.evthandler = vsfvm_ext_kernel_evthandler;
        vsf_teda_init(&thread->use_as__vsf_teda_t, VSFVM_CFG_PRIORITY, false);
        vsf_teda_set_timer_ex(&thread->use_as__vsf_teda_t, vsf_systimer_ms_to_tick(timeout->uval32));
        return VSFVM_RET_PEND;
    } else {
        vsf_eda_fini(&thread->use_as__vsf_eda_t);
        thread->fn.evthandler = NULL;
        return VSFVM_RET_FINISHED;
    }
}

#endif

#if VSFVM_CFG_COMPILER_EN == ENABLED
extern const vsfvm_ext_op_t vsfvm_ext_kernel_op;
static const vsfvm_lexer_sym_t vsfvm_ext_kernel_sym[] = {
    VSFVM_LEXERSYM_EXTFUNC("delay_ms", &vsfvm_ext_kernel_op, NULL, NULL, 1, VSFVM_KERNEL_EXTFUNC_DELAY_MS),
};
#endif

#if VSFVM_CFG_RUNTIME_EN == ENABLED
static const vsfvm_extfunc_t vsfvm_ext_kernel_func[VSFVM_KERNEL_EXTFUNC_NUM] = {
    [VSFVM_KERNEL_EXTFUNC_DELAY_MS] = VSFVM_EXTFUNC(vsfvm_ext_kernel_delay_ms, 1),
};
#endif

static const vsfvm_ext_op_t vsfvm_ext_kernel_op = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .name = "kernel",
    .sym = vsfvm_ext_kernel_sym,
    .sym_num = dimof(vsfvm_ext_kernel_sym),
#endif
#if VSFVM_CFG_RUNTIME_EN == ENABLED
    .init = NULL,
    .fini = NULL,
    .func = (vsfvm_extfunc_t *)vsfvm_ext_kernel_func,
#endif
    .func_num = dimof(vsfvm_ext_kernel_func),
};

void vsfvm_ext_register_kernel(void)
{
    memset(&vsfvm_ext_kernel, 0, sizeof(vsfvm_ext_kernel));

    vsfvm_ext_kernel.op = &vsfvm_ext_kernel_op;
    vsfvm_register_ext(&vsfvm_ext_kernel.use_as__vsfvm_ext_t);
}

#endif      // VSFVM_CFG_RUNTIME_EN || VSFVM_CFG_COMPILER_EN
