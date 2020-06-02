#ifndef __LINUX_TYPES_H__
#define __LINUX_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__WIN__) && defined(__CPU_X64__)
typedef int                 ssize_t;
typedef unsigned long long  size_t;
#else
typedef int                 ssize_t;
typedef unsigned int        size_t;
#endif

typedef int                 pid_t;
typedef int                 mode_t;
typedef unsigned int        uid_t;
typedef unsigned int        gid_t;
typedef long                off_t;

#ifdef __cplusplus
}
#endif

#endif
