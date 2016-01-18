#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <linux/futex.h>
#include "timecore.h"


static inline int get_iterations(int ws_pages) {
  int iterations = 1000;
  if(!ws_pages)
      ws_pages = 1;
  while (iterations * ws_pages * 4096 <( (1l)<<28)) {
    iterations += 1000;
  }
  return iterations;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <size of working set in 4K pages> <stride>\n", *argv);
    return 1;
  }
  const long ws_pages = strtol(argv[1], NULL, 10);
  const long ws_stride= strtol(argc > 2 ? argv[2] : "1", NULL, 10);
  if (ws_pages < 0) {
    fprintf(stderr, "Invalid usage: working set size must be positive\n");
    return 1;
  }
  const ssize_t iterations = get_iterations(ws_pages);
  struct timespec ts;
  uint64_t tsc;
  uint64_t delta_tsc;
  uint64_t memset_time = 0;
  uint64_t memset_tsc  = 0;
  if (ws_pages) {
    char* buf = malloc(ws_pages * 4096);
    clock_start(&ts);
    tsc_start(&tsc);
    for (int i = 0; i < iterations; i++) {
      for(ssize_t j = 0; j < ws_pages * 4096; j+=ws_stride)
        buf[j] = i;
    }
    memset_time = clock_end(&ts);
    memset_tsc  = tsc_end(&tsc);
    printf("%ld memset on %4zuK in %10zu(%.1fns/page)\n",
           iterations, ws_pages * 4, memset_time,
           (memset_time / ((float) ws_pages * iterations)));
    free(buf);
  }

  const int shm_id = shmget(IPC_PRIVATE, (ws_pages + 1) * 4096, IPC_CREAT | 0666);
  const pid_t other = fork();
  int* futex = shmat(shm_id, NULL, 0);
  char* ws = ((char *) futex) + 4096;
  *futex = 0xA;
  if (other == 0) {
    for (ssize_t i = 0; i < iterations; i++) {
      sched_yield();
      while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42))
        // retry
        sched_yield();
      *futex = 0xB;
      if (ws_pages)
        for(ssize_t j = 0; j < ws_pages * 4096; j+=ws_stride)
            ws[j] = i;
      while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42))
        // retry
        sched_yield();
    }
    return 0;
  }
  clock_start(&ts);
  tsc_start(&tsc);
  for (ssize_t i = 0; i < iterations; i++) {
    *futex = 0xA;
    if (ws_pages) {
        for(ssize_t j = 0; j < ws_pages * 4096; j+=ws_stride)
            ws[j] = i;
    }
    while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
      // retry
      sched_yield();
    }
    sched_yield();
    while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xB, NULL, NULL, 42)) {
      // retry
      sched_yield();
    }
  }
  const uint64_t delta = clock_end(&ts) - memset_time * 2;
  delta_tsc = tsc_end(&tsc) - memset_tsc * 2;
  const int64_t nswitches = iterations * 4;
  printf("%ld process context switches (wss:%4zuK) in %12zu(%.1fns/ctxsw, %.1f clocks/ctxsw)\n",
         nswitches, ws_pages * 4, delta, (delta / (float) nswitches), delta_tsc / (double) ( nswitches ));
  wait(futex);
  return 0;
}
