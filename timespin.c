#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

static inline long long unsigned time_ns(struct timespec* const ts) {
  if (clock_gettime(CLOCK_REALTIME, ts)) {
    exit(1);
  }
  return ((long long unsigned) ts->tv_sec) * 1000000000LLU
    + (long long unsigned) ts->tv_nsec;
}

int main(void) {
  const long iterations = 1000000000;
  struct timespec ts;
  const long long unsigned start_ns = time_ns(&ts);
  volatile long asignto = 0;
  for (long i = 0; i < iterations; i++) {
    asignto = i;
  }
  const long long unsigned delta = time_ns(&ts) - start_ns;
  printf("asignto = %ld\n",asignto);
  printf("%ld spins in %lluns (%.1fns/spin)\n",
         iterations, delta, (delta / (float) iterations));
  return 0;
}
