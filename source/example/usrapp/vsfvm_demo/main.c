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

#if VSF_USE_USB_HOST == ENABLED || VSFVM_CFG_COMPILER_EN == ENABLED
struct usrapp_const_t {
#   if VSF_USE_USB_HOST == ENABLED
    struct {
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        vsf_ohci_param_t ohci_param;
#       endif
    } usbh;
#   endif

#   if VSFVM_CFG_COMPILER_EN == ENABLED
    struct {
        char *dart_code;
    } vm;
#   endif
};
typedef struct usrapp_const_t usrapp_const_t;
#endif

struct usrapp_t {
#if VSF_USE_USB_HOST == ENABLED
    struct {
        vsf_usbh_t host;
#   if VSF_USE_USB_HOST_HUB == ENABLED
        vsf_usbh_class_t hub;
#   endif
#   if VSF_USE_USB_HOST_LIBUSB == ENABLED
        vsf_usbh_class_t libusb;
#   endif
    } usbh;
#endif

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
    0,
};
#endif

#if VSF_USE_USB_HOST == ENABLED || VSFVM_CFG_COMPILER_EN == ENABLED
static const usrapp_const_t usrapp_const = {
#   if VSF_USE_USB_HOST == ENABLED
#       if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
    .usbh.ohci_param        = {
        .op                 = &VSF_USB_HC0_IP,
        .priority           = vsf_arch_prio_0,
    },
#       endif
#   endif

#   if VSFVM_CFG_COMPILER_EN == ENABLED
    .vm.dart_code           = "\
print(\'vsfvm demo started...\\r\\n\');\r\n"
#       if VSF_USE_USB_HOST == ENABLED
"\
libusb_on_evt(evt, libusb_dev dev) {\r\n\
    if (USB_EVT_ON_ARRIVED == evt) {\r\n\
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
#if VSF_USE_USB_HOST == ENABLED
    .usbh                   = {
#   if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
        .host.drv           = &vsf_ohci_drv,
        .host.param         = (void *)&usrapp_const.usbh.ohci_param,
#   endif
#   if VSF_USE_USB_HOST_HUB == ENABLED
        .hub.drv            = &vsf_usbh_hub_drv,
#   endif
#   if VSF_USE_USB_HOST_LIBUSB == ENABLED
        .libusb.drv         = &vsf_usbh_libusb_drv,
#   endif
    },
#endif

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

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsfvm_bytecode_t vsfvm_get_bytecode_imp(const void *token, uint_fast32_t *pc)
{
    if (*pc < usrapp.vm.bytecode_num) {
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
        vsfvm_bytecode_t token = usrapp.vm.bytecode[(*pc)++];
        vsf_trace(VSF_TRACE_DEBUG, "%d:", *pc - 1);
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

int main(void)
{
    vsf_trace_init(NULL);
#if USRAPP_CFG_STDIO_EN == ENABLED
    vsf_stdio_init();
#endif

#if VSF_USE_USB_HOST == ENABLED
#   if VSF_USE_USB_HOST_HCD_OHCI == ENABLED
    vsf_ohci_init();
#   endif
    vsf_usbh_init(&usrapp.usbh.host);
#   if VSF_USE_USB_HOST_LIBUSB == ENABLED
    vsf_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.libusb);
#   endif
#   if VSF_USE_USB_HOST_HUB == ENABLED
    vsf_usbh_register_class(&usrapp.usbh.host, &usrapp.usbh.hub);
#   endif
#endif

#if VSFVM_CFG_COMPILER_EN == ENABLED
    vsfvm_compiler_t *compiler = &usrapp.vm.compiler;
    char *code = usrapp_const.vm.dart_code;
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
        usrapp.vm.bytecode_num = compiler->bytecode_pos + 1;
        vsfvm_objdump(usrapp.vm.bytecode, usrapp.vm.bytecode_num);

        // 3. compile succeed, start runtime
        vsfvm_runtime_init(runtime);
        vsfvm_runtime_script_init(runtime, script);
    }
#else
    script->token = &usrapp.vm.bytecode;
    vsfvm_runtime_init(runtime);
    vsfvm_runtime_script_init(runtime, script);
#endif
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
