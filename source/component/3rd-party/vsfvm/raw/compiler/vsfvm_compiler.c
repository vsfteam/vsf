/*****************************************************************************
 *   Copyright(C)2009-2020 by SimonQian                                      *
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

#include "../vsf_vm_cfg.h"

#if VSFVM_CFG_COMPILER_EN == ENABLED

#include "hal/vsf_hal.h"            // for get_unaligned_cpu32
#include "service/vsf_service.h"

#include "./vsfvm_compiler.h"
#include "../common/vsfvm_objdump.h"
#include "./lexer/vsfvm_lexer.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

static void __vsfvm_compiler_script_fini(vsfvm_compiler_t *compiler);

extern int vsfvm_set_bytecode_imp(vsfvm_compiler_t *compiler, vsfvm_bytecode_t code, uint_fast32_t offset);
extern char * vsfvm_module_get_lexer_imp(const char *path);
extern int vsfvm_module_require_lib_imp(vsfvm_compiler_t *compiler, const char *path);

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSFVM_SET_BYTECODE_IMP

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

WEAK(vsfvm_set_bytecode_imp)
int vsfvm_set_bytecode_imp(vsfvm_compiler_t *compiler, vsfvm_bytecode_t code, uint_fast32_t offset)
{
    VSFVM_ASSERT(false);
    return -VSFVM_BUG;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

#endif

#ifndef WEAK_VSFVM_MODULE_GET_LEXER_IMP
WEAK(vsfvm_module_get_lexer_imp)
char * vsfvm_module_get_lexer_imp(const char *path)
{
    // usr dart lexar by default
    return "dart";
}
#endif

#ifndef WEAK_VSFVM_MODULE_REQUIRE_LIB_IMP

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

WEAK(vsfvm_module_require_lib_imp)
int vsfvm_module_require_lib_imp(vsfvm_compiler_t *compiler, const char *path)
{
    VSFVM_ASSERT(false);
    return -VSFVM_NOT_SUPPORT;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

#endif


static vsfvm_compiler_func_t * __vsfvm_get_rootfunc(vsfvm_compiler_script_t *script)
{
    if (script->func_stack.sp) {
        return vsf_dynarr_get(&script->func_stack.use_as__vsf_dynarr_t, 1);
    } else {
        return &script->cur_func;
    }
}

static int __vsfvm_push_bytecode(vsfvm_compiler_t *compiler, vsfvm_bytecode_t code)
{
#if VSFVM_COMPILER_DEBUG_EN == ENABLED
    vsf_trace_debug("%d:", compiler->bytecode_pos);
    vsfvm_tkdump(code);
#endif
    return vsfvm_set_bytecode_imp(compiler, code, compiler->bytecode_pos++);
}

static int __vsfvm_add_res(vsfvm_compiler_func_t *func, vsfvm_linktbl_t *linktbl)
{
    return vsf_dynstack_push(&func->linktbl, linktbl, 1);
}

static int __vsfvm_compiler_get_var(vsfvm_compiler_t *compiler,
    vsfvm_compiler_script_t *script, vsfvm_compiler_func_t *func,
    vsfvm_lexer_sym_t *sym, uint8_t *pos, uint16_t *idx)
{
    vsfvm_compiler_func_t *rootfunc = __vsfvm_get_rootfunc(script);
    int varidx;

    if (sym->type == VSFVM_LEXER_SYM_EXTVAR) {
        vsfvm_ext_t *ext = NULL;
        __vsf_slist_foreach_unsafe(vsfvm_ext_t, ext_node, &vsfvm_ext_list) {
            if (_->op == sym->ext_op) {
                ext = _;
                break;
            }
        }
        if (NULL == ext) {
            return VSFVM_BUG;
        }
        *pos = VSFVM_CODE_VARIABLE_POS_EXT;
        *idx = ext->var_id + sym->var.pos;
        return 0;
    }

    // if rootfunc is NULL, then current func is root
    //    variable in root stack is local variable
    if (!rootfunc) {
        rootfunc = func;
        goto local_var;
    }

    varidx = vsf_slist_get_index(vsfvm_lexer_sym_t, symbol_node, &func->varlist, sym);
    if (varidx >= 0) {
        *pos = VSFVM_CODE_VARIABLE_POS_FUNCAUTO;
        *idx = varidx;
        return 0;
    }

    varidx = vsf_slist_get_index(vsfvm_lexer_sym_t, symbol_node, &func->arglist, sym);
    if (varidx >= 0) {
        *pos = VSFVM_CODE_VARIABLE_POS_FUNCARG;
        *idx = varidx;
        return 0;
    }

local_var:
    varidx = vsf_slist_get_index(vsfvm_lexer_sym_t, symbol_node, &rootfunc->varlist, sym);
    if (varidx >= 0) {
        *pos = VSFVM_CODE_VARIABLE_POS_LOCAL;
        *idx = varidx;
        return 0;
    }
    return -VSFVM_BUG;
}

static int __vsfvm_push_res(vsfvm_compiler_t *compiler, vsfvm_compiler_func_t *func)
{
    vsfvm_linktbl_t *linktbl = vsf_dynstack_pop(&func->linktbl, 1);

    if (linktbl != NULL) {
        uint_fast32_t goto_anchor = compiler->bytecode_pos++;
        vsfvm_bytecode_t fix_code, goto_code = VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_goto, 0, 0);
        uint_fast32_t len, token_num;
        uint8_t *res;

        while (linktbl != NULL) {
            switch (linktbl->type) {
            case VSFVM_LINKTBL_STR:
                res = (uint8_t *)linktbl->sym->name;
                // string starts with "
                VSFVM_ASSERT('"' == res[0]);
                res++;
                len = strlen((const char *)res) + 1;
                token_num = ((len + 3) & ~3) >> 2;
                break;
            default:
                return -VSFVM_BUG;
            }

            fix_code = linktbl->token;
            fix_code |= (uint16_t)(compiler->bytecode_pos - linktbl->bytecode_pos - 1);
            vsfvm_set_bytecode_imp(compiler, fix_code, linktbl->bytecode_pos);

            if (__vsfvm_push_bytecode(compiler,
                    VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_RESOURCES, 0, len)) < 0) {
                return -VSFVM_BYTECODE_TOOLONG;
            }

            for (len = 0; len < token_num; len++) {
                if (__vsfvm_push_bytecode(compiler, get_unaligned_cpu32(res)) < 0) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
                res += 4;
            }

            linktbl = vsf_dynstack_pop(&func->linktbl, 1);
        }

        goto_code |= (int16_t)(compiler->bytecode_pos - goto_anchor - 1);
        vsfvm_set_bytecode_imp(compiler, goto_code, goto_anchor);
    }
    return 0;
}

static int __vsfvm_push_expr(vsfvm_compiler_t *compiler, vsf_dynstack_t *expr_stack)
{
    vsfvm_compiler_script_t *script = &compiler->script;
    vsfvm_compiler_func_t *func = &script->cur_func;

    vsfvm_linktbl_t linktbl;
    vsfvm_lexer_etoken_t *etoken;
    vsfvm_token_data_t *data;
    uint8_t arg8;
    uint16_t arg16;
    uint_fast16_t token, token_param;

    for (uint_fast32_t i = 0; i < expr_stack->sp; i++) {
        etoken = vsf_dynarr_get(&expr_stack->use_as__vsf_dynarr_t, i);
        if (!etoken) { return -VSFVM_BUG; }

        data = &etoken->data;
        token_param = etoken->token >> 16;
        token = etoken->token & 0xFFFF;
        switch (token) {
        case VSFVM_TOKEN_NUM:
            if (data->ival & (0xFFFFFFFF << VSFVM_CODE_LENGTH)) {
                if (    (__vsfvm_push_bytecode(compiler, VSFVM_NUMBER(data->ival >> (32 - VSFVM_CODE_LENGTH))) < 0)
                    ||  (__vsfvm_push_bytecode(compiler, VSFVM_NUMBER(32 - VSFVM_CODE_LENGTH)) < 0)
                    ||  (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SHL, 0, 0)) < 0)
                    ||  (__vsfvm_push_bytecode(compiler, VSFVM_NUMBER(data->ival & ((1 << (32 - VSFVM_CODE_LENGTH)) - 1))) < 0)
                    ||  (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ADD, 0, 0)) < 0)) {
                    return -VSFVM_BYTECODE_TOOLONG;
                    }
            } else if (__vsfvm_push_bytecode(compiler, VSFVM_NUMBER(data->ival)) < 0) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
            break;
        case VSFVM_TOKEN_RESOURCES:
            linktbl.bytecode_pos = compiler->bytecode_pos;
            linktbl.token = VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_RESOURCES, 0, 0);
            linktbl.sym = etoken->data.sym;
            linktbl.type = VSFVM_LINKTBL_STR;
            if (__vsfvm_add_res(func, &linktbl)) {
                return -VSFVM_NOT_ENOUGH_RESOURCES;
            }
            compiler->bytecode_pos++;
            break;
        case VSFVM_TOKEN_VAR_ID:
            if (__vsfvm_compiler_get_var(compiler, script, func, data->sym, &arg8, &arg16)) {
                return -VSFVM_PARSER_INVALID_EXPR;
            }
            if (__vsfvm_push_bytecode(compiler, VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_NORMAL, arg8, arg16)) < 0) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
            break;
        case VSFVM_TOKEN_VAR_ID_REF:
            if (__vsfvm_compiler_get_var(compiler, script, func, data->sym, &arg8, &arg16)) {
                return -VSFVM_PARSER_INVALID_EXPR;
            }
            if (__vsfvm_push_bytecode(compiler, VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_REFERENCE, arg8, arg16)) < 0) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
            break;
        case VSFVM_TOKEN_FUNC_ID:
            if (__vsfvm_push_bytecode(compiler, VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_FUNCTION,
                    data->sym->func.param_num, data->sym->func.pos - compiler->bytecode_pos - 1)) < 0) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
            break;
        case VSFVM_TOKEN_FUNC_CALL:
            if ((data->sym->func.param_num >= 0) && (data->sym->func.param_num != token_param)) {
                return -VSFVM_COMPILER_INVALID_FUNC_PARAM;
            }
            if (data->sym->type == VSFVM_LEXER_SYM_FUNCTION) {
                if (!strcmp(data->sym->name, "thread")) {
                    if (__vsfvm_push_bytecode(compiler, VSFVM_FUNCTION(VSFVM_CODE_FUNCTION_THREAD,
                            token_param, 0)) < 0) {
                        return -VSFVM_BYTECODE_TOOLONG;
                    }
                } else if (__vsfvm_push_bytecode(compiler, VSFVM_FUNCTION(VSFVM_CODE_FUNCTION_SCRIPT,
                        token_param, data->sym->func.pos - compiler->bytecode_pos - 1)) < 0) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
            } else if (data->sym->type == VSFVM_LEXER_SYM_EXTFUNC) {
                vsfvm_ext_t *ext = NULL;
                __vsf_slist_foreach_unsafe(vsfvm_ext_t, ext_node, &vsfvm_ext_list) {
                    if (_->op == data->sym->ext_op) {
                        ext = _;
                        break;
                    }
                }
                if (__vsfvm_push_bytecode(compiler, VSFVM_FUNCTION(VSFVM_CODE_FUNCTION_EXT,
                        token_param, ext->func_id + data->sym->func.pos)) < 0) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
            } else {
                return -VSFVM_COMPILER_INVALID_FUNC;
            }
            break;
        default:
            if (token > VSFVM_TOKEN_BINARY_OP) {
                int ret;
                switch (token) {
                case VSFVM_TOKEN_MUL:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_MUL, 0, 0));  break;
                case VSFVM_TOKEN_DIV:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_DIV, 0, 0));  break;
                case VSFVM_TOKEN_MOD:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_MOD, 0, 0));  break;
                case VSFVM_TOKEN_ADD:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ADD, 0, 0));  break;
                case VSFVM_TOKEN_SUB:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SUB, 0, 0));  break;
                case VSFVM_TOKEN_SHL:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SHL, 0, 0));  break;
                case VSFVM_TOKEN_SHR:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SHR, 0, 0));  break;
                case VSFVM_TOKEN_LT:       ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_LT, 0, 0));   break;
                case VSFVM_TOKEN_LE:       ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_LE, 0, 0));   break;
                case VSFVM_TOKEN_GT:       ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_GT, 0, 0));   break;
                case VSFVM_TOKEN_GE:       ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_GE, 0, 0));   break;
                case VSFVM_TOKEN_EQ:       ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_EQ, 0, 0));   break;
                case VSFVM_TOKEN_NE:       ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_NE, 0, 0));   break;
                case VSFVM_TOKEN_AND:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_AND, 0, 0));  break;
                case VSFVM_TOKEN_XOR:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_XOR, 0, 0));  break;
                case VSFVM_TOKEN_OR:       ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_OR, 0, 0));   break;
                case VSFVM_TOKEN_LAND:     ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_LAND, 0, 0)); break;
                case VSFVM_TOKEN_LOR:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_LOR, 0, 0));  break;
                case VSFVM_TOKEN_ASSIGN:   ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ASSIGN, 0, 0));break;
                case VSFVM_TOKEN_COMMA_OP: ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_COMMA, 0, 0));break;
                default:
                    return -VSFVM_NOT_SUPPORT;
                }
                if (ret < 0) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
            } else if (token > VSFVM_TOKEN_UNARY_OP) {
                int ret;
                switch (token)
                {
                case VSFVM_TOKEN_NOT:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_NOT, 0, 0));  break;
                case VSFVM_TOKEN_REV:      ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_REV, 0, 0));  break;
                case VSFVM_TOKEN_NEGA:     ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_NEGA, 0, 0)); break;
                case VSFVM_TOKEN_POSI:     ret = __vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_POSI, 0, 0)); break;
                default:
                    return -VSFVM_NOT_SUPPORT;
                }
                if (ret < 0) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
            } else {
                return -VSFVM_NOT_SUPPORT;
            }
        }
    }
    return 0;
}

static vsfvm_lexer_list_t * __vsfvm_get_lexer(vsfvm_compiler_t *compiler, const char *ext)
{
    __vsf_slist_foreach_unsafe(vsfvm_lexer_list_t, op_node, &compiler->lexer_list) {
        if (!strcmp(_->op->ext, ext)) {
            return _;
        }
    }
    return NULL;
}

static int __vsfvm_push_func(vsfvm_compiler_script_t *script, vsfvm_lexer_sym_t *sym)
{
    vsfvm_compiler_func_t *func = &script->cur_func;

    int err = vsf_dynstack_push(&script->func_stack, func, 1);
    if (err) { return err; }

    memset(func, 0, sizeof(*func));
    func->name = sym;
    func->symtbl_idx = -1;
    func->curctx.symtbl_idx = -1;

    func->ctx.item_size = sizeof(vsfvm_compiler_func_ctx_t);
    func->ctx.item_num_per_buf_bitlen = 4;
    func->ctx.buf_num_per_table_bitlen = 4;
    vsf_dynstack_init(&func->ctx);

    func->linktbl.item_size = sizeof(vsfvm_linktbl_t);
    func->linktbl.item_num_per_buf_bitlen = 4;
    func->linktbl.buf_num_per_table_bitlen = 4;
    vsf_dynstack_init(&func->linktbl);
    return 0;
}

static void __vsfvm_compiler_func_fini(vsfvm_compiler_func_t *func)
{
    vsf_dynstack_fini(&func->linktbl);
    vsf_dynstack_fini(&func->ctx);
}

#if VSFVM_PARSER_DEBUG_EN == ENABLED
static void __vsfvm_print_expr(vsf_dynstack_t *expr_stack)
{
    vsfvm_lexer_etoken_t *etoken;
    uint_fast16_t token, token_param;

    vsf_trace_debug("parser expr: ");
    for (uint_fast32_t i = 0; i < expr_stack->sp; i++) {
        etoken = vsf_dynarr_get(&expr_stack->use_as__vsf_dynarr_t, i);
        if (!etoken) {
            vsf_trace_error("fail to get token" VSF_TRACE_CFG_LINEEND);
            return;
        }

        token_param = etoken->token >> 16;
        token = etoken->token & 0xFFFF;
        switch (token) {
        case VSFVM_TOKEN_NUM:
            vsf_trace_debug("num=%d,", etoken->data.ival);
            break;
        case VSFVM_TOKEN_RESOURCES:
            vsf_trace_debug("res=%s,", etoken->data.sym->name);
            break;
        case VSFVM_TOKEN_VAR_ID:
            vsf_trace_debug("var=%s,", etoken->data.sym->name);
            break;
        case VSFVM_TOKEN_VAR_ID_REF:
            vsf_trace_debug("ref=%s,", etoken->data.sym->name);
            break;
        case VSFVM_TOKEN_FUNC_ID:
            vsf_trace_debug("func=%s,", etoken->data.sym->name);
            break;
        case VSFVM_TOKEN_FUNC_CALL:
            vsf_trace_debug("call=%s(%d),", etoken->data.sym->name, token_param);
            break;
        default:
            if (token > VSFVM_TOKEN_BINARY_OP) {
                switch (token) {
                case VSFVM_TOKEN_MUL:       vsf_trace_debug("mul,");      break;
                case VSFVM_TOKEN_DIV:       vsf_trace_debug("div,");      break;
                case VSFVM_TOKEN_MOD:       vsf_trace_debug("mod,");      break;
                case VSFVM_TOKEN_ADD:       vsf_trace_debug("add,");      break;
                case VSFVM_TOKEN_SUB:       vsf_trace_debug("sub,");      break;
                case VSFVM_TOKEN_SHL:       vsf_trace_debug("shl,");      break;
                case VSFVM_TOKEN_SHR:       vsf_trace_debug("shr,");      break;
                case VSFVM_TOKEN_LT:        vsf_trace_debug("lt,");       break;
                case VSFVM_TOKEN_LE:        vsf_trace_debug("le,");       break;
                case VSFVM_TOKEN_GT:        vsf_trace_debug("gt,");       break;
                case VSFVM_TOKEN_GE:        vsf_trace_debug("ge,");       break;
                case VSFVM_TOKEN_EQ:        vsf_trace_debug("eq,");       break;
                case VSFVM_TOKEN_NE:        vsf_trace_debug("ne,");       break;
                case VSFVM_TOKEN_AND:       vsf_trace_debug("and,");      break;
                case VSFVM_TOKEN_XOR:       vsf_trace_debug("xor,");      break;
                case VSFVM_TOKEN_OR:        vsf_trace_debug("or,");       break;
                case VSFVM_TOKEN_LAND:      vsf_trace_debug("land,");     break;
                case VSFVM_TOKEN_LOR:       vsf_trace_debug("lor,");      break;
                case VSFVM_TOKEN_ASSIGN:    vsf_trace_debug("assign,");   break;
                case VSFVM_TOKEN_COMMA_OP:  vsf_trace_debug("comma,");    break;
                default:
                    vsf_trace_error("token not support" VSF_TRACE_CFG_LINEEND);
                    return;
                }
            } else if (token > VSFVM_TOKEN_UNARY_OP) {
                switch (token) {
                case VSFVM_TOKEN_NOT:       vsf_trace_debug("not,");      break;
                case VSFVM_TOKEN_REV:       vsf_trace_debug("rev,");      break;
                case VSFVM_TOKEN_NEGA:      vsf_trace_debug("nega,");     break;
                case VSFVM_TOKEN_POSI:      vsf_trace_debug("posi,");     break;
                default:
                    vsf_trace_error("token not support" VSF_TRACE_CFG_LINEEND);
                    return;
                }
            } else {
                vsf_trace_error("token not support" VSF_TRACE_CFG_LINEEND);
                return;
            }
        }
    }
    vsf_trace_debug(VSF_TRACE_CFG_LINEEND);
}
#endif

static vsf_err_t __vsfvm_func_push_ctx(vsfvm_compiler_func_t *func)
{
    vsfvm_compiler_func_ctx_t *ctx = &func->curctx;
    vsf_err_t err;

    err = vsf_dynstack_push(&func->ctx, ctx, 1);
    if (err) { return err; }

    memset(ctx, 0, sizeof(*ctx));
    ctx->symtbl_idx = -1;
    return VSF_ERR_NONE;
}

static vsfvm_compiler_func_ctx_t * __vsfvm_func_pop_ctx(vsfvm_compiler_func_t *func)
{
    vsfvm_compiler_func_ctx_t *ctx = vsf_dynstack_pop(&func->ctx, 1);
    if (ctx != NULL) {
        func->curctx = *ctx;
    }
    return &func->curctx;
}

static vsf_err_t __vsfvm_parse_stmt(vsfvm_compiler_t *compiler, vsfvm_pt_t *pt,
    vsfvm_pt_evt_t evt, uint_fast32_t token, vsfvm_token_data_t *data)
{
    vsfvm_compiler_script_t *script = &compiler->script;
    vsfvm_lexer_t *lexer = &script->lexer;
    vsfvm_compiler_func_t *func = &script->cur_func;
    vsfvm_compiler_func_ctx_t *ctx = &func->curctx;
    vsf_dynstack_t *stack_exp = &lexer->expr.stack_exp;
    vsfvm_bytecode_t fix_code;
    int err;
    bool token_unprocessed = false;

    vsfvm_pt_begin(pt);

    while (1) {
        if (token == VSFVM_TOKEN_BLOCK_BEGIN) {
            if (func->symtbl_idx < 0) {
                func->symtbl_idx = script->lexer.symtbl_stack.sp - 1;
            } else if (ctx->etoken.token > 0) {
                if (ctx->symtbl_idx < 0) {
                    ctx->symtbl_idx = script->lexer.symtbl_stack.sp - 1;
                }
            }

#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: block begin, level = %d" VSF_TRACE_CFG_LINEEND, func->block_level);
#endif
            func->block_level++;
        } else if (token == VSFVM_TOKEN_BLOCK_END) {
            func->block_level--;
#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: block end, level = %d" VSF_TRACE_CFG_LINEEND, func->block_level);
#endif

            if (func->block_level < 0) {
                return -VSFVM_PARSER_INVALID_CLOSURE;
            } else if (!func->block_level) {
#if VSFVM_PARSER_DEBUG_EN == ENABLED
                vsf_trace_debug("parser: func %s end" VSF_TRACE_CFG_LINEEND, func->name->name);
#endif
                if (__vsfvm_push_bytecode(compiler, VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_return, 0, 0)) < 0) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }

                __vsfvm_compiler_func_fini(func);
                func = vsf_dynstack_pop(&script->func_stack, 1);
                if (!func) { return -VSFVM_BUG; }

                script->cur_func = *func;
                func = &script->cur_func;
                ctx = &func->curctx;
                if (ctx->etoken.token == VSFVM_TOKEN_FUNC) {
                    if (ctx->func_ctx.goto_anchor >= 0) {
                        fix_code = ctx->func_ctx.goto_code;
                        fix_code |= (int16_t)(compiler->bytecode_pos - ctx->func_ctx.goto_anchor - 1);
                        vsfvm_set_bytecode_imp(compiler, fix_code, ctx->func_ctx.goto_anchor);
                    }
                    ctx = __vsfvm_func_pop_ctx(func);
                }
            } else {
                if (data->uval) {

                    if (__vsfvm_push_bytecode(compiler, VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_goto, data->uval, 0)) < 0) {
                        return -VSFVM_BYTECODE_TOOLONG;
                    }
                    while (data->uval--) {
                        vsf_slist_remove_tail(vsfvm_lexer_sym_t, symbol_node, &func->varlist);
                    }
                }

                if ((ctx->etoken.token > 0) && (ctx->block_level == func->block_level)) {
                block_close:
                    if (ctx->etoken.token == VSFVM_TOKEN_IF) {
                        vsfvm_pt_wait(pt);
                        if (token != VSFVM_TOKEN_ELSE) {
                            token_unprocessed = true;
                            fix_code = ctx->if_ctx.if_code;
                            fix_code |= (int16_t)(compiler->bytecode_pos - ctx->if_ctx.if_anchor - 1);
                            vsfvm_set_bytecode_imp(compiler, fix_code, ctx->if_ctx.if_anchor);

                            ctx = __vsfvm_func_pop_ctx(func);
                            if ((ctx->etoken.token > 0) && (ctx->block_level == func->block_level)) {
                                goto block_close;
                            }
                        } else {
                            ctx->etoken.token = VSFVM_TOKEN_ELSE;
                            ctx->if_ctx.else_anchor = compiler->bytecode_pos;
                            ctx->if_ctx.else_code = VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_goto, 0, 0);
                            compiler->bytecode_pos++;
                            fix_code = ctx->if_ctx.if_code;
                            fix_code |= (int16_t)(compiler->bytecode_pos - ctx->if_ctx.if_anchor - 1);
                            vsfvm_set_bytecode_imp(compiler, fix_code, ctx->if_ctx.if_anchor);
                        }
                    } else if (ctx->etoken.token == VSFVM_TOKEN_ELSE) {
                        fix_code = ctx->if_ctx.else_code;
                        fix_code |= (int16_t)(compiler->bytecode_pos - ctx->if_ctx.else_anchor - 1);
                        vsfvm_set_bytecode_imp(compiler, fix_code, ctx->if_ctx.else_anchor);

                        ctx = __vsfvm_func_pop_ctx(func);
                        if ((ctx->etoken.token > 0) && (ctx->block_level == func->block_level)) {
                            goto block_close;
                        }
                    } else if (ctx->etoken.token == VSFVM_TOKEN_WHILE) {
                        if (__vsfvm_push_bytecode(compiler, VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_goto, 0,
                                (int16_t)(ctx->while_ctx.calc_anchor - compiler->bytecode_pos - 1))) < 0) {
                            return -VSFVM_BYTECODE_TOOLONG;
                        }
                        fix_code = ctx->while_ctx.if_code;
                        fix_code |= (int16_t)(compiler->bytecode_pos - ctx->while_ctx.if_anchor - 1);
                        vsfvm_set_bytecode_imp(compiler, fix_code, ctx->while_ctx.if_anchor);
                        ctx = __vsfvm_func_pop_ctx(func);

                        if ((ctx->etoken.token > 0) && (ctx->block_level == func->block_level)) {
                            goto block_close;
                        }
                    } else {
                        return -VSFVM_BUG;
                    }
                }
            }
        } else if (token == VSFVM_TOKEN_IMPORT) {
            char *path = data->sym->name;

#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: import module \"%s\"" VSF_TRACE_CFG_LINEEND, path);
#endif

            vsfvm_lexer_list_t *list = __vsfvm_get_lexer(compiler, vsfvm_module_get_lexer_imp(path));
            int err;

            if (!list) {
                vsf_trace_error("fail to find lexer for usrlib %s" VSF_TRACE_CFG_LINEEND, path);
                return -VSFVM_COMPILER_FAIL_USRLIB;
            }

            vsfvm_lexer_ctx_new(lexer, list);
            err = vsfvm_module_require_lib_imp(compiler, path);
            vsfvm_lexer_ctx_delete(lexer);

            if (err) {
                vsf_trace_error("fail to import usrlib %s" VSF_TRACE_CFG_LINEEND, path);
                return -VSFVM_COMPILER_FAIL_USRLIB;
            }
        } else if (token == VSFVM_TOKEN_VAR) {
#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: variable \"%s\", type %s" VSF_TRACE_CFG_LINEEND, data->sym->name,
                    data->sym->c ? data->sym->c->name : "value");
            if (stack_exp->sp > 0) {
                __vsfvm_print_expr(stack_exp);
            }
#endif

            if (!data->sym->c) {
                vsfvm_lexer_etoken_t *etoken = vsf_dynstack_get(stack_exp, 0);

                data->sym->c = NULL;
                if (    (etoken != NULL)
                    &&  ((etoken->token & 0xFFFF) == VSFVM_TOKEN_FUNC_CALL)
                    &&  (etoken->data.sym->func.retc != NULL)) {
                    data->sym->c = etoken->data.sym->func.retc;
                }
            }

            vsf_slist_append(vsfvm_lexer_sym_t, symbol_node, &func->varlist, data->sym);
            if (__vsfvm_push_bytecode(compiler, VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_var, VSFVM_CODE_VAR_I32, 0)) < 0) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
            if (stack_exp->sp) {
                uint8_t pos;
                uint16_t idx;

                if (__vsfvm_compiler_get_var(compiler, script, func, data->sym, &pos, &idx)) {
                    return -VSFVM_BUG;
                }
                if (__vsfvm_push_bytecode(compiler, VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_REFERENCE, pos, idx)) < 0) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
                err = __vsfvm_push_expr(compiler, stack_exp);
                if (err) { return err; }
                if (    (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ASSIGN, 0, 0)) < 0)
                    ||  (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0)) < 0)) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
                err = __vsfvm_push_res(compiler, func);
                if (err) { return err; }
            }
        } else if (token == VSFVM_TOKEN_CONST) {
#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: const \"%s\" = %d" VSF_TRACE_CFG_LINEEND, data->sym->name, data->sym->ival);
#endif
        } else if (token == VSFVM_TOKEN_FUNC) {
            __vsfvm_func_push_ctx(func);
            ctx = &func->curctx;
            ctx->etoken.token = token;
            ctx->block_level = func->block_level;

            if (func->name) {
                ctx->func_ctx.goto_anchor = compiler->bytecode_pos;
                ctx->func_ctx.goto_code = VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_goto, 0, 0);
                compiler->bytecode_pos++;
            } else {
                ctx->func_ctx.goto_anchor = -1;
            }

            data->sym->func.pos = compiler->bytecode_pos;
            __vsfvm_push_func(script, data->sym);
            func = &script->cur_func;

            vsfvm_lexer_etoken_t *etoken;
            for (uint_fast32_t i = 0; i < stack_exp->sp; i++) {
                etoken = vsf_dynarr_get(&stack_exp->use_as__vsf_dynarr_t, i);
                vsf_slist_append(vsfvm_lexer_sym_t, symbol_node, &func->arglist, etoken->data.sym);
            }

#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: function \"%s\"(%d)" VSF_TRACE_CFG_LINEEND, data->sym->name, stack_exp->sp);
            if (stack_exp->sp > 0) {
                vsf_trace_debug("parser: function argument" VSF_TRACE_CFG_LINEEND);
                __vsfvm_print_expr(stack_exp);
            }
#endif
        } else if (token == VSFVM_TOKEN_IF) {
#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: if" VSF_TRACE_CFG_LINEEND);
            __vsfvm_print_expr(stack_exp);
#endif

            __vsfvm_func_push_ctx(func);
            ctx = &func->curctx;
            ctx->etoken.token = token;
            ctx->block_level = func->block_level;

        push_if:
            if (    (__vsfvm_push_bytecode(compiler, VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_var, VSFVM_CODE_VAR_I32, 0)) < 0)
                ||  (__vsfvm_push_bytecode(compiler, VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_REFERENCE, VSFVM_CODE_VARIABLE_POS_STACK_END, 0)) < 0)) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
            err = __vsfvm_push_expr(compiler, stack_exp);
            if (err) { return err; }
            if (    (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ASSIGN, 0, 0)) < 0)
                ||  (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0)) < 0)) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
            err = __vsfvm_push_res(compiler, func);
            if (err) { return err; }
            ctx->if_ctx.if_anchor = compiler->bytecode_pos;
            ctx->if_ctx.if_code = VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_if, 0, 0);
            compiler->bytecode_pos++;
        } else if (token == VSFVM_TOKEN_WHILE) {
#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: while" VSF_TRACE_CFG_LINEEND);
            __vsfvm_print_expr(stack_exp);
#endif

            __vsfvm_func_push_ctx(func);
            ctx = &func->curctx;
            ctx->etoken.token = token;
            ctx->block_level = func->block_level;
            ctx->while_ctx.calc_anchor = compiler->bytecode_pos;
            goto push_if;
        } else if (token == VSFVM_TOKEN_RET) {
#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: return" VSF_TRACE_CFG_LINEEND);
            __vsfvm_print_expr(stack_exp);
#endif

            if (stack_exp->sp) {
                if (__vsfvm_push_bytecode(compiler, VSFVM_VARIABLE(VSFVM_CODE_VARIABLE_REFERENCE_NOTRACE, VSFVM_CODE_VARIABLE_POS_FUNCARG, 0)) < 0) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
                err = __vsfvm_push_expr(compiler, stack_exp);
                if (err) { return err; }
                if (    (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_ASSIGN, 0, 0)) < 0)
                    ||  (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0)) < 0)) {
                    return -VSFVM_BYTECODE_TOOLONG;
                }
                err = __vsfvm_push_res(compiler, func);
                if (err) { return err; }
            }
            if (__vsfvm_push_bytecode(compiler, VSFVM_KEYWORD(VSFVM_CODE_KEYWORD_return, 0, 0)) < 0) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
        } else if (token == VSFVM_TOKEN_EXPR) {
#if VSFVM_PARSER_DEBUG_EN == ENABLED
            vsf_trace_debug("parser: expression" VSF_TRACE_CFG_LINEEND);
            __vsfvm_print_expr(stack_exp);
#endif

            err = __vsfvm_push_expr(compiler, stack_exp);
            if (err) { return err; }
            if (__vsfvm_push_bytecode(compiler, VSFVM_SYMBOL(VSFVM_CODE_SYMBOL_SEMICOLON, 0, 0)) < 0) {
                return -VSFVM_BYTECODE_TOOLONG;
            }
            err = __vsfvm_push_res(compiler, func);
            if (err) { return err; }
        } else {
            return -VSFVM_NOT_SUPPORT;
        }

        if (!token_unprocessed) {
            vsfvm_pt_wait(pt);
        }

        token_unprocessed = false;
    }

    vsfvm_pt_end(pt);
    return VSF_ERR_NONE;
}

vsf_err_t vsfvm_on_stmt(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data)
{
    vsfvm_compiler_script_t *script = container_of(lexer, vsfvm_compiler_script_t, lexer);
    vsfvm_compiler_t *compiler = container_of(script, vsfvm_compiler_t, script);
    return __vsfvm_parse_stmt(compiler, &script->pt_stmt, VSFVM_EVT_ON_STMT, token, data);
}

int vsfvm_compiler_register_lexer(vsfvm_compiler_t *compiler, vsfvm_lexer_list_t *lexer_list)
{
    vsf_slist_add_to_head(vsfvm_lexer_list_t, op_node, &compiler->lexer_list, lexer_list);
    return 0;
}

void vsfvm_compiler_fini(vsfvm_compiler_t *compiler)
{
    __vsfvm_compiler_script_fini(compiler);
}

int vsfvm_compiler_init(vsfvm_compiler_t *compiler)
{
    memset(compiler, 0, sizeof(*compiler));
    return 0;
}

static int __vsfvm_compiler_ext_init(vsfvm_compiler_t *compiler, vsfvm_ext_t *ext)
{
    vsfvm_compiler_script_t *script = &compiler->script;

    ext->symarr.sym = (vsfvm_lexer_sym_t *)ext->op->sym;
    ext->symarr.num = ext->op->sym_num;
    ext->symarr.id = script->lexer.symid;
    script->lexer.symid += ext->symarr.num;
    return 0;
}

static void __vsfvm_compiler_script_fini(vsfvm_compiler_t *compiler)
{
    vsfvm_compiler_script_t *script = &compiler->script;
    vsfvm_compiler_func_t *f;

    vsfvm_lexer_fini(&script->lexer);
    __vsfvm_compiler_func_fini(&script->cur_func);

    do {
        if ((f = vsf_dynstack_pop(&script->func_stack, 1))) {
            __vsfvm_compiler_func_fini(f);
        }
    } while (f != NULL);
    vsf_dynstack_fini(&script->func_stack);
}

int vsfvm_compiler_set_script(vsfvm_compiler_t *compiler, const char *script_name)
{
    vsfvm_compiler_script_t *script = &compiler->script;
    vsfvm_lexer_list_t *lexer;
    uint_fast16_t func_id = 0, var_id = 0;
    int err;

    memset(script, 0, sizeof(*script));

    script->func_stack.item_size = sizeof(script->cur_func);
    script->func_stack.item_num_per_buf_bitlen = 1;
    script->func_stack.buf_num_per_table_bitlen = 4;
    vsf_dynstack_init(&script->func_stack);

    script->name = script_name;
    script->pt_stmt.state = 0;

    lexer = __vsfvm_get_lexer(compiler, vsfvm_module_get_lexer_imp(script_name));
    if (!lexer) { return VSFVM_NOT_SUPPORT; }
    err = vsfvm_lexer_init(&script->lexer, lexer);
    if (err) { return err; }

    __vsf_slist_foreach_unsafe(vsfvm_ext_t, ext_node, &vsfvm_ext_list) {
        err = __vsfvm_compiler_ext_init(compiler, _);
        if (err) { return err; }
        _->func_id = func_id;
        func_id += _->op->func_num;
        _->var_id = var_id;
        var_id += _->op->var_num;
    }

    err = vsfvm_compiler_input(compiler, "__startup__(){");
    if (err < 0) { __vsfvm_compiler_script_fini(compiler); }
    return err;
}

int vsfvm_compiler_input(vsfvm_compiler_t *compiler, const char *code)
{
    vsfvm_lexer_t *lexer = &compiler->script.lexer;
    int err;

    if (code[0] == '\xFF') {
        err = vsfvm_lexer_input(lexer, "}");
        __vsfvm_compiler_script_fini(compiler);
    } else {
        err = vsfvm_lexer_input(lexer, code);
    }
    return err;
}

#endif      // VSFVM_CFG_COMPILER_EN
