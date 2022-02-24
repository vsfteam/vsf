#include "../config.h"

#if VSF_USE_LINUX == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_IMPLEMENT
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../../include/unistd.h"
#   include "../../../include/errno.h"
#   include "../../../include/sys/wait.h"
#   include "../../../include/dirent.h"
#else
#   include <unistd.h>
#   include <errno.h>
#   include <sys/wait.h>
#   include <dirent.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_CTYPE == ENABLED
#   include "../../../include/simple_libc/ctype.h"
#else
#   include <ctype.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDLIB == ENABLED
#   include "../../../include/simple_libc/stdlib.h"
#else
#   include <stdlib.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
#   include "../../../include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STRING == ENABLED
#   include "../../../include/simple_libc/string.h"
#else
#   include <string.h>
#endif

#define VSH_PROMPT                  ">>>"

#ifndef PATH_MAX
#   define PATH_MAX                 256
#endif

#if VSH_HAS_COLOR
#   define VSH_COLOR_EXECUTABLE     "\033[1;32m"
#   define VSH_COLOR_DIRECTORY      "\033[1;34m"
#   define VSH_COLOR_NORMAL         "\033[1;37m"
#endif

typedef struct vsh_cmd_ctx_t {
    char cmd[VSH_CMD_SIZE];
    uint16_t pos;
    uint8_t escpos;

#if VSH_HISTORY_NUM > 0
    struct {
        char * entries[VSH_HISTORY_NUM];
        uint8_t entry_num;
        uint8_t cur_save_entry;
        uint8_t cur_disp_entry;
    } history;
#endif
} vsh_cmd_ctx_t;

typedef enum vsh_shell_state_t {
    SHELL_STATE_NORMAL,
    SHELL_STATE_ESC,
} vsh_shell_state_t;

#if VSF_LINUX_LIBC_USE_ENVIRON != ENABLED
static char *__vsh_path;
#endif

#if VSH_HISTORY_NUM > 0
static void __vsh_history_apply(vsh_cmd_ctx_t *ctx, uint8_t history_entry)
{
    while (ctx->pos > 0) {
        ctx->pos--;
        write(STDOUT_FILENO, "\b \b", 3);
    }

    strcpy(ctx->cmd, ctx->history.entries[history_entry]);
    ctx->pos = strlen(ctx->cmd);
    ctx->history.cur_disp_entry = history_entry;
    write(STDOUT_FILENO, ctx->cmd, ctx->pos);
}

static void __vsh_history_up(vsh_cmd_ctx_t *ctx)
{
    if (ctx->history.entry_num > 0) {
        uint8_t new_entry = (0 == ctx->history.cur_disp_entry) ?
                ctx->history.entry_num - 1 : ctx->history.cur_disp_entry - 1;
        __vsh_history_apply(ctx, new_entry);
    }
}

static void __vsh_history_down(vsh_cmd_ctx_t *ctx)
{
    if (ctx->history.entry_num > 0) {
        uint8_t new_entry = ((ctx->history.entry_num - 1) <= ctx->history.cur_disp_entry) ?
                0 : ctx->history.cur_disp_entry + 1;
        __vsh_history_apply(ctx, new_entry);
    }
}
#endif

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

static vsh_shell_state_t __vsh_process_escape(vsh_cmd_ctx_t *ctx)
{
    int esclen = ctx->escpos;
    char type = ctx->cmd[ctx->pos];
    char lastch = ctx->cmd[ctx->pos + esclen - 1];

    // esclen will be used for full support to escape strings
    UNUSED_PARAM(esclen);

    bool is_end = false;
    switch (type) {
    case '[':
        is_end =    ((lastch >= 'a') && (lastch <= 'z'))
                ||  ((lastch >= 'A') && (lastch <= 'Z'))
                ||  (lastch == '~');
        break;
    case 'O':
        is_end = esclen == 2;
        break;
    default:
        ctx->cmd[ctx->pos++] = type;
        goto return_to_normal;
    }
    if (!is_end) {
        return SHELL_STATE_ESC;
    }

    char *esc = &ctx->cmd[ctx->pos + 1];
    switch (type) {
    case '[':
        if (esclen == 2) {
#if VSH_HISTORY_NUM > 0
            switch (*esc) {
            case 'A':       __vsh_history_up(ctx);      break;
            case 'B':       __vsh_history_down(ctx);    break;
            }
#endif
        }
        break;
    }
return_to_normal:
    ctx->cmd[ctx->pos] = '\0';
    return SHELL_STATE_NORMAL;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

void vsh_set_path(char *path)
{
#if VSF_LINUX_LIBC_USE_ENVIRON != ENABLED
    __vsh_path = path;
#else
    setenv("PATH", path, true);
#endif
}

int __vsh_get_exe(char *pathname, int path_out_lenlen, char *cmd, vsf_linux_main_entry_t *entry)
{
    char pathname_local[PATH_MAX], pathname_dir[PATH_MAX], *path, *path_end;
    int exefd = 1, pathlen;

#if VSF_LINUX_LIBC_USE_ENVIRON != ENABLED
    path = __vsh_path;
#else
    path = getenv("PATH");
#endif
    VSF_LINUX_ASSERT(path != NULL);

    if (NULL == pathname) {
        pathname = pathname_local;
        path_out_lenlen = sizeof(pathname_local);
    }

    while (*path != '\0') {
        path_end = strchr(path, ':');
        pathlen = (path_end != NULL) ?  path_end - path : strlen(path);
        VSF_LINUX_ASSERT(pathlen < sizeof(pathname_dir) - 1);
        memcpy(pathname_dir, path, pathlen);
        pathname_dir[pathlen] = '\0';
        path += pathlen;
        if (*path == ':') {
            path++;
        }

        if (!vsf_linux_generate_path(pathname, path_out_lenlen, pathname_dir, cmd)) {
            exefd = vsf_linux_fs_get_executable(pathname, entry);
            if (exefd >= 0) {
                break;
            }
        }
    }
    return exefd;
}

static char * __vsh_get_next_arg(char *cmd)
{
    while ((*cmd != '\0') && !isspace((int)*cmd)) { cmd++; }
    while ((*cmd != '\0') && isspace((int)*cmd)) { *cmd++ = '\0'; }
    return cmd;
}

static vsf_linux_process_t * __vsh_prepare_process(char *cmd, int fd_in, int fd_out)
{
    char *next;
    int exefd = -1;
    vsf_linux_main_entry_t entry;
    char *env[2];

    // skip spaces
    while ((*cmd != '\0') && isspace((int)*cmd)) { cmd++; }
    if ('\0' == *cmd) { return 0; }
    next = &cmd[strlen(cmd) - 1];
    while (isspace((int)*next)) { *next-- = '\0'; }

    vsf_linux_process_t *process = vsf_linux_create_process(0);
    if (NULL == process) { return NULL; }

    env[1] = NULL;
    for (;;) {
        next = __vsh_get_next_arg(cmd);
        if (!strchr(cmd, '=')) {
            break;
        }

        env[0] = cmd;
        cmd = next;
        if (vsf_linux_merge_env(process, env) < 0) {
            goto delete_process_and_fail;
        }
    }

    // search in path first if not absolute path
    if (cmd[0] != '/') {
        exefd = __vsh_get_exe(NULL, 0, cmd, &entry);
    }

    if (exefd < 0) {
        exefd = vsf_linux_fs_get_executable(cmd, &entry);
        if (exefd < 0) {
            printf("%s not found" VSH_LINEEND, cmd);
            errno = ENOENT;
            goto delete_process_and_fail;
        }
    }
    close(exefd);

    vsf_linux_process_ctx_t *ctx = &process->ctx;
    char *strdupped = __strdup_ex(process, cmd);
    if (NULL == strdupped) {
        goto delete_process_and_fail;
    }
    ctx->entry = entry;
    ctx->arg.argv[ctx->arg.argc++] = strdupped;
    ctx->arg.is_dyn_argv = true;
    char *nextnext;
    while ((*next != '\0') && (ctx->arg.argc < dimof(ctx->arg.argv))) {
        nextnext = __vsh_get_next_arg(next);
        strdupped = __strdup_ex(process, next);
        if (NULL == strdupped) {
            goto delete_process_and_fail;
        }

        ctx->arg.argv[ctx->arg.argc++] = strdupped;
        next = nextnext;
    }

    extern int __vsf_linux_fd_create_ex(vsf_linux_process_t *process, vsf_linux_fd_t **sfd,
                const vsf_linux_fd_op_t *op, int fd_desired, vsf_linux_fd_priv_t *priv);
    extern vsf_linux_fd_t * vsf_linux_fd_get(int fd);
    vsf_linux_fd_t *sfd, *sfd_from;
    if (fd_in >= 0) {
        sfd_from = vsf_linux_fd_get(fd_in);
        VSF_LINUX_ASSERT(sfd_from != NULL);

        if (STDIN_FILENO != __vsf_linux_fd_create_ex(process, &sfd, sfd_from->op, STDIN_FILENO, sfd_from->priv)) {
            goto delete_process_and_fail;
        }
    }
    if (fd_out >= 0) {
        sfd_from = vsf_linux_fd_get(fd_out);
        VSF_LINUX_ASSERT(sfd_from != NULL);

        if (STDOUT_FILENO != __vsf_linux_fd_create_ex(process, &sfd, sfd_from->op, STDOUT_FILENO, sfd_from->priv)) {
            goto delete_process_and_fail;
        }
    }

    VSF_LINUX_ASSERT(ctx->entry != NULL);
    return process;
delete_process_and_fail:
    vsf_linux_delete_process(process);
    return NULL;
}

int __vsh_run_cmd(char *cmd)
{
    vsf_linux_process_t * processes[4];
    int process_cnt = 0, fd_in = -1;
    char *next;
    bool is_environ_expanded = false;

    // expand environ
    char *real_cmd = NULL;
    if (strchr(cmd, '$') != NULL) {
        int cmdlen = vsf_linux_expandenv(cmd, NULL, 0);
        if (cmdlen < 0) {
            printf("fail to parse command %s" VSH_LINEEND, cmd);
            return -1;
        }
        real_cmd = malloc(cmdlen + 1);
        if (NULL == real_cmd) {
            printf("fail to allocate buffer to parse command %s" VSH_LINEEND, cmd);
            return -1;
        }
        vsf_linux_expandenv(cmd, real_cmd, cmdlen + 1);
        is_environ_expanded = true;
        cmd = real_cmd;
    }

    // remove spaces
    next = &cmd[strlen(cmd) - 1];
    while (isspace((int)*next)) { *next-- = '\0'; }
    bool is_background = *next == '&';
    if (is_background) { *next = '\0'; }

    for (;;) {
        next = strchr(cmd, '|');
        if (NULL == next) {
            break;
        }
        *next++ = '\0';

        int pipefd[2];
        if (pipe(pipefd) < 0) {
            goto cleanup;
        }

        VSF_LINUX_ASSERT(process_cnt < dimof(processes));
        processes[process_cnt] = __vsh_prepare_process(cmd, fd_in, pipefd[1]);
        if (NULL == processes[process_cnt]) {
            close(pipefd[0]);
            close(pipefd[1]);
            goto cleanup;
        }

        close(pipefd[1]);
        if (fd_in >= 0) {
            close(fd_in);
        }
        fd_in = pipefd[0];
        process_cnt++;
        cmd = next;
    }
    VSF_LINUX_ASSERT(process_cnt < dimof(processes));
    processes[process_cnt] = __vsh_prepare_process(cmd, fd_in, -1);
    if (fd_in >= 0) {
        close(fd_in);
        fd_in = -1;
    }
    if (NULL == processes[process_cnt++]) {
        goto cleanup;
    }
    if (is_environ_expanded) {
        free(real_cmd);
    }

    if (is_background) {
        for (int i = 0; i < process_cnt; i++) {
            vsf_linux_detach_process(processes[i]);
        }
    }

    for (int i = 0; i < process_cnt; i++) {
        vsf_linux_start_process(processes[i]);
    }

    if (is_background) {
        // yield to new process, make sure main_on_run is called,
        //  and stdio fds are initialized, then return to vsh, call read will be OK
        vsf_thread_yield();
        return 0;
    } else {
        int result;
        for (int i = 0; i < process_cnt; i++) {
            waitpid(processes[i]->id.pid, &result, 0);
        }
        return result;
    }
cleanup:
    if (is_environ_expanded) {
        free(real_cmd);
    }
    if (fd_in >= 0) {
        close(fd_in);
    }
    for (int i = 0; i < process_cnt; i++) {
        if (processes[i] != NULL) {
            vsf_linux_delete_process(processes[i]);
        }
    }
    return -1;
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

int vsh_main(int argc, char *argv[])
{
    vsh_cmd_ctx_t ctx = { 0 };
    char ch;
    vsh_shell_state_t state;

    if ((argc > 2) && !strcmp(argv[1], "-c")) {
        if (strlen(argv[2]) >= VSH_CMD_SIZE) {
            return -ENOMEM;
        }

        strcpy(ctx.cmd, argv[2]);
        return __vsh_run_cmd(ctx.cmd);
    }

#if VSH_HAS_COLOR
    printf(VSH_COLOR_NORMAL);
#endif
    printf("VSF Linux" VSH_LINEEND "path: %s" VSH_LINEEND,
#if VSF_LINUX_LIBC_USE_ENVIRON != ENABLED
        __vsh_path
#else
        getenv("PATH")
#endif
    );

    while (1) {
    input_cmd:
        memset(ctx.cmd, 0, sizeof(ctx.cmd));
        ctx.pos = 0;

        state = SHELL_STATE_NORMAL;
        printf(VSH_PROMPT);
        fflush(stdout);
        while (1) {
            while (read(STDIN_FILENO, &ch, 1) != 1) {
                if (errno != EINTR) {
                    fprintf(stderr, "fail to read from stdin, is stdin disconnected?" VSH_LINEEND);
                    VSF_LINUX_ASSERT(false);
                    return -1;
                }
            }
            switch (ch) {
            case '\033':
                state = SHELL_STATE_ESC;
                ctx.escpos = 0;
                continue;
            default:
            input_char:
                if (state != SHELL_STATE_NORMAL) {
                    if (ctx.pos + ctx.escpos >= VSH_CMD_SIZE - 1) {
                    input_too_long:
                        // User should increase VSH_CMD_SIZE in config.h
                        VSF_LINUX_ASSERT(false);
                        return -ENOMEM;
                    }
                    ctx.cmd[ctx.pos + ctx.escpos++] = ch;
                    state = __vsh_process_escape(&ctx);
                    continue;
                }

                switch (ch) {
                case VSH_ENTER_CHAR:
                    if (vsf_linux_is_stdio_stream(STDIN_FILENO)) {
#if VSH_ENTER_CHAR == '\r'
                        write(STDOUT_FILENO, "\n", 1);
#endif
                    }
                    if (strlen(ctx.cmd) > 0) {
#if VSH_HISTORY_NUM > 0
                        if (ctx.history.entry_num < dimof(ctx.history.entries)) {
                            ctx.history.entry_num++;
                        }
                        if (ctx.history.entries[ctx.history.cur_save_entry] != NULL) {
                            free(ctx.history.entries[ctx.history.cur_save_entry]);
                        }
                        ctx.history.entries[ctx.history.cur_save_entry] = strdup(ctx.cmd);
                        if (++ctx.history.cur_save_entry >= dimof(ctx.history.entries)) {
                            ctx.history.cur_save_entry = 0;
                        }
                        ctx.history.cur_disp_entry = ctx.history.cur_save_entry;
#endif
                        if (__vsh_run_cmd(ctx.cmd) < 0) {
                            printf("fail to execute %s" VSH_LINEEND, ctx.cmd);
                        }
                    }
                    goto input_cmd;
#if     VSH_ENTER_CHAR == '\r'
                case '\n':
#elif   VSH_ENTER_CHAR == '\n'
                case '\r':
#else
#   error invalid VSH_ENTER_CHAR
#endif
                    continue;
                case '\b':
                case 0x7F:
                    if (ctx.pos > 0) {
                        ctx.cmd[--ctx.pos] = '\0';
                    }
                    break;
                default:
                    if (ctx.pos >= VSH_CMD_SIZE - 1) {
                        goto input_too_long;
                    }
                    ctx.cmd[ctx.pos++] = ch;
                    break;
                }
            }
        }
    }
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_warning=pe111
#endif

int pwd_main(int argc, char *argv[])
{
    char path[MAX_PATH];
    getcwd(path, sizeof(path));
    printf("%s" VSH_LINEEND, path);
    return 0;
}

int cd_main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("format: cd directory_name" VSH_LINEEND);
        return -1;
    }

    DIR *dir = opendir(argv[1]);
    if (NULL == dir) {
    cd_main_dir_not_exists:
        printf("%s not exists" VSH_LINEEND, argv[1]);
        return -ENOENT;
    }
    closedir(dir);

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    if (vsf_linux_chdir(process->shell_process, argv[1])) {
        goto cd_main_dir_not_exists;
    }
    return 0;
}

int ls_main(int argc, char *argv[])
{
    int dirnum, childnum;
    char **dirnames, *dirnames_dot = ".";

    if (1 == argc) {
        dirnum = 1;
        dirnames = &dirnames_dot;
    } else {
        dirnum = argc - 1;
        dirnames = &argv[1];
    }

    DIR *dir;
    struct dirent *ent;
    for (int i = 0; i < dirnum; i++) {
        if (dirnum > 1) {
            printf("%s:" VSH_LINEEND, dirnames[i]);
        }
        dir = opendir(dirnames[i]);
        if (NULL == dir) {
            printf("%s not exists" VSH_LINEEND, dirnames[i]);
            continue;
        }

        childnum = 0;
        do {
            ent = readdir(dir);
            if (ent != NULL) {
#if VSH_HAS_COLOR
                if (ent->d_type == DT_DIR) {
                    printf(VSH_COLOR_DIRECTORY);
                } else if (ent->d_type == DT_EXE) {
                    printf(VSH_COLOR_EXECUTABLE);
                } else {
                    printf(VSH_COLOR_NORMAL);
                }
#endif
                write(STDOUT_FILENO, ent->d_name, strlen(ent->d_name));
                printf("  ");
                fflush(stdout);
            }
            childnum++;
        } while (ent != NULL);
#if VSH_HAS_COLOR
        printf(VSH_COLOR_NORMAL);
#endif
        if (childnum > 0) {
            printf(VSH_LINEEND);
        }

        closedir(dir);
    }
    return 0;
}

int mkdir_main(int argc, char *argv[])
{
    if (argc < 2) {
    arg_fail:
        printf("format: mkdir [-p] directory_name" VSH_LINEEND);
        return -1;
    }

    if (argv[1][0] == '-') {
        if (argv[1][1] != 'p') {
            printf("unknown option: %s" VSH_LINEEND, argv[1]);
            return -1;
        }
        if (argc < 3) {
            goto arg_fail;
        }
        if (0 != mkdirs(argv[2], 0)) {
            printf("fail to create directory %s" VSH_LINEEND, argv[2]);
            return -1;
        }
    } else if (0 != mkdir(argv[1], 0)) {
        printf("fail to create directory %s" VSH_LINEEND, argv[1]);
        return -1;
    }
    return 0;
}

int echo_main(int argc, char *argv[])
{
    argv++;
    argc--;
    while (argc-- > 0) {
        printf("%s%s", *argv++, argc > 0 ? " " : "");
    }
    printf(VSH_LINEEND);
    return 0;
}

int cat_main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("format: cat file_name" VSH_LINEEND);
        return -1;
    }

    int fd = open(argv[1], 0);
    if (fd < 0) {
        printf("%s not found" VSH_LINEEND, argv[1]);
        return -ENOENT;
    }

    char buf[256];
    ssize_t size;
    while ((size = read(fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, size);
    }
    printf(VSH_LINEEND);
    close(fd);
    return 0;
}

int clear_main(int argc, char *argv[])
{
    printf("\033[2J\033[H");
    return 0;
}

#if !defined(VSF_ARCH_PROVIDE_HEAP) && VSF_HEAP_CFG_STATISTICS == ENABLED
int free_main(int argc, char *argv[])
{
    vsf_heap_statistics_t statistics;
    vsf_heap_statistics(&statistics);

    // 20 bytes is enough for 64-bit value
    char numbuf_total[20], numbuf_used[20], numbuf_free[20];
    itoa(statistics.all_size, numbuf_total, 10);
    itoa(statistics.used_size, numbuf_used, 10);
    itoa(statistics.all_size - statistics.used_size, numbuf_free, 10);

    printf("          total       used       free\nMem:%11s%11s%11s\n",
            numbuf_total, numbuf_used, numbuf_free);
    return 0;
}
#endif

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
int export_main(int argc, char *argv[])
{
    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    VSF_LINUX_ASSERT((process != NULL) && (process->shell_process != NULL));

    if (1 == argc) {
        char **env = process->__environ;
        if (NULL == env) {
            return 0;
        }

        while (*env != NULL) {
            printf("%s" VSH_LINEEND, *env);
            env++;
        }
        return 0;
    }

    char *env_str;
    if (strchr(argv[1], '=') == NULL) {
        env_str = __malloc_ex(process->shell_process, strlen(argv[1]) + 2);
        if (env_str != NULL) {
            sprintf(env_str, "%s=", argv[1]);
        }
    } else {
        env_str = __strdup_ex(process->shell_process, argv[1]);
    }
    if (NULL == env_str) {
        printf("fail to allocate environment string" VSH_LINEEND);
        return -1;
    }

    // env_str not belong to us after putenv
    extern int __putenv_ex(vsf_linux_process_t *process, char *string);
    int ret = __putenv_ex(process->shell_process, env_str);
    if (ret) {
        __free_ex(process->shell_process, env_str);
    }
    return ret;
}
#endif

#endif
