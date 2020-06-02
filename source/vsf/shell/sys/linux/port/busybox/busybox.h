#ifndef __BUSYBOX_H__
#define __BUSYBOX_H__

#ifdef __cplusplus
extern "C" {
#endif

int busybox_bind(char *path, vsf_linux_main_entry_t entry);
int busybox_install(void);

#ifdef __cplusplus
}
#endif

#endif
