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

#include "../../vsf_vm_cfg.h"

#if VSFVM_CFG_COMPILER_EN == ENABLED

#include "../vsfvm_compiler.h"
#include "./vsfvm_lexer.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static char * vsfvm_symtbl_strpush(vsfvm_lexer_symtbl_t *symtbl, char *symbol)
{
    vsf_dynarr_t *strpool = &symtbl->strpool;
    uint_fast32_t len = strlen(symbol) + 1;
    char *ret;

    if (len > strpool->item_size) {
        return NULL;
    }

    if (    !symtbl->strbuf
        ||  (symtbl->strpos + len >= symtbl->strbuf + strpool->item_size)) {
        uint_fast32_t size = vsf_dynarr_get_size(strpool);
        if (vsf_dynarr_set_size(strpool, size + 1) < 0) {
            return NULL;
        }
        symtbl->strbuf = symtbl->strpos = vsf_dynarr_get(strpool, size);
    }

    ret = symtbl->strpos;
    strcpy(symtbl->strpos, symbol);
    symtbl->strpos += len;
    return ret;
}

static vsfvm_lexer_sym_t * vsfvm_symarr_get(vsfvm_lexer_symarr_t *symarr, char *symbol, uint16_t *id)
{
    vsfvm_lexer_sym_t *sym = symarr->sym;
    for (uint_fast16_t i = 0; i < symarr->num; i++, sym++) {
        if (!strcmp(sym->name, symbol)) {
            if (id != NULL) {
                *id = i;
            }
            return sym;
        }
    }
    return NULL;
}

static vsfvm_lexer_sym_t * vsfvm_symtbl_get(vsfvm_lexer_symtbl_t *symtbl, char *symbol)
{
    vsfvm_lexer_sym_t *sym;
    uint_fast32_t size = vsf_dynarr_get_size(&symtbl->table);

    for (uint_fast32_t i = 0; i < size; i++) {
        sym = vsf_dynarr_get(&symtbl->table, i);
        if (!strcmp(sym->name, symbol)) {
            return sym;
        }
    }
    return NULL;
}

static vsfvm_lexer_sym_t * vsfvm_symtbl_add(vsfvm_lexer_symtbl_t *symtbl, char *symbol)
{
    vsfvm_lexer_sym_t *sym = vsfvm_symtbl_get(symtbl, symbol);
    if (!sym) {
        uint_fast32_t size = vsf_dynarr_get_size(&symtbl->table);
        char *name = vsfvm_symtbl_strpush(symtbl, symbol);

        if (!name || (vsf_dynarr_set_size(&symtbl->table, size + 1) < 0)) {
            return NULL;
        }

        sym = vsf_dynarr_get(&symtbl->table, size);
        if (sym != NULL) {
            memset(sym, 0, sizeof(*sym));
            sym->name = name;
        }
    }
    return sym;
}

static int vsfvm_symtbl_fini(vsfvm_lexer_symtbl_t *symtbl)
{
    vsf_dynarr_fini(&symtbl->table);
    vsf_dynarr_fini(&symtbl->strpool);
    return 0;
}

static int vsfvm_symtbl_init(vsfvm_lexer_symtbl_t *symtbl)
{
    symtbl->varnum = 0;

    symtbl->table.item_size = sizeof(vsfvm_lexer_sym_t);
    symtbl->table.item_num_per_buf_bitlen = 2;
    symtbl->table.buf_num_per_table_bitlen = 2;
    vsf_dynarr_init(&symtbl->table);

    symtbl->strpool.item_size = VSFVM_LEXER_MAX_SYMLEN;
    symtbl->strpool.item_num_per_buf_bitlen = 0;
    symtbl->strpool.buf_num_per_table_bitlen = 4;
    vsf_dynarr_init(&symtbl->strpool);
    symtbl->strbuf = symtbl->strpos = NULL;
    return 0;
}

vsfvm_lexer_sym_t * vsfvm_lexer_symtbl_get(vsfvm_lexer_t *lexer, char *symbol, uint16_t *id)
{
    vsfvm_lexer_symtbl_t *symtbl;
    vsfvm_lexer_symarr_t *symarr;
    vsfvm_lexer_sym_t *sym;
    uint16_t offset;
    uint_fast32_t idx = 0;

    symarr = (vsfvm_lexer_symarr_t *)&lexer->curctx.op->keyword;
    sym = vsfvm_symarr_get(symarr, symbol, &offset);
    if (sym != NULL) {
    symarr_ok:
        if (id != NULL) {
            *id = symarr->id + offset;
        }
        return sym;
    }

    __vsf_slist_foreach_unsafe(vsfvm_ext_t, ext_node, &vsfvm_ext_list) {
        symarr = &_->symarr;
        sym = vsfvm_symarr_get(symarr, symbol, &offset);
        if (sym != NULL) { goto symarr_ok; }
    }

    do {
        symtbl = vsf_dynstack_get(&lexer->symtbl_stack, idx++);
        if (symtbl != NULL) {
            sym = vsfvm_symtbl_get(symtbl, symbol);
            if (sym != NULL) {
                if (id != NULL) {
                    *id = sym->id;
                }
                return sym;
            }
        }
    } while ((symtbl != NULL) && !lexer->within_cur_symtbl);
    return NULL;
}

vsfvm_lexer_sym_t * vsfvm_lexer_symtbl_add_symbol(vsfvm_lexer_t *lexer, char *symbol, uint16_t *id)
{
    vsfvm_lexer_sym_t *sym = vsfvm_lexer_symtbl_get(lexer, symbol, id);
    if (sym != NULL) { return sym; }

    vsfvm_lexer_symtbl_t *symtbl = vsf_dynstack_get(&lexer->symtbl_stack, 0);
    sym = vsfvm_symtbl_add(symtbl, symbol);
    if (sym != NULL) {
        sym->id = lexer->symid++;
        if (id != NULL) {
            *id = sym->id;
        }
#if VSFVM_LEXER_DEBUG_EN == ENABLED
        if ('"' == symbol[0]) {
            vsf_trace_debug("string(%08X): %s, id = %d" VSF_TRACE_CFG_LINEEND, symtbl, symbol + 1, sym->id);
        } else {
            vsf_trace_debug("symbol(%08X): %s, id = %d" VSF_TRACE_CFG_LINEEND, symtbl, symbol, sym->id);
        }
#endif
    }
    return sym;
}

int vsfvm_lexer_symtbl_delete(vsfvm_lexer_t *lexer)
{
    vsfvm_lexer_symtbl_t *symtbl = vsf_dynstack_pop(&lexer->symtbl_stack, 1);
    if (symtbl) { return vsfvm_symtbl_fini(symtbl); }
    return -1;
}

int vsfvm_lexer_symtbl_new(vsfvm_lexer_t *lexer)
{
    vsfvm_lexer_symtbl_t symtbl;
    vsfvm_symtbl_init(&symtbl);
    return vsf_dynstack_push(&lexer->symtbl_stack, &symtbl, 1);
}

int vsfvm_lexer_getchar(vsfvm_lexer_t *lexer)
{
    int ch = vsfvm_lexer_peekchar(lexer);

    if (ch != '\0') {
        lexer->curctx.pos++;
        lexer->curctx.col++;
        if (ch == '\n') {
            lexer->curctx.line++;
            lexer->curctx.col = 0;
        }
    }
    return ch;
}

int vsfvm_lexer_on_token(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data)
{
    return lexer->curctx.op->parse_token(lexer, &lexer->curctx.pt, VSFVM_EVT_ON_TOKEN, token, data);
}

vsfvm_lexer_etoken_t * vsfvm_lexer_expr_popexp(vsfvm_lexer_t *lexer)
{
    return (vsfvm_lexer_etoken_t *)vsf_dynstack_pop(&lexer->expr.stack_exp, 1);
}

int vsfvm_lexer_expr_pushexp(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data)
{
    vsfvm_lexer_etoken_t etoken = { token };
    if (data != NULL) { etoken.data = *data; }
    return vsf_dynstack_push(&lexer->expr.stack_exp, &etoken, 1);
}

vsfvm_lexer_etoken_t * vsfvm_lexer_expr_popop(vsfvm_lexer_t *lexer)
{
    return (vsfvm_lexer_etoken_t *)vsf_dynstack_pop(&lexer->expr.stack_op, 1);
}

int vsfvm_lexer_expr_pushop(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data)
{
    uint_fast32_t prio = token & VSFVM_TOKEN_OP_PRIO_MASK;
    vsfvm_lexer_etoken_t *etoken = vsf_dynstack_get(&lexer->expr.stack_op, 0);

    while ( (lexer->expr.stack_op.sp > lexer->expr.ctx.opsp)
        &&  (etoken != NULL)
        &&  ((etoken->token & VSFVM_TOKEN_OP_PRIO_MASK) <= prio)) {
        etoken = vsfvm_lexer_expr_popop(lexer);
        vsfvm_lexer_expr_pushexp(lexer, etoken->token, &etoken->data);
        etoken = vsf_dynstack_get(&lexer->expr.stack_op, 0);
    }

    vsfvm_lexer_etoken_t etoken_tmp = { token };
    if (data != NULL) { etoken_tmp.data = *data; }
    return vsf_dynstack_push(&lexer->expr.stack_op, &etoken_tmp, 1);
}

static int vsfvm_lexer_expr_pushctx(vsfvm_lexer_t *lexer, vsfvm_lexer_exprctx_t *ctx)
{
    int err = vsf_dynstack_push_ext(&lexer->expr.stack_op, ctx, sizeof(*ctx));
    if (!err) { lexer->expr.nesting++; }
    return err;
}

static int vsfvm_lexer_expr_popctx(vsfvm_lexer_t *lexer, vsfvm_lexer_exprctx_t *ctx)
{
    int err = vsf_dynstack_pop_ext(&lexer->expr.stack_op, ctx, sizeof(*ctx));
    if (!err) { lexer->expr.nesting--; }
    return err;
}

static int vsfvm_lexer_expr_nesting(vsfvm_lexer_t *lexer)
{
    vsfvm_lexer_exprctx_t *ctx = &lexer->expr.ctx;
    uint_fast32_t pre_token = ctx->pre_etoken.token;

    if (vsfvm_lexer_expr_pushctx(lexer, ctx)) {
        return -VSFVM_NOT_ENOUGH_RESOURCES;
    }

    if (pre_token == VSFVM_TOKEN_FUNC_CALL) {
        ctx->comma_is_op = false;
    } else if ((pre_token == VSFVM_TOKEN_LGROUPING)
        ||  (pre_token == VSFVM_TOKEN_EXPR_OPERAND_VARIABLE)) {
        ctx->comma_is_op = true;
    }
    ctx->pt.state = 0;
    ctx->pre_etoken.token = VSFVM_TOKEN_NONE;
    ctx->opsp = lexer->expr.stack_op.sp;
    ctx->expsp = lexer->expr.stack_exp.sp;
    return 0;
}

int vsfvm_lexer_expr_reset(vsfvm_lexer_t *lexer)
{
    vsf_dynarr_set_size(&lexer->expr.stack_exp.use_as__vsf_dynarr_t, 0);
    vsf_dynarr_set_size(&lexer->expr.stack_op.use_as__vsf_dynarr_t, 0);
    lexer->expr.stack_exp.sp = 0;
    lexer->expr.stack_op.sp = 0;
    memset(&lexer->expr.ctx, 0, sizeof(lexer->expr.ctx));
    lexer->expr.nesting = 0;
    return 0;
}

static vsf_err_t vsfvm_lexer_calc_expr(vsf_dynstack_t *expr_stack, int32_t *ival)
{
    vsf_dynstack_t calc_stack;
    vsfvm_lexer_etoken_t *etoken, *arg1, *arg2;
    vsf_err_t err = -VSFVM_NOT_SUPPORT;

    calc_stack.item_size = expr_stack->use_as__vsf_dynarr_t.item_size;
    calc_stack.item_num_per_buf_bitlen = expr_stack->use_as__vsf_dynarr_t.item_num_per_buf_bitlen;
    calc_stack.buf_num_per_table_bitlen = expr_stack->use_as__vsf_dynarr_t.buf_num_per_table_bitlen;
    vsf_dynstack_init(&calc_stack);

    for (uint_fast32_t i = 0; i < expr_stack->sp; i++) {
        etoken = vsf_dynarr_get(&expr_stack->use_as__vsf_dynarr_t, i);
        if (!etoken) {
        expr_bug:
            err = -VSFVM_BUG;
            goto end;
        }

        switch (etoken->token) {
        case VSFVM_TOKEN_NUM:
            vsf_dynstack_push(&calc_stack, etoken, 1);
            break;
        case VSFVM_TOKEN_VARIABLE:
        case VSFVM_TOKEN_VARIABLE_REF:
        case VSFVM_TOKEN_FUNC_ID:
        case VSFVM_TOKEN_FUNC_CALL:
            err = -VSFVM_NOT_SUPPORT;
            goto end;
        default:
            if (etoken->token > VSFVM_TOKEN_BINARY_OP) {
                arg2 = vsf_dynstack_pop(&calc_stack, 1);
                arg1 = vsf_dynstack_get(&calc_stack, 0);
                if (!arg1 || !arg2) {
                    goto expr_bug;
                }

                switch (etoken->token) {
                case VSFVM_TOKEN_MUL:       arg1->data.ival = arg1->data.ival * arg2->data.ival;    break;
                case VSFVM_TOKEN_DIV:
                    if (!arg2->data.ival) {
                        err = -VSFVM_PARSER_DIV0;
                        goto end;
                    }
                                            arg1->data.ival = arg1->data.ival / arg2->data.ival;    break;
                case VSFVM_TOKEN_MOD:       arg1->data.ival = arg1->data.ival & arg2->data.ival;    break;
                case VSFVM_TOKEN_ADD:       arg1->data.ival = arg1->data.ival + arg2->data.ival;    break;
                case VSFVM_TOKEN_SUB:       arg1->data.ival = arg1->data.ival - arg2->data.ival;    break;
                case VSFVM_TOKEN_SHL:       arg1->data.ival = arg1->data.ival << arg2->data.ival;   break;
                case VSFVM_TOKEN_SHR:       arg1->data.ival = arg1->data.ival >> arg2->data.ival;   break;
                case VSFVM_TOKEN_LT:        arg1->data.ival = arg1->data.ival < arg2->data.ival;    break;
                case VSFVM_TOKEN_LE:        arg1->data.ival = arg1->data.ival <= arg2->data.ival;   break;
                case VSFVM_TOKEN_GT:        arg1->data.ival = arg1->data.ival > arg2->data.ival;    break;
                case VSFVM_TOKEN_GE:        arg1->data.ival = arg1->data.ival >= arg2->data.ival;   break;
                case VSFVM_TOKEN_EQ:        arg1->data.ival = arg1->data.ival == arg2->data.ival;   break;
                case VSFVM_TOKEN_NE:        arg1->data.ival = arg1->data.ival != arg2->data.ival;   break;
                case VSFVM_TOKEN_AND:       arg1->data.ival = arg1->data.ival & arg2->data.ival;    break;
                case VSFVM_TOKEN_XOR:       arg1->data.ival = arg1->data.ival ^ arg2->data.ival;    break;
                case VSFVM_TOKEN_OR:        arg1->data.ival = arg1->data.ival | arg2->data.ival;    break;
                case VSFVM_TOKEN_LAND:      arg1->data.ival = arg1->data.ival && arg2->data.ival;   break;
                case VSFVM_TOKEN_LOR:       arg1->data.ival = arg1->data.ival || arg2->data.ival;   break;
                case VSFVM_TOKEN_COMMA_OP:  arg1->data.ival = arg2->data.ival;                      break;
                default:
                expr_not_support:
                    err = -VSFVM_NOT_SUPPORT;
                    goto end;
                }
            } else if (etoken->token > VSFVM_TOKEN_UNARY_OP) {
                arg1 = vsf_dynstack_get(&calc_stack, 0);
                if (!arg1) { goto expr_bug; }

                switch (etoken->token) {
                case VSFVM_TOKEN_NOT:       arg1->data.ival = !arg1->data.ival;                     break;
                case VSFVM_TOKEN_REV:       arg1->data.ival = ~arg1->data.ival;                     break;
                case VSFVM_TOKEN_NEGA:      arg1->data.ival = -arg1->data.ival;                     break;
                case VSFVM_TOKEN_POSI:      arg1->data.ival = +arg1->data.ival;                     break;
                default: goto expr_not_support;
                }
            } else {
                goto expr_not_support;
            }
        }
    }
    if (calc_stack.sp == 1) {
        arg1 = vsf_dynstack_pop(&calc_stack, 1);
        if (ival != NULL) {
            *ival = arg1->data.ival;
        }
        err = VSF_ERR_NONE;
    }

end:
    vsf_dynstack_fini(&calc_stack);
    return err;
}

static vsf_err_t vsfvm_lexer_terminate_expr(vsfvm_lexer_t *lexer, uint_fast32_t token)
{
    vsfvm_lexer_exprctx_t *ctx = &lexer->expr.ctx, ctx_tmp;
    vsfvm_lexer_etoken_t *etoken, etoken_tmp = { VSFVM_TOKEN_NUM };

    if (    ctx->pre_etoken.token
        &&  (   (ctx->pre_etoken.token < VSFVM_TOKEN_EXPR_OPERAND)
            ||  (ctx->pre_etoken.token > VSFVM_TOKEN_FUNC_CALL))) {
        return -VSFVM_PARSER_UNEXPECTED_TOKEN;
    }
    while (lexer->expr.stack_op.sp > ctx->opsp) {
        etoken = vsfvm_lexer_expr_popop(lexer);
        if (!etoken) { return -VSFVM_BUG; }
        vsfvm_lexer_expr_pushexp(lexer, etoken->token, &etoken->data);
    }
    if (lexer->expr.nesting) {
        if (vsfvm_lexer_expr_popctx(lexer, &ctx_tmp) < 0) {
            return -VSFVM_BUG;
        }

        if (ctx_tmp.pre_etoken.token == VSFVM_TOKEN_FUNC_CALL) {
            if (token == VSFVM_TOKEN_RGROUPING) {
                if (ctx->pre_etoken.token != VSFVM_TOKEN_NONE) {
                    if (    ((ctx->pre_etoken.token < VSFVM_TOKEN_EXPR_OPERAND)
                                ||  (ctx->pre_etoken.token > VSFVM_TOKEN_OPERATOR))
                        &&  (ctx->pre_etoken.token != VSFVM_TOKEN_FUNC_CALL)) {
                        return -VSFVM_PARSER_EXPECT_FUNC_PARAM;
                    }
                    ctx_tmp.func_param++;
                    if (!ctx_tmp.func_param) {
                        return -VSFVM_PARSER_TOO_MANY_FUNC_PARAM;
                    }
                }

                vsfvm_lexer_expr_pushexp(lexer,
                    ctx_tmp.pre_etoken.token + (ctx_tmp.func_param << 16),
                    &ctx_tmp.pre_etoken.data);
                ctx_tmp.func_param = 0;
            expr_goon:
                *ctx = ctx_tmp;
                return VSF_ERR_NOT_READY;
            } else if (token == VSFVM_TOKEN_COMMA) {
                ctx_tmp.func_param++;
                if (!ctx_tmp.func_param) {
                    return -VSFVM_PARSER_TOO_MANY_FUNC_PARAM;
                }
                vsfvm_lexer_expr_pushctx(lexer, &ctx_tmp);
                ctx->pre_etoken.token = VSFVM_TOKEN_NONE;
                return VSF_ERR_NOT_READY;
            } else {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }
        } else if (ctx_tmp.pre_etoken.token == VSFVM_TOKEN_LGROUPING) {
            if (token == VSFVM_TOKEN_RGROUPING) {
                goto expr_goon;
            } else if (token == VSFVM_TOKEN_COMMA) {
            expr_comma_op_nesting:
                vsfvm_lexer_expr_pushctx(lexer, &ctx_tmp);
                ctx->pre_etoken.token = VSFVM_TOKEN_NONE;
                goto expr_comma_op;
            } else {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }
        } else if (ctx_tmp.pre_etoken.token == VSFVM_TOKEN_EXPR_OPERAND_VARIABLE) {
            if (token == VSFVM_TOKEN_COMMA) {
                goto expr_comma_op_nesting;
            } else {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }
        } else {
            return -VSFVM_BUG;
        }
    } else if (token == VSFVM_TOKEN_COMMA) {
        if (ctx->comma_is_op) {
        expr_comma_op:
            if (ctx->pre_etoken.token > VSFVM_TOKEN_OPERATOR) {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }
            vsfvm_lexer_expr_pushop(lexer, VSFVM_TOKEN_COMMA_OP, NULL);
            return VSF_ERR_NOT_READY;
        }
        goto terminate_expr;
    } else if ((token == VSFVM_TOKEN_SEMICOLON) || (token == VSFVM_TOKEN_RGROUPING)) {
    terminate_expr:
        // VSFVM_TOKEN_VAR_OP and VSFVM_TOKEN_FUNC_CALL are valid
        if (ctx->pre_etoken.token > VSFVM_TOKEN_UNARY_OP) {
            return -VSFVM_PARSER_UNEXPECTED_TOKEN;
        }
        if (lexer->expr.stack_op.sp != 0) {
            return -VSFVM_PARSER_INVALID_EXPR;
        }

        if (!vsfvm_lexer_calc_expr(&lexer->expr.stack_exp, &etoken_tmp.data.ival)) {
            vsf_dynstack_reset(&lexer->expr.stack_exp);
            vsf_dynstack_push(&lexer->expr.stack_exp, &etoken_tmp, 1);
        }
        return VSF_ERR_NONE;
    } else {
        return -VSFVM_PARSER_UNEXPECTED_TOKEN;
    }
}

static vsf_err_t vsfvm_lexer_parse_expr(vsfvm_lexer_t *lexer, vsfvm_pt_t *pt,
    vsfvm_pt_evt_t evt, uint_fast32_t token, vsfvm_token_data_t *data)
{
    vsfvm_lexer_exprctx_t *ctx = &lexer->expr.ctx;
    vsf_err_t err;

    vsfvm_pt_begin(pt);

    while (1) {
        if (token < VSFVM_TOKEN_STMT_END) {
            return -VSFVM_PARSER_UNEXPECTED_TOKEN;
        } else if ((token > VSFVM_TOKEN_EXPR_TERMINATOR)
               &&   (token < VSFVM_TOKEN_EXPR_TERMINATOR_END)) {
            err = vsfvm_lexer_terminate_expr(lexer, token);
            if (err < 0) {
                return err;
            } else if (err > 0) {
                if (    (token == VSFVM_TOKEN_RGROUPING)
                    &&  (   (ctx->pre_etoken.token != VSFVM_TOKEN_FUNC_CALL)
                         || !ctx->pre_etoken.data.sym->func.retc)) {
                    ctx->pre_etoken.token = VSFVM_TOKEN_EXPR_OPERAND_CONST;
                }
                goto expr_wait_next;
            } else {
                return VSF_ERR_NONE;
            }
        } else if ((token == VSFVM_TOKEN_NUM) || (token == VSFVM_TOKEN_RESOURCES)) {
            vsfvm_lexer_expr_pushexp(lexer, token, data);
            ctx->pre_etoken.token = VSFVM_TOKEN_EXPR_OPERAND_CONST;
        } else if (token == VSFVM_TOKEN_FUNC_CALL) {
            ctx->pre_etoken.token = VSFVM_TOKEN_FUNC_CALL;
            ctx->pre_etoken.data = *data;
            vsfvm_pt_wait(pt);
            if (token == VSFVM_TOKEN_LGROUPING) {
            expr_nesting:
                err = vsfvm_lexer_expr_nesting(lexer);
                if (err < 0) { return err; }
                goto expr_wait_next;
            } else {
                vsfvm_lexer_expr_pushexp(lexer, VSFVM_TOKEN_FUNC_ID, &ctx->pre_etoken.data);
                ctx->pre_etoken.token = VSFVM_TOKEN_EXPR_OPERAND_CONST;
                continue;
            }
        } else if (token == VSFVM_TOKEN_VARIABLE) {
            ctx->pre_etoken.token = VSFVM_TOKEN_EXPR_OPERAND_VARIABLE;
            ctx->pre_etoken.data = *data;
            vsfvm_pt_wait(pt);
            if (token == VSFVM_TOKEN_ASSIGN) {
                vsfvm_lexer_expr_pushexp(lexer, VSFVM_TOKEN_VARIABLE_REF, &ctx->pre_etoken.data);
                continue;
            } else {
                vsfvm_lexer_expr_pushexp(lexer, VSFVM_TOKEN_VARIABLE,
                    &ctx->pre_etoken.data);
                continue;
            }
        } else if (token == VSFVM_TOKEN_VARIABLE_REF) {
            ctx->pre_etoken.token = VSFVM_TOKEN_EXPR_OPERAND_VARIABLE;
            ctx->pre_etoken.data = *data;
            vsfvm_lexer_expr_pushexp(lexer, VSFVM_TOKEN_VARIABLE_REF, &ctx->pre_etoken.data);
        } else if (token == VSFVM_TOKEN_DOT) {
            vsfvm_pt_wait(pt);
            if (    (token != VSFVM_TOKEN_SYMBOL)
                &&  (token != VSFVM_TOKEN_VARIABLE)
                &&  (token != VSFVM_TOKEN_FUNC_CALL)) {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }

            const vsfvm_class_t *c;
            if (ctx->pre_etoken.token == VSFVM_TOKEN_FUNC_CALL) {
                c = ctx->pre_etoken.data.sym->func.retc;
            } else if (ctx->pre_etoken.token == VSFVM_TOKEN_EXPR_OPERAND_VARIABLE) {
                c = ctx->pre_etoken.data.sym->c;
            } else {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }
            if (!c) { return -VSFVM_PARSER_UNEXPECTED_TOKEN; }

            strcpy(lexer->cur_symbol, c->name);
            strcat(lexer->cur_symbol, "_");
            strcat(lexer->cur_symbol, data->sym->name);
            data->sym = vsfvm_lexer_symtbl_get(lexer, lexer->cur_symbol, NULL);
            if (!data->sym) { return -VSFVM_PARSER_MEMFUNC_NOT_FOUND; }

            do {
                vsfvm_lexer_etoken_t etoken = ctx->pre_etoken;

                ctx->pre_etoken.token = VSFVM_TOKEN_FUNC_CALL;
                ctx->pre_etoken.data = *data;
                ctx->func_param = 0;
                err = vsfvm_lexer_expr_nesting(lexer);
                if (err < 0) { return err; }
                ctx->pre_etoken = etoken;
            } while (0);
            vsfvm_pt_wait(pt);
            if (token != VSFVM_TOKEN_LGROUPING) {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }
            vsfvm_pt_wait(pt);
            if (token != VSFVM_TOKEN_RGROUPING) {
                vsfvm_lexer_terminate_expr(lexer, VSFVM_TOKEN_COMMA);
            }
            continue;
        } else if (token == VSFVM_TOKEN_LGROUPING) {
            if (    (ctx->pre_etoken.token != VSFVM_TOKEN_NONE)
                &&  (ctx->pre_etoken.token < VSFVM_TOKEN_OPERATOR)) {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }
            ctx->pre_etoken.token = VSFVM_TOKEN_LGROUPING;
            goto expr_nesting;
        } else if (token > VSFVM_TOKEN_BINARY_OP) {
            if (    (ctx->pre_etoken.token != VSFVM_TOKEN_EXPR_OPERAND_CONST)
                &&  (   (ctx->pre_etoken.token != VSFVM_TOKEN_EXPR_OPERAND_VARIABLE)
                     || (ctx->pre_etoken.data.sym->type < VSFVM_LEXER_SYM_OPRAND)
                     || ((ctx->pre_etoken.data.sym->c != NULL) && (token != VSFVM_TOKEN_ASSIGN)))) {
                if (token == VSFVM_TOKEN_ADD) {
                    token = VSFVM_TOKEN_POSI;
                    goto expr_parse_unary_op;
                } else if (token == VSFVM_TOKEN_SUB) {
                    token = VSFVM_TOKEN_NEGA;
                    goto expr_parse_unary_op;
                }
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            } else if ((token == VSFVM_TOKEN_ASSIGN)
                &&  (ctx->pre_etoken.token != VSFVM_TOKEN_EXPR_OPERAND_VARIABLE)) {
                return -VSFVM_PARSER_UNEXPECTED_TOKEN;
            }
            vsfvm_lexer_expr_pushop(lexer, token, data);
            ctx->pre_etoken.token = VSFVM_TOKEN_BINARY_OP;
            ctx->pre_etoken.data.uval = token;
        } else if (token > VSFVM_TOKEN_UNARY_OP) {
        expr_parse_unary_op:
            vsfvm_lexer_expr_pushop(lexer, token, data);
            ctx->pre_etoken.token = VSFVM_TOKEN_UNARY_OP;
            ctx->pre_etoken.data.uval = token;
        } else {
            return -VSFVM_PARSER_UNEXPECTED_TOKEN;
        }

    expr_wait_next:
        vsfvm_pt_wait(pt);
    }

    vsfvm_pt_end(pt);
    return VSF_ERR_NONE;
}

vsf_err_t vsfvm_lexer_on_expr(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data)
{
    vsf_err_t err = vsfvm_lexer_parse_expr(lexer, &lexer->expr.ctx.pt, VSFVM_EVT_ON_EXPR, token, data);
    if (err < 0) { vsfvm_lexer_expr_reset(lexer); }
    return err;
}

vsf_err_t vsfvm_lexer_on_stmt(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data)
{
    vsfvm_token_data_t local_data;

    switch (token) {
    case VSFVM_TOKEN_BLOCK_BEGIN:
        if (vsfvm_lexer_symtbl_new(lexer)) {
            return -VSFVM_NOT_ENOUGH_RESOURCES;
        }
        break;
    case VSFVM_TOKEN_BLOCK_END:
        do {
            vsfvm_lexer_symtbl_t *symtbl = vsf_dynstack_get(&lexer->symtbl_stack, 0);
            if (!symtbl) { return -VSFVM_BUG; }
            local_data.uval = symtbl->varnum;
            data = &local_data;
        } while (0);
        if (vsfvm_lexer_symtbl_delete(lexer)) {
            return -VSFVM_BUG;
        }
        break;
    case VSFVM_TOKEN_VAR:
        do {
            vsfvm_lexer_symtbl_t *symtbl;
            symtbl = vsf_dynstack_get(&lexer->symtbl_stack, 0);
            if (!symtbl) { return -VSFVM_BUG; }
            symtbl->varnum++;
        } while (0);
        break;
    }
    return vsfvm_on_stmt(lexer, token, data);
}

void vsfvm_lexer_fini(vsfvm_lexer_t *lexer)
{
    int err;

    if (lexer->curctx.priv != NULL) {
        vsf_heap_free(lexer->curctx.priv);
        lexer->curctx.priv = NULL;
    }

    vsfvm_lexer_expr_reset(lexer);

    do {
        err = vsfvm_lexer_ctx_delete(lexer);
    } while (!err);
    vsf_dynstack_fini(&lexer->ctx_stack);

    do {
        err = vsfvm_lexer_symtbl_delete(lexer);
    } while (!err);
    vsf_dynstack_fini(&lexer->symtbl_stack);
}

static int vsfvm_lexer_ctx_fini(vsfvm_lexer_t *lexer)
{
    vsfvm_lexer_ctx_t *ctx = &lexer->curctx;
    if (ctx->priv != NULL) {
        vsf_heap_free(ctx->priv);
        ctx->priv = NULL;
    }
    return 0;
}

static int vsfvm_lexer_ctx_init(vsfvm_lexer_t *lexer, vsfvm_lexer_list_t *list)
{
    vsfvm_lexer_ctx_t *ctx = &lexer->curctx;
    int ret;

    memset(ctx, 0, sizeof(*ctx));
    ctx->op = list->op;
    if (ctx->op->priv_size) {
        ctx->priv = vsf_heap_malloc(ctx->op->priv_size);
        if (!ctx->priv) { return -VSFVM_NOT_ENOUGH_RESOURCES; }
        memset(ctx->priv, 0, ctx->op->priv_size);
    }

    if (ctx->op->init != NULL) {
        ret = ctx->op->init(lexer);
        if (ret) { return ret; }
    }

    lexer->symid = VSFVM_LEXER_USER_SYMID;
    return 0;
}

int vsfvm_lexer_ctx_delete(vsfvm_lexer_t *lexer)
{
    vsfvm_lexer_ctx_t *ctx;

    vsfvm_lexer_ctx_fini(lexer);
    ctx = vsf_dynstack_pop(&lexer->ctx_stack, 1);
    if (!ctx) { return -1; }
    lexer->curctx = *ctx;
    return 0;
}

int vsfvm_lexer_ctx_new(vsfvm_lexer_t *lexer, vsfvm_lexer_list_t *list)
{
    vsfvm_lexer_ctx_t *ctx = &lexer->curctx;
    int err;

    if (ctx->op != NULL) {
        vsf_dynstack_push(&lexer->ctx_stack, ctx, 1);
    }
    err = vsfvm_lexer_ctx_init(lexer, list);
    if (err) { vsfvm_lexer_ctx_delete(lexer); }
    return err;
}

int vsfvm_lexer_init(vsfvm_lexer_t *lexer, vsfvm_lexer_list_t *list)
{
    int ret;

    memset(&lexer->expr, 0, sizeof(lexer->expr));

    lexer->ctx_stack.item_size = sizeof(vsfvm_lexer_ctx_t);
    lexer->ctx_stack.item_num_per_buf_bitlen = 1;
    lexer->ctx_stack.buf_num_per_table_bitlen = 4;
    vsf_dynstack_init(&lexer->ctx_stack);

    lexer->expr.stack_exp.item_size = sizeof(vsfvm_lexer_etoken_t);
    lexer->expr.stack_exp.item_num_per_buf_bitlen = 4;
    lexer->expr.stack_exp.buf_num_per_table_bitlen = 4;
    vsf_dynstack_init(&lexer->expr.stack_exp);

    lexer->expr.stack_op.item_size = sizeof(vsfvm_lexer_etoken_t);
    lexer->expr.stack_op.item_num_per_buf_bitlen = 4;
    lexer->expr.stack_op.buf_num_per_table_bitlen = 4;
    vsf_dynstack_init(&lexer->expr.stack_op);

    lexer->symtbl_stack.item_size = sizeof(vsfvm_lexer_symtbl_t);
    lexer->symtbl_stack.item_num_per_buf_bitlen = 1;
    lexer->symtbl_stack.buf_num_per_table_bitlen = 4;
    vsf_dynstack_init(&lexer->symtbl_stack);

    ret = vsfvm_lexer_symtbl_new(lexer);
    if (ret) { return ret; }

    ret = vsfvm_lexer_ctx_new(lexer, list);
    if (ret) { goto cleanup_symtbl; }
    return 0;
cleanup_symtbl:
    vsfvm_lexer_symtbl_delete(lexer);
    return ret;
}

int vsfvm_lexer_input(vsfvm_lexer_t *lexer, const char *code)
{
    if (!lexer->curctx.op) {
        return -VSFVM_LEXER_INVALID_OP;
    }

    lexer->curctx.pos = code;
    return lexer->curctx.op->input(lexer);
}

#endif        // VSFVM_CFG_COMPILER_EN
