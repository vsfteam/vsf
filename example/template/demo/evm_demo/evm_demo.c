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

#include "vsf.h"

#if APP_USE_EVM_DEMO == ENABLED && APP_USE_LINUX_DEMO == ENABLED

#include "evm_module.h"

/*============================ MACROS ========================================*/

#ifndef EVM_ROOT_PATH
#   define EVM_ROOT_PATH            "/memfs/evm"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

evm_err_t vm_generate_path(evm_t *e, char *pathbuf, int pathbuf_size, char *path, int type)
{
    const char *format = (type == EVM_LOAD_MAIN) ? "%s/%s" : "%s/modules/%s";
    int len = snprintf(pathbuf, pathbuf_size - 1, format, EVM_ROOT_PATH, path);
    pathbuf[len] = '\0';
    return ec_ok;
}

int evm_demo_main(int argc, char *argv[])
{
    extern int evm_main(void);
    return evm_main();
}

#endif
