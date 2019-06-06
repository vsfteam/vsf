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
#include "service/vsf_service_cfg.h"

#include "./vsf_service.h"

/*============================ MACROS ========================================*/
#if !defined(VSF_PBUF_ADAPTERS) && VSF_USE_SERVICE_STREAM == ENABLED
#warning No defined VSF_PBUF_ADAPTERS, all allocated pbuf objects need to be \
free-ed explicitly to their origins or General PBUF Pool should be Enabled.
#endif

#if defined(VSF_SERVICE_CFG_INSERTION)
VSF_SERVICE_CFG_INSERTION
#endif

#if defined(VSF_SERVICE_CFG_DEPENDENCY)
#include VSF_SERVICE_CFG_DEPENDENCY
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

//! user must implement this interface
extern vsf_mem_t vsf_service_req___heap_memory_buffer__from_usr(void);

/*============================ IMPLEMENTATION ================================*/

WEAK void vsf_service_init(void)
{
#if VSF_USE_HEAP == ENABLED
    vsf_heap_init();
    vsf_heap_add_memory(vsf_service_req___heap_memory_buffer__from_usr());
#endif

#if VSF_USE_PBUF == ENABLED && (                                                \
            VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED                         \
        ||  defined(VSF_PBUF_ADAPTERS))

#   ifndef VSF_PBUF_ADAPTERS
#       define VSF_PBUF_ADAPTERS
#   endif
    /*! \brief register pbuf adapters
     */
    do {
        static const vsf_pbuf_adapter_t c_tpbufAdapters [] = {
#   if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED
            vsf_pbuf_pool_adapter(0, &g_tGeneralPBUFPool),
#   endif
            VSF_PBUF_ADAPTERS
        };

        vsf_adapter_register(c_tpbufAdapters, UBOUND(c_tpbufAdapters));
    } while(0);
#endif

#if VSF_USE_SERVICE_STREAM == ENABLED
    //! initialise pbuf pool
    vsf_service_stream_init();
#endif
}

/* EOF */
