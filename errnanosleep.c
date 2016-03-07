#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

#include "timecore.h"

int main(int argc, char **argv) {
  if(argc < 2)
    exit(EXIT_FAILURE);
  int64_t sleep_for = atoll(argv[1]);
  struct sched_param param;
  param.sched_priority = 1;
  if (sched_setscheduler(getpid(), SCHED_RR, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));
  const int64_t iterations = 1e9 / (sleep_for + 4000);
  double calib = 0;
  double calib_inv = 0;
  {
    uint32_t ignore;
    uint64_t before =__rdtscp(&ignore);
    usleep(1000);
    uint64_t after  =__rdtscp(&ignore);
    calib = (after - before) * 1e-9 / (1000 * 1e-6);
    calib_inv = 1.0 / calib;
  }
  struct timespec ts;
  uint64_t tsc;
  uint64_t err = 0;
  clock_start(&ts);
  tsc_start(&tsc);
  for (int64_t i = 0; i < iterations; i++) {
    struct timespec ts_dummy = { 0, sleep_for};
    uint32_t dummy;
    uint64_t before = __rdtscp(&dummy);
    nanosleep(&ts_dummy,NULL);
    uint64_t after  = __rdtscp(&dummy);
    err += ( after - before );
  }
  const uint64_t delta = clock_end(&ts);
  const uint64_t delta_tsc = tsc_end(&tsc);
  err *= calib_inv;
  err -= iterations * sleep_for;
  printf("%ld system calls in %zu (%.1fns/syscall, %.1f clocks/syscall))\n",
         iterations, delta, (delta / (double) iterations), delta_tsc / (double)iterations);
  printf("average error is %E ns ( %E clocks )\n",
    err * ( 1.0 / iterations), err * ( calib / iterations ));

  return 0;
}
