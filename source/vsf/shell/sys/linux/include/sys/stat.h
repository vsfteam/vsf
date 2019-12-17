#ifndef __STAT_H__
#define __STAT_H__

#include <sys/types.h>

struct stat {
    mode_t     st_mode;
};

int stat(const char *pathname, struct stat *buf);

#endif
