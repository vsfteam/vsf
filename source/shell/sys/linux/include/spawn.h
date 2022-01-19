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

#ifdef __cplusplus
}
#endif

#endif
