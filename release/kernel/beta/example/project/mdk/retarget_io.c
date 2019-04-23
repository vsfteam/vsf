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
#include <rt_sys.h>
 
//#include "RTE_Components.h"
#define RTE_Compiler_IO_STDOUT
#define RTE_Compiler_IO_STDOUT_User

#ifdef RTE_Compiler_IO_STDOUT_EVR
#include "EventRecorder.h"
#endif
 
#ifdef RTE_Compiler_IO_File_FS
#include "rl_fs_lib.h"
#endif
 
 
#ifndef STDIN_ECHO
#define STDIN_ECHO      0       /* STDIN: echo to STDOUT */
#endif
#ifndef STDOUT_CR_LF
#define STDOUT_CR_LF    0       /* STDOUT: add CR for LF */
#endif
#ifndef STDERR_CR_LF
#define STDERR_CR_LF    0       /* STDERR: add CR for LF */
#endif
 
 
#if (defined(RTE_Compiler_IO_TTY_ITM)    || \
     defined(RTE_Compiler_IO_STDIN_ITM)  || \
     defined(RTE_Compiler_IO_STDOUT_ITM) || \
     defined(RTE_Compiler_IO_STDERR_ITM))
 
/* ITM registers */
#define ITM_PORT0_U8          (*((volatile uint8_t  *)0xE0000000))
#define ITM_PORT0_U32         (*((volatile uint32_t *)0xE0000000))
#define ITM_TER               (*((volatile uint32_t *)0xE0000E00))
#define ITM_TCR               (*((volatile uint32_t *)0xE0000E80))
 
#define ITM_TCR_ITMENA_Msk    (1UL << 0)
 
/*!< Value identifying \ref ITM_RxBuffer is ready for next character. */
#define ITM_RXBUFFER_EMPTY    0x5AA55AA5
 
/*!< Variable to receive characters. */
extern
volatile int32_t ITM_RxBuffer;
volatile int32_t ITM_RxBuffer = ITM_RXBUFFER_EMPTY;   
 
/** \brief  ITM Send Character
 
    The function transmits a character via the ITM channel 0, and
    \li Just returns when no debugger is connected that has booked the output.
    \li Is blocking when a debugger is connected, but the previous character
        sent has not been transmitted.
 
    \param [in]     ch  Character to transmit.
 
    \returns            Character to transmit.
 */
int32_t ITM_SendChar (int32_t ch);
int32_t ITM_SendChar (int32_t ch) {
  if ((ITM_TCR & ITM_TCR_ITMENA_Msk) && /* ITM enabled */
      (ITM_TER & (1UL << 0)        )) { /* ITM Port #0 enabled */
    while (ITM_PORT0_U32 == 0);
    ITM_PORT0_U8 = (uint8_t)ch;
  }
  return (ch);
}
 
/** \brief  ITM Receive Character
 
    The function inputs a character via the external variable \ref ITM_RxBuffer.
    This variable is monitored and altered by the debugger to provide input.
 
    \return             Received character.
    \return         -1  No character pending.
 */
int32_t ITM_ReceiveChar (void);
int32_t ITM_ReceiveChar (void) {
  int32_t ch = -1;                      /* no character available */
 
  if (ITM_RxBuffer != ITM_RXBUFFER_EMPTY) {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ITM_RXBUFFER_EMPTY;  /* ready for next character */
  }
 
  return (ch);
}
 
#endif  /* RTE_Compiler_IO_STDxxx_ITM */
 
 
/**
  Get a character from the stdio
 
  \return     The next character from the input, or -1 on read error.
*/
#if   defined(RTE_Compiler_IO_STDIN)
#if   defined(RTE_Compiler_IO_STDIN_User)
extern int stdin_getchar (void);
#elif defined(RTE_Compiler_IO_STDIN_ITM)
static int stdin_getchar (void) {
  int32_t ch;
 
  do {
    ch = ITM_ReceiveChar();
  } while (ch == -1);
  return (ch);
}
#elif defined(RTE_Compiler_IO_STDIN_BKPT)
static int stdin_getchar (void) {
  int32_t ch = -1;
 
  __asm("BKPT 0");
  return (ch);
}
#endif
#endif
 
 
/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
#if   defined(RTE_Compiler_IO_STDOUT)
#if   defined(RTE_Compiler_IO_STDOUT_User)
extern int stdout_putchar (int ch);
#elif defined(RTE_Compiler_IO_STDOUT_ITM)
static int stdout_putchar (int ch) {
  return (ITM_SendChar(ch));
}
#elif defined(RTE_Compiler_IO_STDOUT_EVR)
static int stdout_putchar (int ch) {
  static uint32_t index = 0U;
  static uint8_t  buffer[8];
 
  if (index >= 8U) {
    return (-1);
  }
  buffer[index++] = (uint8_t)ch;
  if ((index == 8U) || (ch == '\n')) {
    EventRecordData(EventID(EventLevelOp, 0xFE, 0x00), buffer, index);
    index = 0U;
  }
  return (ch);
}
#elif defined(RTE_Compiler_IO_STDOUT_BKPT)
static int stdout_putchar (int ch) {
  __asm("BKPT 0");
  return (ch);
}
#endif
#endif
 
 
/**
  Put a character to the stderr
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
#if   defined(RTE_Compiler_IO_STDERR)
#if   defined(RTE_Compiler_IO_STDERR_User)
extern int stderr_putchar (int ch);
#elif defined(RTE_Compiler_IO_STDERR_ITM)
static int stderr_putchar (int ch) {
  return (ITM_SendChar(ch));
}
#elif defined(RTE_Compiler_IO_STDERR_BKPT)
static int stderr_putchar (int ch) {
  __asm("BKPT 0");
  return (ch);
}
#endif
#endif
 
 
#ifdef __MICROLIB
 
 
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
__attribute__((weak))
int fputc (int c, FILE * stream) {
#if (!defined(RTE_Compiler_IO_STDOUT) && !defined(RTE_Compiler_IO_STDERR))
  (void)c;
  (void)stream;
#endif
 
#ifdef RTE_Compiler_IO_STDOUT
  if (stream == &__stdout) {
#if (STDOUT_CR_LF != 0)
    if (c == '\n') stdout_putchar('\r');
#endif
    return (stdout_putchar(c));
  }
#endif
 
#ifdef RTE_Compiler_IO_STDERR
  if (stream == &__stderr) {
#if (STDERR_CR_LF != 0)
    if (c == '\n') stderr_putchar('\r');
#endif
    return (stderr_putchar(c));
  }
#endif
 
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
__attribute__((weak))
int fgetc (FILE * stream) {
#ifdef RTE_Compiler_IO_STDIN
  int ch;
 
  if (stream == &__stdin) {
    if (getchar_undo) {
      ch = getchar_ch;
      getchar_ch = -1;
      getchar_undo = 0;
      return (ch);
    }
    ch = stdin_getchar();
#if (STDIN_ECHO != 0)
    stdout_putchar(ch);
#endif
    getchar_ch = ch;
    return (ch);
  }
#else
  (void)stream;
#endif
 
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
__attribute__((weak))
int __backspace(FILE *stream);
int __backspace(FILE *stream) {
 
#ifdef RTE_Compiler_IO_STDIN
  if (stream == &__stdin) {
    if (getchar_ch != -1) {
      getchar_undo = 1;
      return (0);
    }
    return (-1);
  }
#else
  (void)stream;
#endif
 
  return (-1);
}
 
 
/**
  Called from assert() and prints a message on stderr and calls abort().
 
  \param[in] expr  assert expression that was not TRUE
  \param[in] file  source file of the assertion
  \param[in] line  source line of the assertion
*/
__attribute__((weak,noreturn))
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
 
 
__attribute__((weak))
void abort(void) {
  for (;;);
}
 
 
#else  /* __MICROLIB */
 
 
#if (defined(RTE_Compiler_IO_STDIN)  || \
     defined(RTE_Compiler_IO_STDOUT) || \
     defined(RTE_Compiler_IO_STDERR) || \
     defined(RTE_Compiler_IO_File))
#define RETARGET_SYS
 
/* IO device file handles. */
#define FH_STDIN    0x8001
#define FH_STDOUT   0x8002
#define FH_STDERR   0x8003
// User defined ...
 
/* Standard IO device name defines. */
const char __stdin_name[]  = ":STDIN";
const char __stdout_name[] = ":STDOUT";
const char __stderr_name[] = ":STDERR";
 
#endif
 
 
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
 
  \return    The return value is –1 if an error occurs.
*/
#ifdef RETARGET_SYS
__attribute__((weak))
FILEHANDLE _sys_open (const char *name, int openmode) {
#if (!defined(RTE_Compiler_IO_File))
  (void)openmode;
#endif
 
  if (name == NULL) {
    return (-1);
  }
 
  if (name[0] == ':') {
    if (strcmp(name, ":STDIN") == 0) {
      return (FH_STDIN);
    }
    if (strcmp(name, ":STDOUT") == 0) {
      return (FH_STDOUT);
    }
    if (strcmp(name, ":STDERR") == 0) {
      return (FH_STDERR);
    }
    return (-1);
  }
 
#ifdef RTE_Compiler_IO_File
#ifdef RTE_Compiler_IO_File_FS
  return (__sys_open(name, openmode));
#endif
#else
  return (-1);
#endif
}
#endif
 
 
/**
  Defined in rt_sys.h, this function closes a file previously opened
  with _sys_open().
  
  This function must be defined if any input/output function is to be used.
 
  \param[in] fh File handle
 
  \return    The return value is 0 if successful. A nonzero value indicates
             an error.
*/
#ifdef RETARGET_SYS
__attribute__((weak))
int _sys_close (FILEHANDLE fh) {
 
  switch (fh) {
    case FH_STDIN:
      return (0);
    case FH_STDOUT:
      return (0);
    case FH_STDERR:
      return (0);
  }
 
#ifdef RTE_Compiler_IO_File
#ifdef RTE_Compiler_IO_File_FS
  return (__sys_close(fh));
#endif
#else
  return (-1);
#endif
}
#endif
 
 
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
#ifdef RETARGET_SYS
__attribute__((weak))
int _sys_write (FILEHANDLE fh, const uint8_t *buf, uint32_t len, int mode) {
#if (defined(RTE_Compiler_IO_STDOUT) || defined(RTE_Compiler_IO_STDERR))
  int ch;
#elif (!defined(RTE_Compiler_IO_File))
  (void)buf;
  (void)len;
#endif
  (void)mode;
 
  switch (fh) {
    case FH_STDIN:
      return (-1);
    case FH_STDOUT:
#ifdef RTE_Compiler_IO_STDOUT
      for (; len; len--) {
        ch = *buf++;
#if (STDOUT_CR_LF != 0)
        if (ch == '\n') stdout_putchar('\r');
#endif
        stdout_putchar(ch);
      }
#endif
      return (0);
    case FH_STDERR:
#ifdef RTE_Compiler_IO_STDERR
      for (; len; len--) {
        ch = *buf++;
#if (STDERR_CR_LF != 0)
        if (ch == '\n') stderr_putchar('\r');
#endif
        stderr_putchar(ch);
      }
#endif
      return (0);
  }
 
#ifdef RTE_Compiler_IO_File
#ifdef RTE_Compiler_IO_File_FS
  return (__sys_write(fh, buf, len));
#endif
#else
  return (-1);
#endif
}
#endif
 
 
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
#ifdef RETARGET_SYS
__attribute__((weak))
int _sys_read (FILEHANDLE fh, uint8_t *buf, uint32_t len, int mode) {
#ifdef RTE_Compiler_IO_STDIN
  int ch;
#elif (!defined(RTE_Compiler_IO_File))
  (void)buf;
  (void)len;
#endif
  (void)mode;
 
  switch (fh) {
    case FH_STDIN:
#ifdef RTE_Compiler_IO_STDIN
      ch = stdin_getchar();
      if (ch < 0) {
        return ((int)(len | 0x80000000U));
      }
      *buf++ = (uint8_t)ch;
#if (STDIN_ECHO != 0)
      stdout_putchar(ch);
#endif
      len--;
      return ((int)(len));
#else
      return ((int)(len | 0x80000000U));
#endif
    case FH_STDOUT:
      return (-1);
    case FH_STDERR:
      return (-1);
  }
 
#ifdef RTE_Compiler_IO_File
#ifdef RTE_Compiler_IO_File_FS
  return (__sys_read(fh, buf, len));
#endif
#else
  return (-1);
#endif
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
#if   defined(RTE_Compiler_IO_TTY)
#if   defined(RTE_Compiler_IO_TTY_User)
extern void ttywrch (int ch);
__attribute__((weak))
void _ttywrch (int ch) {
  ttywrch(ch);
}
#elif defined(RTE_Compiler_IO_TTY_ITM)
__attribute__((weak))
void _ttywrch (int ch) {
  ITM_SendChar(ch);
}
#elif defined(RTE_Compiler_IO_TTY_BKPT)
__attribute__((weak))
void _ttywrch (int ch) {
  (void)ch;
  __asm("BKPT 0");
}
#endif
#endif
 
 
/**
  Defined in rt_sys.h, this function determines if a file handle identifies
  a terminal.
 
  When a file is connected to a terminal device, this function is used to
  provide unbuffered behavior by default (in the absence of a call to
  set(v)buf) and to prohibit seeking.
 
  \param[in] fh File handle
 
  \return    The return value is one of the following values:
             - 0:     There is no interactive device.
             - 1:     There is an interactive device.
             - other: An error occurred.
*/
#ifdef RETARGET_SYS
__attribute__((weak))
int _sys_istty (FILEHANDLE fh) {
 
  switch (fh) {
    case FH_STDIN:
      return (1);
    case FH_STDOUT:
      return (1);
    case FH_STDERR:
      return (1);
  }
 
  return (0);
}
#endif
 
 
/**
  Defined in rt_sys.h, this function puts the file pointer at offset pos from
  the beginning of the file.
 
  This function sets the current read or write position to the new location pos
  relative to the start of the current file fh.
 
  \param[in] fh  File handle
  \param[in] pos File pointer offset
 
  \return    The result is:
             - non-negative if no error occurs
             - negative if an error occurs
*/
#ifdef RETARGET_SYS
__attribute__((weak))
int _sys_seek (FILEHANDLE fh, long pos) {
#if (!defined(RTE_Compiler_IO_File))
  (void)pos;
#endif
 
  switch (fh) {
    case FH_STDIN:
      return (-1);
    case FH_STDOUT:
      return (-1);
    case FH_STDERR:
      return (-1);
  }
 
#ifdef RTE_Compiler_IO_File
#ifdef RTE_Compiler_IO_File_FS
  return (__sys_seek(fh, (uint32_t)pos));
#endif
#else
  return (-1);
#endif
}
#endif
 
 
/**
  Defined in rt_sys.h, this function returns the current length of a file.
 
  This function is used by _sys_seek() to convert an offset relative to the
  end of a file into an offset relative to the beginning of the file.
  You do not have to define _sys_flen() if you do not intend to use fseek().
  If you retarget at system _sys_*() level, you must supply _sys_flen(),
  even if the underlying system directly supports seeking relative to the
  end of a file.
 
  \param[in] fh File handle
 
  \return    This function returns the current length of the file fh,
             or a negative error indicator.
*/
#ifdef RETARGET_SYS
__attribute__((weak))
long _sys_flen (FILEHANDLE fh) {
 
  switch (fh) {
    case FH_STDIN:
      return (0);
    case FH_STDOUT:
      return (0);
    case FH_STDERR:
      return (0);
  }
 
#ifdef RTE_Compiler_IO_File
#ifdef RTE_Compiler_IO_File_FS
  return (__sys_flen(fh));
#endif
#else
  return (0);
#endif
}
#endif
 
 
#endif  /* __MICROLIB */
