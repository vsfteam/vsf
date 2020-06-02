#ifndef __MOUNT_H__
#define __MOUNT_H__

#ifdef __cplusplus
extern "C" {
#endif

// TODO: implement standard mount after device mapping is implemented
//int mount(const char *source, const char *target,
//   const char *filesystemtype, unsigned long mountflags, const void *data);
int mount(const char *source, const char *target,
   const vk_fs_op_t *filesystem, unsigned long mountflags, const void *data);
int umount(const char *target);

#ifdef __cplusplus
}
#endif

#endif
