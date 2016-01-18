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
#include <semaphore.h>
#include "timecore.h"

static const int iterations = 100000;
int main(void) {
  struct timespec ts;
  sem_t sem;
  sem_init(&sem,0,0);
  clock_start(&ts);
  for (int i = 0; i < iterations ; i++) {
    sem_post(&sem);
    sem_wait(&sem);
  }
  const uint64_t delta = clock_end(&ts);
  sem_destroy(&sem);
  const int nswitches = (iterations << 1) ;
  printf("%i  semaphore wait/post pairs in %zu(%.1fns/post-wait)\n",nswitches, delta, (delta / (float) nswitches));
  return 0;
}
