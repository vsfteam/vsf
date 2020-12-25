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

#if VSH_HISTORY_NUM && !VSH_ECHO
#   error VSH_ECHO must be enabled for VSH_HISTORY_NUM
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

// TODO: working dir should not be maintained in vsh
static char __vsh_working_dir[PATH_MAX];
static char **__vsh_path;

char * getcwd(char * buffer, size_t maxlen)
{
    int len = strlen(__vsh_working_dir) + 1;

    if ((NULL == buffer) && !maxlen) {
        buffer = malloc(len);
    }
    if (len > maxlen) {
        // TODO: set errno to ERANGE
        return NULL;
    }
    strcpy(buffer, __vsh_working_dir);
    return buffer;
}

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

void vsh_set_path(char **path)
{
    __vsh_path = path;
}

int vsh_generate_path(char *path, int pathlen, char *dir, char *path_in)
{
    if (path_in[0] == '/') {
        if (strlen(path_in) >= pathlen) {
            return -ENOMEM;
        }
        strcpy(path, path_in);
    } else {
        if (strlen(dir) + strlen(path_in) >= pathlen) {
            return -ENOMEM;
        }
        strcpy(path, dir);
        strcat(path, path_in);
    }

    // process .. an .
    char *tmp, *tmp_replace;
    while ((tmp = (char *)strstr(path, "/..")) != NULL) {
        tmp[0] = '\0';
        tmp_replace = (char *)strrchr(path, '/');
        if (NULL == tmp_replace) {
            return -ENOENT;
        }
        strcpy(tmp_replace, &tmp[3]);
    }
    while ((tmp = (char *)strstr(path, "/./")) != NULL) {
        strcpy(tmp, &tmp[2]);
    }

    // fix surfix "/."
    size_t len = strlen(path);
    if ((len >= 2) && ('.' == path[len - 1]) && ('/' == path[len - 2])) {
        path[len - 1] = '\0';
    }
    return 0;
}

static int __vsh_run_cmd(vsh_cmd_ctx_t *cmd_ctx)
{
    vsf_linux_process_t *process = vsf_linux_create_process(0);
    if (NULL == process) { return -ENOMEM; }

    vsf_linux_process_ctx_t *ctx = &process->ctx;
    char *cur = cmd_ctx->cmd;
    do {
        ctx->arg.argv[ctx->arg.argc++] = cur;
        while ((*cur != '\0') && !isspace(*cur)) { cur++; }
        while ((*cur != '\0') && isspace(*cur)) { *cur++ = '\0'; }
    } while (*cur != '\0');

    char pathname[PATH_MAX];
    int exefd = -1, err;
    char **path = __vsh_path;

    // search in path first if not absolute path
    if (ctx->arg.argv[0][0] != '/') {
        while (*path != NULL) {
            if (!vsh_generate_path(pathname, sizeof(pathname), *path, (char *)ctx->arg.argv[0])) {
                exefd = vsf_linux_fs_get_executable(pathname, &ctx->entry);
                if (exefd >= 0) {
                    break;
                }
            }
            path++;
        }
    }

    // search in working_dir if not found in path
    if (exefd < 0) {
        err = vsh_generate_path(pathname, sizeof(pathname), __vsh_working_dir, (char *)ctx->arg.argv[0]);
        if (err != 0) { goto fail; }
        exefd = vsf_linux_fs_get_executable(pathname, &ctx->entry);
        if (exefd < 0) {
            printf("%s not found" VSH_LINEEND, ctx->arg.argv[0]);
            err = -ENOENT;
        fail:
            free(process);
            return err;
        }
    }

    VSF_LINUX_ASSERT(ctx->entry != NULL);
    vsf_linux_start_process(process);
    waitpid(process->id.pid, NULL, 0);
    close(exefd);
    return 0;
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
    printf("path: %s\r\n", *__vsh_path);

    strcpy(__vsh_working_dir, "/");
    if ((argc > 2) && !strcmp(argv[1], "-c")) {
        if (strlen(argv[2]) >= VSH_CMD_SIZE) {
            return -ENOMEM;
        }

        strcpy(ctx.cmd, argv[2]);
        return __vsh_run_cmd(&ctx);
    }

    while (1) {
    input_cmd:
        memset(ctx.cmd, 0, sizeof(ctx.cmd));
        ctx.pos = 0;

        state = SHELL_STATE_NORMAL;
        printf(VSH_PROMPT);
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
#if VSH_ECHO
                    printf(VSH_LINEEND);
#endif
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
                        if (__vsh_run_cmd(&ctx) < 0) {
                            printf("fail to execute %s\r\n", ctx.cmd);
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
#if VSH_ECHO
                        write(STDOUT_FILENO, "\b \b", 3);
#endif
                    }
                    break;
                default:
                    if (ctx.pos >= VSH_CMD_SIZE - 1) {
                        goto input_too_long;
                    }
                    ctx.cmd[ctx.pos++] = ch;
#if VSH_ECHO
                    write(STDOUT_FILENO, &ch, 1);
#endif
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
    printf("%s" VSH_LINEEND, __vsh_working_dir);
    return 0;
}

int cd_main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("format: cd directory_name" VSH_LINEEND);
        return -1;
    }

    if (strlen(__vsh_working_dir) + strlen(argv[1]) + 1 >= PATH_MAX) {
        return -ENOMEM;
    }

    char pathname[PATH_MAX];
    int err = vsh_generate_path(pathname, sizeof(pathname), __vsh_working_dir, argv[1]);
    if (err != 0) {
        printf("invalid path %s" VSH_LINEEND, argv[1]);
        return err;
    }

    if (pathname[strlen(pathname) - 1] != '/') {
        strcat(pathname, "/");
    }

    DIR *dir = opendir(pathname);
    if (NULL == dir) {
        printf("%s not exists" VSH_LINEEND, argv[1]);
        goto return_no_directory;
    }
    closedir(dir);
    strcpy(__vsh_working_dir, pathname);
    return 0;
return_no_directory:
    printf("current path is %s" VSH_LINEEND, __vsh_working_dir);
    return -ENOENT;
}

int ls_main(int argc, char *argv[])
{
    int dirnum, childnum;
    char **dirnames;

    if (1 == argc) {
        argv[0] = "";
        dirnum = 1;
        dirnames = &argv[0];
    } else {
        dirnum = argc - 1;
        dirnames = &argv[1];
    }

    DIR *dir;
    struct dirent *ent;
    char dirname[PATH_MAX];
    int err;
    for (int i = 0; i < dirnum; i++) {
        err = vsh_generate_path(dirname, sizeof(dirname), __vsh_working_dir, dirnames[i]);
        if (err != 0) { return err; }

        if (dirnum > 1) {
            printf("%s:" VSH_LINEEND, dirnames[i]);
        }
        dir = opendir(dirname);
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

    char pathname[PATH_MAX];
    int err = vsh_generate_path(pathname, sizeof(pathname), __vsh_working_dir, argv[1]);
    if (err != 0) {
        printf("invalid directory %s" VSH_LINEEND, argv[1]);
        return -1;
    }

    if (0 != mkdir(pathname, 0)) {
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

    char pathname[PATH_MAX];
    int err = vsh_generate_path(pathname, sizeof(pathname), __vsh_working_dir, argv[1]);
    if (err != 0) { return err; }

    int fd = open(pathname, 0);
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

#endif
