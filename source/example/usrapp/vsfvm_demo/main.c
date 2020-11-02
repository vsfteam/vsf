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
#include "../usrapp_common.h"

#include "component/3rd-party/vsfvm/raw/vsf_vm.h"
#include "component/3rd-party/vsfvm/extension/vsf/kernel/vsfvm_ext_kernel.h"
#include "component/3rd-party/vsfvm/extension/vsf/libusb/vsfvm_ext_libusb.h"

/*============================ MACROS ========================================*/

#ifndef VSFVM_CFG_RUNTIME_EN
#   error vsfvm demo need VSFVM_CFG_RUNTIME_EN
#endif

#ifndef USRAPP_CFG_VM_BYTECODE_MAX_NUMBER
#   define USRAPP_CFG_VM_BYTECODE_MAX_NUMBER            1024
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSFVM_CFG_COMPILER_EN == ENABLED
struct usrapp_const_t {
#   if VSFVM_CFG_COMPILER_EN == ENABLED
    struct {
        char *dart_code;
    } vm;
#   endif
};
typedef struct usrapp_const_t usrapp_const_t;
#endif

struct usrapp_t {
    struct {
        vsfvm_runtime_t runtime;
        vsfvm_runtime_script_t script;

#if VSFVM_CFG_COMPILER_EN == ENABLED
        vsfvm_compiler_t compiler;
        vsfvm_lexer_list_t lexer_dart;
        vsfvm_bytecode_t bytecode[USRAPP_CFG_VM_BYTECODE_MAX_NUMBER];
#else
        const vsfvm_bytecode_t *bytecode;
#endif
        uint32_t bytecode_num;
    } vm;
};
typedef struct usrapp_t usrapp_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if VSFVM_CFG_COMPILER_EN != ENABLED
static const vsfvm_bytecode_t vsfvm_bytecode[] = {
    0x63000003, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_RESOURCES, VSFVM_CODE_VARIABLE_POS_LOCAL, 3),
    0x81010000, //VSFVM_FUNCTION(VSFVM_CODE_FUNCTION_EXT, 1, 0),
    0x19000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0),
    0x21000007, //VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_goto, 0, 7),
    0x63000018, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_RESOURCES, VSFVM_CODE_VARIABLE_POS_LOCAL, 24),
    0x76667376, //VSFVM_VARIABLE(unknown variable type, unknown variable position, 29558),
    0x6564206D, //VSFVM_VARIABLE(unknown variable type, unknown variable position, 8301),
    0x73206F6D, //VSFVM_VARIABLE(unknown variable type, unknown variable position, 28525),
    0x74726174, //VSFVM_VARIABLE(unknown variable type, unknown variable position, 24948),
    0x2E2E6465, //unknown keyword: 14
    0x000A0D2E, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_NOT, 10, 3374),
    0x20000000, //VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_var, 0, 0),
    0x61000000, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_REFERENCE, VSFVM_CODE_VARIABLE_POS_LOCAL, 0),
    0x40000000, //VSFVM_NUMBER(0),
    0x18000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ASSIGN, 0, 0),
    0x19000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0),
    0x20000000, //VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_var, 0, 0),
    0x61020000, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_REFERENCE, VSFVM_CODE_VARIABLE_POS_STACK_END, 0),
    0x40000001, //VSFVM_NUMBER(1),
    0x18000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ASSIGN, 0, 0),
    0x19000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0),
    0x22000013, //VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_if, 0, 19),
    0x63000007, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_RESOURCES, VSFVM_CODE_VARIABLE_POS_LOCAL, 7),
    0x60000000, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_NORMAL, VSFVM_CODE_VARIABLE_POS_LOCAL, 0),
    0x63000003, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_RESOURCES, VSFVM_CODE_VARIABLE_POS_LOCAL, 3),
    0x81030000, //VSFVM_FUNCTION(VSFVM_CODE_FUNCTION_EXT, 3, 0),
    0x19000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0),
    0x21000006, //VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_goto, 0, 6),
    0x63000003, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_RESOURCES, VSFVM_CODE_VARIABLE_POS_LOCAL, 3),
    0x00000A0D, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_NOT, 0, 2573),
    0x6300000C, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_RESOURCES, VSFVM_CODE_VARIABLE_POS_LOCAL, 12),
    0x72616568, //VSFVM_VARIABLE(unknown variable type, unknown variable position, 25960),
    0x65622074, //VSFVM_VARIABLE(unknown variable type, unknown variable position, 8308),
    0x00207461, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_NOT, 32, 29793),
    0x61000000, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_REFERENCE, VSFVM_CODE_VARIABLE_POS_LOCAL, 0),
    0x60000000, //VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_NORMAL, VSFVM_CODE_VARIABLE_POS_LOCAL, 0),
    0x40000001, //VSFVM_NUMBER(1),
    0x07000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ADD, 0, 0),
    0x18000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ASSIGN, 0, 0),
    0x19000000, //VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0),
    0x2100FFE7, //VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_goto, 0, 65511),
    0x23000000, //VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_return, 0, 0),
    0x23000000, //VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_return, 0, 0),
};
#endif

#if VSFVM_CFG_COMPILER_EN == ENABLED
static const usrapp_const_t usrapp_const = {
#   if VSFVM_CFG_COMPILER_EN == ENABLED
    .vm.dart_code           = "\
print(\'vsfvm demo started...\\r\\n\');\r\n"
#       if VSF_USE_USB_HOST == ENABLED
"\
libusb_on_evt(evt, libusb_dev dev) {\r\n\
    if (USB_EVT_ON_ARRIVED == evt) {\r\n\
        print(\'libusb_dev_on_arrived: \', dev);\r\n\
        var desc = buffer_create(18);\r\n\
        var result;\r\n\
        dev.transfer(0, @result, 0x80, 0x06, 0x0100, 0x0000, 18, desc);\r\n\
        print(desc, \'\\r\\n\');\r\n\
    }\r\n\
}\r\n\
libusb_start(libusb_on_evt);\r\n\
"
#       endif
"\
var heartbeat = 0;\r\n\
while (1) {\r\n\
    delay_ms(1000);\r\n\
    print(\'heart beat \', heartbeat, \'\\r\\n\');\r\n\
    heartbeat = heartbeat + 1;\r\n\
}\r\n\
",
#   endif
};
#endif

static usrapp_t usrapp = {
#if VSFVM_CFG_COMPILER_EN == ENABLED
    .vm.lexer_dart.op       = &vsfvm_lexer_op_dart,
#else
    .vm.bytecode            = vsfvm_bytecode,
    .vm.bytecode_num        = dimof(vsfvm_bytecode),
#endif
};

#if VSFVM_CFG_COMPILER_EN == ENABLED
static const char *vsfvm_errcode_str[] = {
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
#endif

#ifdef __CPU_MCS51__
#   if VSF_USE_TRACE == ENABLED
static uint_fast32_t vsf_stdio_console_stream_write(
        vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t vsf_stdio_console_stream_get_data_length(vsf_stream_t *stream);

static const vsf_stream_op_t vsf_stdio_console_stream_op = {
    .get_data_length    = vsf_stdio_console_stream_get_data_length,
    .write              = vsf_stdio_console_stream_write,
};

vsf_stream_t VSF_DEBUG_STREAM_TX = {
    .op = &vsf_stdio_console_stream_op,
};
#   endif
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#ifdef __CPU_MCS51__
#   if VSF_USE_TRACE == ENABLED
#include <stdio.h>
static uint_fast32_t vsf_stdio_console_stream_write(
        vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    return printf("%s", buf);
}

static uint_fast32_t vsf_stdio_console_stream_get_data_length(vsf_stream_t *stream)
{
    return 0;
}
#   endif
#endif

vsfvm_bytecode_t vsfvm_get_bytecode_imp(const void *token, uint_fast32_t *pc)
{
    if (*pc < usrapp.vm.bytecode_num) {
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
        vsfvm_bytecode_t token = usrapp.vm.bytecode[(*pc)++];
        vsf_trace_debug("%d:", *pc - 1);
        vsfvm_tkdump(token);
        return token;
#else
        return usrapp.vm.bytecode[(*pc)++];
#endif
    }
    return VSFVM_CODE(VSFVM_CODE_TYPE_EOF, 0);
}

int_fast32_t vsfvm_get_res_imp(const void *token, uint_fast32_t offset, uint8_t **buffer)
{
    int_fast32_t size = -1;
    if (offset < usrapp.vm.bytecode_num) {
        size = usrapp.vm.bytecode[offset] & 0xFFFF;
        *buffer = (uint8_t *)&usrapp.vm.bytecode[offset + 1];
    }
    return size;
}

#if VSFVM_CFG_COMPILER_EN == ENABLED
int vsfvm_set_bytecode_imp(vsfvm_compiler_t *compiler, vsfvm_bytecode_t code, uint_fast32_t pos)
{
    if (pos >= dimof(usrapp.vm.bytecode)) {
        return -1;
    }

    usrapp.vm.bytecode[pos] = code;
    return 0;
}
#endif

#ifdef __WIN__
#include <stdio.h>

// TODO: SDL require that main need argc and argv
int main(int argc, char *argv[])
{
#   if VSFVM_CFG_COMPILER_EN == ENABLED
    if (argc > 3) {
        fprintf(stderr, "invalid command line" VSF_TRACE_CFG_LINEEND);
    print_info_and_exit:
        printf("format: %s [script_file] [output_file]" VSF_TRACE_CFG_LINEEND, argv[0]);
    exit:
        exit(-1);
    }

    FILE *fp;
    long size;

    char *code = usrapp_const.vm.dart_code;
    char *output = NULL;
    if (argc > 1) {
        fp = fopen(argv[1], "rb");

        if (NULL == fp) {
            fprintf(stderr, "fail to open script file: %s" VSF_TRACE_CFG_LINEEND, argv[1]);
            goto print_info_and_exit;
        }

        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        code = malloc(size + 1);
        if (NULL == code) {
            fprintf(stderr, "not enough resources" VSF_TRACE_CFG_LINEEND);
        close_and_exit:
            fclose(fp);
            goto exit;
        }

        if (size != fread(code, 1, size, fp)) {
            fprintf(stderr, "fail to read from file: %s, errcode: %d" VSF_TRACE_CFG_LINEEND, argv[1], ferror(fp));
            goto close_and_exit;
        }
        code[size] = '\0';
    }
    if (argc > 2) {
        output = argv[2];
    }
#   endif
#else
int main(void)
{
#   if VSFVM_CFG_COMPILER_EN == ENABLED
#       ifdef USRAPP_CFG_SCRIPT_ADDR
    char *code = (*(char *)USRAPP_CFG_SCRIPT_ADDR == 0xFF) ?
                usrapp_const.vm.dart_code : (char *)USRAPP_CFG_SCRIPT_ADDR;
#       else
    char *code = usrapp_const.vm.dart_code;
#       endif
#   endif
#endif

    __usrapp_common_init();

#if VSFVM_CFG_COMPILER_EN == ENABLED
    vsfvm_compiler_t *compiler = &usrapp.vm.compiler;
    int err;
#endif

    vsfvm_runtime_t *runtime = &usrapp.vm.runtime;
    vsfvm_runtime_script_t *script = &usrapp.vm.script;

    // 1. register extension(s)
    vsfvm_ext_register_std();
    vsfvm_ext_register_kernel();
#if VSF_USE_USB_HOST == ENABLED
    vsfvm_ext_register_libusb();
#endif

#if VSFVM_CFG_COMPILER_EN == ENABLED
    vsf_trace_string(VSF_TRACE_INFO, "script code:" VSF_TRACE_CFG_LINEEND);
    vsf_trace_string(VSF_TRACE_INFO, code);
    vsf_trace_string(VSF_TRACE_INFO, VSF_TRACE_CFG_LINEEND);

    // 2. compile script
    vsfvm_compiler_init(compiler);
    vsfvm_compiler_register_lexer(compiler, &usrapp.vm.lexer_dart);
    vsfvm_compiler_set_script(compiler, "shell.dart");
    err = vsfvm_compiler_input(compiler, code);
    if (err < 0) {
        err = -err;
        vsf_trace(VSF_TRACE_ERROR, "command line compile error: %s" VSF_TRACE_CFG_LINEEND,
            (err >= VSFVM_ERRCODE_END) ? "unknwon error" : vsfvm_errcode_str[err]);
        vsf_trace(VSF_TRACE_ERROR, "compile error around line %d column %d" VSF_TRACE_CFG_LINEEND,
            compiler->script.lexer.curctx.line + 1, compiler->script.lexer.curctx.col + 1);
        vsfvm_compiler_fini(compiler);
    } else if (err > 0) {
        vsf_trace(VSF_TRACE_ERROR, "invalid environment code" VSF_TRACE_CFG_LINEEND);
    } else {
        const char eof = 0xFF;
        vsfvm_compiler_input(compiler, &eof);
        script->token = &usrapp.vm.bytecode;
        usrapp.vm.bytecode_num = compiler->bytecode_pos;
        vsfvm_objdump(usrapp.vm.bytecode, usrapp.vm.bytecode_num);

#ifdef __WIN__
        if (output != NULL) {
            fp = fopen(output, "wb");
            if (NULL == fp) {
                fprintf(stderr, "fail to open output file: %s" VSF_TRACE_CFG_LINEEND, output);
                goto print_info_and_exit;
            }

            size = sizeof(vsfvm_bytecode_t) * usrapp.vm.bytecode_num;
            if (size != fwrite(usrapp.vm.bytecode, 1, size, fp)) {
                fprintf(stderr, "fail to write to file: %s, errcode: %d" VSF_TRACE_CFG_LINEEND, output, ferror(fp));
                goto close_and_exit;
            }
            fclose(fp);
        }
#endif

        // 3. compile succeed, start runtime
        vsfvm_runtime_init(runtime);
        vsfvm_runtime_script_init(runtime, script);
    }
#else
#   ifdef USRAPP_CFG_BYTECODE_ADDR
    if (*(uint32_t *)USRAPP_CFG_BYTECODE_ADDR != 0xFFFFFFFF) {
        usrapp.vm.bytecode = (vsfvm_bytecode_t *)USRAPP_CFG_BYTECODE_ADDR;
        usrapp.vm.bytecode_num = (uint32_t)-1;
    }
#   endif

    script->token = &usrapp.vm.bytecode;
    vsfvm_runtime_init(runtime);
    vsfvm_runtime_script_init(runtime, script);
#endif
    return 0;
}

void vsf_plug_in_on_kernel_idle(void)
{
    vsfvm_runtime_t *runtime = &usrapp.vm.runtime;
    int err;

    // 4. poll if thread is pending
    do {
        err = vsfvm_runtime_poll(runtime);
        if (err < 0) {
            vsf_trace(VSF_TRACE_ERROR, "vsfvm_runtime_poll failed with %d\n", err);
            break;
        }
    } while (vsfvm_runtime_is_thread_pending(runtime));

    vsf_arch_sleep(0);
}

/* EOF */
