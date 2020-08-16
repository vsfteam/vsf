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

#ifndef __VSFVM_COMPILER_H__
#define __VSFVM_COMPILER_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_vm_cfg.h"

#if VSFVM_CFG_COMPILER_EN == ENABLED

#include "../common/vsfvm_common.h"
#include "./lexer/vsfvm_lexer.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum vsfvm_compiler_token_t {
    VSFVM_TOKEN_NONE = 0,
    VSFVM_TOKEN_SYMBOL,

    VSFVM_TOKEN_STMT_START,
    VSFVM_TOKEN_IMPORT,
    VSFVM_TOKEN_VAR,                    // define a variable
    VSFVM_TOKEN_FUNC,                   // define a function
    VSFVM_TOKEN_CONST,
    VSFVM_TOKEN_IF,
    VSFVM_TOKEN_ELSE,
    VSFVM_TOKEN_WHILE,
    VSFVM_TOKEN_RET,
    VSFVM_TOKEN_BREAK,
    VSFVM_TOKEN_CONTINUE,
    VSFVM_TOKEN_BLOCK_BEGIN,
    VSFVM_TOKEN_BLOCK_END,
    VSFVM_TOKEN_EXPR,
    VSFVM_TOKEN_STMT_END,

    VSFVM_TOKEN_EXPR_START,
    VSFVM_TOKEN_EXPR_TERMINATOR,        // to terminate a expression
    VSFVM_TOKEN_SEMICOLON,
    VSFVM_TOKEN_RGROUPING,
    VSFVM_TOKEN_COMMA,
    VSFVM_TOKEN_EXPR_TERMINATOR_END,
    VSFVM_TOKEN_LGROUPING,

    VSFVM_TOKEN_EXPR_OPERAND,
    VSFVM_TOKEN_EXPR_OPERAND_CONST,
    VSFVM_TOKEN_NUM,
    VSFVM_TOKEN_RESOURCES,
    VSFVM_TOKEN_FUNC_ID,                // use a function as a value
    VSFVM_TOKEN_EXPR_OPERAND_VARIABLE,
    VSFVM_TOKEN_VARIABLE,               // from lexer
    VSFVM_TOKEN_VARIABLE_REF,
    VSFVM_TOKEN_VAR_ID = VSFVM_TOKEN_VARIABLE,
    VSFVM_TOKEN_VAR_ID_REF = VSFVM_TOKEN_VARIABLE_REF,
                                        // use a variable

    VSFVM_TOKEN_OPERATOR,
    VSFVM_TOKEN_OP_PRIO1 = 0x100,
    VSFVM_TOKEN_FUNC_CALL,
    VSFVM_TOKEN_DOT,
    VSFVM_TOKEN_REF,

    VSFVM_TOKEN_UNARY_OP,               // unary op
    VSFVM_TOKEN_OP_PRIO2 = 0x200,
    VSFVM_TOKEN_NOT,
    VSFVM_TOKEN_REV,
    VSFVM_TOKEN_NEGA,                   // negative sign
    VSFVM_TOKEN_POSI,                   // positive sign
    VSFVM_TOKEN_SIZEOF,
    VSFVM_TOKEN_ADDR,
    VSFVM_TOKEN_PTR,

    VSFVM_TOKEN_BINARY_OP,              // binary operator
    VSFVM_TOKEN_OP_PRIO3 = 0x300,
    VSFVM_TOKEN_MUL,
    VSFVM_TOKEN_DIV,
    VSFVM_TOKEN_MOD,

    VSFVM_TOKEN_OP_PRIO4 = 0x400,
    VSFVM_TOKEN_ADD,
    VSFVM_TOKEN_SUB,

    VSFVM_TOKEN_OP_PRIO5 = 0x500,
    VSFVM_TOKEN_SHL,
    VSFVM_TOKEN_SHR,

    VSFVM_TOKEN_OP_PRIO6 = 0x600,
    VSFVM_TOKEN_LT,
    VSFVM_TOKEN_LE,
    VSFVM_TOKEN_GT,
    VSFVM_TOKEN_GE,

    VSFVM_TOKEN_OP_PRIO7 = 0x700,
    VSFVM_TOKEN_EQ,
    VSFVM_TOKEN_NE,

    VSFVM_TOKEN_OP_PRIO8 = 0x800,
    VSFVM_TOKEN_AND,

    VSFVM_TOKEN_OP_PRIO9 = 0x900,
    VSFVM_TOKEN_XOR,

    VSFVM_TOKEN_OP_PRIO10 = 0xA00,
    VSFVM_TOKEN_OR,

    VSFVM_TOKEN_OP_PRIO11 = 0xB00,
    VSFVM_TOKEN_LAND,

    VSFVM_TOKEN_OP_PRIO12 = 0xC00,
    VSFVM_TOKEN_LOR,

    VSFVM_TOKEN_OP_PRIO13 = 0xD00,
    VSFVM_TOKEN_COND,
    VSFVM_TOKEN_COLON,

    VSFVM_TOKEN_OP_PRIO14 = 0xE00,
    VSFVM_TOKEN_ASSIGN,

    VSFVM_TOKEN_OP_PRIO15 = 0xF00,
    VSFVM_TOKEN_COMMA_OP = VSFVM_TOKEN_COMMA + VSFVM_TOKEN_OP_PRIO15,

    VSFVM_TOKEN_OP_PRIO16 = 0x1000,
    VSFVM_TOKEN_OP_PRIO_MAX = VSFVM_TOKEN_OP_PRIO16,
    VSFVM_TOKEN_OP_PRIO_MASK = 0xFF00,

    VSFVM_TOKEN_EXPR_END,
};

enum vsfvm_compiler_errcode_t {
    VSFVM_ERRCODE_NONE = 0,

    // common error
    VSFVM_BUG,
    VSFVM_BYTECODE_TOOLONG,
    VSFVM_NOT_ENOUGH_RESOURCES,
    VSFVM_FATAL_ERROR,              // fatal error above
    VSFVM_NOT_SUPPORT,

    // lexer error
    VSFVM_LEXER_NOT_SUPPORT,
    VSFVM_LEXER_INVALID_OP,
    VSFVM_LEXER_INVALID_CHAR,
    VSFVM_LEXER_INVALID_STRING,
    VSFVM_LEXER_INVALID_ESCAPE,
    VSFVM_LEXER_SYMBOL_TOO_LONG,

    // parser error
    VSFVM_PARSER_UNEXPECTED_TOKEN,
    VSFVM_PARSER_ALREADY_DEFINED,
    VSFVM_PARSER_INVALID_CLOSURE,
    VSFVM_PARSER_INVALID_EXPR,
    VSFVM_PARSER_UNINITED_CONST,
    VSFVM_PARSER_INVALID_CONST,
    VSFVM_PARSER_DIV0,
    VSFVM_PARSER_EXPECT_FUNC_PARAM,
    VSFVM_PARSER_TOO_MANY_FUNC_PARAM,
    VSFVM_PARSER_MEMFUNC_NOT_FOUND,

    // compiler error
    VSFVM_COMPILER_INVALID_MODULE,
    VSFVM_COMPILER_INVALID_FUNC,
    VSFVM_COMPILER_INVALID_FUNC_PARAM,
    VSFVM_COMPILER_FAIL_USRLIB,

    VSFVM_ERRCODE_END,
};

typedef struct vsfvm_compiler_func_ctx_t {
    vsfvm_lexer_etoken_t etoken;
    int symtbl_idx;
    int block_level;
    union {
        struct {
            int if_anchor;
            vsfvm_bytecode_t if_code;
            int else_anchor;
            vsfvm_bytecode_t else_code;
        } if_ctx;
        struct {
            int if_anchor;
            vsfvm_bytecode_t if_code;
            int calc_anchor;
        } while_ctx;
        struct {
            int goto_anchor;
            vsfvm_bytecode_t goto_code;
        } func_ctx;
    };
} vsfvm_compiler_func_ctx_t;

typedef struct vsfvm_linktbl_t {
    int bytecode_pos;
    uint32_t token;
    enum {
        VSFVM_LINKTBL_STR,
    } type;
    vsfvm_lexer_sym_t *sym;
} vsfvm_linktbl_t;

typedef struct vsfvm_compiler_func_t {
    vsfvm_lexer_sym_t *name;
    int symtbl_idx;
    int block_level;

    vsf_slist_t arglist;
    vsf_slist_t varlist;

    vsf_dynstack_t linktbl;
    vsfvm_compiler_func_ctx_t curctx;
    vsf_dynstack_t ctx;
} vsfvm_compiler_func_t;

typedef struct vsfvm_compiler_script_t {
    const char *name;
    vsfvm_lexer_t lexer;
    vsf_slist_t ext;

    vsfvm_pt_t pt_stmt;
    vsfvm_compiler_func_t cur_func;

    vsf_dynstack_t func_stack;
} vsfvm_compiler_script_t;

typedef struct vsfvm_compiler_t {
    vsfvm_compiler_script_t script;
    uint32_t bytecode_pos;
    vsf_slist_t lexer_list;
} vsfvm_compiler_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int vsfvm_compiler_register_lexer(vsfvm_compiler_t *compiler, vsfvm_lexer_list_t *lexer_list);
extern int vsfvm_compiler_set_script(vsfvm_compiler_t *compiler, const char *script_name);
extern int vsfvm_compiler_init(vsfvm_compiler_t *compiler);
extern void vsfvm_compiler_fini(vsfvm_compiler_t *compiler);
extern int vsfvm_compiler_input(vsfvm_compiler_t *compiler, const char *code);

#endif      // VSFVM_CFG_COMPILER_EN
#endif      // __VSFVM_COMPILER_H__
