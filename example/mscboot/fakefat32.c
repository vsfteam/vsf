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

#define VSF_FS_INHERIT
#include "vsf.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED && USRAPP_CFG_FAKEFAT32 == ENABLED

#include "./fakefat32.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __mscbot_on_firmware_read)
dcl_vsf_peda_methods(static, __mscbot_on_firmware_write)

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

vk_fakefat32_file_t fakefat32_root[2] = {
    {
        .name               = "mscboot",
        .attr               = (vk_file_attr_t)VSF_FAT_FILE_ATTR_VOLUMID,
    },
    {
        .name               = "firmware.bin",
        .size               = MSCBOT_CFG_FW_SIZE,
        .attr               = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .callback.fn_read   = (vsf_peda_evthandler_t)vsf_peda_func(__mscbot_on_firmware_read),
        .callback.fn_write  = (vsf_peda_evthandler_t)vsf_peda_func(__mscbot_on_firmware_write),
    }
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ IMPLEMENTATION ================================*/

vsf_component_peda_ifs_entry(__mscbot_on_firmware_read, vk_memfs_callback_read)
{
    vsf_peda_begin();
    memset(vsf_local.buff, 0, vsf_local.size);
    vsf_eda_return(vsf_local.size);
    vsf_peda_end();
}

vsf_component_peda_ifs_entry(__mscbot_on_firmware_write, vk_memfs_callback_write)
{
    vsf_peda_begin();
    // TODO: write vsf_local.buff to vsf_local.offset with size vsf_local.size
    vsf_eda_return(vsf_local.size);
    vsf_peda_end();
}

#endif

/* EOF */
