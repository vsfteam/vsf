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

#ifndef __VSF_DISTBUS_H__
#define __VSF_DISTBUS_H__

/*============================ INCLUDES ======================================*/
#include "service/vsf_service_cfg.h"

#if VSF_USE_DISTBUS == ENABLED

#include "../simple_stream/vsf_simple_stream.h"

#include "utilities/vsf_utilities.h"

#if     defined(__VSF_DISTBUS_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_DISTBUS_CLASS_IMPLEMENT
#elif   defined(__VSF_DISTBUS_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_DISTBUS_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_simple_class(vsf_distbus_t)
def_simple_class(vsf_distbus_t) {
    public_member(
        vsf_stream_t *stream_rx, *stream_tx;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_distbus_connect(vsf_distbus_t *distbus);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_DISTBUS
#endif      // __VSF_DISTBUS_H__
