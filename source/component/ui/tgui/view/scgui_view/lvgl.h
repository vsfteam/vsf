

/**
 * @file lvgl.h
 * Include all LittleV GL related headers
 */

#ifndef _LVGL_H
#define _LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/
/*Current version of LVGL*/
#define LVGL_VERSION_MAJOR   7
#define LVGL_VERSION_MINOR   0
#define LVGL_VERSION_PATCH   1
#define LVGL_VERSION_INFO    "dev"


#ifndef NULL
   #define NULL ((void*) 0)
#endif

#include <stdbool.h>

#define FALSE   0
#define TRUE    1
//#define false   0
//#define true    1

//typedef unsigned char   u8, bool, BOOL, bit1, uint8_t, BaseType_t;

typedef signed char    int8_t;
//typedef unsigned char   bool;
typedef unsigned char   u8,  uint8_t;
typedef unsigned short  u16, uint16_t;
typedef signed short    s16, int16_t;
typedef unsigned int    u32, tu8, tu16, tbool, tu32, uint32_t;
typedef signed int      s32,int32_t;
/* -------------------------------------------------------------------------------- */
/* -- TYPEDEFS                                                                   -- */
/* -------------------------------------------------------------------------------- */

typedef int16_t lv_coord_t;
typedef uint32_t lv_uintptr_t;

//lv_font_fmt_txt.h
/** 描述一个符号. */
typedef struct {
    uint32_t bitmap_index : 20;     /**< Start index of the bitmap. A font can be max 1 MB. */
    uint32_t adv_w : 12;            /**< Draw the next glyph after this width. 8.4 format (real_value * 16 is stored). */
    uint8_t box_w;                  /**< Width of the glyph's bounding box*/
    uint8_t box_h;                  /**< Height of the glyph's bounding box*/
    int8_t ofs_x;                   /**< x offset of the bounding box*/
    int8_t ofs_y;                  /**< y offset of the bounding box. Measured from the top of the line*/
} lv_font_fmt_txt_glyph_dsc_t;


/** Format of font character map. */
enum {
    LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY,
    LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL,
    LV_FONT_FMT_TXT_CMAP_SPARSE_TINY,
    LV_FONT_FMT_TXT_CMAP_SPARSE_FULL,
};

typedef uint8_t lv_font_fmt_txt_cmap_type_t;

/* Map codepoints to a `glyph_dsc`s
 * Several formats are supported to optimize memory usage
 * See https://github.com/lvgl/lv_font_conv/blob/master/doc/font_spec.md
 */
typedef struct {
    /** First Unicode character for this range */
    uint32_t range_start;

    /** Number of Unicode characters related to this range.
     * Last Unicode character = range_start + range_length - 1*/
    uint16_t range_length;

    /** First glyph ID (array index of `glyph_dsc`) for this range */
    uint16_t glyph_id_start;

    const uint16_t * unicode_list;

    /** if(type == LV_FONT_FMT_TXT_CMAP_FORMAT0_...) it's `uint8_t *`
     * if(type == LV_FONT_FMT_TXT_CMAP_SPARSE_...)  it's `uint16_t *`
     */
    const void * glyph_id_ofs_list;

    /** Length of `unicode_list` and/or `glyph_id_ofs_list`*/
    uint16_t list_length;

    /** Type of this character map*/
    lv_font_fmt_txt_cmap_type_t type;
} lv_font_fmt_txt_cmap_t;


//lv_font_fmt_txt.h
/*Describe store additional data for fonts */
typedef struct {
    /*The bitmaps os all glyphs*/
    const uint8_t * glyph_bitmap;

    /*Describe the glyphs*/
    const lv_font_fmt_txt_glyph_dsc_t * glyph_dsc;

    /* Map the glyphs to Unicode characters.
     * Array of `lv_font_cmap_fmt_txt_t` variables*/
    const lv_font_fmt_txt_cmap_t * cmaps;

    /* Store kerning values.
     * Can be  `lv_font_fmt_txt_kern_pair_t *  or `lv_font_kern_classes_fmt_txt_t *`
     * depending on `kern_classes`
     */
    const void * kern_dsc;

    /*Scale kern values in 12.4 format*/
    uint16_t kern_scale;

    /*Number of cmap tables*/
    uint16_t cmap_num       : 10;

    /*Bit per pixel: 1, 2, 3, 4, 8*/
    uint16_t bpp            : 4;

    /*Type of `kern_dsc`*/
    uint16_t kern_classes   : 1;

    /*
     * storage format of the bitmap
     * from `lv_font_fmt_txt_bitmap_format_t`
     */
    uint16_t bitmap_format  : 2;

    /*Cache the last letter and is glyph id*/
    uint32_t last_letter;
    uint32_t last_glyph_id;

} lv_font_fmt_txt_dsc_t;

/** 描述符号的属性。*/
typedef struct {
    uint16_t adv_w; /**< The glyph needs this space. Draw the next glyph after this width. 8 bit integer, 4 bit fractional */
    uint16_t box_w;  /**< Width of the glyph's bounding box*/
    uint16_t box_h;  /**< Height of the glyph's bounding box*/
    int16_t ofs_x;   /**< x offset of the bounding box*/
    int16_t ofs_y;   /**< y offset of the bounding box*/
    uint8_t bpp;    /**< Bit-per-pixel: 1, 2, 4, 8*/
} lv_font_glyph_dsc_t;

//lv_font.h
/** Describe the properties of a font*/
typedef struct _lv_font_struct {
    /** Get a glyph's  descriptor from a font*/
    bool (*get_glyph_dsc)(const struct _lv_font_struct *, lv_font_glyph_dsc_t *, uint32_t letter, uint32_t letter_next);

    /** Get a glyph's bitmap from a font*/
    const uint8_t * (*get_glyph_bitmap)(const struct _lv_font_struct *, uint32_t);

    /*Pointer to the font in a font pack (must have the same line height)*/
    lv_coord_t line_height;         /**< The real line height where any text fits*/
    lv_coord_t base_line;           /**< Base line measured from the top of the line_height*/
    uint8_t subpx  : 2;             /**< An element of `lv_font_subpx_t`*/
    void * dsc;                     /**< Store implementation specific or run_time data or caching here*/

} lv_font_t;
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/** Gives 1 if the x.y.z version is supported in the current version
 * Usage:
 *
 * - Require v6
 * #if LV_VERSION_CHECK(6,0,0)
 *   new_func_in_v6();
 * #endif
 *
 *
 * - Require at least v5.3
 * #if LV_VERSION_CHECK(5,3,0)
 *   new_feature_from_v5_3();
 * #endif
 *
 *
 * - Require v5.3.2 bugfixes
 * #if LV_VERSION_CHECK(5,3,2)
 *   bugfix_in_v5_3_2();
 * #endif
 *
 * */
#define LV_VERSION_CHECK(x,y,z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))


#ifdef __cplusplus

}
#endif

#endif /*LVGL_H*/
