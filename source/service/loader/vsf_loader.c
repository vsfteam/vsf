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

#if VSF_USE_LOADER == ENABLED

#define __VSF_LOADER_CLASS_IMPLEMENT
#include "./vsf_loader.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_loader_cleanup(vsf_loader_t *loader)
{
    VSF_SERVICE_ASSERT(loader != NULL);
    VSF_SERVICE_ASSERT(loader->cfg != NULL);
    VSF_SERVICE_ASSERT(loader->cfg->fn_free != NULL);

    if (loader->text != NULL) {
        loader->cfg->fn_free(loader, loader->text);
        loader->text = NULL;
    }
    if (loader->bss != NULL) {
        loader->cfg->fn_free(loader, loader->bss);
        loader->bss = NULL;
    }
    if (loader->data != NULL) {
        loader->cfg->fn_free(loader, loader->data);
        loader->data = NULL;
    }
    if (loader->rodata != NULL) {
        loader->cfg->fn_free(loader, loader->rodata);
        loader->rodata = NULL;
    }
    if (loader->got != NULL) {
        loader->cfg->fn_free(loader, loader->got);
        loader->got = NULL;
    }
}

#endif      // VSF_USE_LOADER
