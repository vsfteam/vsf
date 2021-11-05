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

#ifndef __VSF_LINUX_INTERNAL_H__
#define __VSF_LINUX_INTERNAL_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "./vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./include/signal.h"
#   include "./include/dirent.h"
#else
#   include <signal.h>
#   include <dirent.h>
#endif

#include "./kernel/fs/vsf_linux_fs.h"

#if     defined(__VSF_LINUX_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_LINUX_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE MUST be enabled to use vsf_linux
#endif

#ifndef VSF_LINUX_CFG_MAX_ARG_NUM
#   define VSF_LINUX_CFG_MAX_ARG_NUM        31
#endif

#ifndef VSF_LINUX_CFG_STACKSIZE
#   define VSF_LINUX_CFG_STACKSIZE          1024
#endif
#if VSF_LINUX_CFG_STACKSIZE > 0xFFFF
#   error VSF_LINUX_CFG_STACKSIZE should be 16-bit
#endif
#if     (VSF_LINUX_CFG_STACKSIZE < (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE))\
    ||  VSF_LINUX_CFG_STACKSIZE & (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1)
#   error invalid VSF_LINUX_CFG_STACKSIZE
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define vsf_linux_thread_get_priv(__thread)         (void *)(&(((vsf_linux_thread_t *)(__thread))[1]))

/*============================ TYPES =========================================*/

vsf_dcl_class(vsf_linux_process_t)
vsf_dcl_class(vsf_linux_thread_t)

typedef struct vsf_linux_process_arg_t {
    int argc;
    char const *argv[VSF_LINUX_CFG_MAX_ARG_NUM + 1];
} vsf_linux_process_arg_t;

typedef int (*vsf_linux_main_entry_t)(int, char **);
typedef int (*vsf_linux_process_arg_parser_t)(vsf_linux_process_arg_t *arg);

typedef struct vsf_linux_thread_op_t {
    int priv_size;
    void (*on_run)(vsf_thread_cb_t *cb);
    void (*on_terminate)(vsf_linux_thread_t *thread);
} vsf_linux_thread_op_t;

typedef struct vsf_linux_process_ctx_t {
    vsf_linux_process_arg_t arg;
    vsf_linux_main_entry_t entry;
} vsf_linux_process_ctx_t;

vsf_class(vsf_linux_thread_t) {
    public_member(
        implement(vsf_thread_t)
        implement(vsf_thread_cb_t)
        const vsf_linux_thread_op_t *op;
    )

    protected_member(
        int retval;
        int tid;
        vsf_linux_thread_t *thread_pending;
    )

    private_member(
        vsf_linux_process_t *process;
        vsf_dlist_node_t thread_node;
    )
};

typedef struct vsf_linux_sig_handler_t {
    vsf_dlist_node_t node;
    uint_fast8_t sig;
    void (*handler)(int, siginfo_t *, void *);
} vsf_linux_sig_handler_t;

typedef struct vsf_linux_stdio_stream_t {
    vsf_stream_t *in;
    vsf_stream_t *out;
    vsf_stream_t *err;
} vsf_linux_stdio_stream_t;

#ifdef VSF_LINUX_CFG_FD_BITMAP_SIZE
dcl_vsf_bitmap(vsf_linux_fd_bitmap, VSF_LINUX_CFG_FD_BITMAP_SIZE);
#endif

vsf_class(vsf_linux_process_t) {
    public_member(
        vsf_linux_process_ctx_t ctx;
    )

    protected_member(
        vsf_linux_process_t *shell_process;
        struct {
            pid_t pid;
            pid_t ppid;
        } id;
#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_CHECK == ENABLED
        size_t heap_usage;
        int heap_balance;
#endif
    )

    private_member(
        vsf_linux_stdio_stream_t stdio_stream;
        char *working_dir;

        vsf_dlist_node_t process_node;
        vsf_dlist_t thread_list;
        vsf_dlist_t fd_list;
        vsf_linux_thread_t *thread_pending;
#ifdef VSF_LINUX_CFG_FD_BITMAP_SIZE
        vsf_bitmap(vsf_linux_fd_bitmap) fd_bitmap;
#else
        int cur_fd;
#endif

        struct {
            sigset_t pending;
            sigset_t mask;
            vsf_dlist_t handler_list;
        } sig;

//        struct termios term;

        vsf_prio_t prio;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

// IMPORTANT: priority of stdio_stream MUST be within scheduler priorities
extern vsf_err_t vsf_linux_init(vsf_linux_stdio_stream_t *stdio_stream);

extern int vsf_linux_generate_path(char *path_out, int path_out_lenlen, char *dir, char *path_in);
extern int vsf_linux_chdir(vsf_linux_process_t *process, char *working_dir);

extern int vsf_linux_fd_bind_executable(int fd, vsf_linux_main_entry_t entry);

#if defined(__VSF_LINUX_CLASS_IMPLEMENT) || defined(__VSF_LINUX_CLASS_INHERIT__)
extern void vsf_linux_set_dominant_process(void);

extern int vsf_linux_fs_get_executable(const char *pathname, vsf_linux_main_entry_t *entry);

extern vsf_linux_process_t * vsf_linux_create_process_ex(int stack_size, vsf_linux_stdio_stream_t *stdio_stream, char *working_dir);
extern vsf_linux_process_t * vsf_linux_create_process(int stack_size);
extern int vsf_linux_start_process(vsf_linux_process_t *process);

extern vsf_linux_thread_t * vsf_linux_create_thread(vsf_linux_process_t *process,
            const vsf_linux_thread_op_t *op,
            int stack_size, void *stack);
extern int vsf_linux_start_thread(vsf_linux_thread_t *thread, vsf_prio_t priority);

extern void vsf_linux_thread_on_terminate(vsf_linux_thread_t *thread);
extern vsf_linux_thread_t * vsf_linux_get_cur_thread(void);
extern vsf_linux_process_t * vsf_linux_get_cur_process(void);
extern vsf_linux_thread_t * vsf_linux_get_thread(int tid);
extern vsf_linux_process_t * vsf_linux_get_process(pid_t pid);
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_LINUX_CLASS_IMPLEMENT
#undef __VSF_LINUX_CLASS_INHERIT__

#endif      // VSF_USE_LINUX
#endif      // __VSF_LINUX_INTERNAL_H__
/* EOF */