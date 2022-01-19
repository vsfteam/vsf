#include "../config.h"

#if VSF_USE_LINUX == ENABLED

#define __VSF_LINUX_CLASS_INHERIT__
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

    // refer to https://en.wikipedia.org/wiki/ANSI_escape_code
    SHELL_STATE_SS2,
    SHELL_STATE_SS3,
    SHELL_STATE_DCS,
    SHELL_STATE_CSI,
    SHELL_STATE_ST,
    SHELL_STATE_OSC,
    SHELL_STATE_SOS,
    SHELL_STATE_PM,
    SHELL_STATE_APC,
    SHELL_STATE_RIS,
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
        uint8_t new_entry = ((ctx->history.entry_num - 1) == ctx->history.cur_disp_entry) ?
                0 : ctx->history.cur_disp_entry + 1;
        __vsh_history_apply(ctx, new_entry);
    }
}
#endif

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

static vsh_shell_state_t __vsh_process_escape(vsh_cmd_ctx_t *ctx, vsh_shell_state_t type)
{
    char esc = ctx->cmd[ctx->pos];
    int esclen = ctx->escpos;

    switch (type) {
    case SHELL_STATE_CSI:
        VSF_LINUX_ASSERT(1 == esclen);
        switch (esc) {
        case 'A':       // up
#if VSH_HISTORY_NUM > 0
            __vsh_history_up(ctx);
            goto exit_csi;
#endif
        case 'B':       // down
#if VSH_HISTORY_NUM > 0
            __vsh_history_down(ctx);
            goto exit_csi;
#endif
        case 'C':       // right
        case 'D':       // left
        default:
        exit_csi:
            ctx->cmd[ctx->pos] = '\0';
            return SHELL_STATE_NORMAL;
        }
        return type;
    default:
        VSF_LINUX_ASSERT(false);
        return SHELL_STATE_NORMAL;
    }
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

int __vsh_run_cmd(char *cmd)
{
    char *cur;
    int exefd = -1, err;
    vsf_linux_main_entry_t entry;

    while ((*cmd != '\0') && isspace((int)*cmd)) { cmd++; }
    if ('\0' == *cmd) { return 0; }

    cur = &cmd[strlen(cmd) - 1];
    while (isspace((int)*cur)) { *cur-- = '\0'; }
    bool is_background = *cur == '&';
    if (is_background) { *cur = '\0'; }

    cur = cmd;
    while ((*cur != '\0') && !isspace((int)*cur)) { cur++; }
    while ((*cur != '\0') && isspace((int)*cur)) { *cur++ = '\0'; }

    // search in path first if not absolute path
    if (cmd[0] != '/') {
        exefd = __vsh_get_exe(NULL, 0, cmd, &entry);
    }

    if (exefd < 0) {
        exefd = vsf_linux_fs_get_executable(cmd, &entry);
        if (exefd < 0) {
            printf("%s not found" VSH_LINEEND, cmd);
            err = -ENOENT;
            return err;
        }
    }

    vsf_linux_process_t *process = vsf_linux_create_process(0);
    if (NULL == process) { return -ENOMEM; }

    vsf_linux_process_ctx_t *ctx = &process->ctx;
    vsf_linux_process_t *cur_process = vsf_linux_get_cur_process();
    process->shell_process = cur_process->shell_process;
    ctx->entry = entry;
    ctx->arg.argv[ctx->arg.argc++] = cmd;
    while ((*cur != '\0') && (ctx->arg.argc < dimof(ctx->arg.argv))) {
        ctx->arg.argv[ctx->arg.argc++] = cur;
        while ((*cur != '\0') && !isspace((int)*cur)) { cur++; }
        while ((*cur != '\0') && isspace((int)*cur)) { *cur++ = '\0'; }
    }

    VSF_LINUX_ASSERT(ctx->entry != NULL);
    vsf_linux_start_process(process);

    if (is_background) {
        // yield to run new process, and then set current process dominant
        vsf_thread_yield();
        vsf_linux_set_dominant_process();
        return 0;
    } else {
        int result;
        waitpid(process->id.pid, &result, 0);
        close(exefd);
        vsf_linux_set_dominant_process();
        return result;
    }
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


#if VSH_HAS_COLOR
    printf(VSH_COLOR_NORMAL);
#endif
    printf("path: %s" VSH_LINEEND,
#if VSF_LINUX_LIBC_USE_ENVIRON != ENABLED
        __vsh_path
#else
        getenv("PATH")
#endif
    );

    if ((argc > 2) && !strcmp(argv[1], "-c")) {
        if (strlen(argv[2]) >= VSH_CMD_SIZE) {
            return -ENOMEM;
        }

        strcpy(ctx.cmd, argv[2]);
        return __vsh_run_cmd(ctx.cmd);
    }

    while (1) {
    input_cmd:
        memset(ctx.cmd, 0, sizeof(ctx.cmd));
        ctx.pos = 0;

        state = SHELL_STATE_NORMAL;
        printf(VSH_PROMPT);
        fflush(stdout);
        while (1) {
            read(STDIN_FILENO, &ch, 1);
            switch (ch) {
            case '\033':        // ESC
                state = SHELL_STATE_ESC;
                ctx.escpos = 0;
                continue;
            case 'N':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_SS2; continue; }    goto input_char;
            case 'O':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_SS3; continue; }    goto input_char;
            case 'P':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_DCS; continue; }    goto input_char;
            case '[':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_CSI; continue; }    goto input_char;
            case '\\':  if (SHELL_STATE_ESC == state) { state = SHELL_STATE_ST;  continue; }    goto input_char;
            case ']':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_OSC; continue; }    goto input_char;
            case 'X':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_SOS; continue; }    goto input_char;
            case '^':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_PM;  continue; }    goto input_char;
            case '_':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_APC; continue; }    goto input_char;
            case 'c':   if (SHELL_STATE_ESC == state) { state = SHELL_STATE_RIS; continue; }    goto input_char;
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
                    state = __vsh_process_escape(&ctx, state);
                    continue;
                }

                switch (ch) {
                case VSH_ENTER_CHAR:
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
    char **dirnames;

    if (1 == argc) {
        argv[0] = ".";
        dirnum = 1;
        dirnames = &argv[0];
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
                write(STDOUT_FILENO, ent->d_name, ent->d_reclen);
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
    if (argc != 2) {
        printf("format: mkdir directory_name" VSH_LINEEND);
        return -1;
    }

    if (0 != mkdir(argv[1], 0)) {
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

#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
int export_main(int argc, char *argv[])
{
    if (1 == argc) {
        extern char **environ;
        char **env = environ;
        if (NULL == env) {
            return 0;
        }

        while (*env != NULL) {
            printf("%s\n", *env);
            env++;
        }
    }

    return 0;
}
#endif

#endif
