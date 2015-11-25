#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <linux/futex.h>

#include "timecore.h"

int main(void) {
  const int iterations = 100000;
  struct timespec ts;
  uint64_t tsc;
  const int shm_id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);
  const pid_t other = fork();
  int* futex = shmat(shm_id, NULL, 0);
  *futex = 0xA;
  if (other == 0) {
    for (int i = 0; i < iterations; i++) {
      sched_yield();
      while (syscall(SYS_futex, futex, FUTEX_WAIT, 0xA, NULL, NULL, 42)) {
        // retry
        sched_yield();
      }
      *futex = 0xB;
      while (!syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 42)) {
        // retry
        sched_yield();
      }
    }
    return 0;
  }
  clock_start(&ts);
  tsc_start(&tsc);
  for (int i = 0; i < iterations; i++) {
    *futex = 0xA;
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
  const uint64_t delta = clock_end(&ts);
  const uint64_t delta_tsc = tsc_end(&tsc);

  const int nswitches = iterations << 2;
  printf("%i process context switches in %zu (%.1fns/context switch, %0.1f clocks/context switch)\n",
         nswitches, delta, (delta / (double) nswitches), delta_tsc/(double)nswitches);
  wait(futex);
  return 0;
}
