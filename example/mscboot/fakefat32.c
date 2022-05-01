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

#include "vsf.h"

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED && USRAPP_CFG_FAKEFAT32 == ENABLED

#include "./fakefat32.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __mscboot_on_firmware_read)
dcl_vsf_peda_methods(static, __mscboot_on_firmware_write)

/*============================ LOCAL VARIABLES ===============================*/

static bool __flash_is_inited = false;
static vsf_eda_t *__eda = NULL;

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
        .size               = MSCBOOT_CFG_FW_SIZE,
        .attr               = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .callback.fn_read   = (vsf_peda_evthandler_t)vsf_peda_func(__mscboot_on_firmware_read),
        .callback.fn_write  = (vsf_peda_evthandler_t)vsf_peda_func(__mscboot_on_firmware_write),
    }
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

/*============================ IMPLEMENTATION ================================*/

static void __flash_isrhandler(     void *target_ptr,
                                    vsf_flash_irq_type_t type,
                                    vsf_flash_t *flash_ptr)
{
    VSF_ASSERT(__eda != NULL);
    vsf_eda_post_evt(__eda, VSF_EVT_USER);
}

static void __flash_init(void)
{
    flash_cfg_t cfg         = {
        .isr                = {
            .handler_fn     = __flash_isrhandler,
        },
    };
    vsf_flash_init(&MSCBOOT_CFG_FLASH, &cfg);
    vsf_flash_enable(&MSCBOOT_CFG_FLASH);
}

vsf_component_peda_ifs_entry(__mscboot_on_firmware_read, vk_memfs_callback_read)
{
    vsf_peda_begin();

    uint32_t cur_size;

    switch (evt) {
    case VSF_EVT_INIT:
        if (!__flash_is_inited) {
            __flash_is_inited = true;
            __flash_init();
        }

        vsf_local.offset += MSCBOOT_CFG_FW_ADDR;
        vsf_local.rsize = 0;
        __eda = vsf_eda_get_cur();
        // fall through
    case VSF_EVT_USER:
        if (!vsf_local.size) {
            __eda = NULL;
            vsf_eda_return(vsf_local.rsize);
            break;
        }

#if MSCBOT_CFG_READ_ALIGN != 0
        VSF_ASSERT(!(vsf_local.offset & (MSCBOT_CFG_READ_ALIGN - 1)));
#endif

        cur_size = 0 == MSCBOOT_CFG_READ_BLOCK_SIZE ? vsf_local.size : MSCBOOT_CFG_READ_BLOCK_SIZE;
        VSF_ASSERT(cur_size <= vsf_local.size);
        vsf_flash_read(&MSCBOOT_CFG_FLASH, vsf_local.offset, vsf_local.buff, cur_size);
        vsf_local.offset += cur_size;
        vsf_local.buff += cur_size;
        vsf_local.rsize += cur_size;
        vsf_local.size -= cur_size;
        break;
    }

    vsf_peda_end();
}

vsf_component_peda_ifs_entry(__mscboot_on_firmware_write, vk_memfs_callback_write)
{
    vsf_peda_begin();

    uint32_t cur_size;

    switch (evt) {
    case VSF_EVT_INIT:
        if (!__flash_is_inited) {
            __flash_is_inited = true;
            __flash_init();
        }

        vsf_local.offset += MSCBOOT_CFG_FW_ADDR;
        vsf_local.wsize = 0;
        __eda = vsf_eda_get_cur();
        // fall through
    case VSF_EVT_USER:
        if (!vsf_local.size) {
            __eda = NULL;
            vsf_eda_return(vsf_local.wsize);
            break;
        }

        if (vsf_local.wsize & 1) {
            // erase done
            vsf_local.wsize &= ~1;
        } else {
            // write done, need erase next block?
            VSF_ASSERT(MSCBOOT_CFG_ERASE_ALIGN != 0);
            if (!(vsf_local.offset & (MSCBOOT_CFG_ERASE_ALIGN - 1))) {
                vsf_local.wsize |= 1;
                cur_size = 0 == MSCBOOT_CFG_ERASE_BLOCK_SIZE ? vsf_local.size : MSCBOOT_CFG_ERASE_BLOCK_SIZE;
                vsf_flash_erase(&MSCBOOT_CFG_FLASH, vsf_local.offset, cur_size);
                break;
            }
        }

#if MSCBOT_CFG_WRITE_ALIGN != 0
        VSF_ASSERT(!(vsf_local.offset & (MSCBOT_CFG_WRITE_ALIGN - 1)));
#endif

        cur_size = 0 == MSCBOOT_CFG_WRITE_BLOCK_SIZE ? vsf_local.size : MSCBOOT_CFG_WRITE_BLOCK_SIZE;
        VSF_ASSERT(cur_size <= vsf_local.size);
        vsf_flash_write(&MSCBOOT_CFG_FLASH, vsf_local.offset, vsf_local.buff, cur_size);
        vsf_local.offset += cur_size;
        vsf_local.buff += cur_size;
        vsf_local.wsize += cur_size;
        vsf_local.size -= cur_size;
        break;
    }

    vsf_peda_end();
}

#endif

/* EOF */
