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

#include "service/vsf_service_cfg.h"

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_PE == ENABLED

#define __VSF_PELOADER_CLASS_IMPLEMENT
#define __VSF_LOADER_CLASS_INHERIT__
#include "../vsf_loader.h"

// for vsf_trace
#include "service/vsf_service.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const struct vsf_loader_op_t vsf_peloader_op = {
    .fn_load                = (int (*)(vsf_loader_t *, vsf_loader_target_t *))vsf_peloader_load,
    .fn_cleanup             = (void (*)(vsf_loader_t *))vsf_peloader_cleanup,
    .fn_call_init_array     = (int (*)(vsf_loader_t *))vsf_peloader_call_init_array,
    .fn_call_fini_array     = (void (*)(vsf_loader_t *))vsf_peloader_call_fini_array,
};

/*============================ IMPLEMENTATION ================================*/

int vsf_peloader_load(vsf_peloader_t *peloader, vsf_loader_target_t *target)
{
    return -1;
}

void vsf_peloader_cleanup(vsf_peloader_t *peloader)
{
}

int vsf_peloader_call_init_array(vsf_peloader_t *peloader)
{
    return -1;
}

void vsf_peloader_call_fini_array(vsf_peloader_t *peloader)
{
}

#endif      // VSF_USE_LOADER && VSF_LOADER_USE_ELF
