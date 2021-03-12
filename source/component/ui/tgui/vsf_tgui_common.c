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

/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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
#include "./vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED
#include "./vsf_tgui.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern vsf_tgui_size_t vsf_tgui_idx_root_tile_get_size(const vsf_tgui_tile_t* ptTile);
/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_TGUI_IDX_ROOT_TILE_GET_SIZE
WEAK(vsf_tgui_idx_root_tile_get_size)
vsf_tgui_size_t vsf_tgui_idx_root_tile_get_size(const vsf_tgui_tile_t* ptTile)
{
    vsf_tgui_size_t tSize = {0};
    VSF_TGUI_ASSERT(0);
    return tSize;
}
#endif

vsf_tgui_size_t vsf_tgui_root_tile_get_size(const vsf_tgui_tile_t* ptTile)
{
    VSF_TGUI_ASSERT(ptTile != NULL);
    VSF_TGUI_ASSERT(ptTile->_.tCore.Attribute.bIsRootTile == 1);

    if (ptTile->_.tCore.Attribute.u2RootTileType == 1) {   //vsf_tgui_tile_idx_root_t
        return vsf_tgui_idx_root_tile_get_size(ptTile);
    } else {                                            //vsf_tgui_tile_buf_root_t
        return ptTile->tBufRoot.tSize;
    }
}


vsf_tgui_tile_t* vsf_tgui_tile_get_root(    const vsf_tgui_tile_t* ptTile,
                                            vsf_tgui_region_t* region_ptr)
{
    VSF_TGUI_ASSERT(ptTile != NULL);
    VSF_TGUI_ASSERT(region_ptr != NULL);

    if (ptTile->_.tCore.Attribute.bIsRootTile == 0) {      //vsf_tgui_tile_child_t

        const vsf_tgui_tile_child_t* ptChildTile = &ptTile->tChild;
        ptTile = vsf_tgui_tile_get_root((const vsf_tgui_tile_t *)ptChildTile->parent_ptr, region_ptr);


        region_ptr->tLocation.iX += ptChildTile->tLocation.iX;
        region_ptr->tLocation.iY += ptChildTile->tLocation.iY;
        region_ptr->tSize = ptChildTile->tSize;

        VSF_TGUI_ASSERT(ptTile != NULL);
        VSF_TGUI_ASSERT(ptTile->_.tCore.Attribute.bIsRootTile == 1);
    } else {
        region_ptr->tLocation.iX = 0;
        region_ptr->tLocation.iY = 0;

        region_ptr->tSize = vsf_tgui_root_tile_get_size(ptTile);
    }

    return (vsf_tgui_tile_t *)ptTile;
}

bool vsf_tgui_tile_is_root(const vsf_tgui_tile_t* ptTile)
{
    VSF_TGUI_ASSERT(ptTile != NULL);
    return ptTile->_.tCore.Attribute.bIsRootTile == 1;
}


void vsf_tgui_text_set( vsf_tgui_text_info_t *ptTextInfo,
                        const vsf_tgui_string_t *pstrNew)
{
    VSF_TGUI_ASSERT(NULL != ptTextInfo);
    VSF_TGUI_ASSERT(NULL != pstrNew);

    ptTextInfo->tString = (*pstrNew);
#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING
    ptTextInfo->bIsChanged = true;
#endif
}

#endif


/* EOF */
