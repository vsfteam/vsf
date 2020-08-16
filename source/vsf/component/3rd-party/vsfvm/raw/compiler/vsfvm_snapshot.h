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

#ifndef __VSFVM_SNAPSHOT_H__
#define __VSFVM_SNAPSHOT_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_vm_cfg.h"

#if VSFVM_CFG_COMPILER_EN == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfvm_snapshot_func_t {
    vsfvm_lexer_sym_t *name;
    int symtbl_idx;
    int block_level;

       vsf_slist_t arglist;
    vsf_slist_t varlist;

    vsfvm_compiler_func_ctx_t curctx;
    uint32_t ctx_sp;
    uint32_t linktbl_sp;
} vsfvm_snapshot_func_t;

typedef struct vsfvm_snapshot_expr_t {
    vsfvm_lexer_exprctx_t ctx;
    uint32_t nesting;
    uint32_t stack_exp_sp;
    uint32_t stack_op_sp;
} vsfvm_snapshot_expr_t;

typedef struct vsfvm_snapshot_lexer_t {
    const vsfvm_lexer_op_t *op;
    void *priv;
    int pt_state;

    uint32_t ctx_sp;
    uint32_t symtbl_sp;
    int symid;

    vsfvm_snapshot_expr_t expr;
} vsfvm_snapshot_lexer_t;

typedef struct vsfvm_snapshot_script_t {
    vsfvm_snapshot_lexer_t lexer;
    vsfvm_snapshot_func_t cur_func;

    int pt_stmt_state;
    uint32_t func_stack_sp;
} vsfvm_snapshot_script_t;

typedef struct vsfvm_snapshot_t {
    vsfvm_snapshot_script_t script;
    uint32_t bytecode_pos;
} vsfvm_snapshot_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern int vsfvm_snapshot_take(vsfvm_compiler_t *compiler, vsfvm_snapshot_t *snapshot);
extern int vsfvm_snapshot_free(vsfvm_snapshot_t *snapshot);
extern int vsfvm_snapshot_restore(vsfvm_compiler_t *compiler, vsfvm_snapshot_t *snapshot);

#endif      // VSFVM_CFG_COMPILER_EN
#endif      // __VSFVM_SNAPSHOT_H__
