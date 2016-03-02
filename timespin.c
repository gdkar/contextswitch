#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#include "timecore.h"

static int function_call(int arg)
{
    return arg;
}

int main(void) {
  const long iterations = 1000000000;
  struct timespec ts;
  uint64_t tsc;
  clock_start (&ts);
  tsc_start(&tsc);
  volatile int asignto = 0;
  for (long i = 0; i < iterations; i++) {
    asignto = function_call(i);
  }
  const long long unsigned delta = clock_end(&ts);
  const long long unsigned delta_tsc = tsc_end(&tsc);
  (void) asignto;
  printf("%ld spins in %lluns (%.1fns/spin, %.1f clocks/spin)\n",
         iterations, delta, (delta / (double) iterations),delta_tsc / (double)iterations);
  return 0;
}
