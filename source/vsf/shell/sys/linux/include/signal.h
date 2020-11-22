#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#else
#   include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define kill                __vsf_linux_kill
#define sigprocmask         __vsf_linux_sigprocmask

#define sigemptyset         __vsf_linux_sigemptyset
#define sigfillset          __vsf_linux_sigfillset
#define sigaddsetmask       __vsf_linux_sigaddsetmask
#define sigdelsetmask       __vsf_linux_sigdelsetmask
#define sigtestsetmask      __vsf_linux_sigtestsetmask

#define _NSIG           32

typedef struct {
    unsigned long sig[_NSIG / (sizeof(unsigned long) << 3)];
} sigset_t;

typedef struct {
    int si_signo;
    int si_errno;
    int si_code;
} siginfo_t;

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
#define SIGWINCH        28  //                      ignore
#define SIGPOLL         20  //                      terminate
#define SIGIO           29  //                      terminate
#define SIGPWR          30  //                      terminate
#define SIGSYS          31  //                      coredump

#define SIG_BLOCK       0
#define SIG_UNBLOCK     1
#define SIG_SETMASK     2

#define SIG_DFL         0
#define SIG_IGN         -1

static inline void sigemptyset(sigset_t *set)
{
    set->sig[0] = 0;
}

static inline void sigfillset(sigset_t *set)
{
    set->sig[0] = -1;
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

#ifdef __cplusplus
}
#endif

#endif
