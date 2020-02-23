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
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED
declare_class(vsf_tgui_t)
#include "./vsf_tgui_v.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

bool vsf_tgui_align_mode_is_valid(vsf_tgui_align_mode_t tMode)
{
    if (tMode == VSF_TGUI_ALIGN_FILL) {
        return false;
    } else if ((tMode & VSF_TGUI_ALIGN_LEFT) && (tMode & VSF_TGUI_ALIGN_RIGHT)) {
        return false;
    } else if ((tMode & VSF_TGUI_ALIGN_TOP)  && (tMode & VSF_TGUI_ALIGN_BOTTOM)) {
        return false;
    }

    return true;
}

void vsf_tgui_region_update_with_align(vsf_tgui_region_t* ptDrawRegion, vsf_tgui_region_t* ptResourceRegion, vsf_tgui_align_mode_t tMode)
{
    int16_t iWidth = 0;
    int16_t iHeight = 0;

    VSF_TGUI_ASSERT(ptDrawRegion != NULL);
    VSF_TGUI_ASSERT(ptResourceRegion != NULL);
    VSF_TGUI_ASSERT(vsf_tgui_align_mode_is_valid(tMode));

    if (ptDrawRegion->tSize.iWidth > ptResourceRegion->tSize.iWidth) {
        if (tMode & VSF_TGUI_ALIGN_LEFT) {
        } else if (tMode & VSF_TGUI_ALIGN_RIGHT) {
            iWidth = ptDrawRegion->tSize.iWidth - ptResourceRegion->tSize.iWidth;
        } else /*VSF_TGUI_ALIGN_CENTER*/ {
            iWidth = (ptDrawRegion->tSize.iWidth - ptResourceRegion->tSize.iWidth) / 2;
        }
        ptDrawRegion->tLocation.iX += iWidth;
        ptDrawRegion->tSize.iWidth  = ptResourceRegion->tSize.iWidth;
    } else {
        if (tMode & VSF_TGUI_ALIGN_LEFT) {
        } else if (tMode & VSF_TGUI_ALIGN_RIGHT) {
            iWidth = ptResourceRegion->tSize.iWidth - ptDrawRegion->tSize.iWidth;
        } else /*VSF_TGUI_ALIGN_CENTER*/ {
            iWidth = (ptResourceRegion->tSize.iWidth - ptDrawRegion->tSize.iWidth) / 2;
        }
        ptResourceRegion->tLocation.iX += iWidth;
        ptResourceRegion->tSize.iWidth  = ptDrawRegion->tSize.iWidth;
    }

    if (ptDrawRegion->tSize.iHeight > ptResourceRegion->tSize.iHeight) {
        if (tMode & VSF_TGUI_ALIGN_TOP) {
        } else if (tMode & VSF_TGUI_ALIGN_BOTTOM) {
            iHeight = ptDrawRegion->tSize.iHeight - ptResourceRegion->tSize.iHeight;
        } else /*VSF_TGUI_ALIGN_CENTER*/ {
            iHeight = (ptDrawRegion->tSize.iHeight - ptResourceRegion->tSize.iHeight) / 2;
        }
        ptDrawRegion->tLocation.iY += iHeight;
        ptDrawRegion->tSize.iHeight = ptResourceRegion->tSize.iHeight;
    } else {
        if (tMode & VSF_TGUI_ALIGN_TOP) {
        } else if (tMode & VSF_TGUI_ALIGN_BOTTOM) {
            iHeight = ptResourceRegion->tSize.iHeight - ptDrawRegion->tSize.iHeight;
        } else /*VSF_TGUI_ALIGN_CENTER*/ {
            iHeight = (ptResourceRegion->tSize.iHeight - ptDrawRegion->tSize.iHeight) / 2;
        }
        ptResourceRegion->tLocation.iY  += iHeight;
        ptResourceRegion->tSize.iHeight  = ptDrawRegion->tSize.iHeight;
    }
}


#endif


/* EOF */
