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

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void __usrapp_on_file_read(uintptr_t target, vsf_evt_t evt);
static void __usrapp_on_file_write(uintptr_t target, vsf_evt_t evt);

/*============================ LOCAL VARIABLES ===============================*/

static const char __readme[] = "\
readme\r\n\
";
static uint8_t __control = 0;

/*============================ GLOBAL VARIABLES ==============================*/

vk_fakefat32_file_t fakefat32_root[3] = {
    {
        .name               = "FAKEFAT32",
        .attr               = (vk_file_attr_t)VSF_FAT_FILE_ATTR_VOLUMID,
    },
    {
        .name               = "readme.txt",
        .size               = sizeof(__readme),
        .attr               = VSF_FILE_ATTR_READ,
        .f.buff             = (uint8_t *)__readme,
    },
    {
        .name               = "control.bin",
        .size               = 1,
        .attr               = VSF_FILE_ATTR_READ | VSF_FILE_ATTR_WRITE,
        .callback.read      = __usrapp_on_file_read,
        .callback.write     = __usrapp_on_file_write,
    },
};

/*============================ IMPLEMENTATION ================================*/

static void __usrapp_on_file_read(uintptr_t target, vsf_evt_t evt)
{
    vk_fakefat32_file_t *file = (vk_fakefat32_file_t *)target;
    uint8_t *buff = vk_file_get_ctx(&file->use_as__vk_file_t)->io.buff;
    vsf_err_t err;
    int_fast32_t rlen;

    if ((file->ctx.io.offset == 0) && (file->ctx.io.size > 1)) {
        buff[0] = __control;
        vsf_trace(VSF_TRACE_INFO, "read control: %d" VSF_TRACE_CFG_LINEEND, __control);
        rlen = 1;
        err = VSF_ERR_NONE;
    } else {
        rlen = 0;
        err = VSF_ERR_FAIL;
    }

    vk_file_set_io_result(&file->use_as__vk_file_t, err, rlen);
    vk_fakefat32_return(file, err);
}

static void __usrapp_on_file_write(uintptr_t target, vsf_evt_t evt)
{
    vk_fakefat32_file_t *file = (vk_fakefat32_file_t *)target;
    uint8_t *buff = vk_file_get_ctx(&file->use_as__vk_file_t)->io.buff;
    vsf_err_t err;
    int_fast32_t wlen;

    if ((file->ctx.io.offset == 0) && (file->ctx.io.size == 1)) {
        __control = buff[0];
        vsf_trace(VSF_TRACE_INFO, "write control: %d" VSF_TRACE_CFG_LINEEND, __control);
        wlen = 1;
        err = VSF_ERR_NONE;
    } else {
        wlen = 0;
        err = VSF_ERR_FAIL;
    }

    vk_file_set_io_result(&file->use_as__vk_file_t, err, wlen);
    vk_fakefat32_return(file, err);
}

/* EOF */
