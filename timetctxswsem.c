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
#include <semaphore.h>

#include "timecore.h"

static const int iterations = 500000;
struct sem_pair {
    sem_t parent;
    sem_t child;
};
static void* thread(void* restrict ftx) {
  struct sem_pair *pair = (struct sem_pair*) ftx;
  for (int i = 0; i < iterations; i++) {
    int ret;
    do {
        ret = sem_wait(&pair->child);
        if(ret < 0)
            ret = errno;
    }while(ret == EINTR || ret == EAGAIN);
    sem_post(&pair->parent);
  }
  return NULL;
}
int main(void) {
  struct timespec ts;
  struct sem_pair *pair = calloc(sizeof(*pair),1);
  sem_init(&pair->child,0,1);
  sem_init(&pair->parent,0,0);
  pthread_t thd;
  struct sched_param param;
  int SCHED = SCHED_FIFO;
  param.sched_priority = sched_get_priority_max(SCHED);
  if (sched_setscheduler(getpid(), SCHED, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setschedparam(&attr, &param);
  pthread_attr_setschedpolicy(&attr, SCHED);
  pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  if (pthread_create(&thd, &attr, thread, pair))
    return -errno;

  clock_start(&ts);
  uint64_t start_tsc = __rdtsc();
  for (int i = 0; i < iterations ; i++) {
    int ret;
    do {
        ret = sem_wait(&pair->parent);
        if(ret < 0)
            ret = errno;
    }while(ret == EINTR || ret == EAGAIN);
    sem_post(&pair->child);
  }
  const uint64_t delta = clock_end(&ts);
  const uint64_t delta_tsc = __rdtsc() - start_tsc;
  pthread_join(thd,NULL);
  sem_destroy(&pair->child);
  sem_destroy(&pair->parent);
  free(pair);
  const int nswitches = (iterations << 1) ;
  printf("%i  thread context switches (sem) in %zu(%.1fns/ctxsw, %1.f clocks/ctxsw)\n",nswitches, delta, (delta / (float) nswitches),delta_tsc / (float)nswitches);
  return 0;
}
