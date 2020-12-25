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
#ifndef VSF_TGUI_CFG_SV_DRAW_LOG
#   define VSF_TGUI_CFG_SV_DRAW_LOG              DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static const char* __sv_get_node_name(const vsf_tgui_control_t* control_ptr)
{
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING
    return (char *)control_ptr->use_as__vsf_msgt_node_t.node_name_ptr;
#else
    return "";
#endif
}

static void __sv_resource_region_update(vsf_tgui_region_t* ptDest, const vsf_tgui_region_t* ptBig, const vsf_tgui_region_t* ptSmall)
{
    VSF_TGUI_ASSERT(ptDest != NULL);
    VSF_TGUI_ASSERT(ptBig != NULL);
    VSF_TGUI_ASSERT(ptSmall != NULL);

    VSF_TGUI_ASSERT(ptBig->tSize.iWidth >= ptSmall->tSize.iWidth);
    VSF_TGUI_ASSERT(ptBig->tSize.iHeight >= ptSmall->tSize.iHeight);

    ptDest->tLocation.iX += ptSmall->tLocation.iX - ptBig->tLocation.iX;
    ptDest->tLocation.iY += ptSmall->tLocation.iY - ptBig->tLocation.iY;
    ptDest->tSize = ptSmall->tSize;
}

static bool __sv_control_get_no_empty_region(const vsf_tgui_control_t* control_ptr, vsf_tgui_region_t* ptRegion)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptRegion != NULL);

    vsf_tgui_size_t* ptSize = vsf_tgui_control_get_size(control_ptr);
    if (ptSize->iWidth <= 0 || ptSize->iHeight <= 0) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                     "[Simple View]%s's area is less than or equal to 0, (width: %d, height: %d)" VSF_TRACE_CFG_LINEEND,
                     __sv_get_node_name(control_ptr), ptRegion->tSize.iWidth, ptRegion->tSize.iHeight);
#endif
        return false;
    }

    // get control region
    ptRegion->tLocation.iX = 0;
    ptRegion->tLocation.iY = 0;
    ptRegion->tSize = *ptSize;

#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                 "[Simple View]%s control relative region(%d, %d, %d, %d)" VSF_TRACE_CFG_LINEEND,
                 __sv_get_node_name(control_ptr),
                 ptRegion->tLocation.iX,
                 ptRegion->tLocation.iY,
                 ptRegion->tSize.iWidth,
                 ptRegion->tSize.iHeight);
#endif
    return true;
}

static bool __sv_place_resource_with_absolute( const vsf_tgui_control_t* control_ptr,
                                               const vsf_tgui_region_t* ptDirtyRegion,
                                               const vsf_tgui_region_t* ptContrlRelativeRegion,
                                               vsf_tgui_location_t* ptRealLocation,
                                               vsf_tgui_region_t* ptResourceRegion)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    VSF_TGUI_ASSERT(ptContrlRelativeRegion != NULL);
    VSF_TGUI_ASSERT(ptRealLocation != NULL);
    VSF_TGUI_ASSERT(ptResourceRegion != NULL);


    vsf_tgui_region_t tAbsoluteRegion;
    vsf_tgui_region_t tAbsoluteRegionInScreen;
    const vsf_tgui_region_t tScreenResion = {
        .tLocation = {.iX = 0, .iY = 0 },
        .tSize = {.iWidth = VSF_TGUI_HOR_MAX, .iHeight = VSF_TGUI_VER_MAX},
    };

#if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
    vsf_tgui_region_t tControlRelativeDirtyRegion;
    // only update dirty region
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    if (!vsf_tgui_region_intersect(&tControlRelativeDirtyRegion, ptContrlRelativeRegion, ptDirtyRegion)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_DEBUG, "[Simple View]%s in dirty resion is empty" VSF_TRACE_CFG_LINEEND,
                     __sv_get_node_name(control_ptr));
#endif
        return false;
    }
    __sv_resource_region_update(ptResourceRegion, ptContrlRelativeRegion, &tControlRelativeDirtyRegion);

#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                 "[Simple View]%s relative region (%d, %d, %d, %d) after in dirty region(%d, %d, %d, %d), resouce region(%d, %d, %d, %d)" VSF_TRACE_CFG_LINEEND,
                 __sv_get_node_name(control_ptr),
                 tControlRelativeDirtyRegion.tLocation.iX,
                 tControlRelativeDirtyRegion.tLocation.iY,
                 tControlRelativeDirtyRegion.tSize.iWidth,
                 tControlRelativeDirtyRegion.tSize.iHeight,
                 ptDirtyRegion->tLocation.iX,
                 ptDirtyRegion->tLocation.iY,
                 ptDirtyRegion->tSize.iWidth,
                 ptDirtyRegion->tSize.iHeight,
                 ptResourceRegion->tLocation.iX,
                 ptResourceRegion->tLocation.iY,
                 ptResourceRegion->tSize.iWidth,
                 ptResourceRegion->tSize.iHeight);
#endif
#endif


    // use absolute coordinates
    tAbsoluteRegion = tControlRelativeDirtyRegion;
    vsf_tgui_control_calculate_absolute_location(control_ptr, &tAbsoluteRegion.tLocation);

#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                 "[Simple View]%s absolute region(%d, %d, %d, %d)" VSF_TRACE_CFG_LINEEND,
                 __sv_get_node_name(control_ptr),
                 tAbsoluteRegion.tLocation.iX,
                 tAbsoluteRegion.tLocation.iY,
                 tAbsoluteRegion.tSize.iWidth,
                 tAbsoluteRegion.tSize.iHeight);
#endif


    // only focus on the region on the screen
    if (!vsf_tgui_region_intersect(&tAbsoluteRegionInScreen, &tAbsoluteRegion, &tScreenResion)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_DEBUG, "[Simple View]%s absolute region in screen is empty" VSF_TRACE_CFG_LINEEND,
                     __sv_get_node_name(control_ptr));
#endif
        return false;
    }
    __sv_resource_region_update(ptResourceRegion, &tAbsoluteRegion, &tAbsoluteRegionInScreen);


    // save result
    *ptRealLocation = tAbsoluteRegionInScreen.tLocation;

#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                 "[Simple View]%s absolute region(%d, %d, %d, %d) in screen(%d, %d, %d, %d), resouce region(%d, %d, %d, %d)" VSF_TRACE_CFG_LINEEND,
                 __sv_get_node_name(control_ptr),
                 tAbsoluteRegionInScreen.tLocation.iX,
                 tAbsoluteRegionInScreen.tLocation.iY,
                 tAbsoluteRegionInScreen.tSize.iWidth,
                 tAbsoluteRegionInScreen.tSize.iHeight,

                 tScreenResion.tLocation.iX,
                 tScreenResion.tLocation.iY,
                 tScreenResion.tSize.iWidth,
                 tScreenResion.tSize.iHeight,

                 ptResourceRegion->tLocation.iX,
                 ptResourceRegion->tLocation.iY,
                 ptResourceRegion->tSize.iWidth,
                 ptResourceRegion->tSize.iHeight);
#endif

    return true;
}

static bool __sv_update_place_resouce_with_align(const vsf_tgui_control_t* control_ptr,
                                                 const vsf_tgui_region_t* ptDirtyRegion,
                                                 const vsf_tgui_align_mode_t tMode,
                                                 vsf_tgui_location_t* ptRealLocation,
                                                 vsf_tgui_region_t* ptResourceRegion)
{
    vsf_tgui_region_t tContrlRelativeRegion;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptRealLocation != NULL);
    VSF_TGUI_ASSERT(ptResourceRegion != NULL);

    // get control region, Maybe it's bigger than the screen
    if (!__sv_control_get_no_empty_region(control_ptr, &tContrlRelativeRegion)) {
        return false;
    }

    // place resouce region in control
    VSF_TGUI_ASSERT(ptResourceRegion->tSize.iWidth > 0 && ptResourceRegion->tSize.iHeight > 0);
    vsf_tgui_region_update_with_align(&tContrlRelativeRegion, ptResourceRegion, tMode);

    return __sv_place_resource_with_absolute(control_ptr, ptDirtyRegion, &tContrlRelativeRegion, ptRealLocation, ptResourceRegion);
}

uint_fast8_t vsf_tgui_control_v_get_tile_trans_rate(const vsf_tgui_control_t* control_ptr)
{
    bool bIsTileTransparency;
    uint_fast8_t chTileTransparencyRate;

    VSF_TGUI_ASSERT(control_ptr != NULL);

	if (vsf_tgui_control_is_container(control_ptr)) {
		const vsf_msgt_node_t* node_ptr = &control_ptr->use_as__vsf_msgt_node_t;
		const vsf_tgui_container_t* container_ptr = (const vsf_tgui_container_t*)node_ptr;

        bIsTileTransparency = container_ptr->bIsTileTransparency;
        chTileTransparencyRate = container_ptr->chTileTransparencyRate;
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

        container_ptr->bIsTileTransparency = bIsTileTransparency;
        container_ptr->chTileTransparencyRate = chTileTransparencyRate;
    } else {
        control_ptr->bIsTileTransparency = bIsTileTransparency;
        control_ptr->chTileTransparencyRate = chTileTransparencyRate;
    }
}


void vsf_tgui_control_v_draw_rect(  const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* ptDirtyRegion,
                                    const vsf_tgui_region_t* ptRectRegion,
                                    const vsf_tgui_sv_color_t tColor)
{
    vsf_tgui_region_t tResourceRegion;
    vsf_tgui_location_t tRealLocation;
    vsf_tgui_sv_color_t tTmpColor = tColor;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    VSF_TGUI_ASSERT(ptRectRegion != NULL);


    if (ptRectRegion->tSize.iHeight <= 0 || ptRectRegion->tSize.iWidth <= 0) {
        return ;
    }

    //Noto: Temporary code, Will be removed when style is supported
    if (!tTmpColor.bIsColorTransparency) {
        vsf_tgui_sv_color_set_trans_rate(&tTmpColor, 0xFF);
    }

    tResourceRegion = *ptRectRegion;

    if (__sv_place_resource_with_absolute(control_ptr, ptDirtyRegion, ptRectRegion,
                                          &tRealLocation, &tResourceRegion)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%s draw rect(0x%x) in (x:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
            __sv_get_node_name(control_ptr), tColor.tColor.Value,
            tRealLocation.iX, tRealLocation.iY, tResourceRegion.tSize.iWidth, tResourceRegion.tSize.iHeight);
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
    vsf_tgui_location_t tRealLocation;
    uint8_t chTileTransparencyRate;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    VSF_TGUI_ASSERT(ptTile != NULL);


    ptTile = vsf_tgui_tile_get_root(ptTile, &tResourceRegion);
    VSF_TGUI_ASSERT(ptTile != NULL);

    if (__sv_update_place_resouce_with_align(control_ptr, ptDirtyRegion, tMode, &tRealLocation, &tResourceRegion)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%s draw tile(0x%p) in (x:%d, y:%d), resource(x:%d, y:%d w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
            __sv_get_node_name(control_ptr), ptTile, tRealLocation.iX, tRealLocation.iY,
                     tResourceRegion.tLocation.iX, tResourceRegion.tLocation.iY,
                     tResourceRegion.tSize.iWidth, tResourceRegion.tSize.iHeight);
#endif

        chTileTransparencyRate = vsf_tgui_control_v_get_tile_trans_rate(control_ptr);
        vsf_tgui_sv_port_draw_root_tile(&tRealLocation,
                                        &tResourceRegion.tLocation,
                                        &tResourceRegion.tSize,
                                        ptTile,
                                        chTileTransparencyRate);
    }
}

static bool __sv_text_get_size(const uint8_t chFontIndex, vsf_tgui_text_info_t* ptStringInfo, vsf_tgui_size_t* ptSize)
{
    VSF_TGUI_ASSERT(ptStringInfo != NULL);
    VSF_TGUI_ASSERT(ptStringInfo->tString.pstrText != NULL);
    VSF_TGUI_ASSERT(ptSize != NULL);

#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    if (ptStringInfo->bIsChanged) {
        ptStringInfo->bIsChanged = false;
        ptStringInfo->tInfoCache.tStringSize = vsf_tgui_text_get_size(chFontIndex,
                                                                      &(ptStringInfo->tString),
                                                                      &(ptStringInfo->tInfoCache.hwLines),
                                                                      &(ptStringInfo->tInfoCache.chCharHeight),
                                                                      ptStringInfo->chInterLineSpace);
    }
    *ptSize = ptStringInfo->tInfoCache.tStringSize;
#else
    * ptSize = vsf_tgui_text_get_size(  chFontIndex, 
                                        &(ptStringInfo->tString), 
                                        NULL, 
                                        NULL, 
                                        ptStringInfo->chInterLineSpace);
#endif

    if (ptSize->iWidth <= 0 || ptSize->iHeight <= 0) {
        return false;
    } else {
        return true;
    }
}

#if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT == ENABLED
static int16_t __sv_text_draw_get_x(const vsf_tgui_string_t* ptString, const uint8_t chFontIndex, size_t tCharOffset, const vsf_tgui_align_mode_t tMode, int16_t iMaxWidth)
{
#if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT_ALIGN != ENABLED
    return 0;
#else
    uint32_t wChar;
    int16_t iWidth = 0;

    if (tMode & VSF_TGUI_ALIGN_LEFT) {
        return 0;
    }

    while (((wChar = vsf_tgui_text_get_next(ptString->pstrText, &tCharOffset)) != '\0')
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
           && (tCharOffset <= ptString->s16_size)
#endif
           ) {
        if (wChar == '\n') {
            break;
        }

        if (wChar == '\r') {
            continue;
        }

        iWidth += vsf_tgui_font_get_char_width(chFontIndex, wChar);
    }

    if (tMode & VSF_TGUI_ALIGN_RIGHT){
        return iMaxWidth - iWidth;
    } else {
        return (iMaxWidth - iWidth) / 2;
    }
#endif
}
#endif

static void __sv_text_draw(const vsf_tgui_location_t* ptLocation,
                           const vsf_tgui_size_t *ptOriginResourceSize,
                           const vsf_tgui_region_t* ptDirtyRegion,
                           const vsf_tgui_string_t* ptString,
                           const uint8_t chFontIndex,
                           const vsf_tgui_sv_color_t tColor,
                           const int_fast8_t chInterLineSpace,
                           const vsf_tgui_align_mode_t tMode)
{
    vsf_tgui_region_t tCharDrawRegion = { 0 };
    vsf_tgui_location_t tLocation;
    vsf_tgui_region_t tDirtyCharRegion = { 0 };
    vsf_tgui_location_t tRelativeCharLocation = { 0 };
    vsf_tgui_location_t tBaseLocation = { 0 };
    uint32_t wChar;
    size_t tCharOffset = 0;

    VSF_TGUI_ASSERT(ptLocation != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    VSF_TGUI_ASSERT(ptString != NULL);
    VSF_TGUI_ASSERT(ptString->pstrText != NULL);

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    VSF_TGUI_ASSERT(ptString->s16_size > 0);
#endif

    VSF_TGUI_ASSERT((0 <= ptLocation->iX) && (ptLocation->iX < VSF_TGUI_HOR_MAX));                  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= ptLocation->iY) && (ptLocation->iY < VSF_TGUI_VER_MAX));                  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (ptLocation->iX + ptDirtyRegion->tSize.iWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iX + ptDirtyRegion->tSize.iWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (ptLocation->iY + ptDirtyRegion->tSize.iHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((ptLocation->iY + ptDirtyRegion->tSize.iHeight) <= VSF_TGUI_VER_MAX);

    tCharDrawRegion.tSize.iHeight = vsf_tgui_font_get_char_height(chFontIndex);
#if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT == ENABLED
    tCharDrawRegion.tLocation.iX = __sv_text_draw_get_x(ptString, chFontIndex, tCharOffset, tMode, ptOriginResourceSize->iWidth);
#endif

    tBaseLocation.iX = ptLocation->iX - ptDirtyRegion->tLocation.iX;
    tBaseLocation.iY = ptLocation->iY - ptDirtyRegion->tLocation.iY;

    while (((wChar = vsf_tgui_text_get_next(ptString->pstrText, &tCharOffset)) != '\0')
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        && (tCharOffset <= ptString->s16_size)
#endif
        ) {
        if (wChar == '\n') {
#if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT == ENABLED
            tCharDrawRegion.tLocation.iX = __sv_text_draw_get_x(ptString, chFontIndex, tCharOffset, tMode, ptOriginResourceSize->iWidth);
            tCharDrawRegion.tLocation.iY += tCharDrawRegion.tSize.iHeight + chInterLineSpace;
#endif
            continue;
        }

        if (wChar == '\r') {
            continue;
        }

        tCharDrawRegion.tSize.iWidth = vsf_tgui_font_get_char_width(chFontIndex, wChar);
        if (vsf_tgui_region_intersect(&tDirtyCharRegion, &tCharDrawRegion, ptDirtyRegion)) {
            tRelativeCharLocation.iX = tDirtyCharRegion.tLocation.iX - tCharDrawRegion.tLocation.iX;
            tRelativeCharLocation.iY = tDirtyCharRegion.tLocation.iY - tCharDrawRegion.tLocation.iY;
            tLocation.iX = tBaseLocation.iX + tDirtyCharRegion.tLocation.iX;
            tLocation.iY = tBaseLocation.iY + tDirtyCharRegion.tLocation.iY;

            vsf_tgui_sv_port_draw_char(&tLocation, &tRelativeCharLocation, &tDirtyCharRegion.tSize, chFontIndex, wChar, tColor);

            tCharDrawRegion.tLocation.iX += tCharDrawRegion.tSize.iWidth;
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
    vsf_tgui_size_t tSize;
    vsf_tgui_location_t tRealLocation;
    vsf_tgui_sv_color_t tTmpColor = tColor;

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

    if (!__sv_text_get_size(chFontIndex, ptStringInfo, &tSize)) {
        return ;
    }
    tResourceRegion.tSize = tSize;

    if (__sv_update_place_resouce_with_align(control_ptr, ptDirtyRegion, tMode, &tRealLocation, &tResourceRegion)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%s draw text(%s) in (:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
            __sv_get_node_name(control_ptr), ptStringInfo->tString, tRealLocation.iX, tRealLocation.iY,
            tResourceRegion.tSize.iWidth, tResourceRegion.tSize.iHeight);
#endif
        __sv_text_draw(&tRealLocation, &tSize, &tResourceRegion, &(ptStringInfo->tString),
                       chFontIndex, tTmpColor, ptStringInfo->chInterLineSpace, tMode);
    }
}

#endif


/* EOF */
