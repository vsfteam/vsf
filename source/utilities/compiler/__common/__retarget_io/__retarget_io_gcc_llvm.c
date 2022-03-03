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

#if VSF_USE_POSIX == ENABLED
// __assert_func is necessary because original function in newlib has dependency issue
//  __assert_func in newlib depends on fprintf, which is not usable outside vsf linux
SECTION(".vsf.utilities.stdio.gcc.__assert_func")
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
#if VSF_USE_TRACE == ENABLED
    vsf_trace_error("assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
            failedexpr, file, line, func ? ", function: " : "", func ? func : "");
#endif
    VSF_ASSERT(false);
}

SECTION(".vsf.utilities.stdio.gcc._open")
int _open(const char *path_name, int flags, mode_t mode)
{
    return open(path_name, flags, mode);
}

SECTION(".vsf.utilities.stdio.gcc._close")
void _close(int handle)
{
    close(handle);
}

SECTION(".vsf.utilities.stdio.gcc._lseek")
off_t _lseek(int handle, off_t offset, int whence)
{
    return lseek(handle, offset, whence);
}

SECTION(".vsf.utilities.stdio.gcc._kill")
int _kill(pid_t pid, int sig)
{
    return kill(pid, sig);
}

SECTION(".vsf.utilities.stdio.gcc._getpid")
pid_t _getpid(void)
{
    return getpid();
}

SECTION(".vsf.utilities.stdio.gcc._fstat")
int _fstat(int fd, struct stat *buf)
{
    return fstat(fd, buf);
}

SECTION(".vsf.utilities.stdio.gcc._isatty")
int _isatty(int fd)
{
    return 0;
}

SECTION(".vsf.utilities.stdio.gcc._unlink")
int _unlink(const char *pathname)
{
    return unlink(pathname);
}

SECTION(".vsf.utilities.stdio.gcc._link")
int _link(const char *oldpath, const char *newpath)
{
    return link(oldpath, newpath);
}

SECTION(".vsf.utilities.stdio.gcc._gettimeofday")
int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    return gettimeofday(tv, tz);
}
#endif

SECTION(".vsf.utilities.stdio.gcc._write")
int _write(int handle, char *buf, int buf_size)
{
    return __vsf_stdio_write(handle, (const unsigned char *)buf, buf_size);
}

SECTION(".vsf.utilities.stdio.gcc._read")
int _read(int handle, char *buf, int buf_size)
{
    return __vsf_stdio_read(handle, (unsigned char *)buf, buf_size);
}

SECTION(".vsf.utilities.stdio.gcc._sbrk")
void * _sbrk(intptr_t increment)
{
    return NULL;
}

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED && VSF_USE_KERNEL == ENABLED
SECTION(".vsf.utilities.stdio.gcc._exit")
void _exit(int status)
{
    vsf_thread_exit();
}
#endif

#endif      // __USE_COMMON_RETARGET_IO_GCC_LLVM_C__
