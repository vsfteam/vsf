#ifndef __WAIT_H__
#define __WAIT_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

pid_t waitpid(pid_t pid, int *status, int options);

#ifdef __cplusplus
}
#endif

#endif
