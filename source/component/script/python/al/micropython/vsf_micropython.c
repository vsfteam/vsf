/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#define __VSF_LINUX_FS_CLASS_INHERIT__
#include "component/script/python/vsf_python.h"

#if VSF_USE_MICROPYTHON == ENABLED

#include "py/stream.h"
#include "py/builtin.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

STATIC void __check_fd_is_open(const mp_obj_file_t *self) {
    if (NULL == self->f) {
        mp_raise_ValueError(MP_ERROR_TEXT("I/O operation on closed file"));
    }
}

STATIC int __file_getfd(const mp_obj_file_t *self)
{
    if (self->fd < 3) {
        return self->fd;
    }
    return ((vsf_linux_fd_t *)self->f)->fd;
}

STATIC void __file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_obj_file_t *self = MP_OBJ_TO_PTR(self_in);
    int fd = __file_getfd(self);
    mp_printf(print, "<io.%s %d>", mp_obj_get_type_str(self_in), fd);
}

STATIC mp_uint_t __file_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode)
{
    mp_obj_file_t *self = MP_OBJ_TO_PTR(self_in);
    __check_fd_is_open(self);
    int fd = __file_getfd(self);
    ssize_t r = read(fd, buf, size);
    if (r < 0) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }
    return (mp_uint_t)r;
}

STATIC mp_uint_t __file_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode)
{
    mp_obj_file_t *self = MP_OBJ_TO_PTR(self_in);
    __check_fd_is_open(self);
    int fd = __file_getfd(self);
    ssize_t r = write(fd, buf, size);
    if (r < 0) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }
    return (mp_uint_t)r;
}

STATIC mp_obj_t __file_fileno(mp_obj_t self_in)
{
    mp_obj_file_t *self = MP_OBJ_TO_PTR(self_in);
    __check_fd_is_open(self);
    int fd = __file_getfd(self);
    return MP_OBJ_NEW_SMALL_INT(fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(__file_fileno_obj, __file_fileno);

STATIC mp_uint_t __file_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode)
{
    mp_obj_file_t *self = MP_OBJ_TO_PTR(self_in);
    int fd = -1;

    if (request != MP_STREAM_CLOSE) {
        __check_fd_is_open(self);
        fd = __file_getfd(self);
    }

    switch (request) {
    case MP_STREAM_FLUSH: {
        int r = fsync(fd);
        if (r < 0) {
            *errcode = errno;
            return MP_STREAM_ERROR;
        }
        return 0;
    }
    case MP_STREAM_SEEK: {
        struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)arg;
        MP_THREAD_GIL_EXIT();
        off_t off = lseek(fd, s->offset, s->whence);
        MP_THREAD_GIL_ENTER();
        if ((off_t)-1 == off) {
            *errcode = errno;
            return MP_STREAM_ERROR;
        }
        s->offset = off;
        return 0;
    }
    case MP_STREAM_CLOSE:
        if (self->f != NULL) {
            MP_THREAD_GIL_EXIT();
            fclose(self->f);
            MP_THREAD_GIL_ENTER();
        }
        self->f = NULL;
        return 0;
    case MP_STREAM_GET_FILENO:
        return fd;
#if MICROPY_PY_SELECT && !MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
    case MP_STREAM_POLL: {
        mp_uint_t ret = 0;
        uint8_t pollevents = 0;
        if (arg & MP_STREAM_POLL_RD) {
            pollevents |= POLLIN;
        }
        if (arg & MP_STREAM_POLL_WR) {
            pollevents |= POLLOUT;
        }
        struct pollfd pfd = { .fd = fd, .events = pollevents };
        if (poll(&pfd, 1, 0) > 0) {
            if (pfd.revents & POLLIN) {
                ret |= MP_STREAM_POLL_RD;
            }
            if (pfd.revents & POLLOUT) {
                ret |= MP_STREAM_POLL_WR;
            }
            if (pfd.revents & POLLERR) {
                ret |= MP_STREAM_POLL_ERR;
            }
            if (pfd.revents & POLLHUP) {
                ret |= MP_STREAM_POLL_HUP;
            }
            if (pfd.revents & POLLNVAL) {
                ret |= MP_STREAM_POLL_NVAL;
            }
        }
        return ret;
    }
#endif
    default:
        *errcode = EINVAL;
        return MP_STREAM_ERROR;
    }
}

/*============================ LOCAL VARIABLES ===============================*/

STATIC const mp_rom_map_elem_t __file_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&__file_fileno_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },
};
STATIC MP_DEFINE_CONST_DICT(__file_locals_dict, __file_locals_dict_table);

STATIC const mp_stream_p_t __fileio_stream_p = {
    .read = __file_read,
    .write = __file_write,
    .ioctl = __file_ioctl,
};

STATIC const mp_stream_p_t __textio_stream_p = {
    .read = __file_read,
    .write = __file_write,
    .ioctl = __file_ioctl,
    .is_text = true,
};

/*============================ GLOBAL VARIABLES ==============================*/

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_fileio,
    MP_QSTR_FileIO,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, __file_print,
    protocol, &__fileio_stream_p,
    locals_dict, &__file_locals_dict
);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_textio,
    MP_QSTR_TextIOWrapper,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, __file_print,
    protocol, &__textio_stream_p,
    locals_dict, &__file_locals_dict
);

#if !MICROPY_VFS
mp_import_stat_t mp_import_stat(const char *path)
{
    struct stat st;
    int result = stat(path, &st);
    if (result < 0) {
        return MP_IMPORT_STAT_NO_EXIST;
    }
    return S_ISDIR(st.st_mode) ? MP_IMPORT_STAT_DIR : MP_IMPORT_STAT_FILE;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_file, ARG_mode, ARG_encoding };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_mode, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_QSTR(MP_QSTR_r)} },
        { MP_QSTR_buffering, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_encoding, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mp_obj_t open_args[2] = {
        args[ARG_file].u_obj,
        args[ARG_mode].u_obj,
    };
    extern vsf_pyal_obj_t os_open(size_t num, const mp_obj_t *args);
    return os_open(dimof(open_args), open_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 0, mp_builtin_open);

mp_obj_t mp_vfs_open(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    return mp_builtin_open(n_args, pos_args, kw_args);
}

const mp_obj_file_t mp_sys_stdin_obj = {
    .base.type = &mp_type_textio,
    .fd = STDIN_FILENO,
};
const mp_obj_file_t mp_sys_stdout_obj = {
    .base.type = &mp_type_textio,
    .fd = STDOUT_FILENO,
};
const mp_obj_file_t mp_sys_stderr_obj = {
    .base.type = &mp_type_textio,
    .fd = STDERR_FILENO,
};

#endif

vsf_pyal_arg_t vsf_pyal_tupleobj_getarg(vsf_pyal_obj_t self_in, int idx)
{
    size_t len;
    vsf_pyal_arg_t *items;
    mp_obj_tuple_get(self_in, &len, &items);
    return idx >= len ? VSF_PYAL_ARG_NULL : items[idx];
}

int vsf_pyal_tupleobj_getnum(vsf_pyal_obj_t self_in)
{
    size_t len;
    vsf_pyal_arg_t *items;
    mp_obj_tuple_get(self_in, &len, &items);
    return len;
}

size_t vsf_pyal_listobj_getnum(vsf_pyal_obj_t self_in)
{
    size_t len;
    vsf_pyal_obj_t *items;
    mp_obj_list_get(self_in, &len, &items);
    return len;
}

vsf_pyal_obj_t vsf_pyal_listobj_getarg(vsf_pyal_obj_t self_in, int idx)
{
    size_t len;
    vsf_pyal_obj_t *items;
    mp_obj_list_get(self_in, &len, &items);
    return idx >= len ? VSF_PYAL_ARG_NULL : items[idx];
}

#endif
/* EOF */
