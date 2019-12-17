#ifndef __TIME_H__
#define __TIME_H__

typedef long long   time_t;

struct timespec {
    long    tv_sec;
    long    tv_nsec;
};

typedef enum {
    CLOCK_MONOTONIC,
} clockid_t;

int clock_gettime(clockid_t clk_id, struct timespec *tp);

void usleep(int usec);
unsigned sleep(unsigned sec);
int nanosleep(const struct timespec *requested_time, struct timespec *remaining);

#endif
