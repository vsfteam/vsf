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

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_port.h"
#include "../../controls/vsf_tgui_controls.h"
#include "../../utilities/vsf_tgui_text.h"
#include "../vsf_tgui_v.h"

/*============================ MACROS ========================================*/
#ifndef VSF_TGUI_SV_CFG_DRAW_LOG
#define VSF_TGUI_SV_CFG_DRAW_LOG      DISABLED
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static bool update_location_and_resouce_region(
                                        const vsf_tgui_control_t* control_ptr,
                                        const vsf_tgui_region_t* ptDirtyRegion,
                                        const vsf_tgui_region_t* ptDrawInCtrlRegion,
                                        vsf_tgui_location_t* ptRealLocation,
                                        vsf_tgui_region_t* ptResourceRegion)
{
    vsf_tgui_region_t tRealRawRegion;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptDrawInCtrlRegion != NULL);
    VSF_TGUI_ASSERT(ptRealLocation != NULL);
    VSF_TGUI_ASSERT(ptResourceRegion != NULL);

#if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

    if (!vsf_tgui_region_intersect(&tRealRawRegion, ptDrawInCtrlRegion, ptDirtyRegion)) {
        return false;
    }

    ptResourceRegion->tLocation.iX += tRealRawRegion.tLocation.iX - ptDrawInCtrlRegion->tLocation.iX;
    ptResourceRegion->tLocation.iY += tRealRawRegion.tLocation.iY - ptDrawInCtrlRegion->tLocation.iY;
    ptResourceRegion->tSize = tRealRawRegion.tSize;

    *ptRealLocation = tRealRawRegion.tLocation;
#else
    *ptRealLocation = ptDrawInCtrlRegion->tLocation;
#endif

    if (ptResourceRegion->tSize.iHeight == 0 || ptResourceRegion->tSize.iWidth == 0) {
        return false;
    }

    vsf_tgui_control_get_absolute_location(control_ptr, ptRealLocation);

    return true;
}

static bool update_location_and_resouce_region_with_mode(
                                                const vsf_tgui_control_t* control_ptr,
                                                const vsf_tgui_region_t* ptDirtyRegion,
                                                const vsf_tgui_align_mode_t tMode,
                                                vsf_tgui_location_t* ptRealLocationBuffer,
                                                vsf_tgui_region_t* ptResourceRegion)
{
    vsf_tgui_region_t tDrawInCtrlRegion = {0};

    //tDrawInCtrlRegion.tLocation = *ptRealLocation;
    tDrawInCtrlRegion.tSize = *vsf_tgui_control_get_size(control_ptr);
    //tDrawInCtrlRegion.tSize.iHeight -= tDrawInCtrlRegion.tLocation.iY;
    //tDrawInCtrlRegion.tSize.iWidth -= tDrawInCtrlRegion.tLocation.iX;
    // resize draw region and resource region
    vsf_tgui_region_update_with_align(&tDrawInCtrlRegion, ptResourceRegion, tMode);

    return update_location_and_resouce_region(control_ptr, ptDirtyRegion, &tDrawInCtrlRegion, ptRealLocationBuffer, ptResourceRegion);
}

uint_fast8_t vsf_tgui_control_v_get_tile_trans_rate(const vsf_tgui_control_t* control_ptr)
{
    bool bIsTileTransparency;
    uint_fast8_t chTileTransparencyRate;

    VSF_TGUI_ASSERT(control_ptr != NULL);

	if (vsf_tgui_control_is_container(control_ptr)) {
		const vsf_msgt_node_t* node_ptr = &control_ptr->use_as__vsf_msgt_node_t;
		const vsf_tgui_container_t* container_ptr = (const vsf_tgui_container_t*)node_ptr;

        bIsTileTransparency = container_ptr->use_as__vsf_tgui_v_container_t.bIsTileTransparency;
        chTileTransparencyRate = container_ptr->use_as__vsf_tgui_v_container_t.chTileTransparencyRate;
    } else {
        bIsTileTransparency = control_ptr->use_as__vsf_tgui_v_control_t.bIsTileTransparency;
        chTileTransparencyRate = control_ptr->use_as__vsf_tgui_v_control_t.chTileTransparencyRate;
    }

    if (bIsTileTransparency) {
        return chTileTransparencyRate;
    } else {
        return 0xFF;
    }
}

void vsf_tgui_control_v_set_tile_trans_rate(vsf_tgui_control_t* control_ptr, uint_fast8_t chTileTransparencyRate)
{
    bool bIsTileTransparency;

    VSF_TGUI_ASSERT(control_ptr != NULL);

    if (chTileTransparencyRate == 0xFF) {
        bIsTileTransparency = 0;
    } else {
        bIsTileTransparency = 1;
    }

	if (vsf_tgui_control_is_container(control_ptr)) {
		const vsf_msgt_node_t* node_ptr = &control_ptr->use_as__vsf_msgt_node_t;
		vsf_tgui_container_t* container_ptr = (vsf_tgui_container_t *)node_ptr;

        container_ptr->use_as__vsf_tgui_v_container_t.bIsTileTransparency = bIsTileTransparency;
        container_ptr->use_as__vsf_tgui_v_container_t.chTileTransparencyRate = chTileTransparencyRate;
    } else {
        control_ptr->use_as__vsf_tgui_v_control_t.bIsTileTransparency = bIsTileTransparency;
        control_ptr->use_as__vsf_tgui_v_control_t.chTileTransparencyRate = chTileTransparencyRate;
    }
}


void vsf_tgui_control_v_draw_rect(  const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* ptDirtyRegion,
                                    const vsf_tgui_region_t* ptRectRegion,
                                    const vsf_tgui_sv_color_t tColor)
{
    vsf_tgui_region_t tResourceRegion;
    vsf_tgui_location_t tRealLocation = {0,0};
    vsf_tgui_sv_color_t tTmpColor = tColor;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

    //Noto: Temporary code, Will be removed when style is supported
    if (!tTmpColor.bIsColorTransparency) {
        vsf_tgui_sv_color_set_trans_rate(&tTmpColor, 0xFF);
    }

    if (update_location_and_resouce_region(control_ptr, ptDirtyRegion, ptRectRegion, &tRealLocation, &tResourceRegion)) {
#if (VSF_TGUI_SV_CFG_DRAW_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%s draw rect(0x%x) in (x:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
            control_ptr->use_as__vsf_msgt_node_t.node_name_ptr, tColor.tColor.wValue, tRealLocation.iX, tRealLocation.iY, tResourceRegion.tSize.iWidth, tResourceRegion.tSize.iHeight);
#endif

        vsf_tgui_sv_port_draw_rect(&tRealLocation, &tResourceRegion.tSize, tTmpColor);
    }
}


void vsf_tgui_control_v_draw_tile(  const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* ptDirtyRegion,
                                    const vsf_tgui_tile_t* ptTile,
                                    const vsf_tgui_align_mode_t tMode)
{
    vsf_tgui_region_t tResourceRegion;
    vsf_tgui_location_t tRealLocation = {0, 0};
    uint8_t chTileTransparencyRate;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    VSF_TGUI_ASSERT(ptTile != NULL);

    ptTile = vsf_tgui_tile_get_root(ptTile, &tResourceRegion);
    VSF_TGUI_ASSERT(ptTile != NULL);

    if (update_location_and_resouce_region_with_mode(control_ptr, ptDirtyRegion, tMode, &tRealLocation, &tResourceRegion)) {
#if (VSF_TGUI_SV_CFG_DRAW_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%s draw tile(0x%p) in (x:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
            control_ptr->use_as__vsf_msgt_node_t.node_name_ptr, ptTile, tRealLocation.iX, tRealLocation.iY, tResourceRegion.tSize.iWidth, tResourceRegion.tSize.iHeight);
#endif

        chTileTransparencyRate = vsf_tgui_control_v_get_tile_trans_rate(control_ptr);
        vsf_tgui_sv_port_draw_root_tile(&tRealLocation,
                                        &tResourceRegion.tLocation,
                                        &tResourceRegion.tSize,
                                        ptTile,
                                        chTileTransparencyRate);
    }
}

static void vsf_tgui_sv_text_draw(vsf_tgui_location_t* ptLocation,
                               vsf_tgui_region_t* ptRelativeRegion,
                               vsf_tgui_string_t* ptString,
                               const uint8_t chFontIndex,
                               const vsf_tgui_sv_color_t tColor,
                               int_fast8_t chInterLineSpace)
{
    vsf_tgui_region_t tVisualCharRegion = { 0 };
    vsf_tgui_location_t tLocation;
    vsf_tgui_region_t tDirtyCharRegion = { 0 };
    vsf_tgui_location_t tRelativeCharLocation = { 0 };
    uint32_t wChar;
    size_t tCharOffset = 0;

    VSF_TGUI_ASSERT(ptLocation != NULL);
    VSF_TGUI_ASSERT(ptRelativeRegion != NULL);
    VSF_TGUI_ASSERT(ptString != NULL);
    VSF_TGUI_ASSERT(ptString->pstrText != NULL);

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    VSF_TGUI_ASSERT(ptString->s16_size > 0);
#endif

    VSF_TGUI_ASSERT((0 <= ptLocation->iX) && (ptLocation->iX < VSF_TGUI_HOR_MAX));  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= ptLocation->iY) && (ptLocation->iY < VSF_TGUI_VER_MAX));  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (ptLocation->iX + ptRelativeRegion->tSize.iWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iX + ptRelativeRegion->tSize.iWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (ptLocation->iY + ptRelativeRegion->tSize.iHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iY + ptRelativeRegion->tSize.iHeight) <= VSF_TGUI_VER_MAX);

    tVisualCharRegion.tSize.iHeight = vsf_tgui_font_get_char_height(chFontIndex) + chInterLineSpace;

    tLocation = *ptLocation;

    while (((wChar = vsf_tgui_text_get_next(ptString->pstrText, &tCharOffset)) != '\0')
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        && (tCharOffset <= ptString->s16_size)
#endif
        ) {
#if VSF_TGUI_SV_CFG_MULTI_LINE_TEXT == ENABLED
        if (wChar == '\n') {
            tVisualCharRegion.tLocation.iX = 0;
            tVisualCharRegion.tLocation.iY += tVisualCharRegion.tSize.iHeight;

            tLocation.iX = ptLocation->iX;
            if (tDirtyCharRegion.tSize.iHeight) {
                tLocation.iY += tDirtyCharRegion.tSize.iHeight;
            }
        }
        else if (wChar == '\r') {
            continue;
        }
        else
#endif
        {
            tVisualCharRegion.tSize.iWidth = vsf_tgui_font_get_char_width(chFontIndex, wChar);
            if (vsf_tgui_region_intersect(&tDirtyCharRegion, &tVisualCharRegion, ptRelativeRegion)) {
                tRelativeCharLocation.iX = tDirtyCharRegion.tLocation.iX - tVisualCharRegion.tLocation.iX,
                tRelativeCharLocation.iY = tDirtyCharRegion.tLocation.iY - tVisualCharRegion.tLocation.iY,

                vsf_tgui_sv_port_draw_char(&tLocation, &tRelativeCharLocation, &tDirtyCharRegion.tSize, chFontIndex, wChar, tColor);
                tLocation.iX += tDirtyCharRegion.tSize.iWidth;
            }
            tVisualCharRegion.tLocation.iX += tVisualCharRegion.tSize.iWidth;
        }
    }
}

void vsf_tgui_control_v_draw_text(  const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* ptDirtyRegion,
                                    vsf_tgui_text_info_t *ptStringInfo,
                                    const uint8_t chFontIndex,
                                    const vsf_tgui_sv_color_t tColor,
                                    const vsf_tgui_align_mode_t tMode)
{
    vsf_tgui_region_t tResourceRegion = {{0, 0}, {0,0}};
    vsf_tgui_location_t tRealLocation;
    vsf_tgui_sv_color_t tTmpColor = tColor;

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    vsf_tgui_string_t tString = {0};
    int16_t iOffset = 0;
#endif

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    VSF_TGUI_ASSERT(ptStringInfo != NULL);
    VSF_TGUI_ASSERT(ptStringInfo->tString.pstrText != NULL);

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    VSF_TGUI_ASSERT(ptStringInfo->tString.s16_size > 0);
#endif

    //Noto: Temporary code, Will be removed when style is supported
    if (!tTmpColor.bIsColorTransparency) {
        vsf_tgui_sv_color_set_trans_rate(&tTmpColor, 0xFF);
    }

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED

#   if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    if (ptStringInfo->bIsChanged) {
        ptStringInfo->bIsChanged = false;
        ptStringInfo->tInfoCache.tStringSize = vsf_tgui_text_get_size( chFontIndex,
                                                    &(ptStringInfo->tString),
                                                    &(ptStringInfo->tInfoCache.hwLines),
                                                    ptStringInfo->chInterLineSpace);
    }

    tResourceRegion.tSize = ptStringInfo->tInfoCache.tStringSize;
#   else
    tResourceRegion.tSize = vsf_tgui_text_get_size( chFontIndex,
                                                    &(ptStringInfo->tString),
                                                    NULL,
                                                    ptStringInfo->chInterLineSpace);
#   endif

    if (update_location_and_resouce_region_with_mode(   control_ptr,
                                                        ptDirtyRegion,
                                                        tMode,
                                                        &tRealLocation,
                                                        &tResourceRegion)) {

        //! get the tailored size
        vsf_tgui_region_t tAbsoluteDirtyRegion = {.tLocation = tRealLocation, .tSize = tResourceRegion.tSize};

        do {
            vsf_tgui_size_t tSize;
            vsf_tgui_location_t tAbsoluteLocation;
            if (NULL == vsf_tgui_text_get_line(&(ptStringInfo->tString), &iOffset, &tString)) {
                break;
            }

            tSize = vsf_tgui_text_get_size( chFontIndex, &tString, NULL, 0);
            tResourceRegion.tSize = tSize;

            if (update_location_and_resouce_region_with_mode(   control_ptr,
                                                                ptDirtyRegion,
                                                                tMode,
                                                                &tAbsoluteLocation,
                                                                &tResourceRegion)) {
        #if (VSF_TGUI_SV_CFG_DRAW_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
                VSF_TGUI_LOG(VSF_TRACE_INFO,
                    "[Simple View]%s draw text(%s) in "
                        "(x:%d, y:%d), size(w:%d, h:%d)"
                        VSF_TRACE_CFG_LINEEND,
                    control_ptr->use_as__vsf_msgt_node_t.node_name_ptr,
                    ptString->pstrText,
                    tRealLocation.iX,
                    tRealLocation.iY,
                    tResourceRegion.tSize.iWidth,
                    tResourceRegion.tSize.iHeight);
        #endif
                tResourceRegion.tSize = tSize;
                tRealLocation.iX = tAbsoluteLocation.iX;

                vsf_tgui_region_t temp_region = {.tLocation = tRealLocation, .tSize = tSize};

                if (vsf_tgui_region_intersect(&temp_region, &temp_region, &tAbsoluteDirtyRegion)) {
                    tResourceRegion.tSize = temp_region.tSize;

                    vsf_tgui_text_draw( &temp_region.tLocation,
                                        &tResourceRegion,
                                        &tString,
                                        chFontIndex,
                                        tColor,
                                        ptStringInfo->chInterLineSpace,
                                        control_ptr->use_as__vsf_tgui_v_control_t.chTransparencyRate);
                }

                tRealLocation.iY += tResourceRegion.tSize.iHeight + ptStringInfo->chInterLineSpace;
            } else {
                break;
            }

        } while(true);

    }

#else
#   if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    if (ptStringInfo->bIsChanged) {
        ptStringInfo->bIsChanged = false;
        ptStringInfo->tInfoCache.tStringSize = vsf_tgui_text_get_size( chFontIndex,
                                                    &(ptStringInfo->tString),
                                                    &(ptStringInfo->tInfoCache.hwLines),
                                                    ptStringInfo->chInterLineSpace);
    }

    tResourceRegion.tSize = ptStringInfo->tInfoCache.tStringSize;
#   else
    tResourceRegion.tSize = vsf_tgui_text_get_size( chFontIndex,
                                                    &(ptStringInfo->tString),
                                                    NULL,
                                                    ptStringInfo->chInterLineSpace);
#endif

    if (update_location_and_resouce_region_with_mode(   control_ptr,
                                                        ptDirtyRegion,
                                                        tMode,
                                                        &tRealLocation,
                                                        &tResourceRegion)) {
#if VSF_TGUI_SV_CFG_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%p draw text(%s) in "
                "(x:%d, y:%d), size(w:%d, h:%d)"
                VSF_TRACE_CFG_LINEEND,
            control_ptr,
            ptStringInfo->tString,
            tRealLocation.iX,
            tRealLocation.iY,
            tResourceRegion.tSize.iWidth,
            tResourceRegion.tSize.iHeight);
#endif
        vsf_tgui_sv_text_draw( &tRealLocation,
                               &tResourceRegion,
                               &(ptStringInfo->tString),
                               chFontIndex,
                               tTmpColor, ptStringInfo->chInterLineSpace);
    }
#endif
}

#endif


/* EOF */
