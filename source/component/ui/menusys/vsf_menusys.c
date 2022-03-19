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

#include "../vsf_ui_cfg.h"

#if VSF_USE_MENUSYS == ENABLED

#define __VSF_MENUSYS_IMPLEMENT
#include "./vsf_menusys.h"

#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsf_menusys_init(vsf_menusys_t *menusys)
{
    VSF_UI_ASSERT(menusys != NULL);
    menusys->screen_stack_pos = -1;
}

vsf_menusys_menu_t * vsf_menusys_poll(vsf_menusys_t *menusys, int_fast16_t *idx)
{
    vsf_menusys_menu_t *menu;
    VSF_UI_ASSERT((menusys != NULL) && (idx != NULL));

    if (*idx < 0) {
        *idx = 0;
    }
    if (menusys->screen_stack_pos >= 0) {
        int_fast16_t idx_orig = *idx;
        vsf_menusys_screen_t *cur_screen = menusys->screen_stack[menusys->screen_stack_pos];
        while (*idx < cur_screen->menu_num) {
            menu = &cur_screen->menus[(*idx)++];
            if (menu->is_updating) {
                return menu;
            }
        }

        if (idx_orig != 0) {
            *idx = 0;
            return NULL;
        }
    }
    *idx = -1;
    return NULL;
}

void vsf_menusys_menu_updated(vsf_menusys_t *menusys, vsf_menusys_menu_t *menu)
{
    menu->is_updating = false;
}

static vsf_menusys_menu_t * __vsf_menusys_get_menu(vsf_menusys_t *menusys, uint_fast8_t menu_idx)
{
    VSF_UI_ASSERT((menusys != NULL) && (menusys->screen_stack_pos >= 0) && (menusys->screen_stack_pos < menusys->screen_stack_size));
    vsf_menusys_screen_t *cur_screen = menusys->screen_stack[menusys->screen_stack_pos];
    VSF_UI_ASSERT(cur_screen->menu_num > menu_idx);
    return &cur_screen->menus[menu_idx];
}

void vsf_menusys_move_item_absolute(vsf_menusys_t *menusys, uint_fast8_t menu_idx, uint_fast8_t item_idx)
{
    vsf_menusys_menu_t *menu = __vsf_menusys_get_menu(menusys, menu_idx);
    if (!menu->is_updating) {
        vsf_menusys_menu_info_t *info = menu->info;
        uint_fast8_t old_idx = menu->item_pos;

        VSF_UI_ASSERT(item_idx < info->item_num);
        menu->item_pos = item_idx;
        if (old_idx != menu->item_pos) {
            menu->item_pos_pre = old_idx;
            menu->is_switching = true;
            menu->is_updating = true;
            menu->update_state = 0;
        }
    }
}

void vsf_menusys_move_item_relative(vsf_menusys_t *menusys, uint_fast8_t menu_idx, int_fast8_t step)
{
    vsf_menusys_menu_t *menu = __vsf_menusys_get_menu(menusys, menu_idx);
    if (!menu->is_updating) {
        vsf_menusys_menu_info_t *info = menu->info;
        VSF_UI_ASSERT((step > -info->item_num) && (step < info->item_num));
        vsf_menusys_move_item_absolute(menusys, menu_idx, (menu->item_pos + info->item_num + step) % info->item_num);
    }
}

static void __vf_menusys_screen_clear_update(vsf_menusys_screen_t *screen)
{
    vsf_menusys_menu_t *menu = &screen->menus[0];
    for (uint_fast8_t i = 0; i < screen->menu_num; i++, menu++) {
        menu->is_switching = false;
        menu->is_updating = false;
        menu->update_state = 0;
    }
}

static void __vf_menusys_screen_set_update(vsf_menusys_screen_t *screen)
{
    vsf_menusys_menu_t *menu = &screen->menus[0];
    for (uint_fast8_t i = 0; i < screen->menu_num; i++, menu++) {
        menu->is_switching = false;
        menu->is_updating = true;
        menu->update_state = 0;
    }
}

void vsf_menusys_enter_screen(vsf_menusys_t *menusys, vsf_menusys_screen_t *screen)
{
    VSF_UI_ASSERT((menusys != NULL) && (menusys->screen_stack_pos < menusys->screen_stack_size));

    if (menusys->screen_stack_pos >= 0) {
        __vf_menusys_screen_clear_update(menusys->screen_stack[menusys->screen_stack_pos]);
    }
    __vf_menusys_screen_set_update(screen);
    menusys->screen_stack[++menusys->screen_stack_pos] = screen;
}

void vsf_menusys_leave_screen(vsf_menusys_t *menusys)
{
    VSF_UI_ASSERT((menusys != NULL) && (menusys->screen_stack_pos >= 0));

    menusys->screen_stack_pos--;
    if (menusys->screen_stack_pos >= 0) {
        __vf_menusys_screen_set_update(menusys->screen_stack[menusys->screen_stack_pos]);
    }
}

void vsf_menusys_select_cur_item(vsf_menusys_t *menusys, uint_fast8_t menu_idx)
{
    vsf_menusys_menu_t *menu = __vsf_menusys_get_menu(menusys, menu_idx);
    VSF_UI_ASSERT(menu->item_pos < menu->info->item_num);
    vsf_menusys_menu_info_t *menu_info = menu->info;
    vsf_menusys_menu_item_t *menu_item = &menu_info->items[menu->item_pos];

    switch (menu_item->anchor_type) {
    case VSF_MENU_ITEM_ANCHOR_TYPE_MENU:
        if (menu_item->anchor.screen != NULL) {
            vsf_menusys_enter_screen(menusys, menu_item->anchor.screen);
        } else {
            vsf_menusys_leave_screen(menusys);
        }
        break;
    case VSF_MENU_ITEM_ANCHOR_TYPE_CALLBACK:
        if (menu_item->anchor.callback != NULL) {
            menu_item->anchor.callback(menu, menu_idx);
        }
        break;
    }
}

#endif      // VSF_USE_MENUSYS
