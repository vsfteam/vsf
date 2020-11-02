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

#include <unistd.h>
#include <stdio.h>

#if APP_USE_VSFVM_DEMO == ENABLED

#include "vsf_vm.h"

#include "component/3rd-party/vsfvm/extension/vsf/kernel/vsfvm_ext_kernel.h"
#include "component/3rd-party/vsfvm/extension/vsf/libusb/vsfvm_ext_libusb.h"

/*============================ MACROS ========================================*/

#if APP_USE_LINUX_DEMO != ENABLED
#   error THIS VSFVM_DEMO NEED LINUX
#endif

#ifndef APP_CFG_VM_BYTECODE_MAX_NUMBER
#   define APP_CFG_VM_BYTECODE_MAX_NUMBER               (1 * 1024 * 1024)
#endif

#ifndef APP_CFG_VM_SHELL_PROMPT
#   define APP_CFG_VM_SHELL_PROMPT                      ">>>"
#endif

#ifndef APP_CFG_VM_LINE_BUFFER_SIZE
#   define APP_CFG_VM_LINE_BUFFER_SIZE                  4096
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct usrapp_vm_t {
    bool is_inited;

    vsfvm_runtime_t runtime;
    vsfvm_runtime_script_t script;

    vsfvm_compiler_t compiler;
    vsfvm_lexer_list_t lexer_dart;
    uint32_t bytecode_num;
    vsfvm_bytecode_t bytecode[APP_CFG_VM_BYTECODE_MAX_NUMBER];
} usrapp_vm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_vm_t __usrapp_vm = {
    .is_inited          = false,
};

static const char *__vsfvm_errcode_str[] = {
    STR(VSFVM_ERRCODE_NONE),

    // common error
    STR(VSFVM_BUG),
    STR(VSFVM_BYTECODE_TOOLONG),
    STR(VSFVM_NOT_ENOUGH_RESOURCES),
    STR(VSFVM_FATAL_ERROR),
    STR(VSFVM_NOT_SUPPORT),

    // lexer error
    STR(VSFVM_LEXER_NOT_SUPPORT),
    STR(VSFVM_LEXER_INVALID_OP),
    STR(VSFVM_LEXER_INVALID_CHAR),
    STR(VSFVM_LEXER_INVALID_STRING),
    STR(VSFVM_LEXER_INVALID_ESCAPE),
    STR(VSFVM_LEXER_SYMBOL_TOO_LONG),

    // parser error
    STR(VSFVM_PARSER_UNEXPECTED_TOKEN),
    STR(VSFVM_PARSER_ALREADY_DEFINED),
    STR(VSFVM_PARSER_INVALID_CLOSURE),
    STR(VSFVM_PARSER_INVALID_EXPR),
    STR(VSFVM_PARSER_UNINITED_CONST),
    STR(VSFVM_PARSER_INVALID_CONST),
    STR(VSFVM_PARSER_DIV0),
    STR(VSFVM_PARSER_EXPECT_FUNC_PARAM),
    STR(VSFVM_PARSER_TOO_MANY_FUNC_PARAM),
    STR(VSFVM_PARSER_MEMFUNC_NOT_FOUND),

    // compiler error
    STR(VSFVM_COMPILER_INVALID_MODULE),
    STR(VSFVM_COMPILER_INVALID_FUNC),
    STR(VSFVM_COMPILER_INVALID_FUNC_PARAM),
    STR(VSFVM_COMPILER_FAIL_USRLIB),
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

// vsfvm porting
vsfvm_bytecode_t vsfvm_get_bytecode_imp(const void *token, uint_fast32_t *pc)
{
    if (*pc < __usrapp_vm.bytecode_num) {
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
        vsfvm_bytecode_t token = __usrapp_vm.bytecode[(*pc)++];
        vsf_trace(VSF_TRACE_DEBUG, "%d:", *pc - 1);
        vsfvm_tkdump(token);
        return token;
#else
        return __usrapp_vm.bytecode[(*pc)++];
#endif
    }
    return VSFVM_CODE(VSFVM_CODE_TYPE_EOF, 0);
}

int_fast32_t vsfvm_get_res_imp(const void *token, uint_fast32_t offset, uint8_t **buffer)
{
    int_fast32_t size = -1;
    if (offset < __usrapp_vm.bytecode_num) {
        size = __usrapp_vm.bytecode[offset] & 0xFFFF;
        *buffer = (uint8_t *)&__usrapp_vm.bytecode[offset + 1];
    }
    return size;
}

int vsfvm_set_bytecode_imp(vsfvm_compiler_t *compiler, vsfvm_bytecode_t code, uint_fast32_t pos)
{
    if (pos >= dimof(__usrapp_vm.bytecode)) {
        return -1;
    }

    __usrapp_vm.bytecode[pos] = code;
    return 0;
}

#if APP_USE_LINUX_DEMO == ENABLED
void vsfvm_user_poll(void) {
#else
void vsf_plug_in_on_kernel_idle(void) {
    vsf_driver_poll();
#endif
    vsfvm_runtime_t *runtime = &__usrapp_vm.runtime;
    int err;

    // 4. poll if thread is pending
    while (__usrapp_vm.is_inited && vsfvm_runtime_is_thread_pending(runtime)) {
        if ((err = vsfvm_runtime_poll(runtime)) < 0) {
            vsf_trace(VSF_TRACE_ERROR, "vsfvm_runtime_poll failed with %d\r\n", err);
            break;
        }
    }

#if APP_USE_LINUX_DEMO != ENABLED
    vsf_arch_sleep(0);
#endif
}

static int vsfvm_module_require_lib_imp(vsfvm_compiler_t *compiler, char *path)
{
    char *fbuff = NULL;
    size_t flen;
    int err = 0;

    FILE *fin = fopen(path, "rt");
    if (!fin) {
        printf("can not open file %s\r\n", path);
        return -VSFVM_COMPILER_FAIL_USRLIB;
    }

    fseek(fin, 0L, SEEK_END);
    flen = ftell(fin);
    fseek(fin, 0L, SEEK_SET);
    if (flen > 0) {
        fbuff = (char *)malloc(flen + 1);
        fread(fbuff, 1, flen, fin);
        fclose(fin);
        fin = NULL;

        fbuff[flen] = '\0';
        err = vsfvm_compiler_input(compiler, fbuff);
        free(fbuff);
        fbuff = NULL;

        if (err < 0) {
            err = -err;
            printf("command line compile error: %s\r\n",
                (err >= VSFVM_ERRCODE_END) ? "unknwon error" : __vsfvm_errcode_str[err]);
            printf("compile error around line %d column %d\r\n",
                    compiler->script.lexer.curctx.line + 1, compiler->script.lexer.curctx.col + 1);
            return -err;
        }
    } else {
        fclose(fin);
    }

    return err;
}

int vsfvm_main(int argc, char *argv[])
{
    char *path = NULL, *ext;
    int err;

    if (argc != 2) {
        printf("format: vsfvm FILE\r\n");
        return -1;
    }
    path = argv[1];

    // 1. register extension(s)
    vsfvm_ext_register_std();
    vsfvm_ext_register_kernel();
#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_LIBUSB == ENABLED
    vsfvm_ext_register_libusb();
#endif

    memset(&__usrapp_vm, 0, sizeof(__usrapp_vm));
    __usrapp_vm.lexer_dart.op = &vsfvm_lexer_op_dart;

    ext = vk_file_getfileext(path);
    if (!strcmp("dart", ext)) {
        vsfvm_compiler_t *compiler = &__usrapp_vm.compiler;

        // 2. compile script
        vsfvm_compiler_init(compiler);
        vsfvm_compiler_register_lexer(compiler, &__usrapp_vm.lexer_dart);
        vsfvm_compiler_set_script(compiler, path);

        err = vsfvm_module_require_lib_imp(compiler, path);
        if (!err) {
            err = vsfvm_compiler_input(compiler, "\xFF");
            if (err < 0) {
                err = -err;
                printf("command line compile error: %s\r\n",
                    (err >= VSFVM_ERRCODE_END) ? "unknwon error" : __vsfvm_errcode_str[err]);
                printf("compile error around line %d column %d\r\n",
                    compiler->script.lexer.curctx.line + 1, compiler->script.lexer.curctx.col + 1);
            }
        }
        vsfvm_compiler_fini(compiler);

        if (!err) {
            __usrapp_vm.bytecode_num = compiler->bytecode_pos;
            printf("objdump:\r\n");
            vsfvm_objdump(__usrapp_vm.bytecode, __usrapp_vm.bytecode_num);
        }
    } else {
        printf("%s not supported.\r\n", path);
        return -1;
    }

    if (!err) {
        // run
        printf("start runtime\r\n");

        // 3. compile succeed, start runtime
        __usrapp_vm.script.token = &__usrapp_vm.bytecode;
        vsfvm_runtime_init(&__usrapp_vm.runtime);
        vsfvm_runtime_script_init(&__usrapp_vm.runtime, &__usrapp_vm.script);
        __usrapp_vm.is_inited = true;
    }

    return 0;
}

#endif      // APP_USE_VSFVM_DEMO
