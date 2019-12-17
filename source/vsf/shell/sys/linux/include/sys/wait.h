#ifndef __WAIT_H__
#define __WAIT_H__

#include <sys/types.h>
pid_t waitpid(pid_t pid, int *status, int options);

#endif
