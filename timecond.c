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
#include "stdatomic.h"
#include <stdbool.h>

#include "timecore.h"

static const int iterations = 10000;
struct cond_pair {
    pthread_mutex_t mtx;
    pthread_cond_t  cond;
    _Atomic bool   die;
};
static void* thread(void* restrict ftx) {
  struct cond_pair *pair = (struct cond_pair*) ftx;
  struct timespec ts = { 0, 1 };
  while(!atomic_load(&pair->die)) {
    pthread_mutex_lock(&pair->mtx);
    pthread_cond_signal(&pair->cond);
    pthread_mutex_unlock(&pair->mtx);
    nanosleep(&ts,NULL);
  }
  return NULL;
}

int main(void) {
  struct timespec ts;
  struct cond_pair *pair = calloc(sizeof(*pair),1);
  pthread_t thd;
  pthread_cond_init(&pair->cond, NULL);
  pthread_mutex_init(&pair->mtx, NULL);
  atomic_store(&pair->die,false);
  pthread_mutex_lock(&pair->mtx);

  if (pthread_create(&thd, NULL, thread, pair)) {
    return -errno;
  }
  uint64_t delta = 0;
  for (int i = 0; i < iterations ; i++) {
    clock_start(&ts);
    pthread_cond_wait  (&pair->cond,&pair->mtx);
    delta += clock_end(&ts);
  }
  pthread_mutex_unlock(&pair->mtx);
  atomic_store(&pair->die,true);
  pthread_join(thd,NULL);
  pthread_mutex_destroy(&pair->mtx);
  pthread_cond_destroy (&pair->cond);
  free(pair);
  const int nswitches = (iterations << 1) ;
  printf("%i  thread context switches (cond) in %zu(%.1fns/ctxsw)\n",nswitches, delta, (delta / (float) nswitches));
  return 0;
}
