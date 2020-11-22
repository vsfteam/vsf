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

#ifndef __USRAPP_COMMON_H__
#define __USRAPP_COMMON_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"

#if VSF_USE_USB_HOST == ENABLED
#   include "./usrapp_usbh_common.h"
#endif

#if VSF_USE_USB_DEVICE == ENABLED
#   include "./usrapp_usbd_common.h"
#endif

#if VSF_USE_UI == ENABLED
#   include "./usrapp_ui_common.h"
#endif

#if VSF_USE_AUDIO == ENABLED
#   include "./usrapp_audio_common.h"
#endif

#if VSF_USE_TCPIP == ENABLED
#   include "./usrapp_net_common.h"
#endif

#if VSF_FS_USE_MEMFS == ENABLED
#   include "../fakefat32.h"
#endif

/*============================ MACROS ========================================*/

#if VSF_FS_USE_MEMFS == ENABLED
#   define USRAPP_CFG_MEMFS_ROOT                fakefat32_root
#endif
#if VSF_FS_USE_WINFS == ENABLED
#   define USRAPP_CFG_WINFS_ROOT                "winfs_root"
#endif

#if USRAPP_CFG_FAKEFAT32 == ENABLED
#   ifndef USRAPP_CFG_FAKEFAT32_SECTOR_SIZE
#       define USRAPP_CFG_FAKEFAT32_SECTOR_SIZE 512
#   endif
#   ifndef USRAPP_CFG_FAKEFAT32_SIZE
#       define USRAPP_CFG_FAKEFAT32_SIZE        (USRAPP_CFG_FAKEFAT32_SECTOR_SIZE * 0x1040)
#   endif
#   if !defined(USRAPP_CFG_FAKEFAT32_ROOT) && defined(USRAPP_CFG_MEMFS_ROOT)
#       define USRAPP_CFG_FAKEFAT32_ROOT        USRAPP_CFG_MEMFS_ROOT
#   endif
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if     VSF_USE_FS == ENABLED                                                   \
    &&  (   VSF_MAL_USE_FAKEFAT32_MAL == ENABLED                                \
        ||  VSF_FS_USE_MEMFS == ENABLED                                         \
        ||  VSF_FS_USE_WINFS == ENABLED)

typedef struct usrapp_common_t {
#   if      VSF_USE_MAL == ENABLED                                              \
        &&  VSF_MAL_USE_FAKEFAT32_MAL == ENABLED
    struct {
#       if VSF_MAL_USE_FAKEFAT32_MAL == ENABLED
        vk_fakefat32_mal_t fakefat32;
#       endif
    } mal;
#   endif

#   if      VSF_USE_FS == ENABLED                                               \
        &&  (VSF_FS_USE_MEMFS == ENABLED || VSF_FS_USE_WINFS == ENABLED)
    struct {
#       if VSF_FS_USE_MEMFS == ENABLED
        vk_memfs_info_t memfs_info;
#       endif
#       if VSF_FS_USE_WINFS == ENABLED
        vk_winfs_info_t winfs_info;
#       endif
    } fs;
#   endif
} usrapp_common_t;
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_FS == ENABLED                                                   \
    &&  (   VSF_MAL_USE_FAKEFAT32_MAL == ENABLED                                \
        ||  VSF_FS_USE_MEMFS == ENABLED                                         \
        ||  VSF_FS_USE_WINFS == ENABLED)
extern usrapp_common_t usrapp_common;
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif      // __USRAPP_COMMON_H__
/* EOF */