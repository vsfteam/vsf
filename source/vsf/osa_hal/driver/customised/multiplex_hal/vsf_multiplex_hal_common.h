/*****************************************************************************
 *   Copyright(C)2009-2020 by VSF Team                                       *
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

#ifndef __OSA_HAL_DRIVER_MULTIPLEX_COMMON_H__
#define __OSA_HAL_DRIVER_MULTIPLEX_COMMON_H__

/*============================ INCLUDES ======================================*/
#include "osa_hal/vsf_osa_hal_cfg.h"

#if VSF_USE_MULTIPLEX_HAL == ENABLED

#include "kernel/vsf_kernel.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */

#if     defined(__VSF_MULTIPLEX_HAL_CLASS_IMPLEMENT)
#   undef __VSF_MULTIPLEX_HAL_CLASS_IMPLEMENT
#   define __PLOOC_CLASS_IMPLEMENT__

#elif   defined(__VSF_MULTIPLEX_HAL_CLASS_INHERIT)
#   undef __VSF_MULTIPLEX_HAL_CLASS_INHERIT
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_multiplex_hal_t)

def_simple_class(vsf_multiplex_hal_t) {
    protected_member(
        vsf_mutex_t mutex;
        vsf_eda_t *notifier_eda;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
#endif

/* EOF */
