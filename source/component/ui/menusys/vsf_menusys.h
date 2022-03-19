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

#ifndef __VSF_MENUSYS_H__
#define __VSF_MENUSYS_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_ui_cfg.h"

#if VSF_USE_MENUSYS == ENABLED

#if     defined(__VSF_MENUSYS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT
#   undef VSF_MENUSYS_IMPLEMENT
#elif   defined(VSF_MENUSYS_INHERIT)
#   define __PLOOC_CLASS_INHERIT
#   undef VSF_MENUSYS_INHERIT
#endif

#include "utilities/ooc_class.h"

/********************************************************************************
*                               vsf_menusys_t                                   *
*********************************************************************************
*           vsf_menusys_screen_t            *       vsf_menusys_screen_t        *
*********************************************************************************
* vsf_menusys_menu_t * vsf_menusys_screen_t *       vsf_menusys_menu_t          *
* vsf_menu_item_t    * vsf_menu_item_t      *       vsf_menu_item_t             *
* vsf_menu_item_t    * vsf_menu_item_t      *       vsf_menu_item_t             *
* vsf_menu_item_t    * vsf_menu_item_t      *       vsf_menu_item_t             *
*********************************************************************************
* 
* vsf_menusys_t represents the menu system, which has a screen stack and some
* screens. Current screen is at the top of the screen stack. A screen consists of
* some menus displaying at the same time. A menu has some items.
*/

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define __describe_menu(__name, __idx, __id, __x, __y, __w, __h, __item_num, __user_type, ...)\
        typedef struct __##__name##_menu##__idx##_info_t {                      \
            implement(vsf_menusys_menu_info_t)                                  \
            vsf_menusys_menu_item_t __items[(__item_num)];                      \
        } __##__name##_menu##__idx##_info_t;                                    \
        __##__name##_menu##__idx##_info_t __##__name##_menu##__idx##_info = {   \
            .x              = (__x),                                            \
            .y              = (__y),                                            \
            .w              = (__w),                                            \
            .h              = (__h),                                            \
            .item_num       = (__item_num),                                     \
            .id             = (__id),                                           \
            .user_type      = (__user_type),                                    \
            .__items        = {                                                 \
                __VA_ARGS__                                                     \
            },                                                                  \
        };

#define __describe_menu_item(__idx, __ui_data)                                  \
        [__idx] = { .ui_data = (void *)(__ui_data), .anchor_type = VSF_MENU_ITEM_ANCHOR_TYPE_NONE, }
#define __describe_menu_item_anchor_menu(__idx, __ui_data, __screen)            \
        [__idx] = { .ui_data = (void *)(__ui_data), .anchor_type = VSF_MENU_ITEM_ANCHOR_TYPE_MENU, .anchor.screen = (vsf_menusys_screen_t *)(__screen), }
#define __describe_menu_item_anchor_callback(__idx, __ui_data, __callback)      \
        [__idx] = { .ui_data = (void *)(__ui_data), .anchor_type = VSF_MENU_ITEM_ANCHOR_TYPE_CALLBACK, .anchor.callback = (__callback), }

#define __end_describe_menu(__n, __value)                                       \
        { .info = &__##__value##_menu##__n##_info.use_as__vsf_menusys_menu_info_t, },

#define __end_describe_menu_screen(__name, __menu_num)                          \
        typedef struct __##__name##_menu_screen_t {                             \
            implement(vsf_menusys_screen_t)                                     \
            vsf_menusys_menu_t __menus[(__menu_num)];                           \
        } __##__name##_menu_screen_t;                                           \
        __##__name##_menu_screen_t __name = {                                   \
            .menu_num = (__menu_num),                                           \
            .__menus = {                                                        \
                VSF_MREPEAT(__menu_num, __end_describe_menu, __name)            \
            },                                                                  \
        };

#define describe_menu_screen(__name)
#define describe_menu(__name, __idx, __id, __x, __y, __w, __h, __item_num, __user_type, ...)\
        __describe_menu(__name, __idx, (__id), (__x), (__y), (__w), (__h), (__item_num), (__user_type), __VA_ARGS__)
#define describe_menu_item(__idx, __ui_data)                                    \
        __describe_menu_item(__idx, (__ui_data))
#define describe_menu_item_anchor_menu(__idx, __ui_data, __screen)              \
        __describe_menu_item_anchor_menu((__idx), (__ui_data), (__screen))
#define describe_menu_item_anchor_callback(__idx, __ui_data, __callback)        \
        __describe_menu_item_anchor_callback((__idx), (__ui_data), (__callback))
#define end_describe_menu_screen(__name, __menu_num)                            \
        __end_describe_menu_screen(__name, __menu_num)

#define describe_menusys(__name, __screen_stack_size)                           \
        typedef struct __##__name##_menusys_t {                                 \
            implement(vsf_menusys_t)                                            \
            vsf_menusys_screen_t * __screen_stack[(__screen_stack_size)];       \
        } __##__name##_menusys_t;                                               \
        __##__name##_menusys_t __name = {                                       \
            .screen_stack_size = (__screen_stack_size),                         \
        };

/*============================ TYPES =========================================*/

declare_simple_class(vsf_menusys_t)
declare_simple_class(vsf_menusys_screen_t)
declare_simple_class(vsf_menusys_menu_t)

typedef enum vsf_menusys_item_anchor_type_t {
    VSF_MENU_ITEM_ANCHOR_TYPE_NONE,
    VSF_MENU_ITEM_ANCHOR_TYPE_MENU,
    VSF_MENU_ITEM_ANCHOR_TYPE_CALLBACK,
} vsf_menusys_item_anchor_type_t;

typedef void (*vsf_menusys_menu_anchor_callback_t)(vsf_menusys_menu_t *menu, uint_fast16_t idx);
typedef struct vsf_menusys_menu_item_t {
    void * ui_data;
    vsf_menusys_item_anchor_type_t anchor_type;
    union {
        vsf_menusys_screen_t *screen;
        vsf_menusys_menu_anchor_callback_t callback;
    } anchor;
} vsf_menusys_menu_item_t;

// vsf_menu_info_t must be followed by actual vsf_menu_item_t array
typedef struct vsf_menusys_menu_info_t {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t item_num;
    uint8_t id;
    uint8_t user_type;
    vsf_menusys_menu_item_t items[0];
} vsf_menusys_menu_info_t;

def_simple_class(vsf_menusys_menu_t) {
    vsf_menusys_menu_info_t *info;
    uint16_t item_pos;
    uint16_t item_pos_pre;
    bool is_switching;
    bool is_updating;
    uint8_t update_state;
};

def_simple_class(vsf_menusys_screen_t) {
    uint8_t menu_num;
    vsf_menusys_menu_t menus[0];
};

def_simple_class(vsf_menusys_t) {
    public_member(
        uint8_t screen_stack_size;
    )

    private_member(
        int8_t screen_stack_pos;
        vsf_menusys_screen_t * screen_stack[0];
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsf_menusys_init(vsf_menusys_t *screen);

extern void vsf_menusys_move_item_relative(vsf_menusys_t *menusys, uint_fast8_t menu_idx, int_fast8_t step);
extern void vsf_menusys_move_item_absolute(vsf_menusys_t *menusys, uint_fast8_t menu_idx, uint_fast8_t item_idx);
extern void vsf_menusys_select_cur_item(vsf_menusys_t *menusys, uint_fast8_t menu_idx);

extern void vsf_menusys_enter_screen(vsf_menusys_t *menusys, vsf_menusys_screen_t *screen);
extern void vsf_menusys_leave_screen(vsf_menusys_t *menusys);

extern vsf_menusys_menu_t * vsf_menusys_poll(vsf_menusys_t *menusys, int_fast16_t *idx);
extern void vsf_menusys_menu_updated(vsf_menusys_t *menusys, vsf_menusys_menu_t *menu);

#endif      // VSF_USE_MENUSYS
#endif      // __VSF_MENUSYS_H__
/* EOF */
