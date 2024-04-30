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

#ifdef __USE_COMMON_RETARGET_IO_GCC_LLVM_C__
#undef __USE_COMMON_RETARGET_IO_GCC_LLVM_C__

// for vsf_thread_exit if thread is enabled
#include "kernel/vsf_kernel.h"

#if VSF_USE_POSIX == ENABLED
// __assert_func is necessary because original function in newlib has dependency issue
//  __assert_func in newlib depends on fprintf, which is not usable outside vsf linux
VSF_CAL_SECTION(".vsf.utilities.stdio.gcc.__assert_func")
VSF_CAL_WEAK(__assert_func)
VSF_CAL_NO_RETURN void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_error("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
            failedexpr, file, line, func ? ", function: " : "", func ? func : "");
#endif
    VSF_ASSERT(false);
    while (1);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._open")
VSF_CAL_WEAK(_open)
int _open(const char *path_name, int flags, mode_t mode)
{
    return open(path_name, flags, mode);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._close")
VSF_CAL_WEAK(_close)
void _close(int handle)
{
    close(handle);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._lseek")
VSF_CAL_WEAK(_lseek)
off_t _lseek(int handle, off_t offset, int whence)
{
    return lseek(handle, offset, whence);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._kill")
VSF_CAL_WEAK(_kill)
int _kill(pid_t pid, int sig)
{
    return kill(pid, sig);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._getpid")
VSF_CAL_WEAK(_getpid)
pid_t _getpid(void)
{
    return getpid();
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._fstat")
VSF_CAL_WEAK(_fstat)
int _fstat(int fd, struct stat *buf)
{
    return fstat(fd, buf);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._isatty")
VSF_CAL_WEAK(_isatty)
int _isatty(int fd)
{
    return isatty(fd);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._unlink")
VSF_CAL_WEAK(_unlink)
int _unlink(const char *pathname)
{
    return unlink(pathname);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._link")
VSF_CAL_WEAK(_link)
int _link(const char *oldpath, const char *newpath)
{
    return link(oldpath, newpath);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._gettimeofday")
VSF_CAL_WEAK(_gettimeofday)
int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    return gettimeofday(tv, tz);
}
#else
VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._close")
VSF_CAL_WEAK(_close)
void _close(int handle) {}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._lseek")
VSF_CAL_WEAK(_lseek)
off_t _lseek(int handle, off_t offset, int whence) { return (off_t)0; }

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._kill")
VSF_CAL_WEAK(_kill)
int _kill(int pid, int sig) { return 0; }

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._getpid")
VSF_CAL_WEAK(_getpid)
int _getpid(void) { return 0; }

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._fstat")
VSF_CAL_WEAK(_fstat)
int _fstat(int fd, void *buf) { return -1; }

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._isatty")
VSF_CAL_WEAK(_isatty)
int _isatty(int fd) { return 0; }
#endif

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._write")
VSF_CAL_WEAK(_write)
int _write(int handle, char *buf, int buf_size)
{
    return __vsf_stdio_write(handle, (const unsigned char *)buf, buf_size);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._read")
VSF_CAL_WEAK(_read)
int _read(int handle, char *buf, int buf_size)
{
    return __vsf_stdio_read(handle, (unsigned char *)buf, buf_size);
}

VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._sbrk")
VSF_CAL_WEAK(_sbrk)
void * _sbrk(intptr_t increment)
{
    return NULL;
}

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED && VSF_USE_KERNEL == ENABLED
VSF_CAL_SECTION(".vsf.utilities.stdio.gcc._exit")
VSF_CAL_WEAK(_exit)
VSF_CAL_NO_RETURN void _exit(int status)
{
    vsf_thread_exit();
}
#endif

#endif      // __USE_COMMON_RETARGET_IO_GCC_LLVM_C__
