#ifndef __VSF_LINUX_UTSNAME_H__
#define __VSF_LINUX_UTSNAME_H__

#ifdef __cplusplus
extern "C" {
#endif

struct utsname {
    char sysname[4];     // "vsf\0"
};

extern int uname(struct utsname *name);

#ifdef __cplusplus
}
#endif

#endif
