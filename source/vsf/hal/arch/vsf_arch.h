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

// TODO: change to vsf_arch_cfg.h
#include "hal/vsf_hal_cfg.h"

#ifdef __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

// __VSF_HEADER_ONLY_SHOW_ARCH_INFO__ is defined outside, only extract arch info from hal
#include "hal/vsf_hal.h"
//__VSF_HEADER_ONLY_SHOW_ARCH_INFO__ will be undefined in vsf_hal.h

#else       // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__

#ifndef __VSF_ARCH_H__
#define __VSF_ARCH_H__

#   define __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
#   include "hal/vsf_hal.h"
//__VSF_HEADER_ONLY_SHOW_ARCH_INFO__ will be undefined in vsf_hal.h

#   include "vsf_arch_abstraction.h"

#   ifdef __cplusplus
extern "C" {
#   endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/



/*! \note initialize architecture specific service
 *  \param none
 *  \retval true initialization succeeded.
 *  \retval false initialization failed
 */
extern bool vsf_arch_init(void);

#   ifdef __cplusplus
}
#   endif

#endif      // __VSF_ARCH_H__
#endif      // __VSF_HEADER_ONLY_SHOW_ARCH_INFO__
/* EOF */
