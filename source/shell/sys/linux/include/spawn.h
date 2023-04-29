#ifndef __VSF_LINUX_SPAWN_H__
#define __VSF_LINUX_SPAWN_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "./sys/types.h"
#   include "./signal.h"
#   include "./sched.h"
#else
#   include <sys/types.h>
#   include <signal.h>
#   include <sched.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    short int flags;
} posix_spawnattr_t;

struct spawn_action {
    enum {
        spawn_do_close,
        spawn_do_dup2,
        spawn_do_open,
        spawn_do_chdir,
        spawn_do_fchdir,
    } tag;
    union {
        struct {
            int fd;
        } close_action;
        struct {
            int fd;
            int newfd;
        } dup2_action;
        struct {
            int fd;
            char *path;
            int oflag;
            mode_t mode;
        } open_action;
        struct {
            char *path;
        } chdir_action;
        struct {
            int fd;
        } fchdir_action;
    } action;
};

typedef struct {
    int allocated;
    int used;
    struct spawn_action *actions;
} posix_spawn_file_actions_t;

#if VSF_LINUX_APPLET_USE_SPAWN == ENABLED
typedef struct vsf_linux_spawn_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawn);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnp);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_getsigdefault);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_setsigdefault);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_getsigmask);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_setsigmask);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_getflags);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_setflags);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_getpgroup);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_setpgroup);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_getschedpolicy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_setschedpolicy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_getschedparam);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawnattr_setschedparam);

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawn_file_actions_init);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawn_file_actions_destroy);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawn_file_actions_addopen);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawn_file_actions_addclose);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawn_file_actions_adddup2);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawn_file_actions_addchdir_np);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(posix_spawn_file_actions_addfchdir_np);
} vsf_linux_spawn_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_linux_spawn_vplt_t vsf_linux_spawn_vplt;
#   endif
#endif

#if defined(__VSF_APPLET__) && defined(__VSF_APPLET_LIB__) && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_LINUX_APPLET_USE_SPAWN == ENABLED

#ifndef VSF_LINUX_APPLET_SPAWN_VPLT
#   if VSF_LINUX_USE_APPLET == ENABLED
#       define VSF_LINUX_APPLET_SPAWN_VPLT                                      \
            ((vsf_linux_spawn_vplt_t *)(VSF_LINUX_APPLET_VPLT->spawn_vplt))
#   else
#       define VSF_LINUX_APPLET_SPAWN_VPLT                                      \
            ((vsf_linux_spawn_vplt_t *)vsf_vplt((void *)0))
#   endif
#endif

#define VSF_LINUX_APPLET_SPAWN_ENTRY(__NAME)                                    \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_LINUX_APPLET_SPAWN_VPLT, __NAME)
#define VSF_LINUX_APPLET_SPAWN_IMP(...)                                         \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_LINUX_APPLET_SPAWN_VPLT, __VA_ARGS__)

VSF_LINUX_APPLET_SPAWN_IMP(posix_spawn, int, pid_t *pid, const char *path, const posix_spawn_file_actions_t *actions, const posix_spawnattr_t *attr, char * const argv[], char * const env[]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawn)(pid, path, actions, attr, argv, env);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnp, int, pid_t *pid, const char *file, const posix_spawn_file_actions_t *actions, const posix_spawnattr_t *attr, char * const argv[], char * const env[]) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnp)(pid, file, actions, attr, argv, env);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_init, int, posix_spawnattr_t *attr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_init)(attr);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_destroy, int, posix_spawnattr_t *attr) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_destroy)(attr);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_getsigdefault, int, const posix_spawnattr_t *attr, sigset_t *sigdefault) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_getsigdefault)(attr, sigdefault);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_setsigdefault, int, posix_spawnattr_t *attr, const sigset_t *sigdefault) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_setsigdefault)(attr, sigdefault);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_getsigmask, int, const posix_spawnattr_t *attr, sigset_t *sigmask) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_getsigmask)(attr, sigmask);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_setsigmask, int, posix_spawnattr_t *attr, const sigset_t *sigmask) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_setsigmask)(attr, sigmask);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_getflags, int, const posix_spawnattr_t *attr, short int *flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_getflags)(attr, flags);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_setflags, int, posix_spawnattr_t *attr, short int flags) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_setflags)(attr, flags);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_getpgroup, int, const posix_spawnattr_t *attr, pid_t *pgroup) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_getpgroup)(attr, pgroup);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_setpgroup, int, posix_spawnattr_t *attr, pid_t pgroup) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_setpgroup)(attr, pgroup);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_getschedpolicy, int, const posix_spawnattr_t *attr, int *schedpolicy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_getschedpolicy)(attr, schedpolicy);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_setschedpolicy, int, posix_spawnattr_t *attr, int schedpolicy) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_setschedpolicy)(attr, schedpolicy);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_getschedparam, int, const posix_spawnattr_t *attr, struct sched_param *schedparam) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_getschedparam)(attr, schedparam);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawnattr_setschedparam, int, posix_spawnattr_t *attr, const struct sched_param *schedparam) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawnattr_setschedparam)(attr, schedparam);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawn_file_actions_init, int, posix_spawn_file_actions_t *actions) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawn_file_actions_init)(actions);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawn_file_actions_destroy, int, posix_spawn_file_actions_t *actions) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawn_file_actions_destroy)(actions);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawn_file_actions_addopen, int, posix_spawn_file_actions_t *actions, int fd, const char *path, int oflag, mode_t mode) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawn_file_actions_addopen)(actions, fd, path, oflag, mode);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawn_file_actions_addclose, int, posix_spawn_file_actions_t *actions, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawn_file_actions_addclose)(actions, fd);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawn_file_actions_adddup2, int, posix_spawn_file_actions_t *actions, int fd, int newfd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawn_file_actions_adddup2)(actions, fd, newfd);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawn_file_actions_addchdir_np, int, posix_spawn_file_actions_t *actions, const char *path) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawn_file_actions_addchdir_np)(actions, path);
}
VSF_LINUX_APPLET_SPAWN_IMP(posix_spawn_file_actions_addfchdir_np, int, posix_spawn_file_actions_t *actions, int fd) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_LINUX_APPLET_SPAWN_ENTRY(posix_spawn_file_actions_addfchdir_np)(actions, fd);
}

#else

int posix_spawn(pid_t *pid, const char *path,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[]);
int posix_spawnp(pid_t *pid, const char *file,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[]);

int posix_spawnattr_init(posix_spawnattr_t *attr);
int posix_spawnattr_destroy(posix_spawnattr_t *attr);
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *attr, sigset_t *sigdefault);
int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr, const sigset_t *sigdefault);
int posix_spawnattr_getsigmask(const posix_spawnattr_t *attr, sigset_t *sigmask);
int posix_spawnattr_setsigmask(posix_spawnattr_t *attr, const sigset_t *sigmask);
int posix_spawnattr_getflags(const posix_spawnattr_t *attr, short int *flags);
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short int flags);
int posix_spawnattr_getpgroup(const posix_spawnattr_t *attr, pid_t *pgroup);
int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup);
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *attr, int *schedpolicy);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int schedpolicy);
int posix_spawnattr_getschedparam(const posix_spawnattr_t *attr, struct sched_param *schedparam);
int posix_spawnattr_setschedparam(posix_spawnattr_t *attr, const struct sched_param *schedparam);

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *actions);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *actions);
int posix_spawn_file_actions_addopen(
                posix_spawn_file_actions_t *actions,
                int fd, const char *path,
                int oflag, mode_t mode);
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *actions, int fd);
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *actions, int fd, int newfd);
int posix_spawn_file_actions_addchdir_np(posix_spawn_file_actions_t *actions, const char *path);
int posix_spawn_file_actions_addfchdir_np(posix_spawn_file_actions_t *actions, int fd);

#endif

#ifdef __cplusplus
}
#endif

#endif
