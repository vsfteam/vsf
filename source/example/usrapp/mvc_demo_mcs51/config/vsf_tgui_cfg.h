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


//! \note Top Level Application Configuration 

#ifndef __TOP_TGUI_USER_CFG_H__
#define __TOP_TGUI_USER_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

/*! \note Users should assign values to those enum symbols according to the 
 *!       target platform.
 *! 
 *! \note Platform: PC
 */
enum {

    VSF_TGUI_KEY_BACKSPACE      = 0x08,
    VSF_TGUI_KEY_OK             = 0x0D,
    VSF_TGUI_KEY_CANCEL         = 0x1B,

    VSF_TGUI_KEY_INSERT         = 0x49,
    VSF_TGUI_KEY_HOME           = 0x4A,
    VSF_TGUI_KEY_PAGE_UP        = 0x4B,
    VSF_TGUI_KEY_END            = 0x4D,
    VSF_TGUI_KEY_PAGE_DOWN      = 0x4E,
    VSF_TGUI_KEY_RIGHT          = 0x4F,
    VSF_TGUI_KEY_LEFT           = 0x50,
    VSF_TGUI_KEY_UP             = 0x51,
    VSF_TGUI_KEY_DOWN           = 0x52,

    VSF_TGUI_KEY_MUTE           = 0x106,
    VSF_TGUI_KEY_VOLUME_DOWN    = 0x81,
    VSF_TGUI_KEY_VOLUME_UP      = 0x80,


    VSF_TGUI_KEY_SYSTEM_CALL    = 0xE3,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */