// This is is modified from evm/modules/evm/zephyr/evm_module.c
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

#include "evm_module.h"
#include "vsf.h"

#ifndef CONFIG_EVM_MODULE_CALLBACK_SIZE
    #define CONFIG_EVM_MODULE_CALLBACK_SIZE     16
#endif

static vsf_sem_t evm_callback_sem;
static intptr_t cb_map[CONFIG_EVM_MODULE_CALLBACK_SIZE];
static intptr_t cb_handles[CONFIG_EVM_MODULE_CALLBACK_SIZE];

static evm_val_t * reference_list;
static uint32_t number_of_references;
evm_t * evm_runtime;

int evm_add_reference(evm_val_t ref){
    for (uint32_t i = 0; i < number_of_references; i++){
        evm_val_t * v = evm_list_get(evm_runtime, reference_list, i);
        if( evm_is_undefined(v) ){
            evm_list_set(evm_runtime, reference_list, i, ref);
            return i;
        }
    }
    return -1;
}

evm_val_t * evm_get_reference(int id){
    return evm_list_get(evm_runtime, reference_list, id);
}

int evm_remove_reference(int id){
    if( id >= number_of_references || id < 0) return ec_index;
    evm_list_set(evm_runtime, reference_list, id, EVM_VAL_UNDEFINED);
    return ec_ok;
}

int evm_add_callback(evm_callback_func fn, void * handle){
    for(int i = 0; i < CONFIG_EVM_MODULE_CALLBACK_SIZE; i++){
        if( cb_map[i] == 0) {
            cb_map[i] = (intptr_t)fn;
            cb_handles[i] = (intptr_t)handle;
            vsf_thread_sem_post(&evm_callback_sem);
            return i;
        }
    }
    return -1;
}

void evm_poll_callbacks(evm_t *e){
    while(1){
        vsf_thread_sem_pend(&evm_callback_sem, -1);
        for(int i = 0; i < CONFIG_EVM_MODULE_CALLBACK_SIZE; i++) {
            evm_callback_func fn = (evm_callback_func)cb_map[i];
            if( fn == NULL) break;
            void * handle = (void*)cb_handles[i];
            fn(handle);
            cb_map[i] = 0;
        }
    }
}

static evm_val_t evm_module_delay_ms(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    vsf_thread_delay_ms(evm_2_integer(v));
    return EVM_VAL_UNDEFINED;
}

static evm_val_t evm_module_delay_us(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    vsf_thread_delay_us(evm_2_integer(v));
    return EVM_VAL_UNDEFINED;
}

int evm_module(evm_t * e){
    vsf_eda_sem_init(&evm_callback_sem, 0);
    evm_runtime = e;
    number_of_references = CONFIG_EVM_MODULE_CALLBACK_SIZE;
    reference_list = evm_list_create(e, GC_LIST, number_of_references);
    if( !reference_list ) return e->err;

    for(uint32_t i = 0; i < number_of_references; i++){
        evm_list_set(e, reference_list, i, EVM_VAL_UNDEFINED);
    }

    for(int i = 0; i < CONFIG_EVM_MODULE_CALLBACK_SIZE; i++){
        cb_map[i] = NULL;
        cb_handles[i] = NULL;
    }

    evm_builtin_t module[] = {
        {"delay_ms", evm_mk_native((intptr_t)evm_module_delay_ms)},
        {"delay_us", evm_mk_native((intptr_t)evm_module_delay_us)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "evm", module);
    return e->err;
}
