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

#ifndef __VSFVM_BYTECODE_H__
#define __VSFVM_BYTECODE_H__

/*============================ INCLUDES ======================================*/

// for stdint.h
#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/

// bytecode layout:
// include
// code
// resources

// bit0..15:    arg16
// bit0..23:    arg24
// bit16..23:   arg8
// bit0..28:    signed value
// bit24..28:   id
// bit29..31:   type
#define VSFVM_CODE_LENGTH                   29

#define VSFVM_CODE(type, num)               ((vsfvm_bytecode_t)(((uint32_t)(type) << VSFVM_CODE_LENGTH) + ((num) & ~(0xFFFFFFFF << VSFVM_CODE_LENGTH))))
#define VSFVM_SYMBOL(symbol, arg8, arg16)   VSFVM_CODE(VSFVM_CODE_TYPE_SYMBOL, ((symbol) << 24) | ((uint8_t)(arg8) << 16) | ((uint16_t)(arg16) << 0))
#define VSFVM_KEYWORD(keyword, arg8, arg16) VSFVM_CODE(VSFVM_CODE_TYPE_KEYWORD, ((keyword) << 24) | ((uint8_t)(arg8) << 16) | ((uint16_t)(arg16) << 0))
#define VSFVM_NUMBER(value)                 VSFVM_CODE(VSFVM_CODE_TYPE_NUMBER, (value))
#define VSFVM_VARIABLE(type, pos, idx)      VSFVM_CODE(VSFVM_CODE_TYPE_VARIABLE, ((type) << 24) | ((uint8_t)(pos) << 16) | ((uint16_t)(idx) << 0))
#define VSFVM_FUNCTION(type, argc, pos)     VSFVM_CODE(VSFVM_CODE_TYPE_FUNCTION, ((type) << 24) | ((uint8_t)(argc) << 16) | ((uint16_t)(pos) << 0))
#define VSFVM_EOF()                         VSFVM_CODE(VSFVM_CODE_TYPE_EOF, 0)

#define VSFVM_CODE_TYPE(code)               ((uint32_t)(code) >> VSFVM_CODE_LENGTH)
#define VSFVM_CODE_VALUE(code)              (((int32_t)(code) << (32 - VSFVM_CODE_LENGTH)) >> (32 - VSFVM_CODE_LENGTH))
#define VSFVM_CODE_ID(code)                 (((uint32_t)(code) >> 24) & ((1UL << (VSFVM_CODE_LENGTH - 24)) - 1))
#define VSFVM_CODE_ARG8(code)               (((uint32_t)(code) >> 16) & 0xFF)
#define VSFVM_CODE_ARG16(code)              (((uint32_t)(code) >> 0) & 0xFFFF)
#define VSFVM_CODE_ARG24(code)              (((uint32_t)(code) >> 0) & 0xFFFFFF)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef uint32_t vsfvm_bytecode_t;

enum VSFVM_CODE_TYPE_t {
    VSFVM_CODE_TYPE_SYMBOL = 0,
    VSFVM_CODE_TYPE_KEYWORD,
    VSFVM_CODE_TYPE_NUMBER,
    VSFVM_CODE_TYPE_VARIABLE,
    VSFVM_CODE_TYPE_FUNCTION,
    VSFVM_CODE_TYPE_EOF = 0x7,
};
typedef enum VSFVM_CODE_TYPE_t VSFVM_CODE_TYPE_t;

enum VSFVM_CODE_FUNCTION_ID_t {
    VSFVM_CODE_FUNCTION_SCRIPT = 0,
    VSFVM_CODE_FUNCTION_EXT,
    VSFVM_CODE_FUNCTION_THREAD,         // special function which start a thread
};
typedef enum VSFVM_CODE_FUNCTION_ID_t VSFVM_CODE_FUNCTION_ID_t;

// arg8 of VSFVM_CODE_TYPE_VARIABLE
enum VSFVM_CODE_VARIABLE_POS_t {
    VSFVM_CODE_VARIABLE_POS_LOCAL = 0,  // in vsfvm_script_t.lvar
    VSFVM_CODE_VARIABLE_POS_STACK_BEGIN,// in vsfvm_thread_t.stack.var from begin
    VSFVM_CODE_VARIABLE_POS_STACK_END,  // in vsfvm_thread_t.stack.var from end
    VSFVM_CODE_VARIABLE_POS_FUNCARG,    // in vsfvm_thread_t.stack.var from thread->call_stack->tmp_reg
    VSFVM_CODE_VARIABLE_POS_FUNCAUTO,   // in vsfvm_thread_t.stack.var from thread->call_stack->auto_reg
    VSFVM_CODE_VARIABLE_POS_EXT,        // in extension
};
typedef enum VSFVM_CODE_VARIABLE_POS_t VSFVM_CODE_VARIABLE_POS_t;

// id of VSFVM_CODE_TYPE_VARIABLE
enum VSFVM_CODE_VARIABLE_ID_t {
    VSFVM_CODE_VARIABLE_NORMAL = 0,     // ARG8 of variable is VSFVM_CODE_VARIABLE_POS_t
    VSFVM_CODE_VARIABLE_REFERENCE,
    VSFVM_CODE_VARIABLE_REFERENCE_NOTRACE,
    VSFVM_CODE_VARIABLE_RESOURCES,      // ARG8 of resources is 0
    VSFVM_CODE_VARIABLE_FUNCTION,       // ARG8 of function is argu_num
};
typedef enum VSFVM_CODE_VARIABLE_ID_t VSFVM_CODE_VARIABLE_ID_t;

// arg8 of VSFVM_CODE_SYMBOL_SEMICOLIN
enum VSFVM_CODE_SYBMOL_SEMICOLIN_ID_t {
    VSFVM_CODE_SYMBOL_SEMICOLON_POP = 0,
    VSFVM_CODE_SYMBOL_SEMICOLON_NOPOP,
};
typedef enum VSFVM_CODE_SYBMOL_SEMICOLIN_ID_t VSFVM_CODE_SYBMOL_SEMICOLIN_ID_t;

enum VSFVM_CODE_SYMBOL_ID_t {
    VSFVM_CODE_SYMBOL_NOT,              // !    operator start
    VSFVM_CODE_SYMBOL_REV,              // ~
    VSFVM_CODE_SYMBOL_NEGA,             // -
    VSFVM_CODE_SYMBOL_POSI,             // +    single param operator end
    VSFVM_CODE_SYMBOL_MUL,              // *
    VSFVM_CODE_SYMBOL_DIV,              // /
    VSFVM_CODE_SYMBOL_MOD,              // %
    VSFVM_CODE_SYMBOL_ADD,              // +
    VSFVM_CODE_SYMBOL_SUB,              // -
    VSFVM_CODE_SYMBOL_AND,              // &
    VSFVM_CODE_SYMBOL_OR,               // |
    VSFVM_CODE_SYMBOL_XOR,              // ^
    VSFVM_CODE_SYMBOL_EQ,               // ==
    VSFVM_CODE_SYMBOL_NE,               // !=
    VSFVM_CODE_SYMBOL_GT,               // >
    VSFVM_CODE_SYMBOL_GE,               // >=
    VSFVM_CODE_SYMBOL_LT,               // <
    VSFVM_CODE_SYMBOL_LE,               // <=
    VSFVM_CODE_SYMBOL_LAND,             // &&
    VSFVM_CODE_SYMBOL_LOR,              // ||
    VSFVM_CODE_SYMBOL_LXOR,             // ^^
    VSFVM_CODE_SYMBOL_COMMA,            // ,
    VSFVM_CODE_SYMBOL_SHL,              // <<
    VSFVM_CODE_SYMBOL_SHR,              // >>
    VSFVM_CODE_SYMBOL_ASSIGN,           // =    operator end
    VSFVM_CODE_SYMBOL_SEMICOLON,        // ;

    VSFVM_CODE_SYMBOL_DOT,              // .    not used in vm
    VSFVM_CODE_SYMBOL_POINT,            // ->
    VSFVM_CODE_SYMBOL_REFERENCE,        // &
};
typedef enum VSFVM_CODE_SYMBOL_ID_t VSFVM_CODE_SYMBOL_ID_t;

// arg8 of VSFVM_CODE_KEYWORD_var
enum VSFVM_CODE_VAR_TYPE_t {
    VSFVM_CODE_VAR_I32 = 0,
    VSFVM_CODE_VAR_U32,
    VSFVM_CODE_VAR_I16,
    VSFVM_CODE_VAR_U16,
    VSFVM_CODE_VAR_I8,
    VSFVM_CODE_VAR_U8,
};
typedef enum VSFVM_CODE_VAR_TYPE_t VSFVM_CODE_VAR_TYPE_t;

enum VSFVM_CODE_KEYWORD_ID_t {
    // can be used in vm
    VSFVM_CODE_KEYWORD_var = 0,
    VSFVM_CODE_KEYWORD_goto,
    VSFVM_CODE_KEYWORD_if,
    VSFVM_CODE_KEYWORD_return,
    VSFVM_CODE_KEYWORD_breakpoint,

    // not used in vm
    VSFVM_CODE_KEYWORD_RIGHT_BRACE,     // }
    VSFVM_CODE_KEYWORD_LEFT_BRACE,      // {
    VSFVM_CODE_KEYWORD_RIGHT_SQUARE,    // ]
    VSFVM_CODE_KEYWORD_LEFT_SQUARE,     // [
    VSFVM_CODE_KEYWORD_RIGHT_GROUPING,  // )
    VSFVM_CODE_KEYWORD_LEFT_GROUPING,   // (
    VSFVM_CODE_KEYWORD_break,
    VSFVM_CODE_KEYWORD_define,
    VSFVM_CODE_KEYWORD_while,
    VSFVM_CODE_KEYWORD_else,
};
typedef enum VSFVM_CODE_KEYWORD_ID_t VSFVM_CODE_KEYWORD_ID_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif      //__VSFVM_BYTECODE_H__
