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



#define __VSF_TGUI_CONTROLS_LABEL_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_label.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
#if VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES == ENABLED
static const vsf_tgui_align_mode_t sTilesAlign[] = {
    {VSF_TGUI_ALIGN_LEFT},
    {VSF_TGUI_ALIGN_RIGHT},
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/


fsm_rt_t vsf_tgui_label_v_init(vsf_tgui_label_t* ptLabel)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) label init" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)ptLabel), ptLabel);
#endif
    return vsf_tgui_control_v_init(&ptLabel->use_as__vsf_tgui_control_t);
}

void __vsf_tgui_label_v_rendering(  vsf_tgui_label_t* ptLabel,
                                    vsf_tgui_region_t* ptDirtyRegion,
                                    vsf_tgui_control_refresh_mode_t tMode)
{
    VSF_TGUI_ASSERT(ptLabel != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) label rendering" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)ptLabel), ptLabel);
#endif

    __vk_tgui_control_v_rendering((vsf_tgui_control_t *)ptLabel, ptDirtyRegion, tMode, true);

    if (    (ptLabel->tLabel.tString.pstrText != NULL)
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        &&  (ptLabel->tLabel.tString.s16_size > 0)
#endif
    ) {
        vsf_tgui_control_v_draw_text((vsf_tgui_control_t *)ptLabel,
                                     ptDirtyRegion,
                                     &(ptLabel->tLabel),
                                     ptLabel->use_as__vsf_tgui_v_label_t.chFontIndex,
                                     ptLabel->use_as__vsf_tgui_v_label_t.tFontColor,
                                     ptLabel->tLabel.u4Align);
    }
}

int_fast16_t __vk_tgui_label_get_line_height( const vsf_tgui_label_t* ptLabel)
{
    VSF_TGUI_ASSERT(NULL != ptLabel);

    return (int_fast16_t)
        vsf_tgui_font_get_char_height(
            ptLabel->use_as__vsf_tgui_v_label_t.chFontIndex);
}



vsf_tgui_size_t __vk_tgui_label_v_text_get_size(vsf_tgui_label_t* ptLabel,
                                                uint16_t *phwLineCount,
                                                uint8_t *pchCharHeight)
{
    VSF_TGUI_ASSERT(ptLabel != NULL);

#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED

    ptLabel->tLabel.bIsChanged = false;
    ptLabel->tLabel.tInfoCache.tStringSize = vsf_tgui_text_get_size(ptLabel->use_as__vsf_tgui_v_label_t.chFontIndex,
                                                                    &(ptLabel->tLabel.tString),
                                                                    &(ptLabel->tLabel.tInfoCache.hwLines),
                                                                    &(ptLabel->tLabel.tInfoCache.chCharHeight),
                                                                    ptLabel->tLabel.chInterLineSpace);

    if (NULL != pchCharHeight) {
        *pchCharHeight = ptLabel->tLabel.tInfoCache.chCharHeight;
    }

    return ptLabel->tLabel.tInfoCache.tStringSize;
#else
    vsf_tgui_size_t tSize = vsf_tgui_text_get_size(
                                    ptLabel->use_as__vsf_tgui_v_label_t.chFontIndex,
                                    &(ptLabel->tLabel.tString),
                                    phwLineCount,
                                    pchCharHeight,
                                    ptLabel->tLabel.chInterLineSpace);

    return tSize;
#endif

}


vsf_tgui_size_t __vk_tgui_label_v_get_minimal_rendering_size(vsf_tgui_label_t* ptLabel)
{
    VSF_TGUI_ASSERT(ptLabel != NULL);
#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    vsf_tgui_size_t tSize = ptLabel->tLabel.tInfoCache.tStringSize;
#else
    vsf_tgui_size_t tSize = __vk_tgui_label_v_text_get_size(ptLabel, NULL, NULL);
#endif

#if VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES == ENABLED
    if (!ptLabel->use_as__vsf_tgui_v_label_t.bIsUseRawView) {
        for (int i = 0; i < dimof(sTilesAlign); i++) {
            vsf_tgui_region_t tRegion;
            vsf_tgui_tile_get_root( &c_tLabelAdditionalTiles.tTiles[i], &tRegion);

            switch (sTilesAlign[i]) {
                case VSF_TGUI_ALIGN_RIGHT:
                case VSF_TGUI_ALIGN_LEFT:
                    tSize.iWidth += tRegion.tSize.iWidth;
                    tSize.iHeight = max(tSize.iHeight, tRegion.tSize.iHeight);
                    break;
                /*
                case VSF_TGUI_ALIGN_TOP:
                    break;
                case VSF_TGUI_ALIGN_BOTTOM:
                    break;

                case VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_LEFT:
                    break;
                case VSF_TGUI_ALIGN_TOP | VSF_TGUI_ALIGN_RIGHT:
                    break;

                case VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_LEFT:
                    break;
                case VSF_TGUI_ALIGN_BOTTOM | VSF_TGUI_ALIGN_RIGHT:
                    break;
                */
            }
        }
    }
#endif

    return tSize;
}

fsm_rt_t vsf_tgui_label_v_rendering(vsf_tgui_label_t* ptLabel,
                                    vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                    vsf_tgui_control_refresh_mode_t tMode)
{
    VSF_TGUI_ASSERT(ptLabel != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) label rendering" VSF_TRACE_CFG_LINEEND,
        vsf_tgui_control_get_node_name((vsf_tgui_control_t*)ptLabel), ptLabel);
#endif

    if (!ptLabel->bIsNoBackgroundColor) {
        vsf_tgui_control_t* control_ptr = (vsf_tgui_control_t*)ptLabel;
        vsf_tgui_region_t tRegion = { 0 };

        tRegion.tSize = *vsf_tgui_control_get_size(control_ptr);

#if VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES == ENABLED
        if (!ptLabel->use_as__vsf_tgui_v_label_t.bIsUseRawView) {
            vsf_tgui_tile_t* ptTile;
            vsf_tgui_region_t tLeftRegion;
            vsf_tgui_region_t tRightRegion;

            ptTile = vsf_tgui_tile_get_root(&c_tLabelAdditionalTiles._.tLeft, &tLeftRegion);
            VSF_TGUI_ASSERT(ptTile != NULL);

            ptTile = vsf_tgui_tile_get_root(&c_tLabelAdditionalTiles._.tRight, &tRightRegion);
            VSF_TGUI_ASSERT(ptTile != NULL);

            tRegion.tLocation.iX += tLeftRegion.tSize.iWidth;
            tRegion.tSize.iWidth -= tLeftRegion.tSize.iWidth + tRightRegion.tSize.iWidth;
        }
#endif

        vsf_tgui_control_v_draw_rect(   control_ptr,
                                        ptDirtyRegion,
                                        &tRegion,
                                        control_ptr->tBackgroundColor);
    }

    if (!ptLabel->use_as__vsf_tgui_v_label_t.bIsUseRawView) {
#if VSF_TGUI_CFG_SV_LABEL_ADDITIONAL_TILES == ENABLED
        vsf_tgui_control_t* control_ptr = &ptLabel->use_as__vsf_tgui_control_t;
        uint_fast8_t tRate = vsf_tgui_control_v_get_tile_trans_rate(control_ptr);
        uint_fast8_t tBackGroundRate = vsf_tgui_sv_color_get_trans_rate(control_ptr->tBackgroundColor);
        vsf_tgui_control_v_set_tile_trans_rate(control_ptr, tBackGroundRate);


        for (int i = 0; i < dimof(sTilesAlign); i++) {
            vsf_tgui_control_v_draw_tile(   control_ptr,
                                            ptDirtyRegion,
                                            &c_tLabelAdditionalTiles.tTiles[i],
                                            sTilesAlign[i]);
        }

        vsf_tgui_control_v_set_tile_trans_rate(control_ptr, tRate);
#endif
    }

    __vsf_tgui_label_v_rendering(ptLabel, ptDirtyRegion, tMode);

    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_label_v_depose(vsf_tgui_label_t* ptLabel)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_label_v_update(vsf_tgui_label_t* ptLabel)
{
    return fsm_rt_cpl;
}


#endif


/* EOF */
