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
 
 
//#if !__IS_COMPILER_IAR__
//#include <rt_sys.h>
//#endif

#ifdef __USE_COMMON_RETARGET_IO_C__
#undef __USE_COMMON_RETARGET_IO_C__

#include <stdio.h>

#ifndef UNUSED_PARAM
# define UNUSED_PARAM(__VAL)    (__VAL) = (__VAL)
#endif

extern int vsf_stdout_putchar(char ch);
extern int vsf_stderr_putchar(char ch);
extern int vsf_stdin_getchar(void);

extern void vsf_stdout_init(void);
extern void vsf_stdin_init(void);

void vsf_stdio_init(void)
{
    vsf_stdout_init();
    vsf_stdin_init();
}

/**
  Defined in rt_sys.h, this function writes a character to the console. The
  console might have been redirected. You can use this function as a last
  resort error handling routine.
  
  The default implementation of this function uses semihosting.
  You can redefine this function, or __raise(), even if there is no other
  input/output. For example, it might write an error message to a log kept
  in nonvolatile memory.
 
  \param[in] ch character to write
*/
//SECTION(".vsf.utilities.stdio.arm_compiler._ttywrch")
WEAK(_ttywrch)
void _ttywrch (int ch) 
{
    vsf_stdout_putchar(ch);
}

//SECTION(".vsf.utilities.stdio.arm_compiler._sys_exit")
WEAK(_sys_exit)
void _sys_exit(int ch)
{
    UNUSED_PARAM(ch);
    while(1);
}

//SECTION(".vsf.utilities.stdio.arm_compiler.stderr_putchar")
WEAK(vsf_stderr_putchar)
int vsf_stderr_putchar(char ch)
{
    return vsf_stdout_putchar(ch);
}

SECTION(".vsf.utilities.stdio.iar.__write")
/* for IAR */
size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{
    size_t nChars = 0;
    /* Check for the command to flush all handles */
    if (handle == -1) {
        return 0;
    }
    /* Check for stdout and stderr
    (only necessary if FILE descriptors are enabled.) */
    if (handle != 1 && handle != 2) {
        return 0;
    }
    for (/* Empty */; bufSize > 0; --bufSize) {
        vsf_stdout_putchar(*buf++);
        ++nChars;
    }
    return nChars;
}

SECTION(".vsf.utilities.stdio.iar.__read")
size_t __read(int handle, unsigned char *buf, size_t bufSize)
{
    size_t nChars = 0;
    /* Check for stdin
    (only necessary if FILE descriptors are enabled) */
    if (handle != 0) {
        return 0;
    }
    for (/*Empty*/; bufSize > 0; --bufSize) {
        uint8_t c = vsf_stdin_getchar();
        if (c == 0) { break; }
        *buf++ = c;
        ++nChars;
    }
    return nChars;
}




/* for GCC / LLVM */
SECTION(".vsf.utilities.stdio.gcc._write")
int _write (int handle, char *buf, int bufSize)
{
    return __write(handle, (const unsigned char *)buf, bufSize);
}

SECTION(".vsf.utilities.stdio.gcc._read")
int _read (int handle, char *buf, int bufSize)
{
    return __read(handle, (unsigned char *)buf, bufSize);
}

#endif
