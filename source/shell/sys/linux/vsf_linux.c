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
#define __VSF_LINUX_FS_CLASS_IMPLEMENT
#define __VSF_LINUX_CLASS_IMPLEMENT

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./include/unistd.h"
#   include "./include/sched.h"
#   include "./include/semaphore.h"
#   include "./include/signal.h"
#   include "./include/sys/wait.h"
#   include "./include/sys/ipc.h"
#   include "./include/sys/shm.h"
#   include "./include/fcntl.h"
#   include "./include/errno.h"
#   include "./include/termios.h"
#   include "./include/pwd.h"
#   include "./include/sys/utsname.h"
#   include "./include/spawn.h"
#else
#   include <unistd.h>
#   include <sched.h>
#   include <semaphore.h>
#   include <signal.h>
#   include <sys/wait.h>
#   include <sys/ipc.h>
#   include <sys/shm.h>
#   include <fcntl.h>
#   include <errno.h>
#   include <termios.h>
#   include <pwd.h>
#   include <sys/utsname.h>
#   include <spawn.h>
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

#include "./kernel/fs/vsf_linux_fs.h"

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_ON_TERMINATE MUST be enbled
#endif

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL != ENABLED
#   error VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL MUST be enabled
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if VSF_LINUX_CFG_SHM_NUM > 0
dcl_vsf_bitmap(vsf_linux_shm_bitmap, VSF_LINUX_CFG_SHM_NUM);
typedef struct vsf_linux_shm_mem_t {
    key_t key;
    void *buffer;
    uint32_t size;
} vsf_linux_shm_mem_t;
#endif

typedef struct vsf_linux_t {
    int cur_tid;
    int cur_pid;
    vsf_dlist_t process_list;

    vsf_linux_process_t *kernel_process;
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    int sig_pid;
#endif

    vsf_linux_stdio_stream_t stdio_stream;

#if VSF_LINUX_CFG_SHM_NUM > 0
    struct {
        vsf_bitmap(vsf_linux_shm_bitmap) bitmap;
        vsf_linux_shm_mem_t mem[VSF_LINUX_CFG_SHM_NUM];
    } shm;
#endif
} vsf_linux_t;

/*============================ GLOBAL VARIABLES ==============================*/

int errno;

const struct passwd __vsf_default_passwd = {
    .pw_name            = "vsf",
    .pw_passwd          = "vsf",
    .pw_uid             = (uid_t)0,
    .pw_gid             = (gid_t)0,
    .pw_gecos           = "vsf",
    .pw_dir             = "/home",
    .pw_shell           = "vsh",
};

/*============================ PROTOTYPES ====================================*/

extern int vsf_linux_create_fhs(void);

#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
extern void vsf_linux_glibc_init(void);
#endif

static void __vsf_linux_main_on_run(vsf_thread_cb_t *cb);
static vsf_linux_process_t * __vsf_linux_start_process_internal(int stack_size,
        vsf_linux_main_entry_t entry, vsf_prio_t prio);
extern int __vsh_get_exe(char *pathname, int path_out_lenlen, char *cmd,
        vsf_linux_main_entry_t *entry);

/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsf_linux_t __vsf_linux;

static const vsf_linux_thread_op_t __vsf_linux_main_op = {
    .priv_size          = 0,
    .on_run             = __vsf_linux_main_on_run,
    .on_terminate       = vsf_linux_thread_on_terminate,
};

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_LINUX_CREATE_FHS
WEAK(vsf_linux_create_fhs)
int vsf_linux_create_fhs(void)
{
    return 0;
}
#endif

int vsf_linux_generate_path(char *path_out, int path_out_lenlen, char *dir, char *path_in)
{
    char working_dir[MAX_PATH];
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
        if (strlen(dir) + strlen(path_in) + 1 >= path_out_lenlen) {
            return -ENOMEM;
        }
        strcpy(path_out, dir);
        if (dir[strlen(dir) - 1] != '/') {
            strcat(path_out, "/");
        }
        strcat(path_out, path_in);
    }

    // process .. an .
    char *tmp, *tmp_replace;
    while ((tmp = (char *)strstr(path_out, "/..")) != NULL) {
        tmp[0] = '\0';
        tmp_replace = (char *)strrchr(path_out, '/');
        if (NULL == tmp_replace) {
            return -ENOENT;
        }
        strcpy(tmp_replace, &tmp[3]);
    }
    while ((tmp = (char *)strstr(path_out, "/./")) != NULL) {
        strcpy(tmp, &tmp[2]);
    }

    // fix surfix "/."
    size_t len = strlen(path_out);
    if ((len >= 2) && ('.' == path_out[len - 1]) && ('/' == path_out[len - 2])) {
        path_out[len - 2] = '\0';
    }
    return 0;
}

static int __vsf_linux_init_thread(int argc, char *argv[])
{
    int err = vsf_linux_create_fhs();
    if (err) { return err; }
    return execlp(VSF_LINUX_CFG_BIN_PATH "/init", "init", NULL);
}

static int __vsf_linux_kernel_thread(int argc, char *argv[])
{
#if VSF_KERNEL_CFG_TRACE == ENABLED
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vsf_kernel_trace_eda_info(&thread->use_as__vsf_eda_t, "linux_kernel_thread",
                                thread->stack, thread->stack_size);
#endif

    __vsf_linux.kernel_process = vsf_linux_get_cur_process();

#if VSF_LINUX_CFG_SUPPORT_SIG != ENABLED
    __vsf_linux_init_thread(argc, argv);
#else
    // create init process(pid1)
    __vsf_linux_start_process_internal(0, __vsf_linux_init_thread, VSF_LINUX_CFG_PRIO_HIGHEST);

    vsf_linux_sig_handler_t *handler;
    vsf_linux_process_t *process;
    vsf_evt_t evt;
    unsigned long sig_mask;
    int sig;
    bool found_handler;

    while (1) {
        evt = vsf_thread_wait();
        VSF_LINUX_ASSERT(VSF_EVT_MESSAGE == evt);

        process = vsf_eda_get_cur_msg();
        sig_mask = process->sig.pending.sig[0] & ~process->sig.mask.sig[0];
        while (sig_mask) {
            sig = vsf_ffz32(~sig_mask);
            sig_mask &= ~(1 << sig);

            found_handler = false;
            __vsf_dlist_foreach_unsafe(vsf_linux_sig_handler_t, node, &process->sig.handler_list) {
                if (_->sig == sig) {
                    handler = _;
                    found_handler = true;
                    break;
                }
            }

            __vsf_linux.sig_pid = process->id.pid;
            if (found_handler && (handler != SIG_DFL)) {
                if (handler != (vsf_linux_sig_handler_t *)SIG_IGN) {
                    siginfo_t siginfo = {
                        .si_signo   = sig,
                        .si_errno   = errno,
                    };
                    handler->handler(sig, &siginfo, NULL);
                }
            } else if (!((1 << sig) & ((1 << SIGURG) | (1 << SIGCONT) | (1 << SIGWINCH)))) {
                // TODO: terminate other thread is not supported in VSF, so just ignore
//                VSF_LINUX_ASSERT(false);
            }
        }
    }
#endif
    // actually will not return, just make compiler happy
    return 0;
}

vsf_err_t vsf_linux_init(vsf_linux_stdio_stream_t *stdio_stream)
{
    VSF_LINUX_ASSERT(stdio_stream != NULL);
    memset(&__vsf_linux, 0, sizeof(__vsf_linux));
    __vsf_linux.stdio_stream = *stdio_stream;
    vk_fs_init();

#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
    vsf_linux_glibc_init();
#endif

    // create kernel process(pid0)
    if (NULL != __vsf_linux_start_process_internal(0, __vsf_linux_kernel_thread, VSF_LINUX_CFG_PRIO_LOWEST)) {
        return VSF_ERR_NONE;
    }
    return VSF_ERR_FAIL;
}

int isatty(int fd)
{
    // terminal is __vsf_linux.stdio_stream
    vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
    VSF_LINUX_ASSERT(sfd != NULL);
    vsf_stream_t *stream = vsf_linux_get_stream(sfd);
    return  (stream == __vsf_linux.stdio_stream.in)
        ||  (stream == __vsf_linux.stdio_stream.out)
        ||  (stream == __vsf_linux.stdio_stream.err);
}

vsf_linux_thread_t * vsf_linux_create_thread(vsf_linux_process_t *process,
            const vsf_linux_thread_op_t *op,
            int stack_size, void *stack)
{
    vsf_linux_thread_t *thread;

    if (!process) {
        process = ((vsf_linux_thread_t *)vsf_eda_get_cur())->process;
    }
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

    thread = (vsf_linux_thread_t *)vsf_heap_malloc_aligned(all_size, alignment);
    if (thread != NULL) {
        memset(thread, 0, thread_size);
        thread->process = process;

        // set entry and on_terminate
        thread->on_terminate = (vsf_eda_on_terminate_t)op->on_terminate;
        thread->entry = (vsf_thread_entry_t *)op->on_run;

        // set stack
        thread->stack_size = stack_size;
        if (stack != NULL) {
            thread->stack = stack;
        } else {
            thread->stack = (void *)((uintptr_t)thread + thread_size);
        }

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

static vsf_linux_process_t * __vsf_linux_create_process(int stack_size)
{
    vsf_linux_process_t *process = vsf_heap_malloc(sizeof(vsf_linux_process_t));
    if (process != NULL) {
        memset(process, 0, sizeof(*process));
        process->prio = vsf_prio_inherit;
        process->shell_process = process;

#if VSF_LINUX_USE_TERMIOS == ENABLED
        static const struct termios __default_term = {
            .c_oflag        = OPOST | ONLCR,
            .c_lflag        = ECHO | ECHOE | ECHOK | ECHONL | ICANON,
            .c_cc[VMIN]     = 1,
            .c_cc[VERASE]   = 010,      // BS
            .c_cc[VWERASE]  = 027,      // ETB
            .c_cc[VKILL]    = 025,      // NAK
        };
        process->term[STDIN_FILENO] = __default_term;
        process->term[STDOUT_FILENO] = __default_term;
        process->term[STDERR_FILENO] = __default_term;
#endif

#if VSF_LINUX_USE_GETOPT == ENABLED
        process->__opterr = 1;
        process->__optind = 1;
        process->__optopt = '?';
#endif

        vsf_linux_thread_t *thread = vsf_linux_create_thread(process, &__vsf_linux_main_op, stack_size, NULL);
        if (NULL == thread) {
            vsf_heap_free(process);
            return NULL;
        }

        vsf_protect_t orig = vsf_protect_sched();
            process->id.pid = __vsf_linux.cur_pid++;
            vsf_dlist_add_to_tail(vsf_linux_process_t, process_node, &__vsf_linux.process_list, process);
        vsf_unprotect_sched(orig);
        if (process->id.pid) {
            process->id.ppid = getpid();
        }
    }
    return process;
}

vsf_linux_process_t * vsf_linux_create_process_ex(int stack_size, vsf_linux_stdio_stream_t *stdio_stream, char *working_dir)
{
    VSF_LINUX_ASSERT(stdio_stream != NULL);
    VSF_LINUX_ASSERT(working_dir != NULL);

    vsf_linux_process_t *process = __vsf_linux_create_process(stack_size);
    if (process != NULL) {
        process->working_dir = vsf_heap_malloc(strlen(working_dir) + 1);
        if (NULL == process->working_dir) {
            vsf_trace_warning("linux: fail to allocate working_dir, maybe error if working_dir is used", VSF_TRACE_CFG_LINEEND);
        } else {
            strcpy(process->working_dir, working_dir);
        }
        process->stdio_stream = *stdio_stream;
        VSF_LINUX_ASSERT(process->working_dir != NULL);

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
        vsf_linux_process_t *cur_process = vsf_linux_get_cur_process();
        VSF_LINUX_ASSERT(cur_process != NULL);

        int envnum = 0;
        char **environ_from = cur_process->__environ, **environ_to;
        if (environ_from != NULL) {
            while (*environ_from++ != NULL) {
                envnum++;
            }
        }
        if (envnum > 0) {
            process->__environ = malloc((envnum + 1) * sizeof(char *));
            if (NULL == process->__environ) {
            fail_env:
                vsf_trace_warning("linux: fail to allocate environ, maybe error if environ is used", VSF_TRACE_CFG_LINEEND);
            } else {
                environ_to = process->__environ;
                environ_from = cur_process->__environ;
                while (*environ_from != NULL) {
                    *environ_to = strdup(*environ_from);
                    if (NULL == *environ_to) {
                        goto fail_env;
                    }
                    environ_to++;
                    environ_from++;
                }
                *environ_to = NULL;
            }
        }
#endif
    }
    return process;
}

vsf_linux_process_t * vsf_linux_create_process(int stack_size)
{
    vsf_linux_process_t *parent_process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(parent_process != NULL);

    return vsf_linux_create_process_ex(stack_size, &parent_process->stdio_stream, parent_process->working_dir);
}

int vsf_linux_start_process(vsf_linux_process_t *process)
{
    // TODO: check if already started
    vsf_linux_thread_t *thread;
    vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
    return vsf_linux_start_thread(thread, vsf_prio_inherit);
}

static vsf_linux_process_t * __vsf_linux_start_process_internal(int stack_size,
        vsf_linux_main_entry_t entry, vsf_prio_t prio)
{
    VSF_LINUX_ASSERT((prio >= VSF_LINUX_CFG_PRIO_LOWEST) && (prio <= VSF_LINUX_CFG_PRIO_HIGHEST));
    vsf_linux_process_t *process = __vsf_linux_create_process(stack_size);
    if (process != NULL) {
        process->prio = prio;
        process->ctx.entry = entry;
        process->working_dir = vsf_heap_malloc(2);
        if (NULL == process->working_dir) {
            vsf_trace_warning("linux: fail to allocate working_dir, maybe error if working_dir is used", VSF_TRACE_CFG_LINEEND);
        } else {
            strcpy(process->working_dir, "/");
        }
        process->stdio_stream = __vsf_linux.stdio_stream;
        VSF_LINUX_ASSERT(process->working_dir != NULL);
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

vsf_linux_thread_t * vsf_linux_get_thread(int tid)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
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

vsf_linux_thread_t * vsf_linux_get_cur_thread(void)
{
    return (vsf_linux_thread_t *)vsf_eda_get_cur();
}

vsf_linux_process_t * vsf_linux_get_cur_process(void)
{
    vsf_linux_thread_t *thread = vsf_linux_get_cur_thread();
    vsf_linux_process_t *process = thread->process;
    return process->id.pid != (pid_t)0 || NULL == __vsf_linux.kernel_process ?
                process : __vsf_linux.kernel_process;
}

void vsf_linux_set_dominant_process(void)
{
    vsf_linux_fd_t *sfd;
    vsf_stream_t *stream;

    sfd = vsf_linux_fd_get(0);
    if (sfd != NULL) {
        stream = vsf_linux_get_stream(sfd);
        if (stream != NULL) {
            stream->rx.param = sfd;
        }
    }

    sfd = vsf_linux_fd_get(1);
    if (sfd != NULL) {
        stream = vsf_linux_get_stream(sfd);
        if (stream != NULL) {
            stream->tx.param = sfd;
        }
    }

    sfd = vsf_linux_fd_get(2);
    if (sfd != NULL) {
        stream = vsf_linux_get_stream(sfd);
        if (stream != NULL) {
            stream->tx.param = sfd;
        }
    }
}

static void __vsf_linux_main_on_run(vsf_thread_cb_t *cb)
{
    vsf_linux_thread_t *thread = container_of(cb, vsf_linux_thread_t, use_as__vsf_thread_cb_t);
    vsf_linux_process_t *process = thread->process;
    vsf_linux_process_ctx_t *ctx = &process->ctx;
    vsf_linux_fd_t *sfd;

    sfd = vsf_linux_fd_get(0);
    if (NULL == sfd) {
        sfd = vsf_linux_rx_stream(thread->process->stdio_stream.in);
        sfd->status_flags = O_RDONLY;
    }

    sfd = vsf_linux_fd_get(1);
    if (NULL == sfd) {
        sfd = vsf_linux_tx_stream(thread->process->stdio_stream.out);
        sfd->status_flags = O_WRONLY;
    }

    sfd = vsf_linux_fd_get(2);
    if (NULL == sfd) {
        sfd = vsf_linux_tx_stream(thread->process->stdio_stream.err);
        sfd->status_flags = O_WRONLY;
    }

    VSF_LINUX_ASSERT(ctx->entry != NULL);
    thread->retval = ctx->entry(ctx->arg.argc, (char **)ctx->arg.argv);

    // clean up
    do {
        vsf_dlist_peek_head(vsf_linux_fd_t, fd_node, &process->fd_list, sfd);
        if (sfd != NULL) {
            close(sfd->fd);
        }
    } while (sfd != NULL);
}

void vsf_linux_thread_on_terminate(vsf_linux_thread_t *thread)
{
    if (thread->thread_pending != NULL) {
        thread->thread_pending->retval = thread->retval;
        vsf_eda_post_evt(&thread->thread_pending->use_as__vsf_eda_t, VSF_EVT_USER);
    }

    vsf_linux_process_t *process = thread->process;
    bool is_to_free_process;
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_remove(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
        is_to_free_process = vsf_dlist_is_empty(&process->thread_list);
    vsf_unprotect_sched(orig);
    vsf_heap_free(thread);

    if (is_to_free_process) {
        vsf_protect_t orig = vsf_protect_sched();
            vsf_dlist_remove(vsf_linux_process_t, process_node, &__vsf_linux.process_list, process);
        vsf_unprotect_sched(orig);

        if (process->thread_pending != NULL) {
            process->thread_pending->retval = thread->retval;
            vsf_eda_post_evt(&process->thread_pending->use_as__vsf_eda_t, VSF_EVT_USER);
        }
        if (process->working_dir != NULL) {
            vsf_heap_free(process->working_dir);
        }
#if     VSF_LINUX_USE_SIMPLE_LIBC == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED\
    &&  VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_CHECK == ENABLED
        if(process->heap_usage != 0) {
            vsf_trace_warning("memory leak %d bytes detected in process 0x%p, balance = %d" VSF_TRACE_CFG_LINEEND,
                        process->heap_usage, process, process->heap_balance);
        }
#endif
#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
        char **environ = process->__environ;
        if (environ != NULL) {
            while (*environ != NULL) {
                free(*environ);
                environ++;
            }
            free(process->__environ);
        }
#endif
        vsf_heap_free(process);
    }
}

exec_ret_t execvp(const char *pathname, char const* const* argv)
{
    // fd will be closed after entry return
    vsf_linux_main_entry_t entry;
    int fd = vsf_linux_fs_get_executable(pathname, &entry);
    if (fd < 0) {
        return -1;
    }

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    vsf_linux_process_ctx_t *ctx = &process->ctx;
    vsf_linux_thread_t *thread;

    ctx->arg.argc = 0;
    while ((*argv != NULL) && (ctx->arg.argc <= VSF_LINUX_CFG_MAX_ARG_NUM)) {
        ctx->arg.argv[ctx->arg.argc++] = *argv++;
    }
    ctx->entry = entry;

    vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
    vsf_eda_post_evt(&thread->use_as__vsf_eda_t, VSF_EVT_INIT);
    vsf_thread_wfe(VSF_EVT_INVALID);
    return 0;
}

exec_ret_t execv(const char *pathname, char const* const* argv)
{
    char fullpath[MAX_PATH];
    int fd = __vsh_get_exe(fullpath, sizeof(fullpath), (char *)pathname, NULL);
    if (fd < 0) {
        return -1;
    }
    close(fd);

    return execvp(fullpath, argv);
}

static exec_ret_t __execlp_va(const char *pathname, const char *arg, va_list ap)
{
    // fd will be closed after entry return
    vsf_linux_main_entry_t entry;
    int fd = vsf_linux_fs_get_executable(pathname, &entry);
    if (fd < 0) {
        return -1;
    }

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    vsf_linux_process_ctx_t *ctx = &process->ctx;
    vsf_linux_thread_t *thread;
    const char *args;

    ctx->arg.argc = 1;
    ctx->arg.argv[0] = arg;
    args = va_arg(ap, const char *);
    while ((args != NULL) && (ctx->arg.argc <= VSF_LINUX_CFG_MAX_ARG_NUM)) {
        ctx->arg.argv[ctx->arg.argc++] = args;
        args = va_arg(ap, const char *);
    }
    ctx->entry = entry;

    vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &process->thread_list, thread);
    vsf_eda_post_evt(&thread->use_as__vsf_eda_t, VSF_EVT_INIT);
    vsf_thread_wfe(VSF_EVT_INVALID);
    return 0;
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

exec_ret_t execl(const char *pathname, const char *arg, ...)
{
    char fullpath[MAX_PATH];
    int fd = __vsh_get_exe(fullpath, sizeof(fullpath), (char *)pathname, NULL);
    if (fd < 0) {
        return -1;
    }
    close(fd);

    exec_ret_t ret;

    va_list ap;
    va_start(ap, arg);
        ret = __execlp_va(pathname, arg, ap);
    va_end(ap);
    return ret;
}

long sysconf(int name)
{
    switch (name) {
    case _SC_PAGESIZE:      return 256;
    }
    return 0;
}

char *realpath(const char *path, char *resolved_path)
{
    return NULL;
}

#if __IS_COMPILER_IAR__
//! transfer of control bypasses initialization
#   pragma diag_suppress=pe546
#endif

int pipe(int pipefd[2])
{
    vsf_linux_fd_t *sfd_rx = NULL, *sfd_tx = NULL;

    sfd_rx = vsf_linux_rx_pipe(NULL);
    if (NULL == sfd_rx) {
        return -1;
    }

    vsf_linux_pipe_rx_priv_t *pipe_priv = (vsf_linux_pipe_rx_priv_t *)sfd_rx->priv;
    vsf_queue_stream_t *queue_stream = (vsf_queue_stream_t *)pipe_priv->stream;
    sfd_tx = vsf_linux_tx_pipe(queue_stream);
    if (NULL == sfd_tx) {
        close(sfd_rx->fd);
        return -1;
    }

    pipefd[0] = sfd_rx->fd;
    pipefd[1] = sfd_tx->fd;
    return 0;
}

int kill(pid_t pid, int sig)
{
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    vsf_linux_process_t *process = vsf_linux_get_process(pid);
    if (process != NULL) {
        if (!process->id.pid) {
            return -1;
        }

        vsf_protect_t orig = vsf_protect_sched();
            process->sig.pending.sig[0] |= 1 << sig;
        vsf_unprotect_sched(orig);

        vsf_linux_thread_t *thread;
        vsf_dlist_peek_head(vsf_linux_thread_t, thread_node, &__vsf_linux.kernel_process->thread_list, thread);
        // TODO: avoid posting event/message to thread,
        //  if the thread is not waiting for the dedicated event/message
        vsf_eda_post_msg(&thread->use_as__vsf_eda_t, process);
        return 0;
    }
#endif
    return -1;
}

#if !defined(__WIN__) || VSF_LINUX_CFG_WRAPPER == ENABLED
// conflicts with signal in ucrt, need VSF_LINUX_CFG_WRAPPER
sighandler_t signal(int signum, sighandler_t handler)
{
#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
    // not supported yet
#else
    return NULL;
#endif
}
#endif

unsigned int alarm(unsigned int seconds)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

pid_t waitpid(pid_t pid, int *status, int options)
{
    if (pid <= 0) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    if (options != 0) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }

    vsf_linux_process_t *process = vsf_linux_get_process(pid);
    if (NULL == process) {
        return -1;
    }

    vsf_linux_thread_t *cur_thread = vsf_linux_get_cur_thread();
    process->thread_pending = cur_thread;
    vsf_thread_wfe(VSF_EVT_USER);
    if (status != NULL) {
        *status = cur_thread->retval << 8;
    }
    return pid;
}

pid_t getpid(void)
{
    return vsf_linux_get_cur_process()->id.pid;
}

pid_t getppid(void)
{
    return vsf_linux_get_cur_process()->id.ppid;
}

pid_t getpgid(pid_t pid)
{
    return pid;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();

    if (oldset != NULL) {
        *oldset = process->sig.mask;
    }

    vsf_protect_t orig = vsf_protect_sched();
        switch (how) {
        case SIG_BLOCK:     sigaddsetmask(&process->sig.mask, set->sig[0]); break;
        case SIG_UNBLOCK:   sigdelsetmask(&process->sig.mask, set->sig[0]); break;
        case SIG_SETMASK:   process->sig.mask = *set;                       break;
        }
    vsf_unprotect_sched(orig);
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
    strcpy(buffer, process->working_dir);
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

char * getpass(const char *prompt)
{
    fprintf(stdout, "%s", prompt);

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
void usleep(int usec)
{
#   if VSF_KERNEL_CFG_TIMER_MODE == VSF_KERNEL_CFG_TIMER_MODE_TICKLESS
    vsf_teda_set_timer_us(usec);
#   else
    // us sleep is not available in non tickless mode
    VSF_LINUX_ASSERT(false);
#   endif
    vsf_thread_wfe(VSF_EVT_TIMER);
}

// TODO: wakeup after signal
unsigned sleep(unsigned sec)
{
    vsf_teda_set_timer_ms(sec * 1000);
    vsf_thread_wfe(VSF_EVT_TIMER);
    return 0;
}
#endif

// malloc.h
void * memalign(size_t alignment, size_t size)
{
    return vsf_heap_malloc_aligned(size, alignment);
}

// ipc.h
key_t ftok(const char *pathname, int id)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

// sys/time.h
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
    return 0;
}
#endif

#if VSF_LINUX_CFG_SHM_NUM > 0
// shm.h
int shmget(key_t key, size_t size, int shmflg)
{
    VSF_LINUX_ASSERT((IPC_PRIVATE == key) || (shmflg & IPC_CREAT));

    vsf_protect_t orig = vsf_protect_sched();
        key = vsf_bitmap_ffz(&__vsf_linux.shm.bitmap, VSF_LINUX_CFG_SHM_NUM);
        if (key >= 0) {
            vsf_bitmap_set(&__vsf_linux.shm.bitmap, key);
        }
    vsf_unprotect_sched(orig);

    if (key < 0) {
        return key;
    }

    vsf_linux_shm_mem_t *mem = &__vsf_linux.shm.mem[key++];
    mem->size = size;
    mem->key = key;
    mem->buffer = malloc(size);
    if (NULL == mem->buffer) {
        shmctl(key, IPC_RMID, NULL);
        return -1;
    }

    return key;
}

void * shmat(int shmid, const void *shmaddr, int shmflg)
{
    vsf_linux_shm_mem_t *mem = &__vsf_linux.shm.mem[shmid];
    return mem->buffer;
}

int shmdt(const void *shmaddr)
{
    return 0;
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
    shmid--;
    VSF_LINUX_ASSERT(shmid < VSF_LINUX_CFG_SHM_NUM);

    vsf_linux_shm_mem_t *mem = &__vsf_linux.shm.mem[shmid];
    switch (cmd) {
    case IPC_STAT:
        memset(buf, 0, sizeof(*buf));
        buf->shm_segsz = mem->size;
        buf->shm_perm.key = mem->key;
        break;
    case IPC_SET:
        VSF_LINUX_ASSERT(false);
        break;
    case IPC_RMID: {
            if (mem->buffer != NULL) {
                free(mem->buffer);
                mem->buffer = NULL;
            }

            vsf_protect_t orig = vsf_protect_sched();
                vsf_bitmap_clear(&__vsf_linux.shm.bitmap, shmid);
            vsf_unprotect_sched(orig);
        }
        break;
    }
    return 0;
}
#endif      // VSF_LINUX_CFG_SHM_NUM

// sched
int sched_get_priority_max(int policy)
{
    return VSF_LINUX_CFG_PRIO_HIGHEST;
}

int sched_get_priority_min(int policy)
{
    return VSF_LINUX_CFG_PRIO_LOWEST;
}

int sched_yield(void)
{
    vsf_thread_yield();
    return 0;
}

int uname(struct utsname *name)
{
    static const struct utsname __name = {
        .sysname        = "vsf",
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
    return -1;
}

int sethostname(const char *name, size_t len)
{
    return -1;
}

// spawn.h
int posix_spawnp(pid_t *pid, const char *file,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[])
{
    extern int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
                const vsf_linux_fd_op_t *op, int fd_desired, bool allocate_priv);
    extern vsf_linux_fd_t * __vsf_linux_fd_get_ex(vsf_linux_process_t *process, int fd);
    extern void __vsf_linux_fd_delete_ex(vsf_linux_process_t *process, int fd);
    extern vk_file_t * __vsf_linux_get_fs_ex(vsf_linux_process_t *process, int fd);

    // fd will be closed after entry return
    vsf_linux_main_entry_t entry;
    int fd = vsf_linux_fs_get_executable(file, &entry);
    if (fd < 0) {
        if (pid != NULL) {
            *pid = -1;
        }
        return -1;
    }
    VSF_LINUX_ASSERT(entry != NULL);

    vsf_linux_process_t *process = vsf_linux_create_process(0);
    if (NULL == process) { return -ENOMEM; }
    vsf_linux_process_ctx_t *ctx = &process->ctx;
    process->shell_process = process;
    ctx->entry = entry;
    VSF_LINUX_ASSERT(argv != NULL);
    while ((*argv != NULL) && (ctx->arg.argc <= VSF_LINUX_CFG_MAX_ARG_NUM)) {
        ctx->arg.argv[ctx->arg.argc++] = *argv++;
    }

    // dup fds
    vsf_linux_process_t *cur_process = vsf_linux_get_cur_process();
    vsf_linux_fd_t *sfd, *sfd_new;
    vsf_protect_t orig;
    __vsf_dlist_foreach_unsafe(vsf_linux_fd_t, fd_node, &cur_process->fd_list) {
        if (!(_->fd_flags & FD_CLOEXEC)) {
            if (__vsf_linux_fd_create_ex(process, &sfd, _->op, _->fd, false) == _->fd) {
                sfd->priv = _->priv;

                orig = vsf_protect_sched();
                    ((vsf_linux_fd_priv_t *)_->priv)->ref++;
                vsf_unprotect_sched(orig);
            }
        }
    }

    // env
#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
    if (env != NULL) {
        extern int __putenv(char *string, char ***environ);
        while (*env != NULL) {
            __putenv(*env++, &process->__environ);
        }
    }
#endif

    // apply actions
    if (actions != NULL) {
        char fullpath[MAX_PATH];
        struct spawn_action *a = actions->actions;
        for (int i = 0; i < actions->used; i++, a++) {
            switch (a->tag) {
            case spawn_do_close:
                sfd = __vsf_linux_fd_get_ex(process, a->action.close_action.fd);
                if (sfd != NULL) {
                    orig = vsf_protect_sched();
                        ((vsf_linux_fd_priv_t *)sfd->priv)->ref--;
                    vsf_unprotect_sched(orig);
                    __vsf_linux_fd_delete_ex(process, sfd->fd);
                }
                break;
            case spawn_do_dup2:
                sfd = __vsf_linux_fd_get_ex(process, a->action.dup2_action.fd);
                if (sfd != NULL) {
                    int ret = __vsf_linux_fd_create_ex(process, &sfd_new, sfd->op, a->action.dup2_action.newfd, false);
                    if (!ret) {
                        vsf_linux_fd_priv_t *priv = sfd->priv;
                        orig = vsf_protect_sched();
                            priv->ref++;
                        vsf_unprotect_sched(orig);
                        sfd_new->priv = priv;
                    }
                }
                break;
            case spawn_do_open:
                VSF_LINUX_ASSERT(false);
                break;
            case spawn_do_fchdir: {
                    vk_file_t *file = __vsf_linux_get_fs_ex(process, a->action.fchdir_action.fd);
                    if (NULL == file) {
                        continue;
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
                        continue;
                    }
                    a->action.chdir_action.path = ptr;
                }
                // fall through
            case spawn_do_chdir:
                vsf_linux_chdir(process, a->action.chdir_action.path);
                break;
            }
        }
    }

    if (pid != NULL) {
        *pid = process->id.pid;
    }
    return vsf_linux_start_process(process);
}

int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[])
{
    char fullpath[MAX_PATH];
    int fd = __vsh_get_exe(fullpath, sizeof(fullpath), (char *)path, NULL);
    if (fd < 0) {
        if (pid != NULL) {
            *pid = -1;
        }
        return -1;
    }
    close(fd);

    return posix_spawnp(pid, fullpath, actions, attr, argv, env);
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
#if VSF_LINUX_USE_TERMIOS == ENABLED
int tcgetattr(int fd, struct termios *termios)
{
    if ((fd >= 3) || (NULL == termios)) {
        return -1;
    }

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    *termios = process->term[fd];
    return 0;
}

int tcsetattr(int fd, int optional_actions, const struct termios *termios)
{
    if ((fd >= 3) || (NULL == termios)) {
        return -1;
    }

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT(process != NULL);
    process->term[fd] = *termios;
    return 0;
}

int tcsendbreak(int fd, int duration)
{
    return 0;
}

int tcdrain(int fd)
{
    return 0;
}

int tcflush(int fd, int queue_selector)
{
    return 0;
}

int tcflow(int fd, int action)
{
    return 0;
}

void cfmakeraw(struct termios *termios_p)
{
}

speed_t cfgetispeed(const struct termios *termios_p)
{
    return 0;
}

speed_t cfgetospeed(const struct termios *termios_p)
{
    return 0;
}

int cfsetispeed(struct termios *termios_p, speed_t speed)
{
    return 0;
}

int cfsetospeed(struct termios *termios_p, speed_t speed)
{
    return 0;
}

int cfsetspeed(struct termios *termios_p, speed_t speed)
{
    return 0;
}
#endif      // VSF_LINUX_USE_TERMIOS

int vsf_linux_expandenv(const char *str, char *output, size_t bufflen)
{
    size_t envlen = 0, copylen, tmplen, namelen;
    char ch, *env_start, *env_end, *name_start, *name_end, *env_value;

#define __copy2output(__str_from, __copylen)                                    \
        do {                                                                    \
            if (output != NULL) {                                               \
                tmplen = min(bufflen, __copylen);                               \
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
        if (name_end <= name_start) {
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

#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif

#endif      // VSF_USE_LINUX
