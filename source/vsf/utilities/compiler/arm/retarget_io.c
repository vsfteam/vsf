/*-----------------------------------------------------------------------------
 * Name:    retarget_io.c
 * Purpose: Retarget I/O
 * Rev.:    1.2.0
 *-----------------------------------------------------------------------------*/
 
/* Copyright (c) 2013 - 2016 ARM LIMITED
 
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
 
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#include <assert.h>

#include "arm_compiler.h"
 
#if !__IS_COMPILER_IAR__
#include <rt_sys.h>
#endif

#ifndef UNUSED_PARAM
# define UNUSED_PARAM(__VAL)    (__VAL) = (__VAL)
#endif

extern int vsf_stdout_putchar(char ch);
extern int vsf_stderr_putchar(char ch);
extern int vsf_stdin_getchar(void);

extern WEAK void vsf_stdout_init(void){}
extern WEAK void vsf_stdin_init(void){}

void vsf_stdio_init(void)
{
    vsf_stdout_init();
    vsf_stdin_init();
}

#if __IS_COMPILER_ARM_COMPILER_5__ || __IS_COMPILER_ARM_COMPILER_6__
#define RTE_Compiler_IO_STDOUT
#define RTE_Compiler_IO_STDOUT_User
#define RTE_Compiler_IO_STDIN
#define RTE_Compiler_IO_STDIN_User
#define RTE_Compiler_IO_STDERR
#define RTE_Compiler_IO_STDERR_User
 
#ifndef STDIN_ECHO
#define STDIN_ECHO      0       /* STDIN: echo to STDOUT */
#endif
#ifndef STDOUT_CR_LF
#define STDOUT_CR_LF    0       /* STDOUT: add CR for LF */
#endif
#ifndef STDERR_CR_LF
#define STDERR_CR_LF    0       /* STDERR: add CR for LF */
#endif
 
#ifdef RTE_Compiler_IO_STDIN
static int getchar_undo =  0;
static int getchar_ch   = -1;
#endif


/**
   Writes the character specified by c (converted to an unsigned char) to
   the output stream pointed to by stream, at the position indicated by the
   associated file position indicator (if defined), and advances the
   indicator appropriately. If the file position indicator is not defined,
   the character is appended to the output stream.
 
  \param[in] c       Character
  \param[in] stream  Stream handle
 
  \return    The character written. If a write error occurs, the error
             indicator is set and fputc returns EOF.
*/
SECTION(".vsf.utilities.stdio.arm_compiler.fputc")
int fputc (int c, FILE * stream) {

  if (stream == &__stdout) {
    return (vsf_stdout_putchar(c));
  }

  if (stream == &__stderr) {
    return (vsf_stderr_putchar(c));
  }
 
  return (-1);
}


 
/**
   Obtains the next character (if present) as an unsigned char converted to
   an int, from the input stream pointed to by stream, and advances the
   associated file position indicator (if defined).
 
  \param[in] stream  Stream handle
 
  \return    The next character from the input stream pointed to by stream.
             If the stream is at end-of-file, the end-of-file indicator is
             set and fgetc returns EOF. If a read error occurs, the error
             indicator is set and fgetc returns EOF.
*/
SECTION(".vsf.utilities.stdio.arm_compiler.fgetc")
int fgetc (FILE * stream) {
  int ch;
 
  if (stream == &__stdin) {
    if (getchar_undo) {
      ch = getchar_ch;
      getchar_ch = -1;
      getchar_undo = 0;
      return (ch);
    }
    ch = vsf_stdin_getchar();

    getchar_ch = ch;
    return (ch);
  }
 
  return (-1);
}
 
 
/**
   The function __backspace() is used by the scanf family of functions, and must
   be re-implemented if you retarget the stdio arrangements at the fgetc() level.
 
  \param[in] stream  Stream handle
 
  \return    The value returned by __backspace() is either 0 (success) or EOF
             (failure). It returns EOF only if used incorrectly, for example,
             if no characters have been read from the stream. When used 
             correctly, __backspace() must always return 0, because the scanf
             family of functions do not check the error return.
*/
SECTION(".vsf.utilities.stdio.arm_compiler.__backspace")
WEAK
int __backspace(FILE *stream) {
 
  if (stream == &__stdin) {
    if (getchar_ch != -1) {
      getchar_undo = 1;
      return (0);
    }
    return (-1);
  }
 
  return (-1);
}
 
 

/**
  Called from assert() and prints a message on stderr and calls abort().
 
  \param[in] expr  assert expression that was not TRUE
  \param[in] file  source file of the assertion
  \param[in] line  source line of the assertion
*/
SECTION(".vsf.utilities.stdio.arm_compiler.__aeabi_assert")
void __aeabi_assert (const char *expr, const char *file, int line) {
  char str[12], *p;
 
  fputs("*** assertion failed: ", stderr);
  fputs(expr, stderr);
  fputs(", file ", stderr);
  fputs(file, stderr);
  fputs(", line ", stderr);
 
  p = str + sizeof(str);
  *--p = '\0';
  *--p = '\n';
  while (line > 0) {
    *--p = '0' + (line % 10);
    line /= 10;
  }
  fputs(p, stderr);
 
  abort();
}
 
SECTION(".vsf.utilities.stdio.arm_compiler.abort")
WEAK
void abort(void) {
  for (;;);
}

 
/* IO device file handles. */
#define FH_STDIN    0x8001
#define FH_STDOUT   0x8002
#define FH_STDERR   0x8003

 
 
/**
  Defined in rt_sys.h, this function opens a file.
 
  The _sys_open() function is required by fopen() and freopen(). These
  functions in turn are required if any file input/output function is to
  be used.
  The openmode parameter is a bitmap whose bits mostly correspond directly to
  the ISO mode specification. Target-dependent extensions are possible, but
  freopen() must also be extended.
 
  \param[in] name     File name
  \param[in] openmode Mode specification bitmap
 
  \return    The return value is ¨C1 if an error occurs.
*/

//__attribute__((weak))
SECTION(".vsf.utilities.stdio.arm_compiler._sys_open")
FILEHANDLE $Sub$$_sys_open (const char *name, int openmode) 
{
    UNUSED_PARAM(name);
    UNUSED_PARAM(openmode);

    return (0);
}

/**
  Defined in rt_sys.h, this function closes a file previously opened
  with _sys_open().
  
  This function must be defined if any input/output function is to be used.
 
  \param[in] fh File handle
 
  \return    The return value is 0 if successful. A nonzero value indicates
             an error.
*/
SECTION(".vsf.utilities.stdio.arm_compiler._sys_close")
WEAK
int _sys_close (FILEHANDLE fh) 
{
    UNUSED_PARAM(fh);
    return 0;
}

 
 
/**
  Defined in rt_sys.h, this function writes the contents of a buffer to a file
  previously opened with _sys_open().
 
  \note The mode parameter is here for historical reasons. It contains
        nothing useful and must be ignored.
 
  \param[in] fh   File handle
  \param[in] buf  Data buffer
  \param[in] len  Data length
  \param[in] mode Ignore this parameter
 
  \return    The return value is either:
             - a positive number representing the number of characters not
               written (so any nonzero return value denotes a failure of
               some sort)
             - a negative number indicating an error.
*/

SECTION(".vsf.utilities.stdio.arm_compiler._sys_write")
WEAK
int _sys_write (FILEHANDLE fh, const uint8_t *buf, uint32_t len, int mode) {

  int ch;
  (void)buf;
  (void)len;

  (void)mode;
 
  switch (fh) {
    case FH_STDIN:
      return (-1);
    case FH_STDOUT:

      for (; len; len--) {
        ch = *buf++;
        vsf_stdout_putchar(ch);
      }

      return (0);
    case FH_STDERR:

      for (; len; len--) {
        ch = *buf++;
        vsf_stderr_putchar(ch);
      }
      return (0);
  }
 
  return -1;
}

 
 
/**
  Defined in rt_sys.h, this function reads the contents of a file into a buffer.
 
  Reading up to and including the last byte of data does not turn on the EOF
  indicator. The EOF indicator is only reached when an attempt is made to read
  beyond the last byte of data. The target-independent code is capable of
  handling:
    - the EOF indicator being returned in the same read as the remaining bytes
      of data that precede the EOF
    - the EOF indicator being returned on its own after the remaining bytes of
      data have been returned in a previous read.
 
  \note The mode parameter is here for historical reasons. It contains
        nothing useful and must be ignored.
 
  \param[in] fh   File handle
  \param[in] buf  Data buffer
  \param[in] len  Data length
  \param[in] mode Ignore this parameter
 
  \return     The return value is one of the following:
              - The number of bytes not read (that is, len - result number of
                bytes were read).
              - An error indication.
              - An EOF indicator. The EOF indication involves the setting of
                0x80000000 in the normal result.
*/

SECTION(".vsf.utilities.stdio.arm_compiler._sys_read")
WEAK
int _sys_read (FILEHANDLE fh, uint8_t *buf, uint32_t len, int mode) {
  int ch;

  (void)buf;
  (void)len;

  (void)mode;
 
  switch (fh) {
    case FH_STDIN:

      ch = vsf_stdin_getchar();
      if (ch < 0) {
        return ((int)(len | 0x80000000U));
      }
      *buf++ = (uint8_t)ch;

      len--;
      return ((int)(len));

    case FH_STDOUT:
      return (-1);
    case FH_STDERR:
      return (-1);
  }
  
  return -1;
}

#endif
 
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
SECTION(".vsf.utilities.stdio.arm_compiler._ttywrch")
WEAK
void _ttywrch (int ch) 
{
    vsf_stdout_putchar(ch);
}

SECTION(".vsf.utilities.stdio.arm_compiler._sys_exit")
WEAK 
void _sys_exit(int ch)
{
    while(1);
}

SECTION(".vsf.utilities.stdio.arm_compiler.stderr_putchar")
WEAK
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
        return -1;
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
        return -1;
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

