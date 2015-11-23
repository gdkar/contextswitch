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

static __attribute__((noinline)) int function_call(int arg) {
    return arg;
}

int main(void) {
  const long iterations = 1000000000;
  struct timespec ts;
  const long long unsigned start_ns = time_ns(&ts);
  int toggle = start_ns;
  for (long i = 0; i < iterations; i++) {
    toggle = function_call(toggle+i);
  }
  const long long unsigned delta = time_ns(&ts) - start_ns;
  printf("out value is %d\n",toggle);
  printf("%ld function calls in %lluns (%.1fns/function call)\n",
         iterations, delta, (delta / (float) iterations));
  return 0;
}
