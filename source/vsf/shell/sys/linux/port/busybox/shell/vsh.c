#include "../config.h"

#define VSF_LINUX_INHERIT
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <dirent.h>

#define VSH_PROMPT              ">>>"

#ifndef VSH_CMD_SIZE
#   define VSH_CMD_SIZE         128
#endif

#if VSH_HAS_COLOR
#define VSH_COLOR_EXECUTABLE    "\033[1;32m"
#define VSH_COLOR_DIRECTORY     "\033[1;34m"
#define VSH_COLOR_NORMAL        "\033[1;37m"
#endif

struct vsh_cmd_ctx_t {
    char cmd[VSH_CMD_SIZE];
    int pos, escpos;
};

enum vsh_shell_state_t {
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
};

static char vsh_working_dir[MAX_PATH];
static char **vsh_path;

static enum vsh_shell_state_t
vsh_process_escape(struct vsh_cmd_ctx_t *ctx, enum vsh_shell_state_t type)
{
    char *esc = &ctx->cmd[ctx->pos];
    int esclen = ctx->escpos;

    switch (type) {
    case SHELL_STATE_CSI:
        // TODO: parse csi
        VSF_LINUX_ASSERT(false);
        return type;
    default:
        VSF_LINUX_ASSERT(false);
    }
}

void vsh_set_path(char **path)
{
    vsh_path = path;
}

static int vsh_generate_path(char *path, int len, char *dir, char *name)
{
    if (name[0] == '/') {
        if (strlen(name) >= len) {
            return -ENOMEM;
        }
        strcpy(path, name);
    } else {
        if (strlen(dir) + strlen(name) >= len) {
            return -ENOMEM;
        }
        strcpy(path, dir);
        strcat(path, name);
    }
    return 0;
}

static int vsh_run_cmd(struct vsh_cmd_ctx_t *cmd_ctx)
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

    char pathname[MAX_PATH];
    int err = vsh_generate_path(pathname, sizeof(pathname), vsh_working_dir, (char *)ctx->arg.argv[0]);
    if (err != 0) { goto fail; }

    int exefd = vsf_linux_fs_get_executable(pathname, &ctx->entry);
    if (exefd < 0) {
        if (*ctx->arg.argv[0] != '/') {
            char **path = vsh_path;
            while (*path != NULL) {
                err = vsh_generate_path(pathname, sizeof(pathname), *path, (char *)ctx->arg.argv[0]);
                if (err != 0) { goto fail; }
                exefd = vsf_linux_fs_get_executable(pathname, &ctx->entry);
                if (exefd < 0) {
                    break;
                }
                path++;
            }
        }

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

int vsh_main(int argc, char *argv[])
{
    struct vsh_cmd_ctx_t *ctx;
    int ret;
    char ch;
    enum vsh_shell_state_t state;

#if VSH_HAS_COLOR
    printf(VSH_COLOR_NORMAL);
#endif
    strcpy(vsh_working_dir, "/");
    if ((argc > 2) && !strcmp(argv[1], "-c")) {
        if (strlen(argv[2]) >= VSH_CMD_SIZE) {
            return -ENOMEM;
        }

        ctx = calloc(1, sizeof(struct vsh_cmd_ctx_t));
        if (NULL == ctx) { return -ENOMEM; }

        strcpy(ctx->cmd, argv[2]);
        ret = vsh_run_cmd(ctx);
        free(ctx);
        return ret;
    }

    while (1) {
        ctx = malloc(sizeof(struct vsh_cmd_ctx_t));
        if (NULL == ctx) { return -ENOMEM; }

    input_cmd:
        memset(ctx, 0, sizeof(*ctx));
        state = SHELL_STATE_NORMAL;
        printf(VSH_PROMPT);
        while (1) {
            read(STDIN_FILENO, &ch, 1);
            switch (ch) {
            case '\033':        // ESC
                state = SHELL_STATE_ESC;
                ctx->escpos = 0;
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
                    if (ctx->pos + ctx->escpos >= VSH_CMD_SIZE - 1) {
                    input_too_long:
                        // User should increase VSH_CMD_SIZE in config.h
                        VSF_LINUX_ASSERT(false);

                        free(ctx);
                        return -ENOMEM;
                    }
                    ctx->cmd[ctx->pos + ctx->escpos++] = ch;
                    state = vsh_process_escape(ctx, state);
                    continue;
                }

                switch (ch) {
                case '\r':
#if VSH_ECHO
                    printf(VSH_LINEEND);
#endif
                    if (strlen(ctx->cmd) > 0) {
                        vsh_run_cmd(ctx);
                    }
                    goto input_cmd;
                case '\n':
                    continue;
                case '\b':
                case 0x7F:
                    ctx->cmd[--ctx->pos] = '\0';
#if VSH_ECHO
                    write(STDOUT_FILENO, "\b \b", 3);
#endif
                    break;
                default:
                    if (ctx->pos >= VSH_CMD_SIZE - 1) {
                        goto input_too_long;
                    }
                    ctx->cmd[ctx->pos++] = ch;
#if VSH_ECHO
                    write(STDOUT_FILENO, &ch, 1);
#endif
                    break;
                }
            }
        }
    }
}

int pwd_main(int argc, char *argv[])
{
    printf("%s" VSH_LINEEND, vsh_working_dir);
    return 0;
}

int cd_main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("format: cd directory_name" VSH_LINEEND);
        return -1;
    }

    if (strlen(vsh_working_dir) + strlen(argv[1]) + 1 >= MAX_PATH) {
        return -ENOMEM;
    }

    if (!strcmp(argv[1], "..")) {
        if (strcmp(vsh_working_dir, "/")) {
            vsh_working_dir[strlen(vsh_working_dir) - 1] = '\0';
            char *pos = strrchr(vsh_working_dir, '/');
            if (pos != NULL) {
                pos[1] = '\0';
            }
        }
    } else {
        char pathname[MAX_PATH];
        int err = vsh_generate_path(pathname, sizeof(pathname), vsh_working_dir, argv[1]);
        if (err != 0) { return err; }

        strcat(pathname, "/");
        DIR *dir = opendir(pathname);
        if (NULL == dir) {
            printf("%s not exists" VSH_LINEEND, argv[1]);
            return -ENOENT;
        }
        closedir(dir);
        strcpy(vsh_working_dir, pathname);
    }
    return 0;
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
    char dirname[MAX_PATH];
    int err;
    for (int i = 0; i < dirnum; i++) {
        err = vsh_generate_path(dirname, sizeof(dirname), vsh_working_dir, dirnames[i]);
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
                }
#endif
                printf("%s  ", ent->d_name);
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

    char pathname[MAX_PATH];
    int err = vsh_generate_path(pathname, sizeof(pathname), vsh_working_dir, argv[1]);
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
