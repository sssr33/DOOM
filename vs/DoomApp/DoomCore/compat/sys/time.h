#pragma once
#include <time.h>
#include <stdio.h>

// WARNING DO NOT USE windows headers because it will conflict with boolean type which must be int
// TODO try refactor to remove duplicate definitions

#ifdef __cplusplus
extern "C" {
#endif

struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* and microseconds */
};

int gettimeofday(struct timeval* tv, struct timezone* tz);

#ifdef __cplusplus
}
#endif