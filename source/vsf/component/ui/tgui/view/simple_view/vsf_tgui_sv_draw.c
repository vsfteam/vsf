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

#define __VSF_TGUI_CONTROLS_CONTROLE_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "vsf_tgui_sv_port.h"
#include "../../controls/vsf_tgui_controls.h"
#include "../../utilities/vsf_tgui_color.h"
#include "../../utilities/vsf_tgui_text.h"
#include "../../utilities/vsf_tgui_font.h"
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
                                        const vsf_tgui_control_t* ptControl,
                                        const vsf_tgui_region_t* ptDirtyRegion,
                                        const vsf_tgui_region_t* ptDrawInCtrlRegion,
                                        vsf_tgui_location_t* ptRealLocation,
                                        vsf_tgui_region_t* ptResourceRegion)
{
    vsf_tgui_region_t tRealRawRegion;

    VSF_TGUI_ASSERT(ptControl != NULL);
    VSF_TGUI_ASSERT(ptDrawInCtrlRegion != NULL);
    VSF_TGUI_ASSERT(ptRealLocation != NULL);
    VSF_TGUI_ASSERT(ptResourceRegion != NULL);

#if VSF_TGUI_V_CFG_PARTIAL_REFRESH == ENABLED
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

    vsf_tgui_control_get_absolute_location(ptControl, ptRealLocation);

    return true;
}

static bool update_location_and_resouce_region_with_mode(
                                                const vsf_tgui_control_t* ptControl,
                                                const vsf_tgui_region_t* ptDirtyRegion,
                                                const vsf_tgui_align_mode_t tMode,
                                                vsf_tgui_location_t* ptRealLocationBuffer,
                                                vsf_tgui_region_t* ptResourceRegion)
{
    vsf_tgui_region_t tDrawInCtrlRegion = {0};

    //tDrawInCtrlRegion.tLocation = *ptRealLocation;
    tDrawInCtrlRegion.tSize = vsf_tgui_control_get_size(ptControl);
    //tDrawInCtrlRegion.tSize.iHeight -= tDrawInCtrlRegion.tLocation.iY;
    //tDrawInCtrlRegion.tSize.iWidth -= tDrawInCtrlRegion.tLocation.iX;
    // resize draw region and resource region
    vsf_tgui_region_update_with_align(&tDrawInCtrlRegion, ptResourceRegion, tMode);

    return update_location_and_resouce_region(ptControl, ptDirtyRegion, &tDrawInCtrlRegion, ptRealLocationBuffer, ptResourceRegion);
}

void vsf_tgui_control_v_draw_rect(  const vsf_tgui_control_t* ptControl,
                                    const vsf_tgui_region_t* ptDirtyRegion,
                                    const vsf_tgui_region_t* ptRectRegion,
                                    const vsf_tgui_color_t tColor)
{
    vsf_tgui_region_t tResourceRegion;
    vsf_tgui_location_t tRealLocation = {0,0};

    VSF_TGUI_ASSERT(ptControl != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);


    if (update_location_and_resouce_region(ptControl, ptDirtyRegion, ptRectRegion, &tRealLocation, &tResourceRegion)) {
#if VSF_TGUI_SV_CFG_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%p draw rect(0x%x) in (x:0x%d, y:0x%d), size(w:0x%d, h:0x%d)" VSF_TRACE_CFG_LINEEND,
            ptControl, tColor.wValue, tRealLocation.iX, tRealLocation.iY, tResourceRegion.tSize.iWidth, tResourceRegion.tSize.iHeight);
#endif
        vsf_tgui_draw_rect(&tRealLocation, &tResourceRegion.tSize, tColor);
    }
}


void vsf_tgui_control_v_draw_tile(  const vsf_tgui_control_t* ptControl,
                                    const vsf_tgui_region_t* ptDirtyRegion,
                                    const vsf_tgui_tile_t* ptTile,
                                    const vsf_tgui_align_mode_t tMode)
{
    vsf_tgui_region_t tResourceRegion;
    vsf_tgui_location_t tRealLocation = {0, 0};

    VSF_TGUI_ASSERT(ptControl != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    VSF_TGUI_ASSERT(ptTile != NULL);

    ptTile = vsf_tgui_tile_get_root(ptTile, &tResourceRegion);
    VSF_TGUI_ASSERT(ptTile != NULL);

    if (update_location_and_resouce_region_with_mode(ptControl, ptDirtyRegion, tMode, &tRealLocation, &tResourceRegion)) {
#if VSF_TGUI_SV_CFG_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%p draw tile(0x%p) in (x:0x%d, y:0x%d), size(w:0x%d, h:0x%d)" VSF_TRACE_CFG_LINEEND,
            ptControl, ptTile, tRealLocation.iX, tRealLocation.iY, tResourceRegion.tSize.iWidth, tResourceRegion.tSize.iHeight); 
#endif
        vsf_tgui_draw_root_tile(&tRealLocation, &tResourceRegion.tLocation, &tResourceRegion.tSize, ptTile);
    }
}

void vsf_tgui_control_v_draw_text(  const vsf_tgui_control_t* ptControl,
                                    const vsf_tgui_region_t* ptDirtyRegion,
                                    vsf_tgui_text_info_t *ptStringInfo,
                                    const uint8_t chFontIndex,
                                    const vsf_tgui_color_t tColor,
                                    const vsf_tgui_align_mode_t tMode)
{
    vsf_tgui_region_t tResourceRegion = {{0, 0}, {0,0}};
    vsf_tgui_location_t tRealLocation;

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    vsf_tgui_string_t tString = {0};
    int16_t iOffset = 0;
#endif
    
    VSF_TGUI_ASSERT(ptControl != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);
    VSF_TGUI_ASSERT(ptStringInfo != NULL);
    VSF_TGUI_ASSERT(ptStringInfo->tString.pstrText != NULL);

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    VSF_TGUI_ASSERT(ptStringInfo->tString.iSize > 0);
#endif

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

    if (update_location_and_resouce_region_with_mode(   ptControl, 
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

            if (update_location_and_resouce_region_with_mode(   ptControl, 
                                                                ptDirtyRegion, 
                                                                tMode, 
                                                                &tAbsoluteLocation, 
                                                                &tResourceRegion)) {
        #if VSF_TGUI_SV_CFG_DRAW_LOG == ENABLED
                VSF_TGUI_LOG(VSF_TRACE_INFO,
                    "[Simple View]%p draw text(%s) in "
                        "(x:0x%d, y:0x%d), size(w:0x%d, h:0x%d)" 
                        VSF_TRACE_CFG_LINEEND,
                    ptControl, 
                    ptString->pstrText, 
                    tRealLocation.iX, 
                    tRealLocation.iY, 
                    tResourceRegion.tSize.iWidth, 
                    tResourceRegion.tSize.iHeight);
        #endif
                tResourceRegion.tSize = tSize;
                tRealLocation.iX = tAbsoluteLocation.iX;

                vsf_tgui_region_t tTempRegion = {.tLocation = tRealLocation, .tSize = tSize};

                if (vsf_tgui_region_intersect(&tTempRegion, &tTempRegion, &tAbsoluteDirtyRegion)) {
                    tResourceRegion.tSize = tTempRegion.tSize;

                    vsf_tgui_text_draw( &tTempRegion.tLocation, 
                                        &tResourceRegion, 
                                        &tString, 
                                        chFontIndex, 
                                        tColor, 0);
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

    if (update_location_and_resouce_region_with_mode(   ptControl, 
                                                        ptDirtyRegion, 
                                                        tMode, 
                                                        &tRealLocation, 
                                                        &tResourceRegion)) {
#if VSF_TGUI_SV_CFG_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%p draw text(%s) in "
                "(x:0x%d, y:0x%d), size(w:0x%d, h:0x%d)" 
                VSF_TRACE_CFG_LINEEND,
            ptControl, 
            ptString->pstrText, 
            tRealLocation.iX, 
            tRealLocation.iY, 
            tResourceRegion.tSize.iWidth, 
            tResourceRegion.tSize.iHeight);
#endif
        vsf_tgui_text_draw( &tRealLocation, 
                            &tResourceRegion, 
                            &(ptStringInfo->tString), 
                            chFontIndex, 
                            tColor, ptStringInfo->chInterLineSpace);
    }
#endif
}

#endif


/* EOF */
