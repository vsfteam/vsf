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

static void usrapp_on_file_read(uintptr_t target, vsf_evt_t evt);
static void usrapp_on_file_write(uintptr_t target, vsf_evt_t evt);

/*============================ LOCAL VARIABLES ===============================*/

static const char readme[] = "\
readme\r\n\
";
static uint8_t control = 0;

/*============================ GLOBAL VARIABLES ==============================*/

static vsf_fakefat32_file_t fakefat32_root[] = {
    {
        .name               = "FAKEFAT32",
        .attr               = VSF_FILE_ATTR_VOLUMID,
    },
    {
        .name               = "readme.txt",
        .size               = sizeof(readme),
        .attr               = VSF_FILE_ATTR_ARCHIVE | VSF_FILE_ATTR_READONLY,
        .f.buff             = (uint8_t *)readme,
    },
    {
        .name               = "control.bin",
        .size               = 1,
        .attr               = VSF_FILE_ATTR_ARCHIVE,
        .callback.read      = usrapp_on_file_read,
        .callback.write     = usrapp_on_file_write,
    },
};

/*============================ IMPLEMENTATION ================================*/

static void usrapp_on_file_read(uintptr_t target, vsf_evt_t evt)
{
    vsf_fakefat32_mal_t *pthis = (vsf_fakefat32_mal_t *)target;
    vsf_fakefat32_file_t *file = pthis->io_ctx.file;
    uint8_t *buff = pthis->io_ctx.buff;

    buff[0] = control;
    vsf_trace(VSF_TRACE_INFO, "read control: %d" VSF_TRACE_CFG_LINEEND, control);
    vsf_eda_return();
}

static void usrapp_on_file_write(uintptr_t target, vsf_evt_t evt)
{
    vsf_fakefat32_mal_t *pthis = (vsf_fakefat32_mal_t *)target;
    vsf_fakefat32_file_t *file = pthis->io_ctx.file;
    uint8_t *buff = pthis->io_ctx.buff;

    control = buff[0];
    vsf_trace(VSF_TRACE_INFO, "write control: %d" VSF_TRACE_CFG_LINEEND, control);
    vsf_eda_return();
}

#endif      // __FAKEFAT32_H__

/* EOF */
