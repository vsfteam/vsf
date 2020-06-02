#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timeval {
    long    tv_sec;
    long    tv_usec;
};

struct timezone {
    int     tz_minuteswest;
    int     tz_dsttime;
};

int gettimeofday(struct timeval * tv, struct timezone * tz);

#ifdef __cplusplus
}
#endif

#endif
