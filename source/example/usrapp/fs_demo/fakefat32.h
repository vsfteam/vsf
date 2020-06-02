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

#ifndef __FAKEFAT32_H__
#define __FAKEFAT32_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

dcl_vsf_peda_methods(static, __usrapp_on_file_read)
dcl_vsf_peda_methods(static, __usrapp_on_file_write)

/*============================ LOCAL VARIABLES ===============================*/

static const char __readme[] = "\
readme\r\n\
";
static uint8_t __control = 0;

/*============================ GLOBAL VARIABLES ==============================*/

static vk_fakefat32_file_t __fakefat32_root[] = {
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
        .callback.read      = (vsf_peda_evthandler_t)vsf_peda_func(__usrapp_on_file_read),
        .callback.write     = (vsf_peda_evthandler_t)vsf_peda_func(__usrapp_on_file_write),
    },
};

/*============================ IMPLEMENTATION ================================*/

vsf_component_peda_ifs_entry(__usrapp_on_file_read, vk_memfs_callback_read)
{
    vsf_peda_begin();
    uint8_t *buff = vsf_local.buff;
    int_fast32_t rlen;

    if ((vsf_local.offset == 0) && (vsf_local.size > 1)) {
        buff[0] = __control;
        vsf_trace(VSF_TRACE_INFO, "read control: %d" VSF_TRACE_CFG_LINEEND, __control);
        rlen = 1;
    } else {
        rlen = VSF_ERR_FAIL;
    }

    vsf_eda_return(rlen);
    vsf_peda_end();
}

vsf_component_peda_ifs_entry(__usrapp_on_file_write, vk_memfs_callback_write)
{
    vsf_peda_begin();
    uint8_t *buff = vsf_local.buff;
    int_fast32_t wlen;

    if ((vsf_local.offset == 0) && (vsf_local.size == 1)) {
        __control = buff[0];
        vsf_trace(VSF_TRACE_INFO, "write control: %d" VSF_TRACE_CFG_LINEEND, __control);
        wlen = 1;
    } else {
        wlen = VSF_ERR_FAIL;
    }

    vsf_eda_return(wlen);
    vsf_peda_end();
}

#endif      // __FAKEFAT32_H__

/* EOF */
