// This is is modified from evm/bsp/common/evm_main.c
//  Chinese comments are removed or transalate to english because it destroys the font in IAR

/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM is a gerneric virtual machine engine with lexer front-end, virtual machine and virtual machine extension framework.
**  Supporting js, python, qml, lua, etc; pure C language; zero dependency; REPL embedded; supporting MCU with ROM > 40KB, RAM > 2KB
**  embedded memory management with garbage collecting, complex comression algo, no memory fragmentation
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/

#include "vsf.h"

#if APP_USE_EVM_DEMO == ENABLED

#include "./evm_main.h"

#if CONFIG_EVM_ECMA
#include "ecma.h"
#endif

#if CONFIG_EVM_LANG_QML
#include "ecma.h"
#include "qml_lvgl_module.h"
#endif

#if CONFIG_EVM_LANG_PYTHON
#include "python_builtins.h"
#endif

#if CONFIG_EVM_HEATSHRINK
#include "wrap_heatshrink.h"
#endif

char evm_repl_tty_read(evm_t * e)
{
    EVM_UNUSED(e);
    return (char)getchar();
}

const char * vm_load(evm_t * e, char * path, int type)
{
    return NULL;
}

void * vm_malloc(int size)
{
    void * m = vsf_heap_malloc(size);
    if (m != NULL) {
        memset(m, 0 ,size);
    }
    return m;
}

void vm_free(void * mem)
{
    if (mem != NULL) {
        vsf_heap_free(mem);
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
int evm_demo(int argc, char *argv[])
{
#else
int VSF_USER_ENTRY(void)
{
#   if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
#       if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#       endif
#   endif
#endif

    int lang_type = EVM_LANG_JS;

    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);

    evm_t * env = (evm_t*)vsf_heap_malloc(sizeof(evm_t));
    memset(env, 0, sizeof(evm_t));
    int err = evm_init(env, EVM_HEAP_SIZE, EVM_STACK_SIZE, EVM_MODULE_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);
    if ( err ) {
        evm_print("Failed to initialize evm\r\n");
        return err;
    }
    err = evm_module(env);
    if ( err ) {
        evm_print("Failed to add evm module\r\n");
        return err;
    }
#if CONFIG_EVM_HEATSHRINK
    err = heatshrink_module(env);
    if ( err ) {
        evm_print("Failed to add evm module\r\n");
        return err;
    }
#endif

#if CONFIG_EVM_LANG_PYTHON
    python_builtins(env);
    if ( err ) {
        evm_print("Failed to add python builtins module\r\n");
    }
    lang_type = EVM_LANG_PY;
#endif

#if CONFIG_EVM_ECMA
    err = ecma_module(env);
    if ( err ) {
        evm_print("Failed to add ecma module\r\n");
    }
#endif

    err = evm_repl_run(env, 20, lang_type);
    while ( err == ec_exit ){
        evm_poll_callbacks(env);
    }
    return err;
}

#endif
