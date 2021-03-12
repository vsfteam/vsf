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

#include "./vsfvm_compiler.h"
#include "./lexer/vsfvm_lexer.h"
#include "./vsfvm_snapshot.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

int vsfvm_snapshot_take(vsfvm_compiler_t *compiler, vsfvm_snapshot_t *snapshot)
{
    vsfvm_lexer_t *lexer = &compiler->script.lexer;
    vsfvm_lexer_ctx_t *lctx = &lexer->curctx;
    vsfvm_snapshot_lexer_t *snapshot_lexer = &snapshot->script.lexer;
    vsfvm_snapshot_func_t *snapshot_curfunc = &snapshot->script.cur_func;
    vsfvm_compiler_func_t *curfunc = &compiler->script.cur_func;

    if (lctx->op->priv_size > 0) {
        snapshot_lexer->priv = vsf_heap_malloc(lctx->op->priv_size);
        if (!snapshot_lexer->priv) { return -1; }
        memcpy(snapshot_lexer->priv, lctx->priv, lctx->op->priv_size);
    }

    snapshot_lexer->op = lctx->op;
    snapshot_lexer->pt_state = lctx->pt.state;
    snapshot_lexer->ctx_sp = lexer->ctx_stack.sp;
    snapshot_lexer->symtbl_sp = lexer->symtbl_stack.sp;
    snapshot_lexer->symid = lexer->symid;
    snapshot_lexer->expr.nesting = lexer->expr.nesting;
    snapshot_lexer->expr.stack_exp_sp = lexer->expr.stack_exp.sp;
    snapshot_lexer->expr.stack_op_sp = lexer->expr.stack_op.sp;
    snapshot_lexer->expr.ctx = lexer->expr.ctx;

    snapshot_curfunc->name = curfunc->name;
    snapshot_curfunc->symtbl_idx = curfunc->symtbl_idx;
    snapshot_curfunc->block_level = curfunc->block_level;
    snapshot_curfunc->arglist = curfunc->arglist;
    snapshot_curfunc->varlist = curfunc->varlist;
    snapshot_curfunc->curctx = curfunc->curctx;
    snapshot_curfunc->ctx_sp = curfunc->ctx.sp;
    snapshot_curfunc->linktbl_sp = curfunc->linktbl.sp;

    snapshot->bytecode_pos = compiler->bytecode_pos;
    snapshot->script.func_stack_sp = compiler->script.func_stack.sp;
    snapshot->script.pt_stmt_state = compiler->script.pt_stmt.state;
    return 0;
}

int vsfvm_snapshot_free(vsfvm_snapshot_t *snapshot)
{
    if (snapshot->script.lexer.priv != NULL) {
        vsf_heap_free(snapshot->script.lexer.priv);
    }
    memset(snapshot, 0, sizeof(*snapshot));
    return 0;
}

int vsfvm_snapshot_restore(vsfvm_compiler_t *compiler, vsfvm_snapshot_t *snapshot)
{
    vsfvm_lexer_t *lexer = &compiler->script.lexer;
    vsfvm_lexer_ctx_t *lctx = &lexer->curctx;
    vsfvm_snapshot_lexer_t *snapshot_lexer = &snapshot->script.lexer;
    vsfvm_snapshot_func_t *snapshot_curfunc = &snapshot->script.cur_func;
    vsfvm_compiler_func_t *curfunc = &compiler->script.cur_func;
    vsfvm_lexer_symtbl_t *symtbl;
    uint_fast32_t symtbl_size;

    if (snapshot_lexer->priv != NULL) {
        memcpy(lctx->priv, snapshot_lexer->priv, snapshot_lexer->op->priv_size);
    }

    lctx->op = snapshot_lexer->op;
    lctx->pt.state = snapshot_lexer->pt_state;

    if (lexer->ctx_stack.sp < snapshot_lexer->ctx_sp) {
        lexer->ctx_stack.sp = snapshot_lexer->ctx_sp;
    }

    if (lexer->symid < snapshot_lexer->symid) {
        return -1;
    }

    while (lexer->symid != snapshot_lexer->symid) {
        symtbl = vsf_dynstack_get(&lexer->symtbl_stack, 0);
        if (!symtbl) { return -1; }
        symtbl_size = vsf_dynarr_get_size(&symtbl->table);
        if (!symtbl_size) { return -1; }

        symtbl_size--;
        vsf_dynarr_set_size(&symtbl->table, symtbl_size);
        if (!symtbl_size && (lexer->symtbl_stack.sp != snapshot_lexer->symtbl_sp)) {
            vsfvm_lexer_symtbl_delete(lexer);
        }
        lexer->symid--;
    }
    if (    (lexer->symtbl_stack.sp != snapshot_lexer->symtbl_sp)
        ||  (lexer->symid != snapshot_lexer->symid)) {
        return -1;
    }

    if (lexer->expr.nesting < snapshot_lexer->expr.nesting) {
        return -1;
    }
    lexer->expr.nesting = snapshot_lexer->expr.nesting;
    if (lexer->expr.stack_exp.sp < snapshot_lexer->expr.stack_exp_sp) {
        return -1;
    }
    lexer->expr.stack_exp.sp = snapshot_lexer->expr.stack_exp_sp;
    if (lexer->expr.stack_op.sp < snapshot_lexer->expr.stack_op_sp) {
        return -1;
    }
    lexer->expr.stack_op.sp = snapshot_lexer->expr.stack_op_sp;
    lexer->expr.ctx = snapshot_lexer->expr.ctx;

    curfunc->name = snapshot_curfunc->name;
    curfunc->symtbl_idx = snapshot_curfunc->symtbl_idx;
    curfunc->block_level = snapshot_curfunc->block_level;
    curfunc->arglist = snapshot_curfunc->arglist;
    curfunc->varlist = snapshot_curfunc->varlist;
    curfunc->curctx = snapshot_curfunc->curctx;
    if (curfunc->ctx.sp < snapshot_curfunc->ctx_sp) {
        return - 1;
    } else {
        vsf_dynstack_pop(&curfunc->ctx, snapshot_curfunc->ctx_sp - curfunc->ctx.sp);
    }

    if (curfunc->linktbl.sp < snapshot_curfunc->linktbl_sp) {
        return -1;
    }
    curfunc->linktbl.sp = snapshot_curfunc->linktbl_sp;

    compiler->bytecode_pos = snapshot->bytecode_pos;
    if (compiler->script.func_stack.sp < snapshot->script.func_stack_sp) {
        return -1;
    }
    compiler->script.func_stack.sp = snapshot->script.func_stack_sp;
    compiler->script.pt_stmt.state = snapshot->script.pt_stmt_state;
    return 0;
}

#endif      // VSFVM_CFG_COMPILER_EN
