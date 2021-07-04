// vsf configurations will be check, so include vsf.h here
//  if user want to remove dependency on vsf, remove modules in evm_module_init,
//  and include stdlib.h/stdio.h/string.h
#include "vsf.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

#if VSF_USE_EVM == ENABLED

#include "evm_module.h"
#include "ecma.h"

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
