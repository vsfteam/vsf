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

extern int vsf_linux_init_main(int argc, char *argv[]);

#ifndef WEAK_VSF_LINUX_INIT_MAIN
WEAK(vsf_linux_init_main)
int vsf_linux_init_main(int argc, char *argv[])
{
    static const char *__path[] = {
        "/sbin/",
        NULL,
    };
    vsh_set_path((char **)__path);
    return vsh_main(argc, argv);
}
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
        vsf_linux_fs_bind_executable(fd, entry);
        close(fd);
    }
    printf("%s installed.\r\n", path);
    return fd;
}

int busybox_install(void)
{
    if (    mkdir("/sbin", 0)
        ||  busybox_bind("/sbin/init", vsf_linux_init_main) < 0) {
        return -1;
    }

#if VSF_LINUX_USE_BUSYBOX == ENABLED
    if (    busybox_bind("/sbin/ls", ls_main) < 0
        ||  busybox_bind("/sbin/cd", cd_main) < 0
        ||  busybox_bind("/sbin/pwd", pwd_main) < 0
        ||  busybox_bind("/sbin/cat", cat_main) < 0
        ||  busybox_bind("/sbin/echo", echo_main) < 0
        ||  busybox_bind("/sbin/mkdir", mkdir_main) < 0) {
        return -1;
    }
#endif
    return 0;
}

#endif
