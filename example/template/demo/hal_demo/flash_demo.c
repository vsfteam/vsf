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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_FLASH_DEMO == ENABLED && VSF_HAL_USE_FLASH == ENABLED

/*============================ MACROS ========================================*/

#ifdef APP_FLASH_DEMO_CFG_FLASH_PREFIX
#   undef VSF_FLASH_CFG_PREFIX
#   define VSF_FLASH_CFG_PREFIX                         APP_FLASH_DEMO_CFG_FLASH_PREFIX
#endif

#ifndef APP_FLASH_DEMO_CFG_FLASH
#   define APP_FLASH_DEMO_CFG_FLASH                     (vsf_flash_t *)&vsf_hw_flash0
#endif

#ifndef APP_FLASH_DEMO_CFG_ADDRESS
#   define APP_FLASH_DEMO_CFG_ADDRESS                   0x081FBFFF
#endif

#ifndef APP_FLASH_DEMO_CFG_SIZE
#   define APP_FLASH_DEMO_CFG_SIZE                      512
#endif

/*============================ IMPLEMENTATION ================================*/

static void __flash_demo(void)
{
    vsf_err_t result;

    uint8_t read_buffer[APP_FLASH_DEMO_CFG_SIZE];

    result = vsf_flash_init(APP_FLASH_DEMO_CFG_FLASH, NULL);
    VSF_ASSERT(result == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_flash_enable(APP_FLASH_DEMO_CFG_FLASH));

    result = vsf_flash_erase(APP_FLASH_DEMO_CFG_FLASH, APP_FLASH_DEMO_CFG_ADDRESS, sizeof(read_buffer));
    VSF_ASSERT(result == VSF_ERR_NONE);

    for (int i = 0; i < dimof(read_buffer); i++) {
        read_buffer[i] = i;
    }
    result = vsf_flash_write(APP_FLASH_DEMO_CFG_FLASH, APP_FLASH_DEMO_CFG_ADDRESS, read_buffer, sizeof(read_buffer));
    VSF_ASSERT(result == VSF_ERR_NONE);

    memset(read_buffer, 0x00, sizeof(read_buffer));
    result = vsf_flash_read(APP_FLASH_DEMO_CFG_FLASH, APP_FLASH_DEMO_CFG_ADDRESS, read_buffer, sizeof(read_buffer));
    VSF_ASSERT(result == VSF_ERR_NONE);

    for (int i = 0; i < dimof(read_buffer); i++) {
        if (read_buffer[i] != i) {
            vsf_trace_debug("veriy flash erase/write/read faild" VSF_TRACE_CFG_LINEEND);
            return ;
        }
    }

    vsf_trace_debug("veriy flash erase/write/read pass" VSF_TRACE_CFG_LINEEND);
}

#if APP_USE_LINUX_DEMO == ENABLED
int flash_main(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#   endif
#   if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#   endif
#endif

    __flash_demo();

    return 0;
}

#endif
