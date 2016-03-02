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

static const int iterations = 10000000;
int main(void) {
  struct sched_param param;
  param.sched_priority = 99;
  if (sched_setscheduler(getpid(), SCHED_FIFO, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));

  struct timespec ts;
  pthread_cond_t cond;
  pthread_mutex_t mtx;
  pthread_mutex_init(&mtx,NULL);
  pthread_cond_init(&cond,NULL);
  {
    clock_start(&ts);
    for (int i = 0; i < iterations ; i++) {
        pthread_cond_signal(&cond);
    }
    const uint64_t delta = clock_end(&ts);
    const int nswitches = (iterations ) ;
    printf("%i  condition signals in %zu(%.1fns/signal)\n",nswitches, delta, (delta / (float) nswitches));
  }
    {
    clock_start(&ts);
    for (int i = 0; i < iterations ; i++) {
        pthread_cond_broadcast(&cond);
    }
    const uint64_t delta = clock_end(&ts);
    const int nswitches = (iterations ) ;
    printf("%i  condition broadcast in %zu(%.1fns/broadcast)\n",nswitches, delta, (delta / (float) nswitches));
  }
  {
    clock_start(&ts);
    struct timespec now = { 0, 0};

    for (int i = 0; i < iterations/100 ; i++) {
        clock_gettime(CLOCK_REALTIME,&now);
        pthread_mutex_lock(&mtx);
        pthread_cond_timedwait(&cond,&mtx,&now);
        pthread_mutex_unlock(&mtx);
    }
    const uint64_t delta = clock_end(&ts);
    const int nswitches = (iterations/100 ) ;
    printf("%i  condition timedwaits in %zu(%.1fns/timedwait)\n",nswitches, delta, (delta / (float) nswitches));
  }
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mtx);
  return 0;
}
