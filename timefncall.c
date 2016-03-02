#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>


#include "timecore.h"

static __attribute__((noinline)) int function_call(int arg) {
    return -arg;
}

int main(void) {
  const long iterations = 100000000;
  struct timespec ts;
  uint64_t tsc;
  clock_start(&ts);
  tsc_start(&tsc);
  volatile int toggle = ts.tv_nsec;
  for (long i = 0; i < iterations; i++) {
    toggle = function_call(i);
  }
  const long long unsigned delta = clock_end(&ts);
  const long long unsigned delta_tsc = tsc_end(&tsc);
  (void)toggle;
  printf("%ld function calls in %lluns (%.1fns/function call, %.1f clocks/function call)\n",
         iterations, delta, (delta / (double) iterations), delta_tsc/(double)iterations);
  return 0;
}
