_Pragma("once")

#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include <x86intrin.h>

static inline void tsc_start(uint64_t *tsc)
{
    if(tsc) *tsc = __rdtsc();
}
static inline uint64_t tsc_end(uint64_t *tsc)
{
    if(tsc) return __rdtsc() - *tsc;
    else return 0;
}
static inline double tsc_end_amortized(uint64_t *tsc, uint64_t reps)
{
    if(tsc && reps) {
        return tsc_end(tsc) * (1.0 / reps);
    } else {
        return 0.0;
    }
}
static inline void clock_start(struct timespec *ts)
{
    clock_gettime(CLOCK_REALTIME, ts);
}
static inline uint64_t clock_end(struct timespec *ts)
{
    if(ts) {
        struct timespec ts_end;
        clock_gettime(CLOCK_REALTIME,&ts_end);
        return 1000000000ll * (int64_t)(ts_end.tv_sec - ts->tv_sec) + (int64_t)(ts_end.tv_nsec - ts->tv_nsec);
    } else {
        return 0;
    }
}
static inline double clock_end_amortized(struct timespec *ts, uint64_t reps)
{
    if(ts && reps) {
        return clock_end(ts) * ( 1.0 / reps);
    } else {
        return 0;
    }
}
