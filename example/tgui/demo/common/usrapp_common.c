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

#include "./usrapp_common.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if     VSF_USE_FS == ENABLED                                                   \
    &&  (   VSF_MAL_USE_FAKEFAT32_MAL == ENABLED                                \
        ||  VSF_FS_USE_MEMFS == ENABLED                                         \
        ||  VSF_FS_USE_WINFS == ENABLED)

usrapp_common_t usrapp_common = {
#   if      VSF_USE_MAL == ENABLED                                              \
        &&  VSF_MAL_USE_FAKEFAT32_MAL == ENABLED
    .mal                        = {
#       if VSF_MAL_USE_FAKEFAT32_MAL == ENABLED
        .fakefat32              = {
            .drv                = &vk_fakefat32_mal_drv,
            .sector_size        = USRAPP_CFG_FAKEFAT32_SECTOR_SIZE,
            .sector_number      = USRAPP_CFG_FAKEFAT32_SIZE / USRAPP_CFG_FAKEFAT32_SECTOR_SIZE,
            .sectors_per_cluster= 8,
            .volume_id          = 0x12345678,
            .disk_id            = 0x9ABCEF01,
            .root               = {
                .name           = "ROOT",
                .d.child        = (vk_memfs_file_t *)USRAPP_CFG_FAKEFAT32_ROOT,
                .d.child_num    = dimof(USRAPP_CFG_FAKEFAT32_ROOT),
            },
        },
#       endif
    },
#   endif

#   if      VSF_USE_FS == ENABLED                                               \
        &&  (VSF_FS_USE_MEMFS == ENABLED || VSF_FS_USE_WINFS == ENABLED)
    .fs                         = {
#       if VSF_FS_USE_MEMFS == ENABLED
        .memfs_info             = {
            .root               = {
                .d.child        = (vk_memfs_file_t *)USRAPP_CFG_MEMFS_ROOT,
                .d.child_num    = dimof(USRAPP_CFG_MEMFS_ROOT),
                .d.child_size   = sizeof(vk_fakefat32_file_t),
            },
        },
#       endif
#       if VSF_FS_USE_WINFS == ENABLED
        .winfs_info             = {
            .root               = {
                .name           = USRAPP_CFG_WINFS_ROOT,
            },
        },
#       endif
    },
#   endif
};
#endif

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/* EOF */