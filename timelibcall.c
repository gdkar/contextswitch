#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <math.h>
#include <unistd.h>


#include "timecore.h"

extern int library_call(int arg);

int main(void) {
  const long  iterations = 10000000;
  struct timespec ts;
  uint64_t tsc;
  clock_start(&ts);
  tsc_start(&tsc);
  int toggle = 0;
  for (long i = 0; i < iterations; i++) {
    toggle = library_call(toggle);
  }
  const long long unsigned delta = clock_end(&ts);
  const long long unsigned delta_tsc = tsc_end(&tsc);
  (void) toggle;
  printf("%ld library calls in %lluns (%.1fns/library call, %.1f clocks/library call)\n",
         iterations, delta, (delta / (double) iterations),delta_tsc/(double)iterations);
  return 0;
}
