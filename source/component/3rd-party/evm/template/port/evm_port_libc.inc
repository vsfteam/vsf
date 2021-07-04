char evm_repl_tty_read(evm_t *e)
{
    EVM_UNUSED(e);
    return (char)getchar();
}

enum FS_MODE {
    FS_READ     = 1,
    FS_WRITE    = 2,
    FS_APPEND   = 4,
    FS_CREATE   = 8,
    FS_OPEN     = 16,
    FS_TEXT     = 32,
    FS_BIN      = 64,
};

void * fs_open(char *name, int mode)
{
    char m[5] = { 0 };
    int pos = 0;

    if (mode & FS_READ) {
        m[pos++] = 'r';
    }

    if (mode & FS_WRITE) {
        m[pos++] = 'w';
    }

    if (mode & FS_TEXT) {
        m[pos++] = 't';
    } else if (mode & FS_BIN) {
        m[pos++] = 'b';
    }

    if (mode & FS_APPEND) {
        m[pos++] = 'a';
    }

    return fopen(name, m);
}

void fs_close(void *handle)
{
    fclose((FILE *)handle);
}

int fs_size(void *handle)
{
    FILE *file = (void *)handle;
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}

int fs_read(void *handle, char *buf, int len)
{
    return fread(buf, 1, len, (FILE *)handle);
}

int fs_write(void *handle, char *buf, int len)
{
    return fwrite(buf, 1, len, (FILE *)handle);
}

#ifndef WEAK_VM_MALLOC
WEAK(vm_malloc)
void * vm_malloc(int size)
{
    void *m = malloc(size);
    if (m != NULL) {
        memset(m, 0 ,size);
    }
    return m;
}
#endif

#ifndef WEAK_VM_FREE
WEAK(vm_free)
void vm_free(void *mem)
{
    if (mem != NULL) {
        free(mem);
    }
}
#endif