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

static const int iterations = 10000000;
int main(void) {
  struct sched_param param;
  param.sched_priority = 1;
  if (sched_setscheduler(getpid(), SCHED_FIFO, &param))
    fprintf(stderr, "sched_setscheduler(): %s\n", strerror(errno));

  uint64_t ts;
  sem_t sem;
  sem_init(&sem,0,0);
  {
    tsc_start(&ts);
    for (int i = 0; i < iterations ; i++) {
        sem_post(&sem);
    }
    const uint64_t delta = tsc_end(&ts);
    const int nswitches = (iterations ) ;
    printf("%i  semaphore posts in %zu(%.1fns/post)\n",nswitches, delta, (delta / (float) nswitches));
  }
  {
    tsc_start(&ts);
    for (int i = 0; i < iterations ; i++) {
        sem_wait(&sem);
    }
    const uint64_t delta = tsc_end(&ts);
    const int nswitches = (iterations ) ;
    printf("%i  semaphore wait in %zu(%.1fns/wait)\n",nswitches, delta, (delta / (float) nswitches));
  }
  {
    tsc_start(&ts);
    for (int i = 0; i < iterations ; i++) {
        sem_trywait(&sem);
    }
    const uint64_t delta = tsc_end(&ts);
    const int nswitches = (iterations ) ;
    printf("%i  semaphore trywaits in %zu(%.1fns/trywait)\n",nswitches, delta, (delta / (float) nswitches));
  }
  {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC,&now);
    tsc_start(&ts);
    for (int i = 0; i < iterations/100 ; i++) {
        clock_gettime(CLOCK_REALTIME,&now);
/*        now.tv_nsec -= 50000;
        if(now.tv_nsec < 0) {
            now.tv_nsec += 1e9;
            now.tv_sec  --;
        }*/
        sem_timedwait(&sem,&now);
    }
    const uint64_t delta = tsc_end(&ts);
    const int nswitches = (iterations/100 ) ;
    printf("%i  semaphore timedwaits in %zu(%.1fns/timedwait)\n",nswitches, delta, (delta / (float) nswitches));
  }
  sem_destroy(&sem);
  return 0;
}
