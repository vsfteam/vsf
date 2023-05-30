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
#include "hal/vsf_hal.h"

#include "./arch/vsf_arch.h"
#include "./driver/driver.h"
#include "kernel/vsf_eda.h"

/*============================ MACROS ========================================*/

#ifndef VSF_HAL_USE_DEFAULT_IRQ_ENTER
#   define VSF_HAL_USE_DEFAULT_IRQ_ENTER            ENABLED
#endif

#ifndef VSF_HAL_USE_DEFAULT_IRQ_LEVEL
#   define VSF_HAL_USE_DEFAULT_IRQ_LEVEL            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern bool vsf_driver_init(void);

/*============================ IMPLEMENTATION ================================*/

WEAK(vsf_app_driver_init)
bool vsf_app_driver_init(void)
{
    return true;
}

WEAK(vsf_driver_poll)
bool vsf_driver_poll(void)
{
    return true;
}

WEAK(vsf_hal_irq_enter)
uintptr_t vsf_hal_irq_enter(void)
{
#if VSF_HAL_USE_DEFAULT_IRQ_ENTER == ENABLED
    return vsf_irq_enter();
#else
    VSF_HAL_ASSERT(0);
    return 0;
#endif
}

WEAK(vsf_hal_irq_leave)
void vsf_hal_irq_leave(uintptr_t ctx)
{
#if VSF_HAL_USE_DEFAULT_IRQ_LEVEL == ENABLED
    vsf_irq_leave(ctx);
#else
    VSF_HAL_ASSERT(0);
#endif
}

/*! \note initialize level 0/1 hardware abstract layer
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
bool vsf_hal_init( void )
{

    if (!vsf_driver_init()) {

        return false;
    }

    if (!vsf_app_driver_init()) {

        return false;
    }

    return true;
}

/* EOF */
