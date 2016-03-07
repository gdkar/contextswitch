#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "timecore.h"

int main(void) {
  const int iterations = 1000000;
  int fd = open("/dev/zero",O_RDWR);
  if(fd < 0) {
    fprintf(stderr,"Failed to open the file, %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  char buf[1];
  struct timespec ts;
  uint64_t tsc;
  clock_start(&ts);
  tsc_start(&tsc);
  for (int i = 0; i < iterations; i++) {
    syscall(SYS_lseek, (unsigned int)fd, 0, SEEK_SET);
    int ret = syscall(SYS_write,(unsigned int)fd,&buf,sizeof(buf));
    if(ret < 0) {
        fprintf(stderr,"Failed to write, %s\n",strerror(-errno));
        exit(ret);
    }

  }
  const uint64_t delta = clock_end(&ts);
  const uint64_t delta_tsc = tsc_end(&tsc);
  printf("%i system calls in %zu (%.1fns/syscall, %.1f clocks/syscall))\n",
         iterations, delta, (delta / (double) iterations), delta_tsc / (double)iterations);
  return 0;
}
