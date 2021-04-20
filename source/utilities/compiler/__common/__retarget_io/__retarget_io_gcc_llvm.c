/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
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

#endif      // __USE_COMMON_RETARGET_IO_GCC_LLVM_C__
