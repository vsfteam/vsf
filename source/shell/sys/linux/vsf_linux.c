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

/*============================ INCLUDES ======================================*/

#include "./vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
#define __VSF_FS_CLASS_INHERIT__
#define __VSF_HEAP_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_IMPLEMENT
#define __VSF_LINUX_CLASS_IMPLEMENT

#include <vsf.h>

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./include/unistd.h"
#   include "./include/sched.h"
#   include "./include/pty.h"
#   include "./include/semaphore.h"
#   include "./include/signal.h"
#   include "./include/sys/wait.h"
#   include "./include/sys/ipc.h"
#   include "./include/sys/sem.h"
#   include "./include/sys/mman.h"
#   include "./include/sys/shm.h"
#   include "./include/sys/random.h"
#   include "./include/sys/stat.h"
#   include "./include/sys/times.h"
#   include "./include/sys/prctl.h"
#   include "./include/sys/reboot.h"
#   include "./include/sys/capability.h"
#   include "./include/sys/timex.h"
#   include "./include/sys/resource.h"
#   include "./include/sys/sysinfo.h"
#   include "./include/sys/xattr.h"
#   include "./include/sys/timeb.h"
#   include "./include/fcntl.h"
#   include "./include/errno.h"
#   include "./include/termios.h"
#   include "./include/pwd.h"
#   include "./include/sys/utsname.h"
#   include "./include/sys/ioctl.h"
#   include "./include/spawn.h"
#   include "./include/langinfo.h"
#   include "./include/poll.h"
#   include "./include/dlfcn.h"
#   include "./include/grp.h"
#   include "./include/fnmatch.h"
#   include "./include/linux/limits.h"
#   include "./include/linux/futex.h"
#   include "./include/linux/hrtimer.h"
#else
#   include <unistd.h>
#   include <sched.h>
#   include <pty.h>
#   include <semaphore.h>
#   include <signal.h>
#   include <sys/wait.h>
#   include <sys/ipc.h>
#   include <sys/sem.h>
#   include <sys/mman.h>
#   include <sys/shm.h>
#   include <sys/random.h>
#   include <sys/stat.h>
#   include <sys/times.h>
#   include <sys/prctl.h>
#   include <sys/reboot.h>
#   include <sys/capability.h>
#   include <sys/timex.h>
#   include <sys/resource.h>
#   include <sys/sysinfo.h>
#   include <sys/xattr.h>
#   include <sys/timeb.h>
#   include <fcntl.h>
#   include <errno.h>
#   include <termios.h>
#   include <pwd.h>
#   include <sys/utsname.h>
#   include <sys/ioctl.h>
#   include <spawn.h>
#   include <langinfo.h>
#   include <poll.h>
#   include <dlfcn.h>
#   include <grp.h>
#   include <fnmatch.h>
// for PATH_MAX
#   include <linux/limits.h>
#   include <linux/futex.h>
#   include <linux/hrtimer.h>
#endif
#include <stdarg.h>
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED
#   include "./include/simple_libc/stdlib.h"
#else
#   include <stdlib.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "./include/simple_libc/string.h"
#else
#   include <string.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
#   include "./include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_CTYPE == ENABLED
#   include "./include/simple_libc/ctype.h"
#else
#   include <ctype.h>
#endif
#if VSF_LINUX_USE_VFORK == ENABLED
#   define __SIMPLE_LIBC_SETJMP_VPLT_ONLY__
#   include "./include/simple_libc/setjmp/setjmp.h"
#endif

#include "./kernel/fs/vsf_linux_fs.h"

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE MUST be enabled for linux
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL MUST be enabled for linux
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_TIMER MUST be enabled for linux
#endif

#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED && VSF_KERNEL_CFG_THREAD_SIGNAL != ENABLED
#    error VSF_KERNEL_CFG_THREAD_SIGNAL MUST be enabled for VSF_LINUX_CFG_SUPPORT_SIG
#endif

#if VSF_LINUX_CFG_STACKSIZE < 512
#   warning You sure use this stack size for linux?
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_LINUX_CFG_SUPPORT_SHM == ENABLED
typedef struct vsf_linux_shm_mem_t {
    implement(vsf_linux_key_t)
    void *buffer;
    uint32_t size;
} vsf_linux_shm_mem_t;
#endif

typedef struct vsf_linux_sem_t {
    vsf_sync_t sync;
} vsf_linux_sem_t;
typedef struct vsf_linux_sem_set_t {
    implement(vsf_linux_key_t)
    int nsems;
    vsf_linux_sem_t sem[0];
} vsf_linux_sem_set_t;

#if VSF_LINUX_CFG_FUTEX_NUM > 0
typedef struct vsf_linux_futex_t {
    vsf_dlist_node_t node;
    uint32_t *futex;
    vsf_trig_t trig;
} vsf_linux_futex_t;

dcl_vsf_pool(vsf_linux_futex_pool)
def_vsf_pool(vsf_linux_futex_pool, vsf_linux_futex_t)
#endif

typedef struct vsf_linux_t {
    int cur_tid;
    int cur_pid;
    vsf_dlist_t process_list;

    vsf_linux_process_t process_for_resources;
    vsf_linux_process_t *kernel_process;

#if VSF_LINUX_CFG_SUPPORT_SHM == ENABLED
    struct {
        vsf_dlist_t list;
    } shm;
#endif
#if VSF_LINUX_CFG_SUPPORT_SEM == ENABLED
    struct {
        vsf_dlist_t list;
    } sem;
#endif

#if VSF_LINUX_CFG_PLS_NUM > 0
    struct {
        vsf_bitmap(vsf_linux_pls_bitmap) bitmap;
        uint16_t ref[VSF_LINUX_CFG_PLS_NUM];
    } pls;
#endif

#if VSF_LINUX_CFG_FUTEX_NUM > 0
    struct {
        vsf_dlist_t list;
        vsf_pool(vsf_linux_futex_pool) pool;
    } futex;
#endif

    long hostid;
    char hostname[HOST_NAME_MAX + 1];

    vsf_eda_t task;
} vsf_linux_t;

typedef struct vsf_linux_process_heap_t {
    vsf_heap_t heap;
    vsf_dlist_t freelist[2];
} vsf_linux_process_heap_t;

/*============================ GLOBAL VARIABLES ==============================*/

static const struct passwd __vsf_linux_default_passwd = {
    .pw_name            = "vsf",
    .pw_passwd          = "vsf",
    .pw_uid             = (uid_t)0,
    .pw_gid             = (gid_t)0,
    .pw_gecos           = "vsf",
    .pw_dir             = "/home/root",
    .pw_shell           = "sh",
};

static const char *__vsf_linux_default_group_users[] = {
    "vsf",
    NULL,
};
static const struct group __vsf_linux_default_group = {
    .gr_name            = "vsfteam",
    .gr_gid             = (gid_t)0,
    .gr_mem             = (char **)__vsf_linux_default_group_users,
};

/*============================ PROTOTYPES ====================================*/

extern int vsf_linux_create_fhs(void);

#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
extern void vsf_linux_glibc_init(void);
#endif

static void __vsf_linux_main_on_run(vsf_thread_cb_t *cb);

#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
static void __vsf_linux_sighandler(vsf_thread_t *thread, int sig);
#endif

// private APIs in other files
extern int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
            const vsf_linux_fd_op_t *op, int fd_desired, vsf_linux_fd_priv_t *priv);
extern vsf_linux_fd_t * __vsf_linux_fd_get_ex(vsf_linux_process_t *process, int fd);
extern void __vsf_linux_fd_delete_ex(vsf_linux_process_t *process, int fd);
extern void ____vsf_linux_fd_delete_ex(vsf_linux_process_t *process, vsf_linux_fd_t *sfd);
extern int __vsf_linux_fd_close_ex(vsf_linux_process_t *process, int fd);
extern int ____vsf_linux_fd_close_ex(vsf_linux_process_t *process, vsf_linux_fd_t *sfd);
extern vk_file_t * __vsf_linux_get_fs_ex(vsf_linux_process_t *process, int fd);

extern void __vsf_linux_rx_stream_init(vsf_linux_stream_priv_t *priv_tx);
extern void __vsf_linux_tx_stream_init(vsf_linux_stream_priv_t *priv_rx);
extern void __vsf_linux_tx_stream_drop(vsf_linux_stream_priv_t *priv_tx);
extern void __vsf_linux_rx_stream_drop(vsf_linux_stream_priv_t *priv_rx);
extern void __vsf_linux_tx_stream_drain(vsf_linux_stream_priv_t *priv_tx);
extern void __vsf_linux_stream_evt(vsf_linux_stream_priv_t *priv, vsf_protect_t orig, short event, bool is_ready);
extern const vsf_linux_fd_op_t __vsf_linux_stream_fdop;

extern void __vsf_eda_sync_pend(vsf_sync_t *sync, vsf_eda_t *eda, vsf_timeout_tick_t timeout);

/*============================ LOCAL VARIABLES ===============================*/

#if VSF_LINUX_CFG_HEAP_SIZE > 0
struct __vsf_linux_heap_t {
    implement(vsf_heap_t)
#ifndef VSF_LINUX_CFG_HEAP_ADDR
    uint8_t memory[VSF_LINUX_CFG_HEAP_SIZE];
#endif
    vsf_dlist_t freelist[2];
} static __vsf_linux_heap;
#endif

static vsf_linux_t __vsf_linux = {
    .cur_pid                = -1,

#if VSF_LINUX_CFG_HEAP_SIZE > 0
    .process_for_resources  = {
        .heap               = &__vsf_linux_heap.use_as__vsf_heap_t,
    },
#endif
};

const vsf_linux_thread_op_t __vsf_linux_main_op = {
    .priv_size              = 0,
    .on_run                 = __vsf_linux_main_on_run,
    .on_terminate           = vsf_linux_thread_on_terminate,
};

/*============================ IMPLEMENTATION ================================*/

#if VSF_LINUX_CFG_HEAP_SIZE > 0
static vsf_dlist_t * __vsf_linux_heap_get_freelist(vsf_heap_t *heap, uint_fast32_t size)
{
    return &__vsf_linux_heap.freelist[0];
}

static void __vsf_linux_heap_init(void)
{
    memset(&__vsf_linux_heap.use_as__vsf_heap_t, 0, sizeof(__vsf_linux_heap.use_as__vsf_heap_t));
    for (uint_fast8_t i = 0; i < dimof(__vsf_linux_heap.freelist); i++) {
        vsf_dlist_init(&__vsf_linux_heap.freelist[i]);
    }
    __vsf_linux_heap.get_freelist = __vsf_linux_heap_get_freelist;
#ifndef VSF_LINUX_CFG_HEAP_ADDR
    __vsf_heap_add_buffer(&__vsf_linux_heap.use_as__vsf_heap_t, __vsf_linux_heap.memory, VSF_LINUX_CFG_HEAP_SIZE);
#else
    __vsf_heap_add_buffer(&__vsf_linux_heap.use_as__vsf_heap_t, (uint8_t *)VSF_LINUX_CFG_HEAP_ADDR, VSF_LINUX_CFG_HEAP_SIZE);
#endif
}
#endif

vsf_linux_process_t * vsf_linux_get_real_process(vsf_linux_process_t *process)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
        if (NULL == process) {
            return NULL;
        }
    }

#if VSF_LINUX_USE_VFORK == ENABLED
    if (process->is_vforking) {
        process = process->vfork_child;
    }
#endif
    return process;
}

#if VSF_LINUX_CFG_PLS_NUM > 0
int vsf_linux_pls_alloc(void)
{
    vsf_protect_t orig = vsf_protect_sched();
    int_fast32_t idx = vsf_bitmap_ffz(&__vsf_linux.pls.bitmap, VSF_LINUX_CFG_PLS_NUM);
    if (idx < 0) {
        vsf_unprotect_sched(orig);
        return -1;
    }
    vsf_bitmap_set(&__vsf_linux.pls.bitmap, idx);
    vsf_unprotect_sched(orig);
    return idx;
}

void vsf_linux_pls_free(int idx)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_LINUX_ASSERT(vsf_bitmap_get(&__vsf_linux.pls.bitmap, idx));
        vsf_bitmap_clear(&__vsf_linux.pls.bitmap, idx);
        process->pls.storage[idx].data = NULL;
        process->pls.storage[idx].destructor = NULL;
    vsf_unprotect_sched(orig);
}

void vsf_linux_pls_ref(int idx)
{
    vsf_protect_t orig = vsf_protect_sched();
    __vsf_linux.pls.ref[idx]++;
    vsf_unprotect_sched(orig);
}

void vsf_linux_pls_deref(int idx)
{
    vsf_protect_t orig = vsf_protect_sched();
    VSF_LINUX_ASSERT(__vsf_linux.pls.ref[idx] > 0);
    if (!--__vsf_linux.pls.ref[idx]) {
        vsf_linux_pls_free(idx);
    }
    vsf_unprotect_sched(orig);
}

vsf_linux_localstorage_t * vsf_linux_pls_get(int idx)
{
    VSF_LINUX_ASSERT((idx >= 0) && (idx < VSF_LINUX_CFG_PLS_NUM));

    vsf_protect_t orig = vsf_protect_sched();
        VSF_LINUX_ASSERT(vsf_bitmap_get(&__vsf_linux.pls.bitmap, idx));
    vsf_unprotect_sched(orig);

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    return &process->pls.storage[idx];
}

vsf_err_t vsf_linux_library_init(int *lib_idx, void *lib_ctx, void (*destructor)(void *))
{
    VSF_LINUX_ASSERT(lib_idx != NULL);
    vsf_protect_t orig = vsf_protect_sched();
    if (*lib_idx < 0) {
        *lib_idx = vsf_linux_pls_alloc();
        if (*lib_idx < 0) {
            vsf_unprotect_sched(orig);
            vsf_trace_error("linux: fail to allocate pls for library" VSF_TRACE_CFG_LINEEND);
            return VSF_ERR_NOT_ENOUGH_RESOURCES;
        }
    }
    vsf_unprotect_sched(orig);

    vsf_linux_localstorage_t *pls = vsf_linux_pls_get(*lib_idx);
    VSF_LINUX_ASSERT(pls != NULL);
    if (pls->data != NULL) {
        vsf_trace_warning("linux: can not initiazlize a initizlized library" VSF_TRACE_CFG_LINEEND);
        return VSF_ERR_ALREADY_EXISTS;
    }
    pls->data = lib_ctx;
    pls->destructor = destructor;
    return VSF_ERR_NONE;
}

void * vsf_linux_library_ctx(int lib_idx)
{
    if (lib_idx < 0) {
        return NULL;
    }
    vsf_linux_localstorage_t *pls = vsf_linux_pls_get(lib_idx);
    VSF_LINUX_ASSERT(pls != NULL);
    return pls->data;
}

vsf_err_t vsf_linux_dynlib_init(int *lib_idx, int module_num, int bss_size)
{
    vsf_linux_dynlib_t *dynlib = calloc(1, sizeof(vsf_linux_dynlib_t) + module_num * sizeof(dynlib->modules[0]) + bss_size);
    if (NULL == dynlib) { return -1; }

    dynlib->module_num = module_num;
    dynlib->bss_size = bss_size;
    vsf_err_t err = vsf_linux_library_init(lib_idx, dynlib, free);
    if (err != VSF_ERR_NONE) {
        free(dynlib);
    }

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    if (process != NULL) {
        vsf_bitmap_set(&process->pls.dynlib_bitmap, *lib_idx);
    }
    vsf_linux_pls_ref(*lib_idx);
    return err;
}

void vsf_linux_dynlib_fini(int lib_idx)
{
    vsf_linux_dynlib_t *dynlib = vsf_linux_library_ctx(lib_idx);
    if (dynlib != NULL) {
        if (dynlib->lib_idx != NULL) {
            *dynlib->lib_idx = -1;
        }
        vsf_linux_pls_deref(lib_idx);
    }
}

static vsf_linux_dynlib_t * __vsf_linux_dynlib_alloc(const vsf_linux_dynlib_mod_t *mod)
{
    int lib_idx = NULL == mod->lib_idx ? 0 : *mod->lib_idx;
    vsf_linux_dynlib_t *result = vsf_linux_library_ctx(lib_idx);
    if (result != NULL) {
        return result;
    }

    if (vsf_linux_dynlib_init(&lib_idx, mod->module_num, mod->bss_size) < 0) {
        vsf_trace_error("linux: fail to allocate dynlib" VSF_TRACE_CFG_LINEEND);
        VSF_LINUX_ASSERT(false);
        return NULL;
    }
    if (mod->lib_idx != NULL) {
        *mod->lib_idx = lib_idx;
    }
    result = vsf_linux_library_ctx(lib_idx);
    result->lib_idx = mod->lib_idx;
    return result;
}

int vsf_linux_dynlib_ctx_set(const vsf_linux_dynlib_mod_t *mod, void *ctx)
{
    VSF_LINUX_ASSERT(mod != NULL);
    vsf_linux_dynlib_t *dynlib = __vsf_linux_dynlib_alloc(mod);
    VSF_LINUX_ASSERT(dynlib != NULL);
    VSF_LINUX_ASSERT(mod->mod_idx < dynlib->module_num);

    dynlib->modules[mod->mod_idx] = ctx;
    return 0;
}

void * vsf_linux_dynlib_ctx_get(const vsf_linux_dynlib_mod_t *mod)
{
    VSF_LINUX_ASSERT(mod != NULL);
    vsf_linux_dynlib_t *dynlib = __vsf_linux_dynlib_alloc(mod);
    VSF_LINUX_ASSERT(dynlib != NULL);
    VSF_LINUX_ASSERT(mod->mod_idx < dynlib->module_num);

    void *ctx = dynlib->modules[mod->mod_idx];
    if (NULL == ctx) {
        uint32_t mod_size = (mod->mod_size + (sizeof(int) - 1)) & ~(sizeof(int) - 1);
        VSF_LINUX_ASSERT(dynlib->bss_size >= dynlib->bss_brk + mod_size);
        ctx = (void *)((uint8_t *)&dynlib->modules[dynlib->module_num] + dynlib->bss_brk);
        dynlib->bss_brk += mod_size;
        dynlib->modules[mod->mod_idx] = ctx;
        if (mod->init != NULL) {
            mod->init(ctx);
        }
    }
    return ctx;
}
#endif

#if VSF_LINUX_CFG_TLS_NUM > 0
int vsf_linux_tls_alloc(void (*destructor)(void*))
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    vsf_protect_t orig = vsf_protect_sched();
    int_fast32_t idx = vsf_bitmap_ffz(&process->tls.bitmap, VSF_LINUX_CFG_TLS_NUM);
    if (idx < 0) {
        vsf_unprotect_sched(orig);
        return -1;
    }
    vsf_bitmap_set(&process->tls.bitmap, idx);
    vsf_unprotect_sched(orig);

    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);

    thread->tls[idx].destructor = destructor;
    thread->tls[idx].data = NULL;
    return idx;
}

void vsf_linux_tls_free(int idx)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_LINUX_ASSERT(vsf_bitmap_get(&process->tls.bitmap, idx));
        vsf_bitmap_clear(&process->tls.bitmap, idx);
    vsf_unprotect_sched(orig);

    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);

    thread->tls[idx].destructor = NULL;
    thread->tls[idx].data = NULL;
}

vsf_linux_localstorage_t * vsf_linux_tls_get(int idx)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    vsf_protect_t orig = vsf_protect_sched();
        VSF_LINUX_ASSERT(vsf_bitmap_get(&process->tls.bitmap, idx));
    vsf_unprotect_sched(orig);

    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);

    return &thread->tls[idx];
}
#endif

#if VSF_LINUX_CFG_SUPPORT_SEM == ENABLED || VSF_LINUX_CFG_SUPPORT_SHM == ENABLED
static void __vsf_linux_keyfree(vsf_dlist_t *list, vsf_linux_key_t *key)
{
    vsf_protect_t orig = vsf_protect_sched();
    vsf_dlist_remove(vsf_linux_key_t, node, list, key);
    vsf_unprotect_sched(orig);
    vsf_linux_free_res(key);
}

static vsf_linux_key_t * __vsf_linux_keyget(vsf_dlist_t *list, key_t key, size_t size, int flags)
{
    vsf_linux_key_t *result = NULL;
    vsf_protect_t orig = vsf_protect_sched();
    __vsf_dlist_foreach_unsafe(vsf_linux_key_t, node, list) {
        if (_->key == key) {
            result = _;
        }
    }
    vsf_unprotect_sched(orig);
    if ((result != NULL) && (flags & IPC_EXCL)) {
        return NULL;
    }

    if (NULL == result) {
        if (flags & IPC_CREAT) {
            result = vsf_linux_malloc_res(size);
            if (result != NULL) {
                memset(result, 0, size);
                result->key = key;
                vsf_dlist_init_node(vsf_linux_key_t, node, result);
                if (key != IPC_PRIVATE) {
                    orig = vsf_protect_sched();
                    vsf_dlist_add_to_head(vsf_linux_key_t, node, list, result);
                    vsf_unprotect_sched(orig);
                }
            }
        } else {
            errno = ENOENT;
        }
    }
    return result;
}
#endif

#if VSF_LINUX_CFG_FUTEX_NUM > 0
//implement_vsf_pool(vsf_linux_futex_pool, vsf_linux_futex_t)
#define __name vsf_linux_futex_pool
#define __type vsf_linux_futex_t
#include "service/pool/impl_vsf_pool.inc"
#endif

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
int vsf_linux_merge_env(vsf_linux_process_t *process, char **env)
{
    extern int __putenv_ex(vsf_linux_process_t *process, char *string);
    char *cur_env;
    if (env != NULL) {
        while (*env != NULL) {
            cur_env = __strdup_ex(process, *env);
            if (cur_env != NULL) {
                __putenv_ex(process, cur_env);
            } else {
                vsf_trace_error("spawn: failed to dup env %s", VSF_TRACE_CFG_LINEEND, *env);
                return -1;
            }
            env++;
        }
    }
    return 0;
}

void vsf_linux_free_env(vsf_linux_process_t *process)
{
    char **environ_tmp = process->__environ;
    if (environ_tmp != NULL) {
        while (*environ_tmp != NULL) {
            __free_ex(process, *environ_tmp);
            environ_tmp++;
        }
        __free_ex(process, process->__environ);
    }
}
#endif

void vsf_linux_trigger_init(vsf_linux_trigger_t *trig)
{
    vsf_eda_trig_init(&trig->use_as__vsf_trig_t, false, true);
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    vsf_dlist_init_node(vsf_linux_trigger_t, node, trig);
    trig->pending_process = NULL;
#endif
}

int vsf_linux_trigger_signal(vsf_linux_trigger_t *trig, int sig)
{
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    vsf_linux_process_t *pending_process;
    vsf_protect_t orig = vsf_protect_sched();
    pending_process = trig->pending_process;
    if (NULL == pending_process) {
        vsf_unprotect_sched(orig);
        return 0;
    }

    trig->sig = sig;
    trig->pending_process = NULL;
    vsf_dlist_remove(vsf_linux_trigger_t, node, &pending_process->sig.trigger_list, trig);
    vsf_unprotect_sched(orig);
#endif
    vsf_eda_trig_set_isr(&trig->use_as__vsf_trig_t);
    return 0;
}

int vsf_linux_trigger_pend(vsf_linux_trigger_t *trig, vsf_timeout_tick_t timeout)
{
    vsf_protect_t orig;
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    trig->sig = 0;
    orig = vsf_protect_sched();
        vsf_dlist_add_to_tail(vsf_linux_trigger_t, node, &process->sig.trigger_list, trig);
        VSF_LINUX_ASSERT(NULL == trig->pending_process);
        trig->pending_process = process;
    vsf_unprotect_sched(orig);

    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    thread->trigger_pending = trig;
#endif

    vsf_sync_reason_t r = vsf_thread_trig_pend(&trig->use_as__vsf_trig_t, timeout);
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    thread->trigger_pending = NULL;
#endif
    orig = vsf_protect_sched();
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    trig->pending_process = NULL;
#endif
    if (VSF_SYNC_TIMEOUT == r) {
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
        vsf_dlist_remove(vsf_linux_trigger_t, node, &process->sig.trigger_list, trig);
#endif
        vsf_unprotect_sched(orig);
        return 1;
    }
    vsf_unprotect_sched(orig);
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    if (trig->sig) {
        errno = EINTR;
    }
    return trig->sig;
#else
    return 0;
#endif
}

VSF_CAL_WEAK(vsf_linux_create_fhs_user)
int vsf_linux_create_fhs_user(void)
{
    return 0;
}

VSF_CAL_WEAK(vsf_linux_create_fhs)
int vsf_linux_create_fhs(void)
{
    vsf_linux_vfs_init();
#if VSF_LINUX_USE_BUSYBOX == ENABLED
    busybox_install();
#endif
    return vsf_linux_create_fhs_user();
}

int * __vsf_linux_errno(void)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);
    return &thread->__errno;
}

int vsf_linux_generate_path(char *path_out, int path_out_lenlen, char *dir, char *path_in)
{
    char working_dir[PATH_MAX];
    if (NULL == dir) {
        getcwd(working_dir, sizeof(working_dir));
        dir = working_dir;
    }

    if (path_in[0] == '/') {
        if (strlen(path_in) >= path_out_lenlen) {
            return -ENOMEM;
        }
        strcpy(path_out, path_in);
    } else {
        if (path_in[0] == '~') {
            dir = getenv("HOME");
            if (NULL == dir) {
                VSF_LINUX_ASSERT(false);
                return -ENOENT;
            }

            if (path_in[1] != '\0') {
                if (path_in[1] != '/') {
                    return -ENOMEM;
                }
                path_in = &path_in[2];
            } else {
                path_in = &path_in[1];
            }
        }

        if (strlen(dir) + strlen(path_in) + 1 >= path_out_lenlen) {
            return -ENOMEM;
        }
        strcpy(path_out, dir);
        if (dir[strlen(dir) - 1] != '/') {
            strcat(path_out, "/");
        }
        strcat(path_out, path_in);
    }

    // process .. an ., process . first
    char *tmp, *tmp_replace;
    while ((tmp = (char *)strstr(path_out, "/./")) != NULL) {
        strcpy(tmp, &tmp[2]);
    }
    while ((tmp = (char *)strstr(path_out, "/..")) != NULL) {
        tmp[0] = '\0';
        tmp_replace = (char *)strrchr(path_out, '/');
        if (NULL == tmp_replace) {
            return -ENOENT;
        }
        strcpy(tmp_replace, &tmp[3]);
    }

    // fix surfix "/."
    size_t len = strlen(path_out);
    if ((len >= 2) && ('.' == path_out[len - 1]) && ('/' == path_out[len - 2])) {
        // for "/.", don't remove /, or it will be empty
        if (len == 2) {
            len++;
        }
        path_out[len - 2] = '\0';
    }
    return 0;
}

void __vsf_linux_process_free_arg(vsf_linux_process_t *process)
{
    vsf_linux_process_arg_t *arg = &process->ctx.arg;
    if (arg->argv != NULL) {
        for (int i = 0; i < arg->argc; i++) {
            __free_ex(process, (void *)arg->argv[i]);
            arg->argv[i] = NULL;
        }
        __free_ex(process, arg->argv);
        arg->argv = NULL;
    }
}

int __vsf_linux_process_parse_arg(vsf_linux_process_t *process, vsf_linux_process_arg_t *arg, char * const * argv)
{
    if (NULL == arg) {
        arg = &process->ctx.arg;
    }

    arg->argc = 0;
    char * const * argv_tmp = argv;
    while (*argv_tmp != NULL) {
        arg->argc++;
        argv_tmp++;
    }
    if (arg->argc > 0) {
        arg->argv = __malloc_ex(process, (arg->argc + 1) * sizeof(argv[0]));
        if (NULL == arg->argv) {
            return -1;
        }

        arg->argv[arg->argc] = NULL;
        for (int i = 0; i < arg->argc; i++, argv++) {
            arg->argv[i] = __strdup_ex(process, *argv);
            if (NULL == arg->argv[i]) {
                vsf_trace_error("linux: fail to allocate space for %s" VSF_TRACE_CFG_LINEEND, *argv);
                return -1;
            }
        }
    }
    return 0;
}

#if VSF_ARCH_USE_THREAD_REG == ENABLED
static void __vsf_linux_thread_on_run(vsf_thread_cb_t *cb)
{
    vsf_linux_thread_t *thread = vsf_container_of(cb, vsf_linux_thread_t, use_as__vsf_thread_cb_t);
    vsf_linux_process_t *process = thread->process;

    VSF_LINUX_ASSERT(process != NULL);
    vsf_arch_set_thread_reg(process->reg);
    thread->op->on_run(cb);
}
#endif

static void __vsf_linux_kernel_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    extern void __vsf_linux_eventfd_inc(vsf_linux_fd_priv_t *priv);
    extern void __vsf_linux_term_rx(vsf_linux_fd_priv_t *priv);

    switch (evt) {
    case __VSF_EVT_LINUX_EVENTFD_INC:
        __vsf_linux_eventfd_inc((vsf_linux_fd_priv_t *)vsf_eda_get_cur_msg());
        break;
    case __VSF_EVT_LINUX_TERM_RX:
        __vsf_linux_term_rx((vsf_linux_fd_priv_t *)vsf_eda_get_cur_msg());
        break;
    }
}

vsf_eda_t * vsf_linux_get_kernel_task(void)
{
    return &__vsf_linux.task;
}

static int __vsf_linux_init_thread(int argc, char *argv[])
{
    int err = vsf_linux_create_fhs();
    if (err) { return err; }

    // init shall be called in pid1
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    // IMPORTANT: the init process MUST be the first linux process and NO OTHER processes are running(can be created and exited).
    //  If asserted here, make sure no other processes are created and still running, espessially in the vsf_linux_create_fhs.
    VSF_LINUX_ASSERT((process->id.pid == 0) && (vsf_dlist_get_length(&__vsf_linux.process_list) == 1));
    process->id.pid = 1;
    __vsf_linux.cur_pid = 2;
    execlp(VSF_LINUX_CFG_BIN_PATH "/init", "init", NULL);

    printf("linux: missing init?\n");
    return -1;
}

static int __vsf_linux_kernel_thread(int argc, char *argv[])
{
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);
    vsf_kernel_trace_eda_info(&thread->use_as__vsf_eda_t, "linux_kernel_thread",
                                thread->stack, thread->stack_size);
#endif

    __vsf_linux.task.fn.evthandler = __vsf_linux_kernel_evthandler;
    __vsf_linux.task.on_terminate = NULL;
    vsf_eda_init(&__vsf_linux.task);

    __vsf_linux.kernel_process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(__vsf_linux.kernel_process != NULL);
    __vsf_linux_init_thread(argc, argv);
    return 0;
}

vsf_linux_process_t * vsf_linux_resources_process(void)
{
    return &__vsf_linux.process_for_resources;
}


void * vsf_linux_malloc_res(size_t size)
{
    return __malloc_ex(vsf_linux_resources_process(), size);
}

void vsf_linux_free_res(void *ptr)
{
    __free_ex(vsf_linux_resources_process(), ptr);
}

static void __vsf_linux_stderr_on_evt(vsf_linux_stream_priv_t *priv, vsf_protect_t orig, short event, bool is_ready)
{
    if (is_ready) {
        vsf_linux_fd_set_status(&priv->use_as__vsf_linux_fd_priv_t, event, orig);
    } else {
        vsf_linux_fd_clear_status(&priv->use_as__vsf_linux_fd_priv_t, event, orig);
    }
    if (event & POLLIN) {
        vsf_linux_stream_priv_t *stdin_priv = priv->target;
        if (stdin_priv != NULL) {
            __vsf_linux_stream_evt(stdin_priv, vsf_protect_sched(), event, is_ready);
        }
    }
}

bool vsf_linux_is_inited(void)
{
    return __vsf_linux.cur_pid >= 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

vsf_err_t vsf_linux_init(vsf_linux_stdio_stream_t *stdio_stream)
{
    VSF_LINUX_ASSERT(stdio_stream != NULL);
    __vsf_linux.cur_pid = 0;
#if VSF_LINUX_CFG_PLS_NUM > 0
    {
        int idx = vsf_linux_pls_alloc();
        VSF_LINUX_ASSERT(0 == idx);
    }
#endif
    vk_fs_init();

#if VSF_LINUX_CFG_HEAP_SIZE > 0
    __vsf_linux_heap_init();
#endif

#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
    vsf_linux_glibc_init();
#endif
#if VSF_LINUX_CFG_FUTEX_NUM > 0
    VSF_POOL_INIT(vsf_linux_futex_pool, &__vsf_linux.futex.pool, VSF_LINUX_CFG_FUTEX_NUM);
#endif

    sethostname(VSF_LINUX_CFG_HOSTNAME, strlen(VSF_LINUX_CFG_HOSTNAME));

    // setup stdio
    vsf_linux_fd_t *sfd;
    vsf_linux_stream_priv_t *priv;
    int ret;

    ret = __vsf_linux_fd_create_ex(vsf_linux_resources_process(), &sfd,
        &vsf_linux_term_fdop, STDIN_FILENO, NULL);
    VSF_LINUX_ASSERT(ret == STDIN_FILENO);
    priv = (vsf_linux_stream_priv_t *)sfd->priv;
    priv->stream_rx = stdio_stream->in;
    priv->flags = O_RDONLY;
    __vsf_linux_rx_stream_init(priv);
    ((vsf_linux_term_priv_t *)priv)->subop = &__vsf_linux_stream_fdop;
    vsf_linux_term_fdop.fn_init(sfd);

    ret = __vsf_linux_fd_create_ex(vsf_linux_resources_process(), &sfd,
        &vsf_linux_term_fdop, STDOUT_FILENO, NULL);
    VSF_LINUX_ASSERT(ret == STDOUT_FILENO);
    priv = (vsf_linux_stream_priv_t *)sfd->priv;
    priv->stream_tx = stdio_stream->out;
    priv->flags = O_WRONLY;
    __vsf_linux_tx_stream_init(priv);
    ((vsf_linux_term_priv_t *)priv)->subop = &__vsf_linux_stream_fdop;
    vsf_linux_term_fdop.fn_init(sfd);

    ret = __vsf_linux_fd_create_ex(vsf_linux_resources_process(), &sfd,
        &vsf_linux_term_fdop, STDERR_FILENO, NULL);
    VSF_LINUX_ASSERT(ret == STDERR_FILENO);
    priv = (vsf_linux_stream_priv_t *)sfd->priv;
    // stderr is initialized after stdin, so stdin events will be bound to stderr
    //  implement a evthandler to redirect to stdin if stdin stream_rx is same as stderr
    priv->on_evt = __vsf_linux_stderr_on_evt;
    priv->stream_rx = stdio_stream->in;
    priv->stream_tx = stdio_stream->err;
    __vsf_linux_rx_stream_init(priv);
    __vsf_linux_tx_stream_init(priv);
    ((vsf_linux_term_priv_t *)priv)->subop = &__vsf_linux_stream_fdop;
    vsf_linux_term_fdop.fn_init(sfd);

    // create kernel process(pid0)
    if (NULL != vsf_linux_start_process_internal(__vsf_linux_kernel_thread)) {
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

int isatty(int fd)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if (NULL == sfd) { return 0; }
    return sfd->op == &vsf_linux_term_fdop;
}

int ttyname_r(int fd, char *buf, size_t buflen)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if ((NULL == sfd) || (sfd->op != &vsf_linux_term_fdop)) {
        return -1;
    }

    vsf_linux_fs_priv_t *fs_priv = (vsf_linux_fs_priv_t *)sfd->priv;
    if (NULL == fs_priv->file) {
        buf[0] = '\0';
    } else {
        strlcpy(buf, fs_priv->file->name, buflen);
    }
    return 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

char *ttyname(int fd)
{
    static char __ttyname[64];
    ttyname_r(fd, __ttyname, sizeof(__ttyname));
    return __ttyname;
}

vsf_linux_thread_t * vsf_linux_create_raw_thread(const vsf_linux_thread_op_t *op,
            int stack_size, void *stack)
{
    vsf_linux_thread_t *thread;

    if (!stack_size) {
        stack_size = VSF_LINUX_CFG_STACKSIZE;
    }

    uint_fast32_t thread_size = op->priv_size + sizeof(*thread);
    uint_fast32_t alignment;
    uint_fast32_t all_size;
    if (NULL == stack) {
        stack_size += (1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1;
        stack_size &= ~((1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1);
        thread_size += (1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1;
        thread_size &= ~((1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1);
        all_size = thread_size + stack_size;
        all_size += (1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT) - 1;
        alignment = 1 << VSF_KERNEL_CFG_THREAD_STACK_ALIGN_BIT;
    } else {
        all_size = thread_size;
        alignment = 0;
    }

    thread = (vsf_linux_thread_t *)vsf_linux_process_heap_malloc_aligned(
            vsf_linux_resources_process(), all_size, alignment);
    if (thread != NULL) {
        memset(thread, 0, thread_size);

        // set entry and on_terminate
        thread->op = op;
        thread->on_terminate = (vsf_eda_on_terminate_t)op->on_terminate;
#if VSF_ARCH_USE_THREAD_REG == ENABLED
        thread->entry = __vsf_linux_thread_on_run;
#else
        thread->entry = (vsf_thread_entry_t *)op->on_run;
#endif
        thread->sighandler = __vsf_linux_sighandler;

        // set stack
        thread->stack_size = stack_size;
        if (stack != NULL) {
            thread->stack = stack;
        } else {
            thread->stack = (void *)((uintptr_t)thread + thread_size);
        }
    }
    return thread;
}

vsf_linux_thread_t * vsf_linux_create_thread(vsf_linux_process_t *process,
            const vsf_linux_thread_op_t *op,
            int stack_size, void *stack)
{
    vsf_linux_thread_t *thread = vsf_linux_create_raw_thread(op, stack_size, stack);
    if (thread != NULL) {
        if (!process) {
            process = vsf_linux_get_cur_process();
            VSF_LINUX_ASSERT(process != NULL);
        }
        thread->process = process;
        vsf_protect_t orig = vsf_protect_sched();
            thread->tid = __vsf_linux.cur_tid++;
            vsf_dlist_add_to_tail(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
        vsf_unprotect_sched(orig);
    }
    return thread;
}

int vsf_linux_start_thread(vsf_linux_thread_t *thread, vsf_prio_t priority)
{
    if (vsf_prio_inherit == priority) {
        priority = thread->process->prio;
    }
    vsf_thread_start(&thread->use_as__vsf_thread_t, &thread->use_as__vsf_thread_cb_t, priority);
    return 0;
}

static vsf_dlist_t * __vsf_linux_process_heap_get_freelist(vsf_heap_t *heap, uint_fast32_t size)
{
    vsf_linux_process_heap_t *process_heap = vsf_container_of(heap, vsf_linux_process_heap_t, heap);
    return &process_heap->freelist[0];
}

size_t vsf_linux_process_heap_size(vsf_linux_process_t *process, void *buffer)
{
    process = vsf_linux_get_real_process(process);
    // if process is NULL, means not in linux environment, maybe allocating for C++ static instance
    if (NULL == process || NULL == process->heap) {
        return vsf_heap_size(buffer);
    } else {
        return __vsf_heap_size(process->heap, buffer);
    }
}

void * vsf_linux_process_heap_realloc(vsf_linux_process_t *process, void *buffer, uint_fast32_t size)
{
    process = vsf_linux_get_real_process(process);
    // if process is NULL, means not in linux environment, maybe allocating for C++ static instance
    if (NULL == process || NULL == process->heap) {
        return vsf_heap_realloc(buffer, size);
    } else {
        if (NULL == buffer) {
            if (size > 0) {
                return vsf_linux_process_heap_malloc(process, size);
            }
            return NULL;
        } else if (0 == size) {
            if (buffer != NULL) {
                vsf_linux_process_heap_free(process, buffer);
            }
        }
        return __vsf_heap_realloc_aligned(process->heap, buffer, size, 0);
    }
}

void * vsf_linux_process_heap_malloc_aligned(vsf_linux_process_t *process, uint_fast32_t size, uint_fast32_t alignment)
{
    process = vsf_linux_get_real_process(process);
    // if process is NULL, means not in linux environment, maybe allocating for C++ static instance
    if (NULL == process || NULL == process->heap) {
        return vsf_heap_malloc_aligned(size, alignment);
    } else {
        return __vsf_heap_malloc_aligned(process->heap, size, alignment);
    }
}

void * vsf_linux_process_heap_malloc(vsf_linux_process_t *process, size_t size)
{
    process = vsf_linux_get_real_process(process);
    // if process is NULL, means not in linux environment, maybe allocating for C++ static instance
    if (NULL == process || NULL == process->heap) {
        return vsf_heap_malloc(size);
    } else {
        return __vsf_heap_malloc_aligned(process->heap, size, 0);
    }
}

void * vsf_linux_process_heap_calloc(vsf_linux_process_t *process, size_t n, size_t size)
{
    process = vsf_linux_get_real_process(process);
    size *= n;

    void *buffer = vsf_linux_process_heap_malloc(process, size);
    if (buffer != NULL) {
        memset(buffer, 0, size);
    }
    return buffer;
}

void vsf_linux_process_heap_free(vsf_linux_process_t *process, void *buffer)
{
    if (buffer != NULL) {
        process = vsf_linux_get_real_process(process);
        // if process is NULL, means not in linux environment, maybe allocating for C++ static instance
        if (NULL == process || NULL == process->heap) {
            vsf_heap_free(buffer);
        } else {
            __vsf_heap_free(process->heap, buffer);
        }
    }
}

char * vsf_linux_process_heap_strdup(vsf_linux_process_t *process, char *str)
{
    if (str != NULL) {
        int str_len = strlen(str);
        char *new_str = vsf_linux_process_heap_malloc(process, str_len + 1);
        if (new_str != NULL) {
            memcpy(new_str, str, str_len);
            new_str[str_len] = '\0';
        }
        return new_str;
    }
    return NULL;
}

static vsf_linux_process_t * __vsf_linux_create_process(int stack_size, int heap_size, int priv_size)
{
    heap_size += heap_size > 0 ? sizeof(vsf_linux_process_heap_t) : 0;
    priv_size = (priv_size + 15) & ~15;
    vsf_linux_process_t *process = vsf_linux_malloc_res(sizeof(vsf_linux_process_t) + priv_size + heap_size);
    if (process != NULL) {
        memset(process, 0, sizeof(*process));
        process->prio = vsf_prio_inherit;
        process->shell_process = process;

        vsf_linux_thread_t *thread = vsf_linux_create_thread(process, &__vsf_linux_main_op, stack_size, NULL);
        if (NULL == thread) {
            vsf_linux_free_res(process);
            return NULL;
        }

        vsf_protect_t orig = vsf_protect_sched();
            process->id.pid = __vsf_linux.cur_pid++;
            vsf_dlist_add_to_tail(vsf_linux_process_t, process_node, &__vsf_linux.process_list, process);
        vsf_unprotect_sched(orig);
        if (process->id.pid) {
            process->id.ppid = process->id.pid;
            if (process->parent_process != NULL) {
                process->id.gid = process->parent_process->id.gid;
            } else {
                process->id.gid = process->id.pid;
            }
        }

        if (heap_size > 0) {
            vsf_linux_process_heap_t *process_heap = (vsf_linux_process_heap_t *)((uintptr_t)&process[1] + priv_size);
            memset(process_heap, 0, sizeof(*process_heap));
            process_heap->heap.get_freelist = __vsf_linux_process_heap_get_freelist;
            process->heap = &process_heap->heap;
            __vsf_heap_add_buffer(&process_heap->heap, (uint8_t *)&process_heap[1], heap_size);
        }
#if VSF_LINUX_CFG_HEAP_SIZE > 0
        else {
            process->heap = __vsf_linux.process_for_resources.heap;
        }
#endif
    }
    return process;
}

void * __vsf_linux_get_process_priv(vsf_linux_process_t *process)
{
    if (NULL == process) {
        process = vsf_linux_get_cur_process();
        VSF_LINUX_ASSERT(process != NULL);
    }

    return &process[1];
}

vsf_linux_process_t * vsf_linux_create_process(int stack_size, int heap_size, int priv_size)
{
    vsf_linux_process_t *process = __vsf_linux_create_process(stack_size, heap_size, priv_size);
    if (process != NULL) {
        vsf_linux_process_t *cur_process = vsf_linux_get_cur_process();
        VSF_LINUX_ASSERT(cur_process != NULL);
        process->parent_process = cur_process;
        process->shell_process = cur_process->shell_process;

        vsf_protect_t orig = vsf_protect_sched();
            vsf_dlist_add_to_tail(vsf_linux_process_t, child_node, &cur_process->child_list, process);
        vsf_unprotect_sched(orig);

        process->working_dir = __strdup_ex(process, cur_process->working_dir);
        if (NULL == process->working_dir) {
            goto delete_process_and_fail;
        }

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
        if (vsf_linux_merge_env(process, cur_process->__environ) < 0) {
            goto delete_process_and_fail;
        }
#endif
    }
    return process;
delete_process_and_fail:
    vsf_linux_delete_process(process);
    return NULL;
}

int vsf_linux_start_process(vsf_linux_process_t *process)
{
    // TODO: check if already started
    vsf_linux_thread_t *thread;
    vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
    process->status = PID_STATUS_RUNNING;
    return vsf_linux_start_thread(thread, vsf_prio_inherit);
}

void vsf_linux_cleanup_process(vsf_linux_process_t *process)
{
    vsf_linux_fd_t *sfd;

#if VSF_LINUX_CFG_PLS_NUM > 0
    for (int i = 0; i < VSF_LINUX_CFG_PLS_NUM; i++) {
        if (vsf_bitmap_get(&__vsf_linux.pls.bitmap, i)) {
            if (process->pls.storage[i].destructor != NULL) {
                process->pls.storage[i].destructor(process->pls.storage[i].data);
            }
            if (vsf_bitmap_get(&process->pls.dynlib_bitmap, i)) {
                vsf_linux_dynlib_fini(i);
            }
        }
    }
#endif

    do {
        vsf_dlist_peek_head(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
        if (sfd != NULL) {
            // do not use close because it depends on current process
            //  and vsf_linux_delete_process can be called in other processes
            ____vsf_linux_fd_close_ex(process, sfd);
        }
    } while (sfd != NULL);

#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    __vsf_dlist_foreach_next_unsafe(vsf_linux_sig_handler_t, node, &process->sig.handler_list) {
        vsf_dlist_remove(vsf_linux_sig_handler_t, node, &process->sig.handler_list, _);
        __free_ex(process, _);
    }
#endif
#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
    vsf_linux_free_env(process);
#endif
    __vsf_linux_process_free_arg(process);
    if (process->working_dir != NULL) {
        __free_ex(process, process->working_dir);
    }

#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
#   if VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_MAX == ENABLED
    vsf_trace_info("max heap usage for process exited is %d" VSF_TRACE_CFG_LINEEND,
                process->heap_monitor.max_usage);
#   endif
    if (process->heap_monitor.usage != 0) {
#   if VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_QUIET != ENABLED
        vsf_trace_warning("memory leak %d bytes detected in process 0x%p, balance = %d" VSF_TRACE_CFG_LINEEND,
                process->heap_monitor.usage, process, process->heap_monitor.balance);
#   endif
        __vsf_dlist_foreach_next_unsafe(vsf_liunx_heap_node_t, node, &process->heap_monitor.list) {
#   if VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_QUIET != ENABLED
#       if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR_TRACE_CALLER == ENABLED
            vsf_trace_warning("    cleanup 0x%p(%d) %s %s %d" VSF_TRACE_CFG_LINEEND,
                _->ptr, _->size, _->file, _->func, _->line);
#       else
            vsf_trace_warning("    cleanup 0x%p(%d)" VSF_TRACE_CFG_LINEEND,
                _->ptr, _->size);
#       endif
#   endif
            free(_->ptr);
        }
    }
#endif
}

void vsf_linux_delete_process(vsf_linux_process_t *process)
{
    VSF_LINUX_ASSERT(process != NULL);
    VSF_LINUX_ASSERT(process->status == 0);

    __vsf_dlist_foreach_unsafe(vsf_linux_thread_t, thread_node, &process->thread_list) {
        vsf_dlist_remove(vsf_linux_thread_t, thread_node, &process->thread_list, _);
        vsf_linux_process_heap_free(process, _);
    }

    vsf_linux_cleanup_process(process);
    vsf_linux_detach_process(process);
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_remove(vsf_linux_process_t, process_node, &__vsf_linux.process_list, process);
    vsf_unprotect_sched(orig);

    vsf_linux_free_res(process);
}

VSF_CAL_NO_RETURN void vsf_linux_exit_process(int status, bool _exit)
{
    vsf_linux_thread_t *thread2wait;
    vsf_linux_thread_t *cur_thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(cur_thread != NULL);
    vsf_linux_process_t *process = cur_thread->process;
    VSF_LINUX_ASSERT(process != NULL);
    vsf_protect_t orig;

#if VSF_LINUX_USE_VFORK == ENABLED
    if (process->is_vforking) {
        vsf_linux_delete_process(process->vfork_child);
        process->vfork_child = NULL;
        process->is_vforking = false;
        longjmp(process->__vfork_jmpbuf, 1);
    }
#endif

    for (int i = 0; i < dimof(process->timers); i++) {
        vsf_callback_timer_remove(&process->timers[i].timer);
    }

    orig = vsf_protect_sched();
        if (process->thread_pending_exit != NULL) {
            vsf_unprotect_sched(orig);
            goto end_no_return;
        }
        process->thread_pending_exit = cur_thread;

    // WIFEXITED
        process->exit_status = status << 8;
    // 1. dequeue cur_thread and wait all other threads
        vsf_dlist_remove(vsf_linux_thread_t, thread_node, &process->thread_list, cur_thread);
    vsf_unprotect_sched(orig);

    // 2. call atexit, some resources maybe freed by user
    if (!_exit) {
        for (int i = 0; i < process->fn_atexit_num; i++) {
            if (process->fn_atexit[i] != NULL) {
                process->fn_atexit[i]();
            }
        }
    }

    // 3. wait child threads
    while (true) {
        orig = vsf_protect_sched();
            vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, thread2wait);
        vsf_unprotect_sched(orig);
        if (NULL == thread2wait) {
            break;
        }

        // tid < 0 is not user thread, do not print warning
        if (thread2wait->tid >= 0) {
            vsf_trace_warning("linux: undetached thread %d, need pthread_join before exit\n", thread2wait->tid);
        }
        vsf_linux_wait_thread(thread2wait->tid, NULL);
    }

    // 5. cleanup process
    vsf_linux_cleanup_process(process);

    // 6. wait child process
    vsf_linux_process_t *process2wait;
    while (true) {
        orig = vsf_protect_sched();
            vsf_dlist_peek_head(vsf_linux_process_t, child_node, &process->child_list, process2wait);
        vsf_unprotect_sched(orig);
        if (NULL == process2wait) {
            break;
        }
        if (process2wait->id.gid != process->id.pid) {
            vsf_linux_detach_process(process2wait);
            continue;
        }

        vsf_trace_warning("linux: undetached process %d, need waitpid before exit\n", process2wait->id.pid);
        waitpid(process2wait->id.pid, NULL, 0);
    }

    // 7. notify pending thread, MUST be the last before exiting current thread
    orig = vsf_protect_sched();
    process->status &= ~PID_STATUS_RUNNING;
    if (process->thread_pending != NULL) {
        vsf_unprotect_sched(orig);
        process->thread_pending->retval = process->exit_status;
        vsf_eda_post_evt(&process->thread_pending->use_as__vsf_eda_t, VSF_EVT_USER);
    } else {
        vsf_linux_process_t *parent_process = process->parent_process;
        if ((parent_process != NULL) && (parent_process->thread_pending_child != NULL)) {
            vsf_linux_thread_t *thread_pending = parent_process->thread_pending_child;
            parent_process->thread_pending_child = NULL;
            vsf_unprotect_sched(orig);
            thread_pending->retval = process->exit_status;
            thread_pending->pid_exited = process->id.pid;
            vsf_eda_post_evt(&thread_pending->use_as__vsf_eda_t, VSF_EVT_USER);
        } else {
            vsf_unprotect_sched(orig);
        }
    }

    // 8. exit current thread
    if (PID_STATUS_DAEMON == process->status) {
        orig = vsf_protect_sched();
            vsf_dlist_remove(vsf_linux_process_t, process_node, &__vsf_linux.process_list, process);
        vsf_unprotect_sched(orig);
    }
    cur_thread->process = NULL;
end_no_return:
    vsf_thread_exit();
}

VSF_CAL_NO_RETURN void _exit(int status)
{
    vsf_linux_exit_process(status, true);
}

int acct(const char *filename)
{
    return -1;
}

#if VSF_ARCH_USE_THREAD_REG == ENABLED
uintptr_t vsf_linux_set_process_reg(uintptr_t reg)
{
    return vsf_arch_set_thread_reg(reg);
}
#endif

vsf_linux_process_t * __vsf_linux_start_process_internal(
        vsf_linux_main_entry_t entry, char * const * argv, int stack_size, vsf_prio_t prio)
{
    VSF_LINUX_ASSERT((prio >= VSF_LINUX_CFG_PRIO_LOWEST) && (prio <= VSF_LINUX_CFG_PRIO_HIGHEST));
    vsf_linux_process_t *process = __vsf_linux_create_process(stack_size, 0, 0);
    if (process != NULL) {
        process->prio = prio;
        process->ctx.entry = entry;
        process->working_dir = __strdup_ex(process, "/");
        if (NULL == process->working_dir) {
            vsf_linux_delete_process(process);
            return NULL;
        }
        if (argv != NULL) {
            __vsf_linux_process_parse_arg(process, NULL, argv);
        }
        vsf_linux_start_process(process);
    }
    return process;
}

vsf_linux_process_t * vsf_linux_get_process(pid_t pid)
{
    vsf_protect_t orig = vsf_protect_sched();
    __vsf_dlist_foreach_unsafe(vsf_linux_process_t, process_node, &__vsf_linux.process_list) {
        if (_->id.pid == pid) {
            vsf_unprotect_sched(orig);
            return _;
        }
    }
    vsf_unprotect_sched(orig);
    return NULL;
}

vsf_linux_thread_t * vsf_linux_get_thread(pid_t pid, int tid)
{
    vsf_linux_process_t *process = pid < 0 ? vsf_linux_get_cur_process() : vsf_linux_get_process(pid);
    VSF_LINUX_ASSERT(process != NULL);
    vsf_protect_t orig = vsf_protect_sched();
    __vsf_dlist_foreach_unsafe(vsf_linux_thread_t, thread_node, &process->thread_list) {
        if (_->tid == tid) {
            vsf_unprotect_sched(orig);
            return _;
        }
    }
    vsf_unprotect_sched(orig);
    return NULL;
}

bool vsf_linux_is_linux_ctx(vsf_linux_thread_t *thread)
{
    if (NULL == thread) {
        thread = (vsf_linux_thread_t *)vsf_eda_get_cur();
    }
    if (NULL == thread) {
        return false;
    }

    extern bool __vsf_linux_is_pthread_ctx(vsf_linux_thread_t *thread);
    return  (thread->on_terminate == (vsf_eda_on_terminate_t)vsf_linux_thread_on_terminate)
        ||  __vsf_linux_is_pthread_ctx(thread);
}

vsf_linux_thread_t * vsf_linux_get_cur_thread(void)
{
    vsf_linux_thread_t *thread = (vsf_linux_thread_t *)vsf_eda_get_cur();
    if ((thread != NULL) && thread->flag.feature.is_stack_owner && vsf_linux_is_linux_ctx(thread)) {
        return thread;
    }
    return NULL;
}

vsf_linux_process_t * vsf_linux_get_cur_process(void)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    if (NULL == thread) {
        return NULL;
    }

    vsf_linux_process_t *process = thread->process;
    VSF_LINUX_ASSERT(process != NULL);
    return process->id.pid != (pid_t)0 || NULL == __vsf_linux.kernel_process ?
                process : __vsf_linux.kernel_process;
}

#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
static vsf_linux_sig_handler_t * __vsf_linux_get_sighandler_ex(vsf_linux_process_t *process, int sig)
{
    vsf_linux_sig_handler_t *handler = NULL;
    VSF_LINUX_ASSERT(process != NULL);
    __vsf_dlist_foreach_unsafe(vsf_linux_sig_handler_t, node, &process->sig.handler_list) {
        if (_->sig == sig) {
            handler = _;
            break;
        }
    }
    return handler;
}

static void __vsf_linux_sighandler(vsf_thread_t *thread, int sig)
{
    vsf_linux_process_t *process = ((vsf_linux_thread_t *)thread)->process;
    vsf_linux_sig_handler_t *handler;
    sighandler_t sighandler;
    vsf_protect_t orig;

    sig++;
    orig = vsf_protect_sched();
    if ((process->sig.pending.sig[0] & ~process->sig.mask.sig[0]) & (1ULL << sig)) {
        process->sig.pending.sig[0] &= ~(1ULL << sig);
    } else {
        vsf_unprotect_sched(orig);
        return;
    }
    vsf_unprotect_sched(orig);

    handler = __vsf_linux_get_sighandler_ex(process, sig);
    sighandler = SIG_DFL;
    if (handler != NULL) {
        if (handler->flags & SA_SIGINFO) {
            VSF_LINUX_ASSERT(handler->sigaction_handler != NULL);
            siginfo_t siginfo = {
                .si_signo   = sig,
                .si_errno   = errno,
            };
            handler->sigaction_handler(sig, &siginfo, NULL);
            // signal processed by sigaction, but still needs to interrupt slow syscall if necessary
            sighandler = SIG_IGN;
        } else {
            sighandler = handler->sighandler;
        }
    }

    if (SIG_DFL == sighandler) {
        // TODO: try to get default handler
        sighandler = SIG_IGN;
    }
    if (sighandler != SIG_IGN) {
        sighandler(sig);
    } else if ((NULL == handler) || !(handler->flags & SA_RESTART)) {
        // handler is not registered, or SA_RESTART is not set
        orig = vsf_protect_sched();
            vsf_linux_trigger_t *trigger;
            do {
                vsf_dlist_peek_head(vsf_linux_trigger_t, node,  &process->sig.trigger_list, trigger);
                if (trigger != NULL) {
                    vsf_linux_trigger_signal(trigger, sig);
                }
            } while (trigger != NULL);
        vsf_unprotect_sched(orig);
    }
}
#endif

static void __vsf_linux_main_on_run(vsf_thread_cb_t *cb)
{
    vsf_linux_thread_t *thread = vsf_container_of(cb, vsf_linux_thread_t, use_as__vsf_thread_cb_t);
    vsf_linux_process_t *process = thread->process;
    vsf_linux_process_t *shell_process = process->shell_process;
    vsf_linux_process_ctx_t *ctx = &process->ctx;
    vsf_linux_fd_priv_t *stdin_priv = NULL;
    vsf_linux_fd_t *sfd, *sfd_from;
    bool process_is_shell = false;
    int ret;

    if ((NULL == shell_process) || (shell_process == process)) {
        shell_process = &__vsf_linux.process_for_resources;
        process_is_shell = true;
    }

    sfd = vsf_linux_fd_get(STDIN_FILENO);
    if (NULL == sfd) {
        sfd_from = __vsf_linux_fd_get_ex(shell_process, STDIN_FILENO);
        VSF_LINUX_ASSERT(sfd_from != NULL);

        ret = __vsf_linux_fd_create_ex(process, &sfd, sfd_from->op, STDIN_FILENO, sfd_from->priv);
        VSF_LINUX_ASSERT(ret == STDIN_FILENO);
        stdin_priv = sfd->priv;
    }

    sfd = vsf_linux_fd_get(STDOUT_FILENO);
    if (NULL == sfd) {
        sfd_from = __vsf_linux_fd_get_ex(shell_process, STDOUT_FILENO);
        VSF_LINUX_ASSERT(sfd_from != NULL);

        ret = __vsf_linux_fd_create_ex(process, &sfd, sfd_from->op, STDOUT_FILENO, sfd_from->priv);
        VSF_LINUX_ASSERT(ret == STDOUT_FILENO);
    }

    sfd = vsf_linux_fd_get(STDERR_FILENO);
    if (NULL == sfd) {
        sfd_from = __vsf_linux_fd_get_ex(shell_process, STDERR_FILENO);
        VSF_LINUX_ASSERT(sfd_from != NULL);

        ret = __vsf_linux_fd_create_ex(process, &sfd, sfd_from->op, STDERR_FILENO, sfd_from->priv);
        VSF_LINUX_ASSERT(ret == STDERR_FILENO);
        if (process_is_shell) {
            sfd->priv->target = stdin_priv;
        }
    }

    //!   Possible memory leakage for IAR, because IAR will use malloc for VLA,
    //! so if main thread is exited by APIs like exit, the argv will not be freed.
#if __IS_COMPILER_IAR__
    char *argv[65];
    VSF_LINUX_ASSERT(dimof(argv) > ctx->arg.argc);
#else
    char *argv[ctx->arg.argc + 1];
#endif
    if (ctx->arg.argv != NULL) {
        memcpy(argv, ctx->arg.argv, (ctx->arg.argc + 1) * sizeof(argv[0]));
    } else {
        argv[0] = NULL;
    }

    VSF_LINUX_ASSERT(ctx->entry != NULL);
#ifdef __CPU_WEBASSEMBLY__
    // One of the rough edges with wasm and C.
    //  Can not add the 3rd argument as env,
    //  or runtime error function signature mismatch will be issued.
    // Refer to: https://github.com/emscripten-core/emscripten/issues/19942
    thread->retval = ctx->entry(ctx->arg.argc, argv);
#else
    thread->retval = ((int (*)(int, char **, char **))ctx->entry)(ctx->arg.argc, argv, process->__environ);
#endif

    vsf_linux_exit_process(thread->retval, false);
}

void vsf_linux_thread_on_terminate(vsf_linux_thread_t *thread)
{
    vsf_protect_t orig = vsf_protect_sched();
    vsf_linux_thread_t *thread_pending = thread->thread_pending;

    for (int i = 0; i < dimof(thread->tls); i++) {
        if ((thread->tls[i].destructor != NULL) && (thread->tls[i].data != NULL)) {
            thread->tls[i].destructor(thread->tls[i].data);
            thread->tls[i].destructor = NULL;
            thread->tls[i].data = NULL;
        }
    }

    thread->op = NULL;
    if (thread_pending != NULL) {
        thread->thread_pending = NULL;
        vsf_unprotect_sched(orig);
        thread_pending->retval = thread->retval;
        vsf_eda_post_evt(&thread_pending->use_as__vsf_eda_t, VSF_EVT_USER);
    } else {
        vsf_linux_process_t *process = thread->process;
        if (    (NULL == process) ||
                !vsf_dlist_is_in(vsf_linux_thread_t, thread_node, &process->thread_list, thread)) {
            vsf_unprotect_sched(orig);
            vsf_linux_process_heap_free(vsf_linux_resources_process(), thread);
            return;
        }
        vsf_unprotect_sched(orig);
    }
}

void vsf_linux_attach_process(vsf_linux_process_t *process, vsf_linux_process_t *parent_process)
{
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_add_to_tail(vsf_linux_process_t, child_node, &parent_process->child_list, process);
    vsf_unprotect_sched(orig);
    process->parent_process = parent_process;
    process->id.gid = parent_process->id.gid;
}

void vsf_linux_detach_process(vsf_linux_process_t *process)
{
    if (process->parent_process != NULL) {
        vsf_protect_t orig = vsf_protect_sched();
            vsf_dlist_remove(vsf_linux_process_t, child_node, &process->parent_process->child_list, process);
        vsf_unprotect_sched(orig);
        process->parent_process = NULL;
        process->id.gid = process->id.pid;
    }
}

void vsf_linux_detach_thread(vsf_linux_thread_t *thread)
{
    vsf_protect_t orig = vsf_protect_sched();
    vsf_linux_process_t *process = thread->process;
    if (process != NULL) {
        // if assert here, process exists before vsf_linux_detach_thread is called
        VSF_LINUX_ASSERT(NULL == thread->thread_pending);
        vsf_dlist_remove(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
    }
    vsf_unprotect_sched(orig);
}

int vsf_linux_wait_thread(int tid, int *retval)
{
    vsf_protect_t orig = vsf_protect_sched();
    vsf_linux_thread_t *thread = vsf_linux_get_thread(-1, tid);
    if (NULL == thread) {
        vsf_unprotect_sched(orig);
        return -1;
    }

    if (thread->op != NULL) {
        thread->thread_pending = vsf_linux_get_cur_thread();
        vsf_unprotect_sched(orig);
        vsf_thread_wfe(VSF_EVT_USER);
    } else {
        vsf_unprotect_sched(orig);
    }

    if (retval != NULL) {
        *retval = thread->retval;
    }
    vsf_linux_detach_thread(thread);
    vsf_linux_process_heap_free(vsf_linux_resources_process(), thread);
    return 0;
}

int daemon(int nochdir, int noclose)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    vsf_linux_process_t *parent_process = process->parent_process;

    if (!nochdir) {
        free(process->working_dir);
        process->working_dir = strdup("/");
    }
    if (!noclose) {
        int fd = open("/dev/null", 0);
        VSF_LINUX_ASSERT(fd >= 0);
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    process->shell_process = process;
    vsf_linux_detach_process(process);

    vsf_linux_thread_t *thread_pending;
    vsf_protect_t orig = vsf_protect_sched();
        process->status = PID_STATUS_DAEMON;
        thread_pending = process->thread_pending;
        if (thread_pending != NULL) {
            process->thread_pending = NULL;
        } else if ((parent_process != NULL) && (parent_process->thread_pending_child != NULL)) {
            thread_pending = parent_process->thread_pending_child;
            parent_process->thread_pending_child = NULL;
            thread_pending->retval = 0;
            thread_pending->pid_exited = process->id.pid;
        }
    vsf_unprotect_sched(orig);
    if (thread_pending != NULL) {
        thread_pending->retval = process->exit_status;
        vsf_eda_post_evt(&thread_pending->use_as__vsf_eda_t, VSF_EVT_USER);
    }
    return 0;
}

#if VSF_LINUX_USE_APPLET == ENABLED
static const vsf_loader_op_t * __vsf_linux_get_applet_loader(uint8_t *header, int size)
{
#   if VSF_LOADER_USE_PE == ENABLED
    if ((size >= 2) && (header[0] == 'M') && (header[1] == 'Z')) {
        return &vsf_peloader_op;
    } else
#   endif
    if ((size >= 4) && (header[0] == 0x7F) && (header[1] == 'E') && (header[2] == 'L') && (header[3] == 'F')) {
        return &vsf_elfloader_op;
    }
    return NULL;
}
#endif

static int __vsf_linux_get_exe_path(char *pathname, int pathname_len, char *cmd, vsf_linux_main_entry_t *entry, char *path)
{
    char pathname_local[PATH_MAX], pathname_tmp[PATH_MAX], *path_end, *pathname_dir, *cmdname = pathname_tmp;
    int exefd = -1, pathlen, pathname_dir_len;
    uint_fast32_t feature;

    // skip parameters in cmd, leave command name only
    int pos = 0;
    for (pos = 0; (cmd[pos] != '\0') && !isspace(cmd[pos]) && (pos < (sizeof(pathname_tmp) - 1)); pos++) {
        cmdname[pos] = cmd[pos];
    }
    cmdname[pos] = '\0';
    pathname_dir = &cmdname[pos + 1];
    pathname_dir_len = sizeof(pathname_tmp) - pos  - 1;

    if ((NULL == path) || (strchr(cmdname, '\\') || strchr(cmdname, '/'))) {
        if (pathname != NULL) {
            strncpy(pathname, cmdname, pathname_len);
        } else {
            pathname = cmdname;
        }
        path = "";
        goto try_open;
    } else if (NULL == pathname) {
        pathname = pathname_local;
        pathname_len = sizeof(pathname_local);
    }

    while (*path != '\0') {
        path_end = strchr(path, ':');
        pathlen = (path_end != NULL) ?  path_end - path : strlen(path);
        VSF_LINUX_ASSERT(pathlen < pathname_dir_len - 1);
        memcpy(pathname_dir, path, pathlen);
        pathname_dir[pathlen] = '\0';
        path += pathlen;
        if (*path == ':') {
            path++;
        }

        if (!vsf_linux_generate_path(pathname, pathname_len, pathname_dir, cmdname)) {
        try_open:
            exefd = open(pathname, 0);
            if (exefd >= 0) {
                if (!vsf_linux_fd_get_feature(exefd, &feature) && (feature & VSF_FILE_ATTR_EXECUTE)) {
                    if (entry != NULL) {
                        vsf_linux_fd_get_target(exefd, (void **)entry);
                    }
                    break;
                } else {
#if VSF_LINUX_USE_APPLET == ENABLED
                    uint8_t head[16];
                    ssize_t headlen = read(exefd, head, sizeof(head));
                    lseek(exefd, 0, SEEK_SET);
                    if (__vsf_linux_get_applet_loader(head, headlen) != NULL) {
                        if (entry != NULL) {
                            int __vsf_linux_dynloader_main(int argc, char **argv);
                            *entry = __vsf_linux_dynloader_main;
                        }
                        break;
                    }
#endif
#if VSF_LINUX_USE_SCRIPT == ENABLED
                    // do not check #!, because it's not a must
                    if (entry != NULL) {
                        int __vsf_linux_script_main(int argc, char **argv);
                        *entry = __vsf_linux_script_main;
                    }
                    break;
#else
                    close(exefd);
                    exefd = -1;
#endif
                }
            }
        }
    }
    if (exefd < 0) {
        errno = ENOENT;
    }
#if VSF_LINUX_CFG_LINK_FILE == ENABLED
    else if (pathname != pathname_local) {
        vsf_linux_fd_t *sfd = __vsf_linux_fd_get_ex(NULL, exefd);
        VSF_LINUX_ASSERT((sfd != NULL) && (sfd->op->feature & VSF_LINUX_FDOP_FEATURE_FS));
        if (sfd->fd_flags & (1 << __FD_OPENBYLINK)) {
            vk_file_t *file = ((vsf_linux_fs_priv_t *)sfd->priv)->file;

            char *tmp = &pathname[pathname_len - 1];
            size_t len;
            *tmp = '\0';
            while ((file != NULL) && (file->name != NULL)) {
                len = strlen(file->name);
                tmp -= len;
                memcpy(tmp, file->name, len);
                *--tmp = '/';
                file = file->parent;
            }
            strcpy(pathname, tmp);
        }
    }
#endif
    return exefd;
}

int __vsf_linux_get_exe(char *pathname, int pathname_len, char *cmd, vsf_linux_main_entry_t *entry, bool use_path)
{
    char *path;
    if (use_path) {
        path = getenv("PATH");
    } else {
        vsf_linux_process_t *process = vsf_linux_get_cur_process();
        VSF_LINUX_ASSERT(process != NULL);
        path = process->working_dir;
    }
    int exefd = __vsf_linux_get_exe_path(pathname, pathname_len, cmd, entry, path);
    if (exefd < 0) {
        return -1;
    }
    close(exefd);
    return 0;
}

const char * find_in_path(const char *progname)
{
    char fullpath[PATH_MAX];
    int fd = __vsf_linux_get_exe(fullpath, sizeof(fullpath), (char *)progname, NULL, true);
    if (fd < 0) {
        return progname;
    }
    close(fd);
    return strdup(fullpath);
}

const char * find_in_given_path(const char *progname, const char *path, const char *directory, bool optimize_for_exec)
{
    char fullpath[PATH_MAX];
    int fd;

    if (directory != NULL) {
        if (vsf_linux_generate_path(fullpath, sizeof(fullpath), (char *)directory, (char *)progname)) {
            return NULL;
        }
        fd = __vsf_linux_get_exe_path(NULL, 0, (char *)fullpath, NULL, (char *)path);
    } else {
        fd = __vsf_linux_get_exe_path(fullpath, sizeof(fullpath), (char *)progname, NULL, (char *)path);
    }

    if (fd < 0) {
        return NULL;
    }
    close(fd);
    return strdup(fullpath);
}

#if VSF_LINUX_USE_SCRIPT == ENABLED
int __vsf_linux_script_main(int argc, char **argv)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    int fd = __vsf_linux_get_exe_path(NULL, 0, process->path, NULL, NULL);
    VSF_LINUX_ASSERT(fd >= 0);

    char head[32], *cmdline;
    ssize_t headlen = read(fd, head, sizeof(head) - 1);
    close(fd);

    if ((headlen < 3) || (head[0] != '#') || (head[1] != '!')) {
        printf("invalid script file %s\n", argv[0]);
        return -1;
    }

    head[headlen] = '\0';
    cmdline = &head[2];
    for (int i = 2; i < headlen; i++, cmdline++) {
        if (!isspace(*cmdline)) {
            break;
        }
    }
    for (char *i = cmdline; *i != '\0'; i++) {
        if ((*i == '\n') || (*i == '\r')) {
            *i = '\0';
            break;
        }
    }

    //!   Possible memory leakage for IAR, because IAR will use malloc for VLA,
    //! so if main thread is exited by APIs like exit, the argv will not be freed.
    char *shell_argv[argc + 2];
    shell_argv[0] = cmdline;
    char script_path[strlen(process->path) + 1];
    strcpy(script_path, process->path);
    shell_argv[1] = script_path;
    memcpy(&shell_argv[2], argv + 1, argc * sizeof(argv[0]));
    execvp((const char *)shell_argv[0], (char * const *)shell_argv);
    printf("failed to call %s\n", argv[0]);
    return -1;
}
#endif

#if VSF_LINUX_USE_APPLET == ENABLED
static void * __vsf_linux_dynloader_remap(vsf_applet_ctx_t *ctx, void *vaddr)
{
    return vsf_loader_remap((vsf_loader_t *)ctx->target, vaddr);
}
int __vsf_linux_dynloader_main(int argc, char **argv)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    vsf_linux_dynloader_t *loader = dlopen(process->path, 0);
    if (NULL == loader) {
        printf("fail to dlopen %s\n", process->path);
        return -1;
    }

    int result = -1;
    if (loader->loader.generic.entry != NULL) {
        vsf_applet_ctx_t applet_ctx = {
            .target     = &loader->loader.generic,
            .fn_init    = NULL,     // fn_init has already been called in dlopen
            .fn_fini    = NULL,     // fn_fini will be called in dlclose
            .fn_remap   = __vsf_linux_dynloader_remap,
            .argc       = argc,
            .argv       = argv,
            .envp       = environ,
            .vplt       = loader->loader.generic.vplt,
        };

#   if VSF_ARCH_USE_THREAD_REG == ENABLED
        process->reg = (uintptr_t)loader->loader.generic.static_base;
        vsf_linux_set_process_reg(process->reg);
#   endif
        result = ((int (*)(vsf_applet_ctx_t*))loader->loader.generic.entry)(&applet_ctx);
    } else {
        printf("no entry found for %s\n", process->path);
    }

    // will be called in __vsf_linux_loader_atexit
//    dlclose(loader);
    return result;
}
#endif

#if VSF_LINUX_USE_VFORK == ENABLED
static int __vsf_linux_exec_start(vsf_linux_process_t *parent_process, vsf_linux_process_t *process)
#else
static int __vsf_linux_exec_start(vsf_linux_process_t *process)
#endif
{
    __vsf_dlist_foreach_next_unsafe(vsf_linux_fd_t, fd_node, &process->fd_list) {
        if (_->fd_flags & FD_CLOEXEC) {
            __vsf_linux_fd_close_ex(process, _->fd);
        }
    }

#if VSF_LINUX_USE_VFORK == ENABLED
    if (parent_process->is_vforking) {
        // clone necessary resources for vfork child process

        vsf_linux_start_process(process);
        parent_process->is_vforking = false;
        longjmp(parent_process->__vfork_jmpbuf, 1);
    } else
#endif
    {
        vsf_linux_thread_t *thread;
        vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
        vsf_eda_post_evt(&thread->use_as__vsf_eda_t, VSF_EVT_INIT);
        vsf_thread_wfe(VSF_EVT_INVALID);
    }
    return 0;
}

static exec_ret_t __vsf_linux_execvpe(vsf_linux_main_entry_t entry, char * const * argv, char  * const * envp, char *path)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
#if VSF_LINUX_USE_VFORK == ENABLED
    vsf_linux_process_t *parent_process = process;
    if (parent_process->is_vforking) {
        process = process->vfork_child;
    }
#endif

#if __VSF_LINUX_PROCESS_HAS_PATH
    if (path != NULL) {
        strcpy(process->path, path);
    }
#endif

    vsf_linux_process_ctx_t *ctx = &process->ctx;

    // MUST parse argument to arg first, then free arg.
    //  Because maybe the argv is in process arg, so if free process arg first, argv will be invalid
    vsf_linux_process_arg_t arg = { 0 };
    __vsf_linux_process_parse_arg(process, &arg, argv);
    __vsf_linux_process_free_arg(process);
    process->ctx.arg = arg;

    vsf_linux_merge_env(process, (char **)envp);
    ctx->entry = entry;

    return __vsf_linux_exec_start(
#if VSF_LINUX_USE_VFORK == ENABLED
        parent_process,
#endif
        process);
}

exec_ret_t execvpe(const char *file, char * const * argv, char  * const * envp)
{
    vsf_linux_main_entry_t entry;
#if __VSF_LINUX_PROCESS_HAS_PATH
#   if VSF_LINUX_CFG_LINK_FILE == ENABLED || VSF_LINUX_USE_VFORK == ENABLED
    char localpath[PATH_MAX];
    if (__vsf_linux_get_exe(localpath, sizeof(localpath), (char *)file, &entry, true) < 0) {
#   else
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    if (__vsf_linux_get_exe(process->path, sizeof(process->path), (char *)file, &entry, true) < 0) {
#   endif
#else
    if (__vsf_linux_get_exe(NULL, 0, (char *)file, &entry, true) < 0) {
#endif
        return -1;
    }
    return __vsf_linux_execvpe(entry, argv, envp,
#if __VSF_LINUX_PROCESS_HAS_PATH && VSF_LINUX_CFG_LINK_FILE == ENABLED
            localpath
#else
            NULL
#endif
        );
}

exec_ret_t execvp(const char *file, char * const * argv)
{
    return execvpe(file, argv, NULL);
}

exec_ret_t execve(const char *pathname, char * const * argv, char * const * envp)
{
    vsf_linux_main_entry_t entry;
#if __VSF_LINUX_PROCESS_HAS_PATH
#   if VSF_LINUX_CFG_LINK_FILE == ENABLED || VSF_LINUX_USE_VFORK == ENABLED
    char localpath[PATH_MAX];
    if (__vsf_linux_get_exe(localpath, sizeof(localpath), (char *)pathname, &entry, false) < 0) {
#   else
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    if (__vsf_linux_get_exe(process->path, sizeof(process->path), (char *)pathname, &entry, false) < 0) {
#   endif
#else
    if (__vsf_linux_get_exe(NULL, 0, (char *)pathname, &entry, false) < 0) {
#endif
        return -1;
    }
    return __vsf_linux_execvpe(entry, argv, envp,
#if __VSF_LINUX_PROCESS_HAS_PATH && VSF_LINUX_CFG_LINK_FILE == ENABLED
            localpath
#else
            NULL
#endif
        );
}

exec_ret_t execv(const char *pathname, char * const * argv)
{
    return execve(pathname, argv, NULL);
}

static exec_ret_t __vsf_linux_execlp_va(vsf_linux_main_entry_t entry, const char *arg, va_list ap, char *path, bool has_env)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
#if VSF_LINUX_USE_VFORK == ENABLED
    vsf_linux_process_t *parent_process = process;
    if (parent_process->is_vforking) {
        process = process->vfork_child;
    }
#endif

#if __VSF_LINUX_PROCESS_HAS_PATH
    if (path != NULL) {
        strcpy(process->path, path);
    }
#endif

    vsf_linux_process_ctx_t *ctx = &process->ctx;
    const char *args;
    va_list ap2;

    __vsf_linux_process_free_arg(process);

    va_copy(ap2, ap);
    args = va_arg(ap, const char *);
    while (args != NULL) {
        ctx->arg.argc++;
        args = va_arg(ap, const char *);
    }
    ctx->arg.argc += 1;

    ctx->arg.argv = __malloc_ex(process, (ctx->arg.argc + 1) * sizeof(char *));
    if (NULL == ctx->arg.argv) {
        return -1;
    }

    ctx->arg.argv[0] = (const char *)__strdup_ex(process, arg);
    ctx->arg.argv[ctx->arg.argc] = NULL;
    args = va_arg(ap2, const char *);
    for (int i = 1; i < ctx->arg.argc; i++) {
        ctx->arg.argv[i] = (const char *)__strdup_ex(process, args);
        args = va_arg(ap2, const char *);
    }
    ctx->entry = entry;

    if (has_env) {
        args = va_arg(ap2, const char *);
        vsf_linux_merge_env(process, (char **)args);
    }

    return __vsf_linux_exec_start(
#if VSF_LINUX_USE_VFORK == ENABLED
        parent_process,
#endif
        process);
}

exec_ret_t __execlp_va(const char *pathname, const char *arg, va_list ap)
{
    vsf_linux_main_entry_t entry;
#if __VSF_LINUX_PROCESS_HAS_PATH
#   if VSF_LINUX_CFG_LINK_FILE == ENABLED || VSF_LINUX_USE_VFORK == ENABLED
    char localpath[PATH_MAX];
    if (__vsf_linux_get_exe(localpath, sizeof(localpath), (char *)pathname, &entry, true) < 0) {
#   else
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    if (__vsf_linux_get_exe(process->path, sizeof(process->path), (char *)pathname, &entry, true) < 0) {
#   endif
#else
    if (__vsf_linux_get_exe(NULL, 0, (char *)pathname, &entry, true) < 0) {
#endif
        return -1;
    }
    return __vsf_linux_execlp_va(entry, arg, ap,
#if __VSF_LINUX_PROCESS_HAS_PATH && VSF_LINUX_CFG_LINK_FILE == ENABLED
            localpath,
#else
            NULL,
#endif
            false
        );
}

exec_ret_t execlp(const char *pathname, const char *arg, ...)
{
    exec_ret_t ret;

    va_list ap;
    va_start(ap, arg);
        ret = __execlp_va(pathname, arg, ap);
    va_end(ap);
    return ret;
}

exec_ret_t __execl_va(const char *pathname, const char *arg, va_list ap, bool has_env)
{
    vsf_linux_main_entry_t entry;
#if __VSF_LINUX_PROCESS_HAS_PATH
#   if VSF_LINUX_CFG_LINK_FILE == ENABLED || VSF_LINUX_USE_VFORK == ENABLED
    char localpath[PATH_MAX];
    if (__vsf_linux_get_exe(localpath, sizeof(localpath), (char *)pathname, &entry, false) < 0) {
#   else
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    if (__vsf_linux_get_exe(process->path, sizeof(process->path), (char *)pathname, &entry, false) < 0) {
#   endif
#else
    if (__vsf_linux_get_exe(NULL, 0, (char *)pathname, &entry, false) < 0) {
#endif
        return -1;
    }
    return __vsf_linux_execlp_va(entry, arg, ap,
#if __VSF_LINUX_PROCESS_HAS_PATH && VSF_LINUX_CFG_LINK_FILE == ENABLED
            localpath,
#else
            NULL,
#endif
            has_env
        );
}

exec_ret_t execl(const char *pathname, const char *arg, ...)
{
    exec_ret_t ret;

    va_list ap;
    va_start(ap, arg);
        ret = __execl_va(pathname, arg, ap, false);
    va_end(ap);
    return ret;
}

exec_ret_t execle(const char *pathname, const char *arg, ...)
{
    exec_ret_t ret;

    va_list ap;
    va_start(ap, arg);
        ret = __execl_va(pathname, arg, ap, true);
    va_end(ap);
    return ret;
}

long sysconf(int name)
{
    switch (name) {
    case _SC_PAGESIZE:      return 256;
    case _SC_OPEN_MAX:      return 65535;
    case _SC_CLK_TCK:       return 100;
    }
    return 0;
}

long fpathconf(int fd, int name)
{
    switch (name) {
    case _PC_NAME_MAX:      return PATH_MAX;
    default:                return -1;
    }
}

long pathconf(const char *path, int name)
{
    if ((NULL == path) || ('\0' == *path)) {
        return -1;
    }

    int fd = open(path, 0);
    if (fd < 0) { return -1; }

    long res = fpathconf(fd, name);
    close(fd);
    return res;
}

#if __IS_COMPILER_IAR__
//! transfer of control bypasses initialization
#   pragma diag_suppress=pe546
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

int pipe(int pipefd[2])
{
    vsf_linux_fd_t *sfd_rx = NULL, *sfd_tx = NULL;

    sfd_rx = vsf_linux_rx_pipe(NULL);
    if (NULL == sfd_rx) {
        return -1;
    }

    sfd_tx = vsf_linux_tx_pipe((vsf_linux_pipe_priv_t *)sfd_rx->priv);
    if (NULL == sfd_tx) {
        close(sfd_rx->fd);
        return -1;
    }

    pipefd[0] = sfd_rx->fd;
    pipefd[1] = sfd_tx->fd;
    return 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

int pipe2(int pipefd[2], int flags)
{
    int res = pipe(pipefd);
    if (0 == res) {
        if (flags & O_CLOEXEC) {
            fcntl(pipefd[0], F_SETFD, FD_CLOEXEC);
            fcntl(pipefd[1], F_SETFD, FD_CLOEXEC);
        }
        if (flags & O_NONBLOCK) {
            fcntl(pipefd[0], F_SETFD, O_NONBLOCK);
            fcntl(pipefd[1], F_SETFD, O_NONBLOCK);
        }
    }
    return res;
}

// signal.h

int kill(pid_t pid, int sig)
{
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    if ((sig <= 0) || (sig > _NSIG) || (sig >= 64)) {
        return -1;
    }

    vsf_protect_t orig = vsf_protect_sched();
    vsf_linux_process_t *process = vsf_linux_get_process(pid);
    if ((NULL == process) || (process->thread_pending_exit != NULL)) {
        vsf_unprotect_sched(orig);
        return -1;
    }

#if _NSIG >= 32
    process->sig.pending.sig[0] |= 1ULL << sig;
#else
    process->sig.pending.sig[0] |= 1 << sig;
#endif
    vsf_unprotect_sched(orig);

    vsf_linux_thread_t *thread;
    vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
    vsf_thread_signal(&thread->use_as__vsf_thread_t, sig - 1);
    return 0;
#else
    return -1;
#endif
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    vsf_linux_sig_handler_t *handler = __vsf_linux_get_sighandler_ex(process, signum);
    bool new_handler = NULL == handler;

    if (new_handler) {
        handler = calloc(1, sizeof(vsf_linux_sig_handler_t));
        if (NULL == handler) {
            return -1;
        }
        handler->sig = signum;
        handler->sighandler = SIG_DFL;
    }

    if (oldact != NULL) {
        oldact->sa_handler = handler->sighandler;
        oldact->sa_flags = handler->flags;
        oldact->sa_mask = handler->mask;
    }
    handler->sighandler = act->sa_handler;
    handler->flags = act->sa_flags;
    handler->mask = act->sa_mask;

    if (new_handler) {
        vsf_protect_t orig = vsf_protect_sched();
            vsf_dlist_add_to_tail(vsf_linux_sig_handler_t, node, &process->sig.handler_list, handler);
        vsf_unprotect_sched(orig);
    }
    return 0;
#else
    return -1;
#endif
}

int sigwaitinfo(const sigset_t *set, siginfo_t *info)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

int sigtimedwait(const sigset_t *set, siginfo_t *info, const struct signal_timespec *timeout)
{
    return -1;
}

int sigsuspend(const sigset_t *set)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

#if !defined(__WIN__) || VSF_LINUX_CFG_WRAPPER == ENABLED
// conflicts with APIs in ucrt, need VSF_LINUX_CFG_WRAPPER
sighandler_t signal(int signum, sighandler_t handler)
{
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    const struct sigaction in = {
        .sa_handler = handler,
    };
    struct sigaction out;
    if (!sigaction(signum, &in, &out)) {
        return out.sa_handler;
    }
    return SIG_DFL;
#else
    return NULL;
#endif
}

int raise(int sig)
{
    return kill(getpid(), sig);
}
#endif

useconds_t ualarm(useconds_t usecs, useconds_t interval)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

unsigned int alarm(unsigned int seconds)
{
    const struct itimerval it_new = {
        .it_value.tv_sec = seconds,
    };
    setitimer(ITIMER_REAL, &it_new, NULL);
    // TODO: return remaining timer of previous scheduled alarm
    return 0;
}

static pid_t __vsf_linux_wait_any(int *status, int options)
{
    vsf_linux_process_t *cur_process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(cur_process != NULL);
    vsf_linux_thread_t *cur_thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(cur_thread != NULL);

    VSF_LINUX_ASSERT(cur_process != NULL);
    vsf_protect_t orig = vsf_protect_sched();
        if (vsf_dlist_is_empty(&cur_process->child_list)) {
            vsf_unprotect_sched(orig);
            return (options & WNOHANG) ? (pid_t)0 : (pid_t)-1;
        }
        __vsf_dlist_foreach_unsafe(vsf_linux_process_t, child_node, &cur_process->child_list) {
            if (0 == _->status) {
                vsf_unprotect_sched(orig);
                cur_thread->retval = _->exit_status;
                cur_thread->pid_exited = _->id.pid;
                goto done;
            }
        }
        if (cur_process->thread_pending_child != NULL) {
            vsf_unprotect_sched(orig);
            return (pid_t)-1;
        }
        cur_process->thread_pending_child = cur_thread;
    vsf_unprotect_sched(orig);

    vsf_thread_wfe(VSF_EVT_USER);

done:
    if (status != NULL) {
        *status = cur_thread->retval;
    }
    vsf_linux_process_t *process = vsf_linux_get_process(cur_thread->pid_exited);
    if (process->status != PID_STATUS_DAEMON) {
        vsf_linux_detach_process(process);
        vsf_protect_t orig = vsf_protect_sched();
            vsf_dlist_remove(vsf_linux_process_t, process_node, &__vsf_linux.process_list, process);
        vsf_unprotect_sched(orig);
        vsf_linux_free_res(process);
    }
    return cur_thread->pid_exited;
}

pid_t wait(int *status)
{
    return __vsf_linux_wait_any(status, 0);
}

pid_t vsf_linux_waitpid(pid_t pid, int *status, int options, bool cleanup)
{
    if (pid <= 0) {
        return __vsf_linux_wait_any(status, options);
    }
    if (options != 0) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }

    vsf_linux_thread_t *cur_thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(cur_thread != NULL);
    bool is_daemon;

    vsf_protect_t orig = vsf_protect_sched();
    vsf_linux_process_t *process = vsf_linux_get_process(pid);
    if ((NULL == process) || (process->parent_process != cur_thread->process)) {
        vsf_unprotect_sched(orig);
        return -1;
    }
    if (process->status & PID_STATUS_RUNNING) {
        if (options & WNOHANG) {
            vsf_unprotect_sched(orig);
            return 0;
        }

        process->thread_pending = cur_thread;
        vsf_unprotect_sched(orig);
        vsf_thread_wfe(VSF_EVT_USER);
        orig = vsf_protect_sched();
    } else {
        cur_thread->retval = process->exit_status;
    }
    is_daemon = process->status == PID_STATUS_DAEMON;
    if (!is_daemon) {
        vsf_dlist_remove(vsf_linux_process_t, process_node, &__vsf_linux.process_list, process);
    }
    vsf_unprotect_sched(orig);

    if (status != NULL) {
        *status = cur_thread->retval;
    }
    if (!is_daemon && cleanup) {
        vsf_linux_detach_process(process);
        vsf_linux_free_res(process);
    }
    return pid;
}

pid_t waitpid(pid_t pid, int *status, int options)
{
    return vsf_linux_waitpid(pid, status, options, true);
}

int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
    switch (idtype) {
    case P_PID:     return waitpid((pid_t)id, NULL, options);
    default:        return -1;
    }
}

char * getlogin(void)
{
    return "root";
}

int getlogin_r(char *buf, size_t bufsize)
{
    strncpy(buf, getlogin(), bufsize);
    return 0;
}

int setgid(gid_t git)
{
    return 0;
}

gid_t getgid(void)
{
    return (gid_t)0;
}
int setegid(gid_t egid)
{
    return 0;
}
gid_t getegid(void)
{
    return (gid_t)0;
}

int setuid(uid_t uid)
{
    return 0;
}

uid_t getuid(void)
{
    return (uid_t)0;
}
int seteuid(uid_t euid)
{
    return 0;
}
uid_t geteuid(void)
{
    return (uid_t)0;
}

pid_t getpid(void)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    if (process->is_vforking) {
        process = process->vfork_child;
    }
    return process->id.pid;
}

pid_t getppid(void)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    if (process->is_vforking) {
        process = process->vfork_child;
    }
    return process->id.ppid;
}

pid_t setsid(void)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    if (process->is_vforking) {
        process = process->vfork_child;
    }
    vsf_linux_detach_process(process);
    return 0;
}

pid_t getsid(pid_t pid)
{
    return -1;
}

int setpgid(pid_t pid, pid_t pgid)
{
    vsf_linux_process_t *process = 0 == pid ? vsf_linux_get_cur_process() : vsf_linux_get_process(pid);
    VSF_LINUX_ASSERT(process != NULL);
    if (process->is_vforking) {
        process = process->vfork_child;
    }
    if (0 == pgid) {
        process->id.gid = process->id.pid;
    } else if (process->id.gid != pgid) {
        vsf_linux_process_t *process_group = vsf_linux_get_process(pgid);
        vsf_linux_detach_process(process);
        vsf_linux_attach_process(process, process_group);
    }
    return 0;
}

pid_t getpgid(pid_t pid)
{
    vsf_linux_process_t *process = vsf_linux_get_process(pid);
    return process->id.gid;
}

int setpgrp(void)
{
    return setpgid(0, 0);
}

pid_t getpgrp(void)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    if (process->is_vforking) {
        process = process->vfork_child;
    }
    return process->id.gid;
}

int setresuid(uid_t ruid, uid_t euid, uid_t suid)
{
    return 0;
}

int setresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
    return 0;
}

pid_t gettid(void)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);
    return thread->tid;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);

    if (oldset != NULL) {
        *oldset = process->sig.mask;
    }

    if (set != NULL) {
        vsf_protect_t orig = vsf_protect_sched();
            switch (how) {
            case SIG_BLOCK:     sigaddsetmask(&process->sig.mask, set->sig[0]); break;
            case SIG_UNBLOCK:   sigdelsetmask(&process->sig.mask, set->sig[0]); break;
            case SIG_SETMASK:   process->sig.mask = *set;                       break;
            }
        vsf_unprotect_sched(orig);
    }
    return 0;
}

char * getcwd(char *buffer, size_t maxlen)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    if (strlen(process->working_dir) >= maxlen) {
        errno = ERANGE;
        return NULL;
    }
    strncpy(buffer, process->working_dir, maxlen);
    return buffer;
}

int vsf_linux_fs_get_executable(const char *pathname, vsf_linux_main_entry_t *entry)
{
    int fd = open(pathname, 0);
    if (fd < 0) {
        return -1;
    }

    uint_fast32_t feature;
    if ((vsf_linux_fd_get_feature(fd, &feature) < 0) || !(feature & VSF_FILE_ATTR_EXECUTE)) {
        close(fd);
        return -1;
    }

    // TODO: support other executable files?
    if (entry != NULL) {
        vsf_linux_fd_get_target(fd, (void **)entry);
    }
    return fd;
}

int vsf_linux_fd_bind_executable(int fd, vsf_linux_main_entry_t entry)
{
    int err = vsf_linux_fd_bind_target(fd, (void *)entry, NULL, NULL);
    if (!err) {
        return vsf_linux_fd_add_feature(fd, VSF_FILE_ATTR_EXECUTE);
    }
    return err;
}

int vsf_linux_fs_bind_executable(char *path, vsf_linux_main_entry_t entry)
{
    int fd = open(path, 0);
    if (fd < 0) {
        fd = creat(path, 0);
        if (fd < 0) {
            printf("fail to install %s.\r\n", path);
            return fd;
        }
    }
    if (fd >= 0) {
        vsf_linux_fd_bind_executable(fd, entry);
        close(fd);
    }
    printf("%s installed.\r\n", path);
    return fd;
}

char * getpass(const char *prompt)
{
    printf("%s", prompt);

    struct termios orig, t;
    tcgetattr(STDIN_FILENO, &orig);
    t = orig;
    t.c_lflag &= ~(ECHO | ISIG);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &t);

    // TODO: read passwd

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);

    return NULL;
}

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
vsf_systimer_tick_t vsf_linux_sleep(vsf_timeout_tick_t ticks)
{
    vsf_systimer_tick_t realticks;
    vsf_linux_trigger_t trigger;
    vsf_linux_trigger_init(&trigger);

    realticks = vsf_systimer_get_tick();
    vsf_linux_trigger_pend(&trigger, ticks);
    realticks = vsf_systimer_get_elapsed(realticks);

    return ticks > realticks ? ticks - realticks : 0;
}

int usleep(int usec)
{
    errno = 0;
    vsf_linux_sleep(vsf_systimer_us_to_tick(usec));
    return errno != 0 ? -1 : 0;
}

// TODO: wakeup after signal
unsigned sleep(unsigned sec)
{
    vsf_systimer_tick_t ticks_remain = vsf_linux_sleep(vsf_systimer_ms_to_tick(sec * 1000));
    return vsf_systimer_tick_to_ms(ticks_remain) / 1000;
}

int pause(void)
{
    vsf_linux_sleep((vsf_timeout_tick_t)-1);
    return -1;
}
#endif

int chown(const char *pathname, uid_t owner, gid_t group)
{
    return 0;
}

int fchown(int fd, uid_t owner, gid_t group)
{
    return 0;
}

int lchown(const char *pathname, uid_t owner, gid_t group)
{
    return 0;
}

int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags)
{
    return 0;
}

size_t confstr(int name, char *buf, size_t len)
{
    return -1;
}

// ipc.h

key_t ftok(const char *pathname, int id)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

#if VSF_LINUX_CFG_SUPPORT_SEM == ENABLED
// sys/sem.h
// TODO: use vsf_linux_trigger_t, so that sleeping thread can be wakened by signal

static void __semfini(vsf_linux_fd_t *sfd)
{
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    unsigned short *semadj = priv->u.sem.semadj_arr;
    vsf_linux_sem_set_t *semset = (vsf_linux_sem_set_t *)priv->key;
    struct sembuf sops;

    sops.sem_flg = 0;
    for (int i = 0; i < semset->nsems; i++) {
        if (*semadj != 0) {
            sops.sem_num = i;
            sops.sem_op = *semadj;
            semop(sfd->fd, &sops, 1);
        }
    }
}

static int __semfree(vsf_linux_fd_t *sfd)
{
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    __vsf_linux_keyfree(&__vsf_linux.sem.list, priv->key);
    return 0;
}

int __semctl_va(int semid, int semnum, int cmd, va_list ap)
{
    union semun {
        int              val;
        struct semid_ds *buf;
        unsigned short  *array;
        struct seminfo  *__buf;
    } u;

    vsf_linux_fd_t *sfd = vsf_linux_fd_get(semid);
    VSF_LINUX_ASSERT(sfd != NULL);
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    vsf_linux_sem_set_t *semset = (vsf_linux_sem_set_t *)priv->key;
    VSF_LINUX_ASSERT(semset->nsems > semnum);
    vsf_linux_sem_t *sem = &semset->sem[semnum];

    u.buf = va_arg(ap, void *);
    switch (cmd) {
    case IPC_STAT:
        u.buf->sem_perm.uid = -1;
        u.buf->sem_perm.gid = -1;
        u.buf->sem_perm.cuid = -1;
        u.buf->sem_perm.cgid = -1;
        u.buf->sem_perm.mode = 0666;
        break;
    case IPC_SET:
        break;
    case IPC_RMID:
        vsf_eda_sync_cancel(&sem->sync);
        close(semid);
        break;
    case GETVAL:
        u.val = sem->sync.cur_union.cur_value;
        break;
    case GETALL:
        sem = semset->sem;
        for (int i = 0; i < semset->nsems; i++, sem++) {
            u.array[i] = sem->sync.cur_union.cur_value;
        }
        break;
    case SETVAL:
        return u.val;
    case SETALL:
        sem = semset->sem;
        for (int i = 0; i < semset->nsems; i++, sem++) {
            sem->sync.cur_union.cur_value = u.array[i];
        }
    default:
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    return 0;
}

int semctl(int semid, int semnum, int cmd, ...)
{
    int result;
    va_list ap;
    va_start(ap, cmd);
        result = __semctl_va(semid, semnum, cmd, ap);
    va_end(ap);
    return result;
}

int semget(key_t key, int nsems, int semflg)
{
    VSF_LINUX_ASSERT(nsems >= 1);
    vsf_linux_sem_set_t *semset = (vsf_linux_sem_set_t *)__vsf_linux_keyget(
        &__vsf_linux.sem.list, key, sizeof(vsf_linux_sem_set_t) + sizeof(vsf_linux_sem_t) * nsems, semflg);
    if ((NULL == semset) || ((semset->nsems != 0) && (semset->nsems  != nsems))) {
        return -1;
    }

    if (!semset->nsems) {
        semset->nsems = nsems;
        for (int i = 0; i < nsems; i++) {
            vsf_eda_sync_init(&semset->sem[i].sync, 0, VSF_SYNC_AUTO_RST);
        }
    }

    vsf_linux_fd_t *sfd;
    vsf_linux_fd_op_t tempop = vsf_linux_key_fdop;
    tempop.priv_size += semset->nsems * sizeof(unsigned short);
    int fd = vsf_linux_fd_create(&sfd, (const vsf_linux_fd_op_t *)&tempop);
    if (fd >= 0) {
        vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
        sfd->op = &vsf_linux_key_fdop;

        priv->u.sem.semadj_arr = (unsigned short *)&priv[1];
        priv->key = &semset->use_as__vsf_linux_key_t;
        priv->fn_fini = __semfini;
        priv->fn_close = __semfree;
    }

    return fd;
}

static vsf_timeout_tick_t __vsf_linux_timespec_to_timeout(const struct timespec *spec)
{
    vsf_timeout_tick_t timeout = -1;
    if (spec != NULL) {
        return  vsf_systimer_ms_to_tick(spec->tv_sec * 1000)
            +   vsf_systimer_us_to_tick(spec->tv_nsec / 1000);
    }
    return timeout;
}

int semtimedop(int semid, struct sembuf *sops, size_t nsops,
                    const struct timespec *timeout)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(semid);
    VSF_LINUX_ASSERT(sfd != NULL);
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    vsf_linux_sem_set_t *semset = (vsf_linux_sem_set_t *)priv->key;
    vsf_linux_sem_t *sem;
    unsigned short *semadj;
    short adjvalue, tgtvalue;
    vsf_protect_t orig;
    vsf_timeout_tick_t timeout_tick = __vsf_linux_timespec_to_timeout(timeout);
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vsf_sync_reason_t reason;

    for (size_t i = 0; i < nsops; i++) {
        VSF_LINUX_ASSERT(sops[i].sem_num < semset->nsems);
        sem = &semset->sem[sops[i].sem_num];
        semadj = &priv->u.sem.semadj_arr[sops[i].sem_num];
        adjvalue = sops[i].sem_op;

        if (adjvalue != 0) {
            orig = vsf_protect_sched();
            if (sops[i].sem_flg & SEM_UNDO) {
                *semadj -= adjvalue;
            }
            if (adjvalue > 0) {
                sem->sync.cur_union.cur_value += adjvalue;

                thread = NULL;
                __vsf_dlist_foreach_unsafe(vsf_linux_thread_t, pending_node, &sem->sync.pending_list) {
                    if (0 == _->func_priv.sem.wantval && 0 == sem->sync.cur_union.cur_value) {
                        thread = _;
                        break;
                    } else if (sem->sync.cur_union.cur_value >= _->func_priv.sem.wantval) {
                        sem->sync.cur_union.cur_value -= _->func_priv.sem.wantval;
                        thread = _;
                        break;
                    }
                }
                if (thread != NULL) {
                    thread->flag.state.is_sync_got = true;
                    vsf_dlist_remove(vsf_linux_thread_t, pending_node, &sem->sync.pending_list, thread);
                    vsf_unprotect_sched(orig);

                    __vsf_eda_post_evt_ex(&thread->use_as__vsf_eda_t, VSF_EVT_SYNC, true);
                } else {
                    vsf_unprotect_sched(orig);
                }
            } else {
                tgtvalue = sem->sync.cur_union.cur_value + adjvalue;
                if (tgtvalue < 0) {
                    if (sops[i].sem_flg & IPC_NOWAIT) {
                        vsf_unprotect_sched(orig);
                        errno = EAGAIN;
                        return -1;
                    }

                    thread->func_priv.sem.wantval = -adjvalue;
                    __vsf_eda_sync_pend(&sem->sync, &thread->use_as__vsf_eda_t, timeout_tick);
                    vsf_unprotect_sched(orig);
                    goto wait_event_and_continue;
                } else {
                    sem->sync.cur_union.cur_value = tgtvalue;
                    vsf_unprotect_sched(orig);
                }
            }
        } else {
            orig = vsf_protect_sched();
            if (!sem->sync.cur_union.cur_value) {
                vsf_unprotect_sched(orig);
                continue;
            } else if (sops[i].sem_flg & IPC_NOWAIT) {
                vsf_unprotect_sched(orig);
                errno = EAGAIN;
                return -1;
            } else {
                thread->func_priv.sem.wantval = 0;
                __vsf_eda_sync_pend(&sem->sync, vsf_eda_get_cur(), timeout_tick);
                vsf_unprotect_sched(orig);

            wait_event_and_continue:
                reason = vsf_eda_sync_get_reason(&sem->sync, vsf_thread_wait());
                switch (reason) {
                case VSF_SYNC_TIMEOUT:
                    errno = EAGAIN;
                    return -1;
                case VSF_SYNC_PENDING:
                    goto wait_event_and_continue;
                case VSF_SYNC_GET:
                    break;
                case VSF_SYNC_CANCEL:
                    errno = EIDRM;
                    return -1;
                default:
                    VSF_LINUX_ASSERT(false);
                    return -1;
                }
            }
        }
    }
    return 0;
}

int semop(int semid, struct sembuf *sops, size_t nsops)
{
    return semtimedop(semid, sops, nsops, NULL);
}
#endif

// sys/time.h and linux/hrtimer.h
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED

#   if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER == ENABLED
void __hrtimer_on_timer(vsf_callback_timer_t *timer)
{
    struct hrtimer *hrtimer = vsf_container_of(timer, struct hrtimer, callback_timer);
    if (hrtimer->function != NULL) {
        if (hrtimer->function(hrtimer) == HRTIMER_RESTART) {
            vsf_callback_timer_add_due(timer, hrtimer->due);
        }
    }
}
#   endif

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if (tz != NULL) {
        tz->tz_dsttime = 0;
        tz->tz_minuteswest = 0;
    }

#ifdef VSF_LINUX_CFG_RTC
    vsf_rtc_tm_t rtc_tm;
    vsf_rtc_get(&VSF_LINUX_CFG_RTC, &rtc_tm);

    VSF_LINUX_ASSERT(rtc_tm.tm_year >= 1900);
    struct tm t = {
        .tm_sec = rtc_tm.tm_sec,
        .tm_min = rtc_tm.tm_min,
        .tm_hour = rtc_tm.tm_hour,
        .tm_mday = rtc_tm.tm_mday,
        .tm_mon = rtc_tm.tm_mon - 1,
        .tm_year = rtc_tm.tm_year - 1900,
    };

    tv->tv_sec = mktime(&t);
    tv->tv_usec = rtc_tm.tm_ms * 1000;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
#endif
    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}
#endif

clock_t times(struct tms *buf)
{
    if (buf != NULL) {
        memset(buf, 0, sizeof(*buf));
    }
    return (clock_t)(vsf_systimer_get_tick() * sysconf(_SC_CLK_TCK) / vsf_systimer_get_freq());
}

// sys/timeb.h

int ftime(struct timeb *timebuf)
{
    if (timebuf != NULL) {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        timebuf->time = tv.tv_sec;
        timebuf->millitm = tv.tv_usec / 1000;
        timebuf->timezone = 0;
        timebuf->dstflag = 0;
    }
    return 0;
}

// sys/reboot.h

int reboot(int howto)
{
    switch (howto) {
    case RB_POWER_OFF:
        printf("Power down.");
        vsf_arch_shutdown();
        break;
    case RB_HALT_SYSTEM:
        printf("System halted.");
        vsf_arch_shutdown();
        break;
    case RB_AUTOBOOT:
        printf("Restarting system.");
        vsf_arch_reset();
        break;
    }
    return 0;
}

// sys/xattr.h

int removexattr(const char *path, const char *name)
{
    return 0;
}

int lremovexattr(const char *path, const char *name)
{
    return 0;
}

int fremovexattr(int fd, const char *name)
{
    return 0;
}

int setxattr(const char *path, const char *name, const void *value, size_t size, int flags)
{
    return 0;
}

int lsetxattr(const char *path, const char *name, const void *value, size_t size, int flags)
{
    return 0;
}

int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags)
{
    return 0;
}

ssize_t getxattr(const char *path, const char *name, void *value, size_t size)
{
    return 0;
}

ssize_t lgetxattr(const char *path, const char *name, void *value, size_t size)
{
    return 0;
}

ssize_t fgetxattr(int fd, const char *name, void *value, size_t size)
{
    return 0;
}

// sys/capability.h

int capget(cap_user_header_t hdrp, cap_user_data_t datap)
{
    VSF_LINUX_ASSERT(false);
    return 0;
}

int capset(cap_user_header_t hdrp, const cap_user_data_t datap)
{
    VSF_LINUX_ASSERT(false);
    return 0;
}

// futex.h
// TODO: use hasp map instead of vsf_dlist for better performance at the cost of resources taken
// TODO: use vsf_linux_trigger_t so that wait operation can be interrupted by signals

static vsf_linux_futex_t * __vsf_linux_futex_get(uint32_t *futex, bool is_to_alloc)
{
    vsf_linux_futex_t *linux_futex = NULL;
    __vsf_dlist_foreach_unsafe(vsf_linux_futex_t, node, &__vsf_linux.futex.list) {
        if (_->futex == futex) {
            linux_futex = _;
            break;
        }
    }
    if ((NULL == linux_futex) && is_to_alloc) {
        linux_futex = VSF_POOL_ALLOC(vsf_linux_futex_pool, &__vsf_linux.futex.pool);
        if (linux_futex != NULL) {
            vsf_dlist_init_node(vsf_linux_futex_t, node, linux_futex);
            linux_futex->futex = futex;
            vsf_eda_trig_init(&linux_futex->trig, false, true);
        }
    }
    return linux_futex;
}

long sys_futex(uint32_t *futex, int futex_op, uint32_t val, uintptr_t val2, uint32_t *futex2, uint32_t val3)
{
    VSF_LINUX_ASSERT(futex != NULL);
    vsf_linux_futex_t *linux_futex;
    vsf_timeout_tick_t timeout_ticks;
    vsf_sync_reason_t reason;
    vsf_protect_t orig;

    switch (futex_op & FUTEX_CMD_MASK) {
    case FUTEX_WAIT:
        {
            extern vsf_systimer_tick_t vsf_linux_timespec2tick(const struct timespec *ts);
            timeout_ticks = !val2 ? -1 : vsf_linux_timespec2tick((const struct timespec *)val2);
        }
        orig = vsf_protect_sched();
        if (*futex != val) {
            vsf_unprotect_sched(orig);
            break;
        }
        linux_futex = __vsf_linux_futex_get(futex, true);
        VSF_LINUX_ASSERT(linux_futex != NULL);

        __vsf_eda_sync_pend(&linux_futex->trig, NULL, timeout_ticks);
        vsf_unprotect_sched(orig);

        do {
            reason = vsf_eda_sync_get_reason(&linux_futex->trig, vsf_thread_wait());
        } while (reason == VSF_SYNC_PENDING);
        switch (reason) {
        case VSF_SYNC_GET:          return 0;
        case VSF_SYNC_TIMEOUT:      return -ETIMEDOUT;
        default:                    VSF_LINUX_ASSERT(false);
        }
        break;
    case FUTEX_WAKE:
        orig = vsf_protect_sched();
        linux_futex = __vsf_linux_futex_get(futex, false);
        vsf_unprotect_sched(orig);

        VSF_LINUX_ASSERT(linux_futex != NULL);
        vsf_eda_trig_set(&linux_futex->trig);

        orig = vsf_protect_sched();
        if (vsf_dlist_is_empty(&linux_futex->trig.pending_list)) {
            VSF_POOL_FREE(vsf_linux_futex_pool, &__vsf_linux.futex.pool, linux_futex);
        }
        vsf_unprotect_sched(orig);
        break;
    default:
        // TODO: add support to futex_op if assert here
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    return -1;
}

// prctl.h

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"
#endif

int prctl(int option, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4, uintptr_t arg5)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    VSF_LINUX_ASSERT(thread != NULL);

    switch (option) {
    case PR_SET_NAME:
        strncpy(thread->name, (const char *)arg2, sizeof(thread->name) - 1);
        thread->name[sizeof(thread->name) - 1] = '\0';
        return 0;
    case PR_GET_NAME:
        strncpy((char *)arg2, (const char *)thread->name, sizeof(thread->name));
        return 0;
    }
    VSF_LINUX_ASSERT(false);
    return -1;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#if VSF_LINUX_CFG_SUPPORT_SHM == ENABLED
// shm.h

static int __shmfree(vsf_linux_fd_t *sfd)
{
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    __vsf_linux_keyfree(&__vsf_linux.shm.list, priv->key);
    return 0;
}

int shmget(key_t key, size_t size, int shmflg)
{
    vsf_linux_shm_mem_t *mem = (vsf_linux_shm_mem_t *)__vsf_linux_keyget(&__vsf_linux.shm.list,
            key, sizeof(vsf_linux_shm_mem_t) + size, shmflg);
    if (NULL == mem) {
        return -1;
    }

    mem->size = size;
    mem->buffer = &mem[1];

    vsf_linux_fd_t *sfd;
    int fd = vsf_linux_fd_create(&sfd, &vsf_linux_key_fdop);
    if (fd >= 0) {
        vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
        priv->key = &mem->use_as__vsf_linux_key_t;
        priv->fn_close = __shmfree;
    }

    return fd;
}

void * shmat(int shmid, const void *shmaddr, int shmflg)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(shmid);
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    vsf_linux_shm_mem_t *mem = (vsf_linux_shm_mem_t *)priv->key;
    return mem->buffer;
}

int shmdt(const void *shmaddr)
{
    return 0;
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(shmid);
    vsf_linux_key_priv_t *priv = (vsf_linux_key_priv_t *)sfd->priv;
    vsf_linux_shm_mem_t *mem = (vsf_linux_shm_mem_t *)priv->key;
    switch (cmd) {
    case IPC_STAT:
        memset(buf, 0, sizeof(*buf));
        buf->shm_segsz = mem->size;
        buf->shm_perm.key = mem->key;
        break;
    case IPC_SET:
        VSF_LINUX_ASSERT(false);
        break;
    case IPC_RMID:
        close(shmid);
        break;
    }
    return 0;
}
#endif      // VSF_LINUX_CFG_SUPPORT_SHM

// sched

int sched_get_priority_max(int policy)
{
    return VSF_LINUX_CFG_PRIO_HIGHEST;
}

int sched_get_priority_min(int policy)
{
    return VSF_LINUX_CFG_PRIO_LOWEST;
}

int sched_getparam(pid_t pid, struct sched_param *param)
{
    vsf_linux_process_t *process = vsf_linux_get_process(pid);
    if (NULL == process) { return -1; }

    vsf_linux_thread_t *thread;
    vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
    if (NULL == thread) { return -1; }

    param->sched_priority = __vsf_eda_get_cur_priority(&thread->use_as__vsf_eda_t);
    return 0;
}

int sched_setparam(pid_t pid, const struct sched_param *param)
{
    vsf_linux_process_t *process = vsf_linux_get_process(pid);
    if (NULL == process) { return -1; }

    __vsf_dlist_foreach_unsafe(vsf_linux_thread_t, thread_node, &process->thread_list) {
        __vsf_eda_set_priority(&_->use_as__vsf_eda_t, param->sched_priority);
    }
    return 0;
}

int sched_getscheduler(pid_t pid)
{
    return 0;
}

int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param)
{
    return sched_setparam(pid, param);
}

int sched_yield(void)
{
    vsf_thread_yield();
    return 0;
}

// pty

int openpty(int *amaster, int *aslave, char *name,
                const struct termios *termp,
                const struct winsize *winp)
{
    char ptyp[11] = "/dev/ptyp0", ttyp[11] = "/dev/ttyp0";
    bool found = false;
    int master, slave;

    for (int i = 0; i < VSF_LINUX_CFG_MAX_PTY; i++) {
        ptyp[9] = ttyp[9] = '0' + i;
        master = open(ptyp, O_RDWR | O_NOCTTY);
        if (master < 0) {
            continue;
        }
        slave = open(ttyp, O_RDWR | O_NOCTTY);
        if (slave < 0) {
            close(master);
            continue;
        }
        found = true;
        break;
    }

    if (!found) {
        errno = ENOENT;
        return -1;
    }

    if (termp) {
        tcsetattr(slave, TCSAFLUSH, termp);
    }
    if (winp) {
        ioctl(slave, TIOCSWINSZ, winp);
    }

    *amaster = master;
    *aslave = slave;
    if (name != NULL) {
        strcpy(name, ttyp);
    }
    return 0;
}

int login_tty(int fd)
{
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
    return 0;
}

int uname(struct utsname *name)
{
    static const struct utsname __name = {
        .sysname    = VSF_LINUX_SYSNAME,
        .nodename   = VSF_LINUX_NODENAME,
        .release    = VSF_LINUX_RELEASE,
        .version    = VSF_LINUX_VERSION,
        .machine    = VSF_LINUX_MACHINE,
        .domainname = VSF_LINUX_DOMAINNAME,
    };
    if (name != NULL) {
        *name = __name;
    }
    return 0;
}

size_t getpagesize(void)
{
    return 1;
}

int gethostname(char *name, size_t len)
{
    strncpy(name, __vsf_linux.hostname, len);
    return 0;
}

int sethostname(const char *name, size_t len)
{
    if (len > HOST_NAME_MAX) {
        return -1;
    }
    memcpy(__vsf_linux.hostname, name, len);
    __vsf_linux.hostname[len] = '\0';
    return 0;
}

// sys/mman.h
void * mmap64(void *addr, size_t len, int prot, int flags, int fd, off64_t off)
{
    if (fd >= 0) {
        vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
        if ((NULL == sfd) || (NULL == sfd->op) || (NULL == sfd->op->fn_mmap)) {
            return MAP_FAILED;
        }
        if ((sfd->mmapped_buffer != NULL) && (sfd->mmapped_buffer != MAP_FAILED)) {
            return MAP_FAILED;
        }

        sfd->mmapped_buffer = sfd->op->fn_mmap(sfd, off, len, prot);
        return sfd->mmapped_buffer;
    }

    return malloc(len);
}

void * mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off)
{
    return mmap64(addr, len, prot, flags, fd, (off64_t)off);
}

int msync(void *addr, size_t len, int flags)
{
    vsf_linux_process_t *cur_process = vsf_linux_get_cur_process();
    __vsf_dlist_foreach_unsafe(vsf_linux_fd_t, fd_node, &cur_process->fd_list) {
        if (_->mmapped_buffer == addr) {
            return NULL == _->op->fn_msync ? 0 : _->op->fn_msync(_, _->mmapped_buffer);
        }
    }
    return 0;
}

int munmap(void *addr, size_t len)
{
    vsf_linux_process_t *cur_process = vsf_linux_get_cur_process();
    __vsf_dlist_foreach_unsafe(vsf_linux_fd_t, fd_node, &cur_process->fd_list) {
        if (_->mmapped_buffer == addr) {
            int ret = NULL == _->op->fn_munmap ? 0 : _->op->fn_munmap(_, _->mmapped_buffer);
            _->mmapped_buffer = NULL;
            return ret;
        }
    }

    free(addr);
    return 0;
}

int mprotect(void *addr, size_t len, int prot)
{
    return 0;
}

int shm_open(const char *name, int oflag, mode_t mode)
{
    return open(name, oflag, mode);
}

int shm_unlink(const char *name)
{
    return unlink(name);
}

// sys/random.h
#if VSF_HAL_USE_RNG == ENABLED && VSF_HW_RNG_COUNT > 0
static void __getrandom_on_ready(void *param, uint32_t *buffer, uint32_t num)
{
    vsf_eda_post_evt((vsf_eda_t *)param, VSF_EVT_USER);
}
#endif

ssize_t getrandom(void *buf, size_t buflen, unsigned int flags)
{
#if VSF_HAL_USE_RNG == ENABLED && VSF_HW_RNG_COUNT > 0
    vsf_eda_t *eda = vsf_eda_get_cur();
    VSF_LINUX_ASSERT(eda != NULL);
    if (VSF_ERR_NONE == vsf_hw_rng_generate_request(&vsf_hw_rng0, buf, buflen / (VSF_HW_RNG_BITLEN >> 3),
            eda, __getrandom_on_ready)) {
        vsf_thread_wfe(VSF_EVT_USER);
        return buflen;
    }
    return 0;
#else
    VSF_LINUX_ASSERT(false);
#endif
    return 0;
}

int getentropy(void *buf, size_t length)
{
    ssize_t length_ret = getrandom(buf, length, 0);
    return (length_ret == length) ? 0 : -1;
}

long gethostid(void)
{
    return __vsf_linux.hostid;
}

int sethostid(long hostid)
{
    __vsf_linux.hostid = hostid;
    return 0;
}

int getdtablesize(void)
{
    return 1024;
}

#if VSF_LINUX_USE_VFORK == ENABLED
pid_t __vsf_linux_vfork_prepare(vsf_linux_process_t *parent_process)
{
    vsf_linux_process_t *child_process = vsf_linux_create_process(0, 0, 0);
    if (NULL == child_process) {
        return (pid_t)-1;
    }

    // clone necessary resources from parent_process, goto delete_process_and_fail on failure
    vsf_linux_fd_t *sfd;
    __vsf_dlist_foreach_unsafe(vsf_linux_fd_t, fd_node, &parent_process->fd_list) {
        if (__vsf_linux_fd_create_ex(child_process, &sfd, _->op, _->fd, _->priv) != _->fd) {
            vsf_trace_error("vfork_exec: failed to dup fd %d", VSF_TRACE_CFG_LINEEND, _->fd);
            goto delete_process_and_fail;
        }
    }

    parent_process->is_vforking = true;
    parent_process->vfork_child = child_process;
    return child_process->id.pid;

delete_process_and_fail:
    vsf_linux_delete_process(child_process);
    return (pid_t)-1;
}
#endif

pid_t fork(void)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

// sysmacros
dev_t makedev(unsigned int maj, unsigned int min)
{
    return (dev_t)0;
}

unsigned int major(dev_t dev)
{
    return 0;
}

unsigned int minor(dev_t dev)
{
    return 0;
}

// spawn.h

static int __vsf_linux_spawn_ex(pid_t *pid, vsf_linux_main_entry_t entry,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[], void *priv, int priv_size, const char *path, bool use_path)
{
    vsf_linux_process_t *process = vsf_linux_create_process(0, VSF_LINUX_CFG_PEOCESS_HEAP_SIZE, priv_size);
    if (NULL == process) { return -ENOMEM; }
    vsf_linux_process_ctx_t *ctx = &process->ctx;
    vsf_linux_process_t *cur_process = process->parent_process;

    if (NULL == entry) {
#if __VSF_LINUX_PROCESS_HAS_PATH
        if (__vsf_linux_get_exe(process->path, sizeof(process->path), (char *)path, &entry, use_path) < 0) {
#else
        if (__vsf_linux_get_exe(NULL, 0, (char *)path, &entry, use_path) < 0) {
#endif
            if (pid != NULL) {
                *pid = -1;
            }
            return -1;
        }
    } else {
#if __VSF_LINUX_PROCESS_HAS_PATH
        strcpy(process->path, cur_process->path);
#endif
    }
    ctx->entry = entry;

    if (argv != NULL) {
        __vsf_linux_process_parse_arg(process, NULL, argv);
    }

    process->shell_process = cur_process->shell_process;

    // dup fds
    vsf_linux_fd_t *sfd, *sfd_new;
    vsf_protect_t orig;
    __vsf_dlist_foreach_unsafe(vsf_linux_fd_t, fd_node, &cur_process->fd_list) {
        if (__vsf_linux_fd_create_ex(process, &sfd, _->op, _->fd, _->priv) != _->fd) {
            vsf_trace_error("spawn: failed to dup fd %d", VSF_TRACE_CFG_LINEEND, _->fd);
            goto delete_process_and_fail;
        }
    }

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
    // env
    if (vsf_linux_merge_env(process, (char **)env) < 0) {
        goto delete_process_and_fail;
    }
#endif

    // apply actions
    if (actions != NULL) {
        char fullpath[PATH_MAX];
        struct spawn_action *a = actions->actions;
        for (int i = 0; i < actions->used; i++, a++) {
            switch (a->tag) {
            case spawn_do_close:
                if (__vsf_linux_fd_close_ex(process, a->action.close_action.fd) < 0) {
                    vsf_trace_error("spawn: action: failed to close fd %d", VSF_TRACE_CFG_LINEEND,
                        a->action.close_action.fd);
                    goto delete_process_and_fail;
                }
                break;
            case spawn_do_dup2:
                sfd = __vsf_linux_fd_get_ex(process, a->action.dup2_action.newfd);
                if (sfd != NULL) {
                    orig = vsf_protect_sched();
                        sfd->priv->ref--;
#if VSF_LINUX_CFG_FD_TRACE == ENABLED
                        vsf_trace_debug("%s action dup2_close: process 0x%p fd %d priv 0x%p ref %d" VSF_TRACE_CFG_LINEEND,
                            __FUNCTION__, process, sfd->fd, sfd->priv, sfd->priv->ref);
#endif
                    vsf_unprotect_sched(orig);
                    ____vsf_linux_fd_delete_ex(process, sfd);
                }

                sfd = __vsf_linux_fd_get_ex(process, a->action.dup2_action.fd);
                if (NULL == sfd) {
                action_fail_dup:
                    vsf_trace_error("spawn: action: failed to dup fd %d", VSF_TRACE_CFG_LINEEND,
                        a->action.dup2_action.fd);
                    goto delete_process_and_fail;
                }

                int ret = __vsf_linux_fd_create_ex(process, &sfd_new, sfd->op, a->action.dup2_action.newfd, sfd->priv);
                if (ret < 0) {
                    goto action_fail_dup;
                }
                break;
            case spawn_do_open:
                VSF_LINUX_ASSERT(false);
                break;
            case spawn_do_fchdir: {
                    vk_file_t *file = __vsf_linux_get_fs_ex(process, a->action.fchdir_action.fd);
                    if (NULL == file) {
                    action_fail_fchdir:
                        vsf_trace_error("spawn: action: failed to fchdir fd %d", VSF_TRACE_CFG_LINEEND,
                            a->action.fchdir_action.fd);
                        goto delete_process_and_fail;
                    }

                    char *ptr = &fullpath[sizeof(fullpath) - 1];
                    size_t namelen;
                    *ptr-- = '\0';
                    while (file != NULL) {
                        namelen = strlen(file->name);
                        ptr -= strlen(file->name);
                        if (ptr < fullpath) {
                            ptr = NULL;
                            break;
                        }
                        memcpy(ptr, file->name, namelen);
                        file = file->parent;
                    }

                    if (NULL == ptr) {
                        goto action_fail_fchdir;
                    }
                    a->action.chdir_action.path = ptr;
                }
                // fall through
            case spawn_do_chdir:
                if (vsf_linux_chdir(process, a->action.chdir_action.path) < 0) {
                    vsf_trace_error("spawn: action: failed to chdir fd %s", VSF_TRACE_CFG_LINEEND,
                        a->action.chdir_action.path);
                    goto delete_process_and_fail;
                }
                break;
            }
        }
    }

    if (pid != NULL) {
        *pid = process->id.pid;
    }
    if ((priv != NULL) && (priv_size > 0)) {
        memcpy(__vsf_linux_get_process_priv(process), priv, priv_size);
    }
    return vsf_linux_start_process(process);
delete_process_and_fail:
    vsf_linux_delete_process(process);
    return -1;
}

// the last parameter, which is priv_size is used to capture necessary variables when
//  use spawn to replace fork/vfork.
// __vsf_linux_get_process_priv can be used to get the buffer in priv_size which is
//  bounded to process.
int __vsf_linux_spawn(pid_t *pid, vsf_linux_main_entry_t entry,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[], void *priv, int priv_size)
{
    return __vsf_linux_spawn_ex(pid, entry, actions, attr, argv, env, priv, priv_size, NULL, false);
}

int posix_spawnp(pid_t *pid, const char *file,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[])
{
    return __vsf_linux_spawn_ex(pid, NULL, actions, attr, argv, env, NULL, 0, file, true);
}

int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[])
{
    return __vsf_linux_spawn_ex(pid, NULL, actions, attr, argv, env, NULL, 0, path, false);
}

int posix_spawnattr_init(posix_spawnattr_t *attr)
{
    memset(attr, 0, sizeof(*attr));
    return 0;
}

int posix_spawnattr_destroy(posix_spawnattr_t *attr)
{
    return 0;
}

int posix_spawnattr_getsigdefault(const posix_spawnattr_t *attr, sigset_t *sigdefault)
{
    return 0;
}

int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr, const sigset_t *sigdefault)
{
    return 0;
}

int posix_spawnattr_getsigmask(const posix_spawnattr_t *attr, sigset_t *sigmask)
{
    return 0;
}

int posix_spawnattr_setsigmask(posix_spawnattr_t *attr, const sigset_t *sigmask)
{
    return 0;
}

int posix_spawnattr_getflags(const posix_spawnattr_t *attr, short int *flags)
{
    return 0;
}

int posix_spawnattr_setflags(posix_spawnattr_t *attr, short int flags)
{
    return 0;
}

int posix_spawnattr_getpgroup(const posix_spawnattr_t *attr, pid_t *pgroup)
{
    return 0;
}

int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup)
{
    return 0;
}

int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *attr, int *schedpolicy)
{
    return 0;
}

int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy)
{
    return 0;
}

int posix_spawnattr_getschedparam(const posix_spawnattr_t *attr, struct sched_param *schedparam)
{
    return 0;
}

int posix_spawnattr_setschedparam(posix_spawnattr_t *attr, const struct sched_param *schedparam)
{
    return 0;
}

static struct spawn_action * __posix_spawn_file_actions_add(posix_spawn_file_actions_t *actions)
{
    if (actions->used == actions->allocated) {
        actions->allocated += 8;
        actions->actions = realloc(actions->actions, actions->allocated * sizeof(struct spawn_action));
    }
    if (NULL == actions->actions) {
        return NULL;
    }

    return &actions->actions[actions->used++];
}

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *actions)
{
    memset(actions, 0, sizeof(*actions));
    return 0;
}

int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *actions)
{
    if (actions->actions != NULL) {
        free(actions->actions);
    }
    return 0;
}

int posix_spawn_file_actions_addopen(
                posix_spawn_file_actions_t *actions,
                int fd, const char *path,
                int oflag, mode_t mode)
{
    struct spawn_action * action = __posix_spawn_file_actions_add(actions);
    if (NULL == action) {
        return -1;
    }

    action->tag = spawn_do_open;
    action->action.open_action.fd = fd;
    action->action.open_action.path = (char *)path;
    action->action.open_action.oflag = oflag;
    action->action.open_action.mode = mode;
    return 0;
}

int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *actions, int fd)
{
    struct spawn_action * action = __posix_spawn_file_actions_add(actions);
    if (NULL == action) {
        return -1;
    }

    action->tag = spawn_do_close;
    action->action.close_action.fd = fd;
    return 0;
}

int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *actions, int fd, int newfd)
{
    struct spawn_action * action = __posix_spawn_file_actions_add(actions);
    if (NULL == action) {
        return -1;
    }

    action->tag = spawn_do_dup2;
    action->action.dup2_action.fd = fd;
    action->action.dup2_action.newfd = newfd;
    return 0;
}

int posix_spawn_file_actions_addchdir_np(posix_spawn_file_actions_t *actions, const char *path)
{
    struct spawn_action * action = __posix_spawn_file_actions_add(actions);
    if (NULL == action) {
        return -1;
    }

    action->tag = spawn_do_chdir;
    action->action.chdir_action.path = (char *)path;
    return 0;
}

int posix_spawn_file_actions_addfchdir_np(posix_spawn_file_actions_t *actions, int fd)
{
    struct spawn_action * action = __posix_spawn_file_actions_add(actions);
    if (NULL == action) {
        return -1;
    }

    action->tag = spawn_do_fchdir;
    action->action.fchdir_action.fd = fd;
    return 0;
}

// termios.h
int tcgetattr(int fd, struct termios *termios)
{
    return ioctl(fd, TCGETS, termios);
}

int tcsetattr(int fd, int optional_actions, const struct termios *termios)
{
    return ioctl(fd, TCSETS, termios);
}

pid_t tcgetpgrp(int fd)
{
    return getpgrp();
}

int tcsetpgrp(int fd, pid_t pgrp)
{
    return setpgid(0, pgrp);
}

int tcsendbreak(int fd, int duration)
{
    return 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wcast-align"
#endif

int tcdrain(int fd)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if ((NULL == sfd) || (sfd->op != &vsf_linux_term_fdop)) { return -1; }
    vsf_linux_term_priv_t *term_priv = (vsf_linux_term_priv_t *)sfd->priv;
    __vsf_linux_tx_stream_drain(&term_priv->use_as__vsf_linux_stream_priv_t);
    return 0;
}

int tcflush(int fd, int queue_selector)
{
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    if ((NULL == sfd) || (sfd->op != &vsf_linux_term_fdop)) { return -1; }
    vsf_linux_term_priv_t *term_priv = (vsf_linux_term_priv_t *)sfd->priv;
    uint8_t op;

    if (queue_selector == TCIOFLUSH) {
        op = (1 << TCIFLUSH) | (1 << TCOFLUSH);
    } else {
        op = 1 << queue_selector;
    }

    if (op & (1 << TCIFLUSH)) {
        __vsf_linux_rx_stream_drop(&term_priv->use_as__vsf_linux_stream_priv_t);
    }
    if (op & (1 << TCOFLUSH)) {
        __vsf_linux_tx_stream_drop(&term_priv->use_as__vsf_linux_stream_priv_t);
    }
    return 0;
}

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

int tcflow(int fd, int action)
{
    return -1;
}

void cfmakeraw(struct termios *termios)
{
}

speed_t cfgetispeed(const struct termios *termios)
{
    return termios->c_ispeed;
}

speed_t cfgetospeed(const struct termios *termios)
{
    return termios->c_ospeed;
}

int cfsetispeed(struct termios *termios, speed_t speed)
{
    termios->c_ispeed = speed;
    return 0;
}

int cfsetospeed(struct termios *termios, speed_t speed)
{
    termios->c_ospeed = speed;
    return 0;
}

int cfsetspeed(struct termios *termios, speed_t speed)
{
    termios->c_ispeed = speed;
    termios->c_ospeed = speed;
    return 0;
}

int vsf_linux_expandenv(const char *str, char *output, size_t bufflen)
{
    size_t envlen = 0, copylen, tmplen, namelen;
    char ch, *env_start, *env_end, *name_start, *name_end, *env_value;

#define __copy2output(__str_from, __copylen)                                    \
        do {                                                                    \
            if (output != NULL) {                                               \
                tmplen = vsf_min(bufflen, __copylen);                           \
                if (tmplen > 0) {                                               \
                    memcpy(output, __str_from, tmplen);                         \
                    output += tmplen;                                           \
                }                                                               \
            }                                                                   \
            envlen += __copylen;                                                \
        } while (false)

    bufflen--;      // reserve one byte for '\0'
    while (true) {
        env_start = strchr(str, '$');
        if (NULL == env_start) {
            copylen = strlen(str);
            __copy2output(str, copylen);
            if (output != NULL) {
                *output = '\0';
            }
            return envlen;
        }
        copylen = env_start - str;
        __copy2output(str, copylen);

        if ('{' == env_start[1]) {
            name_start = env_end = env_start + 2;
        } else {
            name_start = env_end = env_start + 1;
        }
        while (true) {
            ch = *env_end;
            if (!(  (ch == '_')
                ||  ((ch >= '0') && (ch <= '9'))
                ||  ((ch >= 'a') && (ch <= 'z'))
                ||  ((ch >= 'A') && (ch <= 'Z')))) {
                break;
            }
            env_end++;
        }
        if ('{' == env_start[1]) {
            if ('}' != *env_end) {
                return -1;
            }
            name_end = env_end++ - 1;
        } else {
            name_end = env_end - 1;
        }
        if (name_end < name_start) {
            return -1;
        }

        namelen = name_end - name_start + 1;
        char env_name[namelen + 1];
        memcpy(env_name, name_start, namelen);
        env_name[namelen] = '\0';
        env_value = getenv(env_name);
        if (env_value != NULL) {
            copylen = strlen(env_value);
            __copy2output(env_value, copylen);
        }

        str = env_end;
    }

    return envlen;
}

// langinfo
char * nl_langinfo(nl_item item)
{
    switch (item) {
    case CODESET:   return "ANSI_X3.4-1968";
    default:        return NULL;
    }
}

// dlfcn

#if VSF_USE_LOADER == ENABLED
static void * __dlmalloc(int size)
{
    return malloc((size_t)size);
}

static uint32_t __vsf_linux_loader_fd_read(vsf_loader_target_t *target, uint32_t offset, void *buffer, uint32_t size)
{
    int fd = (int)target->object;
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        return 0;
    }
    return read(fd, buffer, size);
}

static void __vsf_linux_loader_atexit(void)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    vsf_linux_dynloader_t *linux_loader = process->loader;
    VSF_LINUX_ASSERT(linux_loader != NULL);

    dlclose(linux_loader);
}

void vsf_linux_loader_keep_ram(void)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    vsf_linux_dynloader_t *linux_loader = process->loader;
    VSF_LINUX_ASSERT(linux_loader != NULL);

    linux_loader->loader.generic.static_base = NULL;
}
#endif

void * dlopen(const char *pathname, int mode)
{
#if VSF_USE_LOADER == ENABLED
    int fd = open(pathname, 0);
    if (fd < 0) {
        return NULL;
    }

    vsf_linux_dynloader_t *linux_loader = calloc(1, sizeof(vsf_linux_dynloader_t));
    if (NULL == linux_loader) {
        goto close_and_fail;
    }

    vk_file_t *file = __vsf_linux_get_fs_ex(NULL, fd);
    void *file_direct_access;
    if ((file != NULL) && ((file_direct_access = vk_file_direct_access(file)) != NULL)) {
        linux_loader->target.object             = (uintptr_t)file_direct_access;
        linux_loader->target.support_xip        = true;
        linux_loader->target.fn_read            = vsf_loader_xip_read;
    } else {
        linux_loader->target.object             = (uintptr_t)fd;
        linux_loader->target.support_xip        = false;
        linux_loader->target.fn_read            = __vsf_linux_loader_fd_read;
    }

    linux_loader->loader.generic.heap_op    = &vsf_loader_default_heap_op;
    linux_loader->loader.generic.vplt       = (void *)&vsf_linux_vplt;
    linux_loader->loader.generic.alloc_vplt = __dlmalloc;
    linux_loader->loader.generic.free_vplt  = free;

    uint8_t header[16];
    uint32_t size = vsf_loader_read(&linux_loader->target, 0, header, sizeof(header));
    linux_loader->loader.generic.op = __vsf_linux_get_applet_loader(header, size);
    if (NULL == linux_loader->loader.generic.op) {
        printf("dlopen: unsupported file format\n");
        goto close_and_fail;
    }

    vsf_linux_process_t *process = vsf_linux_get_real_process(NULL);
    VSF_LINUX_ASSERT(process != NULL);
    process->loader = linux_loader;
    atexit(__vsf_linux_loader_atexit);
    if (!vsf_loader_load(&linux_loader->loader.generic, &linux_loader->target)) {
        vsf_loader_call_init_array(&linux_loader->loader.generic);
        return linux_loader;
    }

close_and_fail:
    close(fd);
    return NULL;
#else
    return NULL;
#endif
}

int dlclose(void *handle)
{
#if VSF_USE_LOADER == ENABLED
    vsf_linux_dynloader_t *linux_loader = handle;
    vsf_loader_call_fini_array(&linux_loader->loader.generic);
    vsf_loader_cleanup(&linux_loader->loader.generic);
    free(linux_loader);
    return 0;
#else
    return -1;
#endif
}

void * dlsym(void *handle, const char *name)
{
    void *vplt = NULL;

    if (RTLD_DEFAULT == handle) {
#if VSF_USE_APPLET == ENABLED
        vplt = (void *)&vsf_vplt;
#endif
    } else {
#if VSF_USE_APPLET == ENABLED && VSF_LINUX_USE_APPLET == ENABLED && VSF_APPLET_CFG_LINKABLE == ENABLED
        vsf_linux_dynloader_t *linux_loader = handle;
        vplt = (void*)linux_loader->loader.generic.vplt_out;
#endif
    }
    if (NULL == vplt) {
        return NULL;
    }

#if VSF_USE_APPLET == ENABLED && VSF_LINUX_USE_APPLET == ENABLED && VSF_APPLET_CFG_LINKABLE == ENABLED
    return vsf_vplt_link(vplt, (char *)name);
#else
    return NULL;
#endif
}

char * dlerror(void)
{
    return "known";
}

// pwd

struct passwd * getpwuid(uid_t uid)
{
    return (struct passwd *)&__vsf_linux_default_passwd;
}

struct passwd * getpwnam(const char *name)
{
    return (struct passwd *)&__vsf_linux_default_passwd;
}

// grp

int initgroups(const char *user, gid_t group)
{
    return -1;
}

int getgroups(size_t size, gid_t list[])
{
    return -1;
}

int setgroups(size_t size, const gid_t *list)
{
    return -1;
}

struct group * getgrnam(const char *name)
{
    if (!strcmp(__vsf_linux_default_group.gr_name, name)) {
        return (struct group *)&__vsf_linux_default_group;
    }
    return NULL;
}

struct group * getgrgid(gid_t gid)
{
    if (gid == __vsf_linux_default_group.gr_gid) {
        return (struct group *)&__vsf_linux_default_group;
    }
    return NULL;
}

int getgrnam_r(const char *name, struct group *grp,
          char *buf, size_t buflen, struct group **result)
{
    return -1;
}

int getgrgid_r(gid_t gid, struct group *grp,
          char *buf, size_t buflen, struct group **result)
{
    return -1;
}

struct group * getgrent(void)
{
    return NULL;
}

void setgrent(void)
{
}

void endgrent(void)
{
}

// resources

int getpriority(int which, id_t who)
{
    return -1;
}

int setpriority(int which, id_t who, int prio)
{
    return -1;
}

// timex

int adjtimex(struct timex *buf)
{
    return -1;
}

int clock_adjtime(clockid_t clk_id, struct timex *buf)
{
    return -1;
}

int ntp_adjtime(struct timex *buf)
{
    return -1;
}

// sysinfo

int sysinfo(struct sysinfo *info)
{
    VSF_LINUX_ASSERT(info != NULL);
    memset(info, 0, sizeof(*info));

    info->uptime = vsf_systimer_get_ms() / 1000;
    vsf_protect_t orig = vsf_protect_sched();
        info->procs = vsf_dlist_get_length(&__vsf_linux.process_list);
    vsf_unprotect_sched(orig);

#if VSF_USE_HEAP == ENABLED && VSF_HEAP_CFG_STATISTICS == ENABLED
    vsf_heap_statistics_t heap_statistics;
#   if VSF_LINUX_CFG_HEAP_SIZE > 0
    __vsf_heap_statistics(&__vsf_linux_heap.use_as__vsf_heap_t, &heap_statistics);
#   elif VSF_USE_ARCH_HEAP != ENABLED || VSF_ARCH_HEAP_HAS_STATISTICS == ENABLED
    vsf_heap_statistics(&heap_statistics);
#   endif
    info->totalram = heap_statistics.all_size;
    info->freeram = heap_statistics.all_size - heap_statistics.used_size;
#endif
    info->mem_unit = 1;
    return 0;
}

// vplt
#if VSF_LINUX_USE_APPLET == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_fundmental_vplt_t vsf_linux_fundmental_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_fundmental_vplt_t, 0, 0, true),

#if VSF_LINUX_CFG_PLS_NUM > 0
    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_linux_dynlib_ctx_get),
    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_linux_dynlib_ctx_set),
#endif
    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_linux_get_cur_process),
#if VSF_USE_LOADER == ENABLED
    VSF_APPLET_VPLT_ENTRY_FUNC(vsf_linux_loader_keep_ram),
#endif
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_RANDOM == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_random_vplt_t vsf_linux_sys_random_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_random_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(getrandom),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_SHM == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_shm_vplt_t vsf_linux_sys_shm_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_shm_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(shmget),
    VSF_APPLET_VPLT_ENTRY_FUNC(shmat),
    VSF_APPLET_VPLT_ENTRY_FUNC(shmdt),
    VSF_APPLET_VPLT_ENTRY_FUNC(shmctl),
};
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#if VSF_LINUX_APPLET_USE_SYS_TIME == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_time_vplt_t vsf_linux_sys_time_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_time_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(gettimeofday),
    VSF_APPLET_VPLT_ENTRY_FUNC(settimeofday),
    VSF_APPLET_VPLT_ENTRY_FUNC(getitimer),
    VSF_APPLET_VPLT_ENTRY_FUNC(setitimer),
    VSF_APPLET_VPLT_ENTRY_FUNC(futimes),
    VSF_APPLET_VPLT_ENTRY_FUNC(utimes),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_TIMES == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_times_vplt_t vsf_linux_sys_times_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_times_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(times),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_TIMEB == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_timeb_vplt_t vsf_linux_sys_timeb_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_timeb_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(ftime),
};
#endif
#endif

#if VSF_LINUX_APPLET_USE_SYS_TIMEX == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_timex_vplt_t vsf_linux_sys_timex_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_timex_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(adjtimex),
    VSF_APPLET_VPLT_ENTRY_FUNC(clock_adjtime),
    VSF_APPLET_VPLT_ENTRY_FUNC(ntp_adjtime),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_UTSNAME == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_utsname_vplt_t vsf_linux_sys_utsname_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_utsname_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(uname),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_WAIT == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_wait_vplt_t vsf_linux_sys_wait_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_wait_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(wait),
    VSF_APPLET_VPLT_ENTRY_FUNC(waitpid),
    VSF_APPLET_VPLT_ENTRY_FUNC(waitid),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_CAPABILITY == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_capability_vplt_t vsf_linux_sys_capability_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_capability_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(capget),
    VSF_APPLET_VPLT_ENTRY_FUNC(capset),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_MMAN == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_mman_vplt_t vsf_linux_sys_mman_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_mman_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(shm_open),
    VSF_APPLET_VPLT_ENTRY_FUNC(shm_unlink),
    VSF_APPLET_VPLT_ENTRY_FUNC(mmap),
    VSF_APPLET_VPLT_ENTRY_FUNC(mmap64),
    VSF_APPLET_VPLT_ENTRY_FUNC(munmap),
    VSF_APPLET_VPLT_ENTRY_FUNC(mprotect),
    VSF_APPLET_VPLT_ENTRY_FUNC(msync),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_RESOURCE == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_resource_vplt_t vsf_linux_sys_resource_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_resource_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(getpriority),
    VSF_APPLET_VPLT_ENTRY_FUNC(setpriority),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_INFO == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_info_vplt_t vsf_linux_sys_info_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_info_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(sysinfo),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_PRCTL == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_prctl_vplt_t vsf_linux_sys_prctl_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_prctl_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(prctl),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_SEM == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_sem_vplt_t vsf_linux_sys_sem_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_sem_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(__semctl_va),
    VSF_APPLET_VPLT_ENTRY_FUNC(semctl),
    VSF_APPLET_VPLT_ENTRY_FUNC(semget),
    VSF_APPLET_VPLT_ENTRY_FUNC(semop),
    VSF_APPLET_VPLT_ENTRY_FUNC(semtimedop),
};
#endif

#if VSF_LINUX_APPLET_USE_SIGNAL == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_signal_vplt_t vsf_linux_signal_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_signal_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(kill),
    VSF_APPLET_VPLT_ENTRY_FUNC(sigprocmask),
    VSF_APPLET_VPLT_ENTRY_FUNC(signal),
    VSF_APPLET_VPLT_ENTRY_FUNC(sigaction),
    VSF_APPLET_VPLT_ENTRY_FUNC(raise),
    VSF_APPLET_VPLT_ENTRY_FUNC(pthread_sigmask),
    VSF_APPLET_VPLT_ENTRY_FUNC(sigwaitinfo),
    VSF_APPLET_VPLT_ENTRY_FUNC(sigtimedwait),
    VSF_APPLET_VPLT_ENTRY_FUNC(sigsuspend),
};
#endif

#if VSF_LINUX_APPLET_USE_SCHED == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sched_vplt_t vsf_linux_sched_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sched_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(sched_get_priority_max),
    VSF_APPLET_VPLT_ENTRY_FUNC(sched_get_priority_min),
    VSF_APPLET_VPLT_ENTRY_FUNC(sched_getparam),
    VSF_APPLET_VPLT_ENTRY_FUNC(sched_getscheduler),
    VSF_APPLET_VPLT_ENTRY_FUNC(sched_setparam),
    VSF_APPLET_VPLT_ENTRY_FUNC(sched_setscheduler),
    VSF_APPLET_VPLT_ENTRY_FUNC(sched_yield),
};
#endif

#if VSF_LINUX_APPLET_USE_SPAWN == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_spawn_vplt_t vsf_linux_spawn_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_spawn_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawn),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnp),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_getsigdefault),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_setsigdefault),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_getsigmask),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_setsigmask),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_getflags),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_setflags),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_getpgroup),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_setpgroup),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_getschedpolicy),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_setschedpolicy),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_getschedparam),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawnattr_setschedparam),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawn_file_actions_init),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawn_file_actions_destroy),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawn_file_actions_addopen),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawn_file_actions_addclose),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawn_file_actions_adddup2),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawn_file_actions_addchdir_np),
    VSF_APPLET_VPLT_ENTRY_FUNC(posix_spawn_file_actions_addfchdir_np),
};
#endif

#if VSF_LINUX_APPLET_USE_PTY == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_pty_vplt_t vsf_linux_pty_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_pty_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(openpty),
    VSF_APPLET_VPLT_ENTRY_FUNC(login_tty),
};
#endif

#if VSF_LINUX_APPLET_USE_DLFCN == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_dlfcn_vplt_t vsf_linux_dlfcn_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_dlfcn_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(dlopen),
    VSF_APPLET_VPLT_ENTRY_FUNC(dlclose),
    VSF_APPLET_VPLT_ENTRY_FUNC(dlsym),
    VSF_APPLET_VPLT_ENTRY_FUNC(dlerror),
};
#endif

#if VSF_LINUX_APPLET_USE_GRP == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_grp_vplt_t vsf_linux_grp_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_grp_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(initgroups),
    VSF_APPLET_VPLT_ENTRY_FUNC(getgroups),
    VSF_APPLET_VPLT_ENTRY_FUNC(setgroups),
    VSF_APPLET_VPLT_ENTRY_FUNC(getgrnam),
    VSF_APPLET_VPLT_ENTRY_FUNC(getgrgid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getgrnam_r),
    VSF_APPLET_VPLT_ENTRY_FUNC(getgrgid_r),
    VSF_APPLET_VPLT_ENTRY_FUNC(getgrent),
    VSF_APPLET_VPLT_ENTRY_FUNC(setgrent),
    VSF_APPLET_VPLT_ENTRY_FUNC(endgrent),
};
#endif

#if VSF_LINUX_APPLET_USE_PWD == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_pwd_vplt_t vsf_linux_pwd_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_pwd_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(getpwuid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getpwnam),
};
#endif

#if VSF_LINUX_APPLET_USE_ERRNO == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_errno_vplt_t vsf_linux_errno_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_errno_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(__vsf_linux_errno),
};
#endif

#if VSF_LINUX_APPLET_USE_TERMIOS == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_termios_vplt_t vsf_linux_termios_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_termios_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(tcgetattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(tcsetattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(tcsendbreak),
    VSF_APPLET_VPLT_ENTRY_FUNC(tcdrain),
    VSF_APPLET_VPLT_ENTRY_FUNC(tcflush),
    VSF_APPLET_VPLT_ENTRY_FUNC(tcflow),
    VSF_APPLET_VPLT_ENTRY_FUNC(cfmakeraw),
    VSF_APPLET_VPLT_ENTRY_FUNC(cfgetispeed),
    VSF_APPLET_VPLT_ENTRY_FUNC(cfgetospeed),
    VSF_APPLET_VPLT_ENTRY_FUNC(cfsetispeed),
    VSF_APPLET_VPLT_ENTRY_FUNC(cfsetospeed),
    VSF_APPLET_VPLT_ENTRY_FUNC(cfsetspeed),
};
#endif

#if VSF_LINUX_APPLET_USE_UNISTD == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_unistd_vplt_t vsf_linux_unistd_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_unistd_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(confstr),
    VSF_APPLET_VPLT_ENTRY_FUNC(usleep),
    VSF_APPLET_VPLT_ENTRY_FUNC(sleep),
    VSF_APPLET_VPLT_ENTRY_FUNC(alarm),
    VSF_APPLET_VPLT_ENTRY_FUNC(ualarm),
    VSF_APPLET_VPLT_ENTRY_FUNC(getlogin),
    VSF_APPLET_VPLT_ENTRY_FUNC(getlogin_r),
    VSF_APPLET_VPLT_ENTRY_FUNC(setgid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getgid),
    VSF_APPLET_VPLT_ENTRY_FUNC(setegid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getegid),
    VSF_APPLET_VPLT_ENTRY_FUNC(setuid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getuid),
    VSF_APPLET_VPLT_ENTRY_FUNC(seteuid),
    VSF_APPLET_VPLT_ENTRY_FUNC(geteuid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getpid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getppid),
    VSF_APPLET_VPLT_ENTRY_FUNC(gettid),
    VSF_APPLET_VPLT_ENTRY_FUNC(setsid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getsid),
    VSF_APPLET_VPLT_ENTRY_FUNC(setpgid),
    VSF_APPLET_VPLT_ENTRY_FUNC(getpgid),
    VSF_APPLET_VPLT_ENTRY_FUNC(setpgrp),
    VSF_APPLET_VPLT_ENTRY_FUNC(getpgrp),
    VSF_APPLET_VPLT_ENTRY_FUNC(setresuid),
    VSF_APPLET_VPLT_ENTRY_FUNC(setresgid),
    VSF_APPLET_VPLT_ENTRY_FUNC(__execl_va),
    VSF_APPLET_VPLT_ENTRY_FUNC(execl),
    VSF_APPLET_VPLT_ENTRY_FUNC(__execlp_va),
    VSF_APPLET_VPLT_ENTRY_FUNC(execlp),
    VSF_APPLET_VPLT_ENTRY_FUNC(execv),
    VSF_APPLET_VPLT_ENTRY_FUNC(execve),
    VSF_APPLET_VPLT_ENTRY_FUNC(execvp),
    VSF_APPLET_VPLT_ENTRY_FUNC(execvpe),
    VSF_APPLET_VPLT_ENTRY_FUNC(daemon),
    VSF_APPLET_VPLT_ENTRY_FUNC(sysconf),
    VSF_APPLET_VPLT_ENTRY_FUNC(pathconf),
    VSF_APPLET_VPLT_ENTRY_FUNC(fpathconf),
    VSF_APPLET_VPLT_ENTRY_FUNC(realpath),
    VSF_APPLET_VPLT_ENTRY_FUNC(pipe),
    VSF_APPLET_VPLT_ENTRY_FUNC(pipe2),
    VSF_APPLET_VPLT_ENTRY_FUNC(access),
    VSF_APPLET_VPLT_ENTRY_FUNC(unlink),
    VSF_APPLET_VPLT_ENTRY_FUNC(unlinkat),
    VSF_APPLET_VPLT_ENTRY_FUNC(link),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkdirat),
    VSF_APPLET_VPLT_ENTRY_FUNC(rmdir),
    VSF_APPLET_VPLT_ENTRY_FUNC(dup),
    VSF_APPLET_VPLT_ENTRY_FUNC(dup2),
    VSF_APPLET_VPLT_ENTRY_FUNC(dup3),
    VSF_APPLET_VPLT_ENTRY_FUNC(chroot),
    VSF_APPLET_VPLT_ENTRY_FUNC(chdir),
    VSF_APPLET_VPLT_ENTRY_FUNC(fchdir),
    VSF_APPLET_VPLT_ENTRY_FUNC(getcwd),
    VSF_APPLET_VPLT_ENTRY_FUNC(close),
    VSF_APPLET_VPLT_ENTRY_FUNC(lseek),
    VSF_APPLET_VPLT_ENTRY_FUNC(read),
    VSF_APPLET_VPLT_ENTRY_FUNC(write),
    VSF_APPLET_VPLT_ENTRY_FUNC(readv),
    VSF_APPLET_VPLT_ENTRY_FUNC(writev),
    VSF_APPLET_VPLT_ENTRY_FUNC(pread),
    VSF_APPLET_VPLT_ENTRY_FUNC(pwrite),
    VSF_APPLET_VPLT_ENTRY_FUNC(preadv),
    VSF_APPLET_VPLT_ENTRY_FUNC(pwritev),
    VSF_APPLET_VPLT_ENTRY_FUNC(sync),
    VSF_APPLET_VPLT_ENTRY_FUNC(syncfs),
    VSF_APPLET_VPLT_ENTRY_FUNC(fsync),
    VSF_APPLET_VPLT_ENTRY_FUNC(fdatasync),
    VSF_APPLET_VPLT_ENTRY_FUNC(isatty),
    VSF_APPLET_VPLT_ENTRY_FUNC(getpagesize),
    VSF_APPLET_VPLT_ENTRY_FUNC(symlink),
    VSF_APPLET_VPLT_ENTRY_FUNC(ftruncate),
    VSF_APPLET_VPLT_ENTRY_FUNC(truncate),
    VSF_APPLET_VPLT_ENTRY_FUNC(ftruncate64),
    VSF_APPLET_VPLT_ENTRY_FUNC(truncate64),
    VSF_APPLET_VPLT_ENTRY_FUNC(readlink),
    VSF_APPLET_VPLT_ENTRY_FUNC(tcgetpgrp),
    VSF_APPLET_VPLT_ENTRY_FUNC(tcsetpgrp),
    VSF_APPLET_VPLT_ENTRY_FUNC(getpass),
    VSF_APPLET_VPLT_ENTRY_FUNC(gethostname),
    VSF_APPLET_VPLT_ENTRY_FUNC(sethostname),
    VSF_APPLET_VPLT_ENTRY_FUNC(chown),
    VSF_APPLET_VPLT_ENTRY_FUNC(fchown),
    VSF_APPLET_VPLT_ENTRY_FUNC(lchown),
    VSF_APPLET_VPLT_ENTRY_FUNC(fchownat),
    VSF_APPLET_VPLT_ENTRY_FUNC(getentropy),
    VSF_APPLET_VPLT_ENTRY_FUNC(gethostid),
    VSF_APPLET_VPLT_ENTRY_FUNC(sethostid),
    VSF_APPLET_VPLT_ENTRY_FUNC(ttyname),
    VSF_APPLET_VPLT_ENTRY_FUNC(ttyname_r),
    VSF_APPLET_VPLT_ENTRY_FUNC(_exit),
    VSF_APPLET_VPLT_ENTRY_FUNC(acct),
    VSF_APPLET_VPLT_ENTRY_FUNC(__vsf_linux_vfork_prepare),
    VSF_APPLET_VPLT_ENTRY_FUNC(mkdirs),
    VSF_APPLET_VPLT_ENTRY_FUNC(getdtablesize),
};
#endif

#if VSF_LINUX_USE_APPLET == ENABLED && !defined(__VSF_APPLET__)
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   if VSF_LINUX_APPLET_USE_PTHREAD == ENABLED
#       include "./include/pthread.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_EPOLL == ENABLED
#       include "./include/sys/epoll.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_EVENTFD == ENABLED
#       include "./include/sys/eventfd.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_SENDFILE == ENABLED
#       include "./include/sys/sendfile.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_FILE == ENABLED
#       include "./include/sys/file.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_PRCTL == ENABLED
#       include "./include/sys/prctl.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_EVENT == ENABLED
#       include "./include/sys/event.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_STATFS == ENABLED
#       include "./include/sys/statfs.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_SYSMACROS == ENABLED
#       include "./include/sys/sysmacros.h"
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_STATVFS == ENABLED
#       include "./include/sys/statvfs.h"
#   endif
#   if VSF_LINUX_APPLET_USE_NETDB == ENABLED
#       include "./include/netdb.h"
#   endif
#   if VSF_LINUX_APPLET_USE_LIBUSB == ENABLED
#       include "./include/libusb/libusb.h"
#   endif
#   if VSF_LINUX_SOCKET_USE_INET == ENABLED
#       if VSF_LINUX_APPLET_USE_IFADDRS == ENABLED
#           include "./include/ifaddrs.h"
#       endif
#       if VSF_LINUX_APPLET_USE_ARPA_INET == ENABLED
#           include "./include/arpa/inet.h"
#       endif
#       if VSF_LINUX_APPLET_USE_NET_IF == ENABLED
#           include "./include/net/if.h"
#       endif
#   endif
#   if VSF_LINUX_APPLET_USE_LIBGEN == ENABLED
#       include "./include/libgen.h"
#   endif
#   if VSF_LINUX_APPLET_USE_MNTENT == ENABLED
#       include "./include/mntent.h"
#   endif
#   if VSF_LINUX_APPLET_USE_FINDPROG == ENABLED
#       include "./include/findprog.h"
#   endif
#   if VSF_LINUX_APPLET_USE_GLOB == ENABLED
#       include "./include/glob.h"
#   endif
#   if VSF_LINUX_APPLET_USE_REGEX == ENABLED
#       include "./include/regex.h"
#   endif
#   if VSF_LINUX_APPLET_USE_LIBC_MATH == ENABLED
#       define __SIMPLE_LIBC_MATH_VPLT_ONLY__
#       include "./include/simple_libc/math/math.h"
#   endif
#   if VSF_LINUX_APPLET_USE_LIBC_WCHAR == ENABLED
#       include "./include/simple_libc/wchar.h"
#   endif
#else
#   if VSF_LINUX_APPLET_USE_PTHREAD == ENABLED
#       include <pthread.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_EPOLL == ENABLED
#       include <sys/epoll.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_EVENTFD == ENABLED
#       include <sys/eventfd.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_SENDFILE == ENABLED
#       include <sys/sendfile.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_FILE == ENABLED
#       include <sys/file.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_PRCTL == ENABLED
#       include <sys/prctl.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_EVENT == ENABLED
#       include <sys/event.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_STATFS == ENABLED
#       include <sys/statfs.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_SYSMACROS == ENABLED
#       include <sys/sysmacros.h>
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_STATVFS == ENABLED
#       include <sys/statvfs.h>
#   endif
#   if VSF_LINUX_APPLET_USE_NETDB == ENABLED
#       include <netdb.h>
#   endif
#   if VSF_LINUX_APPLET_USE_LIBUSB == ENABLED
#       include <libusb/libusb.h>
#   endif
#   if VSF_LINUX_SOCKET_USE_INET == ENABLED
#       if VSF_LINUX_APPLET_USE_IFADDRS == ENABLED
#           include <ifaddrs.h>
#       endif
#       if VSF_LINUX_APPLET_USE_ARPA_INET == ENABLED
#           include <arpa/inet.h>
#       endif
#       if VSF_LINUX_APPLET_USE_NET_IF == ENABLED
#           include <net/if.h>
#       endif
#   endif
#   if VSF_LINUX_APPLET_USE_LIBGEN == ENABLED
#       include <libgen.h>
#   endif
#   if VSF_LINUX_APPLET_USE_MNTENT == ENABLED
#       include <mntent.h>
#   endif
#   if VSF_LINUX_APPLET_USE_FINDPROG == ENABLED
#       include <findprog.h>
#   endif
#   if VSF_LINUX_APPLET_USE_GLOB == ENABLED
#       include <glob.h>
#   endif
#   if VSF_LINUX_APPLET_USE_REGEX == ENABLED
#       include <regex.h>
#   endif
#   if VSF_LINUX_APPLET_USE_LIBC_MATH == ENABLED
#       define __SIMPLE_LIBC_MATH_VPLT_ONLY__
#       include <math/math.h>
#   endif
#   if VSF_LINUX_APPLET_USE_LIBC_WCHAR == ENABLED
#       include <wchar.h>
#   endif
#endif

#if VSF_LINUX_APPLET_USE_SYS_REBOOT == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_reboot_vplt_t vsf_linux_sys_reboot_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_reboot_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(reboot),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_XATTR == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_xattr_vplt_t vsf_linux_sys_xattr_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_xattr_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(removexattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(lremovexattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(fremovexattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(setxattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(lsetxattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(fsetxattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(getxattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(lgetxattr),
    VSF_APPLET_VPLT_ENTRY_FUNC(fgetxattr),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_STATFS == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_statfs_vplt_t vsf_linux_sys_statfs_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_statfs_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(statfs),
    VSF_APPLET_VPLT_ENTRY_FUNC(fstatfs),
};
#endif

#if VSF_LINUX_APPLET_USE_SYS_SYSMACROS == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_sys_sysmacros_vplt_t vsf_linux_sys_sysmacros_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_sys_sysmacros_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(makedev),
    VSF_APPLET_VPLT_ENTRY_FUNC(major),
    VSF_APPLET_VPLT_ENTRY_FUNC(minor),
};
#endif

#if VSF_LINUX_APPLET_USE_FINDPROG == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_findprog_vplt_t vsf_linux_findprog_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_findprog_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(find_in_path),
    VSF_APPLET_VPLT_ENTRY_FUNC(find_in_given_path),
};
#endif

#if VSF_LINUX_APPLET_USE_GLOB == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_glob_vplt_t vsf_linux_glob_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_glob_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(glob),
    VSF_APPLET_VPLT_ENTRY_FUNC(globfree),
};
#endif

#if VSF_LINUX_APPLET_USE_REGEX == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_regex_vplt_t vsf_linux_regex_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_regex_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(regcomp),
    VSF_APPLET_VPLT_ENTRY_FUNC(regerror),
    VSF_APPLET_VPLT_ENTRY_FUNC(regexec),
    VSF_APPLET_VPLT_ENTRY_FUNC(regfree),
};
#endif

#if VSF_LINUX_APPLET_USE_FNMATCH == ENABLED && !defined(__VSF_APPLET__)
__VSF_VPLT_DECORATOR__ vsf_linux_fnmatch_vplt_t vsf_linux_fnmatch_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_fnmatch_vplt_t, 0, 0, true),

    VSF_APPLET_VPLT_ENTRY_FUNC(fnmatch),
};
#endif

#if VSF_APPLET_USE_KERNEL == ENABLED
extern __VSF_VPLT_DECORATOR__ vsf_kernel_vplt_t vsf_kernel_vplt;
#endif

__VSF_VPLT_DECORATOR__ vsf_linux_vplt_t vsf_linux_vplt = {
    VSF_APPLET_VPLT_INFO(vsf_linux_vplt_t, 0, 0, false),

    .applet_vplt        = (void *)&vsf_applet_vplt,
#if VSF_APPLET_USE_ARCH == ENABLED
    .arch_vplt          = (void *)&vsf_arch_vplt,
#endif
#if VSF_APPLET_USE_SERVICE == ENABLED
    .service_vplt       = (void *)&vsf_service_vplt,
#endif
#if VSF_APPLET_USE_ARCH_ABI == ENABLED
    .arch_abi_vplt      = (void *)&vsf_arch_abi_vplt,
#endif
    .dynamic_vplt       = (void *)&vsf_dynamic_vplt,
#if VSF_APPLET_USE_COMPILER == ENABLED
    .compiler_vplt      = (void *)&vsf_compiler_vplt,
#endif
#if VSF_APPLET_USE_KERNEL == ENABLED
    .kernel_vplt        = (void *)&vsf_kernel_vplt,
#endif

    .fundmental_vplt    = (void *)&vsf_linux_fundmental_vplt,

#if VSF_LINUX_APPLET_USE_LIBC_STDIO == ENABLED
    .libc_stdio_vplt    = (void *)&vsf_linux_libc_stdio_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBC_STDLIB == ENABLED
    .libc_stdlib_vplt   = (void *)&vsf_linux_libc_stdlib_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBC_STRING == ENABLED
    .libc_string_vplt   = (void *)&vsf_linux_libc_string_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBC_CTYPE == ENABLED
    .libc_ctype_vplt    = (void *)&vsf_linux_libc_ctype_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBC_TIME == ENABLED
    .libc_time_vplt     = (void *)&vsf_linux_libc_time_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBC_SETJMP == ENABLED
    .libc_setjmp_vplt   = (void *)&vsf_linux_libc_setjmp_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBC_ASSERT == ENABLED
    .libc_assert_vplt   = (void *)&vsf_linux_libc_assert_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBC_MATH == ENABLED
    .libc_math_vplt     = (void *)&vsf_linux_libc_math_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBC_WCHAR == ENABLED
    .libc_wchar_vplt    = (void *)&vsf_linux_libc_wchar_vplt,
#endif

#if VSF_LINUX_APPLET_USE_MNTENT == ENABLED
    .mntent_vplt        = (void *)&vsf_linux_mntent_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_EPOLL == ENABLED
    .sys_epoll_vplt     = (void *)&vsf_linux_sys_epoll_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_EVENTFD == ENABLED
    .sys_eventfd_vplt   = (void *)&vsf_linux_sys_eventfd_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_RANDOM == ENABLED
    .sys_random_vplt    = (void *)&vsf_linux_sys_random_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_SELECT == ENABLED
    .sys_select_vplt    = (void *)&vsf_linux_sys_select_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_SHM == ENABLED
    .sys_shm_vplt       = (void *)&vsf_linux_sys_shm_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_STAT == ENABLED
    .sys_stat_vplt      = (void *)&vsf_linux_sys_stat_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_MMAN == ENABLED
    .sys_mman_vplt      = (void *)&vsf_linux_sys_mman_vplt,
#endif
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
#   if VSF_LINUX_APPLET_USE_SYS_TIME == ENABLED
    .sys_time_vplt      = (void *)&vsf_linux_sys_time_vplt,
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_TIMES == ENABLED
    .sys_times_vplt     = (void *)&vsf_linux_sys_times_vplt,
#   endif
#   if VSF_LINUX_APPLET_USE_SYS_TIMEB == ENABLED
    .sys_timeb_vplt     = (void *)&vsf_linux_sys_timeb_vplt,
#   endif
#endif
#if VSF_LINUX_APPLET_USE_SYS_WAIT == ENABLED
    .sys_wait_vplt      = (void *)&vsf_linux_sys_wait_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_UTSNAME == ENABLED
    .sys_utsname_vplt   = (void *)&vsf_linux_sys_utsname_vplt,
#endif
#if VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_APPLET_USE_SYS_SOCKET == ENABLED
    .sys_socket_vplt    = (void *)&vsf_linux_sys_socket_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_SENDFILE == ENABLED
    .sys_sendfile_vplt  = (void *)&vsf_linux_sys_sendfile_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_REBOOT == ENABLED
    .sys_reboot_vplt    = (void *)&vsf_linux_sys_reboot_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_FILE == ENABLED
    .sys_file_vplt      = (void *)&vsf_linux_sys_file_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_PRCTL == ENABLED
    .sys_prctl_vplt     = (void *)&vsf_linux_sys_prctl_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_EVENT == ENABLED
    .sys_event_vplt     = (void *)&vsf_linux_sys_event_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_STATFS == ENABLED
    .sys_statfs_vplt    = (void *)&vsf_linux_sys_statfs_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_CAPABILITY == ENABLED
    .sys_capability_vplt    = (void *)&vsf_linux_sys_capability_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_TIMEX == ENABLED
    .sys_timex_vplt     = (void *)&vsf_linux_sys_timex_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_SYSMACROS == ENABLED
    .sys_sysmacros_vplt = (void *)&vsf_linux_sys_sysmacros_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_STATVFS == ENABLED
    .sys_statvfs_vplt   = (void *)&vsf_linux_sys_statvfs_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_RESOURCE == ENABLED
    .sys_resource_vplt  = (void *)&vsf_linux_sys_resource_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_INFO == ENABLED
    .sys_info_vplt      = (void *)&vsf_linux_sys_info_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_SEM == ENABLED
    .sys_sem_vplt       = (void *)&vsf_linux_sys_sem_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_IOCTL == ENABLED
    .sys_ioctl_vplt     = (void *)&vsf_linux_sys_ioctl_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SYS_XATTR == ENABLED
    .sys_xattr_vplt     = (void *)&vsf_linux_sys_xattr_vplt,
#endif

#if VSF_LINUX_APPLET_USE_UNISTD == ENABLED
    .unistd_vplt        = (void *)&vsf_linux_unistd_vplt,
#endif
#if VSF_LINUX_APPLET_USE_ERRNO == ENABLED
    .errno_vplt         = (void *)&vsf_linux_errno_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SIGNAL == ENABLED
    .signal_vplt        = (void *)&vsf_linux_signal_vplt,
#endif
#if VSF_LINUX_APPLET_USE_PTHREAD == ENABLED
    .pthread_vplt       = (void *)&vsf_linux_pthread_vplt,
#endif
#if VSF_LINUX_APPLET_USE_POLL == ENABLED
    .poll_vplt          = (void *)&vsf_linux_poll_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SEMAPHORE == ENABLED
    .semaphore_vplt     = (void *)&vsf_linux_semaphore_vplt,
#endif
#if VSF_LINUX_APPLET_USE_DIRENT == ENABLED
    .dirent_vplt        = (void *)&vsf_linux_dirent_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SPAWN == ENABLED
    .spawn_vplt         = (void *)&vsf_linux_spawn_vplt,
#endif
#if VSF_LINUX_APPLET_USE_TERMIOS == ENABLED
    .termios_vplt       = (void *)&vsf_linux_termios_vplt,
#endif
#if VSF_LINUX_APPLET_USE_FCNTL == ENABLED
    .fcntl_vplt         = (void *)&vsf_linux_fcntl_vplt,
#endif
#if VSF_LINUX_APPLET_USE_SCHED == ENABLED
    .sched_vplt         = (void *)&vsf_linux_sched_vplt,
#endif
#if VSF_LINUX_APPLET_USE_PTY == ENABLED
    .pty_vplt           = (void *)&vsf_linux_pty_vplt,
#endif
#if VSF_LINUX_SOCKET_USE_INET == ENABLED
#   if VSF_LINUX_APPLET_USE_IFADDRS == ENABLED
    .ifaddrs_vplt       = (void *)&vsf_linux_ifaddrs_vplt,
#   endif
#   if VSF_LINUX_APPLET_USE_ARPA_INET == ENABLED
    .arpa_inet_vplt     = (void *)&vsf_linux_arpa_inet_vplt,
#   endif
#   if VSF_LINUX_APPLET_USE_NET_IF == ENABLED
    .net_if_vplt        = (void *)&vsf_linux_net_if_vplt,
#   endif
#endif
#if VSF_LINUX_APPLET_USE_DLFCN == ENABLED
    .dlfcn_vplt         = (void *)&vsf_linux_dlfcn_vplt,
#endif
#if VSF_LINUX_APPLET_USE_GLOB == ENABLED
    .glob_vplt          = (void *)&vsf_linux_glob_vplt,
#endif
#if VSF_LINUX_APPLET_USE_REGEX == ENABLED
    .regex_vplt         = (void *)&vsf_linux_regex_vplt,
#endif
#if VSF_LINUX_SOCKET_USE_INET == ENABLED && VSF_LINUX_APPLET_USE_NETDB == ENABLED
    .netdb_vplt         = (void *)&vsf_linux_netdb_vplt,
#endif
#if VSF_LINUX_APPLET_USE_PWD == ENABLED
    .pwd_vplt           = (void *)&vsf_linux_pwd_vplt,
#endif
#if VSF_LINUX_APPLET_USE_FINDPROG == ENABLED
    .findprog_vplt      = (void *)&vsf_linux_findprog_vplt,
#endif
#if VSF_LINUX_APPLET_USE_FNMATCH == ENABLED
    .fnmatch_vplt       = (void *)&vsf_linux_fnmatch_vplt,
#endif
#if VSF_LINUX_APPLET_USE_GRP == ENABLED
    .grp_vplt           = (void *)&vsf_linux_grp_vplt,
#endif

#if VSF_LINUX_USE_LIBUSB == ENABLED && VSF_LINUX_APPLET_USE_LIBUSB == ENABLED
    .libusb_vplt        = (void *)&vsf_linux_libusb_vplt,
#endif
#if VSF_LINUX_APPLET_USE_LIBGEN == ENABLED
    .libgen_vplt        = (void *)&vsf_linux_libgen_vplt,
#endif
};
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#elif   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#endif      // VSF_USE_LINUX
