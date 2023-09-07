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

#include "component/script/python/vsf_python.h"

#if VSF_USE_MICROPYTHON == ENABLED

#include "py/stream.h"

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

STATIC void __file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_obj_file_t *self = MP_OBJ_TO_PTR(self_in);
    int fd = ((vsf_linux_fd_t *)self->f)->fd;
    mp_printf(print, "<io.%s %d>", mp_obj_get_type_str(self_in), fd);
}

STATIC mp_uint_t __file_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode)
{
    mp_obj_file_t *self = MP_OBJ_TO_PTR(self_in);
    __check_fd_is_open(self);
    int fd = ((vsf_linux_fd_t *)self->f)->fd;
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
    int fd = ((vsf_linux_fd_t *)self->f)->fd;
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
    int fd = ((vsf_linux_fd_t *)self->f)->fd;
    return MP_OBJ_NEW_SMALL_INT(fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(__file_fileno_obj, __file_fileno);

STATIC mp_uint_t __file_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode)
{
    mp_obj_file_t *self = MP_OBJ_TO_PTR(self_in);
    int fd = -1;

    if (request != MP_STREAM_CLOSE) {
        __check_fd_is_open(self);
        fd = ((vsf_linux_fd_t *)self->f)->fd;
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
        struct pollfd pfd = { .fd = o->fd, .events = pollevents };
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

#endif
/* EOF */
