#include <stdio.h>

#include "evm_module.h"
#include "ecma.h"

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
    char m[5];
    memset(m, 0, 5);

    if (mode & FS_READ) {
        sprintf(m, "%sr", m);
    }

    if (mode & FS_WRITE) {
        sprintf(m, "%sw", m);
    }

    if (mode & FS_TEXT) {
        sprintf(m, "%st", m);
    }

    if (mode & FS_BIN) {
        sprintf(m, "%sb", m);
    }

    if (mode & FS_APPEND) {
        sprintf(m, "%sa", m);
    }

    if (mode & FS_TEXT) {
        sprintf(m, "%st", m);
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
    rewind (file);
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

char * evm_open(evm_t *e, char *filename)
{
    FILE *file;
    size_t result;
    uint32_t size;
    char *buffer = NULL;

    file = fs_open(filename, FS_READ | FS_TEXT);
    if (file == NULL) { return NULL; }
    size = fs_size(file);
    evm_val_t *b = evm_buffer_create(e, sizeof(uint8_t) * size + 1);
    buffer = (char *)evm_buffer_addr(b);
    memset(buffer, 0, size + 1);
    result = fs_read(file, buffer, size);
    if (!result) {
        fclose(file);
        return NULL;
    }
    buffer[size] = 0;
    fclose(file);
    return buffer;
}

const char * vm_load(evm_t *e, char *path, int type)
{
    static const char *__modules_paths[] = {
        "/",
        "/evm_modules"
    };

    int file_name_len = strlen(path) + 1;
    char *buffer = NULL;
    if (type == EVM_LOAD_MAIN) {
        char *module_name = evm_malloc(file_name_len);
        if (!module_name ) { return NULL; }
        sprintf(module_name, "%s", path);
        sprintf(e->file_name, "%s", path);
        buffer = evm_open(e, module_name);
        evm_free(module_name);
    } else {
        for(int i = 0; i < dimof(__modules_paths); i++) {
            int len = strlen(__modules_paths[i]) + 1 + file_name_len;
            char *modules_path = evm_malloc(len);
            sprintf(modules_path, "%s/%s", __modules_paths[i], path);
            sprintf(e->file_name, "%s", path);
            buffer = evm_open(e, modules_path);
            evm_free(modules_path);
            if (buffer != NULL) {
                break;
            }
        }
    }
    return buffer;
}

void * vm_malloc(int size)
{
    void *m = malloc(size);
    if (m != NULL) {
        memset(m, 0 ,size);
    }
    return m;
}

void vm_free(void *mem)
{
    if (mem != NULL) {
        free(mem);
    }
}

evm_err_t evm_module_init(evm_t *env)
{
    return ec_ok;
}

int evm_main(void)
{
    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);

    evm_t *env = (evm_t *)evm_malloc(sizeof(evm_t));
    evm_err_t err = evm_init(env, EVM_HEAP_SIZE, EVM_STACK_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);

    err = ecma_module(env);
    if (err != ec_ok) {
        evm_print("Failed to create ecma module\r\n");
        return err;
    }

    err = evm_module_init(env);
    if (err != ec_ok) {
        return err;
    }

#ifdef EVM_LANG_ENABLE_REPL
    evm_repl_run(env, 1000, EVM_LANG_JS);
#endif

    err = evm_boot(env, "main.js");
    if (err == ec_no_file) {
        evm_print("can't open file\r\n");
        return err;
    }

    return evm_start(env);
}
