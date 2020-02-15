#include <unistd.h>
#include <stdio.h>

#if VSF_USE_LINUX == ENABLED

extern void vsh_set_path(char **path);
extern int vsh_main(int argc, char *argv[]);
extern int cd_main(int argc, char *argv[]);
extern int ls_main(int argc, char *argv[]);
extern int pwd_main(int argc, char *argv[]);
extern int cat_main(int argc, char *argv[]);
extern int echo_main(int argc, char *argv[]);
extern int mkdir_main(int argc, char *argv[]);

static int init_main(int argc, char *argv[])
{
    return vsh_main(argc, argv);
}

int busybox_bind(char *path, vsf_linux_main_entry_t entry)
{
    int fd = open(path, 0);
    if (fd < 0) {
        fd = creat(path, 0);
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
    if (mkdir("/sbin", 0)) {
        return -1;
    }

    if (    busybox_bind("/sbin/init", init_main) < 0
        ||  busybox_bind("/sbin/ls", ls_main) < 0
        ||  busybox_bind("/sbin/cd", cd_main) < 0
        ||  busybox_bind("/sbin/pwd", pwd_main) < 0
        ||  busybox_bind("/sbin/cat", cat_main) < 0
        ||  busybox_bind("/sbin/echo", echo_main) < 0
        ||  busybox_bind("/sbin/mkdir", mkdir_main) < 0) {
        return -1;
    }

    static const char *path[] = {
        "/sbin/",
        NULL,
    };
    vsh_set_path((char **)path);
    return 0;
}

#endif
