#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "timecore.h"

int main(void) {
  const int iterations = 1000000;
  int fd = open("/dev/zero",O_RDONLY);
  char buf[1];
  struct timespec ts;
  uint64_t tsc;
  clock_start(&ts);
  tsc_start(&tsc);
  for (int i = 0; i < iterations; i++) {
    (void)syscall(SYS_read,(unsigned int)fd,&buf,sizeof(buf));
  }
  const uint64_t delta = clock_end(&ts);
  const uint64_t delta_tsc = tsc_end(&tsc);
  printf("%i system calls in %zu (%.1fns/syscall, %.1f clocks/syscall))\n",
         iterations, delta, (delta / (double) iterations), delta_tsc / (double)iterations);
  return 0;
}
