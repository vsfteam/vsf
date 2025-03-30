/****************************************************************************
 *
 * ftstdlib.h
 *
 *   ANSI-specific library and header configuration file (specification
 *   only).
 *
 * Copyright (C) 2002-2019 by
 * David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 *
 */


  /**************************************************************************
   *
   * This file is used to group all `#includes` to the ANSI~C library that
   * FreeType normally requires.  It also defines macros to rename the
   * standard functions within the FreeType source code.
   *
   * Load a file which defines `FTSTDLIB_H_` before this one to override it.
   *
   */


#ifndef FTSTDLIB_H_
#define FTSTDLIB_H_


#include <stddef.h>

#define ft_ptrdiff_t  ptrdiff_t


  /**************************************************************************
   *
   *                          integer limits
   *
   * `UINT_MAX` and `ULONG_MAX` are used to automatically compute the size of
   * `int` and `long` in bytes at compile-time.  So far, this works for all
   * platforms the library has been tested on.
   *
   * Note that on the extremely rare platforms that do not provide integer
   * types that are _exactly_ 16 and 32~bits wide (e.g., some old Crays where
   * `int` is 36~bits), we do not make any guarantee about the correct
   * behaviour of FreeType~2 with all fonts.
   *
   * In these cases, `ftconfig.h` will refuse to compile anyway with a
   * message like 'couldn't find 32-bit type' or something similar.
   *
   */


#include <limits.h>

#define FT_CHAR_BIT    CHAR_BIT
#define FT_USHORT_MAX  USHRT_MAX
#define FT_INT_MAX     INT_MAX
#define FT_INT_MIN     INT_MIN
#define FT_UINT_MAX    UINT_MAX
#define FT_LONG_MIN    LONG_MIN
#define FT_LONG_MAX    LONG_MAX
#define FT_ULONG_MAX   ULONG_MAX


  /**************************************************************************
   *
   *                character and string processing
   *
   */


#include <string.h>

#define ft_memchr   memchr
#define ft_memcmp   memcmp
#define ft_memcpy   memcpy
#define ft_memmove  memmove
#define ft_memset   memset
#define ft_strcat   strcat
#define ft_strcmp   strcmp
#define ft_strcpy   strcpy
#define ft_strlen   strlen
#define ft_strncmp  strncmp
#define ft_strncpy  strncpy
#define ft_strrchr  strrchr
#define ft_strstr   strstr


  /**************************************************************************
   *
   *                          file handling
   *
   */


#if VSF_USE_FS == ENABLED && VSF_FS_USE_MEMFS == ENABLED && VSF_FREETYPE_USE_STDIO != ENABLED

#include "component/fs/vsf_fs.h"
#include "utilities/vsf_utilities.h"
#include "utilities/ooc_class.h"

typedef struct FT_FILE {
    implement(vk_memfs_file_t)
} FT_FILE;

extern FT_FILE ft_root;

static VSF_CAL_ALWAYS_INLINE int ft_fclose(FT_FILE *f)
{
    return 0;
}

static VSF_CAL_ALWAYS_INLINE FT_FILE * ft_fopen(const char *filename, const char *mode)
{
    return (FT_FILE *)vk_memfs_open(&ft_root.use_as__vk_memfs_file_t, filename);
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

static VSF_CAL_ALWAYS_INLINE int ft_fseek(FT_FILE *f, long offset, int fromwhere)
{
    uint64_t new_pos;

    switch (fromwhere) {
    case SEEK_SET:
        new_pos = 0;
        break;
    case SEEK_CUR:
        new_pos = vk_memfs_tell(&f->use_as__vk_memfs_file_t);
        break;
    case SEEK_END:
        // DO NOT use f->size, which will fail when ft_fseek is called in cpp code
        new_pos = f->use_as__vk_memfs_file_t.use_as__vk_file_t.size;
        break;
    default:
        VSF_FS_ASSERT(false);
        return -1;
    }

    new_pos += offset;
    // DO NOT use f->size, which will fail when ft_fseek is called in cpp code
    if (new_pos > f->use_as__vk_memfs_file_t.use_as__vk_file_t.size) {
        return -1;
    }
    vk_memfs_setpos(&f->use_as__vk_memfs_file_t, new_pos);
    return 0;
}

static VSF_CAL_ALWAYS_INLINE long ft_ftell(FT_FILE *f)
{
    return vk_memfs_tell(&f->use_as__vk_memfs_file_t);
}

static VSF_CAL_ALWAYS_INLINE size_t ft_fread(const void *ptr, size_t size, size_t nmemb, FT_FILE *f)
{
    int_fast32_t rlen = vk_memfs_read(&f->use_as__vk_memfs_file_t, (uint8_t *)ptr, size * nmemb);
    return rlen;
}

static VSF_CAL_ALWAYS_INLINE size_t ft_fwrite(const void *ptr, size_t size, size_t nmemb, FT_FILE *f)
{
    int_fast32_t wlen = vk_memfs_write(&f->use_as__vk_memfs_file_t, (uint8_t *)ptr, size * nmemb);
    return wlen;
}

#define ft_sprintf  sprintf

#else

#include <stdio.h>

#define FT_FILE     FILE
#define ft_fclose   fclose
#define ft_fopen    fopen
#define ft_fread    fread
#define ft_fseek    fseek
#define ft_ftell    ftell
#define ft_sprintf  sprintf

#endif


  /**************************************************************************
   *
   *                            sorting
   *
   */


#include <stdlib.h>

#define ft_qsort  qsort


  /**************************************************************************
   *
   *                       memory allocation
   *
   */

#include "service/vsf_service.h"

#define ft_sfree     vsf_heap_free
#define ft_smalloc   vsf_heap_malloc
#define ft_srealloc  vsf_heap_realloc

static inline void * ft_scalloc(unsigned int num, unsigned int size) {
    unsigned int totalsize = num * size;
    void *result = ft_smalloc(totalsize);
    if (result != NULL) {
        ft_memset(result, 0, totalsize);
    }
    return result;
}

  /**************************************************************************
   *
   *                         miscellaneous
   *
   */


#define ft_strtol  strtol
#define ft_getenv(...)  (NULL)


  /**************************************************************************
   *
   *                        execution control
   *
   */


#include <setjmp.h>

#define ft_jmp_buf     jmp_buf  /* note: this cannot be a typedef since  */
                                /*       `jmp_buf` is defined as a macro */
                                /*       on certain platforms            */

#define ft_longjmp     longjmp
#define ft_setjmp( b ) setjmp( *(ft_jmp_buf*) &(b) ) /* same thing here */


  /* The following is only used for debugging purposes, i.e., if   */
  /* `FT_DEBUG_LEVEL_ERROR` or `FT_DEBUG_LEVEL_TRACE` are defined. */

#include <stdarg.h>


#endif /* FTSTDLIB_H_ */


/* END */
