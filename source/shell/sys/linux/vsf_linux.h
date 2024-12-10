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

// avoid to include vsf.h, include necessary headers only
//#include "vsf.h"

#include "kernel/vsf_kernel.h"

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

#if VSF_USE_SIMPLE_STREAM != ENABLED
#   error VSF_USE_SIMPLE_STREAM MUST be enabled to use vsf_linux
#endif

#if VSF_USE_HEAP != ENABLED
#   error VSF_USE_HEAP MUST be enabled to use vsf_linux
#endif

#ifndef VSF_LINUX_CFG_STACKSIZE
#   if defined(VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE) && defined(VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)
#       if (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE) < 1024
#           define VSF_LINUX_CFG_STACKSIZE  1024
#       else
#           define VSF_LINUX_CFG_STACKSIZE  (VSF_KERNEL_CFG_THREAD_STACK_PAGE_SIZE + VSF_KERNEL_CFG_THREAD_STACK_GUARDIAN_SIZE)
#       endif
#   else
#       define VSF_LINUX_CFG_STACKSIZE      1024
#   endif
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

// linux kernel events

enum {
    __VSF_EVT_LINUX_EVENTFD_INC = VSF_EVT_USER + 0,
    __VSF_EVT_LINUX_TERM_RX     = VSF_EVT_USER + 1,
};

vsf_dcl_class(vsf_linux_process_t)
vsf_dcl_class(vsf_linux_thread_t)

typedef struct vsf_linux_process_arg_t {
    int argc;
    char const **argv;
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

typedef struct vsf_linux_key_t {
    vsf_dlist_node_t node;
    key_t key;
} vsf_linux_key_t;

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
    int * lib_idx;
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

#if VSF_LINUX_CFG_PLS_NUM > 0
dcl_vsf_bitmap(vsf_linux_pls_bitmap, VSF_LINUX_CFG_PLS_NUM);
#endif

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
        pid_t pid_exited;     // used in wait
        int retval;
        pid_t tid;
        vsf_linux_thread_t *thread_pending;
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
        vsf_linux_trigger_t *trigger_pending;
#endif
#if VSF_LINUX_CFG_TLS_NUM > 0
        vsf_linux_localstorage_t tls[VSF_LINUX_CFG_TLS_NUM];
#endif
#if     VSF_ARCH_USE_THREAD_REG == ENABLED                                      \
    &&  VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_APPLET_USE_LIBC_STDLIB == ENABLED && !defined(__VSF_APPLET__)
        union {
            // used to save compar parameter for bsearch and qsort
            void *tmp_ptr;
        } process_reg_backup;
#endif
        char name[16];
    )

    private_member(
        vsf_linux_process_t *process;
        vsf_dlist_node_t thread_node;
        union {
            struct {
                unsigned short wantval;
            } sem;
        } func_priv;
    )
};

typedef struct vsf_linux_sig_handler_t {
    vsf_dlist_node_t node;
    uint8_t sig;
    uint32_t flags;
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
    struct itimerspec value;
    struct sigevent evt;
    int overrun;
    vsf_callback_timer_t timer;
    vsf_systimer_tick_t start;
} vsf_linux_timer_t;
#endif

dcl_vsf_bitmap(vsf_linux_fd_bitmap, VSF_LINUX_CFG_FD_BITMAP_SIZE);

#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED

typedef struct vsf_liunx_heap_node_t {
    vsf_dlist_node_t node;

    void *ptr;
    size_t size;

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED
    int line;
    const char *file;
    const char *func;
#endif
} vsf_liunx_heap_node_t;

typedef struct vsf_linux_heap_monitor_t {
    size_t usage;
#   if VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_MAX == ENABLED
    size_t max_usage;
#   endif
    int balance;
    vsf_dlist_t list;
} vsf_linux_heap_monitor_t;
#endif

vsf_class(vsf_linux_process_t) {
    public_member(
        vsf_linux_process_ctx_t ctx;

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
        char **__environ;
#endif
#if VSF_LINUX_USE_VFORK == ENABLED
        jmp_buf __vfork_jmpbuf;
#endif
    )

    protected_member(
#if VSF_ARCH_USE_THREAD_REG == ENABLED
        uintptr_t reg;
#endif
        int status;
        int exit_status;
        vsf_linux_process_t *shell_process;
        // thread pending child process
        vsf_linux_thread_t *thread_pending_child;
        struct {
            pid_t pid;
            pid_t ppid;
            pid_t gid;
        } id;
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
        vsf_linux_timer_t timers[ITIMER_NUM];
#endif

#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
        vsf_linux_heap_monitor_t heap_monitor;
#endif
        int fn_atexit_num;
        void (*fn_atexit[VSF_LINUX_CFG_ATEXIT_NUM])(void);

        char *working_dir;
#define __VSF_LINUX_PROCESS_HAS_PATH        (VSF_LINUX_USE_PROCFS == ENABLED || VSF_LINUX_USE_APPLET == ENABLED || VSF_LINUX_USE_SCRIPT == ENABLED)
#if __VSF_LINUX_PROCESS_HAS_PATH
        char path[PATH_MAX];
#endif

#if VSF_USE_LOADER == ENABLED
        void *loader;
#endif
    )

    private_member(
        vsf_dlist_node_t process_node;
        vsf_dlist_t thread_list;
        vsf_dlist_t fd_list;
        // thread pending this process
        vsf_linux_thread_t *thread_pending;
        vsf_bitmap(vsf_linux_fd_bitmap) fd_bitmap;

        struct {
            sigset_t pending;
            sigset_t mask;
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
            vsf_dlist_t handler_list;
            vsf_dlist_t trigger_list;
#endif
        } sig;

        vsf_prio_t prio;

        vsf_linux_thread_t *thread_pending_exit;
        vsf_linux_process_t *parent_process;
        vsf_dlist_t child_list;
        vsf_dlist_node_t child_node;

        vsf_heap_t *heap;
#if VSF_LINUX_USE_VFORK == ENABLED
        vsf_linux_process_t *vfork_child;
        bool is_vforking;
#endif
#if VSF_LINUX_CFG_PLS_NUM > 0
        struct {
            vsf_bitmap(vsf_linux_pls_bitmap) dynlib_bitmap;
            vsf_linux_localstorage_t storage[VSF_LINUX_CFG_PLS_NUM];
        } pls;
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

#if VSF_LINUX_USE_APPLET == ENABLED
typedef struct vsf_linux_fundmental_vplt_t {
    vsf_vplt_info_t info;

#if VSF_LINUX_CFG_PLS_NUM > 0
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_linux_dynlib_ctx_get);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_linux_dynlib_ctx_set);
#endif

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_linux_get_cur_process);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(vsf_linux_loader_keep_ram);
} vsf_linux_fundmental_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_fundmental_vplt_t vsf_linux_fundmental_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_LINUX_FUNDMENTAL_LIB__))\
    &&  VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_USE_APPLET == ENABLED

#ifndef VSF_LINUX_APPLET_FUNDMENTAL_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_FUNDMENTAL_VPLT                                \
            ((vsf_linux_fundmental_vplt_t *)(VSF_LINUX_APPLET_VPLT->fundmental_vplt))
#   else
#       define VSF_LINUX_APPLET_FUNDMENTAL_VPLT                                \
            ((vsf_linux_fundmental_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_FUNDMENTAL_ENTRY(__NAME)                               \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_FUNDMENTAL_VPLT, __NAME)
#define VSF_LINUX_APPLET_FUNDMENTAL_IMP(...)                                   \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_FUNDMENTAL_VPLT, __VA_ARGS__)

#if VSF_LINUX_CFG_PLS_NUM > 0
VSF_LINUX_APPLET_FUNDMENTAL_IMP(vsf_linux_dynlib_ctx_get, void *, const vsf_linux_dynlib_mod_t *mod) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_FUNDMENTAL_ENTRY(vsf_linux_dynlib_ctx_get)(mod);
}
VSF_LINUX_APPLET_FUNDMENTAL_IMP(vsf_linux_dynlib_ctx_set, int, const vsf_linux_dynlib_mod_t *mod, void *ctx) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_FUNDMENTAL_ENTRY(vsf_linux_dynlib_ctx_set)(mod, ctx);
}
#endif

VSF_LINUX_APPLET_FUNDMENTAL_IMP(vsf_linux_get_cur_process, vsf_linux_process_t *, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_FUNDMENTAL_ENTRY(vsf_linux_get_cur_process)();
}
#   if VSF_USE_LOADER == ENABLED
VSF_LINUX_APPLET_FUNDMENTAL_IMP(vsf_linux_loader_keep_ram, void, void) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    VSF_LINUX_APPLET_FUNDMENTAL_ENTRY(vsf_linux_loader_keep_ram)();
}
#   endif

#else

#if VSF_LINUX_CFG_PLS_NUM > 0
extern void * vsf_linux_dynlib_ctx_get(const vsf_linux_dynlib_mod_t *mod);
extern int vsf_linux_dynlib_ctx_set(const vsf_linux_dynlib_mod_t *mod, void *ctx);

// for backward compatibility
#define vsf_linux_dynlib_ctx        vsf_linux_dynlib_ctx_get
#endif

// open vsf_linux_get_cur_process for process-related variables like optarg, etc
extern vsf_linux_process_t * vsf_linux_get_cur_process(void);
#   if VSF_USE_LOADER == ENABLED
extern void vsf_linux_loader_keep_ram(void);
#   endif

#endif

// IMPORTANT: priority of stdio_stream MUST be within scheduler priorities
extern vsf_err_t vsf_linux_init(vsf_linux_stdio_stream_t *stdio_stream);

extern vsf_systimer_tick_t vsf_linux_sleep(vsf_timeout_tick_t ticks);

// used for dynamic libraries, allocate/free memory from resources_process
extern vsf_linux_process_t * vsf_linux_resources_process(void);

extern size_t vsf_linux_process_heap_size(vsf_linux_process_t *process, void *buffer);
extern void * vsf_linux_process_heap_realloc(vsf_linux_process_t *process, void *buffer, uint_fast32_t size);
extern void * vsf_linux_process_heap_malloc_aligned(vsf_linux_process_t *process, uint_fast32_t size, uint_fast32_t alignment);
extern void * vsf_linux_process_heap_malloc(vsf_linux_process_t *process, size_t size);
extern void * vsf_linux_process_heap_calloc(vsf_linux_process_t *process, size_t n, size_t size);
extern void vsf_linux_process_heap_free(vsf_linux_process_t *process, void *buffer);
extern char * vsf_linux_process_heap_strdup(vsf_linux_process_t *process, char *str);

#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
extern void __free_ex(vsf_linux_process_t *process, void *ptr);
#   if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED
#       define __malloc_ex(__process, __size)           ____malloc_ex((__process), (__size), __FILE__, __FUNCTION__, __LINE__)
#       define __calloc_ex(__process, __n, __size)      ____calloc_ex((__process), (__n), (__size), __FILE__, __FUNCTION__, __LINE__)
#       define __realloc_ex(__process, __ptr, __size)   ____realloc_ex((__process), (__ptr), (__size), __FILE__, __FUNCTION__, __LINE__)
#       define __strdup_ex(__process, __str)            ____strdup_ex((__process), (char *)(__str), __FILE__, __FUNCTION__, __LINE__)
#   else
#       define __malloc_ex(__process, __size)           ____malloc_ex((__process), (__size), NULL, NULL, 0)
#       define __calloc_ex(__process, __n, __size)      ____calloc_ex((__process), (__n), (__size), NULL, NULL, 0)
#       define __realloc_ex(__process, __ptr, __size)   ____realloc_ex((__process), (__ptr), (__size), NULL, NULL, 0)
#       define __strdup_ex(__process, __str)            ____strdup_ex((__process), (char *)(__str), NULL, NULL, 0)
#   endif
#else
#   define __malloc_ex(__process, __size)               vsf_linux_process_heap_malloc((__process), (__size))
#   define __calloc_ex(__process, __n, __size)          vsf_linux_process_heap_calloc((__process), (__n), (__size))
#   define __free_ex(__process, __ptr)                  vsf_linux_process_heap_free((__process), (__ptr))
#   define __realloc_ex(__process, __ptr, __size)       vsf_linux_process_heap_realloc((__process), (__ptr), (__size))
#   define __strdup_ex(__process, __str)                vsf_linux_process_heap_strdup((__process), (char *)(__str))
#endif

extern vsf_eda_t * vsf_linux_get_kernel_task(void);
extern void * vsf_linux_malloc_res(size_t size);
extern void vsf_linux_free_res(void *ptr);

extern int vsf_linux_generate_path(char *path_out, int path_out_lenlen, char *dir, char *path_in);
extern int vsf_linux_chdir(vsf_linux_process_t *process, char *working_dir);

extern int vsf_linux_fd_bind_executable(int fd, vsf_linux_main_entry_t entry);
extern int vsf_linux_fs_bind_executable(char *path, vsf_linux_main_entry_t entry);

extern int vsf_linux_expandenv(const char *str, char *output, size_t bufflen);

#if VSF_LINUX_CFG_PLS_NUM > 0
extern vsf_err_t vsf_linux_library_init(int *lib_idx, void *lib_ctx, void (*destructor)(void *));
extern void * vsf_linux_library_ctx(int lib_idx);

extern vsf_err_t vsf_linux_dynlib_init(int *lib_idx, int module_num, int bss_size);
#endif

// pls(process local storage) is open to user
#if VSF_LINUX_CFG_PLS_NUM > 0
extern int vsf_linux_pls_alloc(void);
extern void vsf_linux_pls_free(int idx);
extern vsf_linux_localstorage_t * vsf_linux_pls_get(int idx);
#endif

#if defined(__VSF_LINUX_CLASS_IMPLEMENT) || defined(__VSF_LINUX_CLASS_INHERIT__)
#   if VSF_LINUX_CFG_TLS_NUM > 0
extern int vsf_linux_tls_alloc(void (*destructor)(void *));
extern void vsf_linux_tls_free(int idx);
extern vsf_linux_localstorage_t * vsf_linux_tls_get(int idx);
#   endif

extern bool vsf_linux_is_inited(void);
// will get vfork_child is current process is vforking
extern vsf_linux_process_t * vsf_linux_get_real_process(vsf_linux_process_t *process);

#define __vsf_linux_start_process_internal3(__entry, __argv, __stack_size, __prio)\
            __vsf_linux_start_process_internal((__entry), (__argv), (__stack_size), (__prio))
#define __vsf_linux_start_process_internal2(__entry, __argv, __stack_size)      \
            __vsf_linux_start_process_internal3((__entry), (__argv), (__stack_size), VSF_LINUX_CFG_PRIO_LOWEST)
#define __vsf_linux_start_process_internal1(__entry, __argv)                    \
            __vsf_linux_start_process_internal2((__entry), (__argv), 0)
#define __vsf_linux_start_process_internal0(__entry)                            \
            __vsf_linux_start_process_internal1(__entry, NULL)
// prototype: vsf_linux_process_t * vsf_linux_start_process_internal(vsf_linux_main_entry_t *entry,
//        char * const * argv = NULL, int stack_size = 0, vsf_prio_t prio = VSF_LINUX_CFG_PRIO_LOWEST);
#define vsf_linux_start_process_internal(__entry, ...)                          \
            __PLOOC_EVAL(__vsf_linux_start_process_internal, __VA_ARGS__)((__entry), ##__VA_ARGS__)
extern vsf_linux_process_t * __vsf_linux_start_process_internal(
        vsf_linux_main_entry_t entry, char * const * argv, int stack_size, vsf_prio_t prio);

extern vsf_linux_main_entry_t * vsf_linux_fd_get_executable(int fd);
extern int vsf_linux_fs_get_executable(const char *pathname, vsf_linux_main_entry_t *entry);

// delete unstarted/existed process
extern vsf_linux_process_t * vsf_linux_create_process(int stack_size, int heap_size, int priv_size);
extern void vsf_linux_delete_process(vsf_linux_process_t *process);
extern int vsf_linux_start_process(vsf_linux_process_t *process);
extern VSF_CAL_NO_RETURN void vsf_linux_exit_process(int status, bool _exit);
#if VSF_ARCH_USE_THREAD_REG == ENABLED
extern uintptr_t vsf_linux_set_process_reg(uintptr_t reg);
#endif

extern void vsf_linux_cleanup_process(vsf_linux_process_t *process);
extern void vsf_linux_detach_process(vsf_linux_process_t *process);
extern void vsf_linux_detach_thread(vsf_linux_thread_t *thread);
extern int vsf_linux_wait_thread(int tid, int *retval);

extern vsf_linux_thread_t * vsf_linux_create_thread(vsf_linux_process_t *process,
            const vsf_linux_thread_op_t *op,
            int stack_size, void *stack);
extern int vsf_linux_start_thread(vsf_linux_thread_t *thread, vsf_prio_t priority);

extern void vsf_linux_thread_on_terminate(vsf_linux_thread_t *thread);
extern vsf_linux_thread_t * vsf_linux_get_thread(pid_t pid, int tid);
extern vsf_linux_process_t * vsf_linux_get_process(pid_t pid);

extern void __vsf_linux_process_free_arg(vsf_linux_process_t *process);
extern int __vsf_linux_process_parse_arg(vsf_linux_process_t *process, vsf_linux_process_arg_t *arg, char * const * argv);

extern int vsf_linux_merge_env(vsf_linux_process_t *process, char **env);
extern void vsf_linux_free_env(vsf_linux_process_t *process);

extern void vsf_linux_trigger_init(vsf_linux_trigger_t *trig);
// return value:
//  < 0 : triggered by signals, returns the minus signal
//  0   : triggered normally
//  > 0 : triggered because of timeout
extern int vsf_linux_trigger_pend(vsf_linux_trigger_t *trig, vsf_timeout_tick_t timeout);
extern int vsf_linux_trigger_signal(vsf_linux_trigger_t *trig, int sig);
#endif

// open vsf_linux_get_cur_thread for thread-related variables like errno, etc
extern vsf_linux_thread_t * vsf_linux_get_cur_thread(void);

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