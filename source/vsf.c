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

#include "./vsf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_USE_APPLET == ENABLED

#   if VSF_APPLET_CFG_LINKABLE == ENABLED
WEAK(vsf_vplt_link)
void * vsf_vplt_link(void *vplt, char *symname)
{
    vsf_vplt_info_t *vplt_info = vplt;
    void *subentry = &vplt_info[1];

    if (vplt_info->final) {

    }
    return NULL;
}
#   endif

WEAK(vsf_applet_ctx)
vsf_applet_ctx_t * vsf_applet_ctx(void)
{
    return NULL;
}

#endif

/* EOF */
