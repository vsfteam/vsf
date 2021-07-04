// vsf configurations will be check, so include vsf.h here
//  if user want to remove dependency on vsf, remove modules in evm_module_init,
//  and include stdlib.h/stdio.h/string.h
#include "vsf.h"
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

#if VSF_USE_EVM == ENABLED

#include "evm_module.h"

// implement _ctype_ if not available in libc
#if __IS_COMPILER_IAR__
#define CTYPE_U         0x01        // upper
#define CTYPE_L         0x02        // lower
#define CTYPE_N         0x04        // numberic
#define CTYPE_S         0x08        // white space(space/lf/tab)
#define CTYPE_P         0x10        // punct
#define CTYPE_C         0x20        // control
#define CTYPE_X         0x40        // hex
#define CTYPE_B         0x80        // space
#define CTYPE_SP        (CTYPE_S | CTYPE_P)

const char _ctype_[1 + 256] = {
    0,
    CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,
    CTYPE_C,        CTYPE_C|CTYPE_S,CTYPE_C|CTYPE_S,CTYPE_C|CTYPE_S,CTYPE_C|CTYPE_S,CTYPE_C|CTYPE_S,CTYPE_C,        CTYPE_C,
    CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,
    CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,        CTYPE_C,
    CTYPE_S|CTYPE_B,CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,
    CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,
    CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,        CTYPE_N,
    CTYPE_N,        CTYPE_N,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,
    CTYPE_P,        CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U|CTYPE_X,CTYPE_U,
    CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,
    CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_U,
    CTYPE_U,        CTYPE_U,        CTYPE_U,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,
    CTYPE_P,        CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L|CTYPE_X,CTYPE_L,
    CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,
    CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_L,
    CTYPE_L,        CTYPE_L,        CTYPE_L,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_P,        CTYPE_C,
};
#endif

#if     !defined(VSF_EVM_PORT_USE_LIBC) && !defined(VSF_EVM_PORT_USE_VSF)       \
    &&  !defined(VSF_EVM_PORT_USE_POSIX)
#   define VSF_EVM_PORT_USE_LIBC                ENABLED
#endif

#if VSF_EVM_PORT_USE_LIBC == ENABLED
#   include "../template/port/evm_port_libc.inc"
#elif VSF_EVM_PORT_USE_VSF == ENABLED
#   include "../template/port/evm_port_vsf.inc"
#elif VSF_EVM_PORT_USE_POSIX == ENABLED
#   include "../template/port/evm_port_posix.inc"
#else
#   error SHOULD NOT error here, becasue default proting is based on libc
#endif

static char * __evm_open(evm_t *e, char *filename)
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

static const char * __vm_load(evm_t *e, char *path, int type)
{
    char filename[EVM_FILE_NAME_LEN];
    extern evm_err_t vm_generate_path(evm_t *e, char *pathbuf, int pathbuf_size, char *path, int type);
    evm_err_t err = vm_generate_path(e, (char *)&filename, EVM_FILE_NAME_LEN, path, type);
    if (err != ec_ok) {
        return NULL;
    }

    strcpy(e->file_name, filename);
    return __evm_open(e, filename);
}

evm_t * evm_port_init(void)
{
    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)__vm_load);

    evm_t *env = (evm_t *)evm_malloc(sizeof(evm_t));
    evm_err_t err = evm_init(env, EVM_HEAP_SIZE, EVM_STACK_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);
    if (err != ec_ok) {
        evm_free(env);
        env = NULL;
    }

    return env;
}

#endif      // VSF_USE_EVM
