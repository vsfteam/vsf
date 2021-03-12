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

#if VSF_USE_MAL == ENABLED && VSF_MAL_USE_FAKEFAT32_MAL == ENABLED

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

#if APP_USE_VSFVM_DEMO == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
static const char __test_dart[] = "\
var i = 0;\r\n\
\r\n\
thread1(cnt) {\r\n\
  var i = 0;\r\n\
  while (i < cnt) {\r\n\
    delay_ms(500);\r\n\
    print(\"thread1 loop: \", i, \"\\r\\n\");\r\n\
    i = i + 1;\r\n\
  }\r\n\
}\r\n\
\r\n\
thread(thread1, 5);\r\n\
\r\n\
while (i < 2) {\r\n\
  delay_ms(1000);\r\n\
  print(\"thread main loop: \", i, \"\\r\\n\");\r\n\
  i = i + 1;\r\n\
}\r\n\
";
#   else
static const char __test_dart[] = "\
var i = 0;\r\n\
\r\n\
thread1(cnt) {\r\n\
  var i = 0;\r\n\
  while (i < cnt) {\r\n\
    print(\"thread1 loop: \", i, \"\\r\\n\");\r\n\
    i = i + 1;\r\n\
  }\r\n\
}\r\n\
\r\n\
thread(thread1, 5);\r\n\
\r\n\
while (i < 2) {\r\n\
  print(\"thread main loop: \", i, \"\\r\\n\");\r\n\
  i = i + 1;\r\n\
}\r\n\
";
#   endif
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

vk_fakefat32_file_t fakefat32_root[ 3
                                +   (APP_USE_VSFVM_DEMO == ENABLED ? 1 : 0)
                                ] = {
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
#if APP_USE_VSFVM_DEMO == ENABLED
    {
        .name               = "test.dart",
        .size               = sizeof(__test_dart),
        .attr               = VSF_FILE_ATTR_READ,
        .f.buff             = (uint8_t *)__test_dart,
    },
#endif
};

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

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

#endif

/* EOF */
