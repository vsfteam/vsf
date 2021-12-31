/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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
#include "hal/vsf_hal_cfg.h"
#include "./common.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/

#define SWI_COUNT           6
#define SWI_START_IRQ       43

#define __W600_SWI(__N, __VALUE)                                                \
    void SWI##__N##_IRQHandler(void)                                            \
    {                                                                           \
        if (__w600_common.swi[__N].handler != NULL) {                           \
            __w600_common.swi[__N].handler(__w600_common.swi[__N].pparam);      \
        }                                                                       \
    }

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct __w600_common_t {
    struct {
        vsf_swi_handler_t *handler;
        void *pparam;
    } swi[SWI_COUNT];
};
typedef struct __w600_common_t __w600_common_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static __w600_common_t __w600_common;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// SWI
VSF_MREPEAT(SWI_COUNT, __W600_SWI, NULL)

vsf_err_t vsf_drv_swi_init( uint_fast8_t idx, 
                            vsf_arch_prio_t priority,
                            vsf_swi_handler_t *handler, 
                            void *pparam)
{
    VSF_HAL_ASSERT(idx < SWI_COUNT);

    if (handler != NULL) {
        __w600_common.swi[idx].handler = handler;
        __w600_common.swi[idx].pparam = pparam;

        NVIC_SetPriority(SWI_START_IRQ + idx, priority);
        NVIC_EnableIRQ(SWI_START_IRQ + idx);
    } else {
        NVIC_DisableIRQ(SWI_START_IRQ + idx);
    }
    return VSF_ERR_NONE;
}

void vsf_drv_swi_trigger(uint_fast8_t idx)
{
    VSF_HAL_ASSERT(idx < SWI_COUNT);

    NVIC_SetPendingIRQ(SWI_START_IRQ + idx);
}
