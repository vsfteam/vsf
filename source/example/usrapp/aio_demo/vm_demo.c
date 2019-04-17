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

#define VSFSTREAM_CLASS_INHERIT
#define VSFVM_RUNTIME_INHERIT
#include "vsf.h"

/*============================ MACROS ========================================*/

#define VSFVM_SHELL_PROMPT_ENTRY        ">>>"
#define VSFVM_SHELL_PROMPT_CODE         "..."

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct usrapp_vm_t {

    vsfvm_runtime_t runtime;
    vsfvm_runtime_script_t script;

    vsfvm_compiler_t compiler;
    vsfvm_lexer_list_t lexer_dart;

    vsfvm_snapshot_t snapshot;

    bool wait_prompt;
    bool new_line;
    vsf_stream_t *stream_in;
    vsf_stream_t *stream_out;
    vsfvm_bytecode_t bytecode[4 * 1024];
    uint32_t bytecode_pos;
    uint32_t bytecode_num;
    uint32_t linepos;
    char linebuf[1024];
};
typedef struct usrapp_vm_t usrapp_vm_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static usrapp_vm_t usrapp_vm    = {
    .lexer_dart.op              = &vsfvm_lexer_op_dart,
};

static const char *vsfvm_errcode_str[] =
{
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

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

vsfvm_thread_t * vsfvm_alloc_thread_imp(vsfvm_runtime_t *runtime)
{
    return (vsfvm_thread_t *)vsf_heap_malloc(sizeof(vsfvm_thread_t));
}

void vsfvm_free_thread_imp(vsfvm_runtime_t *runtime, vsfvm_thread_t *thread)
{
    vsf_heap_free(thread);
}

vsfvm_bytecode_t vsfvm_get_bytecode_imp(void *token, uint_fast32_t *pc)
{
    if (*pc < usrapp_vm.bytecode_num) {
#if VSFVM_RUNTIME_DEBUG_EN == ENABLED
        vsfvm_bytecode_t token = usrapp_vm.bytecode[(*pc)++];
        vsf_trace(VSF_TRACE_DEBUG, "%d:", *pc - 1);
        vsfvm_tkdump(token);
        return token;
#else
        return usrapp_vm.bytecode[(*pc)++];
#endif
    }
    return VSFVM_CODE(VSFVM_CODE_TYPE_EOF, 0);
}

int_fast32_t vsfvm_get_res_imp(void *token, uint_fast32_t offset, uint8_t **buffer)
{
    int_fast32_t size = -1;
    if (offset < usrapp_vm.bytecode_num) {
        size = usrapp_vm.bytecode[offset] & 0xFFFF;
        *buffer = (uint8_t *)&usrapp_vm.bytecode[offset + 1];
    }
    return size;
}

int vsfvm_set_bytecode_imp(vsfvm_compiler_t *compiler, vsfvm_bytecode_t code, uint_fast32_t pos)
{
    if (pos >= dimof(usrapp_vm.bytecode)) {
        return -1;
    }

    usrapp_vm.bytecode[pos] = code;
    return 0;
}

char * vsfvm_module_get_lexer_imp(const char *path)
{
    return "dart";
}

static void usrapp_vm_shell_evthandler(void *param, vsf_stream_evt_t evt)
{
    if (evt != VSF_STREAM_ON_RX) {
        return;
    }

    vsf_stream_t *stream = usrapp_vm.stream_in;
    char ch, *line = usrapp_vm.linebuf;

    vsfvm_compiler_t *compiler = &usrapp_vm.compiler;
    vsfvm_runtime_t *runtime = &usrapp_vm.runtime;
    vsfvm_runtime_script_t *script = &usrapp_vm.script;

    bool newline = false;
    vsfvm_bytecode_t *tkbuff = usrapp_vm.bytecode;
    uint_fast32_t tkpos;
    int err;

    while (vsf_stream_read(stream, (uint8_t *)&ch, 1) == 1) {
        if (usrapp_vm.linepos >= sizeof(usrapp_vm.linebuf) - 1) {
            vsf_trace(VSF_TRACE_ERROR, VSF_TRACE_CFG_LINEEND "line too long" VSF_TRACE_CFG_LINEEND);
            return;
        }

        switch (ch) {
        case '\x7F':        // ^? <DEL> for VT100
        case '\b':          // ^H <BS> for VT100
            if (usrapp_vm.linepos) {
                usrapp_vm.linepos--;
                vsf_stream_write(usrapp_vm.stream_out, "\b \b", sizeof("\b \b") - 1);
            }
            break;
        case '\r':
            vsf_stream_write(usrapp_vm.stream_out, VSF_TRACE_CFG_LINEEND, sizeof(VSF_TRACE_CFG_LINEEND) - 1);
            goto got_char;
        default:
            vsf_stream_write(usrapp_vm.stream_out, (uint8_t *)&ch, 1);
        got_char:
            line[usrapp_vm.linepos++] = ch;
            if ((ch == '\r') || (ch == '\n')) {
                newline = 1;
                break;
            }
            break;
        }
    }
    if (!newline) {
        return;
    }

    line[usrapp_vm.linepos] = '\0';
    usrapp_vm.linepos = 0;
    usrapp_vm.new_line = true;

    err = vsfvm_compiler_input(compiler, line);
    if (err < 0) {
        err = -err;
        vsf_trace(VSF_TRACE_ERROR, "command line compile error: %s\n",
            (err >= VSFVM_ERRCODE_END) ? "unknwon error" :
            vsfvm_errcode_str[err]);
        if (vsfvm_snapshot_restore(compiler, &usrapp_vm.snapshot) < 0) {
            vsf_trace(VSF_TRACE_ERROR, "fail to restore snapshot.\nexiting...\n");
        exit_vm:
            vsfvm_runtime_script_fini(runtime, script);
            vsfvm_runtime_fini(runtime);
            vsfvm_compiler_fini(compiler);
            exit(0);
        }
        vsf_stream_write(usrapp_vm.stream_out, VSFVM_SHELL_PROMPT_ENTRY, sizeof(VSFVM_SHELL_PROMPT_ENTRY) - 1);
        return;
    } else if (VSF_ERR_NONE == err) {
        vsfvm_snapshot_free(&usrapp_vm.snapshot);
        if (vsfvm_snapshot_take(compiler, &usrapp_vm.snapshot) < 0) {
            vsf_trace(VSF_TRACE_ERROR, "fail to take snapshot.\nexiting...\n");
            goto exit_vm;
        }

        // 1. check if unprocessed anchor
        if (    (compiler->script.cur_func.curctx.etoken.token > 0)
            ||  (compiler->script.cur_func.ctx.sp > 0)
            ||  (compiler->script.func_stack.sp > 1)
            ||  (usrapp_vm.bytecode_pos == compiler->bytecode_pos)) {
            return;
        }

        // 2. save bytecode_pos
        tkpos = usrapp_vm.bytecode_pos = compiler->bytecode_pos;
        // 3. append breakpoint
        tkbuff[tkpos] = VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_breakpoint, 0, 0);
        // 4. wake all threads
        usrapp_vm.bytecode_num = tkpos + 1;
        __vsf_slist_foreach_unsafe(vsfvm_thread_t, thread_node, &script->thread_list) {
            vsfvm_thread_ready(_);
        }
        // 5. run till breakpoint
        usrapp_vm.wait_prompt = true;
    }
}

void usrapp_vm_start(char *bsp_script, vsf_stream_t *stream_in, vsf_stream_t *stream_out)
{
    vsfvm_compiler_t *compiler = &usrapp_vm.compiler;
    vsfvm_runtime_t *runtime = &usrapp_vm.runtime;
    vsfvm_runtime_script_t *script = &usrapp_vm.script;
    int err;

    stream_in->rx.evthandler = usrapp_vm_shell_evthandler;
    usrapp_vm.stream_in = stream_in;
    usrapp_vm.stream_out = stream_out;

    vsfvm_ext_register_std();
//  vsfvm_ext_register_vsf();

    vsf_trace_string(VSF_TRACE_INFO, "environment code:" VSF_TRACE_CFG_LINEEND);
    vsf_trace_string(VSF_TRACE_INFO, bsp_script);
    vsf_trace_string(VSF_TRACE_INFO, VSF_TRACE_CFG_LINEEND);

    vsfvm_compiler_init(compiler);
    vsfvm_compiler_register_lexer(compiler, &usrapp_vm.lexer_dart);
    vsfvm_compiler_set_script(compiler, "shell.dart");
    err = vsfvm_compiler_input(compiler, bsp_script);
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
        usrapp_vm.bytecode[compiler->bytecode_pos] = VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_breakpoint, 0, 0);
        script->token = &usrapp_vm.bytecode;
        usrapp_vm.bytecode_num = compiler->bytecode_pos + 1;

        vsfvm_runtime_init(runtime);
        vsfvm_runtime_script_init(runtime, script);

        vsfvm_snapshot_take(compiler, &usrapp_vm.snapshot);
        vsf_stream_connect_rx(usrapp_vm.stream_in);
        usrapp_vm.wait_prompt = true;
    }
}

void vsf_plug_in_on_kernel_idle(void)
{
    vsfvm_runtime_t *runtime = &usrapp_vm.runtime;
    vsfvm_runtime_script_t *script = &usrapp_vm.script;
    vsfvm_thread_t *thread;
    int err;

    vsf_slist_peek_next(vsfvm_thread_t, thread_node, &script->thread_list, thread);
    do {
        err = vsfvm_runtime_poll(runtime);
        if (err < 0) {
            vsf_trace(VSF_TRACE_ERROR, "vsfvm_runtime_poll failed with %d\n", err);
            break;
        } else if (!err) {
            if (    usrapp_vm.wait_prompt
                &&  (thread->func.type == VSFVM_CODE_FUNCTION_SCRIPT)
                &&  (thread->func.pc == (usrapp_vm.bytecode_num - 1))) {

                vsf_stream_write(usrapp_vm.stream_out, VSFVM_SHELL_PROMPT_ENTRY, sizeof(VSFVM_SHELL_PROMPT_ENTRY) - 1);
                usrapp_vm.wait_prompt = false;
                usrapp_vm.new_line = false;
            } else if (usrapp_vm.new_line) {
                vsf_stream_write(usrapp_vm.stream_out, VSFVM_SHELL_PROMPT_CODE, sizeof(VSFVM_SHELL_PROMPT_CODE) - 1);
                usrapp_vm.new_line = false;
            }
        }
    } while (vsfvm_runtime_is_thread_pending(runtime));

    vsf_arch_sleep(0);
}
