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
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#define __VSF_TGUI_CONTROLS_CONTAINER_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_container.h"

/*============================ MACROS ========================================*/
// Support Panel additional tiles
#ifndef VSF_TGUI_CFG_SV_CONTAINER_ADDITIONAL_TILES
#   define VSF_TGUI_CFG_SV_CONTAINER_ADDITIONAL_TILES 			    DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
#if VSF_TGUI_CFG_SV_CONTAINER_ADDITIONAL_TILES == ENABLED
static const vsf_tgui_align_mode_t s_tTilesAlign[] = {
    {VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_LEFT},
    {VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_RIGHT},
    {VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_LEFT},
    {VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_RIGHT},
};
#endif

/*============================ PROTOTYPES ====================================*/
#if VSF_TGUI_CFG_SV_CONTAINER_ADDITIONAL_TILES == ENABLED
extern const vsf_tgui_sv_container_corner_tiles_t g_tContainerCornerTiles;
#endif


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/


fsm_rt_t vsf_tgui_container_v_init(vsf_tgui_container_t* container_ptr)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) container init" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t *)container_ptr), container_ptr);
#endif
    return fsm_rt_cpl;
}


fsm_rt_t __vk_tgui_container_v_rendering(vsf_tgui_container_t* container_ptr,
                                        vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t tMode,
                                        vsf_tgui_sv_color_t tBackground)
{
    //fsm_rt_t result = fsm_rt_cpl;

    VSF_TGUI_ASSERT(container_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

    if (!container_ptr->bIsNoBackgroundColor) {
        vsf_tgui_control_t* control_ptr = (vsf_tgui_control_t*)container_ptr;
        vsf_tgui_sv_color_t tColor = tBackground;
        vsf_tgui_region_t tRegion = { 0 };

        tRegion.tSize = *vsf_tgui_control_get_size(control_ptr);
#if VSF_TGUI_CFG_SV_CONTAINER_ADDITIONAL_TILES == ENABLED
        if (container_ptr->use_as__vsf_tgui_v_container_t.bIsShowCornerTile) {
			vsf_tgui_tile_t* ptTile;
			vsf_tgui_region_t tTopLeftRegion;
			vsf_tgui_region_t tTopRightRegion;
			vsf_tgui_region_t tBottomLeftRegion;
			vsf_tgui_region_t tBottomRightRegion;

			vsf_tgui_region_t tMiddleRegion;
			vsf_tgui_region_t tLeftRegion;
			vsf_tgui_region_t tRightRegion;

			ptTile = vsf_tgui_tile_get_root(&g_tContainerCornerTiles._.tTopLeft, &tTopLeftRegion);
			VSF_TGUI_ASSERT(ptTile != NULL);
			ptTile = vsf_tgui_tile_get_root(&g_tContainerCornerTiles._.tTopRight, &tTopRightRegion);
			VSF_TGUI_ASSERT(ptTile != NULL);
			ptTile = vsf_tgui_tile_get_root(&g_tContainerCornerTiles._.tBottomLeft, &tBottomLeftRegion);
			VSF_TGUI_ASSERT(ptTile != NULL);
			ptTile = vsf_tgui_tile_get_root(&g_tContainerCornerTiles._.tBottomRight, &tBottomRightRegion);
			VSF_TGUI_ASSERT(ptTile != NULL);

			VSF_TGUI_ASSERT(tTopLeftRegion.tSize.iWidth == tTopRightRegion.tSize.iWidth);
			VSF_TGUI_ASSERT(tTopLeftRegion.tSize.iHeight == tTopRightRegion.tSize.iHeight);
			VSF_TGUI_ASSERT(tTopLeftRegion.tSize.iWidth == tTopRightRegion.tSize.iWidth);
			VSF_TGUI_ASSERT(tTopLeftRegion.tSize.iHeight == tTopRightRegion.tSize.iHeight);
			VSF_TGUI_ASSERT(tTopLeftRegion.tSize.iWidth == tBottomLeftRegion.tSize.iWidth);
			VSF_TGUI_ASSERT(tTopLeftRegion.tSize.iHeight == tBottomLeftRegion.tSize.iHeight);
			VSF_TGUI_ASSERT(tTopLeftRegion.tSize.iWidth == tBottomRightRegion.tSize.iWidth);
			VSF_TGUI_ASSERT(tTopLeftRegion.tSize.iHeight == tBottomRightRegion.tSize.iHeight);

			tMiddleRegion.tLocation.iX = tTopLeftRegion.tSize.iWidth;
			tMiddleRegion.tLocation.iY = 0;
			tMiddleRegion.tSize.iWidth = tRegion.tSize.iWidth - 2 * tTopLeftRegion.tSize.iWidth;
			tMiddleRegion.tSize.iHeight = tRegion.tSize.iHeight;

			tLeftRegion.tLocation.iX = 0;
			tLeftRegion.tLocation.iY = tTopLeftRegion.tSize.iHeight;
			tLeftRegion.tSize.iWidth = tTopLeftRegion.tSize.iWidth;
			tLeftRegion.tSize.iHeight = tRegion.tSize.iHeight - 2 * tTopLeftRegion.tSize.iHeight;

			tRightRegion.tLocation.iX = tRegion.tSize.iWidth - tTopLeftRegion.tSize.iWidth;
			tRightRegion.tLocation.iY = tTopLeftRegion.tSize.iHeight;
			tRightRegion.tSize.iWidth = tTopLeftRegion.tSize.iWidth;
			tRightRegion.tSize.iHeight = tRegion.tSize.iHeight - 2 * tTopLeftRegion.tSize.iHeight;

			vsf_tgui_control_v_draw_rect(control_ptr,
				ptDirtyRegion,
				&tMiddleRegion,
				tColor);
			vsf_tgui_control_v_draw_rect(control_ptr,
				ptDirtyRegion,
				&tLeftRegion,
				tColor);
			vsf_tgui_control_v_draw_rect(control_ptr,
				ptDirtyRegion,
				&tRightRegion,
				tColor);
		} else 
#endif
        {
			vsf_tgui_control_v_draw_rect(control_ptr,
				ptDirtyRegion,
				&tRegion,
				tColor);
        }
    }

#if VSF_TGUI_CFG_SV_CONTAINER_ADDITIONAL_TILES == ENABLED
    if (container_ptr->use_as__vsf_tgui_v_container_t.bIsShowCornerTile) {
        uint_fast8_t tRate = vsf_tgui_control_v_get_tile_trans_rate((const vsf_tgui_control_t *)container_ptr);
        uint_fast8_t tBackGroundRate = vsf_tgui_sv_color_get_trans_rate(tBackground);

        vsf_tgui_control_v_set_tile_trans_rate((vsf_tgui_control_t *)container_ptr, tBackGroundRate);

        for (int i = 0; i < dimof(s_tTilesAlign); i++) {
            vsf_tgui_control_v_draw_tile(   (vsf_tgui_control_t*)container_ptr,
                                            ptDirtyRegion,
                                            &g_tContainerCornerTiles.tTiles[i],
                                            s_tTilesAlign[i]);
        }

        vsf_tgui_control_v_set_tile_trans_rate((vsf_tgui_control_t *)container_ptr, tRate);
    }
#endif

    return __vk_tgui_control_v_rendering((vsf_tgui_control_t *)container_ptr, ptDirtyRegion, tMode, true);
}


fsm_rt_t vsf_tgui_container_v_rendering(vsf_tgui_container_t* container_ptr,
                                        vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t tMode)
{
    //fsm_rt_t result = fsm_rt_cpl;

    VSF_TGUI_ASSERT(container_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) container rendering" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t *)container_ptr), container_ptr);
#endif

    return __vk_tgui_container_v_rendering(container_ptr, ptDirtyRegion, tMode,  container_ptr->tBackgroundColor);
}

fsm_rt_t vsf_tgui_container_v_post_rendering(vsf_tgui_container_t* container_ptr,
                                            vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                            vsf_tgui_control_refresh_mode_t tMode)
{
    return fsm_rt_cpl;
} 

fsm_rt_t vsf_tgui_container_v_depose(vsf_tgui_container_t* container_ptr)
{
    return fsm_rt_cpl;
}


fsm_rt_t vsf_tgui_container_v_update(vsf_tgui_container_t* container_ptr)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
