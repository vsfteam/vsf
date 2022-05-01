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

dcl_vsf_peda_methods(static, __usrapp_on_file_read)
dcl_vsf_peda_methods(static, __usrapp_on_file_write)

/*============================ LOCAL VARIABLES ===============================*/

static const char __readme[] = "\
readme\r\n\
";
static uint8_t __control = 0;

#if APP_USE_VSFVM_DEMO == ENABLED
#   if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
static const char __test_dart[] = STR(
var i = 0;

thread1(cnt) {
  var i = 0;
  while (i < cnt) {
    delay_ms(500);
    print("thread1 loop: ", i, "\r\n");
    i = i + 1;
  }
}

thread(thread1, 5);

while (i < 2) {
  delay_ms(1000);
  print("thread main loop: ", i, "\r\n");
  i = i + 1;
}
);
#   else
static const char __test_dart[] = STR(
var i = 0;

thread1(cnt) {
  var i = 0;
  while (i < cnt) {
    print("thread1 loop: ", i, "\r\n");
    i = i + 1;
  }
}

thread(thread1, 5);

while (i < 2) {
  print("thread main loop: ", i, "\r\n");
  i = i + 1;
}
);
#   endif
#endif

#if APP_USE_EVM_DEMO == ENABLED
static const char __evm_main_js[] = STR(
  print('hello,evm')
);
static vk_fakefat32_file_t __evm_root[3] = {
    {
        .name               = ".",
        .attr               = VSF_FILE_ATTR_DIRECTORY,
    },
    {
        .name               = "..",
        .attr               = VSF_FILE_ATTR_DIRECTORY,
    },
    {
        .name               = "main.js",
        .size               = sizeof(__evm_main_js),
        .attr               = VSF_FILE_ATTR_READ,
        .f.buff             = (uint8_t *)__evm_main_js,
    },
};
#endif

#if USRAPP_FAKEFAT32_CFG_FONT == ENABLED
static const uint8_t __roboto_regular_ttf[] = {
#   include "./Roboto-Regular.inc"
};
static vk_fakefat32_file_t __fakefat32_font[3] = {
    {
        .name               = ".",
        .attr               = VSF_FILE_ATTR_DIRECTORY,
    },
    {
        .name               = "..",
        .attr               = VSF_FILE_ATTR_DIRECTORY,
    },
    {
        .name               = "Roboto-Regular.ttf",
        .size               = sizeof(__roboto_regular_ttf),
        .attr               = VSF_FILE_ATTR_READ,
        .f.buff             = (uint8_t *)__roboto_regular_ttf,
    },
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/

#if     __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

vk_fakefat32_file_t fakefat32_root[ 3
                                +   (APP_USE_VSFVM_DEMO == ENABLED ? 1 : 0)
                                +   (USRAPP_FAKEFAT32_CFG_FONT == ENABLED ? 1 : 0)
                                +   (APP_USE_EVM_DEMO == ENABLED ? 1 : 0)
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
        .callback.fn_read   = (vsf_peda_evthandler_t)vsf_peda_func(__usrapp_on_file_read),
        .callback.fn_write  = (vsf_peda_evthandler_t)vsf_peda_func(__usrapp_on_file_write),
    },
#if APP_USE_VSFVM_DEMO == ENABLED
    {
        .name               = "test.dart",
        .size               = sizeof(__test_dart),
        .attr               = VSF_FILE_ATTR_READ,
        .f.buff             = (uint8_t *)__test_dart,
    },
#endif
#if USRAPP_FAKEFAT32_CFG_FONT == ENABLED
    {
        .name               = "font",
        .attr               = VSF_FILE_ATTR_DIRECTORY,
        .d.child            = (vk_memfs_file_t *)__fakefat32_font,
        .d.child_num        = dimof(__fakefat32_font),
    },
#endif
#if APP_USE_EVM_DEMO == ENABLED
    {
        .name               = "evm",
        .attr               = VSF_FILE_ATTR_DIRECTORY,
        .d.child            = (vk_memfs_file_t *)__evm_root,
        .d.child_num        = dimof(__evm_root),
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
