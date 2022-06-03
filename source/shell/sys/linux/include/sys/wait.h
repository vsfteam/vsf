#ifndef __VSF_LINUX_WAIT_H__
#define __VSF_LINUX_WAIT_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/types.h"
#   include "../signal.h"
#else
#   include <sys/types.h>
#   include <signal.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if VSF_LINUX_CFG_WRAPPER == ENABLED
#define waitpid                 VSF_LINUX_WRAPPER(waitpid)
#endif

// options of waitpid
#define WEXITED                 1
#define WSTOPPED                2
#define WCONTINUED              3
#define WNOHANG                 4
#define WNOWAIT                 5
#define WUNTRACED               6

// internal usage
#define PID_STATUS_RUNNING      (1 << 0)
#define PID_STATUS_DADMON       (1 << 1)

#define WIFEXITED(__STATUS)     !((__STATUS) & 0xFF)
#define WEXITSTATUS(__STATUS)   ((__STATUS) >> 8)
// TODO: implement WIFSIGNALED
#define WIFSIGNALED(__STATUS)   0
#define WTERMSIG(__STATUS)      ((__STATUS) & 0x7F)

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

#ifdef __cplusplus
}
#endif

#endif
