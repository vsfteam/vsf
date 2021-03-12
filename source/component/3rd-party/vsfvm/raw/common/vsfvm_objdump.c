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

#if (VSFVM_CFG_RUNTIME_EN == ENABLED) || (VSFVM_CFG_COMPILER_EN == ENABLED)

#include "service/vsf_service.h"

#include "./vsfvm_bytecode.h"
#include "./vsfvm_objdump.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const char * __vsfvmc_variable_id[] = {
    STR(VSFVM_CODE_VARIABLE_NORMAL),
    STR(VSFVM_CODE_VARIABLE_REFERENCE),
    STR(VSFVM_CODE_VARIABLE_REFERENCE_NOTRACE),
    STR(VSFVM_CODE_VARIABLE_RESOURCES),
    STR(VSFVM_CODE_VARIABLE_FUNCTION),
};

static const char * __vsfvmc_variable_pos[] = {
    STR(VSFVM_CODE_VARIABLE_POS_LOCAL),
    STR(VSFVM_CODE_VARIABLE_POS_STACK_BEGIN),
    STR(VSFVM_CODE_VARIABLE_POS_STACK_END),
    STR(VSFVM_CODE_VARIABLE_POS_FUNCARG),
    STR(VSFVM_CODE_VARIABLE_POS_FUNCAUTO),
    STR(VSFVM_CODE_VARIABLE_POS_EXT),
};

static const char * __vsfvmc_funtion_id[] = {
    STR(VSFVM_CODE_FUNCTION_SCRIPT),
    STR(VSFVM_CODE_FUNCTION_EXT),
    STR(VSFVM_CODE_FUNCTION_THREAD),
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void vsfvm_tkdump(vsfvm_bytecode_t token)
{
    uint_fast32_t type  = VSFVM_CODE_TYPE(token);
    uint_fast32_t id    = VSFVM_CODE_ID(token);
//    uint_fast32_t arg24 = VSFVM_CODE_ARG24(token);
    uint_fast32_t arg8  = VSFVM_CODE_ARG8(token);
    uint_fast32_t arg16 = VSFVM_CODE_ARG16(token);
    int_fast32_t value  = VSFVM_CODE_VALUE(token);

    vsf_trace(VSF_TRACE_NONE, "0x%08X:", token);
    switch (type) {
    case VSFVM_CODE_TYPE_SYMBOL:

#define case_print_symbol(sym)                                                  \
    case sym:                                                                   \
        vsf_trace(VSF_TRACE_NONE,                                               \
            "VSFVM_SYMBOL(" __STR(sym) ", %d, %d)," VSF_TRACE_CFG_LINEEND, arg8, arg16);\
             break

        switch (id) {
            case_print_symbol(VSFVM_CODE_SYMBOL_NOT);
            case_print_symbol(VSFVM_CODE_SYMBOL_REV);
            case_print_symbol(VSFVM_CODE_SYMBOL_NEGA);
            case_print_symbol(VSFVM_CODE_SYMBOL_POSI);
            case_print_symbol(VSFVM_CODE_SYMBOL_MUL);
            case_print_symbol(VSFVM_CODE_SYMBOL_DIV);
            case_print_symbol(VSFVM_CODE_SYMBOL_MOD);
            case_print_symbol(VSFVM_CODE_SYMBOL_ADD);
            case_print_symbol(VSFVM_CODE_SYMBOL_SUB);
            case_print_symbol(VSFVM_CODE_SYMBOL_AND);
            case_print_symbol(VSFVM_CODE_SYMBOL_OR);
            case_print_symbol(VSFVM_CODE_SYMBOL_XOR);
            case_print_symbol(VSFVM_CODE_SYMBOL_EQ);
            case_print_symbol(VSFVM_CODE_SYMBOL_NE);
            case_print_symbol(VSFVM_CODE_SYMBOL_GT);
            case_print_symbol(VSFVM_CODE_SYMBOL_GE);
            case_print_symbol(VSFVM_CODE_SYMBOL_LT);
            case_print_symbol(VSFVM_CODE_SYMBOL_LE);
            case_print_symbol(VSFVM_CODE_SYMBOL_LAND);
            case_print_symbol(VSFVM_CODE_SYMBOL_LOR);
            case_print_symbol(VSFVM_CODE_SYMBOL_LXOR);
            case_print_symbol(VSFVM_CODE_SYMBOL_COMMA);
            case_print_symbol(VSFVM_CODE_SYMBOL_SHL);
            case_print_symbol(VSFVM_CODE_SYMBOL_SHR);
            case_print_symbol(VSFVM_CODE_SYMBOL_ASSIGN);
            case_print_symbol(VSFVM_CODE_SYMBOL_SEMICOLON);
            default: vsf_trace(VSF_TRACE_NONE, "unknown symbol: %d" VSF_TRACE_CFG_LINEEND, id);
        }
        break;
    case VSFVM_CODE_TYPE_KEYWORD:

#define case_print_keyword(kw)                                                  \
    case kw:                                                                    \
        vsf_trace(VSF_TRACE_NONE,                                               \
            "VSFVM_KEYWORD(" __STR(kw) ", %d, %d)," VSF_TRACE_CFG_LINEEND, arg8, arg16);\
                break

        switch (id) {
            case_print_keyword(VSFVM_CODE_KEYWORD_var);
            case_print_keyword(VSFVM_CODE_KEYWORD_goto);
            case_print_keyword(VSFVM_CODE_KEYWORD_if);
            case_print_keyword(VSFVM_CODE_KEYWORD_return);
            case_print_keyword(VSFVM_CODE_KEYWORD_breakpoint);
            default: vsf_trace(VSF_TRACE_NONE, "unknown keyword: %d" VSF_TRACE_CFG_LINEEND, id);
        }
        break;
    case VSFVM_CODE_TYPE_NUMBER:
        vsf_trace(VSF_TRACE_NONE, "VSFVM_NUMBER(%d)," VSF_TRACE_CFG_LINEEND, value);
        break;
    case VSFVM_CODE_TYPE_VARIABLE:
        vsf_trace(VSF_TRACE_NONE, "VSFVM_VARIABLE(%s, %s, %d)," VSF_TRACE_CFG_LINEEND,
                id > VSFVM_CODE_VARIABLE_FUNCTION ? "unknown variable type" : __vsfvmc_variable_id[id],
                arg8 > VSFVM_CODE_VARIABLE_POS_EXT ? "unknown variable position" : __vsfvmc_variable_pos[arg8],
                arg16);
        break;
    case VSFVM_CODE_TYPE_FUNCTION:
        vsf_trace(VSF_TRACE_NONE, "VSFVM_FUNCTION(%s, %d, %d)," VSF_TRACE_CFG_LINEEND,
            id > VSFVM_CODE_FUNCTION_THREAD ? "unknown function type" : __vsfvmc_funtion_id[id],
            arg8, arg16);
        break;
    case VSFVM_CODE_TYPE_EOF:
        vsf_trace(VSF_TRACE_NONE, "VSFVM_EOF()," VSF_TRACE_CFG_LINEEND);
        break;
    }
}

void vsfvm_objdump(vsfvm_bytecode_t *buff, uint_fast32_t len)
{
    for (uint_fast32_t i = 0; i < len; i++) {
        vsf_trace_info("%d:", i);
        vsfvm_tkdump(*buff++);
    }
}

#endif      // VSFVM_CFG_RUNTIME_EN || VSFVM_CFG_COMPILER_EN
