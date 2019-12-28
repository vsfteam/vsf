#ifndef __BUSYBOX_H__
#define __BUSYBOX_H__

int busybox_bind(char *path, vsf_linux_main_entry_t entry);
int busybox_install(void);

#endif
