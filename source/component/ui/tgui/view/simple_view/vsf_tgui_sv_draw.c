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

static void __sv_resource_region_update(vsf_tgui_region_t* dst_ptr, const vsf_tgui_region_t* big_ptr, const vsf_tgui_region_t* small_ptr)
{
    VSF_TGUI_ASSERT(dst_ptr != NULL);
    VSF_TGUI_ASSERT(big_ptr != NULL);
    VSF_TGUI_ASSERT(small_ptr != NULL);

    VSF_TGUI_ASSERT(big_ptr->tSize.iWidth >= small_ptr->tSize.iWidth);
    VSF_TGUI_ASSERT(big_ptr->tSize.iHeight >= small_ptr->tSize.iHeight);

    dst_ptr->tLocation.iX += small_ptr->tLocation.iX - big_ptr->tLocation.iX;
    dst_ptr->tLocation.iY += small_ptr->tLocation.iY - big_ptr->tLocation.iY;
    dst_ptr->tSize = small_ptr->tSize;
}

static bool __sv_control_get_no_empty_region(const vsf_tgui_control_t* control_ptr, vsf_tgui_region_t* region_ptr)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(region_ptr != NULL);

    vsf_tgui_size_t* size_ptr = vsf_tgui_control_get_size(control_ptr);
    if (size_ptr->iWidth <= 0 || size_ptr->iHeight <= 0) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                     "[Simple View]%s's area is less than or equal to 0, (width: %d, height: %d)" VSF_TRACE_CFG_LINEEND,
                     __sv_get_node_name(control_ptr), region_ptr->tSize.iWidth, region_ptr->tSize.iHeight);
#endif
        return false;
    }

    // get control region
    region_ptr->tLocation.iX = 0;
    region_ptr->tLocation.iY = 0;
    region_ptr->tSize = *size_ptr;

#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                 "[Simple View]%s control relative region(%d, %d, %d, %d)" VSF_TRACE_CFG_LINEEND,
                 __sv_get_node_name(control_ptr),
                 region_ptr->tLocation.iX,
                 region_ptr->tLocation.iY,
                 region_ptr->tSize.iWidth,
                 region_ptr->tSize.iHeight);
#endif
    return true;
}

static bool __sv_place_resource_with_absolute( const vsf_tgui_control_t* control_ptr,
                                               const vsf_tgui_region_t* dirty_region_ptr,
                                               const vsf_tgui_region_t* contrl_relative_region_ptr,
                                               vsf_tgui_location_t* real_location_ptr,
                                               vsf_tgui_region_t* resource_region_ptr)
{
    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);
    VSF_TGUI_ASSERT(contrl_relative_region_ptr != NULL);
    VSF_TGUI_ASSERT(real_location_ptr != NULL);
    VSF_TGUI_ASSERT(resource_region_ptr != NULL);


    vsf_tgui_region_t absolute_region;
    vsf_tgui_region_t absolute_region_in_screen;
    const vsf_tgui_region_t screen_region = {
        .tLocation = {.iX = 0, .iY = 0 },
        .tSize = {.iWidth = VSF_TGUI_HOR_MAX, .iHeight = VSF_TGUI_VER_MAX},
    };

#if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
    vsf_tgui_region_t control_relative_dirty_region;
    // only update dirty region
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);
    if (!vsf_tgui_region_intersect(&control_relative_dirty_region, contrl_relative_region_ptr, dirty_region_ptr)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_DEBUG, "[Simple View]%s in dirty resion is empty" VSF_TRACE_CFG_LINEEND,
                     __sv_get_node_name(control_ptr));
#endif
        return false;
    }
    __sv_resource_region_update(resource_region_ptr, contrl_relative_region_ptr, &control_relative_dirty_region);

#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                 "[Simple View]%s relative region (%d, %d, %d, %d) after in dirty region(%d, %d, %d, %d), resouce region(%d, %d, %d, %d)" VSF_TRACE_CFG_LINEEND,
                 __sv_get_node_name(control_ptr),
                 control_relative_dirty_region.tLocation.iX,
                 control_relative_dirty_region.tLocation.iY,
                 control_relative_dirty_region.tSize.iWidth,
                 control_relative_dirty_region.tSize.iHeight,
                 dirty_region_ptr->tLocation.iX,
                 dirty_region_ptr->tLocation.iY,
                 dirty_region_ptr->tSize.iWidth,
                 dirty_region_ptr->tSize.iHeight,
                 resource_region_ptr->tLocation.iX,
                 resource_region_ptr->tLocation.iY,
                 resource_region_ptr->tSize.iWidth,
                 resource_region_ptr->tSize.iHeight);
#endif
#endif


    // use absolute coordinates
    absolute_region = control_relative_dirty_region;
    vsf_tgui_control_calculate_absolute_location(control_ptr, &absolute_region.tLocation);

#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                 "[Simple View]%s absolute region(%d, %d, %d, %d)" VSF_TRACE_CFG_LINEEND,
                 __sv_get_node_name(control_ptr),
                 absolute_region.tLocation.iX,
                 absolute_region.tLocation.iY,
                 absolute_region.tSize.iWidth,
                 absolute_region.tSize.iHeight);
#endif


    // only focus on the region on the screen
    if (!vsf_tgui_region_intersect(&absolute_region_in_screen, &absolute_region, &screen_region)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_DEBUG, "[Simple View]%s absolute region in screen is empty" VSF_TRACE_CFG_LINEEND,
                     __sv_get_node_name(control_ptr));
#endif
        return false;
    }
    __sv_resource_region_update(resource_region_ptr, &absolute_region, &absolute_region_in_screen);


    // save result
    *real_location_ptr = absolute_region_in_screen.tLocation;

#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
    VSF_TGUI_LOG(VSF_TRACE_DEBUG,
                 "[Simple View]%s absolute region(%d, %d, %d, %d) in screen(%d, %d, %d, %d), resouce region(%d, %d, %d, %d)" VSF_TRACE_CFG_LINEEND,
                 __sv_get_node_name(control_ptr),
                 absolute_region_in_screen.tLocation.iX,
                 absolute_region_in_screen.tLocation.iY,
                 absolute_region_in_screen.tSize.iWidth,
                 absolute_region_in_screen.tSize.iHeight,

                 screen_region.tLocation.iX,
                 screen_region.tLocation.iY,
                 screen_region.tSize.iWidth,
                 screen_region.tSize.iHeight,

                 resource_region_ptr->tLocation.iX,
                 resource_region_ptr->tLocation.iY,
                 resource_region_ptr->tSize.iWidth,
                 resource_region_ptr->tSize.iHeight);
#endif

    return true;
}

static bool __sv_update_place_resouce_with_align(const vsf_tgui_control_t* control_ptr,
                                                 const vsf_tgui_region_t* dirty_region_ptr,
                                                 const vsf_tgui_align_mode_t mode,
                                                 vsf_tgui_location_t* real_location_ptr,
                                                 vsf_tgui_region_t* resource_region_ptr)
{
    vsf_tgui_region_t contrl_relative_region;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(real_location_ptr != NULL);
    VSF_TGUI_ASSERT(resource_region_ptr != NULL);

    // get control region, Maybe it's bigger than the screen
    if (!__sv_control_get_no_empty_region(control_ptr, &contrl_relative_region)) {
        return false;
    }

    // place resouce region in control
    VSF_TGUI_ASSERT(resource_region_ptr->tSize.iWidth > 0 && resource_region_ptr->tSize.iHeight > 0);
    vsf_tgui_region_update_with_align(&contrl_relative_region, resource_region_ptr, mode);

    return __sv_place_resource_with_absolute(control_ptr, dirty_region_ptr, &contrl_relative_region, real_location_ptr, resource_region_ptr);
}

void vsf_tgui_control_v_draw_rect(  const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* dirty_region_ptr,
                                    const vsf_tgui_region_t* rect_region_ptr,
                                    const vsf_tgui_sv_color_t color)
{
    vsf_tgui_region_t resource_region;
    vsf_tgui_location_t real_location;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);
    VSF_TGUI_ASSERT(rect_region_ptr != NULL);


    if (rect_region_ptr->tSize.iHeight <= 0 || rect_region_ptr->tSize.iWidth <= 0) {
        return ;
    }

    resource_region = *rect_region_ptr;

    if (__sv_place_resource_with_absolute(control_ptr, dirty_region_ptr, rect_region_ptr,
                                          &real_location, &resource_region)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%s draw rect(0x%x) in (x:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
            __sv_get_node_name(control_ptr), color.value,
            real_location.iX, real_location.iY, resource_region.tSize.iWidth, resource_region.tSize.iHeight);
#endif

        vsf_tgui_sv_port_draw_rect(&real_location, &resource_region.tSize, color);
    }

}


void vsf_tgui_control_v_draw_tile(  const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* dirty_region_ptr,
                                    const vsf_tgui_tile_t* tile_ptr,
                                    const vsf_tgui_align_mode_t mode,
                                    const uint8_t trans_rate,
                                    vsf_tgui_region_t * placed_region_ptr)
{
    vsf_tgui_region_t resource_region;
    vsf_tgui_location_t real_location;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);
    VSF_TGUI_ASSERT(tile_ptr != NULL);

    tile_ptr = vsf_tgui_tile_get_root(tile_ptr, &resource_region);
    VSF_TGUI_ASSERT(tile_ptr != NULL);

    if (placed_region_ptr != NULL) {
        *placed_region_ptr = resource_region;
    }

    if (__sv_update_place_resouce_with_align(control_ptr, dirty_region_ptr, mode, &real_location, &resource_region)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%s draw tile(0x%p) in (x:%d, y:%d), resource(x:%d, y:%d w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
            __sv_get_node_name(control_ptr), tile_ptr, real_location.iX, real_location.iY,
                     resource_region.tLocation.iX, resource_region.tLocation.iY,
                     resource_region.tSize.iWidth, resource_region.tSize.iHeight);
#endif

        vsf_tgui_sv_port_draw_root_tile(&real_location,
                                        &resource_region.tLocation,
                                        &resource_region.tSize,
                                        tile_ptr,
                                        trans_rate);
    } else {
    }
}

static bool __sv_text_get_size(const uint8_t font_index, vsf_tgui_text_info_t* string_info_ptr, vsf_tgui_size_t* size_ptr)
{
    VSF_TGUI_ASSERT(string_info_ptr != NULL);
    VSF_TGUI_ASSERT(string_info_ptr->tString.pstrText != NULL);
    VSF_TGUI_ASSERT(size_ptr != NULL);

#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    if (string_info_ptr->bIsChanged) {
        string_info_ptr->bIsChanged = false;
        string_info_ptr->tInfoCache.tStringSize = vsf_tgui_text_get_size(font_index,
                                                                      &(string_info_ptr->tString),
                                                                      &(string_info_ptr->tInfoCache.hwLines),
                                                                      &(string_info_ptr->tInfoCache.chCharHeight),
                                                                      string_info_ptr->chInterLineSpace);
    }
    *size_ptr = string_info_ptr->tInfoCache.tStringSize;
#else
    * size_ptr = vsf_tgui_text_get_size(  font_index,
                                        &(string_info_ptr->tString),
                                        NULL,
                                        NULL,
                                        string_info_ptr->chInterLineSpace);
#endif

    if (size_ptr->iWidth <= 0 || size_ptr->iHeight <= 0) {
        return false;
    } else {
        return true;
    }
}

#if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT == ENABLED
static int16_t __sv_text_draw_get_x(const vsf_tgui_string_t* string_ptr, const uint8_t font_index, size_t char_offset, const vsf_tgui_align_mode_t mode, int16_t max_width)
{
#if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT_ALIGN != ENABLED
    return 0;
#else
    uint32_t char_u32;
    int16_t width = 0;

    if (mode & VSF_TGUI_ALIGN_LEFT) {
        return 0;
    }

    while (((char_u32 = vsf_tgui_text_get_next(string_ptr->pstrText, &char_offset)) != '\0')
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
           && (char_offset <= string_ptr->s16_size)
#endif
           ) {
        if (char_u32 == '\n') {
            break;
        }

        if (char_u32 == '\r') {
            continue;
        }

        width += vsf_tgui_font_get_char_width(font_index, char_u32);
    }

    if (mode & VSF_TGUI_ALIGN_RIGHT){
        return max_width - width ;
    } else {
        return (max_width - width) / 2;
    }
#endif
}
#endif

static void __sv_text_draw(const vsf_tgui_location_t* location_ptr,
                           const vsf_tgui_size_t *origin_resource_size,
                           const vsf_tgui_region_t* dirty_region_ptr,
                           const vsf_tgui_string_t* string_ptr,
                           const uint8_t font_index,
                           const vsf_tgui_sv_color_t color,
                           const int_fast8_t inter_line_space,
                           const vsf_tgui_align_mode_t mode)
{
    vsf_tgui_region_t char_draw_region = { 0 };
    vsf_tgui_location_t location;
    vsf_tgui_region_t dirty_char_region = { 0 };
    vsf_tgui_location_t relative_char_location = { 0 };
    vsf_tgui_location_t base_location = { 0 };
    uint32_t char_u32;
    size_t char_offset = 0;

    VSF_TGUI_ASSERT(location_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);
    VSF_TGUI_ASSERT(string_ptr != NULL);
    VSF_TGUI_ASSERT(string_ptr->pstrText != NULL);

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    VSF_TGUI_ASSERT(string_ptr->s16_size > 0);
#endif

    VSF_TGUI_ASSERT((0 <= location_ptr->iX) && (location_ptr->iX < VSF_TGUI_HOR_MAX));                  // x_start point in screen
    VSF_TGUI_ASSERT((0 <= location_ptr->iY) && (location_ptr->iY < VSF_TGUI_VER_MAX));                  // y_start point in screen
    VSF_TGUI_ASSERT(0 <= (location_ptr->iX + dirty_region_ptr->tSize.iWidth));                        // x_end   point in screen
    VSF_TGUI_ASSERT((location_ptr->iX + dirty_region_ptr->tSize.iWidth) <= VSF_TGUI_HOR_MAX);
    VSF_TGUI_ASSERT(0 <= (location_ptr->iY + dirty_region_ptr->tSize.iHeight));                       // y_end   point in screen
    VSF_TGUI_ASSERT((location_ptr->iY + dirty_region_ptr->tSize.iHeight) <= VSF_TGUI_VER_MAX);

    char_draw_region.tSize.iHeight = vsf_tgui_font_get_char_height(font_index);
#if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT == ENABLED
    char_draw_region.tLocation.iX = __sv_text_draw_get_x(string_ptr, font_index, char_offset, mode, origin_resource_size->iWidth);
#endif

    base_location.iX = location_ptr->iX - dirty_region_ptr->tLocation.iX;
    base_location.iY = location_ptr->iY - dirty_region_ptr->tLocation.iY;

    while (((char_u32 = vsf_tgui_text_get_next(string_ptr->pstrText, &char_offset)) != '\0')
#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
        && (char_offset <= string_ptr->s16_size)
#endif
        ) {
        if (char_u32 == '\n') {
#if VSF_TGUI_CFG_SUPPORT_SV_MULTI_LINE_TEXT == ENABLED
            char_draw_region.tLocation.iX = __sv_text_draw_get_x(string_ptr, font_index, char_offset, mode, origin_resource_size->iWidth);
            char_draw_region.tLocation.iY += char_draw_region.tSize.iHeight + inter_line_space;
#endif
            continue;
        }

        if (char_u32 == '\r') {
            continue;
        }

        char_draw_region.tSize.iWidth = vsf_tgui_font_get_char_width(font_index, char_u32);
        if (vsf_tgui_region_intersect(&dirty_char_region, &char_draw_region, dirty_region_ptr)) {
            relative_char_location.iX = dirty_char_region.tLocation.iX - char_draw_region.tLocation.iX;
            relative_char_location.iY = dirty_char_region.tLocation.iY - char_draw_region.tLocation.iY;
            location.iX = base_location.iX + dirty_char_region.tLocation.iX;
            location.iY = base_location.iY + dirty_char_region.tLocation.iY;

            vsf_tgui_sv_port_draw_char(&location, &relative_char_location, &dirty_char_region.tSize, font_index, char_u32, color);

            char_draw_region.tLocation.iX += char_draw_region.tSize.iWidth;
        }
    }
}


void vsf_tgui_control_v_draw_text(  const vsf_tgui_control_t* control_ptr,
                                    const vsf_tgui_region_t* dirty_region_ptr,
                                    vsf_tgui_text_info_t *string_info_ptr,
                                    const uint8_t font_index,
                                    const vsf_tgui_sv_color_t color,
                                    const vsf_tgui_align_mode_t mode)
{
    vsf_tgui_region_t resource_region = {{0, 0}, {0,0}};
    vsf_tgui_size_t size;
    vsf_tgui_location_t real_location;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(dirty_region_ptr != NULL);
    VSF_TGUI_ASSERT(string_info_ptr != NULL);
    VSF_TGUI_ASSERT(string_info_ptr->tString.pstrText != NULL);

#if VSF_TGUI_CFG_SAFE_STRING_MODE == ENABLED
    VSF_TGUI_ASSERT(string_info_ptr->tString.s16_size > 0);
#endif

    if (!__sv_text_get_size(font_index, string_info_ptr, &size)) {
        return ;
    }
    resource_region.tSize = size;

    if (__sv_update_place_resouce_with_align(control_ptr, dirty_region_ptr, mode, &real_location, &resource_region)) {
#if VSF_TGUI_CFG_SV_DRAW_LOG == ENABLED
        VSF_TGUI_LOG(VSF_TRACE_INFO,
            "[Simple View]%s draw text(%s) in (:%d, y:%d), size(w:%d, h:%d)" VSF_TRACE_CFG_LINEEND,
            __sv_get_node_name(control_ptr), string_info_ptr->tString, real_location.iX, real_location.iY,
            resource_region.tSize.iWidth, resource_region.tSize.iHeight);
#endif
        __sv_text_draw(&real_location, &size, &resource_region, &(string_info_ptr->tString),
                       font_index, color, string_info_ptr->chInterLineSpace, mode);
    }
}

#endif


/* EOF */
