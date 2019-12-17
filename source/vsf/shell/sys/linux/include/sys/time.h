#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#include <time.h>

#ifndef __WIN__
struct timeval {
    long    tv_sec;
    long    tv_usec;
};
#else
#include <Windows.h>
#endif

struct timezone {
    int     tz_minuteswest;
    int     tz_dsttime;
};

int gettimeofday(struct timeval * tv, struct timezone * tz);

#endif
