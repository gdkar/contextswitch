#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

#include "timecore.h"

int main(void) {
  struct sched_param param;
  param.sched_priority = sched_get_priority_min(SCHED_FIFO);
  if (sched_setscheduler(getpid(), SCHED_FIFO, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));

  const int iterations = 100000;
  struct timespec ts;
  uint64_t tsc;
  struct timespec ts_dummy = { 0, 1};
  clock_gettime(CLOCK_REALTIME,&ts_dummy);
  clock_start(&ts);
  tsc_start(&tsc);
  for (int i = 0; i < iterations; i++) {
    clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME, &ts_dummy,NULL);
  }
  const uint64_t delta = clock_end(&ts);
  const uint64_t delta_tsc = tsc_end(&tsc);
  printf("%i system calls in %zu (%.1fns/syscall, %.1f clocks/syscall))\n",
         iterations, delta, (delta / (double) iterations), delta_tsc / (double)iterations);
  return 0;
}
