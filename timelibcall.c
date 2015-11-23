#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

extern int library_call(int arg);

static inline long long unsigned time_ns(struct timespec* const ts) {
  if (clock_gettime(CLOCK_REALTIME, ts)) {
    exit(1);
  }
  return ((long long unsigned) ts->tv_sec) * 1000000000LLU
    + (long long unsigned) ts->tv_nsec;
}


int main(void) {
  const long  iterations = 100000000;
  struct timespec ts;
  const long long unsigned start_ns = time_ns(&ts);
  int toggle = start_ns;
  for (long i = 0; i < iterations; i++) {
    toggle = library_call(toggle+i);
  }
  const long long unsigned delta = time_ns(&ts) - start_ns;
  (void) toggle;
  printf("%ld library calls in %lluns (%.1fns/library call)\n",
         iterations, delta, (delta / (float) iterations));
  return 0;
}
