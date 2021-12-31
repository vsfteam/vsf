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

#if VSF_USE_EVM == ENABLED

#include "evm_module.h"
#include "ecma.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_EVM_MODULE_INIT
// evm_module_init here will try to add all modules supported by vsf,
//  re-write it if necessary
WEAK(evm_module_init)
evm_err_t evm_module_init(evm_t *env)
{
    evm_err_t err = ec_ok;

#if VSF_EVM_USE_USBH == ENABLED && VSF_USE_LINUX == ENABLED
    extern evm_err_t evm_module_usbh(evm_t *e);
    err = evm_module_usbh(env);
    if (err != ec_ok) {
        evm_print("Failed to create usbh module\r\n");
        return err;
    }
#endif

    return err;
}
#endif

#ifndef WEAK_EVM_MAIN
WEAK(evm_main)
int evm_main(void)
{
    extern evm_t * evm_port_init(void);
    evm_t *env = evm_port_init();

    evm_err_t err = ecma_module(env);
    if (err != ec_ok) {
        evm_print("Failed to create ecma module\r\n");
        return err;
    }

    err = evm_module_init(env);
    if (err != ec_ok) {
        return err;
    }

#ifdef EVM_LANG_ENABLE_REPL
    evm_repl_run(env, 1000, EVM_LANG_JS);
#endif

    err = evm_boot(env, "main.js");
    if (err == ec_no_file) {
        evm_print("can't open file\r\n");
        return err;
    }

    return evm_start(env);
}
#endif

#endif      // VSF_USE_EVM
