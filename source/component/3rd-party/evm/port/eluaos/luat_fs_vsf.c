#include "vsf.h"

#if VSF_USE_EVM == ENABLED && VSF_EVM_USE_LUA == ENABLED

#include <unistd.h>
#include <stdio.h>

FILE * luat_fs_fopen(const char *filename, const char *mode) {
    return fopen(filename, mode);
}

int luat_fs_getc(FILE *stream) {
    return getc(stream);
}

int luat_fs_fseek(FILE *stream, long int offset, int origin) {
    return fseek(stream, offset, origin);
}

int luat_fs_ftell(FILE *stream) {
    return ftell(stream);
}

int luat_fs_fclose(FILE *stream) {
    return fclose(stream);
}

int luat_fs_feof(FILE *stream) {
    return feof(stream);
}

int luat_fs_ferror(FILE *stream) {
    return ferror(stream);
}

size_t luat_fs_fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fread(ptr, size, nmemb, stream);
}

size_t luat_fs_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

int luat_fs_remove(const char *filename) {
    return remove(filename);
}

int luat_fs_rename(const char *old_filename, const char *new_filename) {
    return rename(old_filename, new_filename);
}

int luat_fs_fexist(const char *filename) {
    if (access(filename, 0) == 0) {
        return 1;
    }
    return 0;
}

size_t luat_fs_fsize(const char *filename) {
    FILE *fd;
    size_t size = 0;
    fd = luat_fs_fopen(filename, "rb");
    if (fd) {
        luat_fs_fseek(fd, 0, SEEK_END);
        size = luat_fs_ftell(fd);
        luat_fs_fclose(fd);
    }
    return size;
}

#endif      // VSF_USE_EVM && VSF_EVM_USE_LUA
