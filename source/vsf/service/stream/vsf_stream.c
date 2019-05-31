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

#if VSF_USE_SERVICE_STREAM == ENABLED

#define __VSF_STREAM_CLASS_IMPLEMENT
#include "./vsf_stream_base.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/


void vsf_service_stream_init(void)
{
#if VSF_STREAM_CFG_GENERAL_PBUF_POOL == ENABLED

#   if defined(GENERAL_PBUF_POOL_BLOCK_COUNT)
    //! initialise pbuf pool
    init_pbuf_pool( general_pbuf_pool_t, 
                    &g_tGeneralPBUFPool,
                    0,   
                    GENERAL_PBUF_POOL_BLOCK_COUNT);
#   else
    prepare_pbuf_pool( general_pbuf_pool_t, &g_tGeneralPBUFPool, 0);
#   endif
#endif
}



#endif
/* EOF */
