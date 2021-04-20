#ifndef __LINUX_TYPES_H__
#define __LINUX_TYPES_H__

// do not include this for compiler, because type.h is actually belong to compiler layer,
//  and compiler layer can not include vsf_utilities.h directly
//#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#if __IS_COMPILER_IAR__
//! begin of typedef name has already been declared (with same type)
#   pragma diag_suppress=pe301
#endif

#if defined(__CPU_X64__)
typedef int                 ssize_t;
typedef unsigned long int   size_t;
#else
typedef int                 ssize_t;
typedef unsigned int        size_t;
#endif

typedef int                 pid_t;
typedef int                 mode_t;
typedef unsigned int        uid_t;
typedef unsigned int        gid_t;
typedef long                off_t;
typedef long long           off64_t;

#if __IS_COMPILER_IAR__
//! end of typedef name has already been declared (with same type)
//#   pragma diag_suppress=pe301
#endif

#ifdef __cplusplus
}
#endif

#endif
