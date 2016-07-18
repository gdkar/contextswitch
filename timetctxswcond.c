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
struct cond_pair {
    pthread_mutex_t mtx;
    pthread_cond_t parent;
    pthread_cond_t child;
};
static void* thread(void* restrict ftx) {
  struct cond_pair *pair = (struct cond_pair*) ftx;
  pthread_mutex_lock(&pair->mtx);
  for (int i = 0; i < iterations; i++) {
    pthread_cond_signal(&pair->parent);
    pthread_cond_wait(&pair->child,&pair->mtx);
  }
  pthread_mutex_unlock(&pair->mtx);
  return NULL;
}

int main(void) {
  struct timespec ts;
  struct cond_pair *pair = calloc(sizeof(*pair),1);
  pthread_t thd;
  pthread_cond_init(&pair->parent,NULL);
  pthread_cond_init(&pair->child, NULL);
  pthread_mutex_init(&pair->mtx, NULL);
  pthread_mutex_lock(&pair->mtx);
  struct sched_param param;
  param.sched_priority = 1;
  if (sched_setscheduler(getpid(), SCHED_RR, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));

  if (pthread_create(&thd, NULL, thread, pair)) {
    return -errno;
  }
  clock_start(&ts);
  for (int i = 0; i < iterations ; i++) {
    pthread_cond_wait  (&pair->parent,&pair->mtx);
    pthread_cond_signal(&pair->child);
  }
  pthread_mutex_unlock(&pair->mtx);
  const uint64_t delta = clock_end(&ts);
  pthread_join(thd,NULL);
  pthread_mutex_destroy(&pair->mtx);
  pthread_cond_destroy (&pair->parent);
  pthread_cond_destroy (&pair->child);
  free(pair);
  const int nswitches = (iterations << 1) ;
  printf("%i  thread context switches (cond) in %zu (%.1fns/ctxsw)\n",nswitches, delta, (delta / (double) nswitches));
  return 0;
}
