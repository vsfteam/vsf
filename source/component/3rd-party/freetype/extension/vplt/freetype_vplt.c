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

#include "component/vsf_component_cfg.h"

#if VSF_USE_FREETYPE == ENABLED

// include vsf.h for bool
#include "vsf.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_COLOR_H
#include FT_SIZES_H
#include FT_CACHE_H

#include "./freetype_vplt.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/

__VSF_VPLT_DECORATOR__ vsf_freetype_vplt_t vsf_freetype_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_freetype_vplt_t, 0, 0, true),
    VSF_APPLET_VPLT_ENTRY_MOD(freetype, vsf_freetype_vplt),

    // Core APIs
    //  Library Setup
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Init_FreeType),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Done_FreeType),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Library_Version),
    //  Face
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_New_Face),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Done_Face),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Reference_Face),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_New_Memory_Face),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Face_Properties),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Open_Face),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Attach_File),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Attach_Stream),
    //  Sizing and Scaling
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Set_Char_Size),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Set_Pixel_Sizes),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Request_Size),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Select_Size),
#if FREETYPE_MAJOR >= 2 && FREETYPE_MINOR >= 11
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Set_Transform),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Transform),
#endif
    //  Glyph
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Load_Glyph),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Render_Glyph),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Kerning),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Track_Kerning),
    //  Character mapping
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Select_Charmap),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Set_Charmap),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Charmap_Index),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Char_Index),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_First_Char),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Next_Char),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Load_Char),
    //  Information
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Name_Index),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Glyph_Name),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Postscript_Name),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_FSType_Flags),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_SubGlyph_Info),
    //  Other
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Face_CheckTrueTypePatents),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Face_SetUnpatentedHinting),

    // Extended APIs
    //  Unicode
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Face_GetCharVariantIndex),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Face_GetCharVariantIsDefault),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Face_GetVariantSelectors),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Face_GetVariantsOfChar),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Face_GetCharsOfVariant),
    //  Glyph Color Management
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Palette_Data_Get),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Palette_Select),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Palette_Set_Foreground_Color),
    //  Glyph Layer Management
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Color_Glyph_Layer),
#if FREETYPE_MAJOR >= 2 && FREETYPE_MINOR >= 13
VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Color_Glyph_Paint),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Color_Glyph_ClipBox),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Paint_Layers),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Colorline_Stops),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Paint),
#endif
    //  Glyph Management
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_New_Glyph),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Get_Glyph),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Glyph_Copy),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Glyph_Transform),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Glyph_Get_CBox),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Glyph_To_Bitmap),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Done_Glyph),
    //  Size Management
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_New_Size),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Done_Size),
    VSF_APPLET_VPLT_ENTRY_FUNC(FT_Activate_Size),

    // Cache Sub-System
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_Manager_New),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_Manager_Reset),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_Manager_Done),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_Manager_LookupFace),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_Manager_LookupSize),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_Manager_RemoveFaceID),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_Node_Unref),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_ImageCache_New),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_ImageCache_Lookup),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_SBitCache_New),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_SBitCache_Lookup),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_CMapCache_New),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_CMapCache_Lookup),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_ImageCache_LookupScaler),
    VSF_APPLET_VPLT_ENTRY_FUNC(FTC_SBitCache_LookupScaler),
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ IMPLEMENTATION ================================*/

#endif      // VSF_USE_FREETYPE
/* EOF */
