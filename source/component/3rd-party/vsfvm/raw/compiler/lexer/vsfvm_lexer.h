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

#ifndef __VSFVM_LEXER_H__
#define __VSFVM_LEXER_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_vm_cfg.h"

#if VSFVM_CFG_COMPILER_EN == ENABLED

#include "../../common/vsfvm_common.h"
#include "service/vsf_service.h"

/*============================ MACROS ========================================*/

#ifndef VSFVM_LEXER_MAX_SYMLEN
#   define VSFVM_LEXER_MAX_SYMLEN       256
#endif

#define VSFVM_LEXER_USER_SYMID          0x100

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfvm_lexer_sym_t vsfvm_lexer_sym_t;
typedef struct vsfvm_lexer_op_t vsfvm_lexer_op_t;

typedef struct vsfvm_lexer_symarr_t {
    vsfvm_lexer_sym_t *sym;
    uint16_t num;
    uint16_t id;
} vsfvm_lexer_symarr_t;

typedef struct vsfvm_lexer_symtbl_t {
    vsf_dynarr_t table;
    vsf_dynarr_t strpool;
    char *strbuf;
    char *strpos;
    uint8_t varnum;
} vsfvm_lexer_symtbl_t;

// expression token
typedef union vsfvm_token_data_t {
    uint32_t uval;
    int32_t ival;
    vsfvm_lexer_sym_t *sym;
} vsfvm_token_data_t;

typedef struct vsfvm_lexer_etoken_t {
    uint32_t token;
    vsfvm_token_data_t data;
} vsfvm_lexer_etoken_t;

typedef struct vsfvm_lexer_exprctx_t {
    vsfvm_lexer_etoken_t pre_etoken;
    bool comma_is_op;
    uint8_t func_param;
    uint32_t opsp;
    uint32_t expsp;
    vsfvm_pt_t pt;
} vsfvm_lexer_exprctx_t;

typedef struct vsfvm_lexer_expr_t {
    vsfvm_lexer_exprctx_t ctx;
    uint32_t nesting;

    vsf_dynstack_t stack_exp;
    vsf_dynstack_t stack_op;
} vsfvm_lexer_expr_t;

struct vsfvm_lexer_sym_t {
    char *name;

    union {
        uint16_t id;
        const vsfvm_ext_op_t *ext_op;
    };

    enum {
        VSFVM_LEXER_SYM_UNKNOWN = 0,
        VSFVM_LEXER_SYM_GRAMA,      // keywords in extensions
        VSFVM_LEXER_SYM_KEYWORD,

        VSFVM_LEXER_SYM_FUNCTION,
        VSFVM_LEXER_SYM_EXTFUNC,
        VSFVM_LEXER_SYM_EXTCLASS,
        VSFVM_LEXER_SYM_STRING,

        VSFVM_LEXER_SYM_OPRAND,
        VSFVM_LEXER_SYM_VARIABLE,
        VSFVM_LEXER_SYM_CONST,
        VSFVM_LEXER_SYM_EXTVAR,

        VSFVM_LEXER_SYM_ALIAS,
    } type;

    union {
        int32_t ival;
        uint32_t uval;
        uint32_t length;            // for resources(string)
        struct {
            signed pos : 16;
            signed param_num : 8;
            const vsfvm_class_t *retc;
        } func;                     // for functions
        struct {
            uint16_t pos;
        } var;                      // for vars
        vsfvm_lexer_sym_t *sym;     // for alias
    };
    const vsfvm_class_t *c;

    vsf_slist_node_t symbol_node;
};

typedef struct vsfvm_lexer_ctx_t {
    const vsfvm_lexer_op_t *op;

    const char *pos;
    int line;
    int col;

    vsfvm_pt_t pt;
    void *priv;
} vsfvm_lexer_ctx_t;

typedef struct vsfvm_lexer_t {
    vsfvm_lexer_ctx_t curctx;
    vsf_dynstack_t ctx_stack;

    vsf_dynstack_t symtbl_stack;
    vsfvm_lexer_expr_t expr;
    char cur_symbol[VSFVM_LEXER_MAX_SYMLEN];
    int symid;
    bool within_cur_symtbl;
} vsfvm_lexer_t;

struct vsfvm_lexer_op_t {
    const char *name;
    const char *ext;
    uint16_t priv_size;

    const vsfvm_lexer_symarr_t keyword;

    vsf_err_t (*parse_token)(vsfvm_lexer_t *lexer, vsfvm_pt_t *pt, vsfvm_pt_evt_t evt, uint_fast32_t token, vsfvm_token_data_t *data);
    int (*init)(vsfvm_lexer_t *lexer);
    int (*input)(vsfvm_lexer_t *lexer);
};

typedef struct vsfvm_lexer_list_t {
    const vsfvm_lexer_op_t *op;
    vsf_slist_node_t op_node;
} vsfvm_lexer_list_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

// for specified lexer
#define vsfvm_lexer_peekchar(lexer)        (*(lexer)->curctx.pos)
extern int vsfvm_lexer_getchar(vsfvm_lexer_t *lexer);

extern int vsfvm_lexer_on_token(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data);
extern vsfvm_lexer_sym_t * vsfvm_lexer_symtbl_get(vsfvm_lexer_t *lexer, char *symbol, uint16_t *id);
extern vsfvm_lexer_sym_t * vsfvm_lexer_symtbl_add_symbol(vsfvm_lexer_t *lexer, char *symbol, uint16_t *id);
extern int vsfvm_lexer_symtbl_new(vsfvm_lexer_t *lexer);
extern int vsfvm_lexer_symtbl_delete(vsfvm_lexer_t *lexer);

// expression
extern int vsfvm_lexer_expr_pushexp(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data);
extern int vsfvm_lexer_expr_pushop(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data);
extern vsfvm_lexer_etoken_t * vsfvm_lexer_expr_popexp(vsfvm_lexer_t *lexer);
extern vsfvm_lexer_etoken_t * vsfvm_lexer_expr_popop(vsfvm_lexer_t *lexer);
extern vsf_err_t vsfvm_lexer_on_expr(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data);
extern vsf_err_t vsfvm_lexer_on_stmt(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data);
extern int vsfvm_lexer_expr_reset(vsfvm_lexer_t *lexer);

// for compiler
extern int vsfvm_lexer_init(vsfvm_lexer_t *lexer, vsfvm_lexer_list_t *list);
extern void vsfvm_lexer_fini(vsfvm_lexer_t *lexer);
extern int vsfvm_lexer_input(vsfvm_lexer_t *lexer, const char *code);

extern int vsfvm_lexer_ctx_new(vsfvm_lexer_t *lexer, vsfvm_lexer_list_t *list);
extern int vsfvm_lexer_ctx_delete(vsfvm_lexer_t *lexer);

// from vsfvm_compiler.c
extern vsf_err_t vsfvm_on_stmt(vsfvm_lexer_t *lexer, uint_fast32_t token, vsfvm_token_data_t *data);

#endif      // VSFVM_CFG_COMPILER_EN
#endif      // __VSFVM_LEXER_H__
