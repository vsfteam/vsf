#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/unistd.h"
#else
#   include <unistd.h>
#endif
#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED && VSF_LINUX_USE_SIMPLE_STDIO == ENABLED
#   include "../../include/simple_libc/stdio.h"
#else
#   include <stdio.h>
#endif

extern void vsh_set_path(char **path);
extern int vsh_main(int argc, char *argv[]);
extern int cd_main(int argc, char *argv[]);
extern int ls_main(int argc, char *argv[]);
extern int pwd_main(int argc, char *argv[]);
extern int cat_main(int argc, char *argv[]);
extern int echo_main(int argc, char *argv[]);
extern int mkdir_main(int argc, char *argv[]);
extern int clear_main(int argc, char *argv[]);
extern int time_main(int argc, char *argv[]);
#if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
extern int export_main(int argc, char *argv[]);
#endif
#if !defined(VSF_ARCH_PROVIDE_HEAP) && VSF_HEAP_CFG_STATISTICS == ENABLED
extern int free_main(int argc, char *argv[]);
#endif

extern int vsf_linux_init_main(int argc, char *argv[]);

#ifndef WEAK_VSF_LINUX_INIT_MAIN
#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wcast-align"
#endif
WEAK(vsf_linux_init_main)
int vsf_linux_init_main(int argc, char *argv[])
{
    // GCC: -Wcast-align
    vsh_set_path((char **)VSF_LINUX_CFG_BIN_PATH);

    // run init scripts first
#ifdef VSF_LINUX_CFG_INIT_SCRIPTS
    static const char * __init_scripts[] = {
        VSF_LINUX_CFG_INIT_SCRIPTS
    };
    for (int i = 0; i < dimof(__init_scripts); i++) {
        printf("execute init script: %s\r\n", __init_scripts[i]);
        system(__init_scripts[i]);
    }
#endif
#ifdef VSF_LINUX_CFG_INIT_SCRIPT_FILE
    FILE *f = fopen(VSF_LINUX_CFG_INIT_SCRIPT_FILE, "rt");
    char linebuf[256], *line;
    do {
        line = fgets(linebuf, sizeof(linebuf), f);
        if (line != NULL) {
            while ((*line != '\0') && isspace((int)*line)) { line++; }

            if (*line != '\0' && *line != '#') {
                printf("execute init script: %s\r\n", line);
                system(line);
            }
        }
    } while (line != NULL);
#endif

    return vsh_main(argc, argv);
}
#if __IS_COMPILER_GCC__
#   pragma GCC diagnostic pop
#endif
#endif

int busybox_bind(char *path, vsf_linux_main_entry_t entry)
{
    int fd = open(path, 0);
    if (fd < 0) {
        fd = creat(path, 0);
        if (fd < 0) {
            printf("fail to install %s.\r\n", path);
            return fd;
        }
    }
    if (fd >= 0) {
        vsf_linux_fd_bind_executable(fd, entry);
        close(fd);
    }
    printf("%s installed.\r\n", path);
    return fd;
}

int busybox_install(void)
{
    if (    mkdirs(VSF_LINUX_CFG_BIN_PATH, 0)
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/init", vsf_linux_init_main) < 0) {
        return -1;
    }

#if VSF_LINUX_USE_BUSYBOX == ENABLED
    if (    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/sh", vsh_main) < 0
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/ls", ls_main) < 0
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/cd", cd_main) < 0
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/pwd", pwd_main) < 0
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/cat", cat_main) < 0
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/echo", echo_main) < 0
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/mkdir", mkdir_main) < 0
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/clear", clear_main) < 0
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/time", time_main) < 0
#   if VSF_LINUX_LIBC_USE_ENVIRON == ENABLED
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/export", export_main) < 0
#   endif
#if !defined(VSF_ARCH_PROVIDE_HEAP) && VSF_HEAP_CFG_STATISTICS == ENABLED
        ||  busybox_bind(VSF_LINUX_CFG_BIN_PATH "/free", free_main) < 0
#   endif
        ) {
        return -1;
    }
#endif
    return 0;
}

#endif

