#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/futex.h>


#include "timecore.h"

static const int iterations = 100000;

static void* thread(void* restrict ftx) {
  int* futex = (int*) ftx;
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
  return NULL;
}

int main(void) {
  struct timespec ts;
  const int shm_id = shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | 0666);
  int* futex = shmat(shm_id, NULL, 0);
  pthread_t thd;
  struct sched_param param;
  param.sched_priority = 1;

  if (sched_setscheduler(getpid(), SCHED_RR, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));

  if (pthread_create(&thd, NULL, thread, futex)) {
    return 1;
  }
  *futex = 0xA;

  clock_start(&ts);
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

  const int nswitches = iterations << 2;
  printf("%i  thread context switches in %zu(%.1fns/ctxsw)\n",
         nswitches, delta, (delta / (float) nswitches));
  wait(futex);
  return 0;
}
