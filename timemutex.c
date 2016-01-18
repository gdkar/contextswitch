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

#include "timecore.h"

static const int iterations = 100000;
int main(void) {
  struct timespec ts;
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

  clock_start(&ts);
  for (int i = 0; i < iterations ; i++) {
    pthread_mutex_lock    (&mtx);
    pthread_mutex_unlock  (&mtx);
  }
  const uint64_t delta = clock_end(&ts);
  pthread_mutex_destroy(&mtx);
  const int nswitches = (iterations << 1) ;
  printf("%i  mutex lock/unlock pairs in %zu(%.1fns/lock-unlock)\n",nswitches, delta, (delta / (float) nswitches));
  return 0;
}
