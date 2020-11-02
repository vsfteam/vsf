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

#include "../../../vsf_vm_cfg.h"

#if VSFVM_CFG_COMPILER_EN == ENABLED

// for ctype.h
#include "utilities/vsf_utilities.h"
#include "../../vsfvm_compiler.h"
#include "../vsfvm_lexer.h"

/*============================ MACROS ========================================*/

#define VSFVM_LEXER_DART_USER_SYMID     (VSFVM_DART_TOKEN_SYM | VSFVM_LEXER_USER_SYMID)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfvm_lexer_dart_t {
    bool parse_enter;
    bool isconst;
    bool type_parsed;
    bool isref;
    vsfvm_pt_t pt_expr;

    union {
        vsfvm_lexer_sym_t *type_sym;
        vsfvm_lexer_sym_t *alias_sym;
    };
    vsfvm_lexer_etoken_t pre_etoken;
    vsfvm_lexer_etoken_t etoken;
} vsfvm_lexer_dart_t;

typedef enum vsfvm_dart_token_t {
    VSFVM_DART_TOKEN_NL         = '\n',
    VSFVM_DART_TOKEN_REF        = '@',
    VSFVM_DART_TOKEN_POUND      = '#',
    VSFVM_DART_TOKEN_DOT        = '.',
    VSFVM_DART_TOKEN_ASSIGN     = '=',
    VSFVM_DART_TOKEN_ADD        = '+',
    VSFVM_DART_TOKEN_SUB        = '-',
    VSFVM_DART_TOKEN_MUL        = '*',
    VSFVM_DART_TOKEN_DIV        = '/',
    VSFVM_DART_TOKEN_INTDIV     = '~' + ('/' << 8),
    VSFVM_DART_TOKEN_AND        = '&',
    VSFVM_DART_TOKEN_OR         = '|',
    VSFVM_DART_TOKEN_XOR        = '^',
    VSFVM_DART_TOKEN_MOD        = '%',
    VSFVM_DART_TOKEN_NOT        = '!',
    VSFVM_DART_TOKEN_GT         = '>',
    VSFVM_DART_TOKEN_LT          = '<',
    VSFVM_DART_TOKEN_COND       = '?',
    VSFVM_DART_TOKEN_COMMA      = ',',
    VSFVM_DART_TOKEN_SEMICOLON  = ';',
    VSFVM_DART_TOKEN_REV        = '~',
    VSFVM_DART_TOKEN_LBRACE     = '{',
    VSFVM_DART_TOKEN_RBRACE     = '}',
    VSFVM_DART_TOKEN_LGROUPING  = '(',
    VSFVM_DART_TOKEN_RGROUPING  = ')',
    VSFVM_DART_TOKEN_POINTER    = '-' + ('>' << 8),
    VSFVM_DART_TOKEN_2EQ        = 0x10000,
    VSFVM_DART_TOKEN_EQ         = VSFVM_DART_TOKEN_ASSIGN | VSFVM_DART_TOKEN_2EQ,
    VSFVM_DART_TOKEN_NE         = VSFVM_DART_TOKEN_NOT | VSFVM_DART_TOKEN_2EQ,
    VSFVM_DART_TOKEN_GE         = VSFVM_DART_TOKEN_GT | VSFVM_DART_TOKEN_2EQ,
    VSFVM_DART_TOKEN_LE         = VSFVM_DART_TOKEN_LT | VSFVM_DART_TOKEN_2EQ,
    VSFVM_DART_TOKEN_DOUBLE     = 0x20000,
    VSFVM_DART_TOKEN_LAND       = VSFVM_DART_TOKEN_AND | VSFVM_DART_TOKEN_DOUBLE,
    VSFVM_DART_TOKEN_LOR        = VSFVM_DART_TOKEN_OR | VSFVM_DART_TOKEN_DOUBLE,
    VSFVM_DART_TOKEN_SHL        = VSFVM_DART_TOKEN_LT | VSFVM_DART_TOKEN_DOUBLE,
    VSFVM_DART_TOKEN_SHR        = VSFVM_DART_TOKEN_GT | VSFVM_DART_TOKEN_DOUBLE,
    VSFVM_DART_TOKEN_NUM        = 0x40000,

    VSFVM_DART_TOKEN_SYM        = 0x80000,
    VSFVM_DART_TOKEN_SYM_VAR    = VSFVM_DART_TOKEN_SYM,
    VSFVM_DART_TOKEN_SYM_CONST,
    VSFVM_DART_TOKEN_SYM_ALIAS,
    VSFVM_DART_TOKEN_SYM_IMPORT,
    VSFVM_DART_TOKEN_SYM_IF,
    VSFVM_DART_TOKEN_SYM_ELSE,
    VSFVM_DART_TOKEN_SYM_FOR,
    VSFVM_DART_TOKEN_SYM_WHILE,
    VSFVM_DART_TOKEN_SYM_DO,
    VSFVM_DART_TOKEN_SYM_SWITCH,
    VSFVM_DART_TOKEN_SYM_CASE,
    VSFVM_DART_TOKEN_SYM_RETURN,
    VSFVM_DART_TOKEN_SYM_BREAK,
    VSFVM_DART_TOKEN_SYM_CONTINUE,
    VSFVM_DART_TOKEN_SYM_THREAD,
} vsfvm_dart_token_t;

/*============================ LOCAL VARIABLES ===============================*/

static const vsfvm_lexer_sym_t __vsfvm_lexer_dart_keyword[] = {
    VSFVM_LEXERSYM_KEYWORKD("var"),
    VSFVM_LEXERSYM_KEYWORKD("const"),
    VSFVM_LEXERSYM_KEYWORKD("alias"),
    VSFVM_LEXERSYM_KEYWORKD("import"),
    VSFVM_LEXERSYM_KEYWORKD("if"),
    VSFVM_LEXERSYM_KEYWORKD("else"),
    VSFVM_LEXERSYM_KEYWORKD("for"),
    VSFVM_LEXERSYM_KEYWORKD("while"),
    VSFVM_LEXERSYM_KEYWORKD("do"),
    VSFVM_LEXERSYM_KEYWORKD("switch"),
    VSFVM_LEXERSYM_KEYWORKD("case"),
    VSFVM_LEXERSYM_KEYWORKD("return"),
    VSFVM_LEXERSYM_KEYWORKD("break"),
    VSFVM_LEXERSYM_KEYWORKD("continue"),
    VSFVM_LEXERSYM_FUNCTION("thread", -1),
};

/*============================ PROTOTYPES ====================================*/

static vsf_err_t __vsfvm_dart_parse_token(vsfvm_lexer_t *lexer, vsfvm_pt_t *pt,
    vsfvm_pt_evt_t evt, uint_fast32_t token, vsfvm_token_data_t *data);
static int __vsfvm_dart_input(vsfvm_lexer_t *lexer);

/*============================ GLOBAL VARIABLES ==============================*/

const vsfvm_lexer_op_t vsfvm_lexer_op_dart = {
    .name           = "dart",
    .ext            = "dart",
    .priv_size      = sizeof(vsfvm_lexer_dart_t),

    .keyword.sym    = (vsfvm_lexer_sym_t *)__vsfvm_lexer_dart_keyword,
    .keyword.num    = dimof(__vsfvm_lexer_dart_keyword),
    .keyword.id     = 0,

    .parse_token    = __vsfvm_dart_parse_token,
    .input          = __vsfvm_dart_input,
};

/*============================ IMPLEMENTATION ================================*/

static vsf_err_t __vsfvm_dart_on_expr(vsfvm_lexer_t *lexer,
    uint_fast32_t token, vsfvm_token_data_t *data, bool ref)
{
    if (token == VSFVM_DART_TOKEN_SEMICOLON)        { token = VSFVM_TOKEN_SEMICOLON; }
    else if (token == VSFVM_DART_TOKEN_DOT)         { token = VSFVM_TOKEN_DOT; }
    else if (token == VSFVM_DART_TOKEN_COMMA)       { token = VSFVM_TOKEN_COMMA; }
    else if (token == VSFVM_DART_TOKEN_LGROUPING)   { token = VSFVM_TOKEN_LGROUPING; }
    else if (token == VSFVM_DART_TOKEN_RGROUPING)   { token = VSFVM_TOKEN_RGROUPING; }
    else if (token == VSFVM_DART_TOKEN_ASSIGN)      { token = VSFVM_TOKEN_ASSIGN; }
    else if (token == VSFVM_DART_TOKEN_NUM)         { token = VSFVM_TOKEN_NUM; }
    else if (token == VSFVM_DART_TOKEN_ADD)         { token = VSFVM_TOKEN_ADD; }
    else if (token == VSFVM_DART_TOKEN_SUB)         { token = VSFVM_TOKEN_SUB; }
    else if (token == VSFVM_DART_TOKEN_MUL)         { token = VSFVM_TOKEN_MUL; }
    else if (token == VSFVM_DART_TOKEN_DIV)         { token = VSFVM_TOKEN_DIV; }
    else if (token == VSFVM_DART_TOKEN_AND)         { token = VSFVM_TOKEN_AND; }
    else if (token == VSFVM_DART_TOKEN_OR)          { token = VSFVM_TOKEN_OR; }
    else if (token == VSFVM_DART_TOKEN_XOR)         { token = VSFVM_TOKEN_XOR; }
    else if (token == VSFVM_DART_TOKEN_REV)         { token = VSFVM_TOKEN_REV; }
    else if (token == VSFVM_DART_TOKEN_MOD)         { token = VSFVM_TOKEN_MOD; }
    else if (token == VSFVM_DART_TOKEN_NOT)         { token = VSFVM_TOKEN_NOT; }
    else if (token == VSFVM_DART_TOKEN_GT)          { token = VSFVM_TOKEN_GT; }
    else if (token == VSFVM_DART_TOKEN_LT)          { token = VSFVM_TOKEN_LT; }
    else if (token == VSFVM_DART_TOKEN_EQ)          { token = VSFVM_TOKEN_EQ; }
    else if (token == VSFVM_DART_TOKEN_NE)          { token = VSFVM_TOKEN_NE; }
    else if (token == VSFVM_DART_TOKEN_GE)          { token = VSFVM_TOKEN_GE; }
    else if (token == VSFVM_DART_TOKEN_LE)          { token = VSFVM_TOKEN_LE; }
    else if (token == VSFVM_DART_TOKEN_LAND)        { token = VSFVM_TOKEN_LAND; }
    else if (token == VSFVM_DART_TOKEN_LOR)         { token = VSFVM_TOKEN_LOR; }
    else if (token == VSFVM_DART_TOKEN_SHL)         { token = VSFVM_TOKEN_SHL; }
    else if (token == VSFVM_DART_TOKEN_SHR)         { token = VSFVM_TOKEN_SHR; }
    else if (token & VSFVM_DART_TOKEN_SYM) {
        if (data->sym->type == VSFVM_LEXER_SYM_ALIAS) {
            data->sym = data->sym->sym;
        }

        if (    (data->sym->type == VSFVM_LEXER_SYM_VARIABLE)
            ||  (data->sym->type == VSFVM_LEXER_SYM_EXTVAR)) {
            if (ref) {
                token = VSFVM_TOKEN_VARIABLE_REF;
            } else {
                token = VSFVM_TOKEN_VARIABLE;
            }
        } else if (data->sym->type == VSFVM_LEXER_SYM_CONST) {
            token = VSFVM_TOKEN_NUM;
            data->ival = data->sym->ival;
        } else if ((data->sym->type == VSFVM_LEXER_SYM_FUNCTION)
            ||  (data->sym->type == VSFVM_LEXER_SYM_EXTFUNC)) {
            token = VSFVM_TOKEN_FUNC_CALL;
        } else if (data->sym->type == VSFVM_LEXER_SYM_STRING) {
            token = VSFVM_TOKEN_RESOURCES;
        } else {
            token = VSFVM_TOKEN_SYMBOL;
        }
    } else {
        return -VSFVM_NOT_SUPPORT;
    }

    return vsfvm_lexer_on_expr(lexer, token, data);
}

static bool __vsfvm_dart_is_type(vsfvm_lexer_dart_t *dart,
    uint_fast32_t token, vsfvm_token_data_t *data)
{
    bool istype =   (token >= VSFVM_LEXER_DART_USER_SYMID)
                &&  (data->sym->type == VSFVM_LEXER_SYM_EXTCLASS);
    dart->type_sym = istype ? data->sym : NULL;
    return istype;
}

static vsf_err_t __vsfvm_dart_parse_token(vsfvm_lexer_t *lexer, vsfvm_pt_t *pt,
    vsfvm_pt_evt_t evt, uint_fast32_t token, vsfvm_token_data_t *data)
{
#define dart_next()                                                             \
    do {                                                                        \
        dart->pre_etoken.token = token;                                         \
        if (data != NULL) {                                                     \
            dart->pre_etoken.data = *data;                                      \
        }                                                                       \
        evt = VSFVM_EVT_INVALID;                                                \
        vsfvm_pt_entry(pt);                                                     \
        if (VSFVM_EVT_INVALID == evt){                                          \
            return err;                                                         \
        }                                                                       \
    } while (0)

    vsfvm_lexer_dart_t *dart = (vsfvm_lexer_dart_t *)lexer->curctx.priv;
    vsf_err_t err = VSF_ERR_NOT_READY;

#if VSFVM_LEXER_DEBUG_EN == ENABLED
    if (token & VSFVM_DART_TOKEN_2EQ) {
        vsf_trace_debug("token: %c=" VSF_TRACE_CFG_LINEEND, token & 0xFF);
    } else if (token & VSFVM_DART_TOKEN_DOUBLE) {
        vsf_trace_debug("token: %c%c" VSF_TRACE_CFG_LINEEND, token & 0xFF, token & 0xFF);
    } else if (token & VSFVM_DART_TOKEN_NUM) {
        vsf_trace_debug("token: %d" VSF_TRACE_CFG_LINEEND, data->ival);
    } else if (token & VSFVM_DART_TOKEN_SYM) {
        vsf_trace_debug("token: %s" VSF_TRACE_CFG_LINEEND, data->sym->name + (('"' == data->sym->name[0]) ? 1 : 0));
    } else if (token == '\n') {
        vsf_trace_debug("token: \\n" VSF_TRACE_CFG_LINEEND);
    } else {
        vsf_trace_debug("token: %s" VSF_TRACE_CFG_LINEEND, (char *)&token);
    }
#endif

    vsfvm_pt_begin(pt);

    while (1) {
        if (token == VSFVM_DART_TOKEN_LBRACE) {
            err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_BLOCK_BEGIN, data);
            if (err < 0) { goto dart_error; }
            goto dart_wait_next;
        } else if (token == VSFVM_DART_TOKEN_RBRACE) {
            err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_BLOCK_END, data);
            if (err < 0) { goto dart_error; }
            goto dart_wait_next;
        } else if (token == VSFVM_DART_TOKEN_SYM_IMPORT) {
            dart_next();
            if (token < VSFVM_LEXER_DART_USER_SYMID) {
            dart_invalid_symbol:
                err = -VSFVM_PARSER_UNEXPECTED_TOKEN;
                goto dart_error;
            }

            dart_next();
            if (token != VSFVM_DART_TOKEN_SEMICOLON) {
                err = -VSFVM_PARSER_UNEXPECTED_TOKEN;
                goto dart_error;
            }
            err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_IMPORT, &dart->pre_etoken.data);
            if (err < 0) { goto dart_error; }
            goto dart_wait_next;
        } else if ((token >= VSFVM_DART_TOKEN_SYM_VAR) && (token <= VSFVM_DART_TOKEN_SYM_CONST)) {
            dart->isconst = (token == VSFVM_DART_TOKEN_SYM_CONST);
            dart->type_parsed = false;

        dart_wait_var_name:
            lexer->within_cur_symtbl = true;
            dart_next();
            if (    !dart->type_parsed && __vsfvm_dart_is_type(dart, token, data)
                &&  (dart->pre_etoken.token >= VSFVM_DART_TOKEN_SYM_VAR)) {
                dart_next();
            }
            dart->type_parsed = true;
            lexer->within_cur_symtbl = false;

            if (data->sym->type != VSFVM_LEXER_SYM_UNKNOWN) {
            dart_already_defined_token:
                err = -VSFVM_PARSER_ALREADY_DEFINED;
                goto dart_error;
            }
            if (token < VSFVM_LEXER_DART_USER_SYMID) {
                goto dart_invalid_symbol;
            }
            dart->etoken.data = *data;
            if (dart->type_sym != NULL) {
                dart->etoken.data.sym->c = dart->type_sym->c;
            }

            dart_next();
            if (dart->isconst && (token != VSFVM_DART_TOKEN_ASSIGN)) {
                err = -VSFVM_PARSER_UNINITED_CONST;
                goto dart_error;
            }

            dart->etoken.data.sym->type = dart->isconst ?
                VSFVM_LEXER_SYM_CONST : VSFVM_LEXER_SYM_VARIABLE;
            if (token == VSFVM_DART_TOKEN_ASSIGN) {
                lexer->expr.ctx.comma_is_op = false;
                dart_next();
                err = __vsfvm_dart_on_expr(lexer, token, data, false);
                if (err < 0) {
                    goto dart_error;
                } else if (err > 0) {
                    return err;
                } else {
                    if (dart->isconst) {
                        vsfvm_lexer_etoken_t *etoken;
                        if (lexer->expr.stack_exp.sp != 1) {
                            err = -VSFVM_PARSER_INVALID_CONST;
                            goto dart_expr_error;
                        }
                        etoken = vsfvm_lexer_expr_popexp(lexer);
                        if (!etoken || (etoken->token != VSFVM_TOKEN_NUM)) {
                            err = -VSFVM_PARSER_INVALID_CONST;
                            goto dart_expr_error;
                        }

                        dart->etoken.data.sym->ival = etoken->data.ival;
                        err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_CONST, &dart->etoken.data);
                        if (err < 0) {
                        dart_expr_error:
                            vsfvm_lexer_expr_reset(lexer);
                            goto dart_error;
                        }
                    } else {
                        err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_VAR, &dart->etoken.data);
                        if (err < 0) { goto dart_expr_error; }
                    }
                    vsfvm_lexer_expr_reset(lexer);

                    if (token == VSFVM_DART_TOKEN_COMMA) {
                        goto dart_wait_var_name;
                    } else if (token == VSFVM_DART_TOKEN_SEMICOLON) {
                        goto dart_wait_next;
                    } else {
                        goto dart_unexpected_token;
                    }
                }
            } else if (token == VSFVM_DART_TOKEN_COMMA) {
                if (dart->type_sym) {
                    dart->etoken.data.sym->c = dart->type_sym->c;
                }
                err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_VAR, &dart->etoken.data);
                if (err < 0) { goto dart_error; }
                goto dart_wait_var_name;
            } else if (token == VSFVM_DART_TOKEN_SEMICOLON) {
                if (dart->pre_etoken.token >= VSFVM_LEXER_DART_USER_SYMID) {
                    if (dart->type_sym) {
                        dart->etoken.data.sym->c = dart->type_sym->c;
                    }
                    err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_VAR, &dart->etoken.data);
                    if (err < 0) { goto dart_expr_error; }
                }
                goto dart_wait_next;
            } else {
            dart_unexpected_token:
                err = -VSFVM_PARSER_UNEXPECTED_TOKEN;
                goto dart_error;
            }
        } else if (token == VSFVM_DART_TOKEN_SYM_ALIAS) {
            dart_next();
            if (token < VSFVM_LEXER_DART_USER_SYMID) {
                goto dart_unexpected_token;
            }
            if (data->sym->type != VSFVM_LEXER_SYM_UNKNOWN) {
                goto dart_already_defined_token;
            }
            dart->alias_sym = data->sym;

            dart_next();
            if (token != VSFVM_DART_TOKEN_ASSIGN) {
                goto dart_unexpected_token;
            }

            dart_next();
            if (token < VSFVM_LEXER_DART_USER_SYMID) {
                goto dart_unexpected_token;
            }
            dart->alias_sym->type = VSFVM_LEXER_SYM_ALIAS;
            dart->alias_sym->sym = data->sym;

            dart_next();
            if (token != VSFVM_DART_TOKEN_SEMICOLON) {
                goto dart_unexpected_token;
            }
            goto dart_wait_next;
        } else if ((token == VSFVM_DART_TOKEN_SYM_IF)
            ||  (token == VSFVM_DART_TOKEN_SYM_WHILE)) {
            dart->etoken.token = token == VSFVM_DART_TOKEN_SYM_IF ?
                VSFVM_TOKEN_IF : VSFVM_TOKEN_WHILE;

            dart_next();
            if (token != VSFVM_DART_TOKEN_LGROUPING) {
                goto dart_unexpected_token;
            }

            lexer->expr.ctx.comma_is_op = true;
            dart_next();
            dart->isref = false;
            if (token == VSFVM_DART_TOKEN_REF) {
                dart_next();
                if (    (token < VSFVM_LEXER_DART_USER_SYMID)
                    ||  (data->sym->type != VSFVM_LEXER_SYM_VARIABLE)) {
                    goto dart_unexpected_token;
                }
                dart->isref = true;
            }
            err = __vsfvm_dart_on_expr(lexer, token, data, dart->isref);
            if (err < 0) {
                goto dart_error;
            } else if (err > 0) {
                return err;
            } else if (token != VSFVM_DART_TOKEN_RGROUPING) {
            dart_expr_unwanted_token:
                vsfvm_lexer_expr_reset(lexer);
                goto dart_unexpected_token;
            }

            dart_next();
            if (token != VSFVM_DART_TOKEN_LBRACE) {
                goto dart_expr_unwanted_token;
            }

            err = vsfvm_lexer_on_stmt(lexer, dart->etoken.token, NULL);
            if (err < 0) { goto dart_expr_error; }
            err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_BLOCK_BEGIN, data);
            if (err < 0) { goto dart_expr_error; }
        dart_expr_finished:
            vsfvm_lexer_expr_reset(lexer);
            goto dart_wait_next;
        } else if (token == VSFVM_DART_TOKEN_SYM_ELSE) {
            err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_ELSE, NULL);
            if (err < 0) { goto dart_error; }

            dart_next();
            if ((token == VSFVM_DART_TOKEN_SYM_IF) || (token == VSFVM_DART_TOKEN_LBRACE)) {
                continue;
            }
            goto dart_unexpected_token;
        } else if (token == VSFVM_DART_TOKEN_SYM_RETURN) {
            lexer->expr.ctx.comma_is_op = true;
            dart_next();
            dart->isref = false;
            if (token == VSFVM_DART_TOKEN_REF) {
                dart_next();
                if (    (token < VSFVM_LEXER_DART_USER_SYMID)
                    ||  (data->sym->type != VSFVM_LEXER_SYM_VARIABLE)) {
                    goto dart_unexpected_token;
                }
                dart->isref = true;
            }
            err = __vsfvm_dart_on_expr(lexer, token, data, dart->isref);
            if (err < 0) {
                goto dart_error;
            } else if (err > 0) {
                return err;
            } else if (token != VSFVM_DART_TOKEN_SEMICOLON) {
                goto dart_expr_unwanted_token;
            }

            err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_RET, NULL);
            if (err < 0) { goto dart_expr_error; }
            goto dart_expr_finished;
        } else if ((token == VSFVM_DART_TOKEN_SYM_FOR)
            ||  (token == VSFVM_DART_TOKEN_SYM_DO)
            ||  (token == VSFVM_DART_TOKEN_SYM_BREAK)
            ||  (token == VSFVM_DART_TOKEN_SYM_CONTINUE)) {
            err = -VSFVM_NOT_SUPPORT;
            goto dart_error;
        } else if (token & VSFVM_DART_TOKEN_SYM) {
            if (data->sym->type == VSFVM_LEXER_SYM_UNKNOWN) {
                dart_next();
                if (token != VSFVM_DART_TOKEN_LGROUPING) {
                    goto dart_unexpected_token;
                }

                dart->etoken.data = dart->pre_etoken.data;
                dart->etoken.data.sym->type = VSFVM_LEXER_SYM_FUNCTION;
                vsfvm_lexer_symtbl_new(lexer);

                while (1) {
                    dart_next();
                    if (token == VSFVM_DART_TOKEN_RGROUPING) {
                        if (    (dart->pre_etoken.token < VSFVM_LEXER_DART_USER_SYMID)
                            &&  (dart->pre_etoken.token != VSFVM_DART_TOKEN_LGROUPING)) {
                            goto dart_unexpected_token;
                        }

                        dart_next();
                        if (token == VSFVM_DART_TOKEN_SEMICOLON) {
                            // function declare, not supported
                            // function MUST be implemented before use
                            vsfvm_lexer_symtbl_delete(lexer);
                            vsfvm_lexer_expr_reset(lexer);
                            goto dart_unexpected_token;
                        } else if (token == VSFVM_DART_TOKEN_LBRACE) {
                            dart->etoken.data.sym->func.param_num = lexer->expr.stack_exp.sp;
                            err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_FUNC, &dart->etoken.data);
                            if (err < 0) { goto dart_expr_error; }
                            err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_BLOCK_BEGIN, NULL);
                            if (err < 0) { goto dart_expr_error; }
                            vsfvm_lexer_symtbl_delete(lexer);
                            goto dart_expr_finished;
                        } else {
                            goto dart_unexpected_token;
                        }
                    } else if (token == VSFVM_DART_TOKEN_COMMA) {
                        if (dart->pre_etoken.token < VSFVM_LEXER_DART_USER_SYMID) {
                            goto dart_unexpected_token;
                        }
                    } else if (__vsfvm_dart_is_type(dart, token, data)) {
                        if (    (dart->pre_etoken.token != VSFVM_DART_TOKEN_LGROUPING)
                            &&  (dart->pre_etoken.token != VSFVM_DART_TOKEN_COMMA)) {
                            goto dart_unexpected_token;
                        }
                    } else if (token >= VSFVM_LEXER_DART_USER_SYMID) {
                        if (    (dart->pre_etoken.token != VSFVM_DART_TOKEN_LGROUPING)
                            &&  (dart->pre_etoken.token != VSFVM_DART_TOKEN_COMMA)
                            &&  !__vsfvm_dart_is_type(dart, dart->pre_etoken.token, &dart->pre_etoken.data)) {
                            goto dart_unexpected_token;
                        }
                        data->sym->type = VSFVM_LEXER_SYM_VARIABLE;
                        if (dart->type_sym) {
                            data->sym->c = dart->type_sym->c;
                        }
                        vsfvm_lexer_expr_pushexp(lexer, VSFVM_TOKEN_VARIABLE, data);
                    } else {
                        goto dart_unexpected_token;
                    }
                }
            } else {
                goto dart_expr;
            }
        } else {
        dart_expr:
            lexer->expr.ctx.comma_is_op = true;
            while (1) {
                dart->isref = false;
                if (token == VSFVM_DART_TOKEN_REF) {
                    dart_next();
                    if (    (token < VSFVM_LEXER_DART_USER_SYMID)
                        ||  (data->sym->type != VSFVM_LEXER_SYM_VARIABLE)) {
                        goto dart_unexpected_token;
                    }
                    dart->isref = true;
                }
                err = __vsfvm_dart_on_expr(lexer, token, data, dart->isref);
                if (err < 0) {
                    goto dart_error;
                } else if (!err) {
                    if (token != VSFVM_DART_TOKEN_SEMICOLON) {
                        err = -VSFVM_PARSER_UNEXPECTED_TOKEN;
                        goto dart_expr_error;
                    }
                    err = vsfvm_lexer_on_stmt(lexer, VSFVM_TOKEN_EXPR, NULL);
                    if (err < 0) { goto dart_expr_error; }
                    goto dart_expr_finished;
                } else {
                    dart_next();
                }
            }
        }

    dart_error:
        dart->parse_enter = true;
        while (token != VSFVM_DART_TOKEN_NL) {
            dart_next();
        }
        dart->parse_enter = false;
        return VSF_ERR_FAIL;

    dart_wait_next:
        err = VSF_ERR_NONE;
        dart_next();
    }

    vsfvm_pt_end(pt);
    return VSF_ERR_NONE;
}

static bool __vsfvm_dart_is_space(int ch)
{
    return isspace(ch);
}

static int __vsfvm_dart_get_escape_char(char ch)
{
    switch (ch) {
    case 'a':   return '\a';
    case 'b':   return '\b';
    case 'f':   return '\f';
    case 'n':   return '\n';
    case 'r':   return '\r';
    case 't':   return '\t';
    case 'v':   return '\v';
    case '\\':  return '\\';
    case '\'':  return '\'';
    case '\"':  return '\"';
    case '?':   return '\?';
    case '0':   return '\0';
    }
    return (char)0xFF;
}

static int __vsfvm_dart_input(vsfvm_lexer_t *lexer)
{
    const char *vsfvm_dart_token_1char = ",;()[]{}?:.@";
    const char *vsfvm_dart_token_double = "+-|&<>";         // ?? not supported
    const char *vsfvm_dart_token_2eq = "+-*/^%|&<>=!";

    vsfvm_lexer_dart_t *dart = (vsfvm_lexer_dart_t *)lexer->curctx.priv;
    vsfvm_token_data_t data;
    uint_fast32_t token, token_next;
    uint_fast32_t value;
    uint16_t id;
    int err = 0;

    while ((token = vsfvm_lexer_getchar(lexer))) {
        token_next = vsfvm_lexer_peekchar(lexer);
        if (token == '\r') {
        } else if (token == '\n') {
            if (dart->parse_enter) {
                dart->parse_enter = false;
                err = vsfvm_lexer_on_token(lexer, token, NULL);
                if (err < 0) { return err; }
            }
        } else if (strchr(vsfvm_dart_token_1char, token)) {
            err = vsfvm_lexer_on_token(lexer, token, NULL);
            if (err < 0) { return err; }
        } else if ( ((token >= 'a') && (token <= 'z'))
                ||  ((token >= 'A') && (token <= 'Z'))
                ||  (token == '_')) {
            value = 0;
            lexer->cur_symbol[value++] = (char)token;
            while ( ((token_next >= 'a') && (token_next <= 'z'))
                ||  ((token_next >= 'A') && (token_next <= 'Z'))
                ||  ((token_next >= '0') && (token_next <= '9'))
                ||  (token_next == '_')) {
                token = vsfvm_lexer_getchar(lexer);
                token_next = vsfvm_lexer_peekchar(lexer);
                lexer->cur_symbol[value++] = (char)token;
                if (value >= sizeof(lexer->cur_symbol)) {
                    return -VSFVM_LEXER_SYMBOL_TOO_LONG;
                }
            }
            lexer->cur_symbol[value++] = '\0';

            data.sym = vsfvm_lexer_symtbl_add_symbol(lexer, lexer->cur_symbol, &id);
            if (!data.sym) { return -VSFVM_NOT_ENOUGH_RESOURCES; }

            err = vsfvm_lexer_on_token(lexer, VSFVM_DART_TOKEN_SYM | id, &data);
            if (err < 0) { return err; }
        } else if ((token >= '0') && (token <= '9')) {
            data.ival = 0;
            if ((token == '0') && ((token_next == 'x') || (token_next == 'X'))) {
                vsfvm_lexer_getchar(lexer);
                token_next = vsfvm_lexer_peekchar(lexer);

                while ( ((token_next >= '0') && (token_next <= '9'))
                   ||   ((token_next >= 'a') && (token_next <= 'f'))
                   ||   ((token_next >= 'A') && (token_next <= 'F'))) {
                    token = vsfvm_lexer_getchar(lexer);
                    token_next = vsfvm_lexer_peekchar(lexer);
                    data.ival <<= 4;
                    data.ival += (token & 15) + (token >= 'A' ? 9 : 0);
                }
            } else if ((token == '0') && ((token_next == 'b') || (token_next == 'B'))) {
                vsfvm_lexer_getchar(lexer);
                token_next = vsfvm_lexer_peekchar(lexer);

                while ((token_next >= '0') && (token_next <= '1')) {
                    token = vsfvm_lexer_getchar(lexer);
                    token_next = vsfvm_lexer_peekchar(lexer);
                    data.ival <<= 1;
                    data.ival += token - '0';
                }
            } else {
                data.ival = token - '0';
                while ((token_next >= '0') && (token_next <= '9')) {
                    token = vsfvm_lexer_getchar(lexer);
                    token_next = vsfvm_lexer_peekchar(lexer);
                    data.ival *= 10;
                    data.ival += token - '0';
                }
            }

            err = vsfvm_lexer_on_token(lexer, VSFVM_DART_TOKEN_NUM, &data);
            if (err < 0) { return err; }
        } else if (token == '/') {
            if (token_next == '/') {
                while ((token != '\n') && (token != '\0')) {
                    token = vsfvm_lexer_getchar(lexer);
                }
            } else if (token_next == '*') {
                vsfvm_lexer_getchar(lexer);
                while (!((token == '*') && (token_next == '/'))) {
                    token = vsfvm_lexer_getchar(lexer);
                    token_next = vsfvm_lexer_peekchar(lexer);
                }
                vsfvm_lexer_getchar(lexer);
            } else {
                goto parse_2eq;
            }
        } else if (token == '\'') {
            token_next = vsfvm_lexer_getchar(lexer);
            if (token_next == '\\') {
                if (token_next == '\0')         goto dart_unclosed_string;
                else {
                    int ch = __vsfvm_dart_get_escape_char((char)token_next);
                    if (ch < 0) {
                        return -VSFVM_LEXER_INVALID_ESCAPE;
                    }
                    token_next = ch;
                }
            }
            data.ival = token_next;
            token_next = vsfvm_lexer_getchar(lexer);
            if (token_next != '\'') {
                return -VSFVM_LEXER_INVALID_CHAR;
            }

            err = vsfvm_lexer_on_token(lexer, VSFVM_DART_TOKEN_NUM, &data);
            if (err < 0) { return err; }
        } else if (token == '"') {
            value = 0;
            // string symbol start with "
            lexer->cur_symbol[value++] = '"';
            while (1) {
                token_next = vsfvm_lexer_getchar(lexer);
                if (token_next == token) {
                    token_next = vsfvm_lexer_peekchar(lexer);
                    while (__vsfvm_dart_is_space(token_next)) {
                        vsfvm_lexer_getchar(lexer);
                        token_next = vsfvm_lexer_peekchar(lexer);
                    }
                    if (token_next == token) {
                        vsfvm_lexer_getchar(lexer);
                        continue;
                    }
                    break;
                } else if (token_next == '\\') {
                check_next:
                    token_next = vsfvm_lexer_getchar(lexer);
                    if      (token_next == '\0')    goto dart_unclosed_string;
                    else if (token_next == '\r')    goto check_next;
                    else if (token_next == '\n')    continue;
                    else {
                        int ch = __vsfvm_dart_get_escape_char((char)token_next);
                        if (ch < 0) {
                            return -VSFVM_LEXER_INVALID_ESCAPE;
                        }
                        token_next = ch;
                    }
                } else if ((token_next == '\r') || (token_next == '\n')) {
                    continue;
                } else if (token_next == '\0') {
                dart_unclosed_string:
                    return -VSFVM_LEXER_INVALID_STRING;
                }
                lexer->cur_symbol[value++] = (char)token_next;
            }
            lexer->cur_symbol[value++] = '\0';

            data.sym = vsfvm_lexer_symtbl_add_symbol(lexer, lexer->cur_symbol, &id);
            if (!data.sym) { return -VSFVM_NOT_ENOUGH_RESOURCES; }
            data.sym->type = VSFVM_LEXER_SYM_STRING;
            data.sym->length = strlen(data.sym->name) + 1;

            err = vsfvm_lexer_on_token(lexer, VSFVM_DART_TOKEN_SYM | id, &data);
            if (err < 0) { return err; }
        } else if (strchr(vsfvm_dart_token_double, token)) {
            if (token == token_next) {
                vsfvm_lexer_getchar(lexer);
                token |= VSFVM_DART_TOKEN_DOUBLE;
                if (    ((token == '<') || (token == '>'))
                    &&  (vsfvm_lexer_peekchar(lexer) == '=')) {
                    token |= VSFVM_DART_TOKEN_2EQ;
                    err = vsfvm_lexer_on_token(lexer, token, NULL);
                } else {
                    err = vsfvm_lexer_on_token(lexer, token, NULL);
                }
                if (err < 0) { return err; }
            } else {
                goto parse_2eq;
            }
        } else if (strchr(vsfvm_dart_token_2eq, token)) {
        parse_2eq:
            if (token_next == '=') {
                vsfvm_lexer_getchar(lexer);
                token |= VSFVM_DART_TOKEN_2EQ;
            }

            err = vsfvm_lexer_on_token(lexer, token, NULL);
            if (err < 0) { return err; }
        } else if (token == '~') {
            // ~ ~/ ~/=
            if (token_next != '/') {
                err = vsfvm_lexer_on_token(lexer, token, NULL);
                if (err < 0) { return err; }
            } else {
                vsfvm_lexer_getchar(lexer);
                token_next = vsfvm_lexer_peekchar(lexer);
                if (token_next != '=') {
                    err = vsfvm_lexer_on_token(lexer, VSFVM_DART_TOKEN_INTDIV, NULL);
                    if (err < 0) { return err; }
                } else {
                    vsfvm_lexer_getchar(lexer);
                    err = vsfvm_lexer_on_token(lexer, VSFVM_DART_TOKEN_INTDIV | VSFVM_DART_TOKEN_2EQ, NULL);
                    if (err < 0) { return err; }
                }
            }
        }
    }
    return err;
}

#endif      // VSFVM_CFG_COMPILER_EN
