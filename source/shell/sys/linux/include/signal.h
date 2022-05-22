#ifndef __VSF_LINUX_SIGNAL_H__
#define __VSF_LINUX_SIGNAL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define kill                VSF_LINUX_WRAPPER(kill)
#define signal              VSF_LINUX_WRAPPER(signal)
#define sigaction           VSF_LINUX_WRAPPER(sigaction)
#define sigprocmask         VSF_LINUX_WRAPPER(sigprocmask)
#define raise               VSF_LINUX_WRAPPER(raise)

#define sigemptyset         VSF_LINUX_WRAPPER(sigemptyset)
#define sigfillset          VSF_LINUX_WRAPPER(sigfillset)
#define sigaddsetmask       VSF_LINUX_WRAPPER(sigaddsetmask)
#define sigdelsetmask       VSF_LINUX_WRAPPER(sigdelsetmask)
#define sigtestsetmask      VSF_LINUX_WRAPPER(sigtestsetmask)
#define pthread_sigmask     VSF_LINUX_WRAPPER(pthread_sigmask)
#endif

#if VSF_LINUX_CFG_SUPPORT_SIG == ENABLED
#else
#define sigjmp_buf          jmp_buf
#define siglongjmp(__j, __s)longjmp((__j), (__s))
#define sigsetjmp(__j, __s) setjmp(__j)
#endif

#define _NSIG           32

typedef void (*sighandler_t)(int);
typedef int sig_atomic_t;

typedef struct {
    unsigned long sig[_NSIG / (sizeof(unsigned long) << 3)];
} sigset_t;

typedef struct {
    int si_signo;
    int si_code;
    pid_t si_pid;
    uid_t si_uid;
    int si_errno;
    int si_status;
} siginfo_t;

struct sigaction {
    union {
        sighandler_t sa_handler;
        void (*sa_sigaction)(int, siginfo_t *, void *);
    };
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};

#define SIGHUP          1   // hang up              terminate
#define SIGINT          2   // interrupt            terminate
#define SIGQUIT         3   // quit                 coredump
#define SIGILL          4   // illeagle             coredump
#define SIGTRAP         5   // trap                 coredump
#define SIGABRT         6   // abort                coredump
#define SIGIOT          6   // IO trap              coredump
#define SIGBUS          7   // bus error            coredump
#define SIGFPE          8   // float point error    coredump
#define SIGKILL         9   // kill                 terminate(unmaskable)
#define SIGUSR1         10  // usr1                 terminate
#define SIGSEGV         11  // segment fault        coredump
#define SIGUSR2         12  // usr2                 terminate
#define SIGPIPE         13  // pipe error           terminate
#define SIGALRM         14  // alarm                terminate
#define SIGTERM         15  // terminate            terminate
#define SIGSTKFLT       16  // stack fault          terminate
#define SIGCHLD         17  // child                ignore
#define SIGCONT         18  // continue             ignore
#define SIGSTOP         19  // stop                 stop(unmaskable)
#define SIGTSTP         20  // tty stop             stop
#define SIGTTIN         21  // tty in               stop
#define SIGTTOU         22  // tty out              stop
#define SIGURG          23  //                      ignore
#define SIGXCPU         24  //                      coredump
#define SIGXFSZ         25  //                      coredump
#define SIGVTALRM       26  //                      terminate
#define SIGPROF         27  //                      terminate
// some applications will check SIGWINCH, undefine it to mark unsupported
//#define SIGWINCH        28  //                      ignore
#define SIGPOLL         20  //                      terminate
#define SIGIO           29  //                      terminate
#define SIGPWR          30  //                      terminate
#define SIGSYS          31  //                      coredump
#define NSIG            32

#define SIG_BLOCK       0
#define SIG_UNBLOCK     1
#define SIG_SETMASK     2

#define SIG_DFL         (sighandler_t)0
#define SIG_IGN         (sighandler_t)1
#define SIG_ERR         (sighandler_t)-1

// sa_flags
#define SA_NOCLDSTOP    1
#define SA_NOCLDWAIT    2
#define SA_SIGINFO      4
#define SA_RESTART      0x10000000
#define SA_NODEFER      0x40000000

static inline int sigemptyset(sigset_t *set)
{
    set->sig[0] = 0;
    return 0;
}

static inline int sigfillset(sigset_t *set)
{
    set->sig[0] = -1;
    return 0;
}

static inline int sigaddset(sigset_t *set, int signo)
{
    set->sig[0] |= 1 << signo;
    return 0;
}

static inline int sigdelset(sigset_t *set, int signo)
{
    set->sig[0] &= ~(1 << signo);
    return 0;
}

static inline void sigaddsetmask(sigset_t *set, unsigned long mask)
{
    set->sig[0] |= mask;
}

static inline void sigdelsetmask(sigset_t *set, unsigned long mask)
{
    set->sig[0] &= ~mask;
}

static inline int sigtestsetmask(sigset_t *set, unsigned long mask)
{
    return (set->sig[0] & mask) != 0;
}

int kill(pid_t pid, int sig);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
sighandler_t signal(int signum, sighandler_t handler);
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int raise(int sig);

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset);

#ifdef __cplusplus
}
#endif

#endif
