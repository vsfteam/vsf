#ifndef __VSF_LINUX_UTSNAME_H__
#define __VSF_LINUX_UTSNAME_H__

#ifdef __cplusplus
extern "C" {
#endif

struct utsname {
    char sysname[4];     // "vsf\0"
    char release[1];
    char version[1];
    char machine[1];
};

extern int uname(struct utsname *name);

#ifdef __cplusplus
}
#endif

#endif
