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

#ifndef __VSF_FREETYPE_VPLT_H__
#define __VSF_FREETYPE_VPLT_H__

/*============================ INCLUDES ======================================*/

#include "component/vsf_component_cfg.h"

#if VSF_USE_FREETYPE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsf_freetype_vplt_t {
    vsf_vplt_info_t info;

    // self
    VSF_APPLET_VPLT_ENTRY_MOD_DEF(freetype);

    // Core APIs
    //  Library Setup
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Init_FreeType);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Done_FreeType);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Library_Version);
    //  Face
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_New_Face);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Done_Face);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Reference_Face);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_New_Memory_Face);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Face_Properties);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Open_Face);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Attach_File);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Attach_Stream);
    //  Sizing and Scaling
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Set_Char_Size);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Set_Pixel_Sizes);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Request_Size);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Select_Size);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Set_Transform);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Transform);
    //  Glyph
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Load_Glyph);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Render_Glyph);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Kerning);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Track_Kerning);
    //  Character mapping
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Select_Charmap);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Set_Charmap);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Charmap_Index);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Char_Index);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_First_Char);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Next_Char);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Load_Char);
    //  Information
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Name_Index);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Glyph_Name);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Postscript_Name);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_FSType_Flags);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_SubGlyph_Info);
    //  Other
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Face_CheckTrueTypePatents);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Face_SetUnpatentedHinting);

    // Extended APIs
    //  Unicode
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Face_GetCharVariantIndex);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Face_GetCharVariantIsDefault);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Face_GetVariantSelectors);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Face_GetVariantsOfChar);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Face_GetCharsOfVariant);
    //  Glyph Color Management
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Palette_Data_Get);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Palette_Select);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Palette_Set_Foreground_Color);
    //  Glyph Layer Management
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Color_Glyph_Layer);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Color_Glyph_Paint);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Color_Glyph_ClipBox);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Paint_Layers);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Colorline_Stops);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Paint);
    //  Glyph Management
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_New_Glyph);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Glyph);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Glyph_Copy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Glyph_Transform);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Glyph_Get_CBox);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Glyph_To_Bitmap);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Done_Glyph);
    //  Size Management
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_New_Size);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Done_Size);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Activate_Size);

    // Cache Sub-System
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_Manager_New);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_Manager_Reset);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_Manager_Done);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_Manager_LookupFace);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_Manager_LookupSize);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_Manager_RemoveFaceID);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_Node_Unref);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_ImageCache_New);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_ImageCache_Lookup);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_SBitCache_New);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_SBitCache_Lookup);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_CMapCache_New);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_CMapCache_Lookup);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_ImageCache_LookupScaler);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FTC_SBitCache_LookupScaler);

    // Support API
    //  Computations
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_MulDiv);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_MulFix);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_DivFix);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_RoundFix);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_CeilFix);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_FloorFix);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Vector_Transform);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Matrix_Multiply);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Matrix_Invert);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Sin);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Cos);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Tan);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Atan2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Angle_Diff);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Vector_Unit);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Vector_Rotate);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Vector_Length);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Vector_Polarize);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Vector_From_Polar);
    //  List
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_List_Add);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_List_Insert);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_List_Find);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_List_Remove);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_List_Up);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_List_Iterate);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_List_Finalize);
    //  Outline
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_New);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Done);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Copy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Translate);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Transform);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Embolden);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_EmboldenXY);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Reverse);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Check);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Get_CBox);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Get_BBox);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Get_Bitmap);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Render);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Outline_Decompose);
    //  Advance
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Advance);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Get_Advances);
    // Bitmap
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Bitmap_Init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Bitmap_Copy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Bitmap_Embolden);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Bitmap_Convert);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Bitmap_Blend);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_GlyphSlot_Own_Bitmap);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Bitmap_Done);
    // TODO: more APIs

    // Error
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(FT_Error_String);
} vsf_freetype_vplt_t;

/*============================ GLOBAL VARIABLES ==============================*/

extern __VSF_VPLT_DECORATOR__ vsf_freetype_vplt_t vsf_freetype_vplt;

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_FREETYPE_VPLT_H__
