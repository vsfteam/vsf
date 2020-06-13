#ifndef __XBOOT_H__
#define __XBOOT_H__

#include <unistd.h>
#include <types.h>
#include <math.h>

// from stddef.h
#define clamp(v, a, b)          min(max(a, v), b)
#define idiv255(x)              ((((int)(x) + 1) * 257) >> 16)

// from math.h
#define M_E                     2.7182818284590452354   /* e */
#define M_LOG2E                 1.4426950408889634074   /* log_2 e */
#define M_LOG10E                0.43429448190325182765  /* log_10 e */
#define M_LN2                   0.69314718055994530942  /* log_e 2 */
#define M_LN10                  2.30258509299404568402  /* log_e 10 */
#define M_PI                    3.14159265358979323846  /* pi */
#define M_PI_2                  1.57079632679489661923  /* pi/2 */
#define M_PI_4                  0.78539816339744830962  /* pi/4 */
#define M_1_PI                  0.31830988618379067154  /* 1/pi */
#define M_2_PI                  0.63661977236758134308  /* 2/pi */
#define M_2_SQRTPI              1.12837916709551257390  /* 2/sqrt(pi) */
#define M_SQRT2                 1.41421356237309504880  /* sqrt(2) */
#define M_SQRT1_2               0.70710678118654752440  /* 1/sqrt(2) */

// string
size_t strlcpy(char *dest, const char *src, size_t size);
char *strsep(char **stringp, const char *delim);

// task
struct scheduler_t {
    int dummy;
};
struct task_t {
    implement(vsf_linux_thread_t)
};
typedef void (*task_func_t)(struct task_t * task, void * data);
struct task_t * task_create(struct scheduler_t * sched, const char * name, task_func_t func, void * data, size_t stksz, int nice);
void task_destroy(struct task_t * task);
void task_suspend(struct task_t * task);
void task_resume(struct task_t * task);
void task_yield(void);

// vfs
int vfs_open(const char *pathname, int flags, ...);
off_t vfs_lseek(int fd, off_t offset, int whence);
ssize_t vfs_read(int fd, void *buf, size_t count);
ssize_t vfs_write(int fd, void *buf, size_t count);
int vfs_close(int fd);

void vsf_xboot_bind(vk_disp_t *disp);

#include <spinlock.h>
#include <shash.h>
#include <xfs/xfs.h>

#include <xboot/ktime.h>
ktime_t ktime_get(void);

#include <xui/xui.h>
#include <xui/window.h>
#include <xui/popup.h>
#include <xui/panel.h>
#include <xui/button.h>
#include <xui/checkbox.h>
#include <xui/radio.h>
#include <xui/toggle.h>
#include <xui/badge.h>
#include <xui/progress.h>
#include <xui/split.h>
#include <xui/label.h>
#include <xui/text.h>

#endif      // __XBOOT_H__
