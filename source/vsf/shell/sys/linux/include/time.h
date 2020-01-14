#ifndef __TIME_H__
#define __TIME_H__

typedef long long   time_t;
typedef long        clock_t;

#define CLOCKS_PER_SEC ((clock_t)1000000)

struct timespec {
    long    tv_sec;
    long    tv_nsec;
};

typedef enum {
    CLOCK_MONOTONIC,
    CLOCK_REALTIME,
} clockid_t;

clock_t clock(void);
int clock_gettime(clockid_t clk_id, struct timespec *tp);

void usleep(int usec);
unsigned sleep(unsigned sec);
int nanosleep(const struct timespec *requested_time, struct timespec *remaining);

#endif
