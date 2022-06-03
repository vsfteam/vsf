/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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
#   include "./include/termios.h"
#   include "./include/sys/time.h"
#else
#   include <signal.h>
#   include <dirent.h>
#   include <termios.h>
#   include <sys/time.h>
#endif

#include "./kernel/fs/vsf_linux_fs.h"

#if     defined(__VSF_LINUX_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#elif   defined(__VSF_LINUX_CLASS_INHERIT__)
#   define __VSF_CLASS_INHERIT__
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
#   if VSF_KERNEL_CFG_THREAD_STACK_LARGE != ENABLED
#       error stacks should be 16-bit if VSF_KERNEL_CFG_THREAD_STACK_LARGE is not enabled
#   endif
#endif
#if     (VSF_LINUX_CFG_STACKSIZE < (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE))\
    ||  (VSF_LINUX_CFG_STACKSIZE & (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE - 1))
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
    bool is_dyn_argv;
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

#if VSF_LINUX_CFG_PLS_NUM > 0 || VSF_LINUX_CFG_TLS_NUM > 0
typedef struct vsf_linux_localstorage_t {
    void *data;
    void (*destructor)(void *data);
} vsf_linux_localstorage_t;

typedef struct vsf_linux_dynlib_mod_t {
    int *lib_idx;
    uint16_t mod_idx;
    uint16_t module_num;
    uint32_t bss_size;
    uint32_t mod_size;
    void (*init)(void *ctx);
} vsf_linux_dynlib_mod_t;

typedef struct vsf_linux_dynlib_t {
    uint16_t module_num;
    uint32_t bss_size;
    uint32_t bss_brk;
    // just make iar happy, which does not support zla
    void * modules[1];
} vsf_linux_dynlib_t;
#endif

vsf_class(vsf_linux_trigger_t) {
    public_member(
        implement(vsf_trig_t)
    )
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    private_member(
        vsf_dlist_node_t node;
        vsf_linux_process_t *pending_process;
        int sig;
    )
#endif
};

#if VSF_LINUX_CFG_TLS_NUM > 0
dcl_vsf_bitmap(vsf_linux_tls_bitmap, VSF_LINUX_CFG_TLS_NUM);
#endif

vsf_class(vsf_linux_thread_t) {
    public_member(
        implement(vsf_thread_t)
        implement(vsf_thread_cb_t)
        const vsf_linux_thread_op_t *op;

        int __errno;
        // for gethostbyname, etc
        int __h_errno;
    )

    protected_member(
        int retval;
        int tid;
        vsf_linux_thread_t *thread_pending;
#if VSF_LINUX_CFG_TLS_NUM > 0
        vsf_linux_localstorage_t tls[VSF_LINUX_CFG_TLS_NUM];
#endif
    )

    private_member(
        vsf_linux_process_t *process;
        vsf_dlist_node_t thread_node;
    )
};

typedef struct vsf_linux_sig_handler_t {
    vsf_dlist_node_t node;
    uint8_t sig;
    uint8_t flags;
    sigset_t mask;
    union {
        sighandler_t sighandler;
        void (*sigaction_handler)(int, siginfo_t *, void *);
    };
} vsf_linux_sig_handler_t;

typedef struct vsf_linux_stdio_stream_t {
    vsf_stream_t *in;
    vsf_stream_t *out;
    vsf_stream_t *err;
} vsf_linux_stdio_stream_t;

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
typedef struct vsf_linux_timer_t {
    struct itimerval value;
    vsf_systimer_tick_t start;
} vsf_linux_timer_t;
#endif

dcl_vsf_bitmap(vsf_linux_fd_bitmap, VSF_LINUX_CFG_FD_BITMAP_SIZE);

#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
typedef struct vsf_linux_heap_info_t {
    size_t usage;
    int balance;
} vsf_linux_heap_info_t;

typedef struct vsf_liunx_heap_node_t {
    void *ptr;
    size_t size;

    char *func;
} vsf_liunx_heap_node_t;

#   if VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH > 0
dcl_vsf_bitmap(vsf_linux_heap_nodes_bitmap, VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH);
#   endif

typedef struct vsf_linux_heap_monitor_t {
    vsf_linux_heap_info_t info;

#   if VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH > 0
    vsf_bitmap(vsf_linux_heap_nodes_bitmap) bitmap;
    vsf_liunx_heap_node_t nodes[VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH];
#   endif
} vsf_linux_heap_monitor_t;
#endif

vsf_class(vsf_linux_process_t) {
    public_member(
        vsf_linux_process_ctx_t ctx;

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
        char **__environ;
#endif
    )

    protected_member(
        int status;
        vsf_linux_process_t *shell_process;
        struct {
            pid_t pid;
            pid_t ppid;
            pid_t gid;
        } id;
#if VSF_LINUX_USE_TERMIOS == ENABLED
        struct termios term[3];
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        struct vsf_linux_timer_t timers[ITIMER_NUM];
        vsf_callback_timer_t real_timer;
#endif

#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
        vsf_linux_heap_monitor_t heap_monitor;
#endif
        void (*fn_atexit)(void);
    )

    private_member(
        char *working_dir;

        vsf_dlist_node_t process_node;
        vsf_dlist_t thread_list;
        vsf_dlist_t fd_list;
        vsf_linux_thread_t *thread_pending;
        vsf_bitmap(vsf_linux_fd_bitmap) fd_bitmap;

        struct {
            sigset_t pending;
            sigset_t mask;
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
            vsf_dlist_t handler_list;
            vsf_linux_thread_t *sighandler_thread;
            vsf_dlist_t trigger_list;
#endif
        } sig;

        vsf_prio_t prio;

        vsf_linux_thread_t *thread_pending_exit;
        vsf_linux_process_t *parent_process;
        vsf_dlist_t child_list;
        vsf_dlist_node_t child_node;

#if VSF_LINUX_CFG_PLS_NUM > 0
        vsf_linux_localstorage_t pls[VSF_LINUX_CFG_PLS_NUM];
#endif

#if VSF_LINUX_CFG_TLS_NUM > 0
        struct {
            vsf_bitmap(vsf_linux_tls_bitmap) bitmap;
        } tls;
#endif
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

// IMPORTANT: priority of stdio_stream MUST be within scheduler priorities
extern vsf_err_t vsf_linux_init(vsf_linux_stdio_stream_t *stdio_stream);

extern vsf_systimer_tick_t vsf_linux_sleep(vsf_systimer_tick_t ticks);

// used for dynamic libraries, allocate/free memory from resources_process
extern vsf_linux_process_t * vsf_linux_resources_process(void);
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
// can not put in clib headers because of dependency, so put them here
extern void * __malloc_ex(vsf_linux_process_t *process, int size, ...);
extern void * __calloc_ex(vsf_linux_process_t *process, size_t n, size_t size, ...);
extern void __free_ex(vsf_linux_process_t *process, void *ptr, ...);
extern void * __realloc_ex(vsf_linux_process_t *process, void *p, size_t size, ...);
extern char * __strdup_ex(vsf_linux_process_t *process, const char *str);
#else
#   define __malloc_ex(__process, __size, ...)          malloc(__size)
#   define __calloc_ex(__process, __n, __size, ...)     calloc((__n), (__size))
#   define __free_ex(__process, __ptr, ...)             free(__ptr)
#   define __realloc_ex(__process, __ptr, __size, ...)  realloc((__ptr), (__size))
#   define __strdup_ex(__process, __str)                strdup(__str)
#endif

extern int vsf_linux_generate_path(char *path_out, int path_out_lenlen, char *dir, char *path_in);
extern int vsf_linux_chdir(vsf_linux_process_t *process, char *working_dir);

extern int vsf_linux_fd_bind_executable(int fd, vsf_linux_main_entry_t entry);

extern int vsf_linux_expandenv(const char *str, char *output, size_t bufflen);

#if VSF_LINUX_CFG_PLS_NUM > 0
extern vsf_err_t vsf_linux_library_init(int *lib_idx, void *lib_ctx, void (*destructor)(void *));
extern void * vsf_linux_library_ctx(int lib_idx);

extern vsf_err_t vsf_linux_dynlib_init(int *lib_idx, int module_num, int bss_size);
extern void * vsf_linux_dynlib_ctx(const vsf_linux_dynlib_mod_t *mod);
#endif

#if defined(__VSF_LINUX_CLASS_IMPLEMENT) || defined(__VSF_LINUX_CLASS_INHERIT__)
#   if VSF_LINUX_CFG_PLS_NUM > 0
extern int vsf_linux_pls_alloc(void);
extern void vsf_linux_pls_free(int idx);
extern vsf_linux_localstorage_t * vsf_linux_pls_get(int idx);
#   endif

#   if VSF_LINUX_CFG_TLS_NUM > 0
extern int vsf_linux_tls_alloc(void (*destructor)(void *));
extern void vsf_linux_tls_free(int idx);
extern vsf_linux_localstorage_t * vsf_linux_tls_get(int idx);
#   endif

extern bool vsf_linux_is_inited(void);
extern int vsf_linux_is_stdio_stream(int fd);

extern vsf_linux_main_entry_t * vsf_linux_fd_get_executable(int fd);
extern int vsf_linux_fs_get_executable(const char *pathname, vsf_linux_main_entry_t *entry);

extern vsf_linux_process_t * vsf_linux_create_process(int stack_size);
// delete unstarted/existed process
void vsf_linux_delete_process(vsf_linux_process_t *process);
extern int vsf_linux_start_process(vsf_linux_process_t *process);
extern void vsf_linux_exit_process(int status);

extern void vsf_linux_cleanup_process(vsf_linux_process_t *process);
extern void vsf_linux_detach_process(vsf_linux_process_t *process);
extern void vsf_linux_detach_thread(vsf_linux_thread_t *thread);
extern int vsf_linux_wait_thread(int tid, int *retval);

extern vsf_linux_thread_t * vsf_linux_create_thread(vsf_linux_process_t *process,
            const vsf_linux_thread_op_t *op,
            int stack_size, void *stack);
extern int vsf_linux_start_thread(vsf_linux_thread_t *thread, vsf_prio_t priority);

extern void vsf_linux_thread_on_terminate(vsf_linux_thread_t *thread);
extern vsf_linux_thread_t * vsf_linux_get_thread(int tid);
extern vsf_linux_process_t * vsf_linux_get_process(pid_t pid);

extern void __vsf_linux_process_free_arg(vsf_linux_process_t *process);
extern int __vsf_linux_process_parse_arg(vsf_linux_process_t *process, char * const * argv);

extern int vsf_linux_merge_env(vsf_linux_process_t *process, char **env);
extern void vsf_linux_free_env(vsf_linux_process_t *process);

extern void vsf_linux_trigger_init(vsf_linux_trigger_t *trig);
// return value:
//  < 0 : triggered by signals, returns the minus signal
//  0   : triggered normaly
//  > 0 : triggered because of timeout
extern int vsf_linux_trigger_pend(vsf_linux_trigger_t *trig, vsf_timeout_tick_t timeout);
extern int vsf_linux_trigger_signal(vsf_linux_trigger_t *trig, int sig);
#endif

// open vsf_linux_get_cur_thread for thread-related variables like errno, etc
extern vsf_linux_thread_t * vsf_linux_get_cur_thread(void);

// open vsf_linux_get_cur_process for process-related variables like optarg, etc
extern vsf_linux_process_t * vsf_linux_get_cur_process(void);

extern int vsf_linux_get_errno(void);
extern void vsf_linux_set_errno(int err);

#ifdef __cplusplus
}
#endif

#undef __VSF_LINUX_CLASS_IMPLEMENT
#undef __VSF_LINUX_CLASS_INHERIT__

#endif      // VSF_USE_LINUX
#endif      // __VSF_LINUX_INTERNAL_H__
/* EOF */