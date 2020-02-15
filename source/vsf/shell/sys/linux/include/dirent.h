#ifndef __DIRENT_H__
#define __DIRENT_H__

#include <sys/types.h>

struct dirent {
    long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char *d_name;
};
typedef struct vsf_linux_fd_t DIR;

DIR * opendir(const char *name);
struct dirent * readdir(DIR *dir);
int closedir(DIR *dir);

#endif
